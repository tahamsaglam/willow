/*
 *  Copyright (C) 2010, Samsung Electronics Co. Ltd. All Rights Reserved.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/input.h>
#include <linux/input/mt.h>
#include <linux/interrupt.h>
#include <linux/i2c.h>
#include <linux/delay.h>
#include <linux/earlysuspend.h>
#include <linux/slab.h>
#include <linux/gpio.h>
#include <linux/i2c/1664s_driver_v13.h>
#include <asm/unaligned.h>
#include <linux/firmware.h>
#include <linux/string.h>
#ifdef CONFIG_MACH_MEHMET
#include <mach/mehmet_version.h>
extern MEHMET_HW_VERSION mehmet_get_hw_version( void );
#endif

#ifdef FEATURE_TOUCH_TOUCH_BOOSTER
#include <mach/cpufreq.h>
#define TOUCH_BOOSTER_TIME         3000
#define TOUCH_BOOSTER_LIMIT_CLK    800000

uint32_t touch_dvfs_cpufreq = 800000;
static bool tsp_press_status;
static bool touch_cpu_lock_status;
static int cpu_lv = -1;
#endif

#ifdef FEATURE_SUPPORT_FW_UPDATE_FROM_HEADER
static u8 firmware_mXT[] = {
#ifdef FEATURE_TOUCH_V20
    #include "mXT1664S__APP_V2-0-AB_extid_20.h"
#else
    #include "mXT1664S__APP_V1-3-AC_extid_20.h"
#endif
};
#endif

struct mxt_data *copy_data;
struct report_id_map_t *rid_map;
static int treat_median_error_status;
u8 max_report_id;
static bool rid_map_alloc;
int touch_is_pressed;
EXPORT_SYMBOL(touch_is_pressed);
static int mxt_enabled;

#ifdef FEATURE_TOUCH_1ST_POINT
#include <linux/hrtimer.h>

#define POLLING_TIMER_NS 1500 //15000000

u8 g_touch_1st_point_count = 0;
#endif

#ifdef FEATURE_SUPPORT_ZIG_TUNING
bool g_is_tuning_mode = 0;
#endif

#ifdef FEATURE_TOUCH_NOISE
#define TOUCH_NOISE_LEVEL    30
#define TOUCH_NOISE_THRESHOLD    40

bool g_power_noise = 0;
bool g_touch_noise_detect = 0;
bool g_touch_suspend = 0;
#endif

#ifdef FEATURE_TOUCH_PASSIVEPEN
#define PEN_INTERVAL 600 // 600ms
bool g_passivepen_mode = 1;
#endif

#ifdef FEATURE_TOUCH_ACTIVEPEN
extern bool g_activepen_mode;
#endif


volatile unsigned long pen_mode_time = 0;
volatile unsigned long pen_mode_start = 0;
volatile bool pen_mode_timeout = false;



#ifdef FEATURE_TOUCH_CONFIG_UPDATE 
#include <linux/fs.h>
#include <asm/unistd.h>
#include <asm/uaccess.h>

typedef enum
{
    ERR_OK,
    ERR_MODE,
    ERR_READID,
    ERR_ERASE,
    ERR_STATUS,
    ERR_ECC,
    ERR_DL_ERASE_FAIL,
    ERR_DL_PROGRAM_FAIL,
    ERR_DL_VERIFY_FAIL
} E_UPGRADE_ERR_TYPE;

typedef unsigned char         FTS_BYTE;     //8 bit
typedef unsigned short        FTS_WORD;    //16 bit
typedef unsigned int          FTS_DWRD;    //16 bit
typedef unsigned char         FTS_BOOL;    //8 bit

typedef struct _FTS_CTP_PROJECT_SETTING_T
{
    unsigned char uc_i2C_addr;             //I2C slave address (8 bit address)
    unsigned char uc_io_voltage;           //IO Voltage 0---3.3v;    1----1.8v
    unsigned char uc_panel_factory_id;     //TP panel factory ID
} FTS_CTP_PROJECT_SETTING_T;

#define FTS_NULL                0x0
#define FTS_TRUE                0x01
#define FTS_FALSE              0x0

#define FIRMWARE_NAME "mXT_1664_config.cfg"

#define TOUCH_LG		0
#define TOUCH_LG_CFG_VER	0x1A // from 0x00 (xcfg release date: 20131204)
#define TOUCH_HS		1
#define TOUCH_HS_CFG_VER	0x8B // from 0x80 (xcfg release date: 20131204)

int touch_v = -1; 

#ifdef FEATURE_TOUCH_V20
u8 * t0_info_data = NULL;
u8 t0_info_dataset[][2] = {
/*              touch id, touch config ver */
	{
		TOUCH_LG, TOUCH_LG_CFG_VER
	},
	{
		TOUCH_HS, TOUCH_HS_CFG_VER
	}
};

u8 * t37_config_data = NULL;
u8 t37_config_dataset[][130] = { // all 0
	{ // lg
		0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
		0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
		0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
		0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
		0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
		0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
		0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
		0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
		0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
		0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
		0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
		0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
		0,    0,    0,    0,    0,    0,    0,    0,    0,    0
	},
	{ // hs
		0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
		0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
		0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
		0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
		0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
		0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
		0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
		0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
		0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
		0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
		0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
		0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
		0,    0,    0,    0,    0,    0,    0,    0,    0,    0
	}
};

u8 * t68_config_data = NULL;
u8 t68_config_dataset[][72] = { // all 0
	{
		0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
		0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
		0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
		0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
		0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
		0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
		0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
		0,    0
	},
	{
		0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
		0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
		0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
		0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
		0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
		0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
		0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
		0,    0
	}
};

u8 * t38_config_data = NULL;
u8 t38_config_dataset[][64] = {
	{
		20,    0,    0,    0,    0,    0,    0,    0,    0,    0,
		 0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
		 0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
		 0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
		 0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
		 0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
		 0,    0,    0,    0
	},
	{
		1,    1,    0,    0,    0,    0,    0,    0,    0,    0,
		0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
		0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
		0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
		0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
		0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
		0,    0,    0,    0
	}
};

u8 * t71_config_data = NULL;
u8 t71_config_dataset[][168] = { // all 0
	{
		0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
		0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
		0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
		0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
		0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
		0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
		0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
		0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
		0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
		0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
		0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
		0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
		0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
		0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
		0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
		0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
		0,    0,    0,    0,    0,    0,    0,    0
	},
	{
		0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
		0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
		0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
		0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
		0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
		0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
		0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
		0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
		0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
		0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
		0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
		0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
		0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
		0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
		0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
		0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
		0,    0,    0,    0,    0,    0,    0,    0
	}
}; // sizeof(t71_config_data) = 168

u8 * t7_config_data = NULL;
u8 t7_config_dataset[][4] = {
	{
		64,  255,   50,    3
	},
	{
		255,  255,   255,    3
	}
};

u8 * t8_config_data = NULL;
u8 t8_config_dataset[][10] = {
	{
		  86,    0,    5,    1,    0,    0,    5,    0,   60,   25
	},
	{
		 86,    0,    5,    1,    0,    0,    5,    0,   60,   25
	}
};  

u16 * t9_config_data = NULL;
u16 t9_config_dataset[][45] = {
	{
		143,    0,    0,   32,   52,    0,  145,   90,    2,    3,
		 20,    0,    0,   79,   11,   20,   20,    0, 4095, 4095, /* 10, 11, 12, 13, 14, 15, 16, 17, 18-19, 20-21,(4095:2bytes) */
		  4,    4,    5,    6,  145,   30,  141,   18,   20,   22, /* 22, 23, 24, 25, 26, 27, 28, 29, 30, 31 */
		  0,    0,    0,    0,    2,  200,   30,    0,    0,    0, /* 32, 33, 34, 35, 36, 37, 38, 39, 40, 41 */
		  0,    0,    0,    0,    0
	},
	{
		143,    0,    0,   32,   52,    0,  160,   124,    2,    1,
		 20,    0,    0,   79,   11,   20,   20,    0, 4095, 4095, /* 10, 11, 12, 13, 14, 15, 16, 17, 18-19, 20-21,(4095:2bytes) */
		  4,    4,    5,    5,  149,   26,  143,   20,   20,   36, /* 22, 23, 24, 25, 26, 27, 28, 29, 30, 31 */
		  0,    0,    0,    0,    2,  180,   30,    0,    0,    0, /* 32, 33, 34, 35, 36, 37, 38, 39, 40, 41 */
		  0,    0,    0,    0,    0
	}
};

u8 * t15_config_data = NULL;
u8 t15_config_dataset[][11] = { // all 0
	{
		0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
		0
	},
	{
		0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
		0
	}
};

u8 * t18_config_data = NULL;
u8 t18_config_dataset[][2] = { // all 0
	{
		0,    0
	},
	{
		0,    0
	}
};

u8 * t19_config_data = NULL;
u8 t19_config_dataset[][6] = { // all 0
	{
		0,    0,    0,    0,    0,    0
	},
	{
		0,    0,    0,    0,    0,    0
	}
};

u8 * t24_config_data = NULL;
u8 t24_config_dataset[][14] = { // all 0
	{
		0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
		0,    0,    0,    0
	},
	{
		0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
		0,    0,    0,    0
	}
};

u8 * t25_config_data = NULL;
u8 t25_config_dataset[][12] = { // all 0
	{
		0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
		0,    0
	},
	{
		0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
		0,    0
	}
};

u8 * t27_config_data = NULL;
u8 t27_config_dataset[][6] = { // all 0
	{
		0,    0,    0,    0,    0,    0
	},
	{
		0,    0,    0,    0,    0,    0
	}
};

u8 * t40_config_data = NULL;
u8 t40_config_dataset[][5] = { // all 0
	{
		0,    0,    0,    0,    0
	},
	{
		0,    0,    0,    0,    0
	}
};

u8 * t42_config_data = NULL;
u8 t42_config_dataset[][13] = {
	{
		35,    0,   24,   20,  128,    0,   11,    1,    5,    0,
		 0,    0,    0
	},
	{
		33,    0,   22,   18,  128,    0,   11,    1,    5,    0,
		 0,    0,    0
	}
};

u8 * t43_config_data = NULL;
u8 t43_config_dataset[][10] = { // all 0
	{
		0,    0,    0,    0,    0,    0,    0,    0,    0,    0
	},
	{
		0,    0,    0,    0,    0,    0,    0,    0,    0,    0
	}
};  

u8 * t46_config_data = NULL;
u8 t46_config_dataset[][10] = {
	{
		4,    0,   16,   24,    0,    0,    1,    0,    0,   12
	},
	{
		4,    0,   16,   24,    0,    0,    1,    0,    0,   11
	}
};

u8 * t47_config_data = NULL;
u8 t47_config_dataset[][28] = {
	{
		  9,   15,   45,    4,    2,   50,   40,  254,    1,    8,
		192,    0,    3,    0,    0,    0,    0,    0,    0,    0,
		  0,    0,    0,    0,    0,    0,    0,    2
	},
	{
		  9,   13,   36,    4,    2,   50,   40,  255,    1,    8,
		192,    0,    3,    0,    0,    0,    0,    0,    0,    0,
		  0,    0,    0,    0,    0,    0,    0,    2
	}
};

u8 * t55_config_data = NULL;
u8 t55_config_dataset[][7] = {
	{
		0,    0,    0,    0,    0,    0,    0
	},
	{
		0,    0,    0,    0,    0,    0,    0
	}
};

u8 * t56_config_data = NULL;
u8 t56_config_dataset[][47] = {
	{
		 3,    0,    1,   43,   23,   24,   23,   23,   23,   23,
		23,   23,   22,   22,   22,   22,   21,   21,   21,   20,
		20,   19,   19,   19,   18,   18,   18,   18,   17,   17,
		17,   16,   16,   16,   16,   16,    0,    0,    0,    0,
		 0,    0,    0,    0,    0,    0,    0
	},
	{
		 3,    0,    1,   43,   16,   16,   16,   16,   16,   17,
		17,   18,   18,   18,   19,   19,   19,   19,   20,   20,
		20,   20,   20,   21,   21,   21,   21,   22,   22,   22,
		22,   22,   22,   23,   23,   23,    0,    0,    0,    0,
		 0,    0,    0,    0,    0,    0,    0
	}
};

u8 * t57_config_data = NULL;
u8 t57_config_dataset[][3] = { // all 0
	{
		0,    0,    0
	},
	{
		0,    0,    0
	}
};

u8 * t61_config_data = NULL;
u8 t61_config_dataset[][4] = { // all 0
	{
		0,    0,    0,    0
	},
	{
		0,    0,    0,    0
	}
};

u8 * t62_config_data = NULL;
u8 t62_config_dataset[][74] = {
	{
		 0,    3,    0,    6,    0,    0,    0,    0,   40,    0,
		 0,    0,    0,    0,    5,    0,   10,    5,    5,  145,
		25,   50,   52,   25,   54,    6,    6,    4,   54,    0,
		 0,    0,    0,    0,  128,  120,    2,    1,    0,    0,
		11,   20,   20,    4,    4,    5,    5,  165,   42,  142,
		18,   20,   20,    0,    0,    0,    0,    0,    0,    0,
		 0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
		 0,    0,    0,    0
	},
	{
		 0,    3,    0,    6,    0,    0,    0,    0,   40,    0,
		 0,    0,    0,    0,    5,    0,   10,    5,    5,  145,
		25,   50,   52,   25,   54,    6,    6,    4,   54,    0,
		 0,    0,    0,    0,  128,  120,    2,    1,    0,    0,
		11,   20,   20,    4,    4,    5,    5,  165,   42,  142,
		18,   20,   26,    0,    0,    0,    0,    0,    0,    0,
		 0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
		 0,    0,    0,    0
	}
};

u8 * t63_config_data = NULL;
u8 t63_config_dataset[][23] = { // all 0
	{
		0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
		0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
		0,    0,    0
	},
	{
		0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
		0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
		0,    0,    0
	}
};

u8 * t65_config_data = NULL;
u8 t65_config_dataset[][12] = { // all 0
	{
		0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
		0,    0
	},
	{
		0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
		0,    0
	}
};

u8 * t66_config_data = NULL;
u8 t66_config_dataset[][3] = { // all 0
	{
		0,    0,    0
	},
	{
		0,    0,    0
	}
};

u8 * t70_config_data = NULL;
u8 t70_config_dataset[][8] = { // all 0
	{
		0,    0,    0,    0,    0,    0,    0,    0
	},
	{
		0,    0,    0,    0,    0,    0,    0,    0
	}
};

u8 * t73_config_data = NULL;
u8 t73_config_dataset[][6] = { // all 0
	{
		0,    0,    0,    0,    0,    0
	},
	{
		0,    0,    0,    0,    0,    0
	}
};

u8 * t77_config_data = NULL;
u8 t77_config_dataset[][4] = { // all 0
	{
		0,    0,    0,    0
	},
	{
		0,    0,    0,    0
	}
};

u8 * t79_config_data = NULL;
u8 t79_config_dataset[][3] = { // all 0
	{
		0,    0,    0
	},
	{
		0,    0,    0
	}
};
#else // NOT FEATURE_TOUCH_V20
#ifdef FEATURE_TOUCH_V13
u8 t0_info_data[2] = {0, 2};//20130521_thinkware_mxt1664s_fw13_release.xcfg 4096 * 4096
u8 t37_config_data[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0};  
u8 t68_config_data[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0};
u8 t38_config_data[] = {255, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0};
u8 t7_config_data[] = {64, 255, 50, 3};
u8 t8_config_data[] = {80, 0, 5, 3, 0, 0, 5, 0, 20, 1};  
u16 t9_config_data[] = {143, 0, 0, 32, 52, 0, 135, 80, 2, 3,   20, 0, 0, 79, 10, 20, 20, 0, 4095, 4095,   4, 4, 5, 6, 145, 30, 141, 18, 20, 15,   0, 0, 0, 0, 0, 0, 0};
u8 t15_config_data[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0};
u8 t18_config_data[] = {0, 0};
u8 t19_config_data[] = {0, 0, 0, 0, 0, 0};
u8 t24_config_data[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0};
u8 t25_config_data[] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
u8 t27_config_data[] = {0, 0, 0, 0, 0, 0};
u8 t40_config_data[] = {0, 0, 0, 0, 0};
u8 t42_config_data[] = {35, 0, 24, 20, 128, 0, 10, 1, 5, 0,   0, 0, 0};
u8 t43_config_data[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};  
u8 t46_config_data[] = {4, 0, 16, 24, 0, 0, 1, 0, 0, 12};  
u8 t47_config_data[] = {9, 20, 45, 4, 2, 50, 40, 254, 1, 8,   192, 0, 3, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0};
u8 t55_config_data[] = {0, 0, 0, 0, 0, 0, 0};
u8 t56_config_data[] = {3, 0, 0, 43, 23, 24, 23, 23, 23, 23,   23, 23, 22, 22, 22, 22, 21, 21, 21, 20,   20, 19, 19, 19, 18, 18, 18, 18, 17, 17,   17, 16, 16, 16, 16, 16, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0};
u8 t57_config_data[] = {0, 0, 0};
u8 t61_config_data[] = {0, 0, 0, 0};
u8 t62_config_data[] = {3, 3, 0, 6, 0, 0, 0, 0, 40, 0,   0, 0, 0, 0, 5, 0, 10, 5, 5, 135,   25, 50, 52, 25, 54, 6, 6, 4, 54, 0,   0, 0, 0, 0, 135, 120, 2, 1, 0, 0,   10, 20, 20, 4, 4, 5, 6, 145, 30, 141,   18, 20, 26, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0};
u8 t63_config_data[] = {0, 6, 20, 215, 15, 97, 162, 3, 28, 255,   100, 0, 0, 0, 0, 0, 1, 1, 200, 20};  
u8 t65_config_data[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0};
u8 t66_config_data[] = {0, 0, 0, 0, 0};
u8 t70_config_data[] = {0, 0, 0, 0, 0, 0, 0, 0};
u8 t71_config_data[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0};
#endif
#endif // NOT FEATURE_TOUCH_V20
#endif /* FEATURE_TOUCH_CONFIG_UPDATE */

#ifdef FEATURE_TOUCH_DEBUG
u8 g_touch_debug = 3;
#endif

static u8 firmware_latest[] = {0x20, 0xAB};       /* 2.0 version, build_version */
//static gen_commandprocessor_t6_config_t t6_config= {0, };
static gen_powerconfig_t7_config_t t7_config = {0, };
static gen_acquisitionconfig_t8_config_t t8_config = {0, };
static touch_multitouchscreen_t9_config_t t9_config= {0, };
static touch_keyarray_t15_config_t t15_config = {0, };
static spt_comcconfig_t18_config_t t18_config = {0, };
#if defined(FEATURE_TOUCH_V13) || defined(FEATURE_TOUCH_V20)
static spt_gpiopwm_t19_config_t t19_config={0, };
#endif
//static proci_gripfacesuppression_t20_config_t t20_config={0, };
//static procg_noisesuppression_t22_config_t t22_config={0, };
static proci_onetouchgestureprocessor_t24 t24_config={0, };
static spt_selftest_t25_config_t t25_config={0, };
static proci_twotouchgestureprocessor_t27_config_t t27_config={0, };
//static spt_cteconfig_t28_config_t t28_config={0, };
static debug_diagnostic_t37_config_t t37_config={0, };
static spt_userdata_t38_config_t t38_config ;
static proci_gripsuppression_t40_config_t t40_config={0, };
static proci_touchsuppression_t42_config_t t42_config={0, };
static spt_digitizer_t43_t t43_config={0, };
static spt_cteconfig_t46_config_t t46_config={0, };
static proci_stylus_t47_config_t t47_config={0, };
//static procg_noisesuppression_t48_config_t t48_config={0, };
//static touch_proximity_t52_config_t t52_config={0, };
static proci_adaptivethreshold_t55_config_t t55_config={0, };
static proci_shieldless_t56_t t56_config={0, };
static proc_extratouchscreendata_t57_t t57_config={0, };
static spt_timer_t61_t t61_config={0, };
static proci_noisesupperssion_t62_t t62_config={0, };
static proci_activestylus_t63_t t63_config={0, };
static proci_lensbending_t65_t t65_config={0, };
static spt_goldenreferences_t66_t t66_config={0, };
static spt_serialdatacommand_t68_t t68_config={0, };
#if defined(FEATURE_TOUCH_V13) || defined(FEATURE_TOUCH_V20)
static spt_dynamicconfigurationcontroller_t70_t t70_config={0, };
static spt_dynamicconfigurationcontainer_t71_t t71_config={{0, }, };
#endif
#ifdef FEATURE_TOUCH_V20
static proci_zoneindication_t73_t t73_config={0, };
static spt_ctescanconfig_t77_t t77_config={0, };
static spt_toucheventtrigger_t79_t t79_config={0, };
#endif

/* for registers in runtime mode */
struct mxt_runmode_registers_t runmode_regs = {0, };

extern void set_touch_ic_check(int value );
extern int get_touch_ic_check(void);
struct s3c2410_platform_i2c;
extern void touch_s3c_i2c5_set_platdata(struct s3c2410_platform_i2c *pd, int check_value);

static int get_config(struct mxt_data *data, u8 type, int idx, int size, u8 *value);
static int set_config(struct mxt_data *data, u8 type, int idx, int size, u8 *value);

