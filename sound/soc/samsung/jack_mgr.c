/*  sound/soc/samsung/jack_mgr.c
 */
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/workqueue.h>
#include <linux/irq.h>
#include <linux/delay.h>
#include <linux/types.h>
#include <linux/input.h>
#include <linux/platform_device.h>
#include <linux/errno.h>
#include <linux/err.h>
#include <linux/switch.h>
#include <linux/input.h>
#include <linux/timer.h>
#include <linux/wakelock.h>
#include <linux/slab.h>
#include <linux/gpio.h>
#include <linux/gpio_event.h>
#include <linux/sec_jack.h>
#include <mach/gpio.h>
#include <plat/gpio-cfg.h>
#include <plat/adc.h>

#define MAX_ZONE_LIMIT		10
#define SEND_KEY_CHECK_TIME_MS	30		/* 30ms */
//#define DET_CHECK_TIME_MS	200		/* 200ms */
#define WAKE_LOCK_TIME		(HZ * 5)	/* 5 sec */

#define JACK_MGR_DRV_NAME			"jack_mgr"
#define INPUT_HANDLE_NAME 			"jakc_mgr_buttons"
#define JACK_IST_NAME				"headset_detect"

#define DEBUG_MSG(f, a...)
//#define DEBUG_MSG(f, a...)  printk(f, ## a)

struct s3c_adc_client *adc_client;

struct jack_mgr_info {
	struct sec_jack_platform_data *pdata;
	struct delayed_work jack_detect_work;
	struct work_struct buttons_work;
	struct workqueue_struct *queue;
	struct input_dev *input_dev;
	struct wake_lock det_wake_lock;
	struct sec_jack_zone *zone;
	struct input_handler handler;
	struct input_handle handle;
	struct input_device_id ids;
	int det_irq;
	int dev_id;
	int pressed;
	int pressed_code;
	struct platform_device *send_key_dev;
	unsigned int cur_jack_type;
};

/* with some modifications like moving all the gpio structs inside
 * the platform data and getting the name for the switch and
 * gpio_event from the platform data, the driver could support more than
 * one headset jack, but currently user space is looking only for
 * one key file and switch for a headset so it'd be overkill and
 * untestable so we limit to one instantiation for now.
 */
static atomic_t instantiated = ATOMIC_INIT(0);

/* sysfs name HeadsetObserver.java looks for to track headset state
 */
struct switch_dev switch_jack_detection = {
	.name = "h2w",
};

static struct gpio_event_direct_entry jack_mgr_key_map[] = {
	{
		.code	= KEY_UNKNOWN,
	},
};

static struct gpio_event_input_info jack_mgr_key_info = {
	.info.func = gpio_event_input_func,
	.info.no_suspend = true,
	.type = EV_KEY,
	.debounce_time.tv64 = SEND_KEY_CHECK_TIME_MS * NSEC_PER_MSEC,
	.keymap = jack_mgr_key_map,
	.keymap_size = ARRAY_SIZE(jack_mgr_key_map)
};

static struct gpio_event_info *jack_mgr_input_info[] = {
	&jack_mgr_key_info.info,
};

static struct gpio_event_platform_data jack_mgr_input_data = {
	.name = JACK_MGR_DRV_NAME,
	.info = jack_mgr_input_info,
	.info_count = ARRAY_SIZE(jack_mgr_input_info),
};

/* gpio_input driver does not support to read adc value.
 * We use input filter to support 3-buttons of headset
 * without changing gpio_input driver.
 */
static bool  jack_mgr_buttons_filter(struct input_handle *handle,
				    unsigned int type, unsigned int code,
				    int value)
{
	struct jack_mgr_info *hi = handle->handler->private;

	if (type != EV_KEY || code != KEY_UNKNOWN)
		return false;

	hi->pressed = value;

	/* This is called in timer handler of gpio_input driver.
	 * We use workqueue to read adc value.
	 */
	queue_work(hi->queue, &hi->buttons_work);

	return true;
}

static int  jack_mgr_buttons_connect(struct input_handler *handler,
				    struct input_dev *dev,
				    const struct input_device_id *id)
{
	struct jack_mgr_info *hi;
	struct sec_jack_platform_data *pdata;
	struct sec_jack_buttons_zone *btn_zones;
	int err;
	int i;

	/* bind input_handler to input device related to only sec_jack */
	if (dev->name != jack_mgr_input_data.name)
		return -ENODEV;

	hi = handler->private;
	pdata = hi->pdata;
	btn_zones = pdata->buttons_zones;

	hi->input_dev = dev;
	hi->handle.dev = dev;
	hi->handle.handler = handler;
	hi->handle.open = 0;
	hi->handle.name = INPUT_HANDLE_NAME;

	err = input_register_handle(&hi->handle);
	if (err) {
		pr_err("%s: Failed to register sec_jack buttons handle, "
			"error %d\n", __func__, err);
		goto err_register_handle;
	}

	err = input_open_device(&hi->handle);
	if (err) {
		pr_err("%s: Failed to open input device, error %d\n",
			__func__, err);
		goto err_open_device;
	}

	for (i = 0; i < pdata->num_buttons_zones; i++)
		input_set_capability(dev, EV_KEY, btn_zones[i].code);

	return 0;

 err_open_device:
	input_unregister_handle(&hi->handle);
 err_register_handle:

	return err;
}

