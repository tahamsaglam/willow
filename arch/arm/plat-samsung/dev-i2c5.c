/* linux/arch/arm/plat-samsung/dev-i2c3.c
 *
 * Copyright (c) 2010 Samsung Electronics Co., Ltd.
 *		http://www.samsung.com/
 *
 * S5P series device definition for i2c device 3
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/gfp.h>
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/platform_device.h>

#include <mach/irqs.h>
#include <mach/map.h>

#include <plat/regs-iic.h>
#include <plat/iic.h>
#include <plat/devs.h>
#include <plat/cpu.h>

#include <plat/devs.h>
#include <plat/cpu.h>
#include <linux/clk.h>

#include <asm/io.h>
#include <linux/err.h>

static struct resource s3c_i2c_resource[] = {
	[0] = {
		.start	= S3C_PA_IIC5,
		.end	= S3C_PA_IIC5 + SZ_4K - 1,
		.flags	= IORESOURCE_MEM,
	},
	[1] = {
		.start	= IRQ_IIC5,
		.end	= IRQ_IIC5,
		.flags	= IORESOURCE_IRQ,
	},
};

struct platform_device s3c_device_i2c5 = {
	.name		= "s3c2440-i2c",
	.id		= 5,
	.num_resources	= ARRAY_SIZE(s3c_i2c_resource),
	.resource	= s3c_i2c_resource,
};

static struct s3c2410_platform_i2c touch_100k_i2c_data __initdata = {
	.flags		= 0,
	.slave_addr	= 0x10,
	.frequency	= 95*1000,
	.sda_delay	= 100,
};

static struct s3c2410_platform_i2c touch_300k_i2c_data __initdata = {
	.flags		= 0,
	.slave_addr	= 0x10,
	.frequency	= 200*1000,
	.sda_delay	=  100,
};

void __init s3c_i2c5_set_platdata(struct s3c2410_platform_i2c *pd)
{
	struct s3c2410_platform_i2c *npd;

	if (!pd) {
		pd = &touch_300k_i2c_data;
		pd->bus_num = 5;
	}

	npd = s3c_set_platdata(pd, sizeof(struct s3c2410_platform_i2c),
			       &s3c_device_i2c5);

	if (!npd->cfg_gpio)
		npd->cfg_gpio = s3c_i2c5_cfg_gpio;
}

void touch_s3c_i2c5_set_platdata(struct s3c2410_platform_i2c *pd, int check_value)
{
	struct s3c2410_platform_i2c *npd;

	if(check_value==0) // 100k
		pd = &touch_100k_i2c_data;
	else // 350k
		pd = &touch_300k_i2c_data;

	pd->bus_num = 5;
	
	npd = s3c_set_platdata(pd, sizeof(struct s3c2410_platform_i2c),
			       &s3c_device_i2c5);

	//if (!npd->cfg_gpio)
		npd->cfg_gpio = s3c_i2c5_cfg_gpio;
}
EXPORT_SYMBOL(touch_s3c_i2c5_set_platdata);

void s3c_i2c5_force_stop(void)
{
	void __iomem *regs;
	struct clk *clk;
	unsigned long iicstat;

	regs = ioremap(S3C_PA_IIC5, SZ_4K);
	if(regs == NULL) {
		printk(KERN_ERR "%s, cannot request IO\n", __func__);
		return;
	}

	clk = clk_get(&s3c_device_i2c5.dev, "i2c");
	if(clk == NULL || IS_ERR(clk)) {
		printk(KERN_ERR "%s, cannot get cloock\n", __func__);
		return;
	}

	clk_enable(clk);
	iicstat = readl(regs + S3C2410_IICSTAT);
	writel(iicstat & ~S3C2410_IICSTAT_TXRXEN, regs + S3C2410_IICSTAT);
	clk_disable(clk);

	iounmap(regs);
}
EXPORT_SYMBOL(s3c_i2c5_force_stop);