static u8       *object_type_name[80] = {
	[0]      = "Reserved",
	[1]      = "T1",
	[2]      = "T2",
	[3]      = "T3",
	[4]      = "T4",
	[5]      = "GEN_MESSAGEPROCESSOR_T5",
	[6]      = "GEN_COMMANDPROCESSOR_T6",
	[7]      = "GEN_POWERCONFIG_T7",
	[8]      = "GEN_ACQUIRECONFIG_T8",
	[9]      = "TOUCH_MULTITOUCHSCREEN_T9",
	[10]     = "T10",
	[11]     = "T11",
	[12]     = "T12",
	[13]     = "T13",
	[14]     = "T14",
	[15]     = "T15",
	[16]     = "T16",
	[17]     = "T17",
	[18]     = "SPT_COMCONFIG_T18",
	[19]     = "T19",
	[20]     = "T20",
	[21]     = "T21",
	[22]     = "PROCG_NOISESUPPRESSION_T22",
	[23]     = "TOUCH_PROXIMITY_T23",
	[24]     = "PROCI_ONETOUCHGESTUREPROCESSOR_T24",
	[25]     = "SPT_SELFTEST_T25",
	[26]     = "T26 - Obsolete",
	[27]     = "PROCI_TWOTOUCHGESTUREPROCESSOR_T27",
	[28]     = "SPT_CTECONFIG_T28",
	[29]     = "T29",
	[30]     = "T30",
	[31]     = "T31",
	[32]     = "T32",
	[33]     = "T33",
	[34]     = "T34",
	[35]     = "T35",
	[36]     = "T36",
	[37]     = "DEBUG_DIAGNOSTICS_T37",
	[38]     = "USER_DATA_T38",
	[39]     = "T39",
	[40]     = "PROCI_GRIPSUPPRESSION_T40",
	[41]     = "T40",
	[42]     = "PROCI_TOUCHSUPPRESSION_T42",
	[43]     = "SPT_DIGITIZER_T43",
	[44]     = "T44",
	[45]     = "T45",
	[46]     = "SPT_CTECONFIG_T46",
	[47]     = "T47",
	[48]     = "PROCG_NOISESUPPRESSION_T48",
	[49]     = "T49",
	[50]     = "T50",
	[51]     = "T51",
	[52]     = "T52",
	[53]     = "T53",
	[54]     = "T54",
	[55]     = "T55",
	[56]     = "PROCI_SHIELDLESS_T56",
	[57]     = "SPT_GENERICDATA_T57",
	[58]     = "T58",
	[59]     = "T59",
	[60]     = "T60",
	[61]     = "T61",
	[62]     = "PROCG_NOISESUPPRESSION_T62",
	[63]     = "PROCI_ACTIVESTYLUS_T63",
	[64]     = "T64",
	[65]     = "SPT_GOLDENREFERENCES_T66",
	[66]     = "T66",
	[67]     = "T67",
	[68]     = "SPT_SERIALDATACOMMAND_T68",
	[69]     = "T69",
	[70]     = "SPT_DYNAMICCONFIGURATIONCONTROLLER_T70",
	[71]     = "SPT_DYNAMICCONFIGURATIONCONTAINER_T71",
	[72]     = "T72",
#ifdef FEATURE_TOUCH_V20
	[73]     = "PROCI_ZONEINDICATION_T73",
	[74]     = "T74",
	[75]     = "T75",
	[76]     = "T76",
	[77]     = "SPT_CTESCANCONFIG_T77",
	[78]     = "T78",
	[79]     = "SPT_TOUCHEVENTTRIGGER_T79",
#endif
};
int atmel_tsp_config[] =  {
    DEBUG_DIAGNOSTIC_T37,
    SPT_SERIALDATACOMMAND_T68,
    SPT_USERDATA_T38,
    GEN_POWERCONFIG_T7,
    GEN_ACQUISITIONCONFIG_T8,
    TOUCH_MULTITOUCHSCREEN_T9,
    TOUCH_KEYARRAY_T15,
    SPT_COMMSCONFIG_T18,
#if defined(FEATURE_TOUCH_V13) || defined(FEATURE_TOUCH_V20)
    SPT_GPIOPWM_T19,
#endif
    PROCI_ONETOUCHGESTUREPROCESSOR_T24,
    SPT_SELFTEST_T25,
    PROCI_TWOTOUCHGESTUREPROCESSOR_T27,
    PROCI_GRIPSUPPRESSION_T40,
    PROCI_TOUCHSUPPRESSION_T42,
    SPT_DIGITIZER_T43,
    SPT_CTECONFIG_T46,
    PROCI_STYLUS_T47,
    PROCI_ADAPTIVETHRESHOLD_T55,
    PROCI_SHIELDLESS_T56,
    PROCI_EXTRATOUCHSCREENDATA_T57,
    SPT_TIMER_T61,
    PROCG_NOISESUPPRESSION_T62,
    PROCI_ACTIVESTYLUS_T63,    
    PROCI_LENSBENDING_T65,
    SPT_GOLDENREFERENCES_T66,
#if defined(FEATURE_TOUCH_V13) || defined(FEATURE_TOUCH_V20)
    SPT_DYNAMICCONFIGURATIONCONTROLLER_T70,
    SPT_DYNAMICCONFIGURATIONCONTAINER_T71,
#endif
#ifdef FEATURE_TOUCH_V20
    PROCI_ZONEINDICATION_T73,
    SPT_CTESCANCONFIG_T77,
    SPT_TOUCHEVENTTRIGGER_T79,
#endif
    RESERVED_T255,
};

int get_object_info(struct mxt_data *data, u8 object_type, u16 *size, u16 *address)
{
    int i;

    for ( i = 0; i < data->objects_len; i++ ) {
        if ( data->objects[i].object_type == object_type ) {
            *size = data->objects[i].size + 1;
            *address = data->objects[i].i2c_address;

            return 0;
        }
    }

    return -ENODEV;
}

int mxt_write_block(struct i2c_client *client,
        u16 addr,
        u16 length,
        u8 *value)
{
    int i;
    struct {
        __le16  le_addr;
        u8      data[256];

    } i2c_block_transfer;

    struct mxt_data *mxt;

    if (length > 256)
        return -EINVAL;

    mxt = i2c_get_clientdata(client);
    if (mxt != NULL)
        mxt->last_read_addr = -1;

    for (i = 0; i < length; i++)
        i2c_block_transfer.data[i] = *value++;

    i2c_block_transfer.le_addr = cpu_to_le16(addr);

    i = i2c_master_send(client, (u8 *) &i2c_block_transfer, length + 2);

    if ( i == (length + 2) ) {
#ifdef FEATURE_DEBUG_LOW_DATA_DUMP
        print_hex_dump(KERN_INFO, "MXT TX:", DUMP_PREFIX_NONE, 16, 1, &i2c_block_transfer, length+2, false);
#endif
        return length;

    } else {

        return -EIO;
    }

}

int mxt_GEN_POWERCONFIG_T7(struct mxt_data *mxt)

{
    struct i2c_client *client = mxt->client;
    u16 obj_addr=0, obj_size=0;
    int error=0;
    int i=0;

    get_object_info(copy_data,
            GEN_POWERCONFIG_T7, &obj_size, &obj_addr);
    //printk("mxt_GEN_POWERCONFIG_T7 obj_size=%d obj_addr=%d OBJECT_SIZE=4   OBJECT_ADDRESS=455 \n",obj_size,obj_addr );
    memset(&t7_config, 0, sizeof(t7_config));

    printk("t7_config sizeof (%d), [%d]\n", sizeof(t7_config), GEN_POWERCONFIG_T7);

    t7_config.nIDLEACQINT=t7_config_data[i++];
    t7_config.nACTVACQINT=t7_config_data[i++];
    t7_config.nACTV2IDLETO=t7_config_data[i++];
    t7_config.nCFG=t7_config_data[i++];

    error = mxt_write_block(client,
            obj_addr, obj_size, (u8 *)&t7_config);
    if (error < 0) {
        dev_err(&client->dev, "[TSP] mxt_write_block failed! "
                "(%s, %d)\n", __func__, __LINE__);
        return -EIO;
    }

    return 0;
}


int mxt_GEN_ACQUISITIONCONFIG_T8(struct mxt_data *mxt)
{
    struct i2c_client *client = mxt->client;
    u16 obj_addr=0, obj_size=0;
    int error=0;
    int i=0;

    get_object_info(copy_data,
            GEN_ACQUISITIONCONFIG_T8, &obj_size, &obj_addr);
    //printk("mxt_GEN_ACQUISITIONCONFIG_T8 obj_size=%d obj_addr=%d OBJECT_SIZE=10  OBJECT_ADDRESS=459 \n",obj_size,obj_addr );
    memset(&t8_config, 0, sizeof(t8_config));

    t8_config.nCHRGTIME=t8_config_data[i++];
    t8_config.nATCHDRIFT=t8_config_data[i++];
    t8_config.nTCHDRIFT=t8_config_data[i++];
    t8_config.nDRIFTST=t8_config_data[i++];
    t8_config.nTCHAUTOCAL=t8_config_data[i++];
    t8_config.nSYNC=t8_config_data[i++];
    t8_config.nATCHCALST=t8_config_data[i++];
    t8_config.nATCHCALSTHR=t8_config_data[i++];
    t8_config.nATCHFRCCALTHR=t8_config_data[i++];
    t8_config.nATCHFRCCALRATIO=t8_config_data[i++];

    error = mxt_write_block(client,
            obj_addr, obj_size, (u8 *)&t8_config);
    if (error < 0) {
        dev_err(&client->dev, "[TSP] mxt_write_block failed! "
                "(%s, %d)\n", __func__, __LINE__);
        return -EIO;
    }

    return 0;
}


int mxt_TOUCH_MULTITOUCHSCREEN_T9(struct mxt_data *mxt)

{
    struct i2c_client *client = mxt->client;
    u16 obj_addr=0, obj_size=0;
    int error=0;
    int i=0;

    get_object_info(copy_data,
            TOUCH_MULTITOUCHSCREEN_T9, &obj_size, &obj_addr);
    //printk("mxt_TOUCH_MULTITOUCHSCREEN_T9 obj_size=%d obj_addr=%d  OBJECT_SIZE=36 OBJECT_ADDRESS=469  \n",obj_size,obj_addr );
    memset(&t9_config, 0, sizeof(t9_config));

    t9_config.nCTRL=t9_config_data[i++];
    t9_config.nXORIGIN=t9_config_data[i++];
    t9_config.nYORIGIN=t9_config_data[i++];
    t9_config.nXSIZE=t9_config_data[i++];
    t9_config.nYSIZE=t9_config_data[i++];
    t9_config.nAKSCFG=t9_config_data[i++];
#ifdef FEATURE_TOUCH_ACTIVEPEN
//    if (g_activepen_mode) {
        t9_config.nBLEN=t9_config_data[i++];
//   } else {
//       t9_config.nBLEN=135;
//        i++;
//   }
#else
    t9_config.nBLEN=t9_config_data[i++];
#endif
    t9_config.nTCHTHR=t9_config_data[i++];
    t9_config.nTCHDI=t9_config_data[i++];
    t9_config.nORIENT=t9_config_data[i++];  
    t9_config.nMRGTIMEOUT=t9_config_data[i++];  
    t9_config.nMOVHYSTI=t9_config_data[i++]; 
    t9_config.nMOVHYSTN=t9_config_data[i++];
#if defined(FEATURE_TOUCH_V13) || defined(FEATURE_TOUCH_V20)
    t9_config.nRESERVED1=t9_config_data[i++];
#else
    t9_config.nMOVFILTER=t9_config_data[i++];
#endif
    t9_config.nNUMTOUCH=t9_config_data[i++];  
    t9_config.nMRGHYST=t9_config_data[i++];
    t9_config.nMRGTHR=t9_config_data[i++];
    t9_config.nAMPHYST=t9_config_data[i++];  
#if 0 // we need to set the fields of t9 structure as config says
#ifdef FEATURE_TOUCH_ACTIVEPEN // Active Pen (4096*4096)    
    t9_config.nXRANGE=4095; 
    t9_config.nYRANGE=4095; 
#else
    t9_config.nXRANGE=799; 
    t9_config.nYRANGE=1279; 
#endif
    i = i+2;
#else
    t9_config.nXRANGE=t9_config_data[i++];
    t9_config.nYRANGE=t9_config_data[i++];
#endif
    t9_config.nXLOCLIP=t9_config_data[i++];
    t9_config.nXHICLIP=t9_config_data[i++];
    t9_config.nYLOCLIP=t9_config_data[i++];
    t9_config.nYHICLIP=t9_config_data[i++];
    t9_config.nXEDGECTRL=t9_config_data[i++];
    t9_config.nXEDGEDIST=t9_config_data[i++];
    t9_config.nYEDGECTRL=t9_config_data[i++];
    t9_config.nYEDGEDIST=t9_config_data[i++];
    t9_config.nJUMPLIMIT=t9_config_data[i++]; 
    t9_config.nTCHHYST=t9_config_data[i++]; 
    t9_config.nXPITCH=t9_config_data[i++];
    t9_config.nYPITCH=t9_config_data[i++];
    t9_config.nNEXTTCHDI=t9_config_data[i++];    
    t9_config.nCFG=t9_config_data[i++];
#if defined(FEATURE_TOUCH_V13) || defined(FEATURE_TOUCH_V20)
    t9_config.nMOVFILTER2=t9_config_data[i++];
    t9_config.nMOVSMOOTH=t9_config_data[i++];
    t9_config.nMOVPRED=t9_config_data[i++];
#endif
#ifdef FEATURE_TOUCH_V20
    t9_config.nTRACKTHRSF=t9_config_data[i++];
    t9_config.nNOISETHRSF=t9_config_data[i++];
    t9_config.nRESERVED2=t9_config_data[i++];
    t9_config.nMRGTHRADJSTR=t9_config_data[i++];
    t9_config.nCUTOFFTHR=t9_config_data[i++];
#endif

    error = mxt_write_block(client, obj_addr,
            obj_size, (u8 *)&t9_config);
    /*
         [TOUCH_MULTITOUCHSCREEN_T9 instance 1
         OBJECT_ADDRESS=505
         OBJECT_SIZE=36
     */
    memset(&t9_config, 0, sizeof(t9_config));
#if defined(FEATURE_TOUCH_V13) || defined(FEATURE_TOUCH_V20)
    error = mxt_write_block(client, obj_addr+obj_size, obj_size, (u8 *)&t9_config);
#else
    error = mxt_write_block(client, 505,
            36, (u8 *)&t9_config);
#endif


    if (error < 0) {
        dev_err(&client->dev, "[TSP] mxt_write_block failed! "
                "(%s, %d)\n", __func__, __LINE__);
        return -EIO;
    }

    return 0;
}

int mxt_TOUCH_KEYARRAY_T15(struct mxt_data *mxt)

{
    struct i2c_client *client = mxt->client;
    u16 obj_addr=0, obj_size=0;
    int error=0;
    int i=0;

    get_object_info(copy_data,
            TOUCH_KEYARRAY_T15, &obj_size, &obj_addr);
    //printk("mxt_TOUCH_KEYARRAY_T15 obj_size=%d obj_addr=%d OBJECT_SIZE=11  OBJECT_ADDRESS=541\n",obj_size,obj_addr );
    memset(&t15_config, 0, sizeof(t15_config));

    t15_config.nCTRL=t15_config_data[i++];
    t15_config.nXORIGIN=t15_config_data[i++];
    t15_config.nYORIGIN=t15_config_data[i++];
    t15_config.nXSIZE=t15_config_data[i++];
    t15_config.nYSIZE=t15_config_data[i++];
    t15_config.nAKSCFG=t15_config_data[i++];
    t15_config.nBLEN=t15_config_data[i++];
    t15_config.nTCHTHR=t15_config_data[i++];
    t15_config.nTCHDI=t15_config_data[i++];
    t15_config.nRESERVED[0]=t15_config_data[i++];
    t15_config.nRESERVED[1]=t15_config_data[i++];

    error = mxt_write_block(client, obj_addr,
            obj_size, (u8 *)&t15_config);

    if (error < 0) {
        dev_err(&client->dev, "[TSP] mxt_write_block failed! "
                "(%s, %d)\n", __func__, __LINE__);
        return -EIO;
    }

    return 0;
}

int mxt_SPT_COMMSCONFIG_T18(struct mxt_data *mxt)

{
    struct i2c_client *client = mxt->client;
    u16 obj_addr=0, obj_size=0;
    int error=0;

    get_object_info(copy_data,
            SPT_COMMSCONFIG_T18, &obj_size, &obj_addr);
    //printk("mxt_SPT_COMMSCONFIG_T18 obj_size=%d obj_addr=%d  OBJECT_SIZE=2  OBJECT_ADDRESS=552 \n",obj_size,obj_addr );
    memset(&t18_config, 0, sizeof(t18_config));

    t18_config.nCTRL=0;
    t18_config.nCOMMAND=0;

    error = mxt_write_block(client, obj_addr,
            obj_size, (u8 *)&t18_config);

    if (error < 0) {
        dev_err(&client->dev, "[TSP] mxt_write_block failed! "
                "(%s, %d)\n", __func__, __LINE__);
        return -EIO;
    }

    return 0;
}

#if defined(FEATURE_TOUCH_V13) || defined(FEATURE_TOUCH_V20)
int mxt_SPT_GPIOPWM_T19(struct mxt_data *mxt)
{
    struct i2c_client *client = mxt->client;
    u16 obj_addr=0, obj_size=0;
    int error=0;
    int i=0;

    get_object_info(copy_data,    SPT_GPIOPWM_T19, &obj_size, &obj_addr);
    //printk("-> %s() obj_size=[%d] obj_addr=[%d]\n", __FUNCTION__, obj_size, obj_addr);
    memset(&t19_config, 0, sizeof(t19_config));

    t19_config.ctrl = t19_config_data[i++];
    t19_config.reportmask =t19_config_data[i++];
    t19_config.dir = t19_config_data[i++];
    t19_config.intpullup = t19_config_data[i++];
    t19_config.out = t19_config_data[i++];
    t19_config.wake = t19_config_data[i++];

    error = mxt_write_block(client, obj_addr, obj_size, (u8 *)&t19_config);

    if (error < 0) {
        dev_err(&client->dev, "[TSP] mxt_write_block failed! (%s, %d)\n", __func__, __LINE__);
        return -EIO;
    }

    return 0;
}
#endif

int mxt_PROCI_ONETOUCHGESTUREPROCESSOR_T24(struct mxt_data *mxt)

{
    struct i2c_client *client = mxt->client;
    u16 obj_addr=0, obj_size=0;
    int error=0;

    get_object_info(copy_data,
            PROCI_ONETOUCHGESTUREPROCESSOR_T24, &obj_size, &obj_addr);
    //printk("PROCI_ONETOUCHGESTUREPROCESSOR_T24 obj_size=%d obj_addr=%d OBJECT_SIZE=19  OBJECT_ADDRESS=554 \n",obj_size,obj_addr );
    memset(&t24_config, 0, sizeof(t24_config));

    t24_config.nCTRL=0;
    t24_config.nNUMGEST=0;
    t24_config.nGESTEN=0;
    t24_config.nPROCESS=0;
    t24_config.nTAPTO=0;
    t24_config.nFLICKTO=0;
    t24_config.nDRAGTO=0;
    t24_config.nSPRESSTO=0;
    t24_config.nLPRESSTO=0;
    t24_config.nREPPRESSTO=0;
    t24_config.nFLICKTHR=0;
    t24_config.nDRAGTHR=0;
    t24_config.nTAPTHR=0;
    t24_config.nTHROWTHR=0;

    error = mxt_write_block(client, obj_addr,
            obj_size, (u8 *)&t24_config);

    /*
         [PROCI_ONETOUCHGESTUREPROCESSOR_T24 instance 1
         OBJECT_ADDRESS=573
         OBJECT_SIZE=19
     */

    memset(&t24_config, 0, sizeof(t24_config));
    ////    error = mxt_write_block(client, 573,
    ////            19, (u8 *)&t24_config);
    error = mxt_write_block(client, obj_addr+obj_size, obj_size, (u8 *)&t24_config);


    if (error < 0) {
        dev_err(&client->dev, "[TSP] mxt_write_block failed! "
                "(%s, %d)\n", __func__, __LINE__);
        return -EIO;
    }

    return 0;
}

int mxt_SPT_SELFTEST_T25(struct mxt_data *mxt)

{
    struct i2c_client *client = mxt->client;
    u16 obj_addr=0, obj_size=0;
    int error=0;
    int i=0;

    get_object_info(copy_data,
            SPT_SELFTEST_T25, &obj_size, &obj_addr);
    //printk("mxt_SPT_SELFTEST_T25 obj_size=%d obj_addr=%d   OBJECT_SIZE=15  OBJECT_ADDRESS=592\n",obj_size,obj_addr );
    memset(&t25_config, 0, sizeof(t25_config));

    t25_config.nCTRL=t25_config_data[i++];
    t25_config.nCMD=t25_config_data[i++];
    t25_config.nUPSIGLIM[0]=t25_config_data[i++];
    t25_config.nLOSIGLIM[0]=t25_config_data[i++];
    t25_config.nUPSIGLIM[1]=t25_config_data[i++];
    t25_config.nLOSIGLIM[1]=t25_config_data[i++];
    t25_config.nUPSIGLIM[2]=t25_config_data[i++];
    t25_config.nLOSIGLIM[2]=t25_config_data[i++];
    t25_config.nPINDWELLUS=t25_config_data[i++];
#ifdef FEATURE_TOUCH_V20
    t25_config.nSIGRANGELIM[0]=t25_config_data[i++];
    t25_config.nSIGRANGELIM[1]=t25_config_data[i++];
    t25_config.nSIGRANGELIM[2]=t25_config_data[i++];
#endif

    error = mxt_write_block(client, obj_addr,
            obj_size, (u8 *)&t25_config);

    if (error < 0) {
        dev_err(&client->dev, "[TSP] mxt_write_block failed! "
                "(%s, %d)\n", __func__, __LINE__);
        return -EIO;
    }

    return 0;
}

int mxt_PROCI_TWOTOUCHGESTUREPROCESSOR_T27(struct mxt_data *mxt)

{
    struct i2c_client *client = mxt->client;
    u16 obj_addr=0, obj_size=0;
    int error=0;

    get_object_info(copy_data,
            PROCI_TWOTOUCHGESTUREPROCESSOR_T27, &obj_size, &obj_addr);
    //printk("PROCI_TWOTOUCHGESTUREPROCESSOR_T27 obj_size=%d obj_addr=%d   OBJECT_SIZE=7  OBJECT_ADDRESS=607\n",obj_size,obj_addr );
    memset(&t27_config, 0, sizeof(t27_config));

    error = mxt_write_block(client, obj_addr,
            obj_size, (u8 *)&t27_config);

    memset(&t27_config, 0, sizeof(t27_config));
    ////    error = mxt_write_block(client, 614,
    ////            7, (u8 *)&t25_config);
    error = mxt_write_block(client, obj_addr+obj_size, obj_size, (u8 *)&t25_config);

    if (error < 0) {
        dev_err(&client->dev, "[TSP] mxt_write_block failed! "
                "(%s, %d)\n", __func__, __LINE__);
        return -EIO;
    }

    return 0;
}

static int read_mem(struct mxt_data *data, u16 reg, u8 len, u8 *buf);

int mxt_SPT_USERDATA_T38_read(struct mxt_data *mxt)
{    
    u16 obj_addr=0, obj_size=0;    
    u8 value = 0;

    get_object_info(copy_data, SPT_USERDATA_T38,
            &obj_size, &obj_addr);

    read_mem(mxt, obj_addr, 1, &value);

    //printk("[ATMEL] obj_address=%d  Chip Config Version =%d \n",obj_addr,value);

    return value;
}

int mxt_SPT_USERDATA_T38(struct mxt_data *mxt)

{
    struct i2c_client *client = mxt->client;
    u16 obj_addr=0, obj_size=0;
    int error=0;

    get_object_info(copy_data,
            SPT_USERDATA_T38, &obj_size, &obj_addr);

    memset(&t38_config, 0, sizeof(t38_config));

    t38_config.nDATA[0]=t0_info_data[1]; // config version
    t38_config.nDATA[1]=t38_config_data[1]; // tsp vendor 0:LG 1:HS
    printk("Chip Config Ver (0x%02x), New Config Ver (0x%02x)\n", mxt_SPT_USERDATA_T38_read(mxt), t0_info_data[1]);
    printk("TSP Vendor write: %s\n", t38_config.nDATA[1] == 0 ? "LG" : "HS");

    error = mxt_write_block(client, obj_addr,
            obj_size, (u8 *)&t38_config);

    if (error < 0) {
        dev_err(&client->dev, "[TSP] mxt_write_block failed! "
                "(%s, %d)\n", __func__, __LINE__);
        return -EIO;
    }

    return 0;
}



int mxt_DEBUG_DIAGNOSTIC_T37(struct mxt_data *mxt)

{
    struct i2c_client *client = mxt->client;
    u16 obj_addr=0, obj_size=0;
    int error=0;

    get_object_info(copy_data,
            DEBUG_DIAGNOSTIC_T37, &obj_size, &obj_addr);

    memset(&t37_config, 0, sizeof(t37_config));

    //t37_config.nMODE=16;
    //t37_config.nPAGE=25;
    memcpy((u8*)&t37_config, (u8*)&t37_config_data, obj_size);

    error = mxt_write_block(client, obj_addr,
            obj_size, (u8 *)&t37_config);

    if (error < 0) {
        dev_err(&client->dev, "[TSP] mxt_write_block failed! "
                "(%s, %d)\n", __func__, __LINE__);
        return -EIO;
    }

    return 0;
}

