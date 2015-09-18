/* linux/arch/arm/mach-exynos/setup-fb-s5p.c
 *
 * Copyright (c) 2010 Samsung Electronics Co., Ltd.
 *		http://www.samsung.com/
 *
 * Base FIMD controller configuration
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/

#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/err.h>
#include <linux/gpio.h>
#include <linux/kernel.h>
#include <linux/platform_device.h>
#include <linux/types.h>
#include <linux/io.h>

#include <mach/regs-clock.h>
#include <mach/regs-gpio.h>
#include <mach/map.h>
#include <mach/gpio.h>
#include <mach/board_rev.h>

#include <plat/clock.h>
#include <plat/gpio-cfg.h>
#include <plat/cpu.h>
#include <linux/regulator/consumer.h>

struct platform_device; /* don't need the contents */

#if defined(CONFIG_FB_S5P_LTN101AL03)
//#define FEATURE_S5P_LTN101AL03_DEBUG
#ifdef FEATURE_S5P_LTN101AL03_DEBUG
#define lcd_log(fmt, arg...) 	printk(fmt, ##arg)
#else
#define lcd_log(fmt, arg...)
#endif
#endif

#ifdef CONFIG_FB_S5P
static void s3cfb_gpio_setup_24bpp(unsigned int start, unsigned int size,
		unsigned int cfg, s5p_gpio_drvstr_t drvstr)
{
	u32 reg;

	s3c_gpio_cfgrange_nopull(start, size, cfg);

	for (; size > 0; size--, start++)
		s5p_gpio_set_drvstr(start, drvstr);

	/* Set FIMD0 bypass */
	reg = __raw_readl(S3C_VA_SYS + 0x0210);
	reg |= (1<<1);
	__raw_writel(reg, S3C_VA_SYS + 0x0210);
}

#if defined(CONFIG_FB_S5P_WA101S) || defined(CONFIG_FB_S5P_LTE480WV)
void s3cfb_cfg_gpio(struct platform_device *pdev)
{
	s3cfb_gpio_setup_24bpp(EXYNOS4_GPF0(0), 8, S3C_GPIO_SFN(2), S5P_GPIO_DRVSTR_LV4);
	s3cfb_gpio_setup_24bpp(EXYNOS4_GPF1(0), 8, S3C_GPIO_SFN(2), S5P_GPIO_DRVSTR_LV4);
	s3cfb_gpio_setup_24bpp(EXYNOS4_GPF2(0), 8, S3C_GPIO_SFN(2), S5P_GPIO_DRVSTR_LV4);
	s3cfb_gpio_setup_24bpp(EXYNOS4_GPF3(0), 4, S3C_GPIO_SFN(2), S5P_GPIO_DRVSTR_LV4);
}
#elif defined(CONFIG_FB_S5P_AMS369FG06)
void s3cfb_cfg_gpio(struct platform_device *pdev)
{
	s3cfb_gpio_setup_24bpp(EXYNOS4_GPF0(0), 8, S3C_GPIO_SFN(2), S5P_GPIO_DRVSTR_LV1);
	s3cfb_gpio_setup_24bpp(EXYNOS4_GPF1(0), 8, S3C_GPIO_SFN(2), S5P_GPIO_DRVSTR_LV1);
	s3cfb_gpio_setup_24bpp(EXYNOS4_GPF2(0), 8, S3C_GPIO_SFN(2), S5P_GPIO_DRVSTR_LV1);
	s3cfb_gpio_setup_24bpp(EXYNOS4_GPF3(0), 4, S3C_GPIO_SFN(2), S5P_GPIO_DRVSTR_LV1);
}
#elif defined(CONFIG_FB_S5P_LMS501KF03)
void s3cfb_cfg_gpio(struct platform_device *pdev)
{
	s3cfb_gpio_setup_24bpp(EXYNOS4_GPF0(0), 8, S3C_GPIO_SFN(2), S5P_GPIO_DRVSTR_LV4);
	s3cfb_gpio_setup_24bpp(EXYNOS4_GPF1(0), 8, S3C_GPIO_SFN(2), S5P_GPIO_DRVSTR_LV1);
	s3cfb_gpio_setup_24bpp(EXYNOS4_GPF2(0), 8, S3C_GPIO_SFN(2), S5P_GPIO_DRVSTR_LV1);
	s3cfb_gpio_setup_24bpp(EXYNOS4_GPF3(0), 4, S3C_GPIO_SFN(2), S5P_GPIO_DRVSTR_LV1);
}
#elif defined(CONFIG_FB_S5P_HT101HD1)
void s3cfb_cfg_gpio(struct platform_device *pdev)
{
	s3cfb_gpio_setup_24bpp(EXYNOS4_GPF0(0), 8, S3C_GPIO_SFN(2), S5P_GPIO_DRVSTR_LV1);
	s3cfb_gpio_setup_24bpp(EXYNOS4_GPF1(0), 8, S3C_GPIO_SFN(2), S5P_GPIO_DRVSTR_LV1);
	s3cfb_gpio_setup_24bpp(EXYNOS4_GPF2(0), 8, S3C_GPIO_SFN(2), S5P_GPIO_DRVSTR_LV1);
	s3cfb_gpio_setup_24bpp(EXYNOS4_GPF3(0), 6, S3C_GPIO_SFN(2), S5P_GPIO_DRVSTR_LV1);
}
#elif defined(CONFIG_FB_S5P_LTN101AL03)
void s3cfb_cfg_gpio(struct platform_device *pdev)
{
	s3cfb_gpio_setup_24bpp(EXYNOS4_GPF0(0), 8, S3C_GPIO_SFN(2), S5P_GPIO_DRVSTR_LV4);
	s3cfb_gpio_setup_24bpp(EXYNOS4_GPF1(0), 8, S3C_GPIO_SFN(2), S5P_GPIO_DRVSTR_LV4);
	s3cfb_gpio_setup_24bpp(EXYNOS4_GPF2(0), 8, S3C_GPIO_SFN(2), S5P_GPIO_DRVSTR_LV4);
	s3cfb_gpio_setup_24bpp(EXYNOS4_GPF3(0), 4, S3C_GPIO_SFN(2), S5P_GPIO_DRVSTR_LV4);
}
#endif
#endif