static void  jack_mgr_buttons_disconnect(struct input_handle *handle)
{
	input_close_device(handle);
	input_unregister_handle(handle);
}

/// WILLOW 4pole earjack remote key detection
static int willow_curr_headset_type = SEC_JACK_NO_DEVICE;
int willow_jack_get_type(void)
{
	return willow_curr_headset_type;
}

#define WILLOW_REMOTE_KEY_ADC_ID		2
int jack_mgr_get_adc_data(void)
{
	int adc =  s3c_adc_read(adc_client, WILLOW_REMOTE_KEY_ADC_ID);
	return adc;
}

static void jack_mgr_set_type(struct jack_mgr_info *hi, int jack_type)
{
	//struct sec_jack_platform_data *pdata = hi->pdata;
	
	DEBUG_MSG("[%s] jack_type : %d\n",__func__, jack_type);


/// WILLOW 4pole earjack remote key detection
	willow_curr_headset_type = jack_type;

	/* this can happen during slow inserts where we think we identified
	 * the type but then we get another interrupt and do it again
	 */
	if (jack_type == hi->cur_jack_type)
		return;

	if (jack_type == SEC_HEADSET_4POLE) {
		/* for a 4 pole headset, enable detection of send/end key */
		if (hi->send_key_dev == NULL)
			/* enable to get events again */
			hi->send_key_dev = platform_device_register_data(NULL,
					GPIO_EVENT_DEV_NAME,
					hi->dev_id,
					&jack_mgr_input_data,
					sizeof(jack_mgr_input_data));
	} else {
		/* for all other jacks, disable send/end key detection */
		if (hi->send_key_dev != NULL) {
			/* disable to prevent false events on next insert */
			platform_device_unregister(hi->send_key_dev);
			hi->send_key_dev = NULL;
		}
		/* micbias is left enabled for 4pole and disabled otherwise */
//    printk("[KYJUNG][File: %s][Fun: %s][Line: %d] set_micbias_state\n", __FILE__, __func__, __LINE__);
		//pdata->set_micbias_state(false, jack_type);
	}

	hi->cur_jack_type = jack_type;
	pr_info("%s : jack_type = %d\n", __func__, jack_type);

	/* prevent suspend to allow user space to respond to switch */
	wake_lock_timeout(&hi->det_wake_lock, WAKE_LOCK_TIME);

	switch_set_state(&switch_jack_detection, jack_type);
}

static void handle_jack_not_inserted(struct jack_mgr_info *hi)
{
	jack_mgr_set_type(hi, SEC_JACK_NO_DEVICE);
//	printk("[KYJUNG][File: %s][Fun: %s][Line: %d] set_micbias_state\n", __FILE__, __func__, __LINE__);
//	hi->pdata->set_micbias_state(false, hi->cur_jack_type);
}

static void determine_jack_type(struct jack_mgr_info *hi)
{
	struct sec_jack_zone *zones = hi->pdata->zones;
	int size = hi->pdata->num_zones;
	int count[MAX_ZONE_LIMIT] = {0};
	int adc;
	int i;
	unsigned npolarity = !hi->pdata->det_active_high;

	while (gpio_get_value(hi->pdata->det_gpio) ^ npolarity) {
		adc = hi->pdata->get_adc_value();
		pr_debug("%s: adc = %d\n", __func__, adc);

		/* determine the type of headset based on the
		 * adc value.  An adc value can fall in various
		 * ranges or zones.  Within some ranges, the type
		 * can be returned immediately.  Within others, the
		 * value is considered unstable and we need to sample
		 * a few more types (up to the limit determined by
		 * the range) before we return the type for that range.
		 */
		for (i = 0; i < size; i++) {
			if (adc <= zones[i].adc_high) {
				if (++count[i] > zones[i].check_count) {
					
					jack_mgr_set_type(hi,
							  zones[i].jack_type);
					return;
				}
				msleep(zones[i].delay_ms);
				break;
			}
		}
	}
	/* jack removed before detection complete */
	pr_debug("%s : jack removed before detection complete\n", __func__);
	handle_jack_not_inserted(hi);
}