int mxt_PROCI_GRIPSUPPRESSION_T40(struct mxt_data *mxt)
{
    struct i2c_client *client = mxt->client;
    u16 obj_addr=0, obj_size=0;
    int error=0;
    int i=0;

    get_object_info(copy_data,
            PROCI_GRIPSUPPRESSION_T40, &obj_size, &obj_addr);

    memset(&t40_config, 0, sizeof(t40_config));

    t40_config.nCTRL=t40_config_data[i++];
    t40_config.nXLOGRIP=t40_config_data[i++];
    t40_config.nXHIGRIP=t40_config_data[i++];
    t40_config.nYLOGRIP=t40_config_data[i++];
    t40_config.nYHIGRIP=t40_config_data[i++];

    error = mxt_write_block(client, obj_addr,
            obj_size, (u8 *)&t40_config);

    memset(&t40_config, 0, sizeof(t40_config));
    ////    error = mxt_write_block(client, 626,
    ////            5, (u8 *)&t40_config);
    error = mxt_write_block(client, obj_addr+obj_size, obj_size, (u8 *)&t40_config);

    if (error < 0) {
        dev_err(&client->dev, "[TSP] mxt_write_block failed! "
                "(%s, %d)\n", __func__, __LINE__);
        return -EIO;
    }

    return 0;
}

int mxt_PROCI_TOUCHSUPPRESSION_T42(struct mxt_data *mxt)
{
    struct i2c_client *client = mxt->client;
    u16 obj_addr=0, obj_size=0;
    int error=0;
    int i=0;

    get_object_info(copy_data,
            PROCI_TOUCHSUPPRESSION_T42, &obj_size, &obj_addr);

    memset(&t42_config, 0, sizeof(t42_config));

    t42_config.nCTRL=t42_config_data[i++];
    t42_config.nAPPRTHR=t42_config_data[i++];
    t42_config.nMAXAPPRAREA=t42_config_data[i++];
    t42_config.nMAXTCHAREA=t42_config_data[i++];
    t42_config.nSUPSTRENGTH=t42_config_data[i++];
    t42_config.nSUPEXTTO=t42_config_data[i++];
    t42_config.nMAXNUMTCHS=t42_config_data[i++];
    t42_config.nSHAPESTRENGTH=t42_config_data[i++];
    t42_config.nSUPDIST=t42_config_data[i++];
    t42_config.nDISTHYST=t42_config_data[i++];
#if defined(FEATURE_TOUCH_V13) || defined(FEATURE_TOUCH_V20)
    t42_config.nMAXSCRNAREA=t42_config_data[i++];
    t42_config.nCFG=t42_config_data[i++];
    t42_config.nRESERVED=t42_config_data[i++];
#endif

    error = mxt_write_block(client, obj_addr,
            obj_size, (u8 *)&t42_config);

    memset(&t42_config, 0, sizeof(t42_config));
#if defined(FEATURE_TOUCH_V13) || defined(FEATURE_TOUCH_V20)
    error = mxt_write_block(client, obj_addr+obj_size, obj_size, (u8 *)&t42_config);
#else
    error = mxt_write_block(client, 641,
            10, (u8 *)&t42_config);
#endif

    if (error < 0) {
        dev_err(&client->dev, "[TSP] mxt_write_block failed! "
                "(%s, %d)\n", __func__, __LINE__);
        return -EIO;
    }

    return 0;
}

int mxt_SPT_DIGITIZER_T43(struct mxt_data *mxt)
{
    struct i2c_client *client = mxt->client;
    u16 obj_addr=0, obj_size=0;
    int error=0;
    int i=0;

    get_object_info(copy_data,
            SPT_DIGITIZER_T43, &obj_size, &obj_addr);
    //printk("mxt_SPT_DIGITIZER_T43 obj_size=%d obj_addr=%d   OBJECT_SIZE=12 OBJECT_ADDRESS=651\n",obj_size,obj_addr );
    memset(&t43_config, 0, sizeof(t43_config));

    t43_config.nCTRL=t43_config_data[i++];
    t43_config.nHIDIDLERATE=t43_config_data[i++];
    t43_config.nXLENGTH=t43_config_data[i++];
    t43_config.nYLENGTH=t43_config_data[i++];
    t43_config.nRWKRATE=t43_config_data[i++];
    t43_config.nHEIGHTSCALE=t43_config_data[i++];
    t43_config.nHEIGHTOFFSET=t43_config_data[i++];
    t43_config.nWIDTHSCALE=t43_config_data[i++];
    t43_config.nWIDTHOFFSET=t43_config_data[i++];
#if defined(FEATURE_TOUCH_V13) || defined(FEATURE_TOUCH_V20)
    t43_config.nCFG=t43_config_data[i++];
#else
    t43_config.nRESERVED=t43_config_data[i++];
#endif

    error = mxt_write_block(client, obj_addr,
            obj_size, (u8 *)&t43_config);

    if (error < 0) {
        dev_err(&client->dev, "[TSP] mxt_write_block failed! "
                "(%s, %d)\n", __func__, __LINE__);
        return -EIO;
    }

    return 0;
}

int mxt_SPT_CTECONFIG_T46(struct mxt_data *mxt)
{
    struct i2c_client *client = mxt->client;
    u16 obj_addr=0, obj_size=0;
    int error=0;
    int i=0;

    get_object_info(copy_data,
            SPT_CTECONFIG_T46, &obj_size, &obj_addr);
    //printk("mxt_SPT_CTECONFIG_T46 obj_size=%d obj_addr=%d OBJECT_SIZE=11  OBJECT_ADDRESS=663 \n",obj_size,obj_addr );
    memset(&t46_config, 0, sizeof(t46_config));

    t46_config.nCTRL=t46_config_data[i++];
    t46_config.nMODE=t46_config_data[i++];
    t46_config.nIDLESYNCSPERX=t46_config_data[i++];
    t46_config.nACTVSYNCSPERX=t46_config_data[i++];
#ifndef FEATURE_TOUCH_V20
    t46_config.nADCSPERSYNC=t46_config_data[i++];
#else
    t46_config.nRESERVED1=t46_config_data[i++];
#endif
    t46_config.nPULSESPERADC=t46_config_data[i++];
    t46_config.nXSLEW=t46_config_data[i++];
#ifndef FEATURE_TOUCH_V20
    t46_config.nSYNCDELAY=t46_config_data[i++];
#else
    t46_config.nRESERVED2=t46_config_data[i++];
#endif
    t46_config.nXVOLTAGE=t46_config_data[i++];
    t46_config.nADCCTRL=t46_config_data[i++];

    error = mxt_write_block(client, obj_addr,
            obj_size, (u8 *)&t46_config);

    if (error < 0) {
        dev_err(&client->dev, "[TSP] mxt_write_block failed! "
                "(%s, %d)\n", __func__, __LINE__);
        return -EIO;
    }

    return 0;
}


int mxt_PROCI_STYLUS_T47(struct mxt_data *mxt)
{
    struct i2c_client *client = mxt->client;
    u16 obj_addr=0, obj_size=0;
    int error=0;
    int i=0;

    get_object_info(copy_data,
            PROCI_STYLUS_T47, &obj_size, &obj_addr);
    //printk("mxt_PROCI_STYLUS_T47 obj_size=%d obj_addr=%d  OBJECT_SIZE=22  OBJECT_ADDRESS=674 \n",obj_size,obj_addr );
    memset(&t47_config, 0, sizeof(t47_config));

#ifdef FEATURE_TOUCH_NOISE
    g_power_noise = gpio_get_value(nDC_OK) ? 0 : 1;
    if (g_power_noise) {
        t47_config.nCTRL=0;
        i++;
	printk(KERN_INFO "[ATMEL] passive pen disabled\n");
    } else {
        t47_config.nCTRL=t47_config_data[i++];
	printk(KERN_INFO "[ATMEL] passive pen enabled\n");
    }
#else
    t47_config.nCTRL=t47_config_data[i++];
#endif

    t47_config.nCONTMIN=t47_config_data[i++];
    t47_config.nCONTMAX=t47_config_data[i++];
    t47_config.nSTABILITY=t47_config_data[i++];
    t47_config.nMAXTCHAREA=t47_config_data[i++];
    t47_config.nAMPLTHR=t47_config_data[i++];
    t47_config.nSTYSHAPE=t47_config_data[i++];
    t47_config.nHOVERSUP=t47_config_data[i++];
    t47_config.nCONFTHR=t47_config_data[i++];
    t47_config.nSYNCSPERX=t47_config_data[i++];
    t47_config.nXPOSADJ=t47_config_data[i++];
    t47_config.nYPOSADJ=t47_config_data[i++];
    t47_config.nCFG=t47_config_data[i++];
    t47_config.nRESERVED[0]=t47_config_data[i++];
    t47_config.nRESERVED[1]=t47_config_data[i++];
    t47_config.nRESERVED[2]=t47_config_data[i++];
    t47_config.nRESERVED[3]=t47_config_data[i++];
    t47_config.nRESERVED[4]=t47_config_data[i++];
    t47_config.nRESERVED[5]=t47_config_data[i++];
    t47_config.nRESERVED[6]=t47_config_data[i++];
    t47_config.nSUPSTYTO=t47_config_data[i++];
    t47_config.nMAXNUMSTY=t47_config_data[i++];
#if defined(FEATURE_TOUCH_V13) || defined(FEATURE_TOUCH_V20)
    t47_config.nXEDGECTRL=t47_config_data[i++];
    t47_config.nXEDGEDIST=t47_config_data[i++];
    t47_config.nYEDGECTRL=t47_config_data[i++];
    t47_config.nYEDGEDIST=t47_config_data[i++];
#endif
#ifdef FEATURE_TOUCH_V20
    t47_config.nSUPTO=t47_config_data[i++];
    t47_config.nSUPCLASSMODE=t47_config_data[i++];
#endif

    error = mxt_write_block(client, obj_addr,
            obj_size, (u8 *)&t47_config);

    memset(&t47_config, 0, sizeof(t47_config));
#if defined(FEATURE_TOUCH_V13) || defined(FEATURE_TOUCH_V20)
    error = mxt_write_block(client, obj_addr+obj_size, obj_size, (u8 *)&t47_config);
#else
    error = mxt_write_block(client, 696,
            22, (u8 *)&t47_config);
#endif


    if (error < 0) {
        dev_err(&client->dev, "[TSP] mxt_write_block failed! "
                "(%s, %d)\n", __func__, __LINE__);
        return -EIO;
    }

    return 0;
}

int mxt_PROCI_ADAPTIVETHRESHOLD_T55(struct mxt_data *mxt)
{
    struct i2c_client *client = mxt->client;
    u16 obj_addr=0, obj_size=0;
    int error=0;
    int i=0;

    get_object_info(copy_data,
            PROCI_ADAPTIVETHRESHOLD_T55, &obj_size, &obj_addr);
    //printk("ADAPTIVE_T55 obj_size=%d obj_addr=%d OBJECT_SIZE=7  OBJECT_ADDRESS=718\n",obj_size,obj_addr );
    memset(&t55_config, 0, sizeof(t55_config));

    t55_config.nCTRL=t55_config_data[i++];
    t55_config.nTARGETTHR=t55_config_data[i++];
    t55_config.nTHRADJLIM=t55_config_data[i++];
    t55_config.nRESETSTEPTIME=t55_config_data[i++];
    t55_config.nFORCECHGDIST=t55_config_data[i++];
    t55_config.nFORCECHGTIME=t55_config_data[i++];

    error = mxt_write_block(client, obj_addr,
            obj_size, (u8 *)&t55_config);

    memset(&t55_config, 0, sizeof(t55_config));
    ////    error = mxt_write_block(client, 725,
    ////            7, (u8 *)&t55_config);
    error = mxt_write_block(client, obj_addr+obj_size, obj_size, (u8 *)&t55_config);

    if (error < 0) {
        dev_err(&client->dev, "[TSP] mxt_write_block failed! "
                "(%s, %d)\n", __func__, __LINE__);
        return -EIO;
    }

    return 0;
}

int mxt_PROCI_SHIELDLESS_T56(struct mxt_data *mxt)
{
    struct i2c_client *client = mxt->client;
    u16 obj_addr=0, obj_size=0;
    int error=0;
    int i=0;

    get_object_info(copy_data,
            PROCI_SHIELDLESS_T56, &obj_size, &obj_addr);
    //printk("mxt_PROCI_SHIELDLESS_T56 obj_size=%d obj_addr=%d   OBJECT_SIZE=51 OBJECT_ADDRESS=732\n",obj_size,obj_addr );
    memset(&t56_config, 0, sizeof(t56_config));

    t56_config.nCTRL=t56_config_data[i++];
    t56_config.nCOMMAND=t56_config_data[i++];
    t56_config.nOPTINT=t56_config_data[i++];
    t56_config.nINTTIME=t56_config_data[i++];
    t56_config.nINTDELAY[0]=t56_config_data[i++];
    t56_config.nINTDELAY[1]=t56_config_data[i++];
    t56_config.nINTDELAY[2]=t56_config_data[i++];
    t56_config.nINTDELAY[3]=t56_config_data[i++];
    t56_config.nINTDELAY[4]=t56_config_data[i++];
    t56_config.nINTDELAY[5]=t56_config_data[i++];
    t56_config.nINTDELAY[6]=t56_config_data[i++];
    t56_config.nINTDELAY[7]=t56_config_data[i++];
    t56_config.nINTDELAY[8]=t56_config_data[i++];
    t56_config.nINTDELAY[9]=t56_config_data[i++];
    t56_config.nINTDELAY[10]=t56_config_data[i++];
    t56_config.nINTDELAY[11]=t56_config_data[i++];
    t56_config.nINTDELAY[12]=t56_config_data[i++];
    t56_config.nINTDELAY[13]=t56_config_data[i++];
    t56_config.nINTDELAY[14]=t56_config_data[i++];
    t56_config.nINTDELAY[15]=t56_config_data[i++];
    t56_config.nINTDELAY[16]=t56_config_data[i++];
    t56_config.nINTDELAY[17]=t56_config_data[i++];
    t56_config.nINTDELAY[18]=t56_config_data[i++];
    t56_config.nINTDELAY[19]=t56_config_data[i++];
    t56_config.nINTDELAY[20]=t56_config_data[i++];
    t56_config.nINTDELAY[21]=t56_config_data[i++];
    t56_config.nINTDELAY[22]=t56_config_data[i++];
    t56_config.nINTDELAY[23]=t56_config_data[i++];
    t56_config.nINTDELAY[24]=t56_config_data[i++];
    t56_config.nINTDELAY[25]=t56_config_data[i++];
    t56_config.nINTDELAY[26]=t56_config_data[i++];
    t56_config.nINTDELAY[27]=t56_config_data[i++];
    t56_config.nINTDELAY[28]=t56_config_data[i++];
    t56_config.nINTDELAY[29]=t56_config_data[i++];
    t56_config.nINTDELAY[30]=t56_config_data[i++];
    t56_config.nINTDELAY[31]=t56_config_data[i++];
    t56_config.nMULTICUTGC=t56_config_data[i++];
    t56_config.nGCLIMIT=t56_config_data[i++];
    t56_config.nNCNCL=t56_config_data[i++];
    t56_config.nTOUCHBIAS=t56_config_data[i++];
    t56_config.nBASESCALE=t56_config_data[i++];
    t56_config.nSHIFTLIMIT=t56_config_data[i++];
    t56_config.nYLONOISEMUL=t56_config_data[i++];
    t56_config.nYLONOISEDIV=t56_config_data[i++];
    t56_config.nYHINOISEMUL=t56_config_data[i++];
    t56_config.nYHINOISEDIV=t56_config_data[i++];
    t56_config.nNCNCLMANIDX=t56_config_data[i++];

    error = mxt_write_block(client, obj_addr,
            obj_size, (u8 *)&t56_config);

    if (error < 0) {
        dev_err(&client->dev, "[TSP] mxt_write_block failed! "
                "(%s, %d)\n", __func__, __LINE__);
        return -EIO;
    }

    return 0;
}

int mxt_SPT_GENERICDATA_T57(struct mxt_data *mxt)
{
    struct i2c_client *client = mxt->client;
    u16 obj_addr=0, obj_size=0;
    int error=0;

    get_object_info(copy_data,
            PROCI_EXTRATOUCHSCREENDATA_T57, &obj_size, &obj_addr);
    //printk("SPT_GENERICDATA_T57 obj_size=%d obj_addr=%d  OBJECT_SIZE=3  OBJECT_ADDRESS=783 \n",obj_size,obj_addr );
    memset(&t57_config, 0, sizeof(t57_config));

    error = mxt_write_block(client, obj_addr,
            obj_size, (u8 *)&t57_config);

    t57_config.nCTRL = 227;
    t57_config.nAREATHR = 80;
    t57_config.nAREAHYST = 15;

    memset(&t57_config, 0, sizeof(t57_config));
    ////    error = mxt_write_block(client, 786,
    ////            3, (u8 *)&t57_config);
    error = mxt_write_block(client, obj_addr+obj_size, obj_size, (u8 *)&t57_config);


    if (error < 0) {
        dev_err(&client->dev, "[TSP] mxt_write_block failed! "
                "(%s, %d)\n", __func__, __LINE__);
        return -EIO;
    }

    return 0;
}


int mxt_SPT_TIMER_T61(struct mxt_data *mxt)
{
    struct i2c_client *client = mxt->client;
    u16 obj_addr=0, obj_size=0;
    int error=0;

    get_object_info(copy_data,
            SPT_TIMER_T61, &obj_size, &obj_addr);
    //printk("SPT_TIMER_T61 obj_size=%d obj_addr=%d OBJECT_SIZE=5  OBJECT_ADDRESS=789 \n",obj_size,obj_addr );
    memset(&t61_config, 0, sizeof(t61_config));

    t61_config.nCTRL=0;
    t61_config.nCMD=0;
    t61_config.nMODE=0;
    t61_config.nPERIOD=0;

    error = mxt_write_block(client, obj_addr,
            obj_size, (u8 *)&t61_config);

    memset(&t61_config, 0, sizeof(t61_config));
    ////    error = mxt_write_block(client, 789,
    ////            5, (u8 *)&t61_config);
    error = mxt_write_block(client, obj_addr+obj_size, obj_size, (u8 *)&t61_config);


    if (error < 0) {
        dev_err(&client->dev, "[TSP] mxt_write_block failed! "
                "(%s, %d)\n", __func__, __LINE__);
        return -EIO;
    }

    return 0;
}

int mxt_PROCG_NOISESUPPRESSION_T62(struct mxt_data *mxt)
{
    struct i2c_client *client = mxt->client;
    u16 obj_addr=0, obj_size=0;
    int error=0;
    int i=0;

    get_object_info(copy_data,
            PROCG_NOISESUPPRESSION_T62, &obj_size, &obj_addr);
    //printk("mxt_PROCG_NOISESUPPRESSION_T62 obj_size=%d obj_addr=%d OBJECT_SIZE=74  OBJECT_ADDRESS=799 \n",obj_size,obj_addr );
    memset(&t62_config, 0, sizeof(t62_config));


#ifdef FEATURE_TOUCH_NOISE
    if (g_power_noise) {
        t62_config.nCTRL=1;//39;
        i++;
    } else {
        t62_config.nCTRL=t62_config_data[i++];
    }
#else
    t62_config.nCTRL=t62_config_data[i++];
#endif

    t62_config.nCALCFG1=t62_config_data[i++];
    t62_config.nCALCFG2=t62_config_data[i++];

#ifdef FEATURE_TOUCH_NOISE
    if (g_power_noise) {
        t62_config.nCALCFG3=7;
        i++;
    } else if (g_activepen_mode) {
        t62_config.nCALCFG3=6;
        i++;
    } else {
        t62_config.nCALCFG3=t62_config_data[i++];
    }
#else
    t62_config.nCALCFG3=t62_config_data[i++];
#endif

    t62_config.nCFG1=t62_config_data[i++];
#ifdef FEATURE_TOUCH_V20
    t62_config.nFALLTHR=t62_config_data[i++];
#else
    t62_config.nRESERVED1=t62_config_data[i++];
#endif
    t62_config.nMINTHRADJ=t62_config_data[i++];
    t62_config.nBASEFREQ=t62_config_data[i++];
    t62_config.nMAXSELFREQ=t62_config_data[i++];
    t62_config.nFREQ[0]=t62_config_data[i++];
    t62_config.nFREQ[1]=t62_config_data[i++];
    t62_config.nFREQ[2]=t62_config_data[i++];
    t62_config.nFREQ[3]=t62_config_data[i++];
    t62_config.nFREQ[4]=t62_config_data[i++];
    t62_config.nHOPCNT=t62_config_data[i++];
    t62_config.nALTMAXSELFREQ=t62_config_data[i++];
    t62_config.nHOPCNTPER=t62_config_data[i++];
    t62_config.nHOPEVALTO=t62_config_data[i++];
    t62_config.nHOPST=t62_config_data[i++];

#if 0//def FEATURE_TOUCH_NOISE
    if (g_power_noise) {
        if(touch_v == TOUCH_HS)
	    t62_config.nNLGAIN=160;
	else
	    t62_config.nNLGAIN=145;
        i++;
    } else {
        t62_config.nNLGAIN=t62_config_data[i++];
    }
#else
    t62_config.nNLGAIN=t62_config_data[i++];
#endif

    t62_config.nMINNLTHR=t62_config_data[i++];
    t62_config.nINCNLTHR=t62_config_data[i++];
    t62_config.nADCSPERXTHR=t62_config_data[i++];
    t62_config.nNLTHRMARGIN=t62_config_data[i++];
    t62_config.nMAXADCSPERX=t62_config_data[i++];
    t62_config.nACTVADCSVLDNOD=t62_config_data[i++];
    t62_config.nIDLEADCSVLDNOD=t62_config_data[i++];
    t62_config.nMINGCLIMIT=t62_config_data[i++];
    t62_config.nMAXGCLIMIT=t62_config_data[i++];
    t62_config.nRESERVED[0]=t62_config_data[i++];
    t62_config.nRESERVED[1]=t62_config_data[i++];
    t62_config.nRESERVED[2]=t62_config_data[i++];
    t62_config.nRESERVED[3]=t62_config_data[i++];
    t62_config.nRESERVED[4]=t62_config_data[i++];

    t62_config.nBLEN_0=t62_config_data[i++];
    t62_config.nTCHTHR_0=t62_config_data[i++];
    t62_config.nTCHDI_0=t62_config_data[i++];
    t62_config.nMOVHYSTI_0=t62_config_data[i++];
    t62_config.nMOVHYSTN_0=t62_config_data[i++];
    t62_config.nMOVFILTER_0=t62_config_data[i++];
    t62_config.nNUMTOUCH_0=t62_config_data[i++];
    t62_config.nMRGHYST_0=t62_config_data[i++];
    t62_config.nMRGTHR_0=t62_config_data[i++];
    t62_config.nXLOCLIP_0=t62_config_data[i++];
    t62_config.nXHICLIP_0=t62_config_data[i++];
    t62_config.nYLOCLIP_0=t62_config_data[i++];
    t62_config.nYHICLIP_0=t62_config_data[i++];
    t62_config.nXEDGECTRL_0=t62_config_data[i++];
    t62_config.nXEDGEDIST_0=t62_config_data[i++];
    t62_config.nYEDGECTRL_0=t62_config_data[i++];
    t62_config.nYEDGEDIST_0=t62_config_data[i++];
    t62_config.nJUMPLIMIT_0=t62_config_data[i++];
    t62_config.nTCHHYST_0=t62_config_data[i++];
    t62_config.nNEXTTCHDI_0=t62_config_data[i++];

    t62_config.nBLEN_1=t62_config_data[i++];
    t62_config.nTCHTHR_1=t62_config_data[i++];
    t62_config.nTCHDI_1=t62_config_data[i++];
    t62_config.nMOVHYSTI_1=t62_config_data[i++];
    t62_config.nMOVHYSTN_1=t62_config_data[i++];
    t62_config.nMOVFILTER_1=t62_config_data[i++];
    t62_config.nNUMTOUCH_1=t62_config_data[i++];
    t62_config.nMRGHYST_1=t62_config_data[i++];
    t62_config.nMRGTHR_1=t62_config_data[i++];
    t62_config.nXLOCLIP_1=t62_config_data[i++];
    t62_config.nXHICLIP_1=t62_config_data[i++];
    t62_config.nYLOCLIP_1=t62_config_data[i++];
    t62_config.nYHICLIP_1=t62_config_data[i++];
    t62_config.nXEDGECTRL_1=t62_config_data[i++];
    t62_config.nXEDGEDIST_1=t62_config_data[i++];
    t62_config.nYEDGECTRL_1=t62_config_data[i++];
    t62_config.nYEDGEDIST_1=t62_config_data[i++];
    t62_config.nJUMPLIMIT_1=t62_config_data[i++];
    t62_config.nTCHHYST_1=t62_config_data[i++];
    t62_config.nNEXTTCHDI_1=t62_config_data[i++];

    error = mxt_write_block(client, obj_addr,
            obj_size, (u8 *)&t62_config);

    if (error < 0) {
        dev_err(&client->dev, "[TSP] mxt_write_block failed! "
                "(%s, %d)\n", __func__, __LINE__);
        return -EIO;
    }

    return 0;
}