int s3cfb_clk_on(struct platform_device *pdev, struct clk **s3cfb_clk)
{
	struct clk *sclk = NULL;
	struct clk *mout_mpll = NULL;
	struct clk *lcd_clk = NULL;

	u32 rate = 0;
	int ret = 0;

	lcd_clk = clk_get(&pdev->dev, "lcd");
	if (IS_ERR(lcd_clk)) {
		dev_err(&pdev->dev, "failed to get operation clk for fimd\n");
		goto err_clk0;
	}

	ret = clk_enable(lcd_clk);
	if (ret < 0) {
		dev_err(&pdev->dev, "failed to clk_enable of lcd clk for fimd\n");
		goto err_clk0;
	}
	clk_put(lcd_clk);

	sclk = clk_get(&pdev->dev, "sclk_fimd");
	if (IS_ERR(sclk)) {
		dev_err(&pdev->dev, "failed to get sclk for fimd\n");
		goto err_clk1;
	}

	if (soc_is_exynos4210())
		mout_mpll = clk_get(&pdev->dev, "mout_mpll");
	else
		mout_mpll = clk_get(&pdev->dev, "mout_mpll_user");

	if (IS_ERR(mout_mpll)) {
		dev_err(&pdev->dev, "failed to get mout_mpll for fimd\n");
		goto err_clk2;
	}

	ret = clk_set_parent(sclk, mout_mpll);
	if (ret < 0) {
		dev_err(&pdev->dev, "failed to clk_set_parent for fimd\n");
		goto err_clk2;
	}

	ret = clk_set_rate(sclk, 74200000);
	if (ret < 0) {
		dev_err(&pdev->dev, "failed to clk_set_rate of sclk for fimd\n");
		goto err_clk2;
	}
	dev_dbg(&pdev->dev, "set fimd sclk rate to %d\n", rate);

	clk_put(mout_mpll);

	ret = clk_enable(sclk);
	if (ret < 0) {
		dev_err(&pdev->dev, "failed to clk_enable of sclk for fimd\n");
		goto err_clk2;
	}

	*s3cfb_clk = sclk;

	return 0;

err_clk2:
	clk_put(mout_mpll);
err_clk1:
	clk_put(sclk);
err_clk0:
	clk_put(lcd_clk);

	return -EINVAL;
}