/* thread run whenever the headset detect state changes (either insertion
 * or removal).
 */
static irqreturn_t jack_mgr_detect_irq_thread(int irq, void *dev_id)
{
	struct jack_mgr_info *hi = dev_id;
	//struct sec_jack_platform_data *pdata = hi->pdata;
	unsigned npolarity = !hi->pdata->det_active_high;

	DEBUG_MSG("[%s] #\n",__func__);
	/* set mic bias to enable adc */
//	printk("[KYJUNG][File: %s][Fun: %s][Line: %d] set_micbias_state\n", __FILE__, __func__, __LINE__);
//	pdata->set_micbias_state(true, hi->cur_jack_type);

	/* debounce headset jack.  don't try to determine the type of
	 * headset until the detect state is true for a while.
	 */
	if (!(gpio_get_value(hi->pdata->det_gpio) ^ npolarity)) {
		DEBUG_MSG("[%s] jack not detected!\n",__func__);
		/* jack not detected. */
		handle_jack_not_inserted(hi);
		return IRQ_HANDLED;
	}

	DEBUG_MSG("[%s] jack detected#\n",__func__);	
	
	/* jack presence was detected the whole time, figure out which type */
	determine_jack_type(hi);
	return IRQ_HANDLED;
}

/* thread run whenever the button of headset is pressed or released */
void  jack_mgr_buttons_work(struct work_struct *work)
{
	struct jack_mgr_info *hi =
		container_of(work, struct jack_mgr_info, buttons_work);
	struct sec_jack_platform_data *pdata = hi->pdata;
	struct sec_jack_buttons_zone *btn_zones = pdata->buttons_zones;
	int adc;
	int i;

	/* when button is released */
	if (hi->pressed == 0) {
		input_report_key(hi->input_dev, hi->pressed_code, 0);
		input_sync(hi->input_dev);
		pr_debug("%s: keycode=%d, is released\n", __func__,
			hi->pressed_code);
		return;
	}

	/* when button is pressed */
	adc = pdata->get_adc_value();

	for (i = 0; i < pdata->num_buttons_zones; i++)
		if (adc >= btn_zones[i].adc_low &&
		    adc <= btn_zones[i].adc_high) {
			hi->pressed_code = btn_zones[i].code;
			input_report_key(hi->input_dev, btn_zones[i].code, 1);
			input_sync(hi->input_dev);
			pr_debug("%s: keycode=%d, is pressed\n", __func__,
				btn_zones[i].code);
			return;
		}

	pr_warn("%s: key is skipped. ADC value is %d\n", __func__, adc);
}