int mxt_PROCI_ACTIVESTYLUS_T63(struct mxt_data *mxt)
{
    struct i2c_client *client = mxt->client;
    u16 obj_addr=0, obj_size=0;
    int error=0;
    int i=0;

    get_object_info(copy_data,
            PROCI_ACTIVESTYLUS_T63, &obj_size, &obj_addr);
    //printk("PROCI_ACTIVESTYLUS_T63 obj_size=%d obj_addr=%d OBJECT_SIZE=12  OBJECT_ADDRESS=873 \n",obj_size,obj_addr );
    memset(&t63_config, 0, sizeof(t63_config));

#if 0
//    t63_config.nCTRL  = 53; 
    t63_config.nCTRL  = 55; 
    i++;

//    if (g_activepen_mode) {
//        //t63_config.nCTRL  = 3; 
//        //i++;
//        t63_config.nCTRL  = (3 | t63_config_data[i++]);
//    }
//    else
//        t63_config.nCTRL  =t63_config_data[i++];
#else    
    t63_config.nCTRL  =t63_config_data[i++];
#endif
    t63_config.nMAXTCHAREA  =t63_config_data[i++];
    t63_config.nSIGPWR  =t63_config_data[i++];
    t63_config.nSIGRATIO  =t63_config_data[i++];
    t63_config.nSIGCNTMAX  =t63_config_data[i++];
    t63_config.nXADJUST  =t63_config_data[i++];
    t63_config.nYADJUST  =t63_config_data[i++];
    t63_config.nCOMMSFILTER  =t63_config_data[i++];
    t63_config.nDETADCSPERX  =t63_config_data[i++];
    t63_config.nSUPDIST  =t63_config_data[i++];
    t63_config.nSUPDISTHYST  =t63_config_data[i++];
    t63_config.nSUPTO  =t63_config_data[i++];
#if defined(FEATURE_TOUCH_V13) || defined(FEATURE_TOUCH_V20)
    t63_config.nXEDGECTRL = t63_config_data[i++];
    t63_config.nXEDGEDIST = t63_config_data[i++];
    t63_config.nYEDGECTRL = t63_config_data[i++];
    t63_config.nYEDGEDIST = t63_config_data[i++];
    t63_config.nPRESSUREFILTER = t63_config_data[i++];
    t63_config.nMOVFILTER2 = t63_config_data[i++];
    t63_config.nMOVSMOOTH = t63_config_data[i++];
    t63_config.nMOVPRED = t63_config_data[i++];
#endif
#ifdef FEATURE_TOUCH_V20
    t63_config.nMOVHYSTI= t63_config_data[i++];
    t63_config.nMOVHYSTN= t63_config_data[i++];
    t63_config.nSUPCLASSMODE= t63_config_data[i++];
#endif

    error = mxt_write_block(client, obj_addr,
            obj_size, (u8 *)&t63_config);

    memset(&t63_config, 0, sizeof(t63_config));
#if defined(FEATURE_TOUCH_V13) || defined(FEATURE_TOUCH_V20)
    error = mxt_write_block(client, obj_addr+obj_size, obj_size, (u8 *)&t63_config);
#else
    error = mxt_write_block(client, 897,
            17, (u8 *)&t63_config);
#endif

    if (error < 0) {
        dev_err(&client->dev, "[TSP] mxt_write_block failed! "
                "(%s, %d)\n", __func__, __LINE__);
        return -EIO;
    }

    return 0;
}

int mxt_PROCI_LENSBENDING_T65(struct mxt_data *mxt)
{
    struct i2c_client *client = mxt->client;
    u16 obj_addr=0, obj_size=0;
    int error=0;

    get_object_info(copy_data,
            PROCI_LENSBENDING_T65, &obj_size, &obj_addr);
    //printk("mxt_PROCI_LENSBENDING_T65 obj_size=%d obj_addr=%d OBJECT_SIZE=17  OBJECT_ADDRESS=897 \n",obj_size,obj_addr );
    memset(&t65_config, 0, sizeof(t65_config));

    error = mxt_write_block(client, obj_addr,
            obj_size, (u8 *)&t65_config);

    if (error < 0) {
        dev_err(&client->dev, "[TSP] mxt_write_block failed! "
                "(%s, %d)\n", __func__, __LINE__);
        return -EIO;
    }

    return 0;
}

int mxt_SPT_GOLDENREFERENCES_T66(struct mxt_data *mxt)
{
    struct i2c_client *client = mxt->client;
    u16 obj_addr=0, obj_size=0;
    int error=0;

    get_object_info(copy_data,
            SPT_GOLDENREFERENCES_T66, &obj_size, &obj_addr);
    //printk("mxt_SPT_GOLDENREFERENCES_T66 obj_size=%d obj_addr=%d OBJECT_SIZE=5  OBJECT_ADDRESS=914 \n",obj_size,obj_addr );
    memset(&t66_config, 0, sizeof(t66_config));

    error = mxt_write_block(client, obj_addr,
            obj_size, (u8 *)&t66_config);

    if (error < 0) {
        dev_err(&client->dev, "[TSP] mxt_write_block failed! "
                "(%s, %d)\n", __func__, __LINE__);
        return -EIO;
    }

    return 0;
}

int mxt_SPT_SERIALDATACOMMAND_T68(struct mxt_data *mxt)
{
    struct i2c_client *client = mxt->client;
    u16 obj_addr=0, obj_size=0;
    int error=0;

    get_object_info(copy_data,
            SPT_SERIALDATACOMMAND_T68, &obj_size, &obj_addr);
    //printk("SPT_SERIALDATACOMMAND_T68 obj_size=%d obj_addr=%d OBJECT_SIZE=73 ADDRESS=318 \n",obj_size,obj_addr );
    memset(&t68_config, 0, sizeof(t68_config));

    error = mxt_write_block(client, obj_addr,
            obj_size, (u8 *)&t68_config);

    if (error < 0) {
        dev_err(&client->dev, "[TSP] mxt_write_block failed! "
                "(%s, %d)\n", __func__, __LINE__);
        return -EIO;
    }

    return 0;
}

#if defined(FEATURE_TOUCH_V13) || defined(FEATURE_TOUCH_V20)
int mxt_SPT_DYNAMICCONFIGURATIONCONTROLLER_T70(struct mxt_data *mxt)
{
    struct i2c_client *client = mxt->client;
    u16 obj_addr=0, obj_size=0;
    int error=0;
    int i=0;

    get_object_info(copy_data,    SPT_DYNAMICCONFIGURATIONCONTROLLER_T70, &obj_size, &obj_addr);
    //printk("-> %s() obj_size=[%d] obj_addr=[%d]\n", __FUNCTION__, obj_size, obj_addr);
    memset(&t70_config, 0, sizeof(t70_config));

    t70_config.nCTRL = t70_config_data[i++];
    t70_config.nEVENT =t70_config_data[i++];
    t70_config.nOBJTYPE = t70_config_data[i++];
    t70_config.nRESERVED = t70_config_data[i++];
    t70_config.nOBJINST = t70_config_data[i++];
    t70_config.nDSTOFFSET = t70_config_data[i++];
    t70_config.nSRCOFFSET = t70_config_data[i++];
    t70_config.nLENGTH = t70_config_data[i++];

    error = mxt_write_block(client, obj_addr, obj_size, (u8 *)&t70_config);

    if (error < 0) {
        dev_err(&client->dev, "[TSP] mxt_write_block failed! (%s, %d)\n", __func__, __LINE__);
        return -EIO;
    }

    return 0;
}

int mxt_SPT_DYNAMICCONFIGURATIONCONTAINER_T71(struct mxt_data *mxt)
{
    struct i2c_client *client = mxt->client;
    u16 obj_addr=0, obj_size=0;
    int error=0;
    int i=0;

    get_object_info(copy_data,    SPT_DYNAMICCONFIGURATIONCONTAINER_T71, &obj_size, &obj_addr);
    printk("-> %s() obj_size=[%d] obj_addr=[%d]\n", __FUNCTION__, obj_size, obj_addr);
    memset(&t71_config, 0, sizeof(t71_config));

    for (i = 0; i < 168; i++)
        t71_config.nDATA[i] = t71_config_data[i];

    error = mxt_write_block(client, obj_addr, obj_size, (u8 *)&t71_config);

    if (error < 0) {
        dev_err(&client->dev, "[TSP] mxt_write_block failed! (%s, %d)\n", __func__, __LINE__);
        return -EIO;
    }

    return 0;
}
#endif

#ifdef FEATURE_TOUCH_V20
int mxt_PROCI_ZONEINDICATION_T73(struct mxt_data *mxt)
{
    struct i2c_client *client = mxt->client;
    u16 obj_addr=0, obj_size=0;
    int error=0;
    int i=0;

    get_object_info(copy_data,    PROCI_ZONEINDICATION_T73, &obj_size, &obj_addr);
    //printk("-> %s() obj_size=[%d] obj_addr=[%d]\n", __FUNCTION__, obj_size, obj_addr);
    memset(&t73_config, 0, sizeof(t73_config));

    t73_config.nCTRL = t73_config_data[i++];
    t73_config.nXORIGIN =t73_config_data[i++];
    t73_config.nYORIGIN = t73_config_data[i++];
    t73_config.nXSIZE = t73_config_data[i++];
    t73_config.nYSIZE = t73_config_data[i++];
    t73_config.nINDPERIOD = t73_config_data[i++];

    error = mxt_write_block(client, obj_addr, obj_size, (u8 *)&t73_config);

    if (error < 0) {
        dev_err(&client->dev, "[TSP] mxt_write_block failed! (%s, %d)\n", __func__, __LINE__);
        return -EIO;
    }

    return 0;
}

int mxt_SPT_CTESCANCONFIG_T77(struct mxt_data *mxt)
{
    struct i2c_client *client = mxt->client;
    u16 obj_addr=0, obj_size=0;
    int error=0;
    int i=0;

    get_object_info(copy_data, SPT_CTESCANCONFIG_T77, &obj_size, &obj_addr);
    //printk("-> %s() obj_size=[%d] obj_addr=[%d]\n", __FUNCTION__, obj_size, obj_addr);
    memset(&t77_config, 0, sizeof(t77_config));

    t77_config.nCTRL = t77_config_data[i++];
    t77_config.nACTVPRCSSCNEXT =t77_config_data[i++];
    t77_config.nXZOOMGAIN = t77_config_data[i++];
    t77_config.nXZOOMTCHTHR = t77_config_data[i++];

    error = mxt_write_block(client, obj_addr, obj_size, (u8 *)&t77_config);

    if (error < 0) {
        dev_err(&client->dev, "[TSP] mxt_write_block failed! (%s, %d)\n", __func__, __LINE__);
        return -EIO;
    }

    return 0;
}

int mxt_SPT_TOUCHEVENTTRIGGER_T79(struct mxt_data *mxt)
{
    struct i2c_client *client = mxt->client;
    u16 obj_addr=0, obj_size=0;
    int error=0;
    int i=0;

    get_object_info(copy_data, SPT_TOUCHEVENTTRIGGER_T79, &obj_size, &obj_addr);
    //printk("-> %s() obj_size=[%d] obj_addr=[%d]\n", __FUNCTION__, obj_size, obj_addr);
    memset(&t79_config, 0, sizeof(t79_config));

    t79_config.nCTRL = t79_config_data[i++];
    t79_config.nTOUCHCLASS =t79_config_data[i++];
    t79_config.nTCHRELTO = t79_config_data[i++];

    error = mxt_write_block(client, obj_addr, obj_size, (u8 *)&t79_config);

    if (error < 0) {
        dev_err(&client->dev, "[TSP] mxt_write_block failed! (%s, %d)\n", __func__, __LINE__);
        return -EIO;
    }

    return 0;
}
#endif

int mxt_config_settings(struct mxt_data *mxt)
{
    //printk("[TSP] mxt_config_settings  \n");

    if (mxt_GEN_POWERCONFIG_T7(mxt) < 0)
    {
        printk("[TSP] mxt_GEN_POWERCONFIG_T7 error  \n");
        return -1;
    }
    if (mxt_GEN_ACQUISITIONCONFIG_T8(mxt) < 0)
    {
        printk("[TSP] mxt_GEN_ACQUISITIONCONFIG_T8 error  \n");
        return -1;
    }
    if (mxt_TOUCH_MULTITOUCHSCREEN_T9(mxt) < 0)
    {
        printk("[TSP] mxt_TOUCH_MULTITOUCHSCREEN_T9 error  \n");
        return -1;
    }
    if (mxt_TOUCH_KEYARRAY_T15(mxt) < 0)
    {
        printk("[TSP] mxt_TOUCH_KEYARRAY_T15 error  \n");
        return -1;
    }

    if (mxt_SPT_COMMSCONFIG_T18(mxt) < 0)
    {
        printk("[TSP] mxt_TOUCH_KEYARRAY_T15 error  \n");
        return -1;
    }

    if (mxt_PROCI_GRIPSUPPRESSION_T40(mxt) < 0)
    {
        printk("[TSP] mxt_PROCI_GRIPSUPPRESSION_T40 error  \n");
        return -1;
    }
    if (mxt_PROCI_TOUCHSUPPRESSION_T42(mxt) < 0)
    {
        printk("[TSP] mxt_PROCI_TOUCHSUPPRESSION_T42 error  \n");
        return -1;
    }
    if (mxt_SPT_DIGITIZER_T43(mxt) < 0)    
    {
        printk("[TSP] mxt_SPT_DIGITIZER_T43 error  \n");
        return -1;
    }
    if (mxt_SPT_CTECONFIG_T46(mxt) < 0)
    {
        printk("[TSP] mxt_SPT_CTECONFIG_T46 error  \n");
        return -1;
    }
    if (mxt_PROCI_STYLUS_T47(mxt) < 0)
    {
        printk("[TSP] mxt_PROCI_STYLUS_T47 error  \n");
        return -1;
    }
    if (mxt_PROCI_SHIELDLESS_T56(mxt) < 0)
    {
        printk("[TSP] mxt_PROCI_SHIELDLESS_T56 error  \n");
        return -1;
    }
    if (mxt_PROCG_NOISESUPPRESSION_T62(mxt) < 0)
    {
        printk("[TSP] mxt_PROCG_NOISESUPPRESSION_T62 error  \n");
        return -1;
    }
    if (mxt_PROCI_ACTIVESTYLUS_T63(mxt) < 0)
    {
        printk("[TSP] mxt_PROCI_ACTIVESTYLUS_T63 error  \n");
        return -1;
    }    
    if (mxt_PROCI_LENSBENDING_T65(mxt) < 0)
    {
        printk("[TSP] mxt_PROCI_LENSBENDING_T65 error  \n");
        return -1;
    }    
    if (mxt_SPT_GOLDENREFERENCES_T66(mxt) < 0)
    {
        printk("[TSP] mxt_SPT_GOLDENREFERENCES_T66 error  \n");
        return -1;
    }        
    if (mxt_SPT_SERIALDATACOMMAND_T68(mxt) < 0)
    {
        printk("[TSP] mxt_SPT_SERIALDATACOMMAND_T68 error  \n");
        return -1;
    }
#if defined(FEATURE_TOUCH_V13) || defined(FEATURE_TOUCH_V20)
    if (mxt_SPT_DYNAMICCONFIGURATIONCONTROLLER_T70(mxt) < 0)
    {
        printk("[TSP] mxt_SPT_DYNAMICCONFIGURATIONCONTROLLER_T70 error  \n");
        return -1;
    }
    if (mxt_SPT_DYNAMICCONFIGURATIONCONTAINER_T71(mxt) < 0)
    {
        printk("[TSP] mxt_SPT_DYNAMICCONFIGURATIONCONTAINER_T71 error  \n");
        return -1;
    }
#endif
#ifdef FEATURE_TOUCH_V20
    if (mxt_PROCI_ZONEINDICATION_T73(mxt) < 0)
    {
        printk("[TSP] mxt_PROCI_ZONEINDICATION_T73 error  \n");
        return -1;
    }
    if (mxt_SPT_CTESCANCONFIG_T77(mxt) < 0)
    {
        printk("[TSP] mxt_SPT_CTESCANCONFIG_T77 error  \n");
        return -1;
    }
    if (mxt_SPT_TOUCHEVENTTRIGGER_T79(mxt) < 0)
    {
        printk("[TSP] mxt_SPT_TOUCHEVENTTRIGGER_T79 error  \n");
        return -1;
    }
#endif

    return 0;
}

int mxt_defconfig_settings(struct mxt_data *mxt, int config_num )
{
    int err = 0;

    printk(KERN_INFO "%s(): object_type = %s\n", __func__, object_type_name[config_num]);
    switch ( config_num ) {
        case GEN_POWERCONFIG_T7:
            err = mxt_GEN_POWERCONFIG_T7(mxt);
            break;
        case GEN_ACQUISITIONCONFIG_T8:
            err = mxt_GEN_ACQUISITIONCONFIG_T8(mxt);
            break;
        case TOUCH_MULTITOUCHSCREEN_T9:
            err = mxt_TOUCH_MULTITOUCHSCREEN_T9(mxt);
            break;
        case TOUCH_KEYARRAY_T15:
            err = mxt_TOUCH_KEYARRAY_T15(mxt);
            break;
        case SPT_COMMSCONFIG_T18:
            err = mxt_SPT_COMMSCONFIG_T18(mxt);
            break;
#if defined(FEATURE_TOUCH_V13) || defined(FEATURE_TOUCH_V20)
        case SPT_GPIOPWM_T19:
            err = mxt_SPT_GPIOPWM_T19(mxt);
            break;
#endif
        case PROCI_ONETOUCHGESTUREPROCESSOR_T24:
            err = mxt_PROCI_ONETOUCHGESTUREPROCESSOR_T24(mxt);
            break;
        case SPT_SELFTEST_T25:
            err = mxt_SPT_SELFTEST_T25(mxt);
            break;
        case PROCI_TWOTOUCHGESTUREPROCESSOR_T27:
            err = mxt_PROCI_TWOTOUCHGESTUREPROCESSOR_T27(mxt);
            break;
        case DEBUG_DIAGNOSTIC_T37:
            err = mxt_DEBUG_DIAGNOSTIC_T37(mxt);
            break;
        case SPT_USERDATA_T38:
            err = mxt_SPT_USERDATA_T38(mxt);
            break;
        case PROCI_GRIPSUPPRESSION_T40:
            err = mxt_PROCI_GRIPSUPPRESSION_T40(mxt);
            break;
        case PROCI_TOUCHSUPPRESSION_T42:
            err = mxt_PROCI_TOUCHSUPPRESSION_T42(mxt);
            break;
        case SPT_DIGITIZER_T43:
            err = mxt_SPT_DIGITIZER_T43(mxt);
            break;
        case SPT_CTECONFIG_T46:
            err = mxt_SPT_CTECONFIG_T46(mxt);
            break;
        case PROCI_STYLUS_T47:
            err = mxt_PROCI_STYLUS_T47(mxt);
            break;
        case PROCI_ADAPTIVETHRESHOLD_T55:
            err = mxt_PROCI_ADAPTIVETHRESHOLD_T55(mxt);
            break;
        case PROCI_SHIELDLESS_T56:
            err = mxt_PROCI_SHIELDLESS_T56(mxt);
            break;
        case PROCI_EXTRATOUCHSCREENDATA_T57:
            err = mxt_SPT_GENERICDATA_T57(mxt);
            break;
        case SPT_TIMER_T61:
            err = mxt_SPT_TIMER_T61(mxt);
            break;
        case PROCG_NOISESUPPRESSION_T62:
            err = mxt_PROCG_NOISESUPPRESSION_T62(mxt);
            break;
        case PROCI_ACTIVESTYLUS_T63:
            err = mxt_PROCI_ACTIVESTYLUS_T63(mxt);
            break;
        case PROCI_LENSBENDING_T65:
            err = mxt_PROCI_LENSBENDING_T65(mxt);
            break;
        case SPT_GOLDENREFERENCES_T66:
            err = mxt_SPT_GOLDENREFERENCES_T66(mxt);
            break;
        case SPT_SERIALDATACOMMAND_T68:
            err = mxt_SPT_SERIALDATACOMMAND_T68(mxt);
            break;
#if defined(FEATURE_TOUCH_V13) || defined(FEATURE_TOUCH_V20)
        case SPT_DYNAMICCONFIGURATIONCONTROLLER_T70:
            err = mxt_SPT_DYNAMICCONFIGURATIONCONTROLLER_T70(mxt);
            break;
        case SPT_DYNAMICCONFIGURATIONCONTAINER_T71:
            err = mxt_SPT_DYNAMICCONFIGURATIONCONTAINER_T71(mxt);
            break;
#endif
#ifdef FEATURE_TOUCH_V20
        case PROCI_ZONEINDICATION_T73:
            err = mxt_PROCI_ZONEINDICATION_T73(mxt);
            break;
        case SPT_CTESCANCONFIG_T77:
            err = mxt_SPT_CTESCANCONFIG_T77(mxt);
            break;
        case SPT_TOUCHEVENTTRIGGER_T79:
            err = mxt_SPT_TOUCHEVENTTRIGGER_T79(mxt);
            break;
#endif

        default:
            printk("[%s()] T%d NOT SUPPORT !!!\n", __FUNCTION__, config_num);
            err = -1;
            break;
    }

    return err;
}