int s3cfb_clk_off(struct platform_device *pdev, struct clk **clk)
{
	struct clk *lcd_clk = NULL;

	lcd_clk = clk_get(&pdev->dev, "lcd");
	if (IS_ERR(lcd_clk)) {
		printk(KERN_ERR "failed to get ip clk for fimd0\n");
		goto err_clk0;
	}

	clk_disable(lcd_clk);
	clk_put(lcd_clk);

	clk_disable(*clk);
	clk_put(*clk);

	*clk = NULL;

	return 0;

err_clk0:
	clk_put(lcd_clk);

	return -EINVAL;
}

void s3cfb_get_clk_name(char *clk_name)
{
	strcpy(clk_name, "sclk_fimd");
}

#define EXYNOS4_GPD_0_0_TOUT_0  (0x2)
#define EXYNOS4_GPD_0_1_TOUT_1  (0x2 << 4)
#define EXYNOS4_GPD_0_2_TOUT_2  (0x2 << 8)
#define EXYNOS4_GPD_0_3_TOUT_3  (0x2 << 12)

#if defined(CONFIG_FB_S5P_LTN101AL03)
void LTN101AL03_lvds_on(int onoff)
{
	int err;
	lcd_log(" ltn101al03_lvds_on =%d \n",onoff);
	if(onoff)
	{
		err = gpio_request_one(GPIO_nLVDS_PDN, GPIOF_OUT_INIT_HIGH, "GPM1(0)");
		mdelay(300);
	}
	else
	{
		err = gpio_request_one(GPIO_nLVDS_PDN, GPIOF_OUT_INIT_LOW, "GPM1(0)");
		mdelay(40);
	}
	
	if (err) {
		printk(KERN_ERR "failed to request GPM1(0) for "
			"lcd lvds control\n");
		//return err;
	}

	gpio_free(GPIO_nLVDS_PDN);
}
EXPORT_SYMBOL(LTN101AL03_lvds_on);

void LTN101AL03_backlight_crtl(int onoff)
{
	if(onoff)
	{
		//gpio_request(GPIO_LCD_BL_PWM, "GPIO_LCD_BL_PWM");
		s3c_gpio_cfgpin(GPIO_LCD_BL_PWM, S3C_GPIO_OUTPUT);
		//printk(" LTN101AL03_backlight_crtl output setting  \n");

	}
	else
	{
		s3c_gpio_cfgpin(GPIO_LCD_BL_PWM, S3C_GPIO_SFN(2));	
		//printk(" LTN101AL03_backlight_crtl PWM setting  \n");		
	}

}
EXPORT_SYMBOL(LTN101AL03_backlight_crtl);

int lcd_on_charging_ctrl(int onoff)
{
	int err;

	err = gpio_request(EXYNOS4212_GPM0(3), "GPIO_LCD_ONOFF_CHG");
	if (err) {
		lcd_log(KERN_ERR "failed to request GPM0(3) for "
			"GPIO_LCD_ONOFF_CHG\n");
		//return err;
	}
	s3c_gpio_cfgpin(EXYNOS4212_GPM0(3), S3C_GPIO_OUTPUT);

	if(onoff)
		gpio_set_value(EXYNOS4212_GPM0(3), 0);
	else
		gpio_set_value(EXYNOS4212_GPM0(3), 1);

	gpio_free(EXYNOS4212_GPM0(3));

	return 0;
}

void LTN101AL03_backlight_onoff(int onoff)
{
	int err;
	//LCD PWM on

	err = gpio_request(GPIO_LCD_BL_PWM, "GPIO_LCD_BL_PWM");
	if (err) {
		lcd_log(KERN_ERR "failed to request GPD0 for "
			"lcd backlight control\n");
		//return err;
	}
	s3c_gpio_cfgpin(GPIO_LCD_BL_PWM, S3C_GPIO_OUTPUT);
	
	lcd_log(" LTN101AL03_backlight_onoff %d \n",onoff);
	//gpio_direction_output(GPIO_LCD_BL_PWM, onoff);
	gpio_set_value(GPIO_LCD_BL_PWM, onoff);

	//s3c_gpio_cfgpin(GPIO_LCD_BL_PWM, S3C_GPIO_SFN(2));	
	lcd_on_charging_ctrl(onoff);

	gpio_free(GPIO_LCD_BL_PWM);

}
EXPORT_SYMBOL(LTN101AL03_backlight_onoff);

