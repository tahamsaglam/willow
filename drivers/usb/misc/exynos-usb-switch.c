/*
 * exynos-usb-switch.c - USB switch driver for Exynos
 *
 * Copyright (c) 2010-2011 Samsung Electronics Co., Ltd.
 * Yulgon Kim <yulgon.kim@samsung.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/

#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/err.h>
#include <linux/slab.h>
#include <linux/platform_device.h>
#include <linux/pm_runtime.h>

#include <plat/devs.h>
#include <plat/ehci.h>
#include <plat/usbgadget.h>

#include <mach/regs-clock.h>

#include "../gadget/s3c_udc.h"
#include "exynos-usb-switch.h"

#define DRIVER_DESC "Exynos USB Switch Driver"
#define SWITCH_WAIT_TIME	500
#define WAIT_TIMES		10

static const char switch_name[] = "exynos_usb_switch";

static void exynos_host_port_power_off(void)
{
	s5p_ehci_port_power_off(&s5p_device_ehci);
	s5p_ohci_port_power_off(&s5p_device_ohci);
}

static void __maybe_unused exynos_host_port_power_on(void)
{
	s5p_ehci_port_power_on(&s5p_device_ehci);
	s5p_ohci_port_power_on(&s5p_device_ohci);
}

static int is_host_detect(struct exynos_usb_switch *usb_switch)
{
	msleep(50);
	printk(KERN_INFO ">>>>>>> is_host_detect() = %d (return %d)\n",
			gpio_get_value(usb_switch->gpio_host_detect),
			!gpio_get_value(usb_switch->gpio_host_detect));
	return !gpio_get_value(usb_switch->gpio_host_detect);
}

static void set_host_vbus(struct exynos_usb_switch *usb_switch, int value)
{
	gpio_set_value(usb_switch->gpio_host_vbus, value);
}

static int exynos_change_usb_mode(struct exynos_usb_switch *usb_switch,
				enum usb_cable_status mode)
{
	struct s3c_udc *udc = the_controller;
	enum usb_cable_status cur_mode;
	int ret = 0;

	switch (mode) {
		case USB_HOST_DETACHED:
			if (is_host_detect(usb_switch) == 1) {
				cur_mode = USB_HOST_ATTACHED;
				break;
			}

			if (usb_switch->gpio_host_vbus)
				set_host_vbus(usb_switch, 0);

			pm_runtime_put(&s5p_device_ohci.dev);
			pm_runtime_put(&s5p_device_ehci.dev);

			udc->gadget.ops->vbus_session(&udc->gadget, 1);

			atomic_set(&usb_switch->connect, USB_HOST_DETACHED);
			cur_mode = USB_HOST_DETACHED;
			break;
		case USB_HOST_ATTACHED:
			if (is_host_detect(usb_switch) == 0) {
				cur_mode = USB_HOST_DETACHED;
				break;
			}

			udc->gadget.ops->vbus_session(&udc->gadget, 0);

			pm_runtime_get_sync(&s5p_device_ohci.dev);
			pm_runtime_get_sync(&s5p_device_ehci.dev);

			if (usb_switch->gpio_host_vbus)
				set_host_vbus(usb_switch, 1);

			atomic_set(&usb_switch->connect, USB_HOST_ATTACHED);
			cur_mode = USB_HOST_ATTACHED;
			break;
		default:
			printk(KERN_ERR "Does not changed\n");
	}
	printk(KERN_INFO "mode = %d, cur_mode = %d, connect = %d\n", mode, cur_mode, atomic_read(&usb_switch->connect));

	return ret;
}

static irqreturn_t exynos_host_detect_thread(int irq, void *data)
{
	struct exynos_usb_switch *usb_switch = data;

	mutex_lock(&usb_switch->mutex);

	if (is_host_detect(usb_switch))
		exynos_change_usb_mode(usb_switch, USB_HOST_ATTACHED);
	else
		exynos_change_usb_mode(usb_switch, USB_HOST_DETACHED);

	mutex_unlock(&usb_switch->mutex);

	return IRQ_HANDLED;
}

static int exynos_usb_status_init(struct exynos_usb_switch *usb_switch)
{
	if (atomic_read(&usb_switch->connect) == USB_HOST_ATTACHED)
		printk(KERN_ERR "Already setting\n");
	else if (is_host_detect(usb_switch))
		exynos_change_usb_mode(usb_switch, USB_HOST_ATTACHED);
	else
		exynos_change_usb_mode(usb_switch, USB_HOST_DETACHED);

	return 0;
}

#ifdef CONFIG_PM
static int exynos_usbswitch_suspend(struct device *dev)
{
	struct platform_device *pdev = to_platform_device(dev);
	struct exynos_usb_switch *usb_switch = platform_get_drvdata(pdev);

	printk(KERN_INFO "%s\n", __func__);

	disable_irq(usb_switch->gpio_host_detect);

	return 0;
}

static int exynos_usbswitch_resume(struct device *dev)
{
	struct platform_device *pdev = to_platform_device(dev);
	struct exynos_usb_switch *usb_switch = platform_get_drvdata(pdev);

	printk(KERN_INFO "%s\n", __func__);

	enable_irq(usb_switch->gpio_host_detect);

	return 0;
}
#else
#define exynos_usbswitch_suspend	NULL
#define exynos_usbswitch_resume		NULL
#endif

static int __devinit exynos_usbswitch_probe(struct platform_device *pdev)
{
	struct s5p_usbswitch_platdata *pdata = dev_get_platdata(&pdev->dev);
	struct device *dev = &pdev->dev;
	struct exynos_usb_switch *usb_switch;
	int irq;
	int ret;

	usb_switch = kzalloc(sizeof(struct exynos_usb_switch), GFP_KERNEL);
	if (!usb_switch) {
		ret = -ENOMEM;
		goto fail;
	}

	mutex_init(&usb_switch->mutex);
	usb_switch->gpio_host_detect = pdata->gpio_host_detect;
	usb_switch->gpio_host_vbus = pdata->gpio_host_vbus;

	/* USB Host detect IRQ */
	irq = platform_get_irq(pdev, 0);
	if (!irq) {
		dev_err(&pdev->dev, "Failed to get IRQ\n");
		ret = -ENODEV;
		goto fail;
	}

	ret = request_threaded_irq(irq, NULL, exynos_host_detect_thread,
			IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING,
			"HOST_DETECT", usb_switch);
	if (ret) {
		dev_err(dev, "Failed to allocate an HOST interrupt(%d)\n", irq);
		goto fail;
	}
	usb_switch->host_detect_irq = irq;
	atomic_set(&usb_switch->connect, USB_HOST_DETACHED);

	platform_set_drvdata(pdev, usb_switch);

	return ret;