#ifdef DEBUG_INFO
static u8       *object_type_name[63] = {
    /*      [0]     = "Reserved",   */
    /*      [2]     = "T2 - Obsolete",      */
    /*      [3]     = "T3 - Obsolete",      */
    [5]     = "GEN_MESSAGEPROCESSOR_T5",
    [6]     = "GEN_COMMANDPROCESSOR_T6",
    [7]     = "GEN_POWERCONFIG_T7",
    [8]     = "GEN_ACQUIRECONFIG_T8",
    [9]     = "TOUCH_MULTITOUCHSCREEN_T9",
    [15]    = "TOUCH_KEYARRAY_T15",
    [18]    = "SPT_COMCONFIG_T18",
    /*      [22]    = "PROCG_NOISESUPPRESSION_T22",*/
    /*      [23]    = "TOUCH_PROXIMITY_T23",*/
    /*      [24]    = "PROCI_ONETOUCHGESTUREPROCESSOR_T24",*/
    [25]    = "SPT_SELFTEST_T25",
    /*      [26]    = "T26 - Obsolete",*/
    /*      [27]    = "PROCI_TWOTOUCHGESTUREPROCESSOR_T27",*/
    /*      [28]    = "SPT_CTECONFIG_T28",*/
    [37]    = "DEBUG_DIAGNOSTICS_T37",
    [38]    = "USER_DATA_T38",
    [40]    = "PROCI_GRIPSUPPRESSION_T40",
    [42]    = "PROCI_TOUCHSUPPRESSION_T42",
    [43]    = "SPT_DIGITIZER_T43",
    [46]    = "SPT_CTECONFIG_T46",
    [48]    = "PROCG_NOISESUPPRESSION_T48",
    [56]    = "PROCI_SHIELDLESS_T56",
    [57]    = "SPT_GENERICDATA_T57",
    [62]    = "PROCG_NOISESUPPRESSION_T62",
    [63]    = "PROCI_ACTIVESTYLUS_T63",
    [65]    = "SPT_GOLDENREFERENCES_T66",
    [68]    = "SPT_SERIALDATACOMMAND_T68",
#if defined(FEATURE_TOUCH_V13) || defined(FEATURE_TOUCH_V20)
    [70]    = "SPT_DYNAMICCONFIGURATIONCONTROLLER_T70",
    [71]    = "SPT_DYNAMICCONFIGURATIONCONTAINER_T71",
#endif
#ifdef FEATURE_TOUCH_V20
    [73]    = "PROCI_ZONEINDICATION_T73",
    [77]    = "SPT_CTESCANCONFIG_T77",
    [79]    = "SPT_TOUCHEVENTTRIGGER_T79",
#endif
};
#endif

/* declare function proto type */
static void report_input_data_t9(struct mxt_data *data);
static void report_input_data_t63(struct mxt_data *data, u8 *msg);

static int read_mem(struct mxt_data *data, u16 reg, u8 len, u8 *buf)
{
    int ret;
    u16 le_reg = cpu_to_le16(reg);
    struct i2c_msg msg[2] = {
        {
            .addr = data->client->addr,
            .flags = 0,
            .len = 2,
            .buf = (u8 *)&le_reg,
        },
        {
            .addr = data->client->addr,
            .flags = I2C_M_RD,
            .len = len,
            .buf = buf,
        },
    };

    ret = i2c_transfer(data->client->adapter, msg, 2);
    if ( ret < 0 ) {
        printk("i2c failed ret = %d\n", ret);
        return ret;
    }

    return ret == 2 ? 0 : -EIO;
}

static int write_mem(struct mxt_data *data, u16 reg, u8 len, const u8 *buf)
{
    int ret;
    u8 tmp[len + 2];

    put_unaligned_le16(cpu_to_le16(reg), tmp);
    memcpy(tmp + 2, buf, len);

    ret = i2c_master_send(data->client, tmp, sizeof(tmp));

    if (ret < 0)
        return ret;

    return ret == sizeof(tmp) ? 0 : -EIO;
}

static int __devinit mxt_reset(struct mxt_data *data)
{
    u8 buf = 1u;
    return write_mem(data, data->cmd_proc + CMD_RESET_OFFSET, 1, &buf);
}

static int __devinit mxt_backup(struct mxt_data *data)
{
    u8 buf = 0x55u;
    return write_mem(data, data->cmd_proc + CMD_BACKUP_OFFSET, 1, &buf);
}

static int set_config(struct mxt_data *data, u8 type, int idx, int size, u8 *value)
{
	int ret = 0;
	u16 address = 0;
	u16 obj_size = 0;

	ret = get_object_info(data, type, &obj_size, &address);

	if(size == 0 && address == 0) {
		return 0;
	} else {
		printk(KERN_INFO ">>> %s(%s, addr:%d, size:%d, value:%d)\n",
				__func__, object_type_name[type], address+idx, size, *value);
		ret = write_mem(data, address + idx, size, value);
		if(ret) {
			printk(KERN_INFO ">>> %s() error\n");
		} else {
			*value = 0;
			read_mem(data, address + idx, size, value);
			printk(KERN_INFO ">>> read(%s, addr:%d, size:%d, value:%d)\n",
					object_type_name[type], address+idx, size, *value);
		}
		return ret;
	}
}

static int get_config(struct mxt_data *data, u8 type, int idx, int size, u8 *value)
{
	int ret = 0;
	u16 address = 0;
	u16 obj_size = 0;

	ret = get_object_info(data, type, &obj_size, &address);

	if(size == 0 && address == 0) {
		return 0;
	} else {
		*value = 0;
		ret = read_mem(data, address + idx, size, value);
		printk(KERN_INFO ">>> read(%s, addr:%d, size:%d, value:%d)\n",
				object_type_name[type], address+idx, size, *value);
		return ret;
	}
}
#ifdef NOT_USED_1664S
static int write_config(struct mxt_data *data, u8 type, const u8 *cfg)
{
    int ret;
    u16 address = 0;
    u16 size = 0;

    ret = get_object_info(data, type, &size, &address);

    if (size == 0 && address == 0)
        return 0;
    else
        return write_mem(data, address, size, cfg);
}
#endif

#ifdef NOT_USED_1664S
static int check_instance(struct mxt_data *data, u8 object_type)
{
    int i;

    for (i = 0; i < data->objects_len; i++) {
        if (data->objects[i].object_type == object_type)
            return data->objects[i].instances;
    }
    return 0;
}

static int init_write_config(struct mxt_data *data, u8 type, const u8 *cfg)
{
    int ret;
    u16 address = 0;
    u16 size = 0;
    u8 *temp;

    ret = get_object_info(data, type, &size, &address);

    if ((size == 0) || (address == 0)) {
        pr_err("%s error object_type(%d)\n", __func__, type);
        return -ENODEV;
    }

    ret = write_mem(data, address, size, cfg);

    if (check_instance(data, type)) {
#ifdef DEBUG_INFO
        pr_info("exist instance1 object (%d)\n", type);
#endif
        temp = kmalloc(size * sizeof(u8), GFP_KERNEL);
        memset(temp, 0, size);
        ret |= write_mem(data, address+size, size, temp);
        kfree(temp);
    }

    return ret;
}


static int change_config(struct mxt_data *data,
        u16 reg, u8 offeset, u8 change_value)
{
    u8 value = 0;

    value = change_value;
    return write_mem(data, reg+offeset, 1, &value);
}
#endif

static u32 __devinit crc24(u32 crc, u8 byte1, u8 byte2)
{
    static const u32 crcpoly = 0x80001B;
    u32 res;
    u16 data_word;

    data_word = (((u16)byte2) << 8) | byte1;
    res = (crc << 1) ^ (u32)data_word;

    if (res & 0x1000000)
        res ^= crcpoly;

    return res;
}

static int __devinit calculate_infoblock_crc(struct mxt_data *data,
        u32 *crc_pointer)
{
    u32 crc = 0;
    u8 mem[7 + data->objects_len * 6];
    int status;
    int i;

    status = read_mem(data, 0, sizeof(mem), mem);

    if (status)
        return status;

    for (i = 0; i < sizeof(mem) - 1; i += 2)
        crc = crc24(crc, mem[i], mem[i + 1]);

    *crc_pointer = crc24(crc, mem[i], 0) & 0x00FFFFFF;

    return 0;
}

uint8_t calibrate_chip_e(void)
{
    u8 cal_data = 1;
    int ret = 0;

    /* send calibration command to the chip */
    ret = write_mem(copy_data, copy_data->cmd_proc + CMD_CALIBRATE_OFFSET, 1, &cal_data);

    /* set flag for calibration lockup recovery if cal command was successful */
    if ( !ret ) {
        printk("calibration success !!!\n");
    }

    return ret;
}

static void treat_error_status(void)
{
    bool ta_status = 0;
    int error = 0;
    struct mxt_data *data = copy_data;

    data->read_ta_status(&ta_status);

    if (treat_median_error_status) {
        pr_err("Error status already treated\n");
        return;
    } else
        treat_median_error_status = 1;
#ifdef DEBUG_INFO
    printk("[ATMEL]_______Error status TA is[%d]\n", ta_status);
#endif
    if (ta_status) {
        if (error < 0)
            pr_err("failed to write error status\n");
    } else {
        if (error < 0)
            pr_err("failed to write error status\n");
    }
}

#ifdef FEATURE_TOUCH_TOUCH_BOOSTER
static void mxt_set_dvfs_off(struct work_struct *work)
{
    if ( mxt_enabled ) {
        disable_irq(IRQ_EINT(4));

        if ( touch_cpu_lock_status && !tsp_press_status ) {
            exynos_cpufreq_lock_free(DVFS_LOCK_ID_TSP);
            touch_cpu_lock_status = 0;
        }

        enable_irq(IRQ_EINT(4));
    }
}

static void mxt_set_dvfs_on(struct mxt_data *data)
{
    if (g_touch_debug == 1) printk("+++mxt_set_dvfs_on \n");
    cancel_delayed_work(&data->dvfs_dwork);
    if (cpu_lv < 0)
        exynos_cpufreq_get_level(touch_dvfs_cpufreq/*TOUCH_BOOSTER_LIMIT_CLK*/, &cpu_lv);
    exynos_cpufreq_lock(DVFS_LOCK_ID_TSP, cpu_lv);
    touch_cpu_lock_status = 1;
}
#endif

uint8_t reportid_to_type(struct mxt_data *data, u8 report_id, u8 *instance)
{
    struct report_id_map_t *report_id_map;
    report_id_map = rid_map;

    if (report_id <= max_report_id) {
        *instance = report_id_map[report_id].instance;
        return report_id_map[report_id].object_type;
    } else
        return 0;
}

static int __devinit mxt_init_touch_driver(struct mxt_data *data)
{
    struct object_t *object_table;
    struct report_id_map_t *report_id_map_t;
    u32 read_crc = 0;
    u32 calc_crc;
    u16 crc_address;
    u16 dummy;
    int i, j;
    u8 id[ID_BLOCK_SIZE];
    int ret;
    u8 type_count = 0;
    u8 tmp;
    int current_report_id, start_report_id;

    ret = read_mem(data, 0, sizeof(id), id);
    if (ret)
        return ret;

    printk("[%s()] family=[%#02x] variant=[%#02x] version=[%#02x] build=[%#02x] matrix X,Y size=[%d, %d] objects_len=[%d]\n", __FUNCTION__, id[0], id[1], id[2], id[3], id[4], id[5],id[6]);

    data->family_id = id[0];
    data->tsp_variant = id[1];
    data->tsp_version = id[2];
    data->tsp_build = id[3];
    data->objects_len = id[6];

    object_table = kmalloc(data->objects_len * sizeof(*object_table),
            GFP_KERNEL);
    if (!object_table)
        return -ENOMEM;

    ret = read_mem(data, OBJECT_TABLE_START_ADDRESS,
            data->objects_len * sizeof(*object_table),
            (u8 *)object_table);
    if (ret)
        goto err;

    max_report_id = 0;

    for (i = 0; i < data->objects_len; i++) {
        object_table[i].i2c_address =
            le16_to_cpu(object_table[i].i2c_address);
        max_report_id += object_table[i].num_report_ids *
            (object_table[i].instances + 1);
        tmp = 0;
        if (object_table[i].num_report_ids) {
            tmp = type_count + 1;
            type_count += object_table[i].num_report_ids *
                (object_table[i].instances + 1);
        }
        switch (object_table[i].object_type) {
            case TOUCH_MULTITOUCHSCREEN_T9:
                data->finger_type = tmp;
                pr_info("Finger type = %d\n",
                        data->finger_type);
                break;
            case GEN_MESSAGEPROCESSOR_T5:
                data->msg_proc_addr = object_table[i].i2c_address;
                data->msg_object_size = object_table[i].size + 1;
                break;
        }
    }
    if (rid_map_alloc) {
        rid_map_alloc = false;
        kfree(rid_map);
    }
    rid_map = kmalloc((sizeof(report_id_map_t) * max_report_id + 1),
            GFP_KERNEL);

    if (!rid_map) {
        kfree(object_table);
        return -ENOMEM;
    }
    rid_map_alloc = true;
    rid_map[0].instance = 0;
    rid_map[0].object_type = 0;
    current_report_id = 1;

    for (i = 0; i < data->objects_len; i++) {
        if (object_table[i].num_report_ids != 0) {
            for (j = 0; j <= object_table[i].instances; j++) {
                for (start_report_id = current_report_id;
                        current_report_id <
                        (start_report_id +
                         object_table[i].num_report_ids);
                        current_report_id++) {
                    rid_map[current_report_id].instance = j;
                    rid_map[current_report_id].object_type =
                        object_table[i].object_type;
                }
            }
        }
    }
    data->objects = object_table;

    /* Verify CRC */
    crc_address = OBJECT_TABLE_START_ADDRESS +
        data->objects_len * OBJECT_TABLE_ELEMENT_SIZE;

#ifdef __BIG_ENDIAN
#error The following code will likely break on a big endian machine
#endif
    ret = read_mem(data, crc_address, 3, (u8 *)&read_crc);
    if (ret)
        goto err;

    read_crc = le32_to_cpu(read_crc);

    ret = calculate_infoblock_crc(data, &calc_crc);
    if (ret)
        goto err;

    if (read_crc != calc_crc) {
        pr_err("CRC error\n");
        ret = -EFAULT;
        goto err;
    }

    ret = get_object_info(data, GEN_MESSAGEPROCESSOR_T5, &dummy,
            &data->msg_proc);
    if (ret)
        goto err;

    ret = get_object_info(data, GEN_COMMANDPROCESSOR_T6, &dummy,
            &data->cmd_proc);
    if (ret)
        goto err;

#ifdef DEBUG_INFO
    pr_info("maXTouch: %d Objects\n",
            data->objects_len);

    for (i = 0; i < data->objects_len; i++) {
        pr_info("Type:\t\t\t[%d]: %s\n",
                object_table[i].object_type,
                object_type_name[object_table[i].object_type]);
        pr_info("\tAddress:\t0x%04X\n",
                object_table[i].i2c_address);
        pr_info("\tSize:\t\t%d Bytes\n",
                object_table[i].size);
        pr_info("\tInstances:\t%d\n",
                object_table[i].instances);
        pr_info("\tReport Id's:\t%d\n",
                object_table[i].num_report_ids);
    }
#endif

    return 0;

err:
    kfree(object_table);
    return ret;
}

#ifdef FEATURE_TOUCH_1ST_POINT
static enum hrtimer_restart touch_1st_point_timer_func(struct hrtimer *timer)
{
    struct mxt_data *ts = container_of(timer, struct mxt_data, timer);

    mdelay(5);

    input_mt_slot(ts->input_dev, 0);
    input_mt_report_slot_state(ts->input_dev, MT_TOOL_FINGER, true);
    input_report_abs(ts->input_dev, ABS_MT_POSITION_X, ts->fingers[0].x);
    input_report_abs(ts->input_dev, ABS_MT_POSITION_Y, ts->fingers[0].y);
    input_report_abs(ts->input_dev, ABS_MT_TOUCH_MAJOR, ts->fingers[0].z);
    input_report_abs(ts->input_dev, ABS_MT_PRESSURE,ts->fingers[0].w);
    input_sync(ts->input_dev);

    if(g_touch_debug == 2) printk("1st point (%d, %d)\n", ts->fingers[0].x, ts->fingers[0].y);

    if(g_touch_1st_point_count < 1) {        
        hrtimer_start(&ts->timer, ktime_set(0, POLLING_TIMER_NS), HRTIMER_MODE_REL);
    } 

    g_touch_1st_point_count++;

    return HRTIMER_NORESTART;
}
#endif

#if 0
static void report_input_data_t63(struct mxt_data *data, u8 *msg)
{
    int i;
    int count = 0;
    int report_count = 0;

    for (i = 0; i < data->num_fingers; i++) {
        if (data->fingers[i].state == MXT_STATE_INACTIVE)
            continue;

        if (data->fingers[i].state == MXT_STATE_RELEASE) {
            input_mt_slot(data->input_dev, i);
            input_mt_report_slot_state(data->input_dev,
                    MT_TOOL_PEN, false);
            //#ifdef DEBUG_INFO
            //printk("[ATMEL]_______report_input_data_t9 release slot =%d \n",i);
            //#endif
        } else {
            input_mt_slot(data->input_dev, i);
            input_mt_report_slot_state(data->input_dev,
                    MT_TOOL_PEN, true);
            input_report_abs(data->input_dev, ABS_MT_POSITION_X,
                    data->fingers[i].x);
            input_report_abs(data->input_dev, ABS_MT_POSITION_Y,
                    data->fingers[i].y);
            input_report_abs(data->input_dev, ABS_MT_TOUCH_MAJOR,
                    data->fingers[i].z);
            input_report_abs(data->input_dev, ABS_MT_PRESSURE,data->fingers[i].w);
            //#ifdef DEBUG_INFO
            //printk("[ATMEL]_______report_input_data_t9 x=%d, y=%d w=%d\n",data->fingers[i].x,data->fingers[i].y,data->fingers[i].w);
            //#endif
        }
#ifdef _SUPPORT_SHAPE_TOUCH_
        input_report_abs(data->input_dev, ABS_MT_COMPONENT,
                data->fingers[i].component);
#endif
        //input_mt_sync(data->input_dev);

        report_count++;

        if (data->fingers[i].state == MXT_STATE_RELEASE) {
            data->fingers[i].state = MXT_STATE_INACTIVE;
            data->fingers[i].mcount = 0;
        } else {
            data->fingers[i].state = MXT_STATE_MOVE;
            count++;
        }
    }
    if ( report_count > 0 ) {
        input_sync(data->input_dev);
    }

    if (count)
        touch_is_pressed = 1;
    else
        touch_is_pressed = 0;

#ifdef FEATURE_TOUCH_TOUCH_BOOSTER
    if (count == 0) {
        if (touch_cpu_lock_status) {
            /*
                 if (touch_passivepen_status) {
                 touch_passivepen_status = 0;
            //touch_passivepen_id = id;
            mxt_PROCI_TOUCHSUPPRESSION_T42(copy_data);
            printk("+++touch_passivepen_status (%d)\n", touch_passivepen_status);
            }
             */

            if(g_touch_debug == 1) printk("cancel_delayed .. touch_cpu_lock_status (%d) \n", touch_cpu_lock_status);
            cancel_delayed_work(&data->dvfs_dwork);
            schedule_delayed_work(&data->dvfs_dwork,
                    msecs_to_jiffies(TOUCH_BOOSTER_TIME));
        }
        tsp_press_status = 0;
    } else
        tsp_press_status = 1;
#endif
    data->finger_mask = 0;
#ifdef FEATURE_TOUCH_1ST_POINT    
    if ((!data->fingers[0].mcount)&&(!g_touch_1st_point_count)&&(data->fingers[0].state == 2)) {            
        hrtimer_start(&data->timer, ktime_set(0, POLLING_TIMER_NS), HRTIMER_MODE_REL);            
    }    
#endif
}

#else

static void report_input_data_t63(struct mxt_data *data, u8 *msg)
{
       struct device *dev = &data->client->dev;
       struct input_dev *input_dev = data->input_dev;
       u8 id;
       u16 x, y;
       u8 pressure;

       if (!input_dev)
               return;

       /* stylus slots come after touch slots */
       id = 0; //data->num_touchids + (msg[0] - data->T63_reportid_min);
/*
       if (id < 0 || id > (data->num_touchids + data->num_stylusids)) {
               dev_err(dev, "invalid stylus id %d, max slot is %d\n",
                       id, data->num_stylusids);
               return;
       }
*/

       printk("[T63] data->num_fingers:%d\n", data->num_fingers);


       x = msg[3] | (msg[4] << 8);
       y = msg[5] | (msg[6] << 8);
       pressure = msg[7] & MXT_STYLUS_PRESSURE_MASK;

       x = (u16)((x * 1279) / 4096);
       y = (u16)((y * 799) / 4096);
/*
       dev_dbg(dev,
               "[%d] %c%c%c%c x: %d y: %d pressure: %d stylus:%c%c%c%c\n",
               id,
               (msg[1] & MXT_STYLUS_SUPPRESS) ? 'S' : '.',
               (msg[1] & MXT_STYLUS_MOVE)     ? 'M' : '.',
               (msg[1] & MXT_STYLUS_RELEASE)  ? 'R' : '.',
               (msg[1] & MXT_STYLUS_PRESS)    ? 'P' : '.',
               x, y, pressure,
               (msg[2] & MXT_STYLUS_BARREL) ? 'B' : '.',
               (msg[2] & MXT_STYLUS_ERASER) ? 'E' : '.',
               (msg[2] & MXT_STYLUS_TIP)    ? 'T' : '.',
               (msg[2] & MXT_STYLUS_DETECT) ? 'D' : '.');
*/
       input_mt_slot(input_dev, id);

       if (msg[2] & MXT_STYLUS_DETECT) {
//printk("[t63:%d]\n", __LINE__);
               input_mt_report_slot_state(input_dev, MT_TOOL_PEN, 1);
               input_report_abs(input_dev, ABS_MT_POSITION_X, x);
               input_report_abs(input_dev, ABS_MT_POSITION_Y, y);
               //input_report_abs(input_dev, ABS_MT_PRESSURE, pressure);
               input_report_abs(input_dev, ABS_MT_PRESSURE, 63);
       } else {
//printk("[t63:%d]\n", __LINE__);
               input_mt_report_slot_state(input_dev, MT_TOOL_PEN, 0);
       }

       input_report_key(input_dev, BTN_STYLUS, (msg[2] & MXT_STYLUS_ERASER));
       input_report_key(input_dev, BTN_STYLUS2, (msg[2] & MXT_STYLUS_BARREL));

       input_sync(input_dev);
}
#endif
static void mxt_tune_dwork(struct work_struct *work)
{
	u8 value1, value2;

	printk(KERN_INFO " **** tune_dwork ****\n");

	value1 = 5;
	set_config(copy_data, GEN_ACQUISITIONCONFIG_T8, 2, 1, &value1);

	value1 = 20, value2 = 45;
	set_config(copy_data, PROCI_STYLUS_T47, 1, 1, &value1);
	set_config(copy_data, PROCI_STYLUS_T47, 2, 1, &value2);
}