static int jack_mgr_probe(struct platform_device *pdev)
{
	struct jack_mgr_info *hi;
	struct sec_jack_platform_data *pdata = pdev->dev.platform_data;
	int ret;
	unsigned int npolarity;

	pr_info("%s : Registering jack driver\n", __func__);
	if (!pdata) {
		pr_err("%s : pdata is NULL.\n", __func__);
		return -ENODEV;
	}

	if (!pdata->get_adc_value || !pdata->zones ||
	    !pdata->set_micbias_state || pdata->num_zones > MAX_ZONE_LIMIT) {
		pr_err("%s : need to check pdata\n", __func__);
		return -ENODEV;
	}

	if (atomic_xchg(&instantiated, 1)) {
		pr_err("%s : already instantiated, can only have one\n",
			__func__);
		return -ENODEV;
	}

	jack_mgr_key_map[0].gpio = pdata->send_end_gpio;

	/* If no other keys in pdata, make all keys default to KEY_MEDIA */
	if (pdata->num_buttons_zones == 0){
		jack_mgr_key_map[0].code = KEY_MEDIA;
	}

	hi = kzalloc(sizeof(struct jack_mgr_info), GFP_KERNEL);
	if (hi == NULL) {
		pr_err("%s : Failed to allocate memory.\n", __func__);
		ret = -ENOMEM;
		goto err_kzalloc;
	}

	adc_client = s3c_adc_register(pdev, NULL, NULL, 0);
	if (IS_ERR(adc_client)) {
		pr_err("%s  : failed to register adc client\n",__func__);
		goto err_kzalloc;
	}

	hi->pdata = pdata;

	/* make the id of our gpi_event device the same as our platform device,
	 * which makes it the responsiblity of the board file to make sure
	 * it is unique relative to other gpio_event devices
	 */
	hi->dev_id = pdev->id;
	ret = gpio_request(pdata->det_gpio, "JACK_DET");
	if (ret) {
		pr_err("%s : gpio_request failed for %d\n",
		       __func__, pdata->det_gpio);
		goto err_gpio_request;
	}

	/// Added gpio configuration -->
	/* Configure earjack detect gpio */
	gpio_direction_input(pdata->det_gpio);
	s3c_gpio_cfgpin(pdata->det_gpio, S3C_GPIO_SFN(0xFF));
	s3c_gpio_setpull(pdata->det_gpio, S3C_GPIO_PULL_NONE);

	ret = switch_dev_register(&switch_jack_detection);
	if (ret < 0) {
		pr_err("%s : Failed to register switch device\n", __func__);
		goto err_switch_dev_register;
	}

	wake_lock_init(&hi->det_wake_lock, WAKE_LOCK_SUSPEND, "jack_mgr_det");

	INIT_WORK(&hi->buttons_work,  jack_mgr_buttons_work);
	hi->queue = create_singlethread_workqueue("jack_mgr_wq");
	if (hi->queue == NULL) {
		ret = -ENOMEM;
		pr_err("%s: Failed to create workqueue\n", __func__);
		goto err_create_wq_failed;
	}

	hi->det_irq = gpio_to_irq(pdata->det_gpio);

	set_bit(EV_KEY, hi->ids.evbit);
	hi->ids.flags = INPUT_DEVICE_ID_MATCH_EVBIT;
	hi->handler.filter =  jack_mgr_buttons_filter;
	hi->handler.connect =  jack_mgr_buttons_connect;
	hi->handler.disconnect =  jack_mgr_buttons_disconnect;
	hi->handler.name = INPUT_HANDLE_NAME;
	hi->handler.id_table = &hi->ids;
	hi->handler.private = hi;

	ret = input_register_handler(&hi->handler);
	if (ret) {
		pr_err("%s : Failed to register_handler\n", __func__);
		goto err_register_input_handler;
	}

	ret = request_threaded_irq(hi->det_irq, NULL,
				   jack_mgr_detect_irq_thread,
				   IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING |
				   IRQF_ONESHOT, JACK_IST_NAME, hi);
	if (ret) {
		pr_err("%s : Failed to request_irq. ret = %d\n", __func__,ret);
		goto err_request_detect_irq;
	}

	/* to handle insert/removal when we're sleeping in a call */
	ret = enable_irq_wake(hi->det_irq);
	if (ret) {
		pr_err("%s : Failed to enable_irq_wake.\n", __func__);
		goto err_enable_irq_wake;
	}

	dev_set_drvdata(&pdev->dev, hi);

	return 0;

err_enable_irq_wake:
	free_irq(hi->det_irq, hi);
err_request_detect_irq:
	input_unregister_handler(&hi->handler);
err_register_input_handler:
	destroy_workqueue(hi->queue);
err_create_wq_failed:
	wake_lock_destroy(&hi->det_wake_lock);
	switch_dev_unregister(&switch_jack_detection);
err_switch_dev_register:
	gpio_free(pdata->det_gpio);
err_gpio_request:
	kfree(hi);
err_kzalloc:
	atomic_set(&instantiated, 0);

	return ret;
}

static int jack_mgr_remove(struct platform_device *pdev)
{

	struct jack_mgr_info *hi = dev_get_drvdata(&pdev->dev);

	pr_info("%s :\n", __func__);
	disable_irq_wake(hi->det_irq);
	free_irq(hi->det_irq, hi);
	destroy_workqueue(hi->queue);
	if (hi->send_key_dev) {
		platform_device_unregister(hi->send_key_dev);
		hi->send_key_dev = NULL;
	}
	input_unregister_handler(&hi->handler);
	wake_lock_destroy(&hi->det_wake_lock);
	switch_dev_unregister(&switch_jack_detection);
	gpio_free(hi->pdata->det_gpio);
	kfree(hi);
	atomic_set(&instantiated, 0);
	if (!IS_ERR(adc_client))
		s3c_adc_release(adc_client);

	return 0;
}

static struct platform_driver jack_mgr_driver = {
	.probe = jack_mgr_probe,
	.remove = jack_mgr_remove,
	.driver = {
		.name = JACK_MGR_DRV_NAME,
		.owner = THIS_MODULE,
	},
};
static int __init jack_mgr_init(void)
{
	return platform_driver_register(&jack_mgr_driver);
}

static void __exit jack_mgr_exit(void)
{
	platform_driver_unregister(&jack_mgr_driver);
}

module_init(jack_mgr_init);
module_exit(jack_mgr_exit);

MODULE_AUTHOR("ms17.kim@samsung.com");
MODULE_DESCRIPTION("Samsung Electronics Corp Ear-Jack detection driver");
MODULE_LICENSE("GPL");