fail:
	mutex_destroy(&usb_switch->mutex);
	return ret;
}

static int __devexit exynos_usbswitch_remove(struct platform_device *pdev)
{
	struct exynos_usb_switch *usb_switch = platform_get_drvdata(pdev);
	struct device *dev = &pdev->dev;

	free_irq(usb_switch->host_detect_irq, dev);
	platform_set_drvdata(pdev, 0);
	mutex_destroy(&usb_switch->mutex);
	kfree(usb_switch);

	return 0;
}

static const struct dev_pm_ops exynos_usbswitch_pm_ops = {
	.suspend                = exynos_usbswitch_suspend,
	.resume                 = exynos_usbswitch_resume,
};

static struct platform_driver exynos_usbswitch_driver = {
	.probe		= exynos_usbswitch_probe,
	.remove		= __devexit_p(exynos_usbswitch_remove),
	.driver		= {
		.name	= "exynos-usb-switch",
		.owner	= THIS_MODULE,
		.pm	= &exynos_usbswitch_pm_ops,
	},
};

static int __init exynos_usbswitch_init(void)
{
	int ret;

	ret = platform_device_register(&s5p_device_usbswitch);
	if (ret < 0)
		return ret;

	ret = platform_driver_register(&exynos_usbswitch_driver);
	if (!ret)
		printk(KERN_INFO "%s: " DRIVER_DESC "\n", switch_name);

	return ret;
}
late_initcall(exynos_usbswitch_init);

static void __exit exynos_usbswitch_exit(void)
{
	platform_driver_unregister(&exynos_usbswitch_driver);
}
module_exit(exynos_usbswitch_exit);

MODULE_DESCRIPTION("Exynos USB switch driver");
MODULE_AUTHOR("<yulgon.kim@samsung.com>");
MODULE_LICENSE("GPL");