static void report_input_data_t9(struct mxt_data *data)
{
    int i;
    int count = 0;
    int report_count = 0;

    for (i = 0; i < data->num_fingers; i++) {
        if (data->fingers[i].state == MXT_STATE_INACTIVE)
            continue;

        if (data->fingers[i].state == MXT_STATE_RELEASE) {
            input_mt_slot(data->input_dev, i);
            input_mt_report_slot_state(data->input_dev,
                    MT_TOOL_FINGER, false);
            //#ifdef DEBUG_INFO
            //printk("[ATMEL]_______report_input_data_t9 release slot =%d \n",i);
            //#endif
        } else {
            input_mt_slot(data->input_dev, i);
            input_mt_report_slot_state(data->input_dev,
                    MT_TOOL_FINGER, true);
            input_report_abs(data->input_dev, ABS_MT_POSITION_X,
                    data->fingers[i].x);

            input_report_abs(data->input_dev, ABS_MT_POSITION_Y,
                    data->fingers[i].y);

	    // 20130829 : heesung TSP
            //input_report_abs(data->input_dev, ABS_MT_POSITION_Y,
            //        800 - data->fingers[i].y);

            input_report_abs(data->input_dev, ABS_MT_TOUCH_MAJOR,
                    data->fingers[i].z);
            input_report_abs(data->input_dev, ABS_MT_PRESSURE,data->fingers[i].w);
            //#ifdef DEBUG_INFO
            //printk("[ATMEL]_______report_input_data_t9 x=%d, y=%d w=%d\n",data->fingers[i].x,data->fingers[i].y,data->fingers[i].w);
			//printk("[T9] X(%d), Y(%d)\n", data->fingers[i].x, data->fingers[i].y);
            //#endif
        }
#ifdef _SUPPORT_SHAPE_TOUCH_
        input_report_abs(data->input_dev, ABS_MT_COMPONENT,
                data->fingers[i].component);
#endif
        //input_mt_sync(data->input_dev);

        report_count++;

        if (data->fingers[i].state == MXT_STATE_RELEASE) {
            data->fingers[i].state = MXT_STATE_INACTIVE;
            data->fingers[i].mcount = 0;
        } else {
            data->fingers[i].state = MXT_STATE_MOVE;
            count++;
        }
    }
    if ( report_count > 0 ) {
        input_sync(data->input_dev);
    }

    if (count)
        touch_is_pressed = 1;
    else
        touch_is_pressed = 0;

#ifdef FEATURE_TOUCH_TOUCH_BOOSTER
    if (count == 0) {
        if (touch_cpu_lock_status) {
            /*
                 if (touch_passivepen_status) {
                 touch_passivepen_status = 0;
            //touch_passivepen_id = id;
            mxt_PROCI_TOUCHSUPPRESSION_T42(copy_data);
            printk("+++touch_passivepen_status (%d)\n", touch_passivepen_status);
            }
             */

            if(g_touch_debug == 1) printk("cancel_delayed .. touch_cpu_lock_status (%d) \n", touch_cpu_lock_status);
            cancel_delayed_work(&data->dvfs_dwork);
            schedule_delayed_work(&data->dvfs_dwork,
                    msecs_to_jiffies(TOUCH_BOOSTER_TIME));
        }
        tsp_press_status = 0;
    } else
        tsp_press_status = 1;
#endif
    data->finger_mask = 0;
#ifdef FEATURE_TOUCH_1ST_POINT    
    if ((!data->fingers[0].mcount)&&(!g_touch_1st_point_count)&&(data->fingers[0].state == 2)) {            
        hrtimer_start(&data->timer, ktime_set(0, POLLING_TIMER_NS), HRTIMER_MODE_REL);            
    }    
#endif
}

static irqreturn_t mxt_irq_thread(int irq, void *ptr)
{
    struct mxt_data *data = ptr;
    int id, i;
    u8 msg[data->msg_object_size];
    u8 touch_message_flag = 0;
    u16 obj_address = 0;
    u16 size;
    u8 value;
    int error;
    u8 object_type, instance, finger_cnt=0;        

#ifdef FEATURE_TOUCH_1ST_POINT
    hrtimer_cancel(&data->timer);
    g_touch_1st_point_count = 0;
#endif        
    g_touch_noise_detect = 0;
    do {

        touch_message_flag = 0;
        if (read_mem(data, data->msg_proc, sizeof(msg), msg)) {
#ifdef FEATURE_TOUCH_TOUCH_BOOSTER
            if (touch_cpu_lock_status) {
                exynos_cpufreq_lock_free(DVFS_LOCK_ID_TSP);
                touch_cpu_lock_status = 0;
            }
#endif
            return IRQ_HANDLED;
        }

#ifdef FEATURE_DEBUG_LOW_DATA_DUMP
        print_hex_dump(KERN_INFO, "MXT MSG:", DUMP_PREFIX_NONE, 16, 1, msg, sizeof(msg), false);
#endif
        object_type = reportid_to_type(data, msg[0] , &instance);

        //dev_err(&data->client->dev, "(%s, %d) object_type = %d\n", __func__, __LINE__, object_type);
        if (object_type == GEN_COMMANDPROCESSOR_T6) {
            if (msg[1] == 0x00) /* normal mode */
                printk("normal mode\n");
            if ((msg[1]&0x04) == 0x04) /* I2C checksum error */
                printk("I2C checksum error\n");
            if ((msg[1]&0x08) == 0x08) { /* config error */
                printk("config error\n");
                // config data가 손상되었을 경우에 대한 방어코드(config data writing중 power off시 발생가능)
				for ( i = 0; atmel_tsp_config[i] != RESERVED_T255; i++ ) {
					mxt_defconfig_settings(copy_data, atmel_tsp_config[i]);
				}
				mxt_backup(copy_data);
				msleep(200);
				mxt_reset(copy_data);
			}
            if ((msg[1]&0x10) == 0x10) {/* calibration */
		u8 value1, value2;
                printk("calibration is on going !!\n");

#if 0
		if(touch_v == TOUCH_HS) {
			value1 = 1;
			set_config(data, GEN_ACQUISITIONCONFIG_T8, 2, 1, &value1);

			value1 = 220, value2 = 250;
			set_config(data, PROCI_STYLUS_T47, 1, 1, &value1);
			set_config(data, PROCI_STYLUS_T47, 2, 1, &value2);

			cancel_delayed_work(&data->tune_dwork);
			printk(KERN_INFO " **** schedule tune_dwork 3000ms ****\n");
			schedule_delayed_work(&data->tune_dwork, msecs_to_jiffies(3000));
		}
#endif
	    }
            if ((msg[1]&0x20) == 0x20) /* signal error */
                printk("signal error\n");
            if ((msg[1]&0x40) == 0x40) /* overflow */
                printk("overflow detected\n");
            if ((msg[1]&0x80) == 0x80) /* reset */
                printk("reset is ongoing\n");
        }

        if (object_type == PROCI_TOUCHSUPPRESSION_T42) {
            get_object_info(data, GEN_ACQUISITIONCONFIG_T8,
                    &size, &obj_address);
            if ((msg[1] & 0x01) == 0x00) {
                /* Palm release */
                printk("palm touch released\n");
                touch_is_pressed = 0;

            } else if ((msg[1] & 0x01) == 0x01) {
                /* Palm Press */
                printk("palm touch detected\n");
                touch_is_pressed = 1;
                touch_message_flag = 1;
            }
        }

        if (object_type == PROCG_NOISESUPPRESSION_T48) {
            /* pr_info("T48 [STATUS]:%#02x"
                 "[ADCSPERX]:%#02x[FRQ]:%#02x"
                 "[STATE]:%#02x[NLEVEL]:%#02x\n"
                 , msg[1], msg[2], msg[3], msg[4], msg[5]);*/

            if (msg[4] == 5) { /* Median filter error */
                printk("Median filter error\n");
                if ((data->family_id == 0xA1)
                        && ((data->tsp_version == 0x13)
                            || (data->tsp_version == 0x20))) {
                    if (data->read_ta_status)
                        treat_error_status();
                } else {
                    get_object_info(data,
                            PROCG_NOISESUPPRESSION_T48,
                            &size, &obj_address);
                    value = data->calcfg_batt;
                    error = write_mem(data,
                            obj_address+2, 1, &value);
                    msleep(20);
                    value |= 0x20;
                    error |= write_mem(data,
                            obj_address+2, 1, &value);
                    if (error)
                        printk("failed to"
                                "reenable CHRGON\n");
                }
            }
        }

        if (object_type == PROCG_NOISESUPPRESSION_T62) {
            //printk("T62 : massage data (0x%x, %d, %d, %d, %d, %d, %d)\n", msg[0], msg[1], msg[2], msg[3], msg[4], msg[5], msg[6]);    
            if ((msg[5] > TOUCH_NOISE_LEVEL)&&(msg[5] > TOUCH_NOISE_THRESHOLD)) {
                g_touch_noise_detect = 1;
                printk("T62 Touch Noise Detect, NOISELVL(0x%x) NLTHR(0x%x)\n", msg[5], msg[6]);
            }
            else
                g_touch_noise_detect = 0;
        }

#ifdef FEATURE_TOUCH_ACTIVEPEN
        if ( object_type == PROCI_ACTIVESTYLUS_T63 ) {
printk("[IRQ]T63\n");

            if (!g_touch_noise_detect) {
                data->finger_mask |= 1U << 0;
                report_input_data_t63(data, (u8*)msg);
            }

/*
            if ( g_touch_debug == 3 ) {
//                printk("T63 : massage data (0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x)\n", msg[0], msg[1], msg[2], msg[3], msg[4], msg[5], msg[6]);
            }
#ifdef FEATURE_TOUCH_ACTIVEPEN_TEST
            //id == 0, reserved for active pen
            id = 0;
            finger_cnt++;

            if ( data->finger_mask & (1U << id) ) {
                if (!g_touch_noise_detect) {
                    report_input_data_t63(data, msg);                                
            }

            if ( msg[1] & 0x02 ) { //RELEASE_MSG_MASK
                data->fingers[id].z = 0;
                data->fingers[id].w = 0;
                data->finger_mask |= 1U << id;
                data->fingers[id].state = MXT_STATE_RELEASE;

                if ( g_touch_debug == 3 ) {
//                    printk("MXT_STATE_RELEASE (%d)\n", id);
                }
            } else if ( (msg[1] & ( 0x01| 0x4)) && (msg[2] & 0x10) ) { //PRESS_MSG_MASK, MOVE_MSG_MASK, DETECT_MSG_MASK
#ifdef FEATURE_TOUCH_TOUCH_BOOSTER
                if ( !touch_cpu_lock_status ) {
                    mxt_set_dvfs_on(data);
                }
#endif
//                data->fingers[id].z = msg[6];
                data->fingers[id].z = 0;
                data->fingers[id].w = 3;                                
                data->fingers[id].x = (((msg[4] << 8) & 0xFF00) | (msg[3] & 0x00FF));
                data->fingers[id].y = (((msg[6] << 8) & 0xFF00) | (msg[5] & 0x00FF));
#if 1
                data->fingers[id].x = (u16)((data->fingers[id].x * 1279) / 4096);
                data->fingers[id].y = (u16)((data->fingers[id].y * 799) / 4096);
#endif
                data->finger_mask |= 1U << id;

                if ( msg[1] & 0x01 ) { //PRESS_MSG_MASK
                    data->fingers[id].state = MXT_STATE_PRESS;
                    data->fingers[id].mcount = 0;
                    if ( g_touch_debug == 3 ) {
//                        printk("MXT_STATE_RELEASE (%d)\n", id);
                    }
                } else if ( msg[1] & 0x4 ) { //MOVE_MSG_MASK
                    data->fingers[id].mcount += 1;                                    
                }        

                if ( g_touch_debug == 3) {
//                    printk("fcnt/m(%d,%d),id(%d), pos(%d,%d,)\n", finger_cnt, data->fingers[id].mcount, id, data->fingers[id].x, data->fingers[id].y);
                }
            } else if ( (msg[1] & 0x08) && (data->fingers[id].state != MXT_STATE_INACTIVE) ) { //SUPPRESS_MSG_MASK
                data->fingers[id].z = 0;
                data->fingers[id].w = 0;
                data->fingers[id].state = MXT_STATE_RELEASE;
                data->finger_mask |= 1U << id;
            }
#endif
*/
        }
#endif

        if (object_type == TOUCH_MULTITOUCHSCREEN_T9) {
//printk("[IRQ]T9\n");
#ifdef FEATURE_TOUCH_ACTIVEPEN_TEST
            id = msg[0] - data->finger_type + 1; //id == 0, reserved for active pen
#else
            id = msg[0] - data->finger_type;
#endif
//           printk("[T9] id=%d data->num_fingers=%d \n",id,data->num_fingers);

            finger_cnt++;

            /* If not a touch event, then keep going */
            if (id < 0 || id >= data->num_fingers)
                continue;                        
            if (data->finger_mask & (1U << id)) {
                if(g_touch_debug == 2) printk("T9 report_input_data_t9\n");
                if (!g_touch_noise_detect)
                    report_input_data_t9(data);                                
            }

		if ((msg[1] & DETECT_MSG_MASK) && (msg[1] &
                        (PRESS_MSG_MASK | MOVE_MSG_MASK))) {
#ifdef FEATURE_TOUCH_TOUCH_BOOSTER
                if (!touch_cpu_lock_status)
                    mxt_set_dvfs_on(data);
#endif
                touch_message_flag = 1;
                data->fingers[id].z = msg[6];
                data->fingers[id].w = msg[5];        

#ifdef FEATURE_TOUCH_ACTIVEPEN // Active Pen (4096*4096)
                data->fingers[id].x = (((msg[2] << 4) | (msg[4] >> 4)) >> data->x_dropbits);
                data->fingers[id].y = (((msg[3] << 4) | (msg[4] & 0xF)) >> data->y_dropbits);
                //printk("+++++[ATMEL] source temp_x=%d temp_y=%d\n",data->fingers[id].x,data->fingers[id].y);

#if 1
                data->fingers[id].x = (u16)((data->fingers[id].x * 1279) / 4096);
                data->fingers[id].y = (u16)((data->fingers[id].y * 799) / 1024);
#endif
                //printk("[ATMEL] 1280*800 id[%d] area[%d] temp_x=%d temp_y=%d\n",id, msg[5], data->fingers[id].x,data->fingers[id].y);
#else
                data->fingers[id].x = (((msg[2] << 4) | (msg[4] >> 4)) >> data->x_dropbits);
                data->fingers[id].y = (((msg[3] << 4) | (msg[4] & 0xF)) >> data->y_dropbits);
                //printk("[ATMEL] temp_x=%d temp_y=%d\n",data->fingers[id].x,data->fingers[id].y);
#endif

#ifdef FEATURE_TOUCH_PASSIVEPEN
#ifdef PEN_INTERVAL
		if(msg[5] == 0) { // passive pen
			if(!((data->fingers[id].y < 15 || data->fingers[id].y > 784) ||
						(data->fingers[id].x < 15 || data->fingers[id].x > 1264))) {

				if(pen_mode_timeout == true)
					printk(KERN_INFO "[ATMEL] PEN MODE START(%lu)\n", jiffies);

				pen_mode_timeout = false;
				pen_mode_start = jiffies;
				//		    printk(KERN_INFO "[ATMEL] msg[5]=%d, PEN touch id = %d\n", msg[5], id);
			}
			data->fingers[id].w = 3;
//			printk("[ATMEL] id[%d]: x=%4d,y=%4d (pen)\n",id, data->fingers[id].x,data->fingers[id].y);
		} else { // normal touch
			if(pen_mode_timeout == false) { // pessive pen mode
				if(jiffies < pen_mode_start || jiffies - pen_mode_start > pen_mode_time) { // pen mode timeout
					pen_mode_timeout = true;
					pen_mode_start = 0;
					printk(KERN_INFO "[ATMEL] PEN MODE END(%lu)\n", jiffies);
				} else { // pen mode: ignore normal touch
					data->fingers[id].w = 0;
				}
			}
//			printk("[ATMEL] id[%d]: x=%4d,y=%4d (%s)\n",id, data->fingers[id].x,data->fingers[id].y, data->fingers[id].w ? "finger":"ignore");
		}
#else
	    if(msg[5] == 0)
		    data->fingers[id].w = 3;
#endif
#endif
                data->finger_mask |= 1U << id;

                if (msg[1] & PRESS_MSG_MASK) {
                    data->fingers[id].state =
                        MXT_STATE_PRESS;
                    data->fingers[id].mcount = 0;
                    if(g_touch_debug == 1) printk("MXT_STATE_PRESS (%d) \n", id); 
//                    printk("[ATMEL] id[%d] release\n",id);
                } else if (msg[1] & MOVE_MSG_MASK) {
                    //printk("data->fingers[%d].mcount(%d)\n", id, data->fingers[id].mcount);
                    data->fingers[id].mcount += 1;                                        
                }        

                if(g_touch_debug == 2) printk("fcnt/m(%d,%d),id(%d), pos(%d,%d,)\n", finger_cnt, data->fingers[id].mcount, id, data->fingers[id].x, data->fingers[id].y);

#ifdef _SUPPORT_SHAPE_TOUCH_
                data->fingers[id].component = msg[7];
#endif

            } else if (msg[1] & RELEASE_MSG_MASK) {
                data->fingers[id].z = 0;
                data->fingers[id].w = msg[5];
                data->finger_mask |= 1U << id;
                data->fingers[id].state = MXT_STATE_RELEASE;
                if(g_touch_debug == 1) printk("MXT_STATE_RELEASE (%d) \n", id);
            } else if ((msg[1] & SUPPRESS_MSG_MASK)
                    && (data->fingers[id].state != MXT_STATE_INACTIVE)) {
                data->fingers[id].z = 0;
                data->fingers[id].w = msg[5];
                data->fingers[id].state = MXT_STATE_RELEASE;
                data->finger_mask |= 1U << id;
            } else {
                /* ignore changed amplitude message */
                if (!((msg[1] & DETECT_MSG_MASK)
                            && (msg[1] & AMPLITUDE_MSG_MASK)))
                    pr_err("Unknown state %#02x %#02x\n",
                            msg[0], msg[1]);
                continue;
            }
        }
    } while (!gpio_get_value(data->gpio_read_done));

    finger_cnt = 0;
    //printk("[ATMEL] data->finger_mask=%d \n",data->finger_mask);

    if (data->finger_mask) {
        //if(g_touch_debug == 3) printk("T9 out report_input_data : fm (%d)\n", data->finger_mask);
        if (!g_touch_noise_detect) {
            if (object_type == PROCI_ACTIVESTYLUS_T63) {
                report_input_data_t63(data, (u8*)msg);                
            } else {
                report_input_data_t9(data);
            }
        }                
    }
/*
    if (data->finger_mask) {
        if(g_touch_debug == 2) printk("T9 out report_input_data_t9 : fm (%d)\n", data->finger_mask);
        if (!g_touch_noise_detect)
            report_input_data_t9(data);                
    }
*/
    return IRQ_HANDLED;
}

static int mxt_internal_suspend(struct mxt_data *data)
{
    int i;
    int count = 0;

    for (i = 0; i < data->num_fingers; i++) {
        if (data->fingers[i].state == MXT_STATE_INACTIVE)
            continue;
        data->fingers[i].z = 0;
        data->fingers[i].state = MXT_STATE_RELEASE;
        count++;
    }
    if (count)
        report_input_data_t9(data);

#ifdef FEATURE_TOUCH_TOUCH_BOOSTER
    cancel_delayed_work(&data->dvfs_dwork);
    tsp_press_status = 0;
    if (touch_cpu_lock_status) {
        exynos_cpufreq_lock_free(DVFS_LOCK_ID_TSP);
        touch_cpu_lock_status = 0;
    }
#endif

#ifdef FEATURE_TOUCH_NOISE
    g_touch_suspend = 1;
#endif        
#ifdef FEATURE_TOUCH_PASSIVEPEN
    pen_mode_timeout = true;
    pen_mode_start = 0;
#endif
    data->power_off();

    return 0;
}

static int mxt_internal_resume(struct mxt_data *data)
{
    data->power_on();

#ifdef FEATURE_TOUCH_NOISE        
    if(g_touch_debug == 1) printk("mxt_internal_resume : g_power_noise (%d)", g_power_noise);

#ifdef FEATURE_SUPPORT_ZIG_TUNING
    if ( !g_is_tuning_mode ) {
#endif
        mxt_PROCI_STYLUS_T47(copy_data);	// T47 : passive stylus pen
        mxt_PROCG_NOISESUPPRESSION_T62(copy_data);
        mxt_PROCI_ACTIVESTYLUS_T63(copy_data);
#ifdef FEATURE_SUPPORT_ZIG_TUNING
    } /* if ( !g_is_tuning_mode ) */
#endif

    g_touch_suspend = 0;
#endif
    return 0;
}





#ifdef CONFIG_HAS_EARLYSUSPEND
#define mxt_suspend     NULL
#define mxt_resume      NULL

static void mxt_early_suspend(struct early_suspend *h)
{
    struct mxt_data *data = container_of(h, struct mxt_data,
            early_suspend);
    printk("mxt_early_suspend  mxt_enabled=%d \n", mxt_enabled);
    if (mxt_enabled == 1) {
        pr_info("%s\n", __func__);
        mxt_enabled = 0;
        touch_is_pressed = 0;
        disable_irq(IRQ_EINT(4));
    	cancel_delayed_work(&data->tune_dwork);
        mxt_internal_suspend(data);
    } else
        pr_err("%s. but touch already off\n", __func__);
}

static void mxt_late_resume(struct early_suspend *h)
{
    bool ta_status = 0;
    struct mxt_data *data = container_of(h, struct mxt_data,
            early_suspend);
    printk("mxt_late_resume  mxt_enabled=%d \n", mxt_enabled);
    if (mxt_enabled == 0) {
        pr_info("%s\n", __func__);
        mxt_internal_resume(data);

        mxt_enabled = 1;

        if (data->read_ta_status) {
            data->read_ta_status(&ta_status);
            pr_info("ta_status is %d\n", ta_status);
        }

        treat_median_error_status = 0;
        calibrate_chip_e();

        enable_irq(IRQ_EINT(4));
    } else
        pr_err("%s. but touch already on\n", __func__);
}
#else
static int mxt_suspend(struct device *dev)
{
    struct i2c_client *client = to_i2c_client(dev);
    struct mxt_data *data = i2c_get_clientdata(client);

    mxt_enabled = 0;
    touch_is_pressed = 0;
#ifdef FEATURE_TOUCH_TOUCH_BOOSTER
    tsp_press_status = 0;
#endif
    return mxt_internal_suspend(data);
}

static int mxt_resume(struct device *dev)
{
    int ret = 0;
    bool ta_status = 0;
    struct i2c_client *client = to_i2c_client(dev);
    struct mxt_data *data = i2c_get_clientdata(client);

    ret = mxt_internal_resume(data);

    mxt_enabled = 1;

    if (data->read_ta_status) {
        data->read_ta_status(&ta_status);
        pr_info("ta_status is %d\n", ta_status);
    }
    return ret;
}
#endif





void Mxt_force_released(void)
{
    struct mxt_data *data = copy_data;
    int i;

    if (!mxt_enabled) {
        pr_err("mxt_enabled is 0\n");
        return;
    }

    for (i = 0; i < data->num_fingers; i++) {
        if (data->fingers[i].state == MXT_STATE_INACTIVE)
            continue;
        data->fingers[i].z = 0;
        data->fingers[i].state = MXT_STATE_RELEASE;
    }
    report_input_data_t9(data);

    calibrate_chip_e();
};

static int mxt_check_bootloader(struct i2c_client *client,
        unsigned int state)
{
    u8 val;
    u8 temp;

recheck:
    if (i2c_master_recv(client, &val, 1) != 1)
        return -EIO;

    if (val & 0x20) {
        if (i2c_master_recv(client, &temp, 1) != 1)
            return -EIO;

        if (i2c_master_recv(client, &temp, 1) != 1)
            return -EIO;
        val &= ~0x20;
    }

    if ((val & 0xF0) == MXT_APP_CRC_FAIL) {
        pr_info("MXT_APP_CRC_FAIL\n");
        if (i2c_master_recv(client, &val, 1) != 1)
            return -EIO;

        if (val & 0x20) {
            if (i2c_master_recv(client, &temp, 1) != 1)
                return -EIO;

            if (i2c_master_recv(client, &temp, 1) != 1)
                return -EIO;
            val &= ~0x20;
        }
    }

    switch (state) {
        case MXT_WAITING_BOOTLOAD_CMD:
        case MXT_WAITING_FRAME_DATA:
            val &= ~MXT_BOOT_STATUS_MASK;
            break;
        case MXT_FRAME_CRC_PASS:
            if (val == MXT_FRAME_CRC_CHECK)
                goto recheck;
            break;
        default:
            return -EINVAL;
    }

    if (val != state) {
        pr_err("Unvalid bootloader mode state\n");
        return -EINVAL;
    }

    return 0;
}

