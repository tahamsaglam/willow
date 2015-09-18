/*
*  UBLOX G6010 GPS ON/OFF Driver
*/
#ifdef CONFIG_GPS_G6010

#include <linux/input.h>
#include <linux/device.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/jiffies.h>
#include <linux/i2c.h>
#include <linux/delay.h>

#include <linux/cdev.h>
#include <linux/platform_device.h>

#include <asm/gpio.h>
#include <plat/gpio-cfg.h>
#include <linux/regulator/machine.h>
//#include <linux/regulator/consumer.h>

#define GPS_CTL_DEV_NAME "g6010_ctl"
#define GPS_CTL_CLASS_NAME GPS_CTL_DEV_NAME
#define GPS_CTL_DEV_MAJOR  124

#define dbg(format, arg...)
#define check_func_in()
#define check_func_out()

static struct class *gpsctl_dev_class;
static struct cdev gpsctl_cdev;
static dev_t gpsctl_dev_t;

struct regulator *gps_io;
struct regulator *gps_lna;
int gisRegualtorOn = 0;

int gpsctl_dev_open(struct inode *inode, struct file *filp)
{
	check_func_in();
	check_func_out();
	return 0;
}

long gpsctl_dev_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	int err;
	check_func_in();
	//dbg("[%s] ============cmd : %d, arg : %d", __func__,cmd,arg);
	
	switch (cmd)
	{
		case 0:
			if(gisRegualtorOn){
				gisRegualtorOn = 0;
				regulator_disable(gps_io);
				msleep(10);
				regulator_disable(gps_lna);
			}
			break;
		case 1:
			if(!gisRegualtorOn){
				gisRegualtorOn = 1;
				regulator_enable(gps_io);
				msleep(10);
				regulator_enable(gps_lna);
			}
			break;
		default:break;
	}

	check_func_out();
	return 0;
}

int gpsctl_dev_release(struct inode *inode, struct file *filp)
{
	check_func_in();
	check_func_out();
	return 0;
}

struct file_operations ioctl_fops =
{
	.owner = THIS_MODULE,
	.unlocked_ioctl = gpsctl_dev_ioctl,
	.open = gpsctl_dev_open,
	.release = gpsctl_dev_release,
};

static int unregister_gps_device(void)
{
	check_func_in();
	unregister_chrdev(GPS_CTL_DEV_MAJOR,GPS_CTL_DEV_NAME);
	class_destroy(gpsctl_dev_class);
	unregister_chrdev_region(gpsctl_dev_t, 1);
	check_func_out();
	return 0;
}
static int register_gps_device(void)
{
	struct device *gpsctl_dev;
	int retval;

	check_func_in();

	retval = alloc_chrdev_region(&gpsctl_dev_t, 0/*requested minor*/, 1/*count*/, GPS_CTL_DEV_NAME);
	if (retval < 0) {
		dbg("Failed to alloc_chrdev_region");
	}

	dbg("=============major = %d, minor = %d",MAJOR(gpsctl_dev_t),MINOR(gpsctl_dev_t));

	cdev_init(&gpsctl_cdev, &ioctl_fops);

	retval = cdev_add(&gpsctl_cdev, gpsctl_dev_t, 1);
	if (retval < 0) {
		dbg("Failed to cdev_add");
		//    device_destroy(gpsctl_dev_class, gpsctl_dev_t);
	}

	gpsctl_dev_class = class_create(THIS_MODULE, GPS_CTL_CLASS_NAME);
	if(IS_ERR(gpsctl_dev_class))
	{
		unregister_gps_device();
		return -EFAULT;
	}

	gpsctl_dev = device_create(gpsctl_dev_class, NULL, gpsctl_dev_t, "%s", GPS_CTL_DEV_NAME);

	if(IS_ERR(gpsctl_dev))
	{
		unregister_gps_device();
		return -EFAULT;
	}

	check_func_out();

	return 0;
}

static int __init gps_init(void)
{
	int resultv;
	check_func_in();
	resultv=register_gps_device();
	if(resultv<0)
	{
		dbg("Failed to register gps device");
		return resultv;
	}
	dbg("[gps] resultv = %d",resultv);
	check_func_out();

	gps_io = regulator_get(NULL, "vdd_gps_1v8");
	gps_lna = regulator_get(NULL, "vdd_gps_2v8");

	return resultv;
}

static void __exit gps_exit(void)
{
	regulator_put(gps_io);
	regulator_put(gps_lna);

	check_func_in();
	unregister_gps_device();
	check_func_out();
}

module_init(gps_init);
module_exit(gps_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("VEGITO");
MODULE_DESCRIPTION("ublox g6010 driver");
#endif