void LTN101AL03_lcd_onoff(int onoff)
{
	struct regulator *lcd_ldo = regulator_get(NULL, "vdd_lcd");
	
	if(onoff==1)
	{
		regulator_enable(lcd_ldo);
		regulator_put(lcd_ldo);
		mdelay(10); //T1+T2
	}
	else
	{
		regulator_force_disable(lcd_ldo);
		regulator_put(lcd_ldo);
		mdelay(500); //T1+T2
	}

}
EXPORT_SYMBOL(LTN101AL03_lcd_onoff);

int s3cfb_backlight_on(struct platform_device *pdev)
{
#if !defined(CONFIG_BACKLIGHT_PWM)
	int err;

	err = gpio_request_one(GPIO_LCD_BL_PWM, GPIOF_OUT_INIT_HIGH, "GPD0");
	if (err) {
		printk(KERN_ERR "failed to request GPD0 for "
			"lcd backlight control\n");
		return err;
	}
	
	gpio_free(GPIO_LCD_BL_PWM);
#endif
	return 0;
}

int s3cfb_backlight_off(struct platform_device *pdev)
{
#if !defined(CONFIG_BACKLIGHT_PWM)
	int err;

	err = gpio_request_one(GPIO_LCD_BL_PWM, GPIOF_OUT_INIT_LOW, "GPD0");
	if (err) {
		printk(KERN_ERR "failed to request GPD0 for "
			"lcd backlight control\n");
		return err;
	}
	s3c_gpio_cfgpin(GPIO_LCD_BL_PWM, S3C_GPIO_SFN(2));	

	gpio_free(GPIO_LCD_BL_PWM);
#endif
	return 0;
}

int s3cfb_lcd_on(struct platform_device *pdev)
{
#if 0
	struct regulator *lcd_ldo = regulator_get(NULL, "vdd_lcd");  
	int err=0;
	printk(" s3cfb_lcd_on start \n");

	ltn101al03_lvds_on(1);
	
	regulator_enable(lcd_ldo);

	regulator_put(lcd_ldo);

	mdelay(100); //T1+T2

	err = gpio_request_one(EXYNOS4212_GPM0(3), GPIOF_OUT_INIT_LOW, "GPM0(3)");

	if (err) {
		printk(KERN_ERR "failed to request GPM0(3) for "
			"lcd off charging control\n");
		return err;
	}
	printk(" s3cfb_lcd_on end \n");

	gpio_free(EXYNOS4212_GPM0(3));
#endif
	return 0;
}

int s3cfb_lcd_off(struct platform_device *pdev)
{
#if 0
	struct regulator *lcd_ldo = regulator_get(NULL, "vdd_lcd");  
	int err=0;

	msleep(300);	

	printk(" s3cfb_lcd_off  \n");
	
	regulator_disable(lcd_ldo);
	regulator_put(lcd_ldo);

	err = gpio_request_one(EXYNOS4212_GPM0(3), GPIOF_OUT_INIT_HIGH, "GPM0(3)");

	if (err) {
		printk(KERN_ERR "failed to request GPM0(3) for "
			"lcd off charging control\n");
		return err;
	}
	gpio_free(EXYNOS4212_GPM0(3));
	
	ltn101al03_lvds_on(0);
	
#endif
return 0;
}

#else
void s3cfb_cfg_gpio(struct platform_device *pdev)
{
	return;
}
int s3cfb_backlight_on(struct platform_device *pdev)
{
	return 0;
}

int s3cfb_backlight_off(struct platform_device *pdev)
{
	return 0;
}

int s3cfb_lcd_on(struct platform_device *pdev)
{
	return 0;
}

int s3cfb_lcd_off(struct platform_device *pdev)
{
	return 0;
}
#endif