static int mxt_unlock_bootloader(struct i2c_client *client)
{
    u8 buf[2];

    buf[0] = MXT_UNLOCK_CMD_LSB;
    buf[1] = MXT_UNLOCK_CMD_MSB;

    if (i2c_master_send(client, buf, 2) != 2) {
        pr_err("%s: i2c send failed\n",
                __func__);

        return -EIO;
    }

    return 0;
}

static int mxt_fw_write(struct i2c_client *client,
        const u8 *data, unsigned int frame_size)
{
    if (i2c_master_send(client, data, frame_size) != frame_size) {
        pr_err("%s: i2c send failed\n", __func__);
        return -EIO;
    }

    return 0;
}

static int mxt_load_fw(struct device *dev, const char *fn)
{
    struct mxt_data *data = copy_data;
    struct i2c_client *client = copy_data->client;
    unsigned int frame_size;
    unsigned int pos = 0;
    int ret;
    u16 obj_address = 0;
    u16 size_one;
    u8 value;
    unsigned int object_register;
    int check_frame_crc_error = 0;
    int check_wating_frame_data_error = 0;

#ifdef FEATURE_SUPPORT_FW_UPDATE_FROM_HEADER
    struct firmware *fw = NULL;

    pr_info("mxt_load_fw start from header!!!\n");
    fw = kzalloc(sizeof(struct firmware), GFP_KERNEL);

    fw->data = firmware_mXT;
    fw->size = sizeof(firmware_mXT);
#else
    const struct firmware *fw = NULL;

    pr_info("mxt_load_fw startl!!!\n");
    ret = request_firmware(&fw, fn, &client->dev);
    if (ret) {
        pr_err("Unable to open firmware %s\n", fn);
        return ret;
    }
#endif
    /* Change to the bootloader mode */
    object_register = 0;
    value = (u8)MXT_BOOT_VALUE;
    ret = get_object_info(data,
            GEN_COMMANDPROCESSOR_T6, &size_one, &obj_address);
    if (ret) {
        pr_err("fail to get object_info\n");
        release_firmware(fw);
        return ret;
    }
    size_one = 1;
    write_mem(data, obj_address+(u16)object_register, (u8)size_one, &value);
    msleep(MXT_SW_RESET_TIME);

    /* Change to slave address of bootloader */
    if (client->addr == MXT_APP_LOW)
        client->addr = MXT_BOOT_LOW;//JSK
    else
        client->addr = MXT_BOOT_HIGH;

    ret = mxt_check_bootloader(client, MXT_WAITING_BOOTLOAD_CMD);
    if (ret)
        goto out;

    /* Unlock bootloader */
    mxt_unlock_bootloader(client);

    while (pos < fw->size) {
        ret = mxt_check_bootloader(client, MXT_WAITING_FRAME_DATA);
        if (ret) {
            check_wating_frame_data_error++;
            if (check_wating_frame_data_error > 10) {
                pr_err("firm update fail. wating_frame_data err\n");
#ifdef FEATURE_TOUCH_WIFI_TOOL
                data->state = BOOTLOADER;
#endif
                goto out;
            } else {
                pr_err("check_wating_frame_data_error = %d, retry\n",
                        check_wating_frame_data_error);
                continue;
            }
        }

        frame_size = ((*(fw->data + pos) << 8) | *(fw->data + pos + 1));

        /* We should add 2 at frame size as the the firmware data is not
         * included the CRC bytes.
         */
        frame_size += 2;

        /* Write one frame to device */
        mxt_fw_write(client, fw->data + pos, frame_size);

        ret = mxt_check_bootloader(client, MXT_FRAME_CRC_PASS);
        if (ret) {
            check_frame_crc_error++;
            if (check_frame_crc_error > 10) {
                pr_err("firm update fail. frame_crc err\n");
                goto out;
            } else {
                pr_err("check_frame_crc_error = %d, retry\n",
                        check_frame_crc_error);
                continue;
            }
        }

        pos += frame_size;

        pr_info("Updated %d bytes / %zd bytes\n",
                pos, fw->size);

        msleep(20);
    }

#ifdef FEATURE_TOUCH_WIFI_TOOL
    data->state = APPMODE;
#endif

out:
#ifdef FEATURE_SUPPORT_FW_UPDATE_FROM_HEADER
    kfree(fw);
#else
    release_firmware(fw);
#endif
    /* Change to slave address of application */
    if (client->addr == MXT_BOOT_LOW)
        client->addr = MXT_APP_LOW;
    else
        client->addr = MXT_APP_HIGH;
    return ret;
}

static int mxt_load_fw_bootmode(struct device *dev, const char *fn)
{
    struct i2c_client *client = copy_data->client;
    unsigned int frame_size;
    unsigned int pos = 0;
    int ret;

    int check_frame_crc_error = 0;
    int check_wating_frame_data_error = 0;

#ifdef FEATURE_SUPPORT_FW_UPDATE_FROM_HEADER
    struct firmware *fw = NULL;
    pr_info("mxt_load_fw start from header!!!\n");
    fw = kzalloc(sizeof(struct firmware), GFP_KERNEL);

    fw->data = firmware_mXT;
    fw->size = sizeof(firmware_mXT);
#else
    const struct firmware *fw = NULL;
    pr_info("mxt_load_fw start!!!\n");

    ret = request_firmware(&fw, fn, &client->dev);
    if (ret) {
        pr_err("Unable to open firmware %s\n", fn);
        return ret;
    }
#endif
    /* Unlock bootloader */
    mxt_unlock_bootloader(client);

    while (pos < fw->size) {
        ret = mxt_check_bootloader(client,
                MXT_WAITING_FRAME_DATA);
        if (ret) {
            check_wating_frame_data_error++;
            if (check_wating_frame_data_error > 10) {
                pr_err("firm update fail. wating_frame_data err\n");
                goto out;
            } else {
                pr_err("check_wating_frame_data_error = %d, retry\n",
                        check_wating_frame_data_error);
                continue;
            }
        }

        frame_size = ((*(fw->data + pos) << 8) | *(fw->data + pos + 1));

        /* We should add 2 at frame size as the the firmware data is not
         * included the CRC bytes.
         */
        frame_size += 2;

        /* Write one frame to device */
        mxt_fw_write(client, fw->data + pos, frame_size);

        ret = mxt_check_bootloader(client,
                MXT_FRAME_CRC_PASS);
        if (ret) {
            check_frame_crc_error++;
            if (check_frame_crc_error > 10) {
                pr_err("firm update fail. frame_crc err\n");
                goto out;
            } else {
                pr_err("check_frame_crc_error = %d, retry\n",
                        check_frame_crc_error);
                continue;
            }
        }

        pos += frame_size;

        pr_info("Updated %d bytes / %zd bytes\n",
                pos, fw->size);

        msleep(20);
    }

out:
#ifdef FEATURE_SUPPORT_FW_UPDATE_FROM_HEADER
    kfree(fw);
#else
    release_firmware(fw);
#endif
    /* Change to slave address of application */
    if (client->addr == MXT_BOOT_LOW)
        client->addr = MXT_APP_LOW;
    else
        client->addr = MXT_APP_HIGH;
    return ret;
}





#ifdef FEATURE_TOUCH_NOISE
void atm1664_power_noise(bool power_detect)
{    
    if (power_detect)
        g_power_noise = 1;
    else
        g_power_noise = 0;

    if(g_touch_debug == 1) printk("atm1664_power_noise (%d)\n", g_power_noise);

    if (!g_touch_suspend) {
#ifdef FEATURE_SUPPORT_ZIG_TUNING
        if ( !g_is_tuning_mode ) {
#endif
            mxt_PROCI_STYLUS_T47(copy_data);
            mxt_PROCG_NOISESUPPRESSION_T62(copy_data);    
#ifdef FEATURE_SUPPORT_ZIG_TUNING
        } /* if ( !g_is_tuning_mode ) */
#endif
    }
}
EXPORT_SYMBOL(atm1664_power_noise);
#endif

#ifdef FEATURE_TOUCH_PASSIVEPEN
static ssize_t atm1664_passivepen_show(struct device *dev, struct device_attribute *attr, char *buf)
{
    return sprintf(buf, " g_passivepen_mode=[%d]\n",  g_passivepen_mode);
}

static ssize_t atm1664_passivepen_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
    static int tmp = 0;

    tmp = buf[0]-'0';    

    if ( tmp ) {
        g_passivepen_mode = 1;
    } else {
        g_passivepen_mode = 0;
    }

    printk("PassivePen[%s] g_passivepen_mode=[%d]\n", g_passivepen_mode ? "Enable" : "Disable", g_passivepen_mode);

    mxt_PROCI_STYLUS_T47(copy_data);
    mxt_PROCG_NOISESUPPRESSION_T62(copy_data);    

    return strlen(buf);
}
static DEVICE_ATTR(tsp_passivepen, S_IRUGO|S_IWUSR|S_IWGRP, atm1664_passivepen_show, atm1664_passivepen_store);
static ssize_t atm1664_penmode_time_show(struct device *dev, struct device_attribute *attr, char *buf)
{
    return sprintf(buf, " pen_mode_time =[%u]\n",  jiffies_to_msecs(pen_mode_time));
}

static ssize_t atm1664_penmode_time_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
	unsigned long time = 0;
	if(buf) {
		sscanf(buf, "%lu", &time);
		if(time > 1000) {
			printk("invalid value\n");
		} else {
			pen_mode_time = msecs_to_jiffies(time);
			printk("pen_mode_time =[%u]\n", jiffies_to_msecs(pen_mode_time));
		}
	} else {
		printk("invalid value\n");
	}

	return strlen(buf);
}
static DEVICE_ATTR(tsp_penmode_time, S_IRUGO|S_IWUSR|S_IWGRP, atm1664_penmode_time_show, atm1664_penmode_time_store);
#endif

#ifdef FEATURE_TOUCH_ACTIVEPEN
static ssize_t atm1664_activepen_show(struct device *dev, struct device_attribute *attr, char *buf)
{
    return sprintf(buf, "g_activepen_mode=[%d]\n",  g_activepen_mode);
}

static ssize_t atm1664_activepen_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
    static int tmp = 0;

    tmp = buf[0]-'0';        

    if ( tmp ) {
        g_activepen_mode = 1;
    } else {
        g_activepen_mode = 0;
    }

    printk("ActivePen[%s] g_activepen_mode=[%d]\n", g_activepen_mode ? "Enable" : "Disable", g_activepen_mode);

    mxt_PROCI_ACTIVESTYLUS_T63(copy_data);

    disable_irq(IRQ_EINT(4));
    copy_data->power_off();
    mdelay(100);
    copy_data->power_on();
    enable_irq(IRQ_EINT(4));

    return strlen(buf);
}
static DEVICE_ATTR(tsp_pen, S_IRUGO|S_IWUSR|S_IWGRP, atm1664_activepen_show, atm1664_activepen_store);
#endif

#ifdef FEATURE_TOUCH_CONFIG_UPDATE 
static int tsp_GetConfigSize(char * firmware_name)
{
    struct file* pfile = NULL;
    struct inode *inode;
    unsigned long magic; 
    off_t fsize = 0; 
    char filepath[128];

    memset(filepath, 0, sizeof(filepath));

    sprintf(filepath, "/mnt/ext_sd/tsp_bin/%s", firmware_name);
    printk("[%s()] filepath=[%s]\n", __FUNCTION__, filepath);

    if ( NULL == pfile ) {
        pfile = filp_open(filepath, O_RDONLY, 0);
    }

    if ( IS_ERR(pfile) ) {
        printk("[%s()] error occured while opening file %s.\n", __FUNCTION__, filepath);
        return -1;
    }

    inode = pfile->f_dentry->d_inode; 
    magic = inode->i_sb->s_magic;
    fsize = inode->i_size; 

    filp_close(pfile, NULL);

    return fsize;
}

static int tsp_ReadConfig(char * firmware_name, unsigned char * firmware_buf)
{
    struct file* pfile = NULL;
    struct inode *inode;
    unsigned long magic; 
    off_t fsize; 
    char filepath[128];
    loff_t pos;
    mm_segment_t old_fs;

    memset(filepath, 0, sizeof(filepath));

    sprintf(filepath, "/mnt/ext_sd/tsp_bin/%s", firmware_name);
    printk("[%s()] filepath=[%s]\n", __FUNCTION__, filepath);

    if ( NULL == pfile ) {
        pfile = filp_open(filepath, O_RDONLY, 0);
    }

    if ( IS_ERR(pfile) ) {
        printk("[%s()] error occured while opening file %s.\n", __FUNCTION__, filepath);
        return -1;
    }

    inode=pfile->f_dentry->d_inode; 
    magic=inode->i_sb->s_magic;
    fsize=inode->i_size; 

    old_fs = get_fs();
    set_fs(KERNEL_DS);
    pos = 0;

    vfs_read(pfile, firmware_buf, fsize, &pos);

    filp_close(pfile, NULL);
    set_fs(old_fs);

    return 0;
}

int mxt_load_config_file(FTS_BYTE *file_buf)
{
    int c;

    char object[255];
    char tmp[255];
    int object_id;
    int instance;
    int object_address;
    int object_size;
    int data;
    int file_read = 0;
    bool ignore_line = false;

    int i;
    int bytes_read;
    int p = 0;
#ifdef FEATURE_SUPPORT_FW_UPDATE_OUTPUT_CODE
    int cr=1;
#endif

    while (!file_read) {
        /* First character is expected to be '[' - skip empty lines and spaces  */
        c = file_buf[p++];
        while ((c == '\n') || (c == '\r') || (c == 0x20)) {
            c = file_buf[p++];
        }

        if (c != '[') {
            if (c == -1/*EOF*/)
                break;

            /* If we are ignoring the current section then look for the next section */
            if (ignore_line) {
                continue;
            }

            printk("Parse error: expected '[', read ascii char %c!", file_buf[p]);
            return -1;
        }

        if (sscanf(&file_buf[p], "%s", object) != 1) {
            printk("Parse error\n");
            return -1;
        } else {
#ifdef FEATURE_SUPPORT_FW_UPDATE_OUTPUT_DEBUG
            printk("mov p +%d\n", strlen(object));
#endif
            p += strlen(object);
        }

        /* Ignore the comments and file header sections */
        if (strncmp(object, "COMMENTS", 8) == 0
                || strncmp(object, "VERSION_INFO_HEADER", 19) == 0
                || strncmp(object, "APPLICATION_INFO_HEADER", 23) == 0) {

            ignore_line = true;
#ifdef FEATURE_SUPPORT_FW_UPDATE_OUTPUT_DEBUG
            printk("Skipping %s\n", object);
#endif
            continue;
        } else {
#ifdef FEATURE_SUPPORT_FW_UPDATE_OUTPUT_DEBUG
            printk("object <%s>\n", object); //dbg
#endif
        }

        ignore_line = false;
        if (sscanf(&file_buf[p], "%s", tmp) != 1) {
            printk("Parse error\n");
            return -1;
        } else {
#ifdef FEATURE_SUPPORT_FW_UPDATE_OUTPUT_DEBUG
            printk("mov p +%d\n", strlen(tmp));
#endif
            p += strlen(tmp); //INSTANC'E' 0]
            p++; //INSTANCE' '0]
        }
#ifdef FEATURE_SUPPORT_FW_UPDATE_OUTPUT_DEBUG
        printk("tmp %s\n", tmp);//dbg
#endif

        if (strcmp(tmp, "INSTANCE")) {
            printk("Parse error, expected INSTANCE\n");
            return(-1);
        }

        if (sscanf(&file_buf[p], " %d", &instance) != 1) {
            printk("Parse error\n");
            return -1;
        } else {
#ifdef FEATURE_SUPPORT_FW_UPDATE_OUTPUT_DEBUG
            printk("instance <%d>\n", instance);//dbg
#endif
        }

        c = file_buf[p++];
        while(c != ']') {
            c = file_buf[p++];
        }
        p -= 1;
        // INSTANCE 0']'

        /* Last character is expected to be ']' */
        c = file_buf[p++];
        if (c != ']') {
            printk("Parse error, expected ]\n");
            return(-1);
        }

        while(c != '\n') {
            c = file_buf[p++];
        }

        while ((c != '=') && (c != -1/*EOF*/)) {
            c = file_buf[p++];
        }

        if (sscanf(&file_buf[p], "%d", &object_address) != 1) {
            printk("Parse error\n");
            return -1;
        } else {
#ifdef FEATURE_SUPPORT_FW_UPDATE_OUTPUT_DEBUG
            printk("object_address <%d>\n", object_address);
#endif
        }

        c = file_buf[p++];

        while((c != '=') && (c != -1/*EOF*/)) {
            c = file_buf[p++];
        }

        if (sscanf(&file_buf[p], "%d", &object_size) != 1) {
            printk("Parse error\n");
            return -1;
        } else {
#ifdef FEATURE_SUPPORT_FW_UPDATE_OUTPUT_DEBUG
            printk("object_size <%d>\n", object_size);
#endif
        }

        c = file_buf[p++];
        while(c != '\n') {
            c = file_buf[p++];
        }
        // '0' 1 MODE=0 // 1'st DATA line

        /* Get object type ID number at end of object string (1 or 2 digits) */
        if (sscanf(object + strlen(object) - 2, "%d", &object_id) != 1) {
            if (sscanf(object + strlen(object) - 1, "%d", &object_id) != 1) {
                printk("Unable to get object type ID for %s\n", object);
                return -1;
            }
        }
#ifdef FEATURE_SUPPORT_FW_UPDATE_OUTPUT_DEBUG
        printk("object_id <%d>\n", object_id);
#endif

#ifdef FEATURE_SUPPORT_FW_UPDATE_OUTPUT_DEBUG
        printk("Writing object of size %d at address %d...\n", object_size, object_address);
#endif
#ifdef FEATURE_SUPPORT_FW_UPDATE_OUTPUT_CODE
        if ( 0 == instance ) {
            printk("\t\tu8 t%d_config_data[] = {", object_id);
        }
        cr=1;
#endif

        bytes_read = 0;
        while (bytes_read < object_size) {
            /* Find next line, check first character valid and rewind */
            c = file_buf[p++];
            while ((c == '\n') || (c == '\r') || (c == 0x20)) {
                c = file_buf[p++];
            }

            p--;

            if (c == '[') {
                printk("Skipping %d bytes at end of T%u\n", object_size - bytes_read, object_id);
                break;
            }

            /* Read address (discarded as we don't really need it) */
            if (sscanf(&file_buf[p], "%d", &i) != 1) {
                printk("Parse error\n");
                return -1;
            } else {
#ifdef FEATURE_SUPPORT_FW_UPDATE_OUTPUT_DEBUG
                printk("address <%d>\n", i);
#endif
            }

            c = file_buf[p++];
            while (c != 0x20/*space*/) {
                c = file_buf[p++];
            }
            //0 '1' MODE=0

            /* Read byte count of this register (max 2) */
            if (sscanf(&file_buf[p], "%d", &i) != 1)            {
                printk("Parse error\n");
                return -1;
            } else {
#ifdef FEATURE_SUPPORT_FW_UPDATE_OUTPUT_DEBUG
                printk("byte <%d>\n", i);
#endif
            }

            c = file_buf[p++];
            while(c != 0x20/*space*/) {
                c = file_buf[p++];
            }
            //0 1 'M'ODE=0

            c = file_buf[p++];
            while((c != '=') && (c != -1/*EOF*/)) {
                c =  file_buf[p++];
            }
            //0 1 MODE='0'

            if (sscanf(&file_buf[p], "%d", &data) != 1) {
                printk("Parse error\n");
                return -1;
            } else {
#ifdef FEATURE_SUPPORT_FW_UPDATE_OUTPUT_DEBUG
                printk("data <%d>\n", data);
#endif
#ifdef FEATURE_SUPPORT_FW_UPDATE_OUTPUT_CODE
                if ( 0 == instance ) {
                    printk("%ld, ", data >= 0 ? data : (~abs(data) & 0xFF) + 1);
                    if (cr++ == 10) { printk("  "); cr = 1; }
                }
#endif
            }
            c = file_buf[p++];
            while(c != '\n') {
                c = file_buf[p++];
            }
            // '1' 1 PAGE=0 // 2'nd DATA line

            if (i == 1) {
                bytes_read++;
            } else if (i == 2) {
                bytes_read++;
                bytes_read++;
            } else {
                printk("Only 16-bit / 8-bit config values supported!\n");
                return -1;
            }
        }

#ifdef FEATURE_SUPPORT_FW_UPDATE_OUTPUT_CODE
        if ( 0 == instance ) {
            if (cr==1) { printk("\b\b"); }  printk("\b\b};\n");
        }
#endif
    }

    return 0;
}

static ssize_t atm1664_config_show(struct device *dev, struct device_attribute *attr, char *buf)
{
    int config_value = mxt_SPT_USERDATA_T38_read(copy_data);

    return sprintf(buf, "Touch Config Value=[0x%02X(%d)]\n", config_value, config_value);
}

static ssize_t atm1664_config_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
    FTS_BYTE* pbt_buf = FTS_NULL;
    int fwsize = tsp_GetConfigSize(FIRMWARE_NAME);

    if ( buf[0] != 'c' ) {
        printk("[%s()] ERROR: unknown command\n", __FUNCTION__);
        return strlen(buf);
    }

    printk("[%s()] fwsize=[%d] firmware_name=[%s] \n", __FUNCTION__, fwsize, FIRMWARE_NAME);

    if ( fwsize <= 0 ) {
        printk("[%s()] ERROR: Get firmware size failed\n", __FUNCTION__);
        return -1;
    }

    pbt_buf = (unsigned char *) kmalloc(fwsize+1,GFP_ATOMIC);

    if ( tsp_ReadConfig(FIRMWARE_NAME, pbt_buf) ) {
        printk("[%s()] ERROR: tsp_ReadConfig failed\n", __FUNCTION__);
        kfree(pbt_buf);
        return -1;
    }    

    if ( !strncmp("MXT1665S_CONFIG_INFO", pbt_buf, 20) ) {
        printk("[%s()] ERROR: THIS CONFIG DO NOT SUPPORT !!! \n", __FUNCTION__);
        kfree(pbt_buf);
        return -1;
    } else if ( !strncmp("[COMMENTS]", pbt_buf, 10) ) {
        printk("[%s()] XCFG LOADING ...\n", __FUNCTION__);
        mxt_load_config_file(pbt_buf);
    }

    kfree(pbt_buf);
    return 1; //JSK

    mxt_backup(copy_data);
    msleep(200);

    mxt_reset(copy_data);
    msleep(MXT_SW_RESET_TIME);

    calibrate_chip_e();

    disable_irq(IRQ_EINT(4));
    copy_data->power_off();
    msleep(100);
    copy_data->power_on();
    enable_irq(IRQ_EINT(4));

    return strlen(buf);
}
static DEVICE_ATTR(tsp_config, S_IRUGO|S_IWUSR|S_IWGRP, atm1664_config_show, atm1664_config_store);
#endif


#ifdef FEATURE_TOUCH_DEBUG
static ssize_t atm1664_debug_show(struct device *dev, struct device_attribute *attr, char *buf)
{
#ifdef FEATURE_TOUCH_TOUCH_BOOSTER
    return sprintf(buf, " touch_dvfs_cpufreq=[%d]KHz \n g_touch_debug=[%d]\n",  touch_dvfs_cpufreq, g_touch_debug);
#else
    return sprintf(buf, " g_touch_debug=[%d]\n",  g_touch_debug);
#endif
}

static ssize_t atm1664_debug_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
    static int tmp = 0;

    tmp = buf[0]-'0';    
    g_touch_debug = tmp;

    printk(" g_touch_debug=[%d]\n", g_touch_debug);

#ifdef FEATURE_TOUCH_TOUCH_BOOSTER    
    if (g_touch_debug == 7) touch_dvfs_cpufreq = (g_touch_debug-1) * 100000;
    if (g_touch_debug == 8) touch_dvfs_cpufreq = g_touch_debug * 100000;
    if (g_touch_debug == 9) touch_dvfs_cpufreq = (g_touch_debug+1) * 100000;
    printk(" touch_dvfs_cpufreq=[%d]KHz\n", touch_dvfs_cpufreq);                
#endif

    return strlen(buf);
}

static DEVICE_ATTR(tsp_debug, S_IRUGO|S_IWUSR|S_IWGRP, atm1664_debug_show, atm1664_debug_store);
#endif

#ifdef FEATURE_TOUCH_WIFI_TOOL // for Wi-Fi tool
static ssize_t mxt_debug_enable_show(struct device *dev, struct device_attribute *attr, char *buf)
{
    struct i2c_client *client = to_i2c_client(dev);
    struct mxt_data *data = i2c_get_clientdata(client);

    int count;
    char c;

    c = data->debug_enabled ? '1' : '0';
    count = sprintf(buf, "%c\n", c);

    return count;
}

static ssize_t mxt_debug_enable_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
    struct i2c_client *client = to_i2c_client(dev);
    struct mxt_data *data = i2c_get_clientdata(client);
    int i;

    if (sscanf(buf, "%u", &i) == 1 && i < 2) {
        data->debug_enabled = (i == 1);
        dev_dbg(dev, "%s\n", i ? "debug enabled" : "debug disabled");
        return count;
    } else {
        dev_dbg(dev, "debug_enabled write error\n");
        return -EINVAL;
    }
}

static int mxt_check_mem_access_params(struct mxt_data *data, loff_t off,
				       size_t *count)
{
	if (data->state != APPMODE) {
		dev_err(&data->client->dev, "Not in APPMODE\n");
		return -EINVAL;
	}

	if (off >= data->mem_size)
		return -EIO;

	if (off + *count > data->mem_size)
		*count = data->mem_size - off;

	if (*count > MXT_MAX_BLOCK_WRITE)
		*count = MXT_MAX_BLOCK_WRITE;

	return 0;
}

static int __mxt_read_reg(struct i2c_client *client,
                               u16 reg, u16 len, void *val)
{
	struct i2c_msg xfer[2];
	u8 buf[2];
	int ret;
	bool retry = false;

	buf[0] = reg & 0xff;
	buf[1] = (reg >> 8) & 0xff;

	/* Write register */
	xfer[0].addr = client->addr;
	xfer[0].flags = 0;
	xfer[0].len = 2;
	xfer[0].buf = buf;

	/* Read data */
	xfer[1].addr = client->addr;
	xfer[1].flags = I2C_M_RD;
	xfer[1].len = len;
	xfer[1].buf = val;

retry_read:
	ret = i2c_transfer(client->adapter, xfer, ARRAY_SIZE(xfer));
	if (ret != ARRAY_SIZE(xfer)) {
		if (!retry) {
			dev_dbg(&client->dev, "%s: i2c retry\n", __func__);
			msleep(MXT_WAKEUP_TIME);
			retry = true;
			goto retry_read;
		} else {
			dev_err(&client->dev, "%s: i2c transfer failed (%d)\n",
				__func__, ret);
			return -EIO;
		}
	}

	return 0;
}

static int __mxt_write_reg(struct i2c_client *client, u16 reg, u16 len,
			   const void *val)
{
	u8 *buf;
	size_t count;
	int ret;
	bool retry = false;

	count = len + 2;
	buf = kmalloc(count, GFP_KERNEL);
	if (!buf)
		return -ENOMEM;

	buf[0] = reg & 0xff;
	buf[1] = (reg >> 8) & 0xff;
	memcpy(&buf[2], val, len);

retry_write:
	ret = i2c_master_send(client, buf, count);
	if (ret == count) {
		ret = 0;
	} else {
		if (!retry) {
			dev_dbg(&client->dev, "%s: i2c retry\n", __func__);
			msleep(MXT_WAKEUP_TIME);
			retry = true;
			goto retry_write;
		} else {
			dev_err(&client->dev, "%s: i2c send failed (%d)\n",
				__func__, ret);
			ret = -EIO;
		}
	}

	kfree(buf);
	return ret;
}

static ssize_t mxt_mem_access_read(struct file *filp, struct kobject *kobj,
	struct bin_attribute *bin_attr, char *buf, loff_t off, size_t count)
{
	struct device *dev = container_of(kobj, struct device, kobj);
	struct mxt_data *data = dev_get_drvdata(dev);
	int ret = 0;

	ret = mxt_check_mem_access_params(data, off, &count);
	if (ret < 0)
		return ret;

	if (count > 0)
		ret = __mxt_read_reg(data->client, off, count, buf);

	return ret == 0 ? count : ret;
}

static ssize_t mxt_mem_access_write(struct file *filp, struct kobject *kobj,
	struct bin_attribute *bin_attr, char *buf, loff_t off,
	size_t count)
{
	struct device *dev = container_of(kobj, struct device, kobj);
	struct mxt_data *data = dev_get_drvdata(dev);
	int ret = 0;

	ret = mxt_check_mem_access_params(data, off, &count);
	if (ret < 0)
		return ret;

	if (count > 0)
		ret = __mxt_write_reg(data->client, off, count, buf);

	return ret == 0 ? count : 0;
}

static DEVICE_ATTR(debug_enable, S_IWUSR | S_IRUSR/*S_IRUGO|S_IWUSR|S_IWGRP*/, mxt_debug_enable_show, mxt_debug_enable_store);
#endif // FEATURE_TOUCH_WIFI_TOOL

//#define TOUCH_TUNE
#ifdef TOUCH_TUNE
static ssize_t atm1664_setconfig_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	return sprintf(buf, "set config, not get config\n");
}

static ssize_t atm1664_setconfig_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
	int obj_type = 0;
	int idx = 0;
	int size = 0;
	int value = 0;

	if(buf) {
		sscanf(buf, "%d:%d:%d:%d", &obj_type, &idx, &size, &value);
		if(obj_type == 0 && idx == 0 && size == 0 && value == 0) {
			printk(KERN_INFO "[ATMEL] save config (mxt_backup()->mxt_reset()->calibrate_chip_e())\n");
			msleep(50);
			mxt_backup(copy_data);
			msleep(200);
			mxt_reset(copy_data);
			msleep(MXT_SW_RESET_TIME);

			calibrate_chip_e();
		} else {
			printk("set_config(%s, %d, %d, %d)\n", object_type_name[obj_type], idx, size, value);
			set_config(copy_data, obj_type, idx, size, &value);
		}

	} else {
		printk("usage: echo type:idx:size:value > setconfig\n");
	}

	return strlen(buf);
}
static DEVICE_ATTR(tsp_setconfig, S_IALLUGO|S_IRUGO|S_IWUSR|S_IWGRP, atm1664_setconfig_show, atm1664_setconfig_store);
#endif

static struct attribute *atm1664_attributes[] = {     
#ifdef FEATURE_TOUCH_PASSIVEPEN    
    &dev_attr_tsp_passivepen.attr,  
    &dev_attr_tsp_penmode_time.attr,
#endif
#ifdef FEATURE_TOUCH_ACTIVEPEN    
    &dev_attr_tsp_pen.attr,  
#endif    
#ifdef FEATURE_TOUCH_CONFIG_UPDATE
    &dev_attr_tsp_config.attr,      
#endif    
#ifdef FEATURE_TOUCH_DEBUG
    &dev_attr_tsp_debug.attr,
#endif
#ifdef FEATURE_TOUCH_WIFI_TOOL
    &dev_attr_tsp_debug_enable.attr,
#endif
#ifdef TOUCH_TUNE
    &dev_attr_tsp_setconfig.attr,
#endif
    NULL
};

static const struct attribute_group atm1664_attr_group = {
    .attrs = atm1664_attributes,
};

static int __init init_tsp_vendor(char *str)
{
	int retval;
	int tsp_vendor;

	retval = get_option(&str, &tsp_vendor);
	if(retval == 1) {
		if(tsp_vendor == 0) {
			touch_v = TOUCH_LG;
		} else if(tsp_vendor == 1) {
			touch_v = TOUCH_HS;
		}
	}

	return 1;
}

__setup("tsp_vendor=", init_tsp_vendor);

static int mxt_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
    struct mxt_platform_data *pdata = client->dev.platform_data;
    struct mxt_data *data;
    struct input_dev *input_dev;
    int ret;
    int i;
    u8 value1 = 0;
    u8 **tsp_config;
#ifdef CONFIG_MACH_MEHMET
	MEHMET_HW_VERSION mehmet_hw_version;
	mehmet_hw_version = mehmet_get_hw_version();
	if(mehmet_hw_version < MEHMET_HW_MP4) {
		if(touch_v < 0)
			touch_v = TOUCH_LG; // default LG
	} else { // VE1,2
		if(touch_v < 0)
			touch_v = TOUCH_HS; // default HS
	}

	printk(KERN_INFO "[ATMEL] config data: %s\n", touch_v == TOUCH_LG ? "LG" : "HS");
#endif
	t0_info_data    = t0_info_dataset[touch_v]; 
	t37_config_data = t37_config_dataset[touch_v];
	t68_config_data = t68_config_dataset[touch_v];
	t38_config_data = t38_config_dataset[touch_v];
	t71_config_data = t71_config_dataset[touch_v];
	t7_config_data  = t7_config_dataset[touch_v];
	t8_config_data  = t8_config_dataset[touch_v];
	t9_config_data  = t9_config_dataset[touch_v];
	t15_config_data = t15_config_dataset[touch_v];
	t18_config_data = t18_config_dataset[touch_v];
	t19_config_data = t19_config_dataset[touch_v];
	t24_config_data = t24_config_dataset[touch_v];
	t25_config_data = t25_config_dataset[touch_v];
	t27_config_data = t27_config_dataset[touch_v];
	t40_config_data = t40_config_dataset[touch_v];
	t42_config_data = t42_config_dataset[touch_v];
	t43_config_data = t43_config_dataset[touch_v];
	t46_config_data = t46_config_dataset[touch_v];
	t47_config_data = t47_config_dataset[touch_v];
	t55_config_data = t55_config_dataset[touch_v];
	t56_config_data = t56_config_dataset[touch_v];
	t57_config_data = t57_config_dataset[touch_v];
	t61_config_data = t61_config_dataset[touch_v];
	t62_config_data = t62_config_dataset[touch_v];
	t63_config_data = t63_config_dataset[touch_v];
	t65_config_data = t65_config_dataset[touch_v];
	t66_config_data = t66_config_dataset[touch_v];
	t70_config_data = t70_config_dataset[touch_v];
	t73_config_data = t73_config_dataset[touch_v];
	t77_config_data = t77_config_dataset[touch_v];
	t79_config_data = t79_config_dataset[touch_v];
    /* Touch config Version Check */
    u16 obj_address = 0;
    u8 value = 0;
    u16 size;

    touch_is_pressed = 0;
#ifdef FEATURE_TOUCH_TOUCH_BOOSTER
    tsp_press_status = 0;
#endif

#ifdef FEATURE_TOUCH_PASSIVEPEN
	pen_mode_time = msecs_to_jiffies(PEN_INTERVAL); // 300ms
#endif
    if ( !pdata ) {
        printk("[%s()] missing platform data\n", __FUNCTION__);
        return -ENODEV;
    }

    if ( pdata->max_finger_touches <= 0 ) {
        printk("[%s()] confirm max_finger_touches\n", __FUNCTION__);
        return -EINVAL;
    }

#ifdef FEATURE_TOUCH_PASSIVEPEN
	pdata->max_finger_touches += 1;
#endif
	data = kzalloc(sizeof(*data) + pdata->max_finger_touches * sizeof(*data->fingers), GFP_KERNEL);

    if ( !data ) {
        printk("[%s()] kzalloc error\n", __FUNCTION__);
        return -ENOMEM;
    }
    data->pdata = pdata;
	data->num_fingers = pdata->max_finger_touches;
    data->power_on = pdata->power_on;
    data->power_off = pdata->power_off;
    data->read_ta_status = pdata->read_ta_status;
    data->client = client;
    i2c_set_clientdata(client, data);

    input_dev = input_allocate_device();
    if ( !input_dev ) {
        printk("[%s()] input device allocation failed\n", __FUNCTION__);
        ret = -ENOMEM;
        goto err_alloc_dev;
    }

    printk("[%s()] ... \n", __FUNCTION__);

    data->input_dev = input_dev;
    input_dev->name = "atmel_1664";
    input_dev->id.bustype = BUS_I2C;
    input_dev->dev.parent = &client->dev;

    set_bit(EV_SYN, input_dev->evbit);
    set_bit(EV_ABS, input_dev->evbit);
    set_bit(EV_KEY, input_dev->evbit);
    set_bit(MT_TOOL_FINGER, input_dev->keybit);
    set_bit(INPUT_PROP_DIRECT, input_dev->propbit);

    input_mt_init_slots(input_dev, MAX_USING_FINGER_NUM);

    input_set_abs_params(input_dev, ABS_MT_POSITION_X, pdata->min_x, pdata->max_x, 0, 0);
    input_set_abs_params(input_dev, ABS_MT_POSITION_Y, pdata->min_y, pdata->max_y, 0, 0);
    input_set_abs_params(input_dev, ABS_MT_TOUCH_MAJOR, 0, 255, 0, 0);
    input_set_abs_params(input_dev, ABS_MT_PRESSURE, 0, 255, 0, 0);
#ifdef _SUPPORT_SHAPE_TOUCH_
    input_set_abs_params(input_dev, ABS_MT_COMPONENT, 0, 255, 0, 0);
#endif

    input_set_drvdata(input_dev, data);

    ret = input_register_device(input_dev);
    if ( ret ) {
        input_free_device(input_dev);
        goto err_reg_dev;
    }

    data->gpio_read_done = pdata->gpio_read_done;
    msleep(100);

    data->power_on();
    touch_s3c_i2c5_set_platdata(NULL,0);
    msleep(200);        

    copy_data = data;

    if ( client->addr == MXT_APP_LOW ) {
        client->addr = MXT_BOOT_LOW;
    } else {
        client->addr = MXT_BOOT_HIGH;
    }

    ret = mxt_check_bootloader(client, MXT_WAITING_BOOTLOAD_CMD);
    if ( ret >= 0 ) {
        printk("[%s()] boot mode. firm update excute\n", __FUNCTION__);
        mxt_load_fw_bootmode(NULL, MXT_FW_NAME);
        msleep(MXT_SW_RESET_TIME);
    } else {
        if ( client->addr == MXT_BOOT_LOW ) {
            client->addr = MXT_APP_LOW;
        } else {
            client->addr = MXT_APP_HIGH;
        }
    }

    ret = mxt_init_touch_driver(data);
    if ( ret ) {
        printk("[%s()] chip initialization failed\n", __FUNCTION__);
        goto err_init_drv;
    }

#ifdef FEATURE_TOUCH_NOISE
    g_power_noise = gpio_get_value(nDC_OK) ? 0 : 1;
    printk("[%s()] power detected(%d)\n", __FUNCTION__, g_power_noise);
#endif

    /* tsp_family_id - 0xA2 : MXT-1446-S series */
    if ( data->family_id == 0xA2 ) {
        tsp_config = (u8 **)data->pdata->config;

#ifdef FEATURE_SUPPORT_FW_UPDATE
        if ( data->tsp_version != firmware_latest[0] ||
            (data->tsp_version == firmware_latest[0] && data->tsp_build != firmware_latest[1]) ) {

            printk("[%s()] firmware updating...\n", __FUNCTION__);
            if ( mxt_load_fw(NULL, MXT_FW_NAME) ) {
                goto err_config;
            } else {
                msleep(MXT_SW_RESET_TIME);
                mxt_init_touch_driver(data);
            }
        }
#endif

#ifdef FEATURE_TOUCH_TOUCH_BOOSTER
        INIT_DELAYED_WORK(&data->dvfs_dwork, mxt_set_dvfs_off);
#endif
        INIT_DELAYED_WORK(&data->tune_dwork, mxt_tune_dwork);
    } else {
        printk("[%s()] invalid tsp_family_id\n", __FUNCTION__);
        goto err_config;
    }

    get_object_info(data, SPT_USERDATA_T38, &size, &obj_address);

    read_mem(data, obj_address, 1, &value);
	printk("[ATMEL] obj_address=[0x%04x(%d)] Config Version=[0x%02X(%d)]\n", obj_address, obj_address, value, value);

#ifdef FEATURE_SUPPORT_ZIG_TUNING
    if ( 0xFF == value ) {
        g_is_tuning_mode = 1;
    }
#endif

    //if ( value != t0_info_data[1] && value != 0xFF ) {
    get_config(data, SPT_USERDATA_T38, 1, 1, &value1);
    printk("[ATMEL] config data version (0x%02x -> 0x%02x)\n", value, t0_info_data[1]);
    printk("[ATMEL] saved tsp vendor    (%s -> %s)\n", value1 == 0 ? "LG" : "HS", touch_v == 0 ? "LG" : "HS");
    if ( value != t0_info_data[1] || touch_v != value1) { // config version & tsp type check 0:LG, 1:HS
	    printk("[ATMEL] config update starting ...\n");

	    for ( i = 0; atmel_tsp_config[i] != RESERVED_T255; i++ ) {

		    ret = mxt_defconfig_settings(data, atmel_tsp_config[i]);
		    if ( ret < 0 ) {
			    printk("[%s()] T%d ERROR: mxt_defconfig_settings  ...\n", __FUNCTION__, atmel_tsp_config[i]);
		    }
	    }
#ifdef FEATURE_TOUCH_PASSIVEPEN
	    value1 = t47_config_data[0];
	    set_config(data, PROCI_STYLUS_T47, 0, 1, &value1); // passive pen enable
#endif
	    ret = mxt_backup(data); // save config
	    if ( ret ) {
		    goto err_backup;
	    }
	    msleep(100);        
    } else {
    }    

#ifdef FEATURE_TOUCH_1ST_POINT
    hrtimer_init(&data->timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
    data->timer.function = touch_1st_point_timer_func;
#endif    

    data->x_dropbits = 0;
    data->y_dropbits = 2;
    printk("[%s()] x_dropbits=[%d] y_dropbits=[%d]\n", __FUNCTION__, data->x_dropbits, data->y_dropbits);

    /* reset the touch IC. */
    ret = mxt_reset(data);
    if ( ret ) {
        goto err_reset;
    }
    msleep(MXT_SW_RESET_TIME);

#ifdef FEATURE_SUPPORT_ZIG_TUNING
    if ( !g_is_tuning_mode ) {
#endif
	    mxt_PROCI_STYLUS_T47(copy_data);
	    mxt_PROCG_NOISESUPPRESSION_T62(copy_data);    
#ifdef FEATURE_SUPPORT_ZIG_TUNING
    } /* if ( !g_is_tuning_mode ) */
#endif

    calibrate_chip_e();

    for ( i = 0; i < data->num_fingers; i++ ) {
        data->fingers[i].state = MXT_STATE_INACTIVE;
    }

    ret = request_threaded_irq(IRQ_EINT(4), NULL, mxt_irq_thread, IRQF_TRIGGER_LOW | IRQF_ONESHOT, "mxt_ts", data);
    if ( ret < 0 ) {
        goto err_irq;
    }

    ret = sysfs_create_group(&client->dev.kobj, &atm1664_attr_group);
    if ( ret ) {
        printk("[%s()] atm1664_attr_group create failed\n", __FUNCTION__);
    }

#ifdef FEATURE_TOUCH_WIFI_TOOL
    sysfs_bin_attr_init(&data->mem_access_attr);
    data->mem_access_attr.attr.name = "mem_access";
    data->mem_access_attr.attr.mode = S_IRUGO | S_IWUSR;
    data->mem_access_attr.read = mxt_mem_access_read;
    data->mem_access_attr.write = mxt_mem_access_write;
    data->mem_access_attr.size = data->mem_size;

    if (sysfs_create_bin_file(&client->dev.kobj,
                              &data->mem_access_attr) < 0) {
        dev_err(&client->dev, "Failed to create %s\n",
        data->mem_access_attr.attr.name);
        goto err_remove_sysfs_group;
    }
#endif

#ifdef CONFIG_HAS_EARLYSUSPEND
    data->early_suspend.level = EARLY_SUSPEND_LEVEL_BLANK_SCREEN + 1;
    data->early_suspend.suspend = mxt_early_suspend;
    data->early_suspend.resume = mxt_late_resume;
    register_early_suspend(&data->early_suspend);
#endif
    mxt_enabled = 1;
#ifdef FEATURE_TOUCH_WIFI_TOOL
    data->state = APPMODE;
#endif

    return 0;

#ifdef FEATURE_TOUCH_WIFI_TOOL
err_remove_sysfs_group:
	sysfs_remove_group(&client->dev.kobj, &mxt_attr_group);
#endif
err_irq:
    printk("[%s()] err_irq\n", __FUNCTION__);
err_reset:
    printk("[%s()] err_reset\n", __FUNCTION__);
err_backup:
    printk("[%s()] err_backup\n", __FUNCTION__);
err_config:
    printk("[%s()] err_config\n", __FUNCTION__);
    kfree(data->objects);
err_init_drv:
    printk("[%s()] err_init_drv\n", __FUNCTION__);
    gpio_free(data->gpio_read_done);
err_reg_dev:
    printk("[%s()] err_reg_dev\n", __FUNCTION__);
err_alloc_dev:
    printk("[%s()] err_alloc_dev\n", __FUNCTION__);
    kfree(data);

    return ret;
}

static int __devexit mxt_remove(struct i2c_client *client)
{
    struct mxt_data *data = i2c_get_clientdata(client);

#ifdef CONFIG_HAS_EARLYSUSPEND
    unregister_early_suspend(&data->early_suspend);
#endif
    free_irq(IRQ_EINT(4), data);
    kfree(data->objects);
    gpio_free(data->gpio_read_done);
    data->power_off();
    input_unregister_device(data->input_dev);
    kfree(data);

    return 0;
}

static struct i2c_device_id mxt_idtable[] = {
    {"atmel_1664", 0},
    {},
};
MODULE_DEVICE_TABLE(i2c, mxt_idtable);

static struct i2c_driver mxt_i2c_driver = {
    .id_table = mxt_idtable,
    .probe = mxt_probe,
    .remove = __devexit_p(mxt_remove),
    .driver = {
        .owner  = THIS_MODULE,
        .name   = MXT_DEV_NAME,
    },
};

static int __init mxt_init(void)
{
    msleep(500);

    if ( get_touch_ic_check()==1 ) {
        printk("[%s()] initialize failed : confirm IC (FOCALTECH?)\n", __FUNCTION__);
        return 0;
    }

    printk("[%s()] Initializing ...\n", __FUNCTION__);

    return i2c_add_driver(&mxt_i2c_driver);
}

static void __exit mxt_exit(void)
{
    i2c_del_driver(&mxt_i2c_driver);
}

module_init(mxt_init);
module_exit(mxt_exit);

MODULE_DESCRIPTION("Atmel MaXTouch driver");
MODULE_AUTHOR("ki_won.kim<ki_won.kim@samsung.com>");
MODULE_LICENSE("GPL");

