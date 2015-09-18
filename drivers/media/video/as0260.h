/* linux/drivers/media/video/as0260.h
 *
 * Copyright (c) 2010 Samsung Electronics Co., Ltd.
 *	http://www.samsung.com/
 *
 * Driver for as0260 (SXGA camera) from Samsung Electronics
 * 1/6" 1.3Mp CMOS Image Sensor SoC with an Embedded Image Processor
 * supporting MIPI CSI-2
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */


#ifndef __as0260_H__
#define as0260_H

#define AS0260_DRIVER_NAME	"AS0260"

#define DEFAULT_FMT V4L2_PIX_FMT_UYVY

// The Command Register (SYSCTL R128)
#define COMMAND_REG 0x0080
// The System Manager (driver page)
#define SYSMGR_DRV 23
// System Manager variables (offsets)
#define SYSMGR_NEXT_STATE 0
#define SYSMGR_CURRENT_STATE 1
#define SYSMGR_CMD_STATUS 2
// The Sequencer (driver page)
#define SEQUENCER_DRV 1
// Sequencer variables (offsets)
#define SEQ_ERROR_CODE 6
// The Command Handler (driver page)
#define CMD_HANDLER_DRV 31
// Command Handler variables (offsets)
#define CMD_HANDLER_WAIT_EVENT_ID 0
#define CMD_HANDLER_NUM_EVENTS 2
// Command Handler events

#define USE_INIT_REG_AWB_MODIFY			1
#define USE_INIT_REG_POLL_DELAY			0
#define DEBUG_RETRY_TEST					1

#define FEATURE_TW_CAMERA_P_REG_WRITE


//#define FEATURE_TW_CAMERA_FPS28_MCLK24_PCLK92_U  // ok but err
//#define FEATURE_TW_CAMERA_FPS28_MCLK24_PCLK92_H
//#define FEATURE_TW_CAMERA_FPS28_MCLK24_PCLK92_MIPI1_H // preview green 

//#define FEATURE_TW_CAMERA_FPS27_MCLK24_PCLK82_H  // mclk23.6 OK 
//#define FEATURE_TW_CAMERA_FPS28_MCLK24_PCLK92_V

//#define FEATURE_TW_CAMERA_FPS30_MCLK24_PCLK96_V  // mclk23.6 OK default 
#define FEATURE_TW_CAMERA_FPS26_MCLK12_PCLK92_V  // pen ok think no
#if defined(FEATURE_TW_CAMERA_FPS26_MCLK12_PCLK92_V)
//#define FEATURE_TW_CAMERA_NO_SET_PLL
#endif
//#define FEATURE_TW_CAMERA_FPS26_MCLK12_PCLK86_V  // pen ok think no


enum as0260_i2c_size {
	I2C_8BIT	= 1,
	I2C_16BIT	= 2,
	I2C_32BIT	= 4,
	I2C_MAX		= 4,
};

#define POLL_DELAY 30
#define INIT_DELAY 10
//#define OUT_FORMAT 0x4011
//#define OUT_FORMAT 0x4010 Not OK
//#define OUT_FORMAT 0x4010
#define OUT_FORMAT 0x4010 // YCBYCR422 

#define CAM_PORT_PORCH 3 
typedef enum
{
	CMD_HANDLER_EVENT_EOF = 1,// end-of-frame
	CMD_HANDLER_EVENT_SOF = 2,// start-of-frame
} CMD_HANDLER_WAIT_EVENT_E;


typedef enum
{
	HC_APPLY_PATCH = 0x0001,
	HC_SET_STATE = 0x0002,
	HC_REFRESH = 0x0004,
	HC_WAIT_FOR_EVENT= 0x0008,
	HC_OK= 0x8000,
}HOST_COMMAND_E;

typedef enum
{
	SS_ENTER_CONFIG_CHANGE = 0x28, // re-config sensor and subsystems, resume streaming
	SS_STREAMING = 0x31, // system streaming
	SS_START_STREAMING = 0x34, // start streaming (when suspended)
	SS_ENTER_SUSPEND = 0x40, // stop streaming, suspend all subsystems
	SS_SUSPENDED = 0x41, // all subsystems suspended
	SS_ENTER_STANDBY= 0x50,// enter standby
	SS_STANDBY = 0x52,// system in standby
	SS_LEAVE_STANDBY= 0x54,// leave standby (resume streaming)
} SYSTEM_STATE_E;


enum as0260_runmode {
	as0260_RUNMODE_NOTREADY,
	as0260_RUNMODE_IDLE,
	as0260_RUNMODE_RUNNING,
};
struct as0260_reg {
	unsigned short waddr;
//	unsigned short wdata;
	uint32_t wdata;
	u8 len;
};

struct as0260_tbl {
	const struct as0260_reg *as0260_regs;
	int size;
};
/* --------------------------------------------------------------------------*/
/* [WhiteBalance Auto] */
struct as0260_reg as0260_WB_auto[] =
{
	{0x098E, 0xC941,2},
	{0xC941, 0x03,1}, 		// CAM_AWB_AWBMODE
	{0x098E, 0xC944,2}, 		// LOGICAL_ADDRESS_ACCESS [CAM_AWB_K_R_L]
	{0xC944, 0x80,1}, 		// CAM_AWB_K_R_L
	{0xC945, 0x80,1}, 		// CAM_AWB_K_G_L
	{0xC946, 0x80,1}, 		// CAM_AWB_K_B_L
	{0xC947, 0x80,1}, 		// CAM_AWB_K_R_R
	{0xC948, 0x80,1}, 		// CAM_AWB_K_G_R
	{0xC949, 0x80,1}, 		// CAM_AWB_K_B_R
};

/* [WhiteBalance cloudy] */
struct as0260_reg as0260_WB_cloudy[] =
{
	{0x098E, 0xC941,2},
	{0xC941, 0x01,1}, 		// CAM_AWB_AWBMODE
	{0x098E, 0xC947,2}, 		// LOGICAL_ADDRESS_ACCESS [CAM_AWB_K_R_R]
	{0xC944, 0x80,1}, 		// CAM_AWB_K_R_L
	{0xC945, 0x80,1}, 		// CAM_AWB_K_G_L
	{0xC946, 0x80,1}, 		// CAM_AWB_K_B_L
	{0xC947, 0xCB,1}, 		// CAM_AWB_K_R_R
	{0xC948, 0x80,1}, 		// CAM_AWB_K_G_R
	{0xC949, 0x6D,1}, 		// CAM_AWB_K_B_R
	{0xC8F2, 0x1964,2}, 		// CAM_AWB_COLOR_TEMPERATURE

};

/* [WhiteBalance daylight] */
struct as0260_reg as0260_WB_daylight[] =
{
	{0x098E, 0xC941,2},
	{0xC941, 0x01,1}, 		// CAM_AWB_AWBMODE
	{0x098E, 0xC947,2}, 		// LOGICAL_ADDRESS_ACCESS [CAM_AWB_K_R_R]
	{0xC944, 0x80,1}, 		// CAM_AWB_K_R_L
	{0xC945, 0x80,1}, 		// CAM_AWB_K_G_L
	{0xC946, 0x80,1}, 		// CAM_AWB_K_B_L
	{0xC947, 0xA7,1}, 		// CAM_AWB_K_R_R
	{0xC948, 0x80,1}, 		// CAM_AWB_K_G_R
	{0xC949, 0x7E,1}, 		// CAM_AWB_K_B_R
	{0xC8F2, 0x1964,2}, 		// CAM_AWB_COLOR_TEMPERATURE

};

/* [WhiteBalance fluorescent] */
struct as0260_reg as0260_WB_fluorescent[] =
{
	{0x098E, 0xC941,2},
	{0xC941, 0x01,1}, 		// CAM_AWB_AWBMODE
	{0x098E, 0xC947,2}, 		// LOGICAL_ADDRESS_ACCESS [CAM_AWB_K_R_R]
	{0xC944, 0x80,1}, 		// CAM_AWB_K_R_L
	{0xC945, 0x80,1}, 		// CAM_AWB_K_G_L
	{0xC946, 0x80,1}, 		// CAM_AWB_K_B_L
	{0xC947, 0xA9,1}, 		// CAM_AWB_K_R_R
	{0xC948, 0x80,1}, 		// CAM_AWB_K_G_R
	{0xC949, 0x8C,1}, 		// CAM_AWB_K_B_R
	{0xC8F2, 0x0FA0,2}, 		// CAM_AWB_COLOR_TEMPERATURE

};

/* [WhiteBalance Incandescent] */
struct as0260_reg as0260_WB_Incandescent[] =
{
	{0x098E, 0xC941,2},
	{0xC941, 0x01,1}, 		// CAM_AWB_AWBMODE
	{0x098E, 0xC944,2}, 		// LOGICAL_ADDRESS_ACCESS [CAM_AWB_K_R_L]
	{0xC944, 0x72,1}, 		// CAM_AWB_K_R_L
	{0xC945, 0x80,1}, 		// CAM_AWB_K_G_L
	{0xC946, 0x90,1}, 		// CAM_AWB_K_B_L
	{0xC947, 0x80,1}, 		// CAM_AWB_K_R_R
	{0xC948, 0x80,1}, 		// CAM_AWB_K_G_R
	{0xC949, 0x80,1}, 		// CAM_AWB_K_B_R
	{0xC8F2, 0x09C4,2}, 		// CAM_AWB_COLOR_TEMPERATURE

};


/* --------------------------------------------------------------------------*/

/* [brightness+2] */
struct as0260_reg as0260_brightness_P02_regs[] =
{
	{0x098E, 0x49C4,2}, 		//LOGICAL_ADDRESS_ACCESS
	{0xC9C4, 0x017c,2}, 		//CAM_LL_GAMMA
};

/* [brightness+1] */
struct as0260_reg as0260_brightness_P01_regs[] =
{
	{0x098E, 0x49C4,2}, 		//LOGICAL_ADDRESS_ACCESS
	{0xC9C4, 0x0118,2}, 		//CAM_LL_GAMMA
};
/* [brightness0] */
struct as0260_reg as0260_brightness_P00_regs[] =
{
	{0x098E, 0x49C4,2}, 		//LOGICAL_ADDRESS_ACCESS
	{0xC9C4, 0x00DC,2}, 		//CAM_LL_GAMMA
};
/* [brightness-1] */
struct as0260_reg as0260_brightness_M01_regs[] =
{
	{0x098E, 0x49C4,2}, 		//LOGICAL_ADDRESS_ACCESS
	{0xC9C4, 0x00A0,2}, 		//CAM_LL_GAMMA
};
/* [brightness-2] */
struct as0260_reg as0260_brightness_M02_regs[] =
{
	{0x098E, 0x49C4,2}, 		//LOGICAL_ADDRESS_ACCESS
	{0xC9C4, 0x0050,2}, 		//CAM_LL_GAMMA
};


struct as0260_reg as0260_effect_mono_regs[] =
{
	{0x098E, 0xC878,2},      // LOGICAL_ADDRESS_ACCESS
	{0xC878, 0x01,1},      // CAM_SFX_CONTROL
	//{0xDC00, 0x28,1},      // SYSMGR_NEXT_STATE
	//{0x0080, 0x8004,2},      // COMMAND_REGISTER
};

struct as0260_reg as0260_effect_sepia_regs[] =
{
	{0x098E, 0xC87A,2},      // LOGICAL_ADDRESS_ACCESS [CAM_SFX_SEPIA_CR]
	{0xC87A, 0x23,1},      // CAM_SFX_SEPIA_CR
	{0xC87B, 0xB2,1},      // CAM_SFX_SEPIA_CB
	{0x098E, 0xC878,2},      // LOGICAL_ADDRESS_ACCESS
	{0xC878, 0x02,1},      // CAM_SFX_CONTROL
	//{0xDC00, 0x28,1},      // SYSMGR_NEXT_STATE
	//{0x0080, 0x8004,1},      // COMMAND_REGISTER

};

struct as0260_reg as0260_effect_Negative_regs[] =
{
	{0x098E, 0xC878,2},      // LOGICAL_ADDRESS_ACCESS
	{0xC878, 0x03,2},      // CAM_SFX_CONTROL
	//{0xDC00, 0x28,2},      // SYSMGR_NEXT_STATE
	//{0x0080, 0x8004,2},      // COMMAND_REGISTER
};


struct as0260_reg as0260_effect_solarize_regs[] =
{
	{0x098E, 0xC878,2},      // LOGICAL_ADDRESS_ACCESS
	{0xC878, 0x04,2},      // CAM_SFX_CONTROL
	//{0xDC00, 0x28,2},      // SYSMGR_NEXT_STATE
	//{0x0080, 0x8004,2},      // COMMAND_REGISTER
};

struct as0260_reg as0260_effect_aqua_regs[] =
{
	{0x098E, 0xC87A,2},      // LOGICAL_ADDRESS_ACCESS [CAM_SFX_SEPIA_CR]
	{0xC87A, 0xD8,2},      // CAM_SFX_SEPIA_CR
	{0xC87B, 0x2D,2},      // CAM_SFX_SEPIA_CB
	{0x098E, 0xC878,2},      // LOGICAL_ADDRESS_ACCESS
	{0xC878, 0x02,2},      // CAM_SFX_CONTROL
	//{0xDC00, 0x28,2},      // SYSMGR_NEXT_STATE
	//{0x0080, 0x8004,2},      // COMMAND_REGISTER
};

struct as0260_reg as0260_effect_off_regs[] =
{
	{0x098E, 0xC878,2},      // LOGICAL_ADDRESS_ACCESS [CAM_SFX_CONTROL]
	{0xC878, 0x00,2},      // CAM_SFX_CONTROL
	//{0xDC00, 0x28,2},      // SYSMGR_NEXT_STATE
	//{0x0080, 0x8004,2},      // COMMAND_REGISTER
};
/* --------------------------------------------------------------------------*/
#if defined(FEATURE_TW_CAMERA_FPS26_MCLK12_PCLK86_V)
struct as0260_reg as0260_1080[] =
{
	{0x098E, 0xCA12,2},      // LOGICAL_ADDRESS_ACCESS [CAM_SYSCTL_PLL_ENABLE]
	{0xCA12, 0x01,1},      // CAM_SYSCTL_PLL_ENABLE
	{0xCA13, 0x00,1},      // CAM_SYSCTL_SENSOR_CLK_DIV2_EN
	{0xCA14, 0x0256,2}, 		//cam_sysctl_pll_divider_m_n = 598
	{0xCA16, 0x0070,2}, 		//cam_sysctl_pll_divider_p = 112
	{0xCA18, 0x7F7C,2}, 		//cam_sysctl_pll_divider_p4_p5_p6 = 32636
	{0xCA1C, 0x8040,2}, 		//cam_port_output_control = 32832
	{0xCA1E, 0x0005,2}, 		//cam_port_porch = 5
	{0xCA20, 0x0F00,2}, 		//cam_port_mipi_timing_t_hs_zero = 3840
	{0xCA22, 0x0B07,2}, 		//cam_port_mipi_timing_t_hs_exit_hs_trail = 2823
	{0xCA24, 0x0D01,2}, 		//cam_port_mipi_timing_t_clk_post_clk_pre = 3329
	{0xCA26, 0x071D,2}, 		//cam_port_mipi_timing_t_clk_trail_clk_zero = 1821
	{0xCA28, 0x0006,2}, 		//cam_port_mipi_timing_t_lpx = 6
	{0xCA2A, 0x0A0C,2}, 		//cam_port_mipi_timing_init_timing = 2572
	{0xCA2C, 0x0000,2}, 		//cam_port_mipi_timing_t_hs_pre = 0


	//[Timing_settings]
	{0xC800, 0x0020,2}, 		//cam_sensor_cfg_y_addr_start = 32
	{0xC802, 0x0020,2}, 		//cam_sensor_cfg_x_addr_start = 32
	{0xC804, 0x045F,2}, 		//cam_sensor_cfg_y_addr_end = 1119
	{0xC806, 0x07A7,2}, 		//cam_sensor_cfg_x_addr_end = 1959
	{0xC808, 0x03278AC4,4}, 		//cam_sensor_cfg_pixclk = 52923076
	{0xC80C, 0x0001,2}, 		//cam_sensor_cfg_row_speed = 1
	{0xC80E, 0x0336,2}, 		//cam_sensor_cfg_fine_integ_time_min = 822
	{0xC810, 0x0AE3,2}, 		//cam_sensor_cfg_fine_integ_time_max = 2787
	{0xC812, 0x0491,2}, 		//cam_sensor_cfg_frame_length_lines = 1169
	{0xC814, 0x0CA1,2}, 		//cam_sensor_cfg_line_length_pck = 3233
	{0xC816, 0x00D4,2}, 		//cam_sensor_cfg_fine_correction = 212
	{0xC818, 0x043B,2}, 		//cam_sensor_cfg_cpipe_last_row = 1083
	{0xC820, 0x0010,2}, 		//cam_sensor_cfg_reg_0_data = 16
	{0xC830, 0x0002,2}, 		//cam_sensor_control_read_mode = 2
	{0xC858, 0x0000,2}, 		//cam_crop_window_xoffset = 0
	{0xC85A, 0x0000,2}, 		//cam_crop_window_yoffset = 0
	{0xC85C, 0x0780,2}, 		//cam_crop_window_width = 1920
	{0xC85E, 0x0438,2}, 		//cam_crop_window_height = 1080
	{0xC86C, 0x0780,2}, 		//cam_output_width = 1920
	{0xC86E, 0x0438,2}, 		//cam_output_height = 1080
	{0xC870, 0x40D0,2}, 		//cam_output_format = 16592
	{0xC87C, 0x00,1}, 		//cam_aet_aemode = 0
	{0xC88E, 0x1C00,2}, 		//cam_aet_max_frame_rate = 7168
	{0xC890, 0x1C00,2}, 		//cam_aet_min_frame_rate = 7168
	{0xC94C, 0x0000,2}, 		//cam_stat_awb_clip_window_xstart = 0
	{0xC94E, 0x0000,2}, 		//cam_stat_awb_clip_window_ystart = 0
	{0xC950, 0x077F,2}, 		//cam_stat_awb_clip_window_xend = 1919
	{0xC952, 0x0437,2}, 		//cam_stat_awb_clip_window_yend = 1079
	{0xC954, 0x0000,2}, 		//cam_stat_ae_initial_window_xstart = 0
	{0xC956, 0x0000,2}, 		//cam_stat_ae_initial_window_ystart = 0
	{0xC958, 0x017F,2}, 		//cam_stat_ae_initial_window_xend = 383
	{0xC95A, 0x00D7,2}, 		//cam_stat_ae_initial_window_yend = 215
	

};

struct as0260_reg as0260_1440[] =
{
	{0x098E, 0xCA12,2},      // LOGICAL_ADDRESS_ACCESS [CAM_SYSCTL_PLL_ENABLE]
	{0xCA12, 0x01,1},      // CAM_SYSCTL_PLL_ENABLE
	{0xCA13, 0x00,1},      // CAM_SYSCTL_SENSOR_CLK_DIV2_EN

	{0xCA14, 0x0256,2}, 		//cam_sysctl_pll_divider_m_n = 598
	{0xCA16, 0x0070,2}, 		//cam_sysctl_pll_divider_p = 112
	{0xCA18, 0x7F7C,2}, 		//cam_sysctl_pll_divider_p4_p5_p6 = 32636
	{0xCA1C, 0x8040,2}, 		//cam_port_output_control = 32832
	{0xCA1E, 0x0005,2}, 		//cam_port_porch = 5
	{0xCA20, 0x0F00,2}, 		//cam_port_mipi_timing_t_hs_zero = 3840
	{0xCA22, 0x0B07,2}, 		//cam_port_mipi_timing_t_hs_exit_hs_trail = 2823
	{0xCA24, 0x0D01,2}, 		//cam_port_mipi_timing_t_clk_post_clk_pre = 3329
	{0xCA26, 0x071D,2}, 		//cam_port_mipi_timing_t_clk_trail_clk_zero = 1821
	{0xCA28, 0x0006,2}, 		//cam_port_mipi_timing_t_lpx = 6
	{0xCA2A, 0x0A0C,2}, 		//cam_port_mipi_timing_init_timing = 2572
	{0xCA2C, 0x0000,2}, 		//cam_port_mipi_timing_t_hs_pre = 0


	//[Timing_settings]
	{0xC800, 0x0020,2}, 		//cam_sensor_cfg_y_addr_start = 32  //\uc218\uc815 Start >>
	{0xC802, 0x0120,2}, 		//cam_sensor_cfg_x_addr_start = 288 
	{0xC804, 0x045F,2}, 		//cam_sensor_cfg_y_addr_end = 1119  
	{0xC806, 0x06C7,2}, 		//cam_sensor_cfg_x_addr_end = 1735  //\uc218\uc815 end>
	{0xC808, 0x03278AC4,4}, 		//cam_sensor_cfg_pixclk = 52923076
	{0xC80C, 0x0001,2}, 		//cam_sensor_cfg_row_speed = 1
	{0xC80E, 0x0336,2}, 		//cam_sensor_cfg_fine_integ_time_min = 822
	{0xC810, 0x0AE3,2}, 		//cam_sensor_cfg_fine_integ_time_max = 2787
	{0xC812, 0x0491,2}, 		//cam_sensor_cfg_frame_length_lines = 1169
	{0xC814, 0x0CA1,2}, 		//cam_sensor_cfg_line_length_pck = 3233
	{0xC816, 0x00D4,2}, 		//cam_sensor_cfg_fine_correction = 212
	{0xC818, 0x043B,2}, 		//cam_sensor_cfg_cpipe_last_row = 1083
	{0xC820, 0x0010,2}, 		//cam_sensor_cfg_reg_0_data = 16
	{0xC830, 0x0002,2}, 		//cam_sensor_control_read_mode = 2
	{0xC858, 0x0000,2}, 		//cam_crop_window_xoffset = 0
	{0xC85A, 0x0000,2}, 		//cam_crop_window_yoffset = 0
	{0xC85C, 0x05A0,2}, 		//cam_crop_window_width = 1440   //\uc218\uc815 Start >>
	{0xC85E, 0x0438,2}, 		//cam_crop_window_height = 1080  
	{0xC86C, 0x05A0,2}, 		//cam_output_width = 1440        
	{0xC86E, 0x0438,2}, 		//cam_output_height = 1080       
	{0xC870, 0x4010,2},	//cam_output_format = 16592        //\uc218\uc815 end >>
	{0xC87C, 0x00,1}, 		//cam_aet_aemode = 0
	{0xC88E, 0x1C00,2}, 		//cam_aet_max_frame_rate = 7168
	{0xC890, 0x1C00,2}, 		//cam_aet_min_frame_rate = 7168
	{0xC94C, 0x0000,2}, 		//cam_stat_awb_clip_window_xstart = 0
	{0xC94E, 0x0000,2}, 		//cam_stat_awb_clip_window_ystart = 0
	{0xC950, 0x05a0,2}, 		//cam_stat_awb_clip_window_xend = 1919  //\uc218\uc815 Start >>
	{0xC952, 0x0438,2}, 		//cam_stat_awb_clip_window_yend = 1079
	{0xC954, 0x0000,2}, 		// CAM_STAT_AE_INITIAL_WINDOW_XSTART
	{0xC956, 0x0000,2}, 		// CAM_STAT_AE_INITIAL_WINDOW_YSTART
	{0xC958, 0x0120,2}, 		// CAM_STAT_AE_INITIAL_WINDOW_XEND  
	{0xC95A, 0x00D8,2}, 		// CAM_STAT_AE_INITIAL_WINDOW_YEND   //\uc218\uc815 end >>;	
};

struct as0260_reg as0260_960[] =
{
	{0x098E, 0xCA12,2},      // LOGICAL_ADDRESS_ACCESS [CAM_SYSCTL_PLL_ENABLE]
	{0xCA12, 0x01,1},      // CAM_SYSCTL_PLL_ENABLE
	{0xCA13, 0x00,1},      // CAM_SYSCTL_SENSOR_CLK_DIV2_EN

{0xCA14, 0x0256,2}, 		//cam_sysctl_pll_divider_m_n = 598
{0xCA16, 0x0070,2}, 		//cam_sysctl_pll_divider_p = 112
{0xCA18, 0x7F7C,2}, 		//cam_sysctl_pll_divider_p4_p5_p6 = 32636
{0xCA1C, 0x8040,2}, 		//cam_port_output_control = 32832
{0xCA1E, 0x0005,2}, 		//cam_port_porch = 5
{0xCA20, 0x0F00,2}, 		//cam_port_mipi_timing_t_hs_zero = 3840
{0xCA22, 0x0B07,2}, 		//cam_port_mipi_timing_t_hs_exit_hs_trail = 2823
{0xCA24, 0x0D01,2}, 		//cam_port_mipi_timing_t_clk_post_clk_pre = 3329
{0xCA26, 0x071D,2}, 		//cam_port_mipi_timing_t_clk_trail_clk_zero = 1821
{0xCA28, 0x0006,2}, 		//cam_port_mipi_timing_t_lpx = 6
{0xCA2A, 0x0A0C,2}, 		//cam_port_mipi_timing_init_timing = 2572
{0xCA2C, 0x0000,2}, 		//cam_port_mipi_timing_t_hs_pre = 0


//[Timing_settings]
{0xC800, 0x0020,2}, 		//cam_sensor_cfg_y_addr_start = 32
{0xC802, 0x0120,2}, 		//cam_sensor_cfg_x_addr_start = 288
{0xC804, 0x045F,2}, 		//cam_sensor_cfg_y_addr_end = 1119
{0xC806, 0x06C7,2}, 		//cam_sensor_cfg_x_addr_end = 1735
{0xC808, 0x03278AC4,4}, 		//cam_sensor_cfg_pixclk = 52923076
{0xC80C, 0x0001,2}, 		//cam_sensor_cfg_row_speed = 1
{0xC80E, 0x0336,2}, 		//cam_sensor_cfg_fine_integ_time_min = 822
{0xC810, 0x0AE3,2}, 		//cam_sensor_cfg_fine_integ_time_max = 2787
{0xC812, 0x0491,2}, 		//cam_sensor_cfg_frame_length_lines = 1169
{0xC814, 0x0CA1,2}, 		//cam_sensor_cfg_line_length_pck = 3233
{0xC816, 0x00D4,2}, 		//cam_sensor_cfg_fine_correction = 212
{0xC818, 0x043B,2}, 		//cam_sensor_cfg_cpipe_last_row = 1083
{0xC820, 0x0010,2}, 		//cam_sensor_cfg_reg_0_data = 16
{0xC830, 0x0002,2}, 		//cam_sensor_control_read_mode = 2
{0xC858, 0x0000,2}, 		//cam_crop_window_xoffset = 0
{0xC85A, 0x0000,2}, 		//cam_crop_window_yoffset = 0
{0xC85C, 0x05A0,2}, 		//cam_crop_window_width = 1440
{0xC85E, 0x0438,2}, 		//cam_crop_window_height = 1080
{0xC86C, 0x0500,2}, 		//cam_output_width = 1280
{0xC86E, 0x03C0,2}, 		//cam_output_height = 960
{0xC870, 0x40D0,2}, 		//cam_output_format = 16592
{0xC87C, 0x00,1}, 		//cam_aet_aemode = 0
{0xC88E, 0x1C00,2}, 		//cam_aet_max_frame_rate = 7168
{0xC890, 0x1C00,2}, 		//cam_aet_min_frame_rate = 7168
{0xC94C, 0x0000,2}, 		//cam_stat_awb_clip_window_xstart = 0
{0xC94E, 0x0000,2}, 		//cam_stat_awb_clip_window_ystart = 0
{0xC950, 0x04FF,2}, 		//cam_stat_awb_clip_window_xend = 1279
{0xC952, 0x03BF,2}, 		//cam_stat_awb_clip_window_yend = 959
{0xC954, 0x0000,2}, 		//cam_stat_ae_initial_window_xstart = 0
{0xC956, 0x0000,2}, 		//cam_stat_ae_initial_window_ystart = 0
{0xC958, 0x00FF,2}, 		//cam_stat_ae_initial_window_xend = 255
{0xC95A, 0x00BF,2}, 		//cam_stat_ae_initial_window_yend = 191	
};

struct as0260_reg as0260_720[] =
{
	{0x098E, 0xCA12,2},      // LOGICAL_ADDRESS_ACCESS [CAM_SYSCTL_PLL_ENABLE]
	{0xCA12, 0x01,1},      // CAM_SYSCTL_PLL_ENABLE
	{0xCA13, 0x00,1},      // CAM_SYSCTL_SENSOR_CLK_DIV2_EN

{0xCA14, 0x0256,2}, 		//cam_sysctl_pll_divider_m_n = 598
{0xCA16, 0x0070,2}, 		//cam_sysctl_pll_divider_p = 112
{0xCA18, 0x7F7C,2}, 		//cam_sysctl_pll_divider_p4_p5_p6 = 32636
{0xCA1C, 0x8040,2}, 		//cam_port_output_control = 32832
{0xCA1E, 0x0005,2}, 		//cam_port_porch = 5
{0xCA20, 0x0F00,2}, 		//cam_port_mipi_timing_t_hs_zero = 3840
{0xCA22, 0x0B07,2}, 		//cam_port_mipi_timing_t_hs_exit_hs_trail = 2823
{0xCA24, 0x0D01,2}, 		//cam_port_mipi_timing_t_clk_post_clk_pre = 3329
{0xCA26, 0x071D,2}, 		//cam_port_mipi_timing_t_clk_trail_clk_zero = 1821
{0xCA28, 0x0006,2}, 		//cam_port_mipi_timing_t_lpx = 6
{0xCA2A, 0x0A0C,2}, 		//cam_port_mipi_timing_init_timing = 2572
{0xCA2C, 0x0000,2}, 		//cam_port_mipi_timing_t_hs_pre = 0


//[Timing_settings]
{0xC800, 0x0020,2}, 		//cam_sensor_cfg_y_addr_start = 32
{0xC802, 0x0020,2}, 		//cam_sensor_cfg_x_addr_start = 32
{0xC804, 0x045F,2}, 		//cam_sensor_cfg_y_addr_end = 1119
{0xC806, 0x07A7,2}, 		//cam_sensor_cfg_x_addr_end = 1959
{0xC808, 0x03278AC4,4}, 		//cam_sensor_cfg_pixclk = 52923076
{0xC80C, 0x0001,2}, 		//cam_sensor_cfg_row_speed = 1
{0xC80E, 0x0336,2}, 		//cam_sensor_cfg_fine_integ_time_min = 822
{0xC810, 0x0AE3,2}, 		//cam_sensor_cfg_fine_integ_time_max = 2787
{0xC812, 0x0491,2}, 		//cam_sensor_cfg_frame_length_lines = 1169
{0xC814, 0x0CA1,2}, 		//cam_sensor_cfg_line_length_pck = 3233
{0xC816, 0x00D4,2}, 		//cam_sensor_cfg_fine_correction = 212
{0xC818, 0x043B,2}, 		//cam_sensor_cfg_cpipe_last_row = 1083
{0xC820, 0x0010,2}, 		//cam_sensor_cfg_reg_0_data = 16
{0xC830, 0x0002,2}, 		//cam_sensor_control_read_mode = 2
{0xC858, 0x0000,2}, 		//cam_crop_window_xoffset = 0
{0xC85A, 0x0000,2}, 		//cam_crop_window_yoffset = 0
{0xC85C, 0x0780,2}, 		//cam_crop_window_width = 1920
{0xC85E, 0x0438,2}, 		//cam_crop_window_height = 1080
{0xC86C, 0x0500,2}, 		//cam_output_width = 1280
{0xC86E, 0x02D0,2}, 		//cam_output_height = 720
{0xC870, 0x40D0,2}, 		//cam_output_format = 16592
{0xC87C, 0x00,1}, 		//cam_aet_aemode = 0
{0xC88E, 0x1C00,2}, 		//cam_aet_max_frame_rate = 7168
{0xC890, 0x1C00,2}, 		//cam_aet_min_frame_rate = 7168
{0xC94C, 0x0000,2}, 		//cam_stat_awb_clip_window_xstart = 0
{0xC94E, 0x0000,2}, 		//cam_stat_awb_clip_window_ystart = 0
{0xC950, 0x04FF,2}, 		//cam_stat_awb_clip_window_xend = 1279
{0xC952, 0x02CF,2}, 		//cam_stat_awb_clip_window_yend = 719
{0xC954, 0x0000,2}, 		//cam_stat_ae_initial_window_xstart = 0
{0xC956, 0x0000,2}, 		//cam_stat_ae_initial_window_ystart = 0
{0xC958, 0x00FF,2}, 		//cam_stat_ae_initial_window_xend = 255
{0xC95A, 0x008F,2}, 		//cam_stat_ae_initial_window_yend = 143
};


struct as0260_reg as0260_640[] =
{
	{0x098E, 0xCA12,2},      // LOGICAL_ADDRESS_ACCESS [CAM_SYSCTL_PLL_ENABLE]
	{0xCA12, 0x01,1},      // CAM_SYSCTL_PLL_ENABLE
	{0xCA13, 0x00,1},      // CAM_SYSCTL_SENSOR_CLK_DIV2_EN

{0xCA14, 0x0256,2}, 		//cam_sysctl_pll_divider_m_n = 598
{0xCA16, 0x0070,2}, 		//cam_sysctl_pll_divider_p = 112
{0xCA18, 0x7F7C,2}, 		//cam_sysctl_pll_divider_p4_p5_p6 = 32636
{0xCA1C, 0x8040,2}, 		//cam_port_output_control = 32832
{0xCA1E, 0x0005,2}, 		//cam_port_porch = 5
{0xCA20, 0x0F00,2}, 		//cam_port_mipi_timing_t_hs_zero = 3840
{0xCA22, 0x0B07,2}, 		//cam_port_mipi_timing_t_hs_exit_hs_trail = 2823
{0xCA24, 0x0D01,2}, 		//cam_port_mipi_timing_t_clk_post_clk_pre = 3329
{0xCA26, 0x071D,2}, 		//cam_port_mipi_timing_t_clk_trail_clk_zero = 1821
{0xCA28, 0x0006,2}, 		//cam_port_mipi_timing_t_lpx = 6
{0xCA2A, 0x0A0C,2}, 		//cam_port_mipi_timing_init_timing = 2572
{0xCA2C, 0x0000,2}, 		//cam_port_mipi_timing_t_hs_pre = 0


//[Timing_settings]
{0xC800, 0x0020,2}, 		//cam_sensor_cfg_y_addr_start = 32
{0xC802, 0x0100,2}, 		//cam_sensor_cfg_x_addr_start = 256
{0xC804, 0x045D,2}, 		//cam_sensor_cfg_y_addr_end = 1117
{0xC806, 0x06AD,2}, 		//cam_sensor_cfg_x_addr_end = 1709
{0xC808, 0x03278AC4,4}, 		//cam_sensor_cfg_pixclk = 52923076
{0xC80C, 0x0001,2}, 		//cam_sensor_cfg_row_speed = 1
{0xC80E, 0x06A4,2}, 		//cam_sensor_cfg_fine_integ_time_min = 1700
{0xC810, 0x14A5,2}, 		//cam_sensor_cfg_fine_integ_time_max = 5285
{0xC812, 0x026D,2}, 		//cam_sensor_cfg_frame_length_lines = 621
{0xC814, 0x17C7,2}, 		//cam_sensor_cfg_line_length_pck = 6087
{0xC816, 0x01D9,2}, 		//cam_sensor_cfg_fine_correction = 473
{0xC818, 0x021B,2}, 		//cam_sensor_cfg_cpipe_last_row = 539
{0xC820, 0x0010,2}, 		//cam_sensor_cfg_reg_0_data = 16
{0xC830, 0x0012,2}, 		//cam_sensor_control_read_mode = 18
{0xC858, 0x0000,2}, 		//cam_crop_window_xoffset = 0
{0xC85A, 0x0000,2}, 		//cam_crop_window_yoffset = 0
{0xC85C, 0x02D0,2}, 		//cam_crop_window_width = 720
{0xC85E, 0x0218,2}, 		//cam_crop_window_height = 536
{0xC86C, 0x0280,2}, 		//cam_output_width = 640
{0xC86E, 0x01E0,2}, 		//cam_output_height = 480
{0xC870, 0x40D0,2}, 		//cam_output_format = 16592
{0xC87C, 0x00,1}, 		//cam_aet_aemode = 0
{0xC88E, 0x1C00,2}, 		//cam_aet_max_frame_rate = 7168
{0xC890, 0x1C00,2}, 		//cam_aet_min_frame_rate = 7168
{0xC94C, 0x0000,2}, 		//cam_stat_awb_clip_window_xstart = 0
{0xC94E, 0x0000,2}, 		//cam_stat_awb_clip_window_ystart = 0
{0xC950, 0x027F,2}, 		//cam_stat_awb_clip_window_xend = 639
{0xC952, 0x01DF,2}, 		//cam_stat_awb_clip_window_yend = 479
{0xC954, 0x0000,2}, 		//cam_stat_ae_initial_window_xstart = 0
{0xC956, 0x0000,2}, 		//cam_stat_ae_initial_window_ystart = 0
{0xC958, 0x007F,2}, 		//cam_stat_ae_initial_window_xend = 127
{0xC95A, 0x005F,2}, 		//cam_stat_ae_initial_window_yend = 95	
};

struct as0260_reg as0260_320[] =
{
	{0x098E, 0xCA12,2},      // LOGICAL_ADDRESS_ACCESS [CAM_SYSCTL_PLL_ENABLE]
	{0xCA12, 0x01,1},      // CAM_SYSCTL_PLL_ENABLE
	{0xCA13, 0x00,1},      // CAM_SYSCTL_SENSOR_CLK_DIV2_EN

{0xCA14, 0x0256,2}, 		//cam_sysctl_pll_divider_m_n = 598
{0xCA16, 0x0070,2}, 		//cam_sysctl_pll_divider_p = 112
{0xCA18, 0x7F7C,2}, 		//cam_sysctl_pll_divider_p4_p5_p6 = 32636
{0xCA1C, 0x8040,2}, 		//cam_port_output_control = 32832
{0xCA1E, 0x0005,2}, 		//cam_port_porch = 5
{0xCA20, 0x0F00,2}, 		//cam_port_mipi_timing_t_hs_zero = 3840
{0xCA22, 0x0B07,2}, 		//cam_port_mipi_timing_t_hs_exit_hs_trail = 2823
{0xCA24, 0x0D01,2}, 		//cam_port_mipi_timing_t_clk_post_clk_pre = 3329
{0xCA26, 0x071D,2}, 		//cam_port_mipi_timing_t_clk_trail_clk_zero = 1821
{0xCA28, 0x0006,2}, 		//cam_port_mipi_timing_t_lpx = 6
{0xCA2A, 0x0A0C,2}, 		//cam_port_mipi_timing_init_timing = 2572
{0xCA2C, 0x0000,2}, 		//cam_port_mipi_timing_t_hs_pre = 0


//[Timing_settings]
{0xC800, 0x0020,2}, 		//cam_sensor_cfg_y_addr_start = 32
{0xC802, 0x0100,2}, 		//cam_sensor_cfg_x_addr_start = 256
{0xC804, 0x045D,2}, 		//cam_sensor_cfg_y_addr_end = 1117
{0xC806, 0x06AD,2}, 		//cam_sensor_cfg_x_addr_end = 1709
{0xC808, 0x03278AC4,4}, 		//cam_sensor_cfg_pixclk = 52923076
{0xC80C, 0x0001,2}, 		//cam_sensor_cfg_row_speed = 1
{0xC80E, 0x06A4,2}, 		//cam_sensor_cfg_fine_integ_time_min = 1700
{0xC810, 0x14A5,2}, 		//cam_sensor_cfg_fine_integ_time_max = 5285
{0xC812, 0x026D,2}, 		//cam_sensor_cfg_frame_length_lines = 621
{0xC814, 0x17C7,2}, 		//cam_sensor_cfg_line_length_pck = 6087
{0xC816, 0x01D9,2}, 		//cam_sensor_cfg_fine_correction = 473
{0xC818, 0x021B,2}, 		//cam_sensor_cfg_cpipe_last_row = 539
{0xC820, 0x0010,2}, 		//cam_sensor_cfg_reg_0_data = 16
{0xC830, 0x0012,2}, 		//cam_sensor_control_read_mode = 18
{0xC858, 0x0000,2}, 		//cam_crop_window_xoffset = 0
{0xC85A, 0x0000,2}, 		//cam_crop_window_yoffset = 0
{0xC85C, 0x02D0,2}, 		//cam_crop_window_width = 720
{0xC85E, 0x0218,2}, 		//cam_crop_window_height = 536
{0xC86C, 0x0140,2}, 		//cam_output_width = 320
{0xC86E, 0x00F0,2}, 		//cam_output_height = 240
{0xC870, 0x40D0,2}, 		//cam_output_format = 16592
{0xC87C, 0x00,1}, 		//cam_aet_aemode = 0
{0xC88E, 0x1C00,2}, 		//cam_aet_max_frame_rate = 7168
{0xC890, 0x1C00,2}, 		//cam_aet_min_frame_rate = 7168
{0xC94C, 0x0000,2}, 		//cam_stat_awb_clip_window_xstart = 0
{0xC94E, 0x0000,2}, 		//cam_stat_awb_clip_window_ystart = 0
{0xC950, 0x013F,2}, 		//cam_stat_awb_clip_window_xend = 319
{0xC952, 0x00EF,2}, 		//cam_stat_awb_clip_window_yend = 239
{0xC954, 0x0000,2}, 		//cam_stat_ae_initial_window_xstart = 0
{0xC956, 0x0000,2}, 		//cam_stat_ae_initial_window_ystart = 0
{0xC958, 0x003F,2}, 		//cam_stat_ae_initial_window_xend = 63
{0xC95A, 0x002F,2}, 		//cam_stat_ae_initial_window_yend = 47
};

#elif defined(FEATURE_TW_CAMERA_FPS26_MCLK12_PCLK92_V)

#if defined(FEATURE_TW_CAMERA_NO_SET_PLL)

struct as0260_reg as0260_1080_reg[] =
{

	{0x098E, 0xC800,2},      // LOGICAL_ADDRESS_ACCESS [CAM_SYSCTL_PLL_ENABLE]

	//[Timing_settings]
	{0xC800, 0x0020,2}, 		//cam_sensor_cfg_y_addr_start = 32
	{0xC802, 0x0020,2}, 		//cam_sensor_cfg_x_addr_start = 32
	{0xC804, 0x045F,2}, 		//cam_sensor_cfg_y_addr_end = 1119
	{0xC806, 0x07A7,2}, 		//cam_sensor_cfg_x_addr_end = 1959
	{0xC808, 0x035FE1D8,4}, 		//cam_sensor_cfg_pixclk = 56615384
	{0xC80C, 0x0001,2}, 		//cam_sensor_cfg_row_speed = 1
	{0xC80E, 0x0336,2}, 		//cam_sensor_cfg_fine_integ_time_min = 822
	{0xC810, 0x0CCF,2}, 		//cam_sensor_cfg_fine_integ_time_max = 3279
	{0xC812, 0x0491,2}, 		//cam_sensor_cfg_frame_length_lines = 1169
	{0xC814, 0x0E8D,2}, 		//cam_sensor_cfg_line_length_pck = 3725
	{0xC816, 0x00D4,2}, 		//cam_sensor_cfg_fine_correction = 212
	{0xC818, 0x043B,2}, 		//cam_sensor_cfg_cpipe_last_row = 1083
	{0xC820, 0x0010,2}, 		//cam_sensor_cfg_reg_0_data = 16
	{0xC830, 0x0002,2}, 		//cam_sensor_control_read_mode = 2
	{0xC858, 0x0000,2}, 		//cam_crop_window_xoffset = 0
	{0xC85A, 0x0000,2}, 		//cam_crop_window_yoffset = 0
	{0xC85C, 0x0780,2}, 		//cam_crop_window_width = 1920
	{0xC85E, 0x0438,2}, 		//cam_crop_window_height = 1080
	{0xC86C, 0x0780,2}, 		//cam_output_width = 1920
	{0xC86E, 0x0438,2}, 		//cam_output_height = 1080
	{0xC870, 0x4010,2}, 		//cam_output_format = 16592
	{0xC87C, 0x00,1}, 		//cam_aet_aemode = 0
	{0xC88E, 0x1A00,2}, 		//cam_aet_max_frame_rate = 6656
	{0xC890, 0x1A00,2}, 		//cam_aet_min_frame_rate = 6656
	{0xC94C, 0x0000,2}, 		//cam_stat_awb_clip_window_xstart = 0
	{0xC94E, 0x0000,2}, 		//cam_stat_awb_clip_window_ystart = 0
	{0xC950, 0x077F,2}, 		//cam_stat_awb_clip_window_xend = 1919
	{0xC952, 0x0437,2}, 		//cam_stat_awb_clip_window_yend = 1079
	{0xC954, 0x0000,2}, 		//cam_stat_ae_initial_window_xstart = 0
	{0xC956, 0x0000,2}, 		//cam_stat_ae_initial_window_ystart = 0
	{0xC958, 0x017F,2}, 		//cam_stat_ae_initial_window_xend = 383
	{0xC95A, 0x00D7,2}, 		//cam_stat_ae_initial_window_yend = 215
};

struct as0260_reg as0260_1440_reg[] =
{

	{0x098E, 0xC800,2},      // LOGICAL_ADDRESS_ACCESS [CAM_SYSCTL_PLL_ENABLE]
	//[Timing_settings]
	{0xC800, 0x0020,2}, 		//cam_sensor_cfg_y_addr_start = 32  //\uc218\uc815 Start >>
	{0xC802, 0x0120,2}, 		//cam_sensor_cfg_x_addr_start = 288 
	{0xC804, 0x045F,2}, 		//cam_sensor_cfg_y_addr_end = 1119  
	{0xC806, 0x06C7,2}, 		//cam_sensor_cfg_x_addr_end = 1735  //\uc218\uc815 end>
	{0xC808, 0x035FE1D8,4}, 		//cam_sensor_cfg_pixclk = 56615384
	{0xC80C, 0x0001,2}, 		//cam_sensor_cfg_row_speed = 1
	{0xC80E, 0x0336,2}, 		//cam_sensor_cfg_fine_integ_time_min = 822
	{0xC810, 0x0CCF,2}, 		//cam_sensor_cfg_fine_integ_time_max = 3279
	{0xC812, 0x0491,2}, 		//cam_sensor_cfg_frame_length_lines = 1169
	{0xC814, 0x0E8D,2}, 		//cam_sensor_cfg_line_length_pck = 3725
	{0xC816, 0x00D4,2}, 		//cam_sensor_cfg_fine_correction = 212
	{0xC818, 0x043B,2}, 		//cam_sensor_cfg_cpipe_last_row = 1083
	{0xC820, 0x0010,2}, 		//cam_sensor_cfg_reg_0_data = 16
	{0xC830, 0x0002,2}, 		//cam_sensor_control_read_mode = 2
	{0xC858, 0x0000,2}, 		//cam_crop_window_xoffset = 0
	{0xC85A, 0x0000,2}, 		//cam_crop_window_yoffset = 0
	{0xC85C, 0x05A0,2}, 		//cam_crop_window_width = 1440   //\uc218\uc815 Start >>
	{0xC85E, 0x0438,2}, 		//cam_crop_window_height = 1080  
	{0xC86C, 0x05A0,2}, 		//cam_output_width = 1440        
	{0xC86E, 0x0438,2}, 		//cam_output_height = 1080       
	{0xC870, 0x4010,2},	//cam_output_format = 16592        //\uc218\uc815 end >>
	{0xC87C, 0x00,1}, 		//cam_aet_aemode = 0
	{0xC88E, 0x1A00,2}, 		//cam_aet_max_frame_rate = 6656
	{0xC890, 0x1A00,2}, 		//cam_aet_min_frame_rate = 6656
	{0xC94C, 0x0000,2}, 		//cam_stat_awb_clip_window_xstart = 0
	{0xC94E, 0x0000,2}, 		//cam_stat_awb_clip_window_ystart = 0
	{0xC950, 0x05a0,2}, 		//cam_stat_awb_clip_window_xend = 1919  //\uc218\uc815 Start >>
	{0xC952, 0x0438,2}, 		//cam_stat_awb_clip_window_yend = 1079
	{0xC954, 0x0000,2}, 		// CAM_STAT_AE_INITIAL_WINDOW_XSTART
	{0xC956, 0x0000,2}, 		// CAM_STAT_AE_INITIAL_WINDOW_YSTART
	{0xC958, 0x0120,2}, 		// CAM_STAT_AE_INITIAL_WINDOW_XEND  
	{0xC95A, 0x00D8,2}, 		// CAM_STAT_AE_INITIAL_WINDOW_YEND   //\uc218\uc815 end >>;	
};

struct as0260_reg as0260_960_reg[] =
{

	{0x098E, 0xC800,2},      // LOGICAL_ADDRESS_ACCESS [CAM_SYSCTL_PLL_ENABLE]

	//[Timing_settings]
	{0xC800, 0x0020,2}, 		//cam_sensor_cfg_y_addr_start = 32
	{0xC802, 0x0120,2}, 		//cam_sensor_cfg_x_addr_start = 288
	{0xC804, 0x045F,2}, 		//cam_sensor_cfg_y_addr_end = 1119
	{0xC806, 0x06C7,2}, 		//cam_sensor_cfg_x_addr_end = 1735
	{0xC808, 0x035FE1D8,4}, 		//cam_sensor_cfg_pixclk = 56615384
	{0xC80C, 0x0001,2}, 		//cam_sensor_cfg_row_speed = 1
	{0xC80E, 0x0336,2}, 		//cam_sensor_cfg_fine_integ_time_min = 822
	{0xC810, 0x0CCF,2}, 		//cam_sensor_cfg_fine_integ_time_max = 3279
	{0xC812, 0x0491,2}, 		//cam_sensor_cfg_frame_length_lines = 1169
	{0xC814, 0x0E8D,2}, 		//cam_sensor_cfg_line_length_pck = 3725
	{0xC816, 0x00D4,2}, 		//cam_sensor_cfg_fine_correction = 212
	{0xC818, 0x043B,2}, 		//cam_sensor_cfg_cpipe_last_row = 1083
	{0xC820, 0x0010,2}, 		//cam_sensor_cfg_reg_0_data = 16
	{0xC830, 0x0002,2}, 		//cam_sensor_control_read_mode = 2
	{0xC858, 0x0000,2}, 		//cam_crop_window_xoffset = 0
	{0xC85A, 0x0000,2}, 		//cam_crop_window_yoffset = 0
	{0xC85C, 0x05A0,2}, 		//cam_crop_window_width = 1440
	{0xC85E, 0x0438,2}, 		//cam_crop_window_height = 1080
	{0xC86C, 0x0500,2}, 		//cam_output_width = 1280
	{0xC86E, 0x03C0,2}, 		//cam_output_height = 960
	{0xC870, 0x4010,2}, 		//cam_output_format = 16592
	{0xC87C, 0x00,1}, 		//cam_aet_aemode = 0
	{0xC88E, 0x1A00,2}, 		//cam_aet_max_frame_rate = 6656
	{0xC890, 0x1A00,2}, 		//cam_aet_min_frame_rate = 6656
	{0xC94C, 0x0000,2}, 		//cam_stat_awb_clip_window_xstart = 0
	{0xC94E, 0x0000,2}, 		//cam_stat_awb_clip_window_ystart = 0
	{0xC950, 0x04FF,2}, 		//cam_stat_awb_clip_window_xend = 1279
	{0xC952, 0x03BF,2}, 		//cam_stat_awb_clip_window_yend = 959
	{0xC954, 0x0000,2}, 		//cam_stat_ae_initial_window_xstart = 0
	{0xC956, 0x0000,2}, 		//cam_stat_ae_initial_window_ystart = 0
	{0xC958, 0x00FF,2}, 		//cam_stat_ae_initial_window_xend = 255
	{0xC95A, 0x00BF,2}, 		//cam_stat_ae_initial_window_yend = 191
};	

struct as0260_reg as0260_720_reg[] =
{

	{0x098E, 0xC800,2},      // LOGICAL_ADDRESS_ACCESS [CAM_SYSCTL_PLL_ENABLE]

	//[Timing_settings]
	{0xC800, 0x0020,2}, 		//cam_sensor_cfg_y_addr_start = 32
	{0xC802, 0x0020,2}, 		//cam_sensor_cfg_x_addr_start = 32
	{0xC804, 0x045F,2}, 		//cam_sensor_cfg_y_addr_end = 1119
	{0xC806, 0x07A7,2}, 		//cam_sensor_cfg_x_addr_end = 1959
	{0xC808, 0x035FE1D8,4}, 		//cam_sensor_cfg_pixclk = 56615384
	{0xC80C, 0x0001,2}, 		//cam_sensor_cfg_row_speed = 1
	{0xC80E, 0x0336,2}, 		//cam_sensor_cfg_fine_integ_time_min = 822
	{0xC810, 0x0CCF,2}, 		//cam_sensor_cfg_fine_integ_time_max = 3279
	{0xC812, 0x0491,2}, 		//cam_sensor_cfg_frame_length_lines = 1169
	{0xC814, 0x0E8D,2}, 		//cam_sensor_cfg_line_length_pck = 3725
	{0xC816, 0x00D4,2}, 		//cam_sensor_cfg_fine_correction = 212
	{0xC818, 0x043B,2}, 		//cam_sensor_cfg_cpipe_last_row = 1083
	{0xC820, 0x0010,2}, 		//cam_sensor_cfg_reg_0_data = 16
	{0xC830, 0x0002,2}, 		//cam_sensor_control_read_mode = 2
	{0xC858, 0x0000,2}, 		//cam_crop_window_xoffset = 0
	{0xC85A, 0x0000,2}, 		//cam_crop_window_yoffset = 0
	{0xC85C, 0x0780,2}, 		//cam_crop_window_width = 1920
	{0xC85E, 0x0438,2}, 		//cam_crop_window_height = 1080
	{0xC86C, 0x0500,2}, 		//cam_output_width = 1280
	{0xC86E, 0x02D0,2}, 		//cam_output_height = 720
	{0xC870, 0x4010,2}, 		//cam_output_format = 16592
	{0xC87C, 0x00,1}, 		//cam_aet_aemode = 0
	{0xC88E, 0x1A00,2}, 		//cam_aet_max_frame_rate = 6656
	{0xC890, 0x1A00,2}, 		//cam_aet_min_frame_rate = 6656
	{0xC94C, 0x0000,2}, 		//cam_stat_awb_clip_window_xstart = 0
	{0xC94E, 0x0000,2}, 		//cam_stat_awb_clip_window_ystart = 0
	{0xC950, 0x04FF,2}, 		//cam_stat_awb_clip_window_xend = 1279
	{0xC952, 0x02CF,2}, 		//cam_stat_awb_clip_window_yend = 719
	{0xC954, 0x0000,2}, 		//cam_stat_ae_initial_window_xstart = 0
	{0xC956, 0x0000,2}, 		//cam_stat_ae_initial_window_ystart = 0
	{0xC958, 0x00FF,2}, 		//cam_stat_ae_initial_window_xend = 255
	{0xC95A, 0x008F,2}, 		//cam_stat_ae_initial_window_yend = 143
};	

struct as0260_reg as0260_640_reg[] =
{
	{0x098E, 0xC800,2},      // LOGICAL_ADDRESS_ACCESS [CAM_SYSCTL_PLL_ENABLE]

	//[Timing_settings]
	{0xC800, 0x0020,2}, 		//cam_sensor_cfg_y_addr_start = 32
	{0xC802, 0x0100,2}, 		//cam_sensor_cfg_x_addr_start = 256
	{0xC804, 0x045D,2}, 		//cam_sensor_cfg_y_addr_end = 1117
	{0xC806, 0x06AD,2}, 		//cam_sensor_cfg_x_addr_end = 1709
	{0xC808, 0x035FE1D8,4}, 		//cam_sensor_cfg_pixclk = 56615384
	{0xC80C, 0x0001,2}, 		//cam_sensor_cfg_row_speed = 1
	{0xC80E, 0x06A4,2}, 		//cam_sensor_cfg_fine_integ_time_min = 1700
	{0xC810, 0x1842,2}, 		//cam_sensor_cfg_fine_integ_time_max = 6210
	{0xC812, 0x026D,2}, 		//cam_sensor_cfg_frame_length_lines = 621
	{0xC814, 0x1B64,2}, 		//cam_sensor_cfg_line_length_pck = 7012
	{0xC816, 0x01D9,2}, 		//cam_sensor_cfg_fine_correction = 473
	{0xC818, 0x021B,2}, 		//cam_sensor_cfg_cpipe_last_row = 539
	{0xC820, 0x0010,2}, 		//cam_sensor_cfg_reg_0_data = 16
	{0xC830, 0x0012,2}, 		//cam_sensor_control_read_mode = 18
	{0xC858, 0x0000,2}, 		//cam_crop_window_xoffset = 0
	{0xC85A, 0x0000,2}, 		//cam_crop_window_yoffset = 0
	{0xC85C, 0x02D0,2}, 		//cam_crop_window_width = 720
	{0xC85E, 0x0218,2}, 		//cam_crop_window_height = 536
	{0xC86C, 0x0280,2}, 		//cam_output_width = 640
	{0xC86E, 0x01E0,2}, 		//cam_output_height = 480
	{0xC870, 0x4010,2}, 		//cam_output_format = 16592
	{0xC87C, 0x00,1}, 		//cam_aet_aemode = 0
	{0xC88E, 0x1A00,2}, 		//cam_aet_max_frame_rate = 6656
	{0xC890, 0x1A00,2}, 		//cam_aet_min_frame_rate = 6656
	{0xC94C, 0x0000,2}, 		//cam_stat_awb_clip_window_xstart = 0
	{0xC94E, 0x0000,2}, 		//cam_stat_awb_clip_window_ystart = 0
	{0xC950, 0x027F,2}, 		//cam_stat_awb_clip_window_xend = 639
	{0xC952, 0x01DF,2}, 		//cam_stat_awb_clip_window_yend = 479
	{0xC954, 0x0000,2}, 		//cam_stat_ae_initial_window_xstart = 0
	{0xC956, 0x0000,2}, 		//cam_stat_ae_initial_window_ystart = 0
	{0xC958, 0x007F,2}, 		//cam_stat_ae_initial_window_xend = 127
	{0xC95A, 0x005F,2}, 		//cam_stat_ae_initial_window_yend = 95

};	

struct as0260_reg as0260_320_reg[] =
{
	{0x098E, 0xC800,2},      // LOGICAL_ADDRESS_ACCESS [CAM_SYSCTL_PLL_ENABLE]

	//[Timing_settings]
	{0xC800, 0x0020,2}, 		//cam_sensor_cfg_y_addr_start = 32
	{0xC802, 0x0100,2}, 		//cam_sensor_cfg_x_addr_start = 256
	{0xC804, 0x045D,2}, 		//cam_sensor_cfg_y_addr_end = 1117
	{0xC806, 0x06AD,2}, 		//cam_sensor_cfg_x_addr_end = 1709
	{0xC808, 0x035FE1D8,4}, 		//cam_sensor_cfg_pixclk = 56615384
	{0xC80C, 0x0001,2}, 		//cam_sensor_cfg_row_speed = 1
	{0xC80E, 0x06A4,2}, 		//cam_sensor_cfg_fine_integ_time_min = 1700
	{0xC810, 0x1842,2}, 		//cam_sensor_cfg_fine_integ_time_max = 6210
	{0xC812, 0x026D,2}, 		//cam_sensor_cfg_frame_length_lines = 621
	{0xC814, 0x1B64,2}, 		//cam_sensor_cfg_line_length_pck = 7012
	{0xC816, 0x01D9,2}, 		//cam_sensor_cfg_fine_correction = 473
	{0xC818, 0x021B,2}, 		//cam_sensor_cfg_cpipe_last_row = 539
	{0xC820, 0x0010,2}, 		//cam_sensor_cfg_reg_0_data = 16
	{0xC830, 0x0012,2}, 		//cam_sensor_control_read_mode = 18
	{0xC858, 0x0000,2}, 		//cam_crop_window_xoffset = 0
	{0xC85A, 0x0000,2}, 		//cam_crop_window_yoffset = 0
	{0xC85C, 0x02D0,2}, 		//cam_crop_window_width = 720
	{0xC85E, 0x0218,2}, 		//cam_crop_window_height = 536
	{0xC86C, 0x0140,2}, 		//cam_output_width = 320
	{0xC86E, 0x00F0,2}, 		//cam_output_height = 240
	{0xC870, 0x4010,2}, 		//cam_output_format = 16592
	{0xC87C, 0x00,1}, 		//cam_aet_aemode = 0
	{0xC88E, 0x1A00,2}, 		//cam_aet_max_frame_rate = 6656
	{0xC890, 0x1A00,2}, 		//cam_aet_min_frame_rate = 6656
	{0xC94C, 0x0000,2}, 		//cam_stat_awb_clip_window_xstart = 0
	{0xC94E, 0x0000,2}, 		//cam_stat_awb_clip_window_ystart = 0
	{0xC950, 0x013F,2}, 		//cam_stat_awb_clip_window_xend = 319
	{0xC952, 0x00EF,2}, 		//cam_stat_awb_clip_window_yend = 239
	{0xC954, 0x0000,2}, 		//cam_stat_ae_initial_window_xstart = 0
	{0xC956, 0x0000,2}, 		//cam_stat_ae_initial_window_ystart = 0
	{0xC958, 0x003F,2}, 		//cam_stat_ae_initial_window_xend = 63
	{0xC95A, 0x002F,2}, 		//cam_stat_ae_initial_window_yend = 47
};	


#endif

struct as0260_reg as0260_1080[] =
{
#if  defined(FEATURE_CAMERA_INIT)
	{0x098E, 0xCA12,2},      // LOGICAL_ADDRESS_ACCESS [CAM_SYSCTL_PLL_ENABLE]
	{0xCA12, 0x01,1},      // CAM_SYSCTL_PLL_ENABLE
	{0xCA13, 0x00,1},      // CAM_SYSCTL_SENSOR_CLK_DIV2_EN

	{0xCA14, 0x025C,2}, 		//cam_sysctl_pll_divider_m_n = 604
	{0xCA16, 0x0070,2}, 		//cam_sysctl_pll_divider_p = 112
	{0xCA18, 0x7F7C,2}, 		//cam_sysctl_pll_divider_p4_p5_p6 = 32636
	{0xCA1C, 0x8043,2}, 		//cam_port_output_control = 32835
	{0xCA1E, 0x0008,2}, 		//cam_port_porch = 8
	{0xCA20, 0x0C00,2}, 		//cam_port_mipi_timing_t_hs_zero = 3072
	{0xCA22, 0x0006,2}, 		//cam_port_mipi_timing_t_hs_exit_hs_trail = 6
	{0xCA24, 0x0B01,2}, 		//cam_port_mipi_timing_t_clk_post_clk_pre = 2817
	{0xCA26, 0x0517,2}, 		//cam_port_mipi_timing_t_clk_trail_clk_zero = 1303
	{0xCA28, 0x0005,2}, 		//cam_port_mipi_timing_t_lpx = 5
	{0xCA2A, 0x0809,2}, 		//cam_port_mipi_timing_init_timing = 2057
	{0xCA2C, 0x0000,2}, 		//cam_port_mipi_timing_t_hs_pre = 0
#else
	{0x098E, 0xC800,2},      // LOGICAL_ADDRESS_ACCESS [CAM_SYSCTL_PLL_ENABLE]
#endif
	//[Timing_settings]
	{0xC800, 0x0020,2}, 		//cam_sensor_cfg_y_addr_start = 32
	{0xC802, 0x0020,2}, 		//cam_sensor_cfg_x_addr_start = 32
	{0xC804, 0x045F,2}, 		//cam_sensor_cfg_y_addr_end = 1119
	{0xC806, 0x07A7,2}, 		//cam_sensor_cfg_x_addr_end = 1959
	{0xC808, 0x035FE1D8,4}, 		//cam_sensor_cfg_pixclk = 56615384
	{0xC80C, 0x0001,2}, 		//cam_sensor_cfg_row_speed = 1
	{0xC80E, 0x0336,2}, 		//cam_sensor_cfg_fine_integ_time_min = 822
	{0xC810, 0x0CCF,2}, 		//cam_sensor_cfg_fine_integ_time_max = 3279
	{0xC812, 0x0491,2}, 		//cam_sensor_cfg_frame_length_lines = 1169
	{0xC814, 0x0E8D,2}, 		//cam_sensor_cfg_line_length_pck = 3725
	{0xC816, 0x00D4,2}, 		//cam_sensor_cfg_fine_correction = 212
	{0xC818, 0x043B,2}, 		//cam_sensor_cfg_cpipe_last_row = 1083
	{0xC820, 0x0010,2}, 		//cam_sensor_cfg_reg_0_data = 16
	{0xC830, 0x0002,2}, 		//cam_sensor_control_read_mode = 2
	{0xC858, 0x0000,2}, 		//cam_crop_window_xoffset = 0
	{0xC85A, 0x0000,2}, 		//cam_crop_window_yoffset = 0
	{0xC85C, 0x0780,2}, 		//cam_crop_window_width = 1920
	{0xC85E, 0x0438,2}, 		//cam_crop_window_height = 1080
	{0xC86C, 0x0780,2}, 		//cam_output_width = 1920
	{0xC86E, 0x0438,2}, 		//cam_output_height = 1080
	{0xC870, 0x4010,2}, 		//cam_output_format = 16592
	{0xC87C, 0x00,1}, 		//cam_aet_aemode = 0
	{0xC88E, 0x1A00,2}, 		//cam_aet_max_frame_rate = 6656
	{0xC890, 0x1A00,2}, 		//cam_aet_min_frame_rate = 6656
	{0xC94C, 0x0000,2}, 		//cam_stat_awb_clip_window_xstart = 0
	{0xC94E, 0x0000,2}, 		//cam_stat_awb_clip_window_ystart = 0
	{0xC950, 0x077F,2}, 		//cam_stat_awb_clip_window_xend = 1919
	{0xC952, 0x0437,2}, 		//cam_stat_awb_clip_window_yend = 1079
	{0xC954, 0x0000,2}, 		//cam_stat_ae_initial_window_xstart = 0
	{0xC956, 0x0000,2}, 		//cam_stat_ae_initial_window_ystart = 0
	{0xC958, 0x017F,2}, 		//cam_stat_ae_initial_window_xend = 383
	{0xC95A, 0x00D7,2}, 		//cam_stat_ae_initial_window_yend = 215
};

struct as0260_reg as0260_1440[] =
{
#if  defined(FEATURE_CAMERA_INIT)
	{0x098E, 0xCA12,2},      // LOGICAL_ADDRESS_ACCESS [CAM_SYSCTL_PLL_ENABLE]
	{0xCA12, 0x01,1},      // CAM_SYSCTL_PLL_ENABLE
	{0xCA13, 0x00,1},      // CAM_SYSCTL_SENSOR_CLK_DIV2_EN

	{0xCA14, 0x025C,2}, 		//cam_sysctl_pll_divider_m_n = 604
	{0xCA16, 0x0070,2}, 		//cam_sysctl_pll_divider_p = 112
	{0xCA18, 0x7F7C,2}, 		//cam_sysctl_pll_divider_p4_p5_p6 = 32636
	{0xCA1C, 0x8043,2}, 		//cam_port_output_control = 32835
	{0xCA1E, 0x0008,2}, 		//cam_port_porch = 8
	{0xCA20, 0x0C00,2}, 		//cam_port_mipi_timing_t_hs_zero = 3072
	{0xCA22, 0x0006,2}, 		//cam_port_mipi_timing_t_hs_exit_hs_trail = 6
	{0xCA24, 0x0B01,2}, 		//cam_port_mipi_timing_t_clk_post_clk_pre = 2817
	{0xCA26, 0x0517,2}, 		//cam_port_mipi_timing_t_clk_trail_clk_zero = 1303
	{0xCA28, 0x0005,2}, 		//cam_port_mipi_timing_t_lpx = 5
	{0xCA2A, 0x0809,2}, 		//cam_port_mipi_timing_init_timing = 2057
	{0xCA2C, 0x0000,2}, 		//cam_port_mipi_timing_t_hs_pre = 0
#else
	{0x098E, 0xC800,2},      // LOGICAL_ADDRESS_ACCESS [CAM_SYSCTL_PLL_ENABLE]
#endif
	//[Timing_settings]
	{0xC800, 0x0020,2}, 		//cam_sensor_cfg_y_addr_start = 32  //\uc218\uc815 Start >>
	{0xC802, 0x0120,2}, 		//cam_sensor_cfg_x_addr_start = 288 
	{0xC804, 0x045F,2}, 		//cam_sensor_cfg_y_addr_end = 1119  
	{0xC806, 0x06C7,2}, 		//cam_sensor_cfg_x_addr_end = 1735  //\uc218\uc815 end>
	{0xC808, 0x035FE1D8,4}, 		//cam_sensor_cfg_pixclk = 56615384
	{0xC80C, 0x0001,2}, 		//cam_sensor_cfg_row_speed = 1
	{0xC80E, 0x0336,2}, 		//cam_sensor_cfg_fine_integ_time_min = 822
	{0xC810, 0x0CCF,2}, 		//cam_sensor_cfg_fine_integ_time_max = 3279
	{0xC812, 0x0491,2}, 		//cam_sensor_cfg_frame_length_lines = 1169
	{0xC814, 0x0E8D,2}, 		//cam_sensor_cfg_line_length_pck = 3725
	{0xC816, 0x00D4,2}, 		//cam_sensor_cfg_fine_correction = 212
	{0xC818, 0x043B,2}, 		//cam_sensor_cfg_cpipe_last_row = 1083
	{0xC820, 0x0010,2}, 		//cam_sensor_cfg_reg_0_data = 16
	{0xC830, 0x0002,2}, 		//cam_sensor_control_read_mode = 2
	{0xC858, 0x0000,2}, 		//cam_crop_window_xoffset = 0
	{0xC85A, 0x0000,2}, 		//cam_crop_window_yoffset = 0
	{0xC85C, 0x05A0,2}, 		//cam_crop_window_width = 1440   //\uc218\uc815 Start >>
	{0xC85E, 0x0438,2}, 		//cam_crop_window_height = 1080  
	{0xC86C, 0x05A0,2}, 		//cam_output_width = 1440        
	{0xC86E, 0x0438,2}, 		//cam_output_height = 1080       
	{0xC870, 0x4010,2},	//cam_output_format = 16592        //\uc218\uc815 end >>
	{0xC87C, 0x00,1}, 		//cam_aet_aemode = 0
	{0xC88E, 0x1A00,2}, 		//cam_aet_max_frame_rate = 6656
	{0xC890, 0x1A00,2}, 		//cam_aet_min_frame_rate = 6656
	{0xC94C, 0x0000,2}, 		//cam_stat_awb_clip_window_xstart = 0
	{0xC94E, 0x0000,2}, 		//cam_stat_awb_clip_window_ystart = 0
	{0xC950, 0x05a0,2}, 		//cam_stat_awb_clip_window_xend = 1919  //\uc218\uc815 Start >>
	{0xC952, 0x0438,2}, 		//cam_stat_awb_clip_window_yend = 1079
	{0xC954, 0x0000,2}, 		// CAM_STAT_AE_INITIAL_WINDOW_XSTART
	{0xC956, 0x0000,2}, 		// CAM_STAT_AE_INITIAL_WINDOW_YSTART
	{0xC958, 0x0120,2}, 		// CAM_STAT_AE_INITIAL_WINDOW_XEND  
	{0xC95A, 0x00D8,2}, 		// CAM_STAT_AE_INITIAL_WINDOW_YEND   //\uc218\uc815 end >>;	
};

struct as0260_reg as0260_960[] =
{
#if  defined(FEATURE_CAMERA_INIT)
	{0x098E, 0xCA12,2},      // LOGICAL_ADDRESS_ACCESS [CAM_SYSCTL_PLL_ENABLE]
	{0xCA12, 0x01,1},      // CAM_SYSCTL_PLL_ENABLE
	{0xCA13, 0x00,1},      // CAM_SYSCTL_SENSOR_CLK_DIV2_EN

	{0xCA14, 0x025C,2}, 		//cam_sysctl_pll_divider_m_n = 604
	{0xCA16, 0x0070,2}, 		//cam_sysctl_pll_divider_p = 112
	{0xCA18, 0x7F7C,2}, 		//cam_sysctl_pll_divider_p4_p5_p6 = 32636
	{0xCA1C, 0x8043,2}, 		//cam_port_output_control = 32835
	{0xCA1E, 0x0008,2}, 		//cam_port_porch = 8
	{0xCA20, 0x0C00,2}, 		//cam_port_mipi_timing_t_hs_zero = 3072
	{0xCA22, 0x0006,2}, 		//cam_port_mipi_timing_t_hs_exit_hs_trail = 6
	{0xCA24, 0x0B01,2}, 		//cam_port_mipi_timing_t_clk_post_clk_pre = 2817
	{0xCA26, 0x0517,2}, 		//cam_port_mipi_timing_t_clk_trail_clk_zero = 1303
	{0xCA28, 0x0005,2}, 		//cam_port_mipi_timing_t_lpx = 5
	{0xCA2A, 0x0809,2}, 		//cam_port_mipi_timing_init_timing = 2057
	{0xCA2C, 0x0000,2}, 		//cam_port_mipi_timing_t_hs_pre = 0
#else
	{0x098E, 0xC800,2},      // LOGICAL_ADDRESS_ACCESS [CAM_SYSCTL_PLL_ENABLE]
#endif


	//[Timing_settings]
	{0xC800, 0x0020,2}, 		//cam_sensor_cfg_y_addr_start = 32
	{0xC802, 0x0120,2}, 		//cam_sensor_cfg_x_addr_start = 288
	{0xC804, 0x045F,2}, 		//cam_sensor_cfg_y_addr_end = 1119
	{0xC806, 0x06C7,2}, 		//cam_sensor_cfg_x_addr_end = 1735
	{0xC808, 0x035FE1D8,4}, 		//cam_sensor_cfg_pixclk = 56615384
	{0xC80C, 0x0001,2}, 		//cam_sensor_cfg_row_speed = 1
	{0xC80E, 0x0336,2}, 		//cam_sensor_cfg_fine_integ_time_min = 822
	{0xC810, 0x0CCF,2}, 		//cam_sensor_cfg_fine_integ_time_max = 3279
	{0xC812, 0x0491,2}, 		//cam_sensor_cfg_frame_length_lines = 1169
	{0xC814, 0x0E8D,2}, 		//cam_sensor_cfg_line_length_pck = 3725
	{0xC816, 0x00D4,2}, 		//cam_sensor_cfg_fine_correction = 212
	{0xC818, 0x043B,2}, 		//cam_sensor_cfg_cpipe_last_row = 1083
	{0xC820, 0x0010,2}, 		//cam_sensor_cfg_reg_0_data = 16
	{0xC830, 0x0002,2}, 		//cam_sensor_control_read_mode = 2
	{0xC858, 0x0000,2}, 		//cam_crop_window_xoffset = 0
	{0xC85A, 0x0000,2}, 		//cam_crop_window_yoffset = 0
	{0xC85C, 0x05A0,2}, 		//cam_crop_window_width = 1440
	{0xC85E, 0x0438,2}, 		//cam_crop_window_height = 1080
	{0xC86C, 0x0500,2}, 		//cam_output_width = 1280
	{0xC86E, 0x03C0,2}, 		//cam_output_height = 960
	{0xC870, 0x4010,2}, 		//cam_output_format = 16592
	{0xC87C, 0x00,1}, 		//cam_aet_aemode = 0
	{0xC88E, 0x1A00,2}, 		//cam_aet_max_frame_rate = 6656
	{0xC890, 0x1A00,2}, 		//cam_aet_min_frame_rate = 6656
	{0xC94C, 0x0000,2}, 		//cam_stat_awb_clip_window_xstart = 0
	{0xC94E, 0x0000,2}, 		//cam_stat_awb_clip_window_ystart = 0
	{0xC950, 0x04FF,2}, 		//cam_stat_awb_clip_window_xend = 1279
	{0xC952, 0x03BF,2}, 		//cam_stat_awb_clip_window_yend = 959
	{0xC954, 0x0000,2}, 		//cam_stat_ae_initial_window_xstart = 0
	{0xC956, 0x0000,2}, 		//cam_stat_ae_initial_window_ystart = 0
	{0xC958, 0x00FF,2}, 		//cam_stat_ae_initial_window_xend = 255
	{0xC95A, 0x00BF,2}, 		//cam_stat_ae_initial_window_yend = 191
};	

struct as0260_reg as0260_720[] =
{
#if  defined(FEATURE_CAMERA_INIT)
	{0x098E, 0xCA12,2},      // LOGICAL_ADDRESS_ACCESS [CAM_SYSCTL_PLL_ENABLE]
	{0xCA12, 0x01,1},      // CAM_SYSCTL_PLL_ENABLE
	{0xCA13, 0x00,1},      // CAM_SYSCTL_SENSOR_CLK_DIV2_EN

	{0xCA14, 0x025C,2}, 		//cam_sysctl_pll_divider_m_n = 604
	{0xCA16, 0x0070,2}, 		//cam_sysctl_pll_divider_p = 112
	{0xCA18, 0x7F7C,2}, 		//cam_sysctl_pll_divider_p4_p5_p6 = 32636
	{0xCA1C, 0x8043,2}, 		//cam_port_output_control = 32835
	{0xCA1E, 0x0008,2}, 		//cam_port_porch = 8
	{0xCA20, 0x0C00,2}, 		//cam_port_mipi_timing_t_hs_zero = 3072
	{0xCA22, 0x0006,2}, 		//cam_port_mipi_timing_t_hs_exit_hs_trail = 6
	{0xCA24, 0x0B01,2}, 		//cam_port_mipi_timing_t_clk_post_clk_pre = 2817
	{0xCA26, 0x0517,2}, 		//cam_port_mipi_timing_t_clk_trail_clk_zero = 1303
	{0xCA28, 0x0005,2}, 		//cam_port_mipi_timing_t_lpx = 5
	{0xCA2A, 0x0809,2}, 		//cam_port_mipi_timing_init_timing = 2057
	{0xCA2C, 0x0000,2}, 		//cam_port_mipi_timing_t_hs_pre = 0
#else
	{0x098E, 0xC800,2},      // LOGICAL_ADDRESS_ACCESS [CAM_SYSCTL_PLL_ENABLE]
#endif


	//[Timing_settings]
	{0xC800, 0x0020,2}, 		//cam_sensor_cfg_y_addr_start = 32
	{0xC802, 0x0020,2}, 		//cam_sensor_cfg_x_addr_start = 32
	{0xC804, 0x045F,2}, 		//cam_sensor_cfg_y_addr_end = 1119
	{0xC806, 0x07A7,2}, 		//cam_sensor_cfg_x_addr_end = 1959
	{0xC808, 0x035FE1D8,4}, 		//cam_sensor_cfg_pixclk = 56615384
	{0xC80C, 0x0001,2}, 		//cam_sensor_cfg_row_speed = 1
	{0xC80E, 0x0336,2}, 		//cam_sensor_cfg_fine_integ_time_min = 822
	{0xC810, 0x0CCF,2}, 		//cam_sensor_cfg_fine_integ_time_max = 3279
	{0xC812, 0x0491,2}, 		//cam_sensor_cfg_frame_length_lines = 1169
	{0xC814, 0x0E8D,2}, 		//cam_sensor_cfg_line_length_pck = 3725
	{0xC816, 0x00D4,2}, 		//cam_sensor_cfg_fine_correction = 212
	{0xC818, 0x043B,2}, 		//cam_sensor_cfg_cpipe_last_row = 1083
	{0xC820, 0x0010,2}, 		//cam_sensor_cfg_reg_0_data = 16
	{0xC830, 0x0002,2}, 		//cam_sensor_control_read_mode = 2
	{0xC858, 0x0000,2}, 		//cam_crop_window_xoffset = 0
	{0xC85A, 0x0000,2}, 		//cam_crop_window_yoffset = 0
	{0xC85C, 0x0780,2}, 		//cam_crop_window_width = 1920
	{0xC85E, 0x0438,2}, 		//cam_crop_window_height = 1080
	{0xC86C, 0x0500,2}, 		//cam_output_width = 1280
	{0xC86E, 0x02D0,2}, 		//cam_output_height = 720
	{0xC870, 0x4010,2}, 		//cam_output_format = 16592
	{0xC87C, 0x00,1}, 		//cam_aet_aemode = 0
	{0xC88E, 0x1A00,2}, 		//cam_aet_max_frame_rate = 6656
	{0xC890, 0x1A00,2}, 		//cam_aet_min_frame_rate = 6656
	{0xC94C, 0x0000,2}, 		//cam_stat_awb_clip_window_xstart = 0
	{0xC94E, 0x0000,2}, 		//cam_stat_awb_clip_window_ystart = 0
	{0xC950, 0x04FF,2}, 		//cam_stat_awb_clip_window_xend = 1279
	{0xC952, 0x02CF,2}, 		//cam_stat_awb_clip_window_yend = 719
	{0xC954, 0x0000,2}, 		//cam_stat_ae_initial_window_xstart = 0
	{0xC956, 0x0000,2}, 		//cam_stat_ae_initial_window_ystart = 0
	{0xC958, 0x00FF,2}, 		//cam_stat_ae_initial_window_xend = 255
	{0xC95A, 0x008F,2}, 		//cam_stat_ae_initial_window_yend = 143
};	

struct as0260_reg as0260_640[] =
{
#if  defined(FEATURE_CAMERA_INIT)
	{0x098E, 0xCA12,2},      // LOGICAL_ADDRESS_ACCESS [CAM_SYSCTL_PLL_ENABLE]
	{0xCA12, 0x01,1},      // CAM_SYSCTL_PLL_ENABLE
	{0xCA13, 0x00,1},      // CAM_SYSCTL_SENSOR_CLK_DIV2_EN

	{0xCA14, 0x025C,2}, 		//cam_sysctl_pll_divider_m_n = 604
	{0xCA16, 0x0070,2}, 		//cam_sysctl_pll_divider_p = 112
	{0xCA18, 0x7F7C,2}, 		//cam_sysctl_pll_divider_p4_p5_p6 = 32636
	{0xCA1C, 0x8043,2}, 		//cam_port_output_control = 32835
	{0xCA1E, 0x0008,2}, 		//cam_port_porch = 8
	{0xCA20, 0x0C00,2}, 		//cam_port_mipi_timing_t_hs_zero = 3072
	{0xCA22, 0x0006,2}, 		//cam_port_mipi_timing_t_hs_exit_hs_trail = 6
	{0xCA24, 0x0B01,2}, 		//cam_port_mipi_timing_t_clk_post_clk_pre = 2817
	{0xCA26, 0x0517,2}, 		//cam_port_mipi_timing_t_clk_trail_clk_zero = 1303
	{0xCA28, 0x0005,2}, 		//cam_port_mipi_timing_t_lpx = 5
	{0xCA2A, 0x0809,2}, 		//cam_port_mipi_timing_init_timing = 2057
	{0xCA2C, 0x0000,2}, 		//cam_port_mipi_timing_t_hs_pre = 0
#else
	{0x098E, 0xC800,2},      // LOGICAL_ADDRESS_ACCESS [CAM_SYSCTL_PLL_ENABLE]
#endif


//[Timing_settings]
{0xC800, 0x0020,2}, 		//cam_sensor_cfg_y_addr_start = 32
{0xC802, 0x0100,2}, 		//cam_sensor_cfg_x_addr_start = 256
{0xC804, 0x045D,2}, 		//cam_sensor_cfg_y_addr_end = 1117
{0xC806, 0x06AD,2}, 		//cam_sensor_cfg_x_addr_end = 1709
{0xC808, 0x035FE1D8,4}, 		//cam_sensor_cfg_pixclk = 56615384
{0xC80C, 0x0001,2}, 		//cam_sensor_cfg_row_speed = 1
{0xC80E, 0x06A4,2}, 		//cam_sensor_cfg_fine_integ_time_min = 1700
{0xC810, 0x1842,2}, 		//cam_sensor_cfg_fine_integ_time_max = 6210
{0xC812, 0x026D,2}, 		//cam_sensor_cfg_frame_length_lines = 621
{0xC814, 0x1B64,2}, 		//cam_sensor_cfg_line_length_pck = 7012
{0xC816, 0x01D9,2}, 		//cam_sensor_cfg_fine_correction = 473
{0xC818, 0x021B,2}, 		//cam_sensor_cfg_cpipe_last_row = 539
{0xC820, 0x0010,2}, 		//cam_sensor_cfg_reg_0_data = 16
{0xC830, 0x0012,2}, 		//cam_sensor_control_read_mode = 18
{0xC858, 0x0000,2}, 		//cam_crop_window_xoffset = 0
{0xC85A, 0x0000,2}, 		//cam_crop_window_yoffset = 0
{0xC85C, 0x02D0,2}, 		//cam_crop_window_width = 720
{0xC85E, 0x0218,2}, 		//cam_crop_window_height = 536
{0xC86C, 0x0280,2}, 		//cam_output_width = 640
{0xC86E, 0x01E0,2}, 		//cam_output_height = 480
{0xC870, 0x4010,2}, 		//cam_output_format = 16592
{0xC87C, 0x00,1}, 		//cam_aet_aemode = 0
{0xC88E, 0x1A00,2}, 		//cam_aet_max_frame_rate = 6656
{0xC890, 0x1A00,2}, 		//cam_aet_min_frame_rate = 6656
{0xC94C, 0x0000,2}, 		//cam_stat_awb_clip_window_xstart = 0
{0xC94E, 0x0000,2}, 		//cam_stat_awb_clip_window_ystart = 0
{0xC950, 0x027F,2}, 		//cam_stat_awb_clip_window_xend = 639
{0xC952, 0x01DF,2}, 		//cam_stat_awb_clip_window_yend = 479
{0xC954, 0x0000,2}, 		//cam_stat_ae_initial_window_xstart = 0
{0xC956, 0x0000,2}, 		//cam_stat_ae_initial_window_ystart = 0
{0xC958, 0x007F,2}, 		//cam_stat_ae_initial_window_xend = 127
{0xC95A, 0x005F,2}, 		//cam_stat_ae_initial_window_yend = 95

};	

struct as0260_reg as0260_320[] =
{
#if  defined(FEATURE_CAMERA_INIT)
	{0x098E, 0xCA12,2},      // LOGICAL_ADDRESS_ACCESS [CAM_SYSCTL_PLL_ENABLE]
	{0xCA12, 0x01,1},      // CAM_SYSCTL_PLL_ENABLE
	{0xCA13, 0x00,1},      // CAM_SYSCTL_SENSOR_CLK_DIV2_EN

	{0xCA14, 0x025C,2}, 		//cam_sysctl_pll_divider_m_n = 604
	{0xCA16, 0x0070,2}, 		//cam_sysctl_pll_divider_p = 112
	{0xCA18, 0x7F7C,2}, 		//cam_sysctl_pll_divider_p4_p5_p6 = 32636
	{0xCA1C, 0x8043,2}, 		//cam_port_output_control = 32835
	{0xCA1E, 0x0008,2}, 		//cam_port_porch = 8
	{0xCA20, 0x0C00,2}, 		//cam_port_mipi_timing_t_hs_zero = 3072
	{0xCA22, 0x0006,2}, 		//cam_port_mipi_timing_t_hs_exit_hs_trail = 6
	{0xCA24, 0x0B01,2}, 		//cam_port_mipi_timing_t_clk_post_clk_pre = 2817
	{0xCA26, 0x0517,2}, 		//cam_port_mipi_timing_t_clk_trail_clk_zero = 1303
	{0xCA28, 0x0005,2}, 		//cam_port_mipi_timing_t_lpx = 5
	{0xCA2A, 0x0809,2}, 		//cam_port_mipi_timing_init_timing = 2057
	{0xCA2C, 0x0000,2}, 		//cam_port_mipi_timing_t_hs_pre = 0
#else
	{0x098E, 0xC800,2},      // LOGICAL_ADDRESS_ACCESS [CAM_SYSCTL_PLL_ENABLE]
#endif


//[Timing_settings]
{0xC800, 0x0020,2}, 		//cam_sensor_cfg_y_addr_start = 32
{0xC802, 0x0100,2}, 		//cam_sensor_cfg_x_addr_start = 256
{0xC804, 0x045D,2}, 		//cam_sensor_cfg_y_addr_end = 1117
{0xC806, 0x06AD,2}, 		//cam_sensor_cfg_x_addr_end = 1709
{0xC808, 0x035FE1D8,4}, 		//cam_sensor_cfg_pixclk = 56615384
{0xC80C, 0x0001,2}, 		//cam_sensor_cfg_row_speed = 1
{0xC80E, 0x06A4,2}, 		//cam_sensor_cfg_fine_integ_time_min = 1700
{0xC810, 0x1842,2}, 		//cam_sensor_cfg_fine_integ_time_max = 6210
{0xC812, 0x026D,2}, 		//cam_sensor_cfg_frame_length_lines = 621
{0xC814, 0x1B64,2}, 		//cam_sensor_cfg_line_length_pck = 7012
{0xC816, 0x01D9,2}, 		//cam_sensor_cfg_fine_correction = 473
{0xC818, 0x021B,2}, 		//cam_sensor_cfg_cpipe_last_row = 539
{0xC820, 0x0010,2}, 		//cam_sensor_cfg_reg_0_data = 16
{0xC830, 0x0012,2}, 		//cam_sensor_control_read_mode = 18
{0xC858, 0x0000,2}, 		//cam_crop_window_xoffset = 0
{0xC85A, 0x0000,2}, 		//cam_crop_window_yoffset = 0
{0xC85C, 0x02D0,2}, 		//cam_crop_window_width = 720
{0xC85E, 0x0218,2}, 		//cam_crop_window_height = 536
{0xC86C, 0x0140,2}, 		//cam_output_width = 320
{0xC86E, 0x00F0,2}, 		//cam_output_height = 240
{0xC870, 0x4010,2}, 		//cam_output_format = 16592
{0xC87C, 0x00,1}, 		//cam_aet_aemode = 0
{0xC88E, 0x1A00,2}, 		//cam_aet_max_frame_rate = 6656
{0xC890, 0x1A00,2}, 		//cam_aet_min_frame_rate = 6656
{0xC94C, 0x0000,2}, 		//cam_stat_awb_clip_window_xstart = 0
{0xC94E, 0x0000,2}, 		//cam_stat_awb_clip_window_ystart = 0
{0xC950, 0x013F,2}, 		//cam_stat_awb_clip_window_xend = 319
{0xC952, 0x00EF,2}, 		//cam_stat_awb_clip_window_yend = 239
{0xC954, 0x0000,2}, 		//cam_stat_ae_initial_window_xstart = 0
{0xC956, 0x0000,2}, 		//cam_stat_ae_initial_window_ystart = 0
{0xC958, 0x003F,2}, 		//cam_stat_ae_initial_window_xend = 63
{0xC95A, 0x002F,2}, 		//cam_stat_ae_initial_window_yend = 47
};	

#elif defined(FEATURE_TW_CAMERA_FPS30_MCLK24_PCLK96_V)
struct as0260_reg as0260_1080[] =
{
	{0x098E, 0xCA12,2},      // LOGICAL_ADDRESS_ACCESS [CAM_SYSCTL_PLL_ENABLE]
	{0xCA12, 0x01,1},      // CAM_SYSCTL_PLL_ENABLE
	{0xCA13, 0x00,1},      // CAM_SYSCTL_SENSOR_CLK_DIV2_EN

	{0xCA14, 0x0010,2}, 		//cam_sysctl_pll_divider_m_n = 16
	{0xCA16, 0x0070,2}, 		//cam_sysctl_pll_divider_p = 112
	{0xCA18, 0x7F7C,2}, 		//cam_sysctl_pll_divider_p4_p5_p6 = 32636
	{0xCA1C, 0x8040,2}, 		//cam_port_output_control = 32832
	{0xCA1E, 0x0005,2}, 		//cam_port_porch = 5
	{0xCA20, 0x0F00,2}, 		//cam_port_mipi_timing_t_hs_zero = 3840
	{0xCA22, 0x0B07,2}, 		//cam_port_mipi_timing_t_hs_exit_hs_trail = 2823
	{0xCA24, 0x0D01,2}, 		//cam_port_mipi_timing_t_clk_post_clk_pre = 3329
	{0xCA26, 0x071D,2}, 		//cam_port_mipi_timing_t_clk_trail_clk_zero = 1821
	{0xCA28, 0x0006,2}, 		//cam_port_mipi_timing_t_lpx = 6
	{0xCA2A, 0x0A0C,2}, 		//cam_port_mipi_timing_init_timing = 2572
	{0xCA2C, 0x0000,2}, 		//cam_port_mipi_timing_t_hs_pre = 0


	//[Timing_settings]
	{0xC800, 0x0020,2}, 		//cam_sensor_cfg_y_addr_start = 32
	{0xC802, 0x0020,2}, 		//cam_sensor_cfg_x_addr_start = 32
	{0xC804, 0x045F,2}, 		//cam_sensor_cfg_y_addr_end = 1119
	{0xC806, 0x07A7,2}, 		//cam_sensor_cfg_x_addr_end = 1959
	{0xC808, 0x0385713B,4}, 		//cam_sensor_cfg_pixclk = 59076923
	{0xC80C, 0x0001,2}, 		//cam_sensor_cfg_row_speed = 1
	{0xC80E, 0x0336,2}, 		//cam_sensor_cfg_fine_integ_time_min = 822
	{0xC810, 0x0B6B,2}, 		//cam_sensor_cfg_fine_integ_time_max = 2923
	{0xC812, 0x0491,2}, 		//cam_sensor_cfg_frame_length_lines = 1169
	{0xC814, 0x0D29,2}, 		//cam_sensor_cfg_line_length_pck = 3369
	{0xC816, 0x00D4,2}, 		//cam_sensor_cfg_fine_correction = 212
	{0xC818, 0x043B,2}, 		//cam_sensor_cfg_cpipe_last_row = 1083
	{0xC820, 0x0010,2}, 		//cam_sensor_cfg_reg_0_data = 16
	{0xC830, 0x0002,2}, 		//cam_sensor_control_read_mode = 2
	{0xC858, 0x0000,2}, 		//cam_crop_window_xoffset = 0
	{0xC85A, 0x0000,2}, 		//cam_crop_window_yoffset = 0
	{0xC85C, 0x0780,2}, 		//cam_crop_window_width = 1920
	{0xC85E, 0x0438,2}, 		//cam_crop_window_height = 1080
	{0xC86C, 0x0780,2}, 		//cam_output_width = 1920
	{0xC86E, 0x0438,2}, 		//cam_output_height = 1080
	{0xC870, 0x4010,2}, 		//cam_output_format = 16592
	{0xC87C, 0x00,1}, 		//cam_aet_aemode = 0
	{0xC88E, 0x1E00,2}, 		//cam_aet_max_frame_rate = 7680
	{0xC890, 0x1E00,2}, 		//cam_aet_min_frame_rate = 7680
	{0xC94C, 0x0000,2}, 		//cam_stat_awb_clip_window_xstart = 0
	{0xC94E, 0x0000,2}, 		//cam_stat_awb_clip_window_ystart = 0
	{0xC950, 0x077F,2}, 		//cam_stat_awb_clip_window_xend = 1919
	{0xC952, 0x0437,2}, 		//cam_stat_awb_clip_window_yend = 1079
	{0xC954, 0x0000,2}, 		//cam_stat_ae_initial_window_xstart = 0
	{0xC956, 0x0000,2}, 		//cam_stat_ae_initial_window_ystart = 0
	{0xC958, 0x017F,2}, 		//cam_stat_ae_initial_window_xend = 383
	{0xC95A, 0x00D7,2}, 		//cam_stat_ae_initial_window_yend = 215

};

struct as0260_reg as0260_1440[] =
{
	{0x098E, 0xCA12,2},      // LOGICAL_ADDRESS_ACCESS [CAM_SYSCTL_PLL_ENABLE]
	{0xCA12, 0x01,1},      // CAM_SYSCTL_PLL_ENABLE
	{0xCA13, 0x00,1},      // CAM_SYSCTL_SENSOR_CLK_DIV2_EN

	{0xCA14, 0x0010,2}, 		//cam_sysctl_pll_divider_m_n = 16
	{0xCA16, 0x0070,2}, 		//cam_sysctl_pll_divider_p = 112
	{0xCA18, 0x7F7C,2}, 		//cam_sysctl_pll_divider_p4_p5_p6 = 32636
	{0xCA1C, 0x8040,2}, 		//cam_port_output_control = 32832
	{0xCA1E, 0x0005,2}, 		//cam_port_porch = 5
	{0xCA20, 0x0F00,2}, 		//cam_port_mipi_timing_t_hs_zero = 3840
	{0xCA22, 0x0B07,2}, 		//cam_port_mipi_timing_t_hs_exit_hs_trail = 2823
	{0xCA24, 0x0D01,2}, 		//cam_port_mipi_timing_t_clk_post_clk_pre = 3329
	{0xCA26, 0x071D,2}, 		//cam_port_mipi_timing_t_clk_trail_clk_zero = 1821
	{0xCA28, 0x0006,2}, 		//cam_port_mipi_timing_t_lpx = 6
	{0xCA2A, 0x0A0C,2}, 		//cam_port_mipi_timing_init_timing = 2572
	{0xCA2C, 0x0000,2}, 		//cam_port_mipi_timing_t_hs_pre = 0


	//[Timing_settings]
	{0xC800, 0x0020,2}, 		//cam_sensor_cfg_y_addr_start = 32  //\uc218\uc815 Start >>
	{0xC802, 0x0120,2}, 		//cam_sensor_cfg_x_addr_start = 288 
	{0xC804, 0x045F,2}, 		//cam_sensor_cfg_y_addr_end = 1119  
	{0xC806, 0x06C7,2}, 		//cam_sensor_cfg_x_addr_end = 1735  //\uc218\uc815 end>
	{0xC808, 0x0385713B,4}, 		//cam_sensor_cfg_pixclk = 59076923
	{0xC80C, 0x0001,2}, 		//cam_sensor_cfg_row_speed = 1
	{0xC80E, 0x0336,2}, 		//cam_sensor_cfg_fine_integ_time_min = 822
	{0xC810, 0x0B6B,2}, 		//cam_sensor_cfg_fine_integ_time_max = 2923
	{0xC812, 0x0491,2}, 		//cam_sensor_cfg_frame_length_lines = 1169
	{0xC814, 0x0D29,2}, 		//cam_sensor_cfg_line_length_pck = 3369
	{0xC816, 0x00D4,2}, 		//cam_sensor_cfg_fine_correction = 212
	{0xC818, 0x043B,2}, 		//cam_sensor_cfg_cpipe_last_row = 1083
	{0xC820, 0x0010,2}, 		//cam_sensor_cfg_reg_0_data = 16
	{0xC830, 0x0002,2}, 		//cam_sensor_control_read_mode = 2
	{0xC858, 0x0000,2}, 		//cam_crop_window_xoffset = 0
	{0xC85A, 0x0000,2}, 		//cam_crop_window_yoffset = 0
	{0xC85C, 0x05A0,2}, 		//cam_crop_window_width = 1440   //\uc218\uc815 Start >>
	{0xC85E, 0x0438,2}, 		//cam_crop_window_height = 1080  
	{0xC86C, 0x05A0,2}, 		//cam_output_width = 1440        
	{0xC86E, 0x0438,2}, 		//cam_output_height = 1080       
	{0xC870, 0x4010,2},	//cam_output_format = 16592        //\uc218\uc815 end >>
	{0xC87C, 0x00,1}, 		//cam_aet_aemode = 0
	{0xC88E, 0x1E00,2}, 		//cam_aet_max_frame_rate = 7680
	{0xC890, 0x1E00,2}, 		//cam_aet_min_frame_rate = 7680
	{0xC94C, 0x0000,2}, 		//cam_stat_awb_clip_window_xstart = 0
	{0xC94E, 0x0000,2}, 		//cam_stat_awb_clip_window_ystart = 0
	{0xC950, 0x05a0,2}, 		//cam_stat_awb_clip_window_xend = 1919  //\uc218\uc815 Start >>
	{0xC952, 0x0438,2}, 		//cam_stat_awb_clip_window_yend = 1079
	{0xC954, 0x0000,2}, 		// CAM_STAT_AE_INITIAL_WINDOW_XSTART
	{0xC956, 0x0000,2}, 		// CAM_STAT_AE_INITIAL_WINDOW_YSTART
	{0xC958, 0x0120,2}, 		// CAM_STAT_AE_INITIAL_WINDOW_XEND  
	{0xC95A, 0x00D8,2}, 		// CAM_STAT_AE_INITIAL_WINDOW_YEND   //\uc218\uc815 end >>
};

struct as0260_reg as0260_960[] =
{
	{0x098E, 0xCA12,2},      // LOGICAL_ADDRESS_ACCESS [CAM_SYSCTL_PLL_ENABLE]
	{0xCA12, 0x01,1},      // CAM_SYSCTL_PLL_ENABLE
	{0xCA13, 0x00,1},      // CAM_SYSCTL_SENSOR_CLK_DIV2_EN

	{0xCA14, 0x0010,2}, 		//cam_sysctl_pll_divider_m_n = 16
	{0xCA16, 0x0070,2}, 		//cam_sysctl_pll_divider_p = 112
	{0xCA18, 0x7F7C,2}, 		//cam_sysctl_pll_divider_p4_p5_p6 = 32636
	{0xCA1C, 0x8040,2}, 		//cam_port_output_control = 32832
	{0xCA1E, 0x0005,2}, 		//cam_port_porch = 5
	{0xCA20, 0x0F00,2}, 		//cam_port_mipi_timing_t_hs_zero = 3840
	{0xCA22, 0x0B07,2}, 		//cam_port_mipi_timing_t_hs_exit_hs_trail = 2823
	{0xCA24, 0x0D01,2}, 		//cam_port_mipi_timing_t_clk_post_clk_pre = 3329
	{0xCA26, 0x071D,2}, 		//cam_port_mipi_timing_t_clk_trail_clk_zero = 1821
	{0xCA28, 0x0006,2}, 		//cam_port_mipi_timing_t_lpx = 6
	{0xCA2A, 0x0A0C,2}, 		//cam_port_mipi_timing_init_timing = 2572
	{0xCA2C, 0x0000,2}, 		//cam_port_mipi_timing_t_hs_pre = 0


	//[Timing_settings]
	{0xC800, 0x0020,2}, 		//cam_sensor_cfg_y_addr_start = 32
	{0xC802, 0x0120,2}, 		//cam_sensor_cfg_x_addr_start = 288
	{0xC804, 0x045F,2}, 		//cam_sensor_cfg_y_addr_end = 1119
	{0xC806, 0x06C7,2}, 		//cam_sensor_cfg_x_addr_end = 1735
	{0xC808, 0x0385713B,4}, 		//cam_sensor_cfg_pixclk = 59076923
	{0xC80C, 0x0001,2}, 		//cam_sensor_cfg_row_speed = 1
	{0xC80E, 0x0336,2}, 		//cam_sensor_cfg_fine_integ_time_min = 822
	{0xC810, 0x0B6B,2}, 		//cam_sensor_cfg_fine_integ_time_max = 2923
	{0xC812, 0x0491,2}, 		//cam_sensor_cfg_frame_length_lines = 1169
	{0xC814, 0x0D29,2}, 		//cam_sensor_cfg_line_length_pck = 3369
	{0xC816, 0x00D4,2}, 		//cam_sensor_cfg_fine_correction = 212
	{0xC818, 0x043B,2}, 		//cam_sensor_cfg_cpipe_last_row = 1083
	{0xC820, 0x0010,2}, 		//cam_sensor_cfg_reg_0_data = 16
	{0xC830, 0x0002,2}, 		//cam_sensor_control_read_mode = 2
	{0xC858, 0x0000,2}, 		//cam_crop_window_xoffset = 0
	{0xC85A, 0x0000,2}, 		//cam_crop_window_yoffset = 0
	{0xC85C, 0x05A0,2}, 		//cam_crop_window_width = 1440
	{0xC85E, 0x0438,2}, 		//cam_crop_window_height = 1080
	{0xC86C, 0x0500,2}, 		//cam_output_width = 1280
	{0xC86E, 0x03C0,2}, 		//cam_output_height = 960
	{0xC870, 0x4010,2}, 		//cam_output_format = 16592
	{0xC87C, 0x00,1}, 		//cam_aet_aemode = 0
	{0xC88E, 0x1E00,2}, 		//cam_aet_max_frame_rate = 7680
	{0xC890, 0x1E00,2}, 		//cam_aet_min_frame_rate = 7680
	{0xC94C, 0x0000,2}, 		//cam_stat_awb_clip_window_xstart = 0
	{0xC94E, 0x0000,2}, 		//cam_stat_awb_clip_window_ystart = 0
	{0xC950, 0x04FF,2}, 		//cam_stat_awb_clip_window_xend = 1279
	{0xC952, 0x03BF,2}, 		//cam_stat_awb_clip_window_yend = 959
	{0xC954, 0x0000,2}, 		//cam_stat_ae_initial_window_xstart = 0
	{0xC956, 0x0000,2}, 		//cam_stat_ae_initial_window_ystart = 0
	{0xC958, 0x00FF,2}, 		//cam_stat_ae_initial_window_xend = 255
	{0xC95A, 0x00BF,2}, 		//cam_stat_ae_initial_window_yend = 191
};

struct as0260_reg as0260_720[] =
{
	{0x098E, 0xCA12,2},      // LOGICAL_ADDRESS_ACCESS [CAM_SYSCTL_PLL_ENABLE]
	{0xCA12, 0x01,1},      // CAM_SYSCTL_PLL_ENABLE
	{0xCA13, 0x00,1},      // CAM_SYSCTL_SENSOR_CLK_DIV2_EN

	{0xCA14, 0x0010,2}, 		//cam_sysctl_pll_divider_m_n = 16
	{0xCA16, 0x0070,2}, 		//cam_sysctl_pll_divider_p = 112
	{0xCA18, 0x7F7C,2}, 		//cam_sysctl_pll_divider_p4_p5_p6 = 32636
	{0xCA1C, 0x8040,2}, 		//cam_port_output_control = 32832
	{0xCA1E, 0x0005,2}, 		//cam_port_porch = 5
	{0xCA20, 0x0F00,2}, 		//cam_port_mipi_timing_t_hs_zero = 3840
	{0xCA22, 0x0B07,2}, 		//cam_port_mipi_timing_t_hs_exit_hs_trail = 2823
	{0xCA24, 0x0D01,2}, 		//cam_port_mipi_timing_t_clk_post_clk_pre = 3329
	{0xCA26, 0x071D,2}, 		//cam_port_mipi_timing_t_clk_trail_clk_zero = 1821
	{0xCA28, 0x0006,2}, 		//cam_port_mipi_timing_t_lpx = 6
	{0xCA2A, 0x0A0C,2}, 		//cam_port_mipi_timing_init_timing = 2572
	{0xCA2C, 0x0000,2}, 		//cam_port_mipi_timing_t_hs_pre = 0


	//[Timing_settings]
	{0xC800, 0x0020,2}, 		//cam_sensor_cfg_y_addr_start = 32
	{0xC802, 0x0020,2}, 		//cam_sensor_cfg_x_addr_start = 32
	{0xC804, 0x045F,2}, 		//cam_sensor_cfg_y_addr_end = 1119
	{0xC806, 0x07A7,2}, 		//cam_sensor_cfg_x_addr_end = 1959
	{0xC808, 0x0385713B,4}, 		//cam_sensor_cfg_pixclk = 59076923
	{0xC80C, 0x0001,2}, 		//cam_sensor_cfg_row_speed = 1
	{0xC80E, 0x0336,2}, 		//cam_sensor_cfg_fine_integ_time_min = 822
	{0xC810, 0x0B6B,2}, 		//cam_sensor_cfg_fine_integ_time_max = 2923
	{0xC812, 0x0491,2}, 		//cam_sensor_cfg_frame_length_lines = 1169
	{0xC814, 0x0D29,2}, 		//cam_sensor_cfg_line_length_pck = 3369
	{0xC816, 0x00D4,2}, 		//cam_sensor_cfg_fine_correction = 212
	{0xC818, 0x043B,2}, 		//cam_sensor_cfg_cpipe_last_row = 1083
	{0xC820, 0x0010,2}, 		//cam_sensor_cfg_reg_0_data = 16
	{0xC830, 0x0002,2}, 		//cam_sensor_control_read_mode = 2
	{0xC858, 0x0000,2}, 		//cam_crop_window_xoffset = 0
	{0xC85A, 0x0000,2}, 		//cam_crop_window_yoffset = 0
	{0xC85C, 0x0780,2}, 		//cam_crop_window_width = 1920
	{0xC85E, 0x0438,2}, 		//cam_crop_window_height = 1080
	{0xC86C, 0x0500,2}, 		//cam_output_width = 1280
	{0xC86E, 0x02D0,2}, 		//cam_output_height = 720
	{0xC870, 0x4010,2}, 		//cam_output_format = 16592
	{0xC87C, 0x00,1}, 		//cam_aet_aemode = 0
	{0xC88E, 0x1E00,2}, 		//cam_aet_max_frame_rate = 7680
	{0xC890, 0x1E00,2}, 		//cam_aet_min_frame_rate = 7680
	{0xC94C, 0x0000,2}, 		//cam_stat_awb_clip_window_xstart = 0
	{0xC94E, 0x0000,2}, 		//cam_stat_awb_clip_window_ystart = 0
	{0xC950, 0x04FF,2}, 		//cam_stat_awb_clip_window_xend = 1279
	{0xC952, 0x02CF,2}, 		//cam_stat_awb_clip_window_yend = 719
	{0xC954, 0x0000,2}, 		//cam_stat_ae_initial_window_xstart = 0
	{0xC956, 0x0000,2}, 		//cam_stat_ae_initial_window_ystart = 0
	{0xC958, 0x00FF,2}, 		//cam_stat_ae_initial_window_xend = 255
	{0xC95A, 0x008F,2}, 		//cam_stat_ae_initial_window_yend = 143
};	

struct as0260_reg as0260_640[] =
{
	{0x098E, 0xCA12,2},      // LOGICAL_ADDRESS_ACCESS [CAM_SYSCTL_PLL_ENABLE]
	{0xCA12, 0x01,1},      // CAM_SYSCTL_PLL_ENABLE
	{0xCA13, 0x00,1},      // CAM_SYSCTL_SENSOR_CLK_DIV2_EN

	{0xCA14, 0x0010,2}, 		//cam_sysctl_pll_divider_m_n = 16
	{0xCA16, 0x0070,2}, 		//cam_sysctl_pll_divider_p = 112
	{0xCA18, 0x7F7C,2}, 		//cam_sysctl_pll_divider_p4_p5_p6 = 32636
	{0xCA1C, 0x8040,2}, 		//cam_port_output_control = 32832
	{0xCA1E, 0x0005,2}, 		//cam_port_porch = 5
	{0xCA20, 0x0F00,2}, 		//cam_port_mipi_timing_t_hs_zero = 3840
	{0xCA22, 0x0B07,2}, 		//cam_port_mipi_timing_t_hs_exit_hs_trail = 2823
	{0xCA24, 0x0D01,2}, 		//cam_port_mipi_timing_t_clk_post_clk_pre = 3329
	{0xCA26, 0x071D,2}, 		//cam_port_mipi_timing_t_clk_trail_clk_zero = 1821
	{0xCA28, 0x0006,2}, 		//cam_port_mipi_timing_t_lpx = 6
	{0xCA2A, 0x0A0C,2}, 		//cam_port_mipi_timing_init_timing = 2572
	{0xCA2C, 0x0000,2}, 		//cam_port_mipi_timing_t_hs_pre = 0


	//[Timing_settings]
	{0xC800, 0x0020,2}, 		//cam_sensor_cfg_y_addr_start = 32
	{0xC802, 0x0100,2}, 		//cam_sensor_cfg_x_addr_start = 256
	{0xC804, 0x045D,2}, 		//cam_sensor_cfg_y_addr_end = 1117
	{0xC806, 0x06AD,2}, 		//cam_sensor_cfg_x_addr_end = 1709
	{0xC808, 0x0385713B,4}, 		//cam_sensor_cfg_pixclk = 59076923
	{0xC80C, 0x0001,2}, 		//cam_sensor_cfg_row_speed = 1
	{0xC80E, 0x06A4,2}, 		//cam_sensor_cfg_fine_integ_time_min = 1700
	{0xC810, 0x15A4,2}, 		//cam_sensor_cfg_fine_integ_time_max = 5540
	{0xC812, 0x026D,2}, 		//cam_sensor_cfg_frame_length_lines = 621
	{0xC814, 0x18C6,2}, 		//cam_sensor_cfg_line_length_pck = 6342
	{0xC816, 0x01D9,2}, 		//cam_sensor_cfg_fine_correction = 473
	{0xC818, 0x021B,2}, 		//cam_sensor_cfg_cpipe_last_row = 539
	{0xC820, 0x0010,2}, 		//cam_sensor_cfg_reg_0_data = 16
	{0xC830, 0x0012,2}, 		//cam_sensor_control_read_mode = 18
	{0xC858, 0x0000,2}, 		//cam_crop_window_xoffset = 0
	{0xC85A, 0x0000,2}, 		//cam_crop_window_yoffset = 0
	{0xC85C, 0x02D0,2}, 		//cam_crop_window_width = 720
	{0xC85E, 0x0218,2}, 		//cam_crop_window_height = 536
	{0xC86C, 0x0280,2}, 		//cam_output_width = 640
	{0xC86E, 0x01E0,2}, 		//cam_output_height = 480
	{0xC870, 0x4010,2}, 		//cam_output_format = 16592
	{0xC87C, 0x00,1}, 		//cam_aet_aemode = 0
	{0xC88E, 0x1E00,2}, 		//cam_aet_max_frame_rate = 7680
	{0xC890, 0x1E00,2}, 		//cam_aet_min_frame_rate = 7680
	{0xC94C, 0x0000,2}, 		//cam_stat_awb_clip_window_xstart = 0
	{0xC94E, 0x0000,2}, 		//cam_stat_awb_clip_window_ystart = 0
	{0xC950, 0x027F,2}, 		//cam_stat_awb_clip_window_xend = 639
	{0xC952, 0x01DF,2}, 		//cam_stat_awb_clip_window_yend = 479
	{0xC954, 0x0000,2}, 		//cam_stat_ae_initial_window_xstart = 0
	{0xC956, 0x0000,2}, 		//cam_stat_ae_initial_window_ystart = 0
	{0xC958, 0x007F,2}, 		//cam_stat_ae_initial_window_xend = 127
	{0xC95A, 0x005F,2}, 		//cam_stat_ae_initial_window_yend = 95

};

struct as0260_reg as0260_320[] =
{
	{0x098E, 0xCA12,2},      // LOGICAL_ADDRESS_ACCESS [CAM_SYSCTL_PLL_ENABLE]
	{0xCA12, 0x01,1},      // CAM_SYSCTL_PLL_ENABLE
	{0xCA13, 0x00,1},      // CAM_SYSCTL_SENSOR_CLK_DIV2_EN

	{0xCA14, 0x0010,2}, 		//cam_sysctl_pll_divider_m_n = 16
	{0xCA16, 0x0070,2}, 		//cam_sysctl_pll_divider_p = 112
	{0xCA18, 0x7F7C,2}, 		//cam_sysctl_pll_divider_p4_p5_p6 = 32636
	{0xCA1C, 0x8040,2}, 		//cam_port_output_control = 32832
	{0xCA1E, 0x0005,2}, 		//cam_port_porch = 5
	{0xCA20, 0x0F00,2}, 		//cam_port_mipi_timing_t_hs_zero = 3840
	{0xCA22, 0x0B07,2}, 		//cam_port_mipi_timing_t_hs_exit_hs_trail = 2823
	{0xCA24, 0x0D01,2}, 		//cam_port_mipi_timing_t_clk_post_clk_pre = 3329
	{0xCA26, 0x071D,2}, 		//cam_port_mipi_timing_t_clk_trail_clk_zero = 1821
	{0xCA28, 0x0006,2}, 		//cam_port_mipi_timing_t_lpx = 6
	{0xCA2A, 0x0A0C,2}, 		//cam_port_mipi_timing_init_timing = 2572
	{0xCA2C, 0x0000,2}, 		//cam_port_mipi_timing_t_hs_pre = 0


	//[Timing_settings]
	{0xC800, 0x0020,2}, 		//cam_sensor_cfg_y_addr_start = 32
	{0xC802, 0x0100,2}, 		//cam_sensor_cfg_x_addr_start = 256
	{0xC804, 0x045D,2}, 		//cam_sensor_cfg_y_addr_end = 1117
	{0xC806, 0x06AD,2}, 		//cam_sensor_cfg_x_addr_end = 1709
	{0xC808, 0x0385713B,4}, 		//cam_sensor_cfg_pixclk = 59076923
	{0xC80C, 0x0001,2}, 		//cam_sensor_cfg_row_speed = 1
	{0xC80E, 0x06A4,2}, 		//cam_sensor_cfg_fine_integ_time_min = 1700
	{0xC810, 0x15A4,2}, 		//cam_sensor_cfg_fine_integ_time_max = 5540
	{0xC812, 0x026D,2}, 		//cam_sensor_cfg_frame_length_lines = 621
	{0xC814, 0x18C6,2}, 		//cam_sensor_cfg_line_length_pck = 6342
	{0xC816, 0x01D9,2}, 		//cam_sensor_cfg_fine_correction = 473
	{0xC818, 0x021B,2}, 		//cam_sensor_cfg_cpipe_last_row = 539
	{0xC820, 0x0010,2}, 		//cam_sensor_cfg_reg_0_data = 16
	{0xC830, 0x0012,2}, 		//cam_sensor_control_read_mode = 18
	{0xC858, 0x0000,2}, 		//cam_crop_window_xoffset = 0
	{0xC85A, 0x0000,2}, 		//cam_crop_window_yoffset = 0
	{0xC85C, 0x02D0,2}, 		//cam_crop_window_width = 720
	{0xC85E, 0x0218,2}, 		//cam_crop_window_height = 536
	{0xC86C, 0x0140,2}, 		//cam_output_width = 320
	{0xC86E, 0x00F0,2}, 		//cam_output_height = 240
	{0xC870, 0x4010,2}, 		//cam_output_format = 16592
	{0xC87C, 0x00,1}, 		//cam_aet_aemode = 0
	{0xC88E, 0x1E00,2}, 		//cam_aet_max_frame_rate = 7680
	{0xC890, 0x1E00,2}, 		//cam_aet_min_frame_rate = 7680
	{0xC94C, 0x0000,2}, 		//cam_stat_awb_clip_window_xstart = 0
	{0xC94E, 0x0000,2}, 		//cam_stat_awb_clip_window_ystart = 0
	{0xC950, 0x013F,2}, 		//cam_stat_awb_clip_window_xend = 319
	{0xC952, 0x00EF,2}, 		//cam_stat_awb_clip_window_yend = 239
	{0xC954, 0x0000,2}, 		//cam_stat_ae_initial_window_xstart = 0
	{0xC956, 0x0000,2}, 		//cam_stat_ae_initial_window_ystart = 0
	{0xC958, 0x003F,2}, 		//cam_stat_ae_initial_window_xend = 63
	{0xC95A, 0x002F,2}, 		//cam_stat_ae_initial_window_yend = 47

};

#elif defined(FEATURE_TW_CAMERA_FPS28_MCLK24_PCLK92_V)

struct as0260_reg as0260_1080[] =
{
	{0x098E, 0xCA12,2},      // LOGICAL_ADDRESS_ACCESS [CAM_SYSCTL_PLL_ENABLE]
	{0xCA12, 0x01,1},      // CAM_SYSCTL_PLL_ENABLE
	{0xCA13, 0x00,1},      // CAM_SYSCTL_SENSOR_CLK_DIV2_EN

	{0xCA14, 0x022E,2}, 		//cam_sysctl_pll_divider_m_n = 558
	{0xCA16, 0x0070,2}, 		//cam_sysctl_pll_divider_p = 112
	{0xCA18, 0x7F7C,2}, 		//cam_sysctl_pll_divider_p4_p5_p6 = 32636
	{0xCA1C, 0x8043,2}, 		//cam_port_output_control = 32835
	{0xCA1E, 0x0008,2}, 		//cam_port_porch = 8
	{0xCA20, 0x0C00,2}, 		//cam_port_mipi_timing_t_hs_zero = 3072
	{0xCA22, 0x0006,2}, 		//cam_port_mipi_timing_t_hs_exit_hs_trail = 6
	{0xCA24, 0x0B01,2}, 		//cam_port_mipi_timing_t_clk_post_clk_pre = 2817
	{0xCA26, 0x0517,2}, 		//cam_port_mipi_timing_t_clk_trail_clk_zero = 1303
	{0xCA28, 0x0005,2}, 		//cam_port_mipi_timing_t_lpx = 5
	{0xCA2A, 0x0809,2}, 		//cam_port_mipi_timing_init_timing = 2057
	{0xCA2C, 0x0000,2}, 		//cam_port_mipi_timing_t_hs_pre = 0

	//[Timing_settings]
	{0xC800, 0x0020,2}, 		//cam_sensor_cfg_y_addr_start = 32
	{0xC802, 0x0020,2}, 		//cam_sensor_cfg_x_addr_start = 32
	{0xC804, 0x045F,2}, 		//cam_sensor_cfg_y_addr_end = 1119
	{0xC806, 0x07A7,2}, 		//cam_sensor_cfg_x_addr_end = 1959
	{0xC808, 0x035FE1D8,4}, 		//cam_sensor_cfg_pixclk = 56615384
	{0xC80C, 0x0001,2}, 		//cam_sensor_cfg_row_speed = 1
	{0xC80E, 0x0336,2}, 		//cam_sensor_cfg_fine_integ_time_min = 822
	{0xC810, 0x0BC5,2}, 		//cam_sensor_cfg_fine_integ_time_max = 3013
	{0xC812, 0x0491,2}, 		//cam_sensor_cfg_frame_length_lines = 1169
	{0xC814, 0x0D83,2}, 		//cam_sensor_cfg_line_length_pck = 3459
	{0xC816, 0x00D4,2}, 		//cam_sensor_cfg_fine_correction = 212
	{0xC818, 0x043B,2}, 		//cam_sensor_cfg_cpipe_last_row = 1083
	{0xC820, 0x0010,2}, 		//cam_sensor_cfg_reg_0_data = 16
	{0xC830, 0x0002,2}, 		//cam_sensor_control_read_mode = 2
	{0xC858, 0x0000,2}, 		//cam_crop_window_xoffset = 0
	{0xC85A, 0x0000,2}, 		//cam_crop_window_yoffset = 0
	{0xC85C, 0x0780,2}, 		//cam_crop_window_width = 1920
	{0xC85E, 0x0438,2}, 		//cam_crop_window_height = 1080
	{0xC86C, 0x0780,2}, 		//cam_output_width = 1920
	{0xC86E, 0x0438,2}, 		//cam_output_height = 1080
	{0xC870, 0x4010,2}, 		//cam_output_format = 16592
	{0xC87C, 0x00,1}, 		//cam_aet_aemode = 0
	{0xC88E, 0x1C00,2}, 		//cam_aet_max_frame_rate = 7168
	{0xC890, 0x1C00,2}, 		//cam_aet_min_frame_rate = 7168
	{0xC94C, 0x0000,2}, 		//cam_stat_awb_clip_window_xstart = 0
	{0xC94E, 0x0000,2}, 		//cam_stat_awb_clip_window_ystart = 0
	{0xC950, 0x077F,2}, 		//cam_stat_awb_clip_window_xend = 1919
	{0xC952, 0x0437,2}, 		//cam_stat_awb_clip_window_yend = 1079
	{0xC954, 0x0000,2}, 		//cam_stat_ae_initial_window_xstart = 0
	{0xC956, 0x0000,2}, 		//cam_stat_ae_initial_window_ystart = 0
	{0xC958, 0x017F,2}, 		//cam_stat_ae_initial_window_xend = 383
	{0xC95A, 0x00D7,2}, 		//cam_stat_ae_initial_window_yend = 215
};

struct as0260_reg as0260_1440[] =
{
	{0x098E, 0xCA12,2},      // LOGICAL_ADDRESS_ACCESS [CAM_SYSCTL_PLL_ENABLE]
	{0xCA12, 0x01,1},      // CAM_SYSCTL_PLL_ENABLE
	{0xCA13, 0x00,1},      // CAM_SYSCTL_SENSOR_CLK_DIV2_EN

	{0xCA14, 0x022E,2}, 		//cam_sysctl_pll_divider_m_n = 558
	{0xCA16, 0x0070,2}, 		//cam_sysctl_pll_divider_p = 112
	{0xCA18, 0x7F7C,2}, 		//cam_sysctl_pll_divider_p4_p5_p6 = 32636
	{0xCA1C, 0x8043,2}, 		//cam_port_output_control = 32835
	{0xCA1E, 0x0008,2}, 		//cam_port_porch = 8
	{0xCA20, 0x0C00,2}, 		//cam_port_mipi_timing_t_hs_zero = 3072
	{0xCA22, 0x0006,2}, 		//cam_port_mipi_timing_t_hs_exit_hs_trail = 6
	{0xCA24, 0x0B01,2}, 		//cam_port_mipi_timing_t_clk_post_clk_pre = 2817
	{0xCA26, 0x0517,2}, 		//cam_port_mipi_timing_t_clk_trail_clk_zero = 1303
	{0xCA28, 0x0005,2}, 		//cam_port_mipi_timing_t_lpx = 5
	{0xCA2A, 0x0809,2}, 		//cam_port_mipi_timing_init_timing = 2057
	{0xCA2C, 0x0000,2}, 		//cam_port_mipi_timing_t_hs_pre = 0

	{0xC800, 0x0020,2}, 		//cam_sensor_cfg_y_addr_start = 32  
	{0xC802, 0x0120,2}, 		//cam_sensor_cfg_x_addr_start = 288 
	{0xC804, 0x045F,2}, 		//cam_sensor_cfg_y_addr_end = 1119  
	{0xC806, 0x06C7,2}, 		//cam_sensor_cfg_x_addr_end = 1735  
	{0xC808, 0x035FE1D8,4}, 		//cam_sensor_cfg_pixclk = 56615384
	{0xC80C, 0x0001,2}, 		//cam_sensor_cfg_row_speed = 1
	{0xC80E, 0x0336,2}, 		//cam_sensor_cfg_fine_integ_time_min = 822
	{0xC810, 0x0BC5,2}, 		//cam_sensor_cfg_fine_integ_time_max = 3013
	{0xC812, 0x0491,2}, 		//cam_sensor_cfg_frame_length_lines = 1169
	{0xC814, 0x0D83,2}, 		//cam_sensor_cfg_line_length_pck = 3459
	{0xC816, 0x00D4,2}, 		//cam_sensor_cfg_fine_correction = 212
	{0xC818, 0x043B,2}, 		//cam_sensor_cfg_cpipe_last_row = 1083
	{0xC820, 0x0010,2}, 		//cam_sensor_cfg_reg_0_data = 16
	{0xC830, 0x0002,2}, 		//cam_sensor_control_read_mode = 2
	{0xC858, 0x0000,2}, 		//cam_crop_window_xoffset = 0
	{0xC85A, 0x0000,2}, 		//cam_crop_window_yoffset = 0
	{0xC85C, 0x05A0,2}, 		//cam_crop_window_width = 1440   
	{0xC85E, 0x0438,2}, 		//cam_crop_window_height = 1080  
	{0xC86C, 0x05A0,2}, 		//cam_output_width = 1440        
	{0xC86E, 0x0438,2}, 		//cam_output_height = 1080       
	{0xC870, 0x4010,2},	//cam_output_format = 16592
	{0xC87C, 0x00,1}, 		//cam_aet_aemode = 0
	{0xC88E, 0x1C00,2}, 		//cam_aet_max_frame_rate = 7168
	{0xC890, 0x1C00,2}, 		//cam_aet_min_frame_rate = 7168
	{0xC94C, 0x0000,2}, 		//cam_stat_awb_clip_window_xstart = 0
	{0xC94E, 0x0000,2}, 		//cam_stat_awb_clip_window_ystart = 0
	{0xC950, 0x05a0,2}, 		//cam_stat_awb_clip_window_xend = 1919
	{0xC952, 0x0438,2}, 		//cam_stat_awb_clip_window_yend = 1079
	{0xC954, 0x0000,2}, 		// CAM_STAT_AE_INITIAL_WINDOW_XSTART
	{0xC956, 0x0000,2}, 		// CAM_STAT_AE_INITIAL_WINDOW_YSTART
	{0xC958, 0x0120,2}, 		// CAM_STAT_AE_INITIAL_WINDOW_XEND  
	{0xC95A, 0x00D8,2}, 		// CAM_STAT_AE_INITIAL_WINDOW_YEND 
};


struct as0260_reg as0260_960[] =
{
	{0x098E, 0xCA12,2},      // LOGICAL_ADDRESS_ACCESS [CAM_SYSCTL_PLL_ENABLE]
	{0xCA12, 0x01,1},      // CAM_SYSCTL_PLL_ENABLE
	{0xCA13, 0x00,1},      // CAM_SYSCTL_SENSOR_CLK_DIV2_EN

	{0xCA14, 0x022E,2}, 		//cam_sysctl_pll_divider_m_n = 558
	{0xCA16, 0x0070,2}, 		//cam_sysctl_pll_divider_p = 112
	{0xCA18, 0x7F7C,2}, 		//cam_sysctl_pll_divider_p4_p5_p6 = 32636
	{0xCA1C, 0x8043,2}, 		//cam_port_output_control = 32835
	{0xCA1E, 0x0008,2}, 		//cam_port_porch = 8
	{0xCA20, 0x0C00,2}, 		//cam_port_mipi_timing_t_hs_zero = 3072
	{0xCA22, 0x0006,2}, 		//cam_port_mipi_timing_t_hs_exit_hs_trail = 6
	{0xCA24, 0x0B01,2}, 		//cam_port_mipi_timing_t_clk_post_clk_pre = 2817
	{0xCA26, 0x0517,2}, 		//cam_port_mipi_timing_t_clk_trail_clk_zero = 1303
	{0xCA28, 0x0005,2}, 		//cam_port_mipi_timing_t_lpx = 5
	{0xCA2A, 0x0809,2}, 		//cam_port_mipi_timing_init_timing = 2057
	{0xCA2C, 0x0000,2}, 		//cam_port_mipi_timing_t_hs_pre = 0


	//[Timing_settings]
	{0xC800, 0x0020,2}, 		//cam_sensor_cfg_y_addr_start = 32
	{0xC802, 0x0120,2}, 		//cam_sensor_cfg_x_addr_start = 288
	{0xC804, 0x045F,2}, 		//cam_sensor_cfg_y_addr_end = 1119
	{0xC806, 0x06C7,2}, 		//cam_sensor_cfg_x_addr_end = 1735
	{0xC808, 0x035FE1D8,4}, 		//cam_sensor_cfg_pixclk = 56615384
	{0xC80C, 0x0001,2}, 		//cam_sensor_cfg_row_speed = 1
	{0xC80E, 0x0336,2}, 		//cam_sensor_cfg_fine_integ_time_min = 822
	{0xC810, 0x0BC5,2}, 		//cam_sensor_cfg_fine_integ_time_max = 3013
	{0xC812, 0x0491,2}, 		//cam_sensor_cfg_frame_length_lines = 1169
	{0xC814, 0x0D83,2}, 		//cam_sensor_cfg_line_length_pck = 3459
	{0xC816, 0x00D4,2}, 		//cam_sensor_cfg_fine_correction = 212
	{0xC818, 0x043B,2}, 		//cam_sensor_cfg_cpipe_last_row = 1083
	{0xC820, 0x0010,2}, 		//cam_sensor_cfg_reg_0_data = 16
	{0xC830, 0x0002,2}, 		//cam_sensor_control_read_mode = 2
	{0xC858, 0x0000,2}, 		//cam_crop_window_xoffset = 0
	{0xC85A, 0x0000,2}, 		//cam_crop_window_yoffset = 0
	{0xC85C, 0x05A0,2}, 		//cam_crop_window_width = 1440
	{0xC85E, 0x0438,2}, 		//cam_crop_window_height = 1080
	{0xC86C, 0x0500,2}, 		//cam_output_width = 1280
	{0xC86E, 0x03C0,2}, 		//cam_output_height = 960
	{0xC870, 0x4010,2}, 		//cam_output_format = 16592
	{0xC87C, 0x00,1}, 		//cam_aet_aemode = 0
	{0xC88E, 0x1C00,2}, 		//cam_aet_max_frame_rate = 7168
	{0xC890, 0x1C00,2}, 		//cam_aet_min_frame_rate = 7168
	{0xC94C, 0x0000,2}, 		//cam_stat_awb_clip_window_xstart = 0
	{0xC94E, 0x0000,2}, 		//cam_stat_awb_clip_window_ystart = 0
	{0xC950, 0x04FF,2}, 		//cam_stat_awb_clip_window_xend = 1279
	{0xC952, 0x03BF,2}, 		//cam_stat_awb_clip_window_yend = 959
	{0xC954, 0x0000,2}, 		//cam_stat_ae_initial_window_xstart = 0
	{0xC956, 0x0000,2}, 		//cam_stat_ae_initial_window_ystart = 0
	{0xC958, 0x00FF,2}, 		//cam_stat_ae_initial_window_xend = 255
	{0xC95A, 0x00BF,2}, 		//cam_stat_ae_initial_window_yend = 191
};

struct as0260_reg as0260_720[] =
{
	{0x098E, 0xCA12,2},      // LOGICAL_ADDRESS_ACCESS [CAM_SYSCTL_PLL_ENABLE]
	{0xCA12, 0x01,1},      // CAM_SYSCTL_PLL_ENABLE
	{0xCA13, 0x00,1},      // CAM_SYSCTL_SENSOR_CLK_DIV2_EN
	{0xCA14, 0x022E,2}, 		//cam_sysctl_pll_divider_m_n = 558
	{0xCA16, 0x0070,2}, 		//cam_sysctl_pll_divider_p = 112
	{0xCA18, 0x7F7C,2}, 		//cam_sysctl_pll_divider_p4_p5_p6 = 32636
	{0xCA1C, 0x8043,2}, 		//cam_port_output_control = 32835
	{0xCA1E, 0x0008,2}, 		//cam_port_porch = 8
	{0xCA20, 0x0C00,2}, 		//cam_port_mipi_timing_t_hs_zero = 3072
	{0xCA22, 0x0006,2}, 		//cam_port_mipi_timing_t_hs_exit_hs_trail = 6
	{0xCA24, 0x0B01,2}, 		//cam_port_mipi_timing_t_clk_post_clk_pre = 2817
	{0xCA26, 0x0517,2}, 		//cam_port_mipi_timing_t_clk_trail_clk_zero = 1303
	{0xCA28, 0x0005,2}, 		//cam_port_mipi_timing_t_lpx = 5
	{0xCA2A, 0x0809,2}, 		//cam_port_mipi_timing_init_timing = 2057
	{0xCA2C, 0x0000,2}, 		//cam_port_mipi_timing_t_hs_pre = 0


	//[Timing_settings]
	{0xC800, 0x0020,2}, 		//cam_sensor_cfg_y_addr_start = 32
	{0xC802, 0x0020,2}, 		//cam_sensor_cfg_x_addr_start = 32
	{0xC804, 0x045F,2}, 		//cam_sensor_cfg_y_addr_end = 1119
	{0xC806, 0x07A7,2}, 		//cam_sensor_cfg_x_addr_end = 1959
	{0xC808, 0x035FE1D8,4}, 		//cam_sensor_cfg_pixclk = 56615384
	{0xC80C, 0x0001,2}, 		//cam_sensor_cfg_row_speed = 1
	{0xC80E, 0x0336,2}, 		//cam_sensor_cfg_fine_integ_time_min = 822
	{0xC810, 0x0BC5,2}, 		//cam_sensor_cfg_fine_integ_time_max = 3013
	{0xC812, 0x0491,2}, 		//cam_sensor_cfg_frame_length_lines = 1169
	{0xC814, 0x0D83,2}, 		//cam_sensor_cfg_line_length_pck = 3459
	{0xC816, 0x00D4,2}, 		//cam_sensor_cfg_fine_correction = 212
	{0xC818, 0x043B,2}, 		//cam_sensor_cfg_cpipe_last_row = 1083
	{0xC820, 0x0010,2}, 		//cam_sensor_cfg_reg_0_data = 16
	{0xC830, 0x0002,2}, 		//cam_sensor_control_read_mode = 2
	{0xC858, 0x0000,2}, 		//cam_crop_window_xoffset = 0
	{0xC85A, 0x0000,2}, 		//cam_crop_window_yoffset = 0
	{0xC85C, 0x0780,2}, 		//cam_crop_window_width = 1920
	{0xC85E, 0x0438,2}, 		//cam_crop_window_height = 1080
	{0xC86C, 0x0500,2}, 		//cam_output_width = 1280
	{0xC86E, 0x02D0,2}, 		//cam_output_height = 720
	{0xC870, 0x4010,2}, 		//cam_output_format = 16592

	{0xC87C, 0x00,1}, 		//cam_aet_aemode = 0
	{0xC88E, 0x1C00,2}, 		//cam_aet_max_frame_rate = 7168
	{0xC890, 0x1C00,2}, 		//cam_aet_min_frame_rate = 7168
	{0xC94C, 0x0000,2}, 		//cam_stat_awb_clip_window_xstart = 0
	{0xC94E, 0x0000,2}, 		//cam_stat_awb_clip_window_ystart = 0
	{0xC950, 0x04FF,2}, 		//cam_stat_awb_clip_window_xend = 1279
	{0xC952, 0x02CF,2}, 		//cam_stat_awb_clip_window_yend = 719
	{0xC954, 0x0000,2}, 		//cam_stat_ae_initial_window_xstart = 0
	{0xC956, 0x0000,2}, 		//cam_stat_ae_initial_window_ystart = 0
	{0xC958, 0x00FF,2}, 		//cam_stat_ae_initial_window_xend = 255
	{0xC95A, 0x008F,2}, 		//cam_stat_ae_initial_window_yend = 143

};

struct as0260_reg as0260_640[] =
{
	{0x098E, 0xCA12,2},      // LOGICAL_ADDRESS_ACCESS [CAM_SYSCTL_PLL_ENABLE]
	{0xCA12, 0x01,1},      // CAM_SYSCTL_PLL_ENABLE
	{0xCA13, 0x00,1},      // CAM_SYSCTL_SENSOR_CLK_DIV2_EN
	{0xCA14, 0x022E,2}, 		//cam_sysctl_pll_divider_m_n = 558
	{0xCA16, 0x0070,2}, 		//cam_sysctl_pll_divider_p = 112
	{0xCA18, 0x7F7C,2}, 		//cam_sysctl_pll_divider_p4_p5_p6 = 32636
	{0xCA1C, 0x8043,2}, 		//cam_port_output_control = 32835
	{0xCA1E, 0x0008,2}, 		//cam_port_porch = 8
	{0xCA20, 0x0C00,2}, 		//cam_port_mipi_timing_t_hs_zero = 3072
	{0xCA22, 0x0006,2}, 		//cam_port_mipi_timing_t_hs_exit_hs_trail = 6
	{0xCA24, 0x0B01,2}, 		//cam_port_mipi_timing_t_clk_post_clk_pre = 2817
	{0xCA26, 0x0517,2}, 		//cam_port_mipi_timing_t_clk_trail_clk_zero = 1303
	{0xCA28, 0x0005,2}, 		//cam_port_mipi_timing_t_lpx = 5
	{0xCA2A, 0x0809,2}, 		//cam_port_mipi_timing_init_timing = 2057
	{0xCA2C, 0x0000,2}, 		//cam_port_mipi_timing_t_hs_pre = 0


	//[Timing_settings]
	{0xC800, 0x0020,2}, 		//cam_sensor_cfg_y_addr_start = 32
	{0xC802, 0x0100,2}, 		//cam_sensor_cfg_x_addr_start = 256
	{0xC804, 0x045D,2}, 		//cam_sensor_cfg_y_addr_end = 1117
	{0xC806, 0x06AD,2}, 		//cam_sensor_cfg_x_addr_end = 1709
	{0xC808, 0x035FE1D8,4}, 		//cam_sensor_cfg_pixclk = 56615384
	{0xC80C, 0x0001,2}, 		//cam_sensor_cfg_row_speed = 1
	{0xC80E, 0x06A4,2}, 		//cam_sensor_cfg_fine_integ_time_min = 1700
	{0xC810, 0x164E,2}, 		//cam_sensor_cfg_fine_integ_time_max = 5710
	{0xC812, 0x026D,2}, 		//cam_sensor_cfg_frame_length_lines = 621
	{0xC814, 0x1970,2}, 		//cam_sensor_cfg_line_length_pck = 6512
	{0xC816, 0x01D9,2}, 		//cam_sensor_cfg_fine_correction = 473
	{0xC818, 0x021B,2}, 		//cam_sensor_cfg_cpipe_last_row = 539
	{0xC820, 0x0010,2}, 		//cam_sensor_cfg_reg_0_data = 16
	{0xC830, 0x0012,2}, 		//cam_sensor_control_read_mode = 18
	{0xC858, 0x0000,2}, 		//cam_crop_window_xoffset = 0
	{0xC85A, 0x0000,2}, 		//cam_crop_window_yoffset = 0
	{0xC85C, 0x02D0,2}, 		//cam_crop_window_width = 720
	{0xC85E, 0x0218,2}, 		//cam_crop_window_height = 536
	{0xC86C, 0x0280,2}, 		//cam_output_width = 640
	{0xC86E, 0x01E0,2}, 		//cam_output_height = 480
	{0xC870, 0x4010,2}, 		//cam_output_format = 16592

	{0xC87C, 0x00,1}, 		//cam_aet_aemode = 0
	{0xC88E, 0x1C00,2}, 		//cam_aet_max_frame_rate = 7168
	{0xC890, 0x1C00,2}, 		//cam_aet_min_frame_rate = 7168
	{0xC94C, 0x0000,2}, 		//cam_stat_awb_clip_window_xstart = 0
	{0xC94E, 0x0000,2}, 		//cam_stat_awb_clip_window_ystart = 0
	{0xC950, 0x027F,2}, 		//cam_stat_awb_clip_window_xend = 639
	{0xC952, 0x01DF,2}, 		//cam_stat_awb_clip_window_yend = 479
	{0xC954, 0x0000,2}, 		//cam_stat_ae_initial_window_xstart = 0
	{0xC956, 0x0000,2}, 		//cam_stat_ae_initial_window_ystart = 0
	{0xC958, 0x007F,2}, 		//cam_stat_ae_initial_window_xend = 127
	{0xC95A, 0x005F,2}, 		//cam_stat_ae_initial_window_yend = 95
};

struct as0260_reg as0260_320[] =
{
	{0x098E, 0xCA12,2},      // LOGICAL_ADDRESS_ACCESS [CAM_SYSCTL_PLL_ENABLE]
	{0xCA12, 0x01,1},      // CAM_SYSCTL_PLL_ENABLE
	{0xCA13, 0x00,1},      // CAM_SYSCTL_SENSOR_CLK_DIV2_EN
	{0xCA14, 0x022E,2}, 		//cam_sysctl_pll_divider_m_n = 558
	{0xCA16, 0x0070,2}, 		//cam_sysctl_pll_divider_p = 112
	{0xCA18, 0x7F7C,2}, 		//cam_sysctl_pll_divider_p4_p5_p6 = 32636
	{0xCA1C, 0x8043,2}, 		//cam_port_output_control = 32835
	{0xCA1E, 0x0008,2}, 		//cam_port_porch = 8
	{0xCA20, 0x0C00,2}, 		//cam_port_mipi_timing_t_hs_zero = 3072
	{0xCA22, 0x0006,2}, 		//cam_port_mipi_timing_t_hs_exit_hs_trail = 6
	{0xCA24, 0x0B01,2}, 		//cam_port_mipi_timing_t_clk_post_clk_pre = 2817
	{0xCA26, 0x0517,2}, 		//cam_port_mipi_timing_t_clk_trail_clk_zero = 1303
	{0xCA28, 0x0005,2}, 		//cam_port_mipi_timing_t_lpx = 5
	{0xCA2A, 0x0809,2}, 		//cam_port_mipi_timing_init_timing = 2057
	{0xCA2C, 0x0000,2}, 		//cam_port_mipi_timing_t_hs_pre = 0

	//[Timing_settings]
	{0xC800, 0x0020,2}, 		//cam_sensor_cfg_y_addr_start = 32
	{0xC802, 0x0100,2}, 		//cam_sensor_cfg_x_addr_start = 256
	{0xC804, 0x045D,2}, 		//cam_sensor_cfg_y_addr_end = 1117
	{0xC806, 0x06AD,2}, 		//cam_sensor_cfg_x_addr_end = 1709
	{0xC808, 0x035FE1D8,4}, 		//cam_sensor_cfg_pixclk = 56615384
	{0xC80C, 0x0001,2}, 		//cam_sensor_cfg_row_speed = 1
	{0xC80E, 0x06A4,2}, 		//cam_sensor_cfg_fine_integ_time_min = 1700
	{0xC810, 0x164E,2}, 		//cam_sensor_cfg_fine_integ_time_max = 5710
	{0xC812, 0x026D,2}, 		//cam_sensor_cfg_frame_length_lines = 621
	{0xC814, 0x1970,2}, 		//cam_sensor_cfg_line_length_pck = 6512
	{0xC816, 0x01D9,2}, 		//cam_sensor_cfg_fine_correction = 473
	{0xC818, 0x021B,2}, 		//cam_sensor_cfg_cpipe_last_row = 539
	{0xC820, 0x0010,2}, 		//cam_sensor_cfg_reg_0_data = 16
	{0xC830, 0x0012,2}, 		//cam_sensor_control_read_mode = 18
	{0xC858, 0x0000,2}, 		//cam_crop_window_xoffset = 0
	{0xC85A, 0x0000,2}, 		//cam_crop_window_yoffset = 0
	{0xC85C, 0x02D0,2}, 		//cam_crop_window_width = 720
	{0xC85E, 0x0218,2}, 		//cam_crop_window_height = 536
	{0xC86C, 0x0140,2}, 		//cam_output_width = 320
	{0xC86E, 0x00F0,2}, 		//cam_output_height = 240
	{0xC870, 0x4010,2}, 		//cam_output_format = 16592

	{0xC87C, 0x00,1}, 		//cam_aet_aemode = 0
	{0xC88E, 0x1C00,2}, 		//cam_aet_max_frame_rate = 7168
	{0xC890, 0x1C00,2}, 		//cam_aet_min_frame_rate = 7168
	{0xC94C, 0x0000,2}, 		//cam_stat_awb_clip_window_xstart = 0
	{0xC94E, 0x0000,2}, 		//cam_stat_awb_clip_window_ystart = 0
	{0xC950, 0x013F,2}, 		//cam_stat_awb_clip_window_xend = 319
	{0xC952, 0x00EF,2}, 		//cam_stat_awb_clip_window_yend = 239
	{0xC954, 0x0000,2}, 		//cam_stat_ae_initial_window_xstart = 0
	{0xC956, 0x0000,2}, 		//cam_stat_ae_initial_window_ystart = 0
	{0xC958, 0x003F,2}, 		//cam_stat_ae_initial_window_xend = 63
	{0xC95A, 0x002F,2}, 		//cam_stat_ae_initial_window_yend = 47
};

struct as0260_reg as0260_176[] =
{
	{0x098E, 0xCA12,2},      // LOGICAL_ADDRESS_ACCESS [CAM_SYSCTL_PLL_ENABLE]
	{0xCA12, 0x01,1},      //cam_sysctl_pll_enable = 1
	{0xCA13, 0x00,0},      //cam_sysctl_pll_enable = 1

	{0xCA14, 0x022E,2}, 		//cam_sysctl_pll_divider_m_n = 558
	{0xCA16, 0x0070,2}, 		//cam_sysctl_pll_divider_p = 112
	{0xCA18, 0x7F7C,2}, 		//cam_sysctl_pll_divider_p4_p5_p6 = 32636
	{0xCA1C, 0x8043,2}, 		//cam_port_output_control = 32835
	{0xCA1E, 0x0008,2}, 		//cam_port_porch = 8
	{0xCA20, 0x0C00,2}, 		//cam_port_mipi_timing_t_hs_zero = 3072
	{0xCA22, 0x0006,2}, 		//cam_port_mipi_timing_t_hs_exit_hs_trail = 6
	{0xCA24, 0x0B01,2}, 		//cam_port_mipi_timing_t_clk_post_clk_pre = 2817
	{0xCA26, 0x0517,2}, 		//cam_port_mipi_timing_t_clk_trail_clk_zero = 1303
	{0xCA28, 0x0005,2}, 		//cam_port_mipi_timing_t_lpx = 5
	{0xCA2A, 0x0809,2}, 		//cam_port_mipi_timing_init_timing = 2057
	{0xCA2C, 0x0000,2}, 		//cam_port_mipi_timing_t_hs_pre = 0

	{0xC800, 0x0020,2}, 		//cam_sensor_cfg_y_addr_start = 32
	{0xC802, 0x0100,2}, 		//cam_sensor_cfg_x_addr_start = 256
	{0xC804, 0x045D,2}, 		//cam_sensor_cfg_y_addr_end = 1117
	{0xC806, 0x06AD,2}, 		//cam_sensor_cfg_x_addr_end = 1709
	{0xC808, 0x035FE1D8,4}, 		//cam_sensor_cfg_pixclk = 56615384
	{0xC80C, 0x0001,2}, 		//cam_sensor_cfg_row_speed = 1
	{0xC80E, 0x06A4,2}, 		//cam_sensor_cfg_fine_integ_time_min = 1700
	{0xC810, 0x1842,2}, 		//cam_sensor_cfg_fine_integ_time_max = 6210
	{0xC812, 0x026D,2}, 		//cam_sensor_cfg_frame_length_lines = 621
	{0xC814, 0x1B64,2}, 		//cam_sensor_cfg_line_length_pck = 7012
	{0xC816, 0x01D9,2}, 		//cam_sensor_cfg_fine_correction = 473
	{0xC818, 0x021B,2}, 		//cam_sensor_cfg_cpipe_last_row = 539
	{0xC820, 0x0010,2}, 		//cam_sensor_cfg_reg_0_data = 16
	{0xC830, 0x0012,2}, 		//cam_sensor_control_read_mode = 18
	{0xC858, 0x0000,2}, 		//cam_crop_window_xoffset = 0
	{0xC85A, 0x0000,2}, 		//cam_crop_window_yoffset = 0
	{0xC85C, 0x02D0,2}, 		//cam_crop_window_width = 720
	{0xC85E, 0x0218,2}, 		//cam_crop_window_height = 536
	{0xC86C, 0x00b0,2}, 		//cam_output_width = 320
	{0xC86E, 0x0090,2}, 		//cam_output_height = 240
	{0xC870, 0x4010,2},	//cam_output_format = 16592
	{0xC87C, 0x00,1}, 		//cam_aet_aemode = 0
	{0xC88E, 0x1A00,2}, 		//cam_aet_max_frame_rate = 6656
	{0xC890, 0x1A00,2}, 		//cam_aet_min_frame_rate = 6656
	{0xC94C, 0x0000,2}, 		//cam_stat_awb_clip_window_xstart = 0
	{0xC94E, 0x0000,2}, 		//cam_stat_awb_clip_window_ystart = 0
	{0xC950, 0x00b0,2}, 		//cam_stat_awb_clip_window_xend = 319
	{0xC952, 0x0090,2}, 		//cam_stat_awb_clip_window_yend = 239
};
#elif defined(FEATURE_TW_CAMERA_FPS28_MCLK24_PCLK92_H)
struct as0260_reg as0260_1080[] =
{
{0x098E, 0xCA12,2},      // LOGICAL_ADDRESS_ACCESS [CAM_SYSCTL_PLL_ENABLE]
{0xCA12, 0x01,1},      //cam_sysctl_pll_enable = 1
{0xCA13, 0x00,1},      //cam_sysctl_pll_enable = 1

{0xCA14, 0x022B,2}, 		//cam_sysctl_pll_divider_m_n = 555
{0xCA16, 0x0070,2}, 		//cam_sysctl_pll_divider_p = 112
{0xCA18, 0x7F7C,2}, 		//cam_sysctl_pll_divider_p4_p5_p6 = 32636
{0xCA1C, 0x8043,2}, 		//cam_port_output_control = 32835
{0xCA1E, 0x0008,2}, 		//cam_port_porch = 8
{0xCA20, 0x0B00,2}, 		//cam_port_mipi_timing_t_hs_zero = 2816
{0xCA22, 0x0005,2}, 		//cam_port_mipi_timing_t_hs_exit_hs_trail = 5
{0xCA24, 0x0A01,2}, 		//cam_port_mipi_timing_t_clk_post_clk_pre = 2561
{0xCA26, 0x0515,2}, 		//cam_port_mipi_timing_t_clk_trail_clk_zero = 1301
{0xCA28, 0x0004,2}, 		//cam_port_mipi_timing_t_lpx = 4
{0xCA2A, 0x0709,2}, 		//cam_port_mipi_timing_init_timing = 1801
{0xCA2C, 0x0000,2}, 		//cam_port_mipi_timing_t_hs_pre = 0


//[Timing_settings]
{0xC800, 0x0020,2}, 		//cam_sensor_cfg_y_addr_start = 32
{0xC802, 0x0020,2}, 		//cam_sensor_cfg_x_addr_start = 32
{0xC804, 0x045F,2}, 		//cam_sensor_cfg_y_addr_end = 1119
{0xC806, 0x07A7,2}, 		//cam_sensor_cfg_x_addr_end = 1959
{0xC808, 0x03278AC4,4}, 		//cam_sensor_cfg_pixclk = 52923076
{0xC80C, 0x0001,2}, 		//cam_sensor_cfg_row_speed = 1
{0xC80E, 0x0336,2}, 		//cam_sensor_cfg_fine_integ_time_min = 822
{0xC810, 0x0A5C,2}, 		//cam_sensor_cfg_fine_integ_time_max = 2652
{0xC812, 0x04C4,2}, 		//cam_sensor_cfg_frame_length_lines = 1220
{0xC814, 0x0C1A,2}, 		//cam_sensor_cfg_line_length_pck = 3098
{0xC816, 0x00D4,2}, 		//cam_sensor_cfg_fine_correction = 212
{0xC818, 0x043B,2}, 		//cam_sensor_cfg_cpipe_last_row = 1083
{0xC820, 0x0010,2}, 		//cam_sensor_cfg_reg_0_data = 16
{0xC830, 0x0002,2}, 		//cam_sensor_control_read_mode = 2
{0xC858, 0x0000,2}, 		//cam_crop_window_xoffset = 0
{0xC85A, 0x0000,2}, 		//cam_crop_window_yoffset = 0
{0xC85C, 0x0780,2}, 		//cam_crop_window_width = 1920
{0xC85E, 0x0438,2}, 		//cam_crop_window_height = 1080
{0xC86C, 0x0780,2}, 		//cam_output_width = 1920
{0xC86E, 0x0438,2}, 		//cam_output_height = 1080
{0xC870, 0x4010,2}, 		//cam_output_format = 16592
{0xC87C, 0x00,1}, 		//cam_aet_aemode = 0
{0xC88E, 0x1C00,2}, 		//cam_aet_max_frame_rate = 7168
{0xC890, 0x1C00,2}, 		//cam_aet_min_frame_rate = 7168
{0xC94C, 0x0000,2}, 		//cam_stat_awb_clip_window_xstart = 0
{0xC94E, 0x0000,2}, 		//cam_stat_awb_clip_window_ystart = 0
{0xC950, 0x077F,2}, 		//cam_stat_awb_clip_window_xend = 1919
{0xC952, 0x0437,2}, 		//cam_stat_awb_clip_window_yend = 1079
{0xC954, 0x0000,2}, 		//cam_stat_ae_initial_window_xstart = 0
{0xC956, 0x0000,2}, 		//cam_stat_ae_initial_window_ystart = 0
{0xC958, 0x017F,2}, 		//cam_stat_ae_initial_window_xend = 383
{0xC95A, 0x00D7,2}, 		//cam_stat_ae_initial_window_yend = 215
};

struct as0260_reg as0260_1440[] =
{
{0x098E, 0xCA12,2},      // LOGICAL_ADDRESS_ACCESS [CAM_SYSCTL_PLL_ENABLE]
{0xCA12, 0x01,1},      //cam_sysctl_pll_enable = 1
{0xCA13, 0x00,1},      //cam_sysctl_pll_enable = 1

{0xCA14, 0x0010,2}, 		//cam_sysctl_pll_divider_m_n = 16
{0xCA16, 0x0070,2}, 		//cam_sysctl_pll_divider_p = 112
{0xCA18, 0x7F7C,2}, 		//cam_sysctl_pll_divider_p4_p5_p6 = 32636
{0xCA1C, 0x8041,2}, 		//cam_port_output_control = 32833
{0xCA1E, 0x0008,2}, 		//cam_port_porch = 8
{0xCA20, 0x0D00,2}, 		//cam_port_mipi_timing_t_hs_zero = 3328
{0xCA22, 0x0006,2}, 		//cam_port_mipi_timing_t_hs_exit_hs_trail = 6
{0xCA24, 0x0B01,2}, 		//cam_port_mipi_timing_t_clk_post_clk_pre = 2817
{0xCA26, 0x0618,2}, 		//cam_port_mipi_timing_t_clk_trail_clk_zero = 1560
{0xCA28, 0x0005,2}, 		//cam_port_mipi_timing_t_lpx = 5
{0xCA2A, 0x080A,2}, 		//cam_port_mipi_timing_init_timing = 2058
{0xCA2C, 0x0000,2}, 		//cam_port_mipi_timing_t_hs_pre = 0


//[Timing_settings]
{0xC800, 0x0020,2}, 		//cam_sensor_cfg_y_addr_start = 32
{0xC802, 0x0020,2}, 		//cam_sensor_cfg_x_addr_start = 32
{0xC804, 0x045F,2}, 		//cam_sensor_cfg_y_addr_end = 1119
{0xC806, 0x07A7,2}, 		//cam_sensor_cfg_x_addr_end = 1959
{0xC808, 0x0385713B,4}, 		//cam_sensor_cfg_pixclk = 59076923
{0xC80C, 0x0001,2}, 		//cam_sensor_cfg_row_speed = 1
{0xC80E, 0x0336,2}, 		//cam_sensor_cfg_fine_integ_time_min = 822
{0xC810, 0x0A5C,2}, 		//cam_sensor_cfg_fine_integ_time_max = 2652
{0xC812, 0x04F7,2}, 		//cam_sensor_cfg_frame_length_lines = 1271
{0xC814, 0x0C1A,2}, 		//cam_sensor_cfg_line_length_pck = 3098
{0xC816, 0x00D4,2}, 		//cam_sensor_cfg_fine_correction = 212
{0xC818, 0x043B,2}, 		//cam_sensor_cfg_cpipe_last_row = 1083
{0xC820, 0x0010,2}, 		//cam_sensor_cfg_reg_0_data = 16
{0xC830, 0x0002,2}, 		//cam_sensor_control_read_mode = 2
{0xC858, 0x0000,2}, 		//cam_crop_window_xoffset = 0
{0xC85A, 0x0000,2}, 		//cam_crop_window_yoffset = 0
{0xC85C, 0x0780,2}, 		//cam_crop_window_width = 1920
{0xC85E, 0x0438,2}, 		//cam_crop_window_height = 1080
{0xC86C, 0x0780,2}, 		//cam_output_width = 1920
{0xC86E, 0x0438,2}, 		//cam_output_height = 1080
{0xC870, 0x4010,2}, 		//cam_output_format = 16592
{0xC87C, 0x00,1}, 		//cam_aet_aemode = 0
{0xC88E, 0x1E00,2}, 		//cam_aet_max_frame_rate = 7680
{0xC890, 0x1E00,2}, 		//cam_aet_min_frame_rate = 7680
{0xC94C, 0x0000,2}, 		//cam_stat_awb_clip_window_xstart = 0
{0xC94E, 0x0000,2}, 		//cam_stat_awb_clip_window_ystart = 0
{0xC950, 0x077F,2}, 		//cam_stat_awb_clip_window_xend = 1919
{0xC952, 0x0437,2}, 		//cam_stat_awb_clip_window_yend = 1079
{0xC954, 0x0000,2}, 		//cam_stat_ae_initial_window_xstart = 0
{0xC956, 0x0000,2}, 		//cam_stat_ae_initial_window_ystart = 0
{0xC958, 0x017F,2}, 		//cam_stat_ae_initial_window_xend = 383
{0xC95A, 0x00D7,2}, 		//cam_stat_ae_initial_window_yend = 215
};


struct as0260_reg as0260_960[] =
{
	{0x098E, 0xCA12,2},      // LOGICAL_ADDRESS_ACCESS [CAM_SYSCTL_PLL_ENABLE]
	{0xCA12, 0x01,1},      //cam_sysctl_pll_enable = 1
	{0xCA13, 0x00,0},      //cam_sysctl_pll_enable = 1
	
{0xCA14, 0x022B,2}, 		//cam_sysctl_pll_divider_m_n = 555
{0xCA16, 0x0070,2}, 		//cam_sysctl_pll_divider_p = 112
{0xCA18, 0x7F7C,2}, 		//cam_sysctl_pll_divider_p4_p5_p6 = 32636
{0xCA1C, 0x8043,2}, 		//cam_port_output_control = 32835
{0xCA1E, 0x0008,2}, 		//cam_port_porch = 8
{0xCA20, 0x0B00,2}, 		//cam_port_mipi_timing_t_hs_zero = 2816
{0xCA22, 0x0005,2}, 		//cam_port_mipi_timing_t_hs_exit_hs_trail = 5
{0xCA24, 0x0A01,2}, 		//cam_port_mipi_timing_t_clk_post_clk_pre = 2561
{0xCA26, 0x0515,2}, 		//cam_port_mipi_timing_t_clk_trail_clk_zero = 1301
{0xCA28, 0x0004,2}, 		//cam_port_mipi_timing_t_lpx = 4
{0xCA2A, 0x0709,2}, 		//cam_port_mipi_timing_init_timing = 1801
{0xCA2C, 0x0000,2}, 		//cam_port_mipi_timing_t_hs_pre = 0


//[Timing_settings]
{0xC800, 0x0020,2}, 		//cam_sensor_cfg_y_addr_start = 32
{0xC802, 0x0120,2}, 		//cam_sensor_cfg_x_addr_start = 288
{0xC804, 0x045F,2}, 		//cam_sensor_cfg_y_addr_end = 1119
{0xC806, 0x06C7,2}, 		//cam_sensor_cfg_x_addr_end = 1735
{0xC808, 0x03278AC4,4}, 		//cam_sensor_cfg_pixclk = 52923076
{0xC80C, 0x0001,2}, 		//cam_sensor_cfg_row_speed = 1
{0xC80E, 0x0336,2}, 		//cam_sensor_cfg_fine_integ_time_min = 822
{0xC810, 0x0AE3,2}, 		//cam_sensor_cfg_fine_integ_time_max = 2787
{0xC812, 0x0491,2}, 		//cam_sensor_cfg_frame_length_lines = 1169
{0xC814, 0x0CA1,2}, 		//cam_sensor_cfg_line_length_pck = 3233
{0xC816, 0x00D4,2}, 		//cam_sensor_cfg_fine_correction = 212
{0xC818, 0x043B,2}, 		//cam_sensor_cfg_cpipe_last_row = 1083
{0xC820, 0x0010,2}, 		//cam_sensor_cfg_reg_0_data = 16
{0xC830, 0x0002,2}, 		//cam_sensor_control_read_mode = 2
{0xC858, 0x0000,2}, 		//cam_crop_window_xoffset = 0
{0xC85A, 0x0000,2}, 		//cam_crop_window_yoffset = 0
{0xC85C, 0x05A0,2}, 		//cam_crop_window_width = 1440
{0xC85E, 0x0438,2}, 		//cam_crop_window_height = 1080
{0xC86C, 0x0500,2}, 		//cam_output_width = 1280
{0xC86E, 0x03C0,2}, 		//cam_output_height = 960
{0xC870, 0x4010,2}, 		//cam_output_format = 16592
{0xC87C, 0x00,1}, 		//cam_aet_aemode = 0
{0xC88E, 0x1C00,2}, 		//cam_aet_max_frame_rate = 7168
{0xC890, 0x1C00,2}, 		//cam_aet_min_frame_rate = 7168
{0xC94C, 0x0000,2}, 		//cam_stat_awb_clip_window_xstart = 0
{0xC94E, 0x0000,2}, 		//cam_stat_awb_clip_window_ystart = 0
{0xC950, 0x04FF,2}, 		//cam_stat_awb_clip_window_xend = 1279
{0xC952, 0x03BF,2}, 		//cam_stat_awb_clip_window_yend = 959
{0xC954, 0x0000,2}, 		//cam_stat_ae_initial_window_xstart = 0
{0xC956, 0x0000,2}, 		//cam_stat_ae_initial_window_ystart = 0
{0xC958, 0x00FF,2}, 		//cam_stat_ae_initial_window_xend = 255
{0xC95A, 0x00BF,2}, 		//cam_stat_ae_initial_window_yend = 191

};

struct as0260_reg as0260_720[] =
{
{0x098E, 0xCA12,2},      // LOGICAL_ADDRESS_ACCESS [CAM_SYSCTL_PLL_ENABLE]
{0xCA12, 0x01,1},      //cam_sysctl_pll_enable = 1
{0xCA13, 0x00,0},      //cam_sysctl_pll_enable = 1
	
{0xCA14, 0x022B,2}, 		//cam_sysctl_pll_divider_m_n = 555
{0xCA16, 0x0070,2}, 		//cam_sysctl_pll_divider_p = 112
{0xCA18, 0x7F7C,2}, 		//cam_sysctl_pll_divider_p4_p5_p6 = 32636
{0xCA1C, 0x8043,2}, 		//cam_port_output_control = 32835
{0xCA1E, 0x0008,2}, 		//cam_port_porch = 8
{0xCA20, 0x0B00,2}, 		//cam_port_mipi_timing_t_hs_zero = 2816
{0xCA22, 0x0005,2}, 		//cam_port_mipi_timing_t_hs_exit_hs_trail = 5
{0xCA24, 0x0A01,2}, 		//cam_port_mipi_timing_t_clk_post_clk_pre = 2561
{0xCA26, 0x0515,2}, 		//cam_port_mipi_timing_t_clk_trail_clk_zero = 1301
{0xCA28, 0x0004,2}, 		//cam_port_mipi_timing_t_lpx = 4
{0xCA2A, 0x0709,2}, 		//cam_port_mipi_timing_init_timing = 1801
{0xCA2C, 0x0000,2}, 		//cam_port_mipi_timing_t_hs_pre = 0


//[Timing_settings]
{0xC800, 0x0020,2}, 		//cam_sensor_cfg_y_addr_start = 32
{0xC802, 0x0020,2}, 		//cam_sensor_cfg_x_addr_start = 32
{0xC804, 0x045F,2}, 		//cam_sensor_cfg_y_addr_end = 1119
{0xC806, 0x07A7,2}, 		//cam_sensor_cfg_x_addr_end = 1959
{0xC808, 0x03278AC4,4}, 		//cam_sensor_cfg_pixclk = 52923076
{0xC80C, 0x0001,2}, 		//cam_sensor_cfg_row_speed = 1
{0xC80E, 0x0336,2}, 		//cam_sensor_cfg_fine_integ_time_min = 822
{0xC810, 0x0A5C,2}, 		//cam_sensor_cfg_fine_integ_time_max = 2652
{0xC812, 0x04C4,2}, 		//cam_sensor_cfg_frame_length_lines = 1220
{0xC814, 0x0C1A,2}, 		//cam_sensor_cfg_line_length_pck = 3098
{0xC816, 0x00D4,2}, 		//cam_sensor_cfg_fine_correction = 212
{0xC818, 0x043B,2}, 		//cam_sensor_cfg_cpipe_last_row = 1083
{0xC820, 0x0010,2}, 		//cam_sensor_cfg_reg_0_data = 16
{0xC830, 0x0002,2}, 		//cam_sensor_control_read_mode = 2
{0xC858, 0x0000,2}, 		//cam_crop_window_xoffset = 0
{0xC85A, 0x0000,2}, 		//cam_crop_window_yoffset = 0
{0xC85C, 0x0780,2}, 		//cam_crop_window_width = 1920
{0xC85E, 0x0438,2}, 		//cam_crop_window_height = 1080
{0xC86C, 0x0500,2}, 		//cam_output_width = 1280
{0xC86E, 0x02D0,2}, 		//cam_output_height = 720
{0xC870, 0x4010,2}, 		//cam_output_format = 16592
{0xC87C, 0x00,1}, 		//cam_aet_aemode = 0
{0xC88E, 0x1C00,2}, 		//cam_aet_max_frame_rate = 7168
{0xC890, 0x1C00,2}, 		//cam_aet_min_frame_rate = 7168
{0xC94C, 0x0000,2}, 		//cam_stat_awb_clip_window_xstart = 0
{0xC94E, 0x0000,2}, 		//cam_stat_awb_clip_window_ystart = 0
{0xC950, 0x04FF,2}, 		//cam_stat_awb_clip_window_xend = 1279
{0xC952, 0x02CF,2}, 		//cam_stat_awb_clip_window_yend = 719
{0xC954, 0x0000,2}, 		//cam_stat_ae_initial_window_xstart = 0
{0xC956, 0x0000,2}, 		//cam_stat_ae_initial_window_ystart = 0
{0xC958, 0x00FF,2}, 		//cam_stat_ae_initial_window_xend = 255
{0xC95A, 0x008F,2}, 		//cam_stat_ae_initial_window_yend = 143
};

struct as0260_reg as0260_640[] =
{
{0x098E, 0xCA12,2},      // LOGICAL_ADDRESS_ACCESS [CAM_SYSCTL_PLL_ENABLE]
{0xCA12, 0x01,1},      //cam_sysctl_pll_enable = 1
{0xCA13, 0x00,0},      //cam_sysctl_pll_enable = 1
	
{0xCA14, 0x022B,2}, 		//cam_sysctl_pll_divider_m_n = 555
{0xCA16, 0x0070,2}, 		//cam_sysctl_pll_divider_p = 112
{0xCA18, 0x7F7C,2}, 		//cam_sysctl_pll_divider_p4_p5_p6 = 32636
{0xCA1C, 0x8043,2}, 		//cam_port_output_control = 32835
{0xCA1E, 0x0008,2}, 		//cam_port_porch = 8
{0xCA20, 0x0B00,2}, 		//cam_port_mipi_timing_t_hs_zero = 2816
{0xCA22, 0x0005,2}, 		//cam_port_mipi_timing_t_hs_exit_hs_trail = 5
{0xCA24, 0x0A01,2}, 		//cam_port_mipi_timing_t_clk_post_clk_pre = 2561
{0xCA26, 0x0515,2}, 		//cam_port_mipi_timing_t_clk_trail_clk_zero = 1301
{0xCA28, 0x0004,2}, 		//cam_port_mipi_timing_t_lpx = 4
{0xCA2A, 0x0709,2}, 		//cam_port_mipi_timing_init_timing = 1801
{0xCA2C, 0x0000,2}, 		//cam_port_mipi_timing_t_hs_pre = 0


//[Timing_settings]
{0xC800, 0x0020,2}, 		//cam_sensor_cfg_y_addr_start = 32
{0xC802, 0x0100,2}, 		//cam_sensor_cfg_x_addr_start = 256
{0xC804, 0x045D,2}, 		//cam_sensor_cfg_y_addr_end = 1117
{0xC806, 0x06AD,2}, 		//cam_sensor_cfg_x_addr_end = 1709
{0xC808, 0x03278AC4,4}, 		//cam_sensor_cfg_pixclk = 52923076
{0xC80C, 0x0001,2}, 		//cam_sensor_cfg_row_speed = 1
{0xC80E, 0x06A4,2}, 		//cam_sensor_cfg_fine_integ_time_min = 1700
{0xC810, 0x077A,2}, 		//cam_sensor_cfg_fine_integ_time_max = 1914
{0xC812, 0x056F,2}, 		//cam_sensor_cfg_frame_length_lines = 1391
{0xC814, 0x0A9C,2}, 		//cam_sensor_cfg_line_length_pck = 2716
{0xC816, 0x01D9,2}, 		//cam_sensor_cfg_fine_correction = 473
{0xC818, 0x021B,2}, 		//cam_sensor_cfg_cpipe_last_row = 539
{0xC820, 0x0010,2}, 		//cam_sensor_cfg_reg_0_data = 16
{0xC830, 0x0012,2}, 		//cam_sensor_control_read_mode = 18
{0xC858, 0x0000,2}, 		//cam_crop_window_xoffset = 0
{0xC85A, 0x0000,2}, 		//cam_crop_window_yoffset = 0
{0xC85C, 0x02D0,2}, 		//cam_crop_window_width = 720
{0xC85E, 0x0218,2}, 		//cam_crop_window_height = 536
{0xC86C, 0x0280,2}, 		//cam_output_width = 640
{0xC86E, 0x01E0,2}, 		//cam_output_height = 480
{0xC870, 0x4010,2}, 		//cam_output_format = 16592
{0xC87C, 0x00,1}, 		//cam_aet_aemode = 0
{0xC88E, 0x1C00,2}, 		//cam_aet_max_frame_rate = 7168
{0xC890, 0x1C00,2}, 		//cam_aet_min_frame_rate = 7168
{0xC94C, 0x0000,2}, 		//cam_stat_awb_clip_window_xstart = 0
{0xC94E, 0x0000,2}, 		//cam_stat_awb_clip_window_ystart = 0
{0xC950, 0x027F,2}, 		//cam_stat_awb_clip_window_xend = 639
{0xC952, 0x01DF,2}, 		//cam_stat_awb_clip_window_yend = 479
{0xC954, 0x0000,2}, 		//cam_stat_ae_initial_window_xstart = 0
{0xC956, 0x0000,2}, 		//cam_stat_ae_initial_window_ystart = 0
{0xC958, 0x007F,2}, 		//cam_stat_ae_initial_window_xend = 127
{0xC95A, 0x005F,2}, 		//cam_stat_ae_initial_window_yend = 95
};

struct as0260_reg as0260_320[] =
{
{0x098E, 0xCA12,2},      // LOGICAL_ADDRESS_ACCESS [CAM_SYSCTL_PLL_ENABLE]
{0xCA12, 0x01,1},      //cam_sysctl_pll_enable = 1
{0xCA13, 0x00,0},      //cam_sysctl_pll_enable = 1
	
{0xCA14, 0x022B,2}, 		//cam_sysctl_pll_divider_m_n = 555
{0xCA16, 0x0070,2}, 		//cam_sysctl_pll_divider_p = 112
{0xCA18, 0x7F7C,2}, 		//cam_sysctl_pll_divider_p4_p5_p6 = 32636
{0xCA1C, 0x8043,2}, 		//cam_port_output_control = 32835
{0xCA1E, 0x0008,2}, 		//cam_port_porch = 8
{0xCA20, 0x0B00,2}, 		//cam_port_mipi_timing_t_hs_zero = 2816
{0xCA22, 0x0005,2}, 		//cam_port_mipi_timing_t_hs_exit_hs_trail = 5
{0xCA24, 0x0A01,2}, 		//cam_port_mipi_timing_t_clk_post_clk_pre = 2561
{0xCA26, 0x0515,2}, 		//cam_port_mipi_timing_t_clk_trail_clk_zero = 1301
{0xCA28, 0x0004,2}, 		//cam_port_mipi_timing_t_lpx = 4
{0xCA2A, 0x0709,2}, 		//cam_port_mipi_timing_init_timing = 1801
{0xCA2C, 0x0000,2}, 		//cam_port_mipi_timing_t_hs_pre = 0


//[Timing_settings]
{0xC800, 0x0020,2}, 		//cam_sensor_cfg_y_addr_start = 32
{0xC802, 0x0100,2}, 		//cam_sensor_cfg_x_addr_start = 256
{0xC804, 0x045D,2}, 		//cam_sensor_cfg_y_addr_end = 1117
{0xC806, 0x06AD,2}, 		//cam_sensor_cfg_x_addr_end = 1709
{0xC808, 0x03278AC4,4}, 		//cam_sensor_cfg_pixclk = 52923076
{0xC80C, 0x0001,2}, 		//cam_sensor_cfg_row_speed = 1
{0xC80E, 0x06A4,2}, 		//cam_sensor_cfg_fine_integ_time_min = 1700
{0xC810, 0x077A,2}, 		//cam_sensor_cfg_fine_integ_time_max = 1914
{0xC812, 0x056F,2}, 		//cam_sensor_cfg_frame_length_lines = 1391
{0xC814, 0x0A9C,2}, 		//cam_sensor_cfg_line_length_pck = 2716
{0xC816, 0x01D9,2}, 		//cam_sensor_cfg_fine_correction = 473
{0xC818, 0x021B,2}, 		//cam_sensor_cfg_cpipe_last_row = 539
{0xC820, 0x0010,2}, 		//cam_sensor_cfg_reg_0_data = 16
{0xC830, 0x0012,2}, 		//cam_sensor_control_read_mode = 18
{0xC858, 0x0000,2}, 		//cam_crop_window_xoffset = 0
{0xC85A, 0x0000,2}, 		//cam_crop_window_yoffset = 0
{0xC85C, 0x02D0,2}, 		//cam_crop_window_width = 720
{0xC85E, 0x0218,2}, 		//cam_crop_window_height = 536
{0xC86C, 0x0140,2}, 		//cam_output_width = 320
{0xC86E, 0x00F0,2}, 		//cam_output_height = 240
{0xC870, 0x4010,2}, 		//cam_output_format = 16592
{0xC87C, 0x00,1}, 		//cam_aet_aemode = 0
{0xC88E, 0x1C00,2}, 		//cam_aet_max_frame_rate = 7168
{0xC890, 0x1C00,2}, 		//cam_aet_min_frame_rate = 7168
{0xC94C, 0x0000,2}, 		//cam_stat_awb_clip_window_xstart = 0
{0xC94E, 0x0000,2}, 		//cam_stat_awb_clip_window_ystart = 0
{0xC950, 0x013F,2}, 		//cam_stat_awb_clip_window_xend = 319
{0xC952, 0x00EF,2}, 		//cam_stat_awb_clip_window_yend = 239
{0xC954, 0x0000,2}, 		//cam_stat_ae_initial_window_xstart = 0
{0xC956, 0x0000,2}, 		//cam_stat_ae_initial_window_ystart = 0
{0xC958, 0x003F,2}, 		//cam_stat_ae_initial_window_xend = 63
{0xC95A, 0x002F,2}, 		//cam_stat_ae_initial_window_yend = 47

};
// config resetting 
struct as0260_reg as0260_176[] =
{
	{0x098E, 0xCA12,2},      // LOGICAL_ADDRESS_ACCESS [CAM_SYSCTL_PLL_ENABLE]
	{0xCA12, 0x01,1},      //cam_sysctl_pll_enable = 1
	{0xCA13, 0x00,0},      //cam_sysctl_pll_enable = 1
	
{0xCA14, 0x022E,2}, 		//cam_sysctl_pll_divider_m_n = 558
{0xCA16, 0x0070,2}, 		//cam_sysctl_pll_divider_p = 112
{0xCA18, 0x7F7C,2}, 		//cam_sysctl_pll_divider_p4_p5_p6 = 32636
{0xCA1C, 0x8043,2}, 		//cam_port_output_control = 32835
{0xCA1E, 0x0008,2}, 		//cam_port_porch = 8
{0xCA20, 0x0C00,2}, 		//cam_port_mipi_timing_t_hs_zero = 3072
{0xCA22, 0x0006,2}, 		//cam_port_mipi_timing_t_hs_exit_hs_trail = 6
{0xCA24, 0x0B01,2}, 		//cam_port_mipi_timing_t_clk_post_clk_pre = 2817
{0xCA26, 0x0517,2}, 		//cam_port_mipi_timing_t_clk_trail_clk_zero = 1303
{0xCA28, 0x0005,2}, 		//cam_port_mipi_timing_t_lpx = 5
{0xCA2A, 0x0809,2}, 		//cam_port_mipi_timing_init_timing = 2057
{0xCA2C, 0x0000,2}, 		//cam_port_mipi_timing_t_hs_pre = 0

{0xC800, 0x0020,2}, 		//cam_sensor_cfg_y_addr_start = 32
{0xC802, 0x0100,2}, 		//cam_sensor_cfg_x_addr_start = 256
{0xC804, 0x045D,2}, 		//cam_sensor_cfg_y_addr_end = 1117
{0xC806, 0x06AD,2}, 		//cam_sensor_cfg_x_addr_end = 1709
{0xC808, 0x035FE1D8,4}, 		//cam_sensor_cfg_pixclk = 56615384
{0xC80C, 0x0001,2}, 		//cam_sensor_cfg_row_speed = 1
{0xC80E, 0x06A4,2}, 		//cam_sensor_cfg_fine_integ_time_min = 1700
{0xC810, 0x1842,2}, 		//cam_sensor_cfg_fine_integ_time_max = 6210
{0xC812, 0x026D,2}, 		//cam_sensor_cfg_frame_length_lines = 621
{0xC814, 0x1B64,2}, 		//cam_sensor_cfg_line_length_pck = 7012
{0xC816, 0x01D9,2}, 		//cam_sensor_cfg_fine_correction = 473
{0xC818, 0x021B,2}, 		//cam_sensor_cfg_cpipe_last_row = 539
{0xC820, 0x0010,2}, 		//cam_sensor_cfg_reg_0_data = 16
{0xC830, 0x0012,2}, 		//cam_sensor_control_read_mode = 18
{0xC858, 0x0000,2}, 		//cam_crop_window_xoffset = 0
{0xC85A, 0x0000,2}, 		//cam_crop_window_yoffset = 0
{0xC85C, 0x02D0,2}, 		//cam_crop_window_width = 720
{0xC85E, 0x0218,2}, 		//cam_crop_window_height = 536
{0xC86C, 0x00b0,2}, 		//cam_output_width = 320
{0xC86E, 0x0090,2}, 		//cam_output_height = 240
{0xC870, 0x4010,2},	//cam_output_format = 16592
{0xC87C, 0x00,1}, 		//cam_aet_aemode = 0
{0xC88E, 0x1A00,2}, 		//cam_aet_max_frame_rate = 6656
{0xC890, 0x1A00,2}, 		//cam_aet_min_frame_rate = 6656
{0xC94C, 0x0000,2}, 		//cam_stat_awb_clip_window_xstart = 0
{0xC94E, 0x0000,2}, 		//cam_stat_awb_clip_window_ystart = 0
{0xC950, 0x00b0,2}, 		//cam_stat_awb_clip_window_xend = 319
{0xC952, 0x0090,2}, 		//cam_stat_awb_clip_window_yend = 239
};

#elif defined(FEATURE_TW_CAMERA_FPS28_MCLK24_PCLK92_MIPI1_H)
struct as0260_reg as0260_1080[] =
{
{0x098E, 0xCA12,2},      // LOGICAL_ADDRESS_ACCESS [CAM_SYSCTL_PLL_ENABLE]
{0xCA12, 0x01,1},      //cam_sysctl_pll_enable = 1
{0xCA13, 0x00,1},      //cam_sysctl_pll_enable = 1

{0xCA14, 0x0010,2}, 		//cam_sysctl_pll_divider_m_n = 16
{0xCA16, 0x0070,2}, 		//cam_sysctl_pll_divider_p = 112
{0xCA18, 0x7F7C,2}, 		//cam_sysctl_pll_divider_p4_p5_p6 = 32636
{0xCA1C, 0x8041,2}, 		//cam_port_output_control = 32833
{0xCA1E, 0x0008,2}, 		//cam_port_porch = 8
{0xCA20, 0x0D00,2}, 		//cam_port_mipi_timing_t_hs_zero = 3328
{0xCA22, 0x0006,2}, 		//cam_port_mipi_timing_t_hs_exit_hs_trail = 6
{0xCA24, 0x0B01,2}, 		//cam_port_mipi_timing_t_clk_post_clk_pre = 2817
{0xCA26, 0x0618,2}, 		//cam_port_mipi_timing_t_clk_trail_clk_zero = 1560
{0xCA28, 0x0005,2}, 		//cam_port_mipi_timing_t_lpx = 5
{0xCA2A, 0x080A,2}, 		//cam_port_mipi_timing_init_timing = 2058
{0xCA2C, 0x0000,2}, 		//cam_port_mipi_timing_t_hs_pre = 0


//[Timing_settings]
{0xC800, 0x0020,2}, 		//cam_sensor_cfg_y_addr_start = 32
{0xC802, 0x0020,2}, 		//cam_sensor_cfg_x_addr_start = 32
{0xC804, 0x045F,2}, 		//cam_sensor_cfg_y_addr_end = 1119
{0xC806, 0x07A7,2}, 		//cam_sensor_cfg_x_addr_end = 1959
{0xC808, 0x0385713B,4}, 		//cam_sensor_cfg_pixclk = 59076923
{0xC80C, 0x0001,2}, 		//cam_sensor_cfg_row_speed = 1
{0xC80E, 0x0336,2}, 		//cam_sensor_cfg_fine_integ_time_min = 822
{0xC810, 0x0A5C,2}, 		//cam_sensor_cfg_fine_integ_time_max = 2652
{0xC812, 0x04F7,2}, 		//cam_sensor_cfg_frame_length_lines = 1271
{0xC814, 0x0C1A,2}, 		//cam_sensor_cfg_line_length_pck = 3098
{0xC816, 0x00D4,2}, 		//cam_sensor_cfg_fine_correction = 212
{0xC818, 0x043B,2}, 		//cam_sensor_cfg_cpipe_last_row = 1083
{0xC820, 0x0010,2}, 		//cam_sensor_cfg_reg_0_data = 16
{0xC830, 0x0002,2}, 		//cam_sensor_control_read_mode = 2
{0xC858, 0x0000,2}, 		//cam_crop_window_xoffset = 0
{0xC85A, 0x0000,2}, 		//cam_crop_window_yoffset = 0
{0xC85C, 0x0780,2}, 		//cam_crop_window_width = 1920
{0xC85E, 0x0438,2}, 		//cam_crop_window_height = 1080
{0xC86C, 0x0780,2}, 		//cam_output_width = 1920
{0xC86E, 0x0438,2}, 		//cam_output_height = 1080
{0xC870, 0x4010,2}, 		//cam_output_format = 16592
{0xC87C, 0x00,1}, 		//cam_aet_aemode = 0
{0xC88E, 0x1E00,2}, 		//cam_aet_max_frame_rate = 7680
{0xC890, 0x1E00,2}, 		//cam_aet_min_frame_rate = 7680
{0xC94C, 0x0000,2}, 		//cam_stat_awb_clip_window_xstart = 0
{0xC94E, 0x0000,2}, 		//cam_stat_awb_clip_window_ystart = 0
{0xC950, 0x077F,2}, 		//cam_stat_awb_clip_window_xend = 1919
{0xC952, 0x0437,2}, 		//cam_stat_awb_clip_window_yend = 1079
{0xC954, 0x0000,2}, 		//cam_stat_ae_initial_window_xstart = 0
{0xC956, 0x0000,2}, 		//cam_stat_ae_initial_window_ystart = 0
{0xC958, 0x017F,2}, 		//cam_stat_ae_initial_window_xend = 383
{0xC95A, 0x00D7,2}, 		//cam_stat_ae_initial_window_yend = 215
};

struct as0260_reg as0260_1440[] =
{
{0x098E, 0xCA12,2},      // LOGICAL_ADDRESS_ACCESS [CAM_SYSCTL_PLL_ENABLE]
{0xCA12, 0x01,1},      //cam_sysctl_pll_enable = 1
{0xCA13, 0x00,1},      //cam_sysctl_pll_enable = 1

{0xCA14, 0x0010,2}, 		//cam_sysctl_pll_divider_m_n = 16
{0xCA16, 0x0070,2}, 		//cam_sysctl_pll_divider_p = 112
{0xCA18, 0x7F7C,2}, 		//cam_sysctl_pll_divider_p4_p5_p6 = 32636
{0xCA1C, 0x8041,2}, 		//cam_port_output_control = 32833
{0xCA1E, 0x0008,2}, 		//cam_port_porch = 8
{0xCA20, 0x0D00,2}, 		//cam_port_mipi_timing_t_hs_zero = 3328
{0xCA22, 0x0006,2}, 		//cam_port_mipi_timing_t_hs_exit_hs_trail = 6
{0xCA24, 0x0B01,2}, 		//cam_port_mipi_timing_t_clk_post_clk_pre = 2817
{0xCA26, 0x0618,2}, 		//cam_port_mipi_timing_t_clk_trail_clk_zero = 1560
{0xCA28, 0x0005,2}, 		//cam_port_mipi_timing_t_lpx = 5
{0xCA2A, 0x080A,2}, 		//cam_port_mipi_timing_init_timing = 2058
{0xCA2C, 0x0000,2}, 		//cam_port_mipi_timing_t_hs_pre = 0


//[Timing_settings]
{0xC800, 0x0020,2}, 		//cam_sensor_cfg_y_addr_start = 32
{0xC802, 0x0020,2}, 		//cam_sensor_cfg_x_addr_start = 32
{0xC804, 0x045F,2}, 		//cam_sensor_cfg_y_addr_end = 1119
{0xC806, 0x07A7,2}, 		//cam_sensor_cfg_x_addr_end = 1959
{0xC808, 0x0385713B,4}, 		//cam_sensor_cfg_pixclk = 59076923
{0xC80C, 0x0001,2}, 		//cam_sensor_cfg_row_speed = 1
{0xC80E, 0x0336,2}, 		//cam_sensor_cfg_fine_integ_time_min = 822
{0xC810, 0x0A5C,2}, 		//cam_sensor_cfg_fine_integ_time_max = 2652
{0xC812, 0x04F7,2}, 		//cam_sensor_cfg_frame_length_lines = 1271
{0xC814, 0x0C1A,2}, 		//cam_sensor_cfg_line_length_pck = 3098
{0xC816, 0x00D4,2}, 		//cam_sensor_cfg_fine_correction = 212
{0xC818, 0x043B,2}, 		//cam_sensor_cfg_cpipe_last_row = 1083
{0xC820, 0x0010,2}, 		//cam_sensor_cfg_reg_0_data = 16
{0xC830, 0x0002,2}, 		//cam_sensor_control_read_mode = 2
{0xC858, 0x0000,2}, 		//cam_crop_window_xoffset = 0
{0xC85A, 0x0000,2}, 		//cam_crop_window_yoffset = 0
{0xC85C, 0x0780,2}, 		//cam_crop_window_width = 1920
{0xC85E, 0x0438,2}, 		//cam_crop_window_height = 1080
{0xC86C, 0x0780,2}, 		//cam_output_width = 1920
{0xC86E, 0x0438,2}, 		//cam_output_height = 1080
{0xC870, 0x4010,2}, 		//cam_output_format = 16592
{0xC87C, 0x00,1}, 		//cam_aet_aemode = 0
{0xC88E, 0x1E00,2}, 		//cam_aet_max_frame_rate = 7680
{0xC890, 0x1E00,2}, 		//cam_aet_min_frame_rate = 7680
{0xC94C, 0x0000,2}, 		//cam_stat_awb_clip_window_xstart = 0
{0xC94E, 0x0000,2}, 		//cam_stat_awb_clip_window_ystart = 0
{0xC950, 0x077F,2}, 		//cam_stat_awb_clip_window_xend = 1919
{0xC952, 0x0437,2}, 		//cam_stat_awb_clip_window_yend = 1079
{0xC954, 0x0000,2}, 		//cam_stat_ae_initial_window_xstart = 0
{0xC956, 0x0000,2}, 		//cam_stat_ae_initial_window_ystart = 0
{0xC958, 0x017F,2}, 		//cam_stat_ae_initial_window_xend = 383
{0xC95A, 0x00D7,2}, 		//cam_stat_ae_initial_window_yend = 215
};


struct as0260_reg as0260_960[] =
{
	{0x098E, 0xCA12,2},      // LOGICAL_ADDRESS_ACCESS [CAM_SYSCTL_PLL_ENABLE]
	{0xCA12, 0x01,1},      //cam_sysctl_pll_enable = 1
	{0xCA13, 0x00,0},      //cam_sysctl_pll_enable = 1
	
{0xCA14, 0x0010,2}, 		//cam_sysctl_pll_divider_m_n = 16
{0xCA16, 0x0070,2}, 		//cam_sysctl_pll_divider_p = 112
{0xCA18, 0x7F7C,2}, 		//cam_sysctl_pll_divider_p4_p5_p6 = 32636
{0xCA1C, 0x8041,2}, 		//cam_port_output_control = 32833
{0xCA1E, 0x0008,2}, 		//cam_port_porch = 8
{0xCA20, 0x0D00,2}, 		//cam_port_mipi_timing_t_hs_zero = 3328
{0xCA22, 0x0006,2}, 		//cam_port_mipi_timing_t_hs_exit_hs_trail = 6
{0xCA24, 0x0B01,2}, 		//cam_port_mipi_timing_t_clk_post_clk_pre = 2817
{0xCA26, 0x0618,2}, 		//cam_port_mipi_timing_t_clk_trail_clk_zero = 1560
{0xCA28, 0x0005,2}, 		//cam_port_mipi_timing_t_lpx = 5
{0xCA2A, 0x080A,2}, 		//cam_port_mipi_timing_init_timing = 2058
{0xCA2C, 0x0000,2}, 		//cam_port_mipi_timing_t_hs_pre = 0


//[Timing_settings]
{0xC800, 0x0020,2}, 		//cam_sensor_cfg_y_addr_start = 32
{0xC802, 0x0120,2}, 		//cam_sensor_cfg_x_addr_start = 288
{0xC804, 0x045F,2}, 		//cam_sensor_cfg_y_addr_end = 1119
{0xC806, 0x06C7,2}, 		//cam_sensor_cfg_x_addr_end = 1735
{0xC808, 0x0385713B,4}, 		//cam_sensor_cfg_pixclk = 59076923
{0xC80C, 0x0001,2}, 		//cam_sensor_cfg_row_speed = 1
{0xC80E, 0x0336,2}, 		//cam_sensor_cfg_fine_integ_time_min = 822
{0xC810, 0x087C,2}, 		//cam_sensor_cfg_fine_integ_time_max = 2172
{0xC812, 0x05E0,2}, 		//cam_sensor_cfg_frame_length_lines = 1504
{0xC814, 0x0A3A,2}, 		//cam_sensor_cfg_line_length_pck = 2618
{0xC816, 0x00D4,2}, 		//cam_sensor_cfg_fine_correction = 212
{0xC818, 0x043B,2}, 		//cam_sensor_cfg_cpipe_last_row = 1083
{0xC820, 0x0010,2}, 		//cam_sensor_cfg_reg_0_data = 16
{0xC830, 0x0002,2}, 		//cam_sensor_control_read_mode = 2
{0xC858, 0x0000,2}, 		//cam_crop_window_xoffset = 0
{0xC85A, 0x0000,2}, 		//cam_crop_window_yoffset = 0
{0xC85C, 0x05A0,2}, 		//cam_crop_window_width = 1440
{0xC85E, 0x0438,2}, 		//cam_crop_window_height = 1080
{0xC86C, 0x0500,2}, 		//cam_output_width = 1280
{0xC86E, 0x03C0,2}, 		//cam_output_height = 960
{0xC870, 0x4010,2}, 		//cam_output_format = 16592
{0xC87C, 0x00,1}, 		//cam_aet_aemode = 0
{0xC88E, 0x1E00,2}, 		//cam_aet_max_frame_rate = 7680
{0xC890, 0x1E00,2}, 		//cam_aet_min_frame_rate = 7680
{0xC94C, 0x0000,2}, 		//cam_stat_awb_clip_window_xstart = 0
{0xC94E, 0x0000,2}, 		//cam_stat_awb_clip_window_ystart = 0
{0xC950, 0x04FF,2}, 		//cam_stat_awb_clip_window_xend = 1279
{0xC952, 0x03BF,2}, 		//cam_stat_awb_clip_window_yend = 959
{0xC954, 0x0000,2}, 		//cam_stat_ae_initial_window_xstart = 0
{0xC956, 0x0000,2}, 		//cam_stat_ae_initial_window_ystart = 0
{0xC958, 0x00FF,2}, 		//cam_stat_ae_initial_window_xend = 255
{0xC95A, 0x00BF,2}, 		//cam_stat_ae_initial_window_yend = 191

};

struct as0260_reg as0260_720[] =
{
{0x098E, 0xCA12,2},      // LOGICAL_ADDRESS_ACCESS [CAM_SYSCTL_PLL_ENABLE]
{0xCA12, 0x01,1},      //cam_sysctl_pll_enable = 1
{0xCA13, 0x00,0},      //cam_sysctl_pll_enable = 1
	
{0xCA14, 0x0010,2}, 		//cam_sysctl_pll_divider_m_n = 16
{0xCA16, 0x0070,2}, 		//cam_sysctl_pll_divider_p = 112
{0xCA18, 0x7F7C,2}, 		//cam_sysctl_pll_divider_p4_p5_p6 = 32636
{0xCA1C, 0x8041,2}, 		//cam_port_output_control = 32833
{0xCA1E, 0x0008,2}, 		//cam_port_porch = 8
{0xCA20, 0x0D00,2}, 		//cam_port_mipi_timing_t_hs_zero = 3328
{0xCA22, 0x0006,2}, 		//cam_port_mipi_timing_t_hs_exit_hs_trail = 6
{0xCA24, 0x0B01,2}, 		//cam_port_mipi_timing_t_clk_post_clk_pre = 2817
{0xCA26, 0x0618,2}, 		//cam_port_mipi_timing_t_clk_trail_clk_zero = 1560
{0xCA28, 0x0005,2}, 		//cam_port_mipi_timing_t_lpx = 5
{0xCA2A, 0x080A,2}, 		//cam_port_mipi_timing_init_timing = 2058
{0xCA2C, 0x0000,2}, 		//cam_port_mipi_timing_t_hs_pre = 0


//[Timing_settings]
{0xC800, 0x00E0,2}, 		//cam_sensor_cfg_y_addr_start = 224
{0xC802, 0x0160,2}, 		//cam_sensor_cfg_x_addr_start = 352
{0xC804, 0x03B7,2}, 		//cam_sensor_cfg_y_addr_end = 951
{0xC806, 0x0667,2}, 		//cam_sensor_cfg_x_addr_end = 1639
{0xC808, 0x0385713B,4}, 		//cam_sensor_cfg_pixclk = 59076923
{0xC80C, 0x0001,2}, 		//cam_sensor_cfg_row_speed = 1
{0xC80E, 0x0336,2}, 		//cam_sensor_cfg_fine_integ_time_min = 822
{0xC810, 0x07DC,2}, 		//cam_sensor_cfg_fine_integ_time_max = 2012
{0xC812, 0x0642,2}, 		//cam_sensor_cfg_frame_length_lines = 1602
{0xC814, 0x099A,2}, 		//cam_sensor_cfg_line_length_pck = 2458
{0xC816, 0x00D4,2}, 		//cam_sensor_cfg_fine_correction = 212
{0xC818, 0x02D3,2}, 		//cam_sensor_cfg_cpipe_last_row = 723
{0xC820, 0x0010,2}, 		//cam_sensor_cfg_reg_0_data = 16
{0xC830, 0x0002,2}, 		//cam_sensor_control_read_mode = 2
{0xC858, 0x0000,2}, 		//cam_crop_window_xoffset = 0
{0xC85A, 0x0000,2}, 		//cam_crop_window_yoffset = 0
{0xC85C, 0x0500,2}, 		//cam_crop_window_width = 1280
{0xC85E, 0x02D0,2}, 		//cam_crop_window_height = 720
{0xC86C, 0x0500,2}, 		//cam_output_width = 1280
{0xC86E, 0x02D0,2}, 		//cam_output_height = 720
{0xC870, 0x4010,2}, 		//cam_output_format = 16592
{0xC87C, 0x00,1}, 		//cam_aet_aemode = 0
{0xC88E, 0x1E00,2}, 		//cam_aet_max_frame_rate = 7680
{0xC890, 0x1E00,2}, 		//cam_aet_min_frame_rate = 7680
{0xC94C, 0x0000,2}, 		//cam_stat_awb_clip_window_xstart = 0
{0xC94E, 0x0000,2}, 		//cam_stat_awb_clip_window_ystart = 0
{0xC950, 0x04FF,2}, 		//cam_stat_awb_clip_window_xend = 1279
{0xC952, 0x02CF,2}, 		//cam_stat_awb_clip_window_yend = 719
{0xC954, 0x0000,2}, 		//cam_stat_ae_initial_window_xstart = 0
{0xC956, 0x0000,2}, 		//cam_stat_ae_initial_window_ystart = 0
{0xC958, 0x00FF,2}, 		//cam_stat_ae_initial_window_xend = 255
{0xC95A, 0x008F,2}, 		//cam_stat_ae_initial_window_yend = 143
};

struct as0260_reg as0260_640[] =
{
{0x098E, 0xCA12,2},      // LOGICAL_ADDRESS_ACCESS [CAM_SYSCTL_PLL_ENABLE]
{0xCA12, 0x01,1},      //cam_sysctl_pll_enable = 1
{0xCA13, 0x00,0},      //cam_sysctl_pll_enable = 1
	
{0xCA14, 0x0010,2}, 		//cam_sysctl_pll_divider_m_n = 16
{0xCA16, 0x0070,2}, 		//cam_sysctl_pll_divider_p = 112
{0xCA18, 0x7F7C,2}, 		//cam_sysctl_pll_divider_p4_p5_p6 = 32636
{0xCA1C, 0x8041,2}, 		//cam_port_output_control = 32833
{0xCA1E, 0x0008,2}, 		//cam_port_porch = 8
{0xCA20, 0x0D00,2}, 		//cam_port_mipi_timing_t_hs_zero = 3328
{0xCA22, 0x0006,2}, 		//cam_port_mipi_timing_t_hs_exit_hs_trail = 6
{0xCA24, 0x0B01,2}, 		//cam_port_mipi_timing_t_clk_post_clk_pre = 2817
{0xCA26, 0x0618,2}, 		//cam_port_mipi_timing_t_clk_trail_clk_zero = 1560
{0xCA28, 0x0005,2}, 		//cam_port_mipi_timing_t_lpx = 5
{0xCA2A, 0x080A,2}, 		//cam_port_mipi_timing_init_timing = 2058
{0xCA2C, 0x0000,2}, 		//cam_port_mipi_timing_t_hs_pre = 0


//[Timing_settings]
{0xC800, 0x0020,2}, 		//cam_sensor_cfg_y_addr_start = 32
{0xC802, 0x0100,2}, 		//cam_sensor_cfg_x_addr_start = 256
{0xC804, 0x045D,2}, 		//cam_sensor_cfg_y_addr_end = 1117
{0xC806, 0x06AD,2}, 		//cam_sensor_cfg_x_addr_end = 1709
{0xC808, 0x0385713B,4}, 		//cam_sensor_cfg_pixclk = 59076923
{0xC80C, 0x0001,2}, 		//cam_sensor_cfg_row_speed = 1
{0xC80E, 0x06A4,2}, 		//cam_sensor_cfg_fine_integ_time_min = 1700
{0xC810, 0x077A,2}, 		//cam_sensor_cfg_fine_integ_time_max = 1914
{0xC812, 0x05AA,2}, 		//cam_sensor_cfg_frame_length_lines = 1450
{0xC814, 0x0A9C,2}, 		//cam_sensor_cfg_line_length_pck = 2716
{0xC816, 0x01D9,2}, 		//cam_sensor_cfg_fine_correction = 473
{0xC818, 0x021B,2}, 		//cam_sensor_cfg_cpipe_last_row = 539
{0xC820, 0x0010,2}, 		//cam_sensor_cfg_reg_0_data = 16
{0xC830, 0x0012,2}, 		//cam_sensor_control_read_mode = 18
{0xC858, 0x0000,2}, 		//cam_crop_window_xoffset = 0
{0xC85A, 0x0000,2}, 		//cam_crop_window_yoffset = 0
{0xC85C, 0x02D0,2}, 		//cam_crop_window_width = 720
{0xC85E, 0x0218,2}, 		//cam_crop_window_height = 536
{0xC86C, 0x0280,2}, 		//cam_output_width = 640
{0xC86E, 0x01E0,2}, 		//cam_output_height = 480
{0xC870, 0x4010,2}, 		//cam_output_format = 16592
{0xC87C, 0x00,1}, 		//cam_aet_aemode = 0
{0xC88E, 0x1E00,2}, 		//cam_aet_max_frame_rate = 7680
{0xC890, 0x1E00,2}, 		//cam_aet_min_frame_rate = 7680
{0xC94C, 0x0000,2}, 		//cam_stat_awb_clip_window_xstart = 0
{0xC94E, 0x0000,2}, 		//cam_stat_awb_clip_window_ystart = 0
{0xC950, 0x027F,2}, 		//cam_stat_awb_clip_window_xend = 639
{0xC952, 0x01DF,2}, 		//cam_stat_awb_clip_window_yend = 479
{0xC954, 0x0000,2}, 		//cam_stat_ae_initial_window_xstart = 0
{0xC956, 0x0000,2}, 		//cam_stat_ae_initial_window_ystart = 0
{0xC958, 0x007F,2}, 		//cam_stat_ae_initial_window_xend = 127
{0xC95A, 0x005F,2}, 		//cam_stat_ae_initial_window_yend = 95
};

struct as0260_reg as0260_320[] =
{
{0x098E, 0xCA12,2},      // LOGICAL_ADDRESS_ACCESS [CAM_SYSCTL_PLL_ENABLE]
{0xCA12, 0x01,1},      //cam_sysctl_pll_enable = 1
{0xCA13, 0x00,0},      //cam_sysctl_pll_enable = 1
	
{0xCA14, 0x0010,2}, 		//cam_sysctl_pll_divider_m_n = 16
{0xCA16, 0x0070,2}, 		//cam_sysctl_pll_divider_p = 112
{0xCA18, 0x7F7C,2}, 		//cam_sysctl_pll_divider_p4_p5_p6 = 32636
{0xCA1C, 0x8041,2}, 		//cam_port_output_control = 32833
{0xCA1E, 0x0008,2}, 		//cam_port_porch = 8
{0xCA20, 0x0D00,2}, 		//cam_port_mipi_timing_t_hs_zero = 3328
{0xCA22, 0x0006,2}, 		//cam_port_mipi_timing_t_hs_exit_hs_trail = 6
{0xCA24, 0x0B01,2}, 		//cam_port_mipi_timing_t_clk_post_clk_pre = 2817
{0xCA26, 0x0618,2}, 		//cam_port_mipi_timing_t_clk_trail_clk_zero = 1560
{0xCA28, 0x0005,2}, 		//cam_port_mipi_timing_t_lpx = 5
{0xCA2A, 0x080A,2}, 		//cam_port_mipi_timing_init_timing = 2058
{0xCA2C, 0x0000,2}, 		//cam_port_mipi_timing_t_hs_pre = 0


//[Timing_settings]
{0xC800, 0x0020,2}, 		//cam_sensor_cfg_y_addr_start = 32
{0xC802, 0x0100,2}, 		//cam_sensor_cfg_x_addr_start = 256
{0xC804, 0x045D,2}, 		//cam_sensor_cfg_y_addr_end = 1117
{0xC806, 0x06AD,2}, 		//cam_sensor_cfg_x_addr_end = 1709
{0xC808, 0x0385713B,4}, 		//cam_sensor_cfg_pixclk = 59076923
{0xC80C, 0x0001,2}, 		//cam_sensor_cfg_row_speed = 1
{0xC80E, 0x06A4,2}, 		//cam_sensor_cfg_fine_integ_time_min = 1700
{0xC810, 0x077A,2}, 		//cam_sensor_cfg_fine_integ_time_max = 1914
{0xC812, 0x05AA,2}, 		//cam_sensor_cfg_frame_length_lines = 1450
{0xC814, 0x0A9C,2}, 		//cam_sensor_cfg_line_length_pck = 2716
{0xC816, 0x01D9,2}, 		//cam_sensor_cfg_fine_correction = 473
{0xC818, 0x021B,2}, 		//cam_sensor_cfg_cpipe_last_row = 539
{0xC820, 0x0010,2}, 		//cam_sensor_cfg_reg_0_data = 16
{0xC830, 0x0012,2}, 		//cam_sensor_control_read_mode = 18
{0xC858, 0x0000,2}, 		//cam_crop_window_xoffset = 0
{0xC85A, 0x0000,2}, 		//cam_crop_window_yoffset = 0
{0xC85C, 0x02D0,2}, 		//cam_crop_window_width = 720
{0xC85E, 0x0218,2}, 		//cam_crop_window_height = 536
{0xC86C, 0x0140,2}, 		//cam_output_width = 320
{0xC86E, 0x00F0,2}, 		//cam_output_height = 240
{0xC870, 0x4010,2}, 		//cam_output_format = 16592
{0xC87C, 0x00,1}, 		//cam_aet_aemode = 0
{0xC88E, 0x1E00,2}, 		//cam_aet_max_frame_rate = 7680
{0xC890, 0x1E00,2}, 		//cam_aet_min_frame_rate = 7680
{0xC94C, 0x0000,2}, 		//cam_stat_awb_clip_window_xstart = 0
{0xC94E, 0x0000,2}, 		//cam_stat_awb_clip_window_ystart = 0
{0xC950, 0x013F,2}, 		//cam_stat_awb_clip_window_xend = 319
{0xC952, 0x00EF,2}, 		//cam_stat_awb_clip_window_yend = 239
{0xC954, 0x0000,2}, 		//cam_stat_ae_initial_window_xstart = 0
{0xC956, 0x0000,2}, 		//cam_stat_ae_initial_window_ystart = 0
{0xC958, 0x003F,2}, 		//cam_stat_ae_initial_window_xend = 63
{0xC95A, 0x002F,2}, 		//cam_stat_ae_initial_window_yend = 47

};
// config resetting 
struct as0260_reg as0260_176[] =
{
	{0x098E, 0xCA12,2},      // LOGICAL_ADDRESS_ACCESS [CAM_SYSCTL_PLL_ENABLE]
	{0xCA12, 0x01,1},      //cam_sysctl_pll_enable = 1
	{0xCA13, 0x00,0},      //cam_sysctl_pll_enable = 1
	
{0xCA14, 0x022E,2}, 		//cam_sysctl_pll_divider_m_n = 558
{0xCA16, 0x0070,2}, 		//cam_sysctl_pll_divider_p = 112
{0xCA18, 0x7F7C,2}, 		//cam_sysctl_pll_divider_p4_p5_p6 = 32636
{0xCA1C, 0x8043,2}, 		//cam_port_output_control = 32835
{0xCA1E, 0x0008,2}, 		//cam_port_porch = 8
{0xCA20, 0x0C00,2}, 		//cam_port_mipi_timing_t_hs_zero = 3072
{0xCA22, 0x0006,2}, 		//cam_port_mipi_timing_t_hs_exit_hs_trail = 6
{0xCA24, 0x0B01,2}, 		//cam_port_mipi_timing_t_clk_post_clk_pre = 2817
{0xCA26, 0x0517,2}, 		//cam_port_mipi_timing_t_clk_trail_clk_zero = 1303
{0xCA28, 0x0005,2}, 		//cam_port_mipi_timing_t_lpx = 5
{0xCA2A, 0x0809,2}, 		//cam_port_mipi_timing_init_timing = 2057
{0xCA2C, 0x0000,2}, 		//cam_port_mipi_timing_t_hs_pre = 0

{0xC800, 0x0020,2}, 		//cam_sensor_cfg_y_addr_start = 32
{0xC802, 0x0100,2}, 		//cam_sensor_cfg_x_addr_start = 256
{0xC804, 0x045D,2}, 		//cam_sensor_cfg_y_addr_end = 1117
{0xC806, 0x06AD,2}, 		//cam_sensor_cfg_x_addr_end = 1709
{0xC808, 0x035FE1D8,4}, 		//cam_sensor_cfg_pixclk = 56615384
{0xC80C, 0x0001,2}, 		//cam_sensor_cfg_row_speed = 1
{0xC80E, 0x06A4,2}, 		//cam_sensor_cfg_fine_integ_time_min = 1700
{0xC810, 0x1842,2}, 		//cam_sensor_cfg_fine_integ_time_max = 6210
{0xC812, 0x026D,2}, 		//cam_sensor_cfg_frame_length_lines = 621
{0xC814, 0x1B64,2}, 		//cam_sensor_cfg_line_length_pck = 7012
{0xC816, 0x01D9,2}, 		//cam_sensor_cfg_fine_correction = 473
{0xC818, 0x021B,2}, 		//cam_sensor_cfg_cpipe_last_row = 539
{0xC820, 0x0010,2}, 		//cam_sensor_cfg_reg_0_data = 16
{0xC830, 0x0012,2}, 		//cam_sensor_control_read_mode = 18
{0xC858, 0x0000,2}, 		//cam_crop_window_xoffset = 0
{0xC85A, 0x0000,2}, 		//cam_crop_window_yoffset = 0
{0xC85C, 0x02D0,2}, 		//cam_crop_window_width = 720
{0xC85E, 0x0218,2}, 		//cam_crop_window_height = 536
{0xC86C, 0x00b0,2}, 		//cam_output_width = 320
{0xC86E, 0x0090,2}, 		//cam_output_height = 240
{0xC870, 0x4010,2},	//cam_output_format = 16592
{0xC87C, 0x00,1}, 		//cam_aet_aemode = 0
{0xC88E, 0x1A00,2}, 		//cam_aet_max_frame_rate = 6656
{0xC890, 0x1A00,2}, 		//cam_aet_min_frame_rate = 6656
{0xC94C, 0x0000,2}, 		//cam_stat_awb_clip_window_xstart = 0
{0xC94E, 0x0000,2}, 		//cam_stat_awb_clip_window_ystart = 0
{0xC950, 0x00b0,2}, 		//cam_stat_awb_clip_window_xend = 319
{0xC952, 0x0090,2}, 		//cam_stat_awb_clip_window_yend = 239
};

#elif defined(FEATURE_TW_CAMERA_FPS28_MCLK24_PCLK92_U)
struct as0260_reg as0260_1080[] =
{
{0x098E, 0xCA12,2},      // LOGICAL_ADDRESS_ACCESS [CAM_SYSCTL_PLL_ENABLE]
{0xCA12, 0x01,1},      //cam_sysctl_pll_enable = 1
{0xCA13, 0x00,1},      //cam_sysctl_pll_enable = 1

{0xCA14, 0x022E,2}, 		//cam_sysctl_pll_divider_m_n = 558
{0xCA16, 0x0070,2}, 		//cam_sysctl_pll_divider_p = 112
{0xCA18, 0x7F7C,2}, 		//cam_sysctl_pll_divider_p4_p5_p6 = 32636
{0xCA1C, 0x8043,2}, 		//cam_port_output_control = 32835
{0xCA1E, 0x0008,2}, 		//cam_port_porch = 8
{0xCA20, 0x0C00,2}, 		//cam_port_mipi_timing_t_hs_zero = 3072
{0xCA22, 0x0006,2}, 		//cam_port_mipi_timing_t_hs_exit_hs_trail = 6
{0xCA24, 0x0B01,2}, 		//cam_port_mipi_timing_t_clk_post_clk_pre = 2817
{0xCA26, 0x0517,2}, 		//cam_port_mipi_timing_t_clk_trail_clk_zero = 1303
{0xCA28, 0x0005,2}, 		//cam_port_mipi_timing_t_lpx = 5
{0xCA2A, 0x0809,2}, 		//cam_port_mipi_timing_init_timing = 2057
{0xCA2C, 0x0000,2}, 		//cam_port_mipi_timing_t_hs_pre = 0


//Timing_settings]
{0xC800, 0x0020,2}, 		//cam_sensor_cfg_y_addr_start = 32
{0xC802, 0x0020,2}, 		//cam_sensor_cfg_x_addr_start = 32
{0xC804, 0x045F,2}, 		//cam_sensor_cfg_y_addr_end = 1119
{0xC806, 0x07A7,2}, 		//cam_sensor_cfg_x_addr_end = 1959
{0xC808, 0x035FE1D8,4}, 		//cam_sensor_cfg_pixclk = 56615384
{0xC80C, 0x0001,2}, 		//cam_sensor_cfg_row_speed = 1
{0xC80E, 0x0336,2}, 		//cam_sensor_cfg_fine_integ_time_min = 822
{0xC810, 0x0A5C,2}, 		//cam_sensor_cfg_fine_integ_time_max = 2652
{0xC812, 0x0519,2}, 		//cam_sensor_cfg_frame_length_lines = 1305
{0xC814, 0x0C1A,2}, 		//cam_sensor_cfg_line_length_pck = 3098
{0xC816, 0x00D4,2}, 		//cam_sensor_cfg_fine_correction = 212
{0xC818, 0x043B,2}, 		//cam_sensor_cfg_cpipe_last_row = 1083
{0xC820, 0x0010,2}, 		//cam_sensor_cfg_reg_0_data = 16
{0xC830, 0x0002,2}, 		//cam_sensor_control_read_mode = 2
{0xC858, 0x0000,2}, 		//cam_crop_window_xoffset = 0
{0xC85A, 0x0000,2}, 		//cam_crop_window_yoffset = 0
{0xC85C, 0x0780,2}, 		//cam_crop_window_width = 1920
{0xC85E, 0x0438,2}, 		//cam_crop_window_height = 1080
{0xC86C, 0x0780,2}, 		//cam_output_width = 1920
{0xC86E, 0x0438,2}, 		//cam_output_height = 1080
{0xC870, 0x4010,2}, 		//cam_output_format = 16592
{0xC87C, 0x00,1}, 		//cam_aet_aemode = 0
{0xC88E, 0x1C00,2}, 		//cam_aet_max_frame_rate = 7168
{0xC890, 0x1C00,2}, 		//cam_aet_min_frame_rate = 7168
{0xC94C, 0x0000,2}, 		//cam_stat_awb_clip_window_xstart = 0
{0xC94E, 0x0000,2}, 		//cam_stat_awb_clip_window_ystart = 0
{0xC950, 0x077F,2}, 		//cam_stat_awb_clip_window_xend = 1919
{0xC952, 0x0437,2}, 		//cam_stat_awb_clip_window_yend = 1079
{0xC954, 0x0000,2}, 		//cam_stat_ae_initial_window_xstart = 0
{0xC956, 0x0000,2}, 		//cam_stat_ae_initial_window_ystart = 0
{0xC958, 0x017F,2}, 		//cam_stat_ae_initial_window_xend = 383
{0xC95A, 0x00D7,2}, 		//cam_stat_ae_initial_window_yend = 215
};

struct as0260_reg as0260_1440[] =
{
{0x098E, 0xCA12,2},      // LOGICAL_ADDRESS_ACCESS [CAM_SYSCTL_PLL_ENABLE]
{0xCA12, 0x01,1},      //cam_sysctl_pll_enable = 1
{0xCA13, 0x00,1},      //cam_sysctl_pll_enable = 1

{0xCA14, 0x022E,2}, 		//cam_sysctl_pll_divider_m_n = 558
{0xCA16, 0x0070,2}, 		//cam_sysctl_pll_divider_p = 112
{0xCA18, 0x7F7C,2}, 		//cam_sysctl_pll_divider_p4_p5_p6 = 32636
{0xCA1C, 0x8043,2}, 		//cam_port_output_control = 32835
{0xCA1E, 0x0008,2}, 		//cam_port_porch = 8
{0xCA20, 0x0C00,2}, 		//cam_port_mipi_timing_t_hs_zero = 3072
{0xCA22, 0x0006,2}, 		//cam_port_mipi_timing_t_hs_exit_hs_trail = 6
{0xCA24, 0x0B01,2}, 		//cam_port_mipi_timing_t_clk_post_clk_pre = 2817
{0xCA26, 0x0517,2}, 		//cam_port_mipi_timing_t_clk_trail_clk_zero = 1303
{0xCA28, 0x0005,2}, 		//cam_port_mipi_timing_t_lpx = 5
{0xCA2A, 0x0809,2}, 		//cam_port_mipi_timing_init_timing = 2057
{0xCA2C, 0x0000,2}, 		//cam_port_mipi_timing_t_hs_pre = 0


//Timing_settings]
{0xC800, 0x0020,2}, 		//cam_sensor_cfg_y_addr_start = 32  
{0xC802, 0x0120,2}, 		//cam_sensor_cfg_x_addr_start = 288 
{0xC804, 0x045F,2}, 		//cam_sensor_cfg_y_addr_end = 1119  
{0xC806, 0x06C7,2}, 		//cam_sensor_cfg_x_addr_end = 1735  
{0xC808, 0x035FE1D8,4}, 		//cam_sensor_cfg_pixclk = 56615384
{0xC80C, 0x0001,2}, 		//cam_sensor_cfg_row_speed = 1
{0xC80E, 0x0336,2}, 		//cam_sensor_cfg_fine_integ_time_min = 822
{0xC810, 0x0A5C,2}, 		//cam_sensor_cfg_fine_integ_time_max = 2652
{0xC812, 0x0519,2}, 		//cam_sensor_cfg_frame_length_lines = 1305
{0xC814, 0x0C1A,2}, 		//cam_sensor_cfg_line_length_pck = 3098
{0xC816, 0x00D4,2}, 		//cam_sensor_cfg_fine_correction = 212
{0xC818, 0x043B,2}, 		//cam_sensor_cfg_cpipe_last_row = 1083
{0xC820, 0x0010,2}, 		//cam_sensor_cfg_reg_0_data = 16
{0xC830, 0x0002,2}, 		//cam_sensor_control_read_mode = 2
{0xC858, 0x0000,2}, 		//cam_crop_window_xoffset = 0
{0xC85A, 0x0000,2}, 		//cam_crop_window_yoffset = 0
{0xC85C, 0x05A0,2}, 		//cam_crop_window_width = 1440   
{0xC85E, 0x0438,2}, 		//cam_crop_window_height = 1080  
{0xC86C, 0x05A0,2}, 		//cam_output_width = 1440        
{0xC86E, 0x0438,2}, 		//cam_output_height = 1080       
{0xC870, 0x4010,2},	//cam_output_format = 16592
{0xC87C, 0x00,1}, 		//cam_aet_aemode = 0
{0xC88E, 0x1C00,2}, 		//cam_aet_max_frame_rate = 7168
{0xC890, 0x1C00,2}, 		//cam_aet_min_frame_rate = 7168
{0xC94C, 0x0000,2}, 		//cam_stat_awb_clip_window_xstart = 0
{0xC94E, 0x0000,2}, 		//cam_stat_awb_clip_window_ystart = 0
{0xC950, 0x05a0,2}, 		//cam_stat_awb_clip_window_xend = 1919
{0xC952, 0x0438,2}, 		//cam_stat_awb_clip_window_yend = 1079
{0xC954, 0x0000,2}, 		// CAM_STAT_AE_INITIAL_WINDOW_XSTART
{0xC956, 0x0000,2}, 		// CAM_STAT_AE_INITIAL_WINDOW_YSTART
{0xC958, 0x0120,2}, 		// CAM_STAT_AE_INITIAL_WINDOW_XEND  
{0xC95A, 0x00D8,2}, 		// CAM_STAT_AE_INITIAL_WINDOW_YEND  
};


struct as0260_reg as0260_960[] =
{
	{0x098E, 0xCA12,2},      // LOGICAL_ADDRESS_ACCESS [CAM_SYSCTL_PLL_ENABLE]
	{0xCA12, 0x01,1},      //cam_sysctl_pll_enable = 1
	{0xCA13, 0x00,0},      //cam_sysctl_pll_enable = 1
	
{0xCA14, 0x022E,2}, 		//cam_sysctl_pll_divider_m_n = 558
{0xCA16, 0x0070,2}, 		//cam_sysctl_pll_divider_p = 112
{0xCA18, 0x7F7C,2}, 		//cam_sysctl_pll_divider_p4_p5_p6 = 32636
{0xCA1C, 0x8043,2}, 		//cam_port_output_control = 32835
{0xCA1E, 0x0008,2}, 		//cam_port_porch = 8
{0xCA20, 0x0C00,2}, 		//cam_port_mipi_timing_t_hs_zero = 3072
{0xCA22, 0x0006,2}, 		//cam_port_mipi_timing_t_hs_exit_hs_trail = 6
{0xCA24, 0x0B01,2}, 		//cam_port_mipi_timing_t_clk_post_clk_pre = 2817
{0xCA26, 0x0517,2}, 		//cam_port_mipi_timing_t_clk_trail_clk_zero = 1303
{0xCA28, 0x0005,2}, 		//cam_port_mipi_timing_t_lpx = 5
{0xCA2A, 0x0809,2}, 		//cam_port_mipi_timing_init_timing = 2057
{0xCA2C, 0x0000,2}, 		//cam_port_mipi_timing_t_hs_pre = 0


//Timing_settings]
{0xC800, 0x0020,2}, 		//cam_sensor_cfg_y_addr_start = 32
{0xC802, 0x0120,2}, 		//cam_sensor_cfg_x_addr_start = 288
{0xC804, 0x045F,2}, 		//cam_sensor_cfg_y_addr_end = 1119
{0xC806, 0x06C7,2}, 		//cam_sensor_cfg_x_addr_end = 1735
{0xC808, 0x035FE1D8,4}, 		//cam_sensor_cfg_pixclk = 56615384
{0xC80C, 0x0001,2}, 		//cam_sensor_cfg_row_speed = 1
{0xC80E, 0x0336,2}, 		//cam_sensor_cfg_fine_integ_time_min = 822
{0xC810, 0x0BC5,2}, 		//cam_sensor_cfg_fine_integ_time_max = 3013
{0xC812, 0x0491,2}, 		//cam_sensor_cfg_frame_length_lines = 1169
{0xC814, 0x0D83,2}, 		//cam_sensor_cfg_line_length_pck = 3459
{0xC816, 0x00D4,2}, 		//cam_sensor_cfg_fine_correction = 212
{0xC818, 0x043B,2}, 		//cam_sensor_cfg_cpipe_last_row = 1083
{0xC820, 0x0010,2}, 		//cam_sensor_cfg_reg_0_data = 16
{0xC830, 0x0002,2}, 		//cam_sensor_control_read_mode = 2
{0xC858, 0x0000,2}, 		//cam_crop_window_xoffset = 0
{0xC85A, 0x0000,2}, 		//cam_crop_window_yoffset = 0
{0xC85C, 0x05A0,2}, 		//cam_crop_window_width = 1440
{0xC85E, 0x0438,2}, 		//cam_crop_window_height = 1080
{0xC86C, 0x0500,2}, 		//cam_output_width = 1280
{0xC86E, 0x03C0,2}, 		//cam_output_height = 960
{0xC870, 0x4010,2}, 		//cam_output_format = 16592
{0xC87C, 0x00,1}, 		//cam_aet_aemode = 0
{0xC88E, 0x1C00,2}, 		//cam_aet_max_frame_rate = 7168
{0xC890, 0x1C00,2}, 		//cam_aet_min_frame_rate = 7168
{0xC94C, 0x0000,2}, 		//cam_stat_awb_clip_window_xstart = 0
{0xC94E, 0x0000,2}, 		//cam_stat_awb_clip_window_ystart = 0
{0xC950, 0x04FF,2}, 		//cam_stat_awb_clip_window_xend = 1279
{0xC952, 0x03BF,2}, 		//cam_stat_awb_clip_window_yend = 959
{0xC954, 0x0000,2}, 		//cam_stat_ae_initial_window_xstart = 0
{0xC956, 0x0000,2}, 		//cam_stat_ae_initial_window_ystart = 0
{0xC958, 0x00FF,2}, 		//cam_stat_ae_initial_window_xend = 255
{0xC95A, 0x00BF,2}, 		//cam_stat_ae_initial_window_yend = 191

};

struct as0260_reg as0260_720[] =
{
{0x098E, 0xCA12,2},      // LOGICAL_ADDRESS_ACCESS [CAM_SYSCTL_PLL_ENABLE]
{0xCA12, 0x01,1},      //cam_sysctl_pll_enable = 1
{0xCA13, 0x00,0},      //cam_sysctl_pll_enable = 1
	
{0xCA14, 0x022E,2}, 		//cam_sysctl_pll_divider_m_n = 558
{0xCA16, 0x0070,2}, 		//cam_sysctl_pll_divider_p = 112
{0xCA18, 0x7F7C,2}, 		//cam_sysctl_pll_divider_p4_p5_p6 = 32636
{0xCA1C, 0x8043,2}, 		//cam_port_output_control = 32835
{0xCA1E, 0x0008,2}, 		//cam_port_porch = 8
{0xCA20, 0x0C00,2}, 		//cam_port_mipi_timing_t_hs_zero = 3072
{0xCA22, 0x0006,2}, 		//cam_port_mipi_timing_t_hs_exit_hs_trail = 6
{0xCA24, 0x0B01,2}, 		//cam_port_mipi_timing_t_clk_post_clk_pre = 2817
{0xCA26, 0x0517,2}, 		//cam_port_mipi_timing_t_clk_trail_clk_zero = 1303
{0xCA28, 0x0005,2}, 		//cam_port_mipi_timing_t_lpx = 5
{0xCA2A, 0x0809,2}, 		//cam_port_mipi_timing_init_timing = 2057
{0xCA2C, 0x0000,2}, 		//cam_port_mipi_timing_t_hs_pre = 0


//Timing_settings]
{0xC800, 0x00E0,2}, 		//cam_sensor_cfg_y_addr_start = 224
{0xC802, 0x0160,2}, 		//cam_sensor_cfg_x_addr_start = 352
{0xC804, 0x03B7,2}, 		//cam_sensor_cfg_y_addr_end = 951
{0xC806, 0x0667,2}, 		//cam_sensor_cfg_x_addr_end = 1639
{0xC808, 0x035FE1D8,4}, 		//cam_sensor_cfg_pixclk = 56615384
{0xC80C, 0x0001,2}, 		//cam_sensor_cfg_row_speed = 1
{0xC80E, 0x0336,2}, 		//cam_sensor_cfg_fine_integ_time_min = 822
{0xC810, 0x11C8,2}, 		//cam_sensor_cfg_fine_integ_time_max = 4552
{0xC812, 0x0329,2}, 		//cam_sensor_cfg_frame_length_lines = 809
{0xC814, 0x1386,2}, 		//cam_sensor_cfg_line_length_pck = 4998
{0xC816, 0x00D4,2}, 		//cam_sensor_cfg_fine_correction = 212
{0xC818, 0x02D3,2}, 		//cam_sensor_cfg_cpipe_last_row = 723
{0xC820, 0x0010,2}, 		//cam_sensor_cfg_reg_0_data = 16
{0xC830, 0x0002,2}, 		//cam_sensor_control_read_mode = 2
{0xC858, 0x0000,2}, 		//cam_crop_window_xoffset = 0
{0xC85A, 0x0000,2}, 		//cam_crop_window_yoffset = 0
{0xC85C, 0x0500,2}, 		//cam_crop_window_width = 1280
{0xC85E, 0x02D0,2}, 		//cam_crop_window_height = 720
{0xC86C, 0x0500,2}, 		//cam_output_width = 1280
{0xC86E, 0x02D0,2}, 		//cam_output_height = 720
{0xC870, 0x4010,2}, 		//cam_output_format = 16592
{0xC87C, 0x00,1}, 		//cam_aet_aemode = 0
{0xC88E, 0x1C00,2}, 		//cam_aet_max_frame_rate = 7168
{0xC890, 0x1C00,2}, 		//cam_aet_min_frame_rate = 7168
{0xC94C, 0x0000,2}, 		//cam_stat_awb_clip_window_xstart = 0
{0xC94E, 0x0000,2}, 		//cam_stat_awb_clip_window_ystart = 0
{0xC950, 0x04FF,2}, 		//cam_stat_awb_clip_window_xend = 1279
{0xC952, 0x02CF,2}, 		//cam_stat_awb_clip_window_yend = 719
{0xC954, 0x0000,2}, 		//cam_stat_ae_initial_window_xstart = 0
{0xC956, 0x0000,2}, 		//cam_stat_ae_initial_window_ystart = 0
{0xC958, 0x00FF,2}, 		//cam_stat_ae_initial_window_xend = 255
{0xC95A, 0x008F,2}, 		//cam_stat_ae_initial_window_yend = 143
};

struct as0260_reg as0260_640[] =
{
{0x098E, 0xCA12,2},      // LOGICAL_ADDRESS_ACCESS [CAM_SYSCTL_PLL_ENABLE]
{0xCA12, 0x01,1},      //cam_sysctl_pll_enable = 1
{0xCA13, 0x00,0},      //cam_sysctl_pll_enable = 1
	
{0xCA14, 0x022E,2}, 		//cam_sysctl_pll_divider_m_n = 558
{0xCA16, 0x0070,2}, 		//cam_sysctl_pll_divider_p = 112
{0xCA18, 0x7F7C,2}, 		//cam_sysctl_pll_divider_p4_p5_p6 = 32636
{0xCA1C, 0x8043,2}, 		//cam_port_output_control = 32835
{0xCA1E, 0x0008,2}, 		//cam_port_porch = 8
{0xCA20, 0x0C00,2}, 		//cam_port_mipi_timing_t_hs_zero = 3072
{0xCA22, 0x0006,2}, 		//cam_port_mipi_timing_t_hs_exit_hs_trail = 6
{0xCA24, 0x0B01,2}, 		//cam_port_mipi_timing_t_clk_post_clk_pre = 2817
{0xCA26, 0x0517,2}, 		//cam_port_mipi_timing_t_clk_trail_clk_zero = 1303
{0xCA28, 0x0005,2}, 		//cam_port_mipi_timing_t_lpx = 5
{0xCA2A, 0x0809,2}, 		//cam_port_mipi_timing_init_timing = 2057
{0xCA2C, 0x0000,2}, 		//cam_port_mipi_timing_t_hs_pre = 0


//Timing_settings]
{0xC800, 0x0020,2}, 		//cam_sensor_cfg_y_addr_start = 32
{0xC802, 0x0100,2}, 		//cam_sensor_cfg_x_addr_start = 256
{0xC804, 0x045D,2}, 		//cam_sensor_cfg_y_addr_end = 1117
{0xC806, 0x06AD,2}, 		//cam_sensor_cfg_x_addr_end = 1709
{0xC808, 0x035FE1D8,4}, 		//cam_sensor_cfg_pixclk = 56615384
{0xC80C, 0x0001,2}, 		//cam_sensor_cfg_row_speed = 1
{0xC80E, 0x06A4,2}, 		//cam_sensor_cfg_fine_integ_time_min = 1700
{0xC810, 0x164E,2}, 		//cam_sensor_cfg_fine_integ_time_max = 5710
{0xC812, 0x026D,2}, 		//cam_sensor_cfg_frame_length_lines = 621
{0xC814, 0x1970,2}, 		//cam_sensor_cfg_line_length_pck = 6512
{0xC816, 0x01D9,2}, 		//cam_sensor_cfg_fine_correction = 473
{0xC818, 0x021B,2}, 		//cam_sensor_cfg_cpipe_last_row = 539
{0xC820, 0x0010,2}, 		//cam_sensor_cfg_reg_0_data = 16
{0xC830, 0x0012,2}, 		//cam_sensor_control_read_mode = 18
{0xC858, 0x0000,2}, 		//cam_crop_window_xoffset = 0
{0xC85A, 0x0000,2}, 		//cam_crop_window_yoffset = 0
{0xC85C, 0x02D0,2}, 		//cam_crop_window_width = 720
{0xC85E, 0x0218,2}, 		//cam_crop_window_height = 536
{0xC86C, 0x0280,2}, 		//cam_output_width = 640
{0xC86E, 0x01E0,2}, 		//cam_output_height = 480
{0xC870, 0x4010,2}, 		//cam_output_format = 16592
{0xC87C, 0x00,1}, 		//cam_aet_aemode = 0
{0xC88E, 0x1C00,2}, 		//cam_aet_max_frame_rate = 7168
{0xC890, 0x1C00,2}, 		//cam_aet_min_frame_rate = 7168
{0xC94C, 0x0000,2}, 		//cam_stat_awb_clip_window_xstart = 0
{0xC94E, 0x0000,2}, 		//cam_stat_awb_clip_window_ystart = 0
{0xC950, 0x027F,2}, 		//cam_stat_awb_clip_window_xend = 639
{0xC952, 0x01DF,2}, 		//cam_stat_awb_clip_window_yend = 479
{0xC954, 0x0000,2}, 		//cam_stat_ae_initial_window_xstart = 0
{0xC956, 0x0000,2}, 		//cam_stat_ae_initial_window_ystart = 0
{0xC958, 0x007F,2}, 		//cam_stat_ae_initial_window_xend = 127
{0xC95A, 0x005F,2}, 		//cam_stat_ae_initial_window_yend = 95
};

struct as0260_reg as0260_320[] =
{
{0x098E, 0xCA12,2},      // LOGICAL_ADDRESS_ACCESS [CAM_SYSCTL_PLL_ENABLE]
{0xCA12, 0x01,1},      //cam_sysctl_pll_enable = 1
{0xCA13, 0x00,0},      //cam_sysctl_pll_enable = 1
	
{0xCA14, 0x022E,2}, 		//cam_sysctl_pll_divider_m_n = 558
{0xCA16, 0x0070,2}, 		//cam_sysctl_pll_divider_p = 112
{0xCA18, 0x7F7C,2}, 		//cam_sysctl_pll_divider_p4_p5_p6 = 32636
{0xCA1C, 0x8043,2}, 		//cam_port_output_control = 32835
{0xCA1E, 0x0008,2}, 		//cam_port_porch = 8
{0xCA20, 0x0C00,2}, 		//cam_port_mipi_timing_t_hs_zero = 3072
{0xCA22, 0x0006,2}, 		//cam_port_mipi_timing_t_hs_exit_hs_trail = 6
{0xCA24, 0x0B01,2}, 		//cam_port_mipi_timing_t_clk_post_clk_pre = 2817
{0xCA26, 0x0517,2}, 		//cam_port_mipi_timing_t_clk_trail_clk_zero = 1303
{0xCA28, 0x0005,2}, 		//cam_port_mipi_timing_t_lpx = 5
{0xCA2A, 0x0809,2}, 		//cam_port_mipi_timing_init_timing = 2057
{0xCA2C, 0x0000,2}, 		//cam_port_mipi_timing_t_hs_pre = 0


//Timing_settings]
{0xC800, 0x0020,2}, 		//cam_sensor_cfg_y_addr_start = 32
{0xC802, 0x0100,2}, 		//cam_sensor_cfg_x_addr_start = 256
{0xC804, 0x045D,2}, 		//cam_sensor_cfg_y_addr_end = 1117
{0xC806, 0x06AD,2}, 		//cam_sensor_cfg_x_addr_end = 1709
{0xC808, 0x035FE1D8,4}, 		//cam_sensor_cfg_pixclk = 56615384
{0xC80C, 0x0001,2}, 		//cam_sensor_cfg_row_speed = 1
{0xC80E, 0x06A4,2}, 		//cam_sensor_cfg_fine_integ_time_min = 1700
{0xC810, 0x164E,2}, 		//cam_sensor_cfg_fine_integ_time_max = 5710
{0xC812, 0x026D,2}, 		//cam_sensor_cfg_frame_length_lines = 621
{0xC814, 0x1970,2}, 		//cam_sensor_cfg_line_length_pck = 6512
{0xC816, 0x01D9,2}, 		//cam_sensor_cfg_fine_correction = 473
{0xC818, 0x021B,2}, 		//cam_sensor_cfg_cpipe_last_row = 539
{0xC820, 0x0010,2}, 		//cam_sensor_cfg_reg_0_data = 16
{0xC830, 0x0012,2}, 		//cam_sensor_control_read_mode = 18
{0xC858, 0x0000,2}, 		//cam_crop_window_xoffset = 0
{0xC85A, 0x0000,2}, 		//cam_crop_window_yoffset = 0
{0xC85C, 0x02D0,2}, 		//cam_crop_window_width = 720
{0xC85E, 0x0218,2}, 		//cam_crop_window_height = 536
{0xC86C, 0x0140,2}, 		//cam_output_width = 320
{0xC86E, 0x00F0,2}, 		//cam_output_height = 240
{0xC870, 0x4010,2}, 		//cam_output_format = 16592
{0xC87C, 0x00,1}, 		//cam_aet_aemode = 0
{0xC88E, 0x1C00,2}, 		//cam_aet_max_frame_rate = 7168
{0xC890, 0x1C00,2}, 		//cam_aet_min_frame_rate = 7168
{0xC94C, 0x0000,2}, 		//cam_stat_awb_clip_window_xstart = 0
{0xC94E, 0x0000,2}, 		//cam_stat_awb_clip_window_ystart = 0
{0xC950, 0x013F,2}, 		//cam_stat_awb_clip_window_xend = 319
{0xC952, 0x00EF,2}, 		//cam_stat_awb_clip_window_yend = 239
{0xC954, 0x0000,2}, 		//cam_stat_ae_initial_window_xstart = 0
{0xC956, 0x0000,2}, 		//cam_stat_ae_initial_window_ystart = 0
{0xC958, 0x003F,2}, 		//cam_stat_ae_initial_window_xend = 63
{0xC95A, 0x002F,2}, 		//cam_stat_ae_initial_window_yend = 47

#if 0
    {0xBC02, 0x0013,2},      // LL_MODE
    {0xC960, 0x0003,2},      // CAM_LL_LLMODE
    {0xBC08, 0x00,1},      // LL_GAMMA_SELECT
    {0xC9C6, 0xC0,1},      // CAM_LL_START_CONTRAST_GRADIENT
    {0xC9C7, 0x60,1},      // CAM_LL_STOP_CONTRAST_GRADIENT
    {0xC9C8, 0x0F,1},      // CAM_LL_START_CONTRAST_LUMA_PERCENTAGE
    {0xC9C9, 0x2C,1},      // CAM_LL_STOP_CONTRAST_LUMA_PERCENTAGE
    {0xE400, 0x00,1},      // PATCHVARS_START_ORIGIN_GRADIENT
    {0xE401, 0x00,1},      // PATCHVARS_STOP_ORIGIN_GRADIENT
    {0xC9C0, 0x0012,2},      // CAM_LL_START_CONTRAST_BM
    {0xC9C2, 0x00B3,2},      // CAM_LL_STOP_CONTRAST_BM
    {0xffff, 100,2},
#endif    
};
// config resetting 
struct as0260_reg as0260_176[] =
{
	{0x098E, 0xCA12,2},      // LOGICAL_ADDRESS_ACCESS [CAM_SYSCTL_PLL_ENABLE]
	{0xCA12, 0x01,1},      //cam_sysctl_pll_enable = 1
	{0xCA13, 0x00,0},      //cam_sysctl_pll_enable = 1
	
{0xCA14, 0x022E,2}, 		//cam_sysctl_pll_divider_m_n = 558
{0xCA16, 0x0070,2}, 		//cam_sysctl_pll_divider_p = 112
{0xCA18, 0x7F7C,2}, 		//cam_sysctl_pll_divider_p4_p5_p6 = 32636
{0xCA1C, 0x8043,2}, 		//cam_port_output_control = 32835
{0xCA1E, 0x0008,2}, 		//cam_port_porch = 8
{0xCA20, 0x0C00,2}, 		//cam_port_mipi_timing_t_hs_zero = 3072
{0xCA22, 0x0006,2}, 		//cam_port_mipi_timing_t_hs_exit_hs_trail = 6
{0xCA24, 0x0B01,2}, 		//cam_port_mipi_timing_t_clk_post_clk_pre = 2817
{0xCA26, 0x0517,2}, 		//cam_port_mipi_timing_t_clk_trail_clk_zero = 1303
{0xCA28, 0x0005,2}, 		//cam_port_mipi_timing_t_lpx = 5
{0xCA2A, 0x0809,2}, 		//cam_port_mipi_timing_init_timing = 2057
{0xCA2C, 0x0000,2}, 		//cam_port_mipi_timing_t_hs_pre = 0

{0xC800, 0x0020,2}, 		//cam_sensor_cfg_y_addr_start = 32
{0xC802, 0x0100,2}, 		//cam_sensor_cfg_x_addr_start = 256
{0xC804, 0x045D,2}, 		//cam_sensor_cfg_y_addr_end = 1117
{0xC806, 0x06AD,2}, 		//cam_sensor_cfg_x_addr_end = 1709
{0xC808, 0x035FE1D8,4}, 		//cam_sensor_cfg_pixclk = 56615384
{0xC80C, 0x0001,2}, 		//cam_sensor_cfg_row_speed = 1
{0xC80E, 0x06A4,2}, 		//cam_sensor_cfg_fine_integ_time_min = 1700
{0xC810, 0x1842,2}, 		//cam_sensor_cfg_fine_integ_time_max = 6210
{0xC812, 0x026D,2}, 		//cam_sensor_cfg_frame_length_lines = 621
{0xC814, 0x1B64,2}, 		//cam_sensor_cfg_line_length_pck = 7012
{0xC816, 0x01D9,2}, 		//cam_sensor_cfg_fine_correction = 473
{0xC818, 0x021B,2}, 		//cam_sensor_cfg_cpipe_last_row = 539
{0xC820, 0x0010,2}, 		//cam_sensor_cfg_reg_0_data = 16
{0xC830, 0x0012,2}, 		//cam_sensor_control_read_mode = 18
{0xC858, 0x0000,2}, 		//cam_crop_window_xoffset = 0
{0xC85A, 0x0000,2}, 		//cam_crop_window_yoffset = 0
{0xC85C, 0x02D0,2}, 		//cam_crop_window_width = 720
{0xC85E, 0x0218,2}, 		//cam_crop_window_height = 536
{0xC86C, 0x00b0,2}, 		//cam_output_width = 320
{0xC86E, 0x0090,2}, 		//cam_output_height = 240
{0xC870, 0x4010,2},	//cam_output_format = 16592
{0xC87C, 0x00,1}, 		//cam_aet_aemode = 0
{0xC88E, 0x1A00,2}, 		//cam_aet_max_frame_rate = 6656
{0xC890, 0x1A00,2}, 		//cam_aet_min_frame_rate = 6656
{0xC94C, 0x0000,2}, 		//cam_stat_awb_clip_window_xstart = 0
{0xC94E, 0x0000,2}, 		//cam_stat_awb_clip_window_ystart = 0
{0xC950, 0x00b0,2}, 		//cam_stat_awb_clip_window_xend = 319
{0xC952, 0x0090,2}, 		//cam_stat_awb_clip_window_yend = 239
};

#elif  defined(FEATURE_TW_CAMERA_FPS27_MCLK24_PCLK82_H)

struct as0260_reg as0260_1440[] =
{
{0x098E, 0xCA12,2},      // LOGICAL_ADDRESS_ACCESS [CAM_SYSCTL_PLL_ENABLE]
{0xCA12, 0x01,1},      //cam_sysctl_pll_enable = 1
{0xCA13, 0x00,1},      //cam_sysctl_pll_enable = 1
	
{0xCA14, 0x0229,2}, 		//cam_sysctl_pll_divider_m_n = 553
{0xCA16, 0x0070,2}, 		//cam_sysctl_pll_divider_p = 112
{0xCA18, 0x7F7C,2}, 		//cam_sysctl_pll_divider_p4_p5_p6 = 32636
{0xCA1C, 0x8043,2}, 		//cam_port_output_control = 32835
{0xCA1E, 0x0004,2}, 		//cam_port_porch = 8
{0xCA20, 0x0B00,2}, 		//cam_port_mipi_timing_t_hs_zero = 2816
{0xCA22, 0x0005,2}, 		//cam_port_mipi_timing_t_hs_exit_hs_trail = 5
{0xCA24, 0x0A01,2}, 		//cam_port_mipi_timing_t_clk_post_clk_pre = 2561
{0xCA26, 0x0514,2}, 		//cam_port_mipi_timing_t_clk_trail_clk_zero = 1300
{0xCA28, 0x0004,2}, 		//cam_port_mipi_timing_t_lpx = 4
{0xCA2A, 0x0709,2}, 		//cam_port_mipi_timing_init_timing = 1801
{0xCA2C, 0x0000,2}, 		//cam_port_mipi_timing_t_hs_pre = 0


//[Timing_settings]
{0xC800, 0x0020,2}, 		//cam_sensor_cfg_y_addr_start = 32
{0xC802, 0x0020,2}, 		//cam_sensor_cfg_x_addr_start = 32
{0xC804, 0x045F,2}, 		//cam_sensor_cfg_y_addr_end = 1119
{0xC806, 0x07A7,2}, 		//cam_sensor_cfg_x_addr_end = 1959
{0xC808, 0x0301FB62,4}, 		//cam_sensor_cfg_pixclk = 50461538
{0xC80C, 0x0001,2}, 		//cam_sensor_cfg_row_speed = 1
{0xC80E, 0x0336,2}, 		//cam_sensor_cfg_fine_integ_time_min = 822
{0xC810, 0x0A5C,2}, 		//cam_sensor_cfg_fine_integ_time_max = 2652
{0xC812, 0x04B6,2}, 		//cam_sensor_cfg_frame_length_lines = 1206
{0xC814, 0x0C1A,2}, 		//cam_sensor_cfg_line_length_pck = 3098
{0xC816, 0x00D4,2}, 		//cam_sensor_cfg_fine_correction = 212
{0xC818, 0x043B,2}, 		//cam_sensor_cfg_cpipe_last_row = 1083
{0xC820, 0x0010,2}, 		//cam_sensor_cfg_reg_0_data = 16
{0xC830, 0x0002,2}, 		//cam_sensor_control_read_mode = 2

{0xC858, 0x00f0,2}, 		//cam_crop_window_xoffset = 0 modify

{0xC85A, 0x0000,2}, 		//cam_crop_window_yoffset = 0

{0xC85C, 0x05A0,2}, 		//cam_crop_window_width = 1440 modify

{0xC85E, 0x0438,2}, 		//cam_crop_window_height = 1080

{0xC86C, 0x05A0,2}, 		//cam_output_width = 1440 modify

{0xC86E, 0x0438,2}, 		//cam_output_height = 1080
{0xC870, 0x4010,2}, 		//cam_output_format = 16592
{0xC87C, 0x00,1}, 		//cam_aet_aemode = 0
{0xC88E, 0x1B00,2}, 		//cam_aet_max_frame_rate = 6912
{0xC890, 0x1B00,2}, 		//cam_aet_min_frame_rate = 6912
{0xC94C, 0x0000,2}, 		//cam_stat_awb_clip_window_xstart = 0
{0xC94E, 0x0000,2}, 		//cam_stat_awb_clip_window_ystart = 0

{0xC950, 0x059f,2}, 		//cam_stat_awb_clip_window_xend = 1919 modify
{0xC952, 0x0438,2}, 		//cam_stat_awb_clip_window_yend = 1079 modify

{0xC954, 0x0000,2}, 		//cam_stat_ae_initial_window_xstart = 0
{0xC956, 0x0000,2}, 		//cam_stat_ae_initial_window_ystart = 0

{0xC958, 0x0120,2}, 		//cam_stat_ae_initial_window_xend = 383 modify
{0xC95A, 0x00D8,2}, 		//cam_stat_ae_initial_window_yend = 215 modify

};

struct as0260_reg as0260_1080[] =
{
{0x098E, 0xCA12,2},      // LOGICAL_ADDRESS_ACCESS [CAM_SYSCTL_PLL_ENABLE]
{0xCA12, 0x01,1},      //cam_sysctl_pll_enable = 1
{0xCA13, 0x00,1},      //cam_sysctl_pll_enable = 1
	
{0xCA14, 0x0229,2}, 		//cam_sysctl_pll_divider_m_n = 553
{0xCA16, 0x0070,2}, 		//cam_sysctl_pll_divider_p = 112
{0xCA18, 0x7F7C,2}, 		//cam_sysctl_pll_divider_p4_p5_p6 = 32636
{0xCA1C, 0x8043,2}, 		//cam_port_output_control = 32835
{0xCA1E, 0x0004,2}, 		//cam_port_porch = 8
{0xCA20, 0x0B00,2}, 		//cam_port_mipi_timing_t_hs_zero = 2816
{0xCA22, 0x0005,2}, 		//cam_port_mipi_timing_t_hs_exit_hs_trail = 5
{0xCA24, 0x0A01,2}, 		//cam_port_mipi_timing_t_clk_post_clk_pre = 2561
{0xCA26, 0x0514,2}, 		//cam_port_mipi_timing_t_clk_trail_clk_zero = 1300
{0xCA28, 0x0004,2}, 		//cam_port_mipi_timing_t_lpx = 4
{0xCA2A, 0x0709,2}, 		//cam_port_mipi_timing_init_timing = 1801
{0xCA2C, 0x0000,2}, 		//cam_port_mipi_timing_t_hs_pre = 0


//[Timing_settings]
{0xC800, 0x0020,2}, 		//cam_sensor_cfg_y_addr_start = 32
{0xC802, 0x0020,2}, 		//cam_sensor_cfg_x_addr_start = 32
{0xC804, 0x045F,2}, 		//cam_sensor_cfg_y_addr_end = 1119
{0xC806, 0x07A7,2}, 		//cam_sensor_cfg_x_addr_end = 1959
{0xC808, 0x0301FB62,4}, 		//cam_sensor_cfg_pixclk = 50461538
{0xC80C, 0x0001,2}, 		//cam_sensor_cfg_row_speed = 1
{0xC80E, 0x0336,2}, 		//cam_sensor_cfg_fine_integ_time_min = 822
{0xC810, 0x0A5C,2}, 		//cam_sensor_cfg_fine_integ_time_max = 2652
{0xC812, 0x04B6,2}, 		//cam_sensor_cfg_frame_length_lines = 1206
{0xC814, 0x0C1A,2}, 		//cam_sensor_cfg_line_length_pck = 3098
{0xC816, 0x00D4,2}, 		//cam_sensor_cfg_fine_correction = 212
{0xC818, 0x043B,2}, 		//cam_sensor_cfg_cpipe_last_row = 1083
{0xC820, 0x0010,2}, 		//cam_sensor_cfg_reg_0_data = 16
{0xC830, 0x0002,2}, 		//cam_sensor_control_read_mode = 2
{0xC858, 0x0000,2}, 		//cam_crop_window_xoffset = 0
{0xC85A, 0x0000,2}, 		//cam_crop_window_yoffset = 0
{0xC85C, 0x0780,2}, 		//cam_crop_window_width = 1920
{0xC85E, 0x0438,2}, 		//cam_crop_window_height = 1080
{0xC86C, 0x0780,2}, 		//cam_output_width = 1920
{0xC86E, 0x0438,2}, 		//cam_output_height = 1080
{0xC870, 0x4010,2}, 		//cam_output_format = 16592
{0xC87C, 0x00,1}, 		//cam_aet_aemode = 0
{0xC88E, 0x1B00,2}, 		//cam_aet_max_frame_rate = 6912
{0xC890, 0x1B00,2}, 		//cam_aet_min_frame_rate = 6912
{0xC94C, 0x0000,2}, 		//cam_stat_awb_clip_window_xstart = 0
{0xC94E, 0x0000,2}, 		//cam_stat_awb_clip_window_ystart = 0
{0xC950, 0x077F,2}, 		//cam_stat_awb_clip_window_xend = 1919
{0xC952, 0x0437,2}, 		//cam_stat_awb_clip_window_yend = 1079
{0xC954, 0x0000,2}, 		//cam_stat_ae_initial_window_xstart = 0
{0xC956, 0x0000,2}, 		//cam_stat_ae_initial_window_ystart = 0
{0xC958, 0x017F,2}, 		//cam_stat_ae_initial_window_xend = 383
{0xC95A, 0x00D7,2}, 		//cam_stat_ae_initial_window_yend = 215

};

struct as0260_reg as0260_960[] =
{
	{0x098E, 0xCA12,2},      // LOGICAL_ADDRESS_ACCESS [CAM_SYSCTL_PLL_ENABLE]
	{0xCA12, 0x01,1},      //cam_sysctl_pll_enable = 1
	{0xCA13, 0x00,0},      //cam_sysctl_pll_enable = 1
	
{0xCA14, 0x0229,2}, 		//cam_sysctl_pll_divider_m_n = 553
{0xCA16, 0x0070,2}, 		//cam_sysctl_pll_divider_p = 112
{0xCA18, 0x7F7C,2}, 		//cam_sysctl_pll_divider_p4_p5_p6 = 32636
{0xCA1C, 0x8043,2}, 		//cam_port_output_control = 32835
{0xCA1E, 0x0004,2}, 		//cam_port_porch = 8
{0xCA20, 0x0B00,2}, 		//cam_port_mipi_timing_t_hs_zero = 2816
{0xCA22, 0x0005,2}, 		//cam_port_mipi_timing_t_hs_exit_hs_trail = 5
{0xCA24, 0x0A01,2}, 		//cam_port_mipi_timing_t_clk_post_clk_pre = 2561
{0xCA26, 0x0514,2}, 		//cam_port_mipi_timing_t_clk_trail_clk_zero = 1300
{0xCA28, 0x0004,2}, 		//cam_port_mipi_timing_t_lpx = 4
{0xCA2A, 0x0709,2}, 		//cam_port_mipi_timing_init_timing = 1801
{0xCA2C, 0x0000,2}, 		//cam_port_mipi_timing_t_hs_pre = 0


//[Timing_settings]
{0xC800, 0x0020,2}, 		//cam_sensor_cfg_y_addr_start = 32
{0xC802, 0x0120,2}, 		//cam_sensor_cfg_x_addr_start = 288
{0xC804, 0x045F,2}, 		//cam_sensor_cfg_y_addr_end = 1119
{0xC806, 0x06C7,2}, 		//cam_sensor_cfg_x_addr_end = 1735
{0xC808, 0x0301FB62,4}, 		//cam_sensor_cfg_pixclk = 50461538
{0xC80C, 0x0001,2}, 		//cam_sensor_cfg_row_speed = 1
{0xC80E, 0x0336,2}, 		//cam_sensor_cfg_fine_integ_time_min = 822
{0xC810, 0x087C,2}, 		//cam_sensor_cfg_fine_integ_time_max = 2172
{0xC812, 0x0593,2}, 		//cam_sensor_cfg_frame_length_lines = 1427
{0xC814, 0x0A3A,2}, 		//cam_sensor_cfg_line_length_pck = 2618
{0xC816, 0x00D4,2}, 		//cam_sensor_cfg_fine_correction = 212
{0xC818, 0x043B,2}, 		//cam_sensor_cfg_cpipe_last_row = 1083
{0xC820, 0x0010,2}, 		//cam_sensor_cfg_reg_0_data = 16
{0xC830, 0x0002,2}, 		//cam_sensor_control_read_mode = 2
{0xC858, 0x0000,2}, 		//cam_crop_window_xoffset = 0
{0xC85A, 0x0000,2}, 		//cam_crop_window_yoffset = 0
{0xC85C, 0x05A0,2}, 		//cam_crop_window_width = 1440
{0xC85E, 0x0438,2}, 		//cam_crop_window_height = 1080
{0xC86C, 0x0500,2}, 		//cam_output_width = 1280
{0xC86E, 0x03C0,2}, 		//cam_output_height = 960
{0xC870, 0x4010,2}, 		//cam_output_format = 16592
{0xC87C, 0x00,1}, 		//cam_aet_aemode = 0
{0xC88E, 0x1B00,2}, 		//cam_aet_max_frame_rate = 6912
{0xC890, 0x1B00,2}, 		//cam_aet_min_frame_rate = 6912
{0xC94C, 0x0000,2}, 		//cam_stat_awb_clip_window_xstart = 0
{0xC94E, 0x0000,2}, 		//cam_stat_awb_clip_window_ystart = 0
{0xC950, 0x04FF,2}, 		//cam_stat_awb_clip_window_xend = 1279
{0xC952, 0x03BF,2}, 		//cam_stat_awb_clip_window_yend = 959
{0xC954, 0x0000,2}, 		//cam_stat_ae_initial_window_xstart = 0
{0xC956, 0x0000,2}, 		//cam_stat_ae_initial_window_ystart = 0
{0xC958, 0x00FF,2}, 		//cam_stat_ae_initial_window_xend = 255
{0xC95A, 0x00BF,2}, 		//cam_stat_ae_initial_window_yend = 191
};

struct as0260_reg as0260_720[] =
{
{0x098E, 0xCA12,2},      // LOGICAL_ADDRESS_ACCESS [CAM_SYSCTL_PLL_ENABLE]
{0xCA12, 0x01,1},      //cam_sysctl_pll_enable = 1
{0xCA13, 0x00,0},      //cam_sysctl_pll_enable = 1
	
{0xCA14, 0x0229,2}, 		//cam_sysctl_pll_divider_m_n = 553
{0xCA16, 0x0070,2}, 		//cam_sysctl_pll_divider_p = 112
{0xCA18, 0x7F7C,2}, 		//cam_sysctl_pll_divider_p4_p5_p6 = 32636
{0xCA1C, 0x8043,2}, 		//cam_port_output_control = 32835
{0xCA1E, 0x0004,2}, 		//cam_port_porch = 8
{0xCA20, 0x0B00,2}, 		//cam_port_mipi_timing_t_hs_zero = 2816
{0xCA22, 0x0005,2}, 		//cam_port_mipi_timing_t_hs_exit_hs_trail = 5
{0xCA24, 0x0A01,2}, 		//cam_port_mipi_timing_t_clk_post_clk_pre = 2561
{0xCA26, 0x0514,2}, 		//cam_port_mipi_timing_t_clk_trail_clk_zero = 1300
{0xCA28, 0x0004,2}, 		//cam_port_mipi_timing_t_lpx = 4
{0xCA2A, 0x0709,2}, 		//cam_port_mipi_timing_init_timing = 1801
{0xCA2C, 0x0000,2}, 		//cam_port_mipi_timing_t_hs_pre = 0


//[Timing_settings]
{0xC800, 0x0020,2}, 		//cam_sensor_cfg_y_addr_start = 32
{0xC802, 0x0020,2}, 		//cam_sensor_cfg_x_addr_start = 32
{0xC804, 0x045F,2}, 		//cam_sensor_cfg_y_addr_end = 1119
{0xC806, 0x07A7,2}, 		//cam_sensor_cfg_x_addr_end = 1959
{0xC808, 0x0301FB62,4}, 		//cam_sensor_cfg_pixclk = 50461538
{0xC80C, 0x0001,2}, 		//cam_sensor_cfg_row_speed = 1
{0xC80E, 0x0336,2}, 		//cam_sensor_cfg_fine_integ_time_min = 822
{0xC810, 0x0A5C,2}, 		//cam_sensor_cfg_fine_integ_time_max = 2652
{0xC812, 0x04B6,2}, 		//cam_sensor_cfg_frame_length_lines = 1206
{0xC814, 0x0C1A,2}, 		//cam_sensor_cfg_line_length_pck = 3098
{0xC816, 0x00D4,2}, 		//cam_sensor_cfg_fine_correction = 212
{0xC818, 0x043B,2}, 		//cam_sensor_cfg_cpipe_last_row = 1083
{0xC820, 0x0010,2}, 		//cam_sensor_cfg_reg_0_data = 16
{0xC830, 0x0002,2}, 		//cam_sensor_control_read_mode = 2
{0xC858, 0x0000,2}, 		//cam_crop_window_xoffset = 0
{0xC85A, 0x0000,2}, 		//cam_crop_window_yoffset = 0
{0xC85C, 0x0780,2}, 		//cam_crop_window_width = 1920
{0xC85E, 0x0438,2}, 		//cam_crop_window_height = 1080
{0xC86C, 0x0500,2}, 		//cam_output_width = 1280
{0xC86E, 0x02D0,2}, 		//cam_output_height = 720
{0xC870, 0x4010,2}, 		//cam_output_format = 16592
{0xC87C, 0x00,1}, 		//cam_aet_aemode = 0
{0xC88E, 0x1B00,2}, 		//cam_aet_max_frame_rate = 6912
{0xC890, 0x1B00,2}, 		//cam_aet_min_frame_rate = 6912
{0xC94C, 0x0000,2}, 		//cam_stat_awb_clip_window_xstart = 0
{0xC94E, 0x0000,2}, 		//cam_stat_awb_clip_window_ystart = 0
{0xC950, 0x04FF,2}, 		//cam_stat_awb_clip_window_xend = 1279
{0xC952, 0x02CF,2}, 		//cam_stat_awb_clip_window_yend = 719
{0xC954, 0x0000,2}, 		//cam_stat_ae_initial_window_xstart = 0
{0xC956, 0x0000,2}, 		//cam_stat_ae_initial_window_ystart = 0
{0xC958, 0x00FF,2}, 		//cam_stat_ae_initial_window_xend = 255
{0xC95A, 0x008F,2}, 		//cam_stat_ae_initial_window_yend = 143

};

struct as0260_reg as0260_640[] =
{
{0x098E, 0xCA12,2},      // LOGICAL_ADDRESS_ACCESS [CAM_SYSCTL_PLL_ENABLE]
{0xCA12, 0x01,1},      //cam_sysctl_pll_enable = 1
{0xCA13, 0x00,0},      //cam_sysctl_pll_enable = 1
	
{0xCA14, 0x0229,2}, 		//cam_sysctl_pll_divider_m_n = 553
{0xCA16, 0x0070,2}, 		//cam_sysctl_pll_divider_p = 112
{0xCA18, 0x7F7C,2}, 		//cam_sysctl_pll_divider_p4_p5_p6 = 32636
{0xCA1C, 0x8043,2}, 		//cam_port_output_control = 32835
{0xCA1E, 0x0004,2}, 		//cam_port_porch = 8
{0xCA20, 0x0B00,2}, 		//cam_port_mipi_timing_t_hs_zero = 2816
{0xCA22, 0x0005,2}, 		//cam_port_mipi_timing_t_hs_exit_hs_trail = 5
{0xCA24, 0x0A01,2}, 		//cam_port_mipi_timing_t_clk_post_clk_pre = 2561
{0xCA26, 0x0514,2}, 		//cam_port_mipi_timing_t_clk_trail_clk_zero = 1300
{0xCA28, 0x0004,2}, 		//cam_port_mipi_timing_t_lpx = 4
{0xCA2A, 0x0709,2}, 		//cam_port_mipi_timing_init_timing = 1801
{0xCA2C, 0x0000,2}, 		//cam_port_mipi_timing_t_hs_pre = 0


//[Timing_settings]
{0xC800, 0x0020,2}, 		//cam_sensor_cfg_y_addr_start = 32
{0xC802, 0x0100,2}, 		//cam_sensor_cfg_x_addr_start = 256
{0xC804, 0x045D,2}, 		//cam_sensor_cfg_y_addr_end = 1117
{0xC806, 0x06AD,2}, 		//cam_sensor_cfg_x_addr_end = 1709
{0xC808, 0x0301FB62,4}, 		//cam_sensor_cfg_pixclk = 50461538
{0xC80C, 0x0001,2}, 		//cam_sensor_cfg_row_speed = 1
{0xC80E, 0x06A4,2}, 		//cam_sensor_cfg_fine_integ_time_min = 1700
{0xC810, 0x077A,2}, 		//cam_sensor_cfg_fine_integ_time_max = 1914
{0xC812, 0x0560,2}, 		//cam_sensor_cfg_frame_length_lines = 1376
{0xC814, 0x0A9C,2}, 		//cam_sensor_cfg_line_length_pck = 2716
{0xC816, 0x01D9,2}, 		//cam_sensor_cfg_fine_correction = 473
{0xC818, 0x021B,2}, 		//cam_sensor_cfg_cpipe_last_row = 539
{0xC820, 0x0010,2}, 		//cam_sensor_cfg_reg_0_data = 16
{0xC830, 0x0012,2}, 		//cam_sensor_control_read_mode = 18
{0xC858, 0x0000,2}, 		//cam_crop_window_xoffset = 0
{0xC85A, 0x0000,2}, 		//cam_crop_window_yoffset = 0
{0xC85C, 0x02D0,2}, 		//cam_crop_window_width = 720
{0xC85E, 0x0218,2}, 		//cam_crop_window_height = 536
{0xC86C, 0x0280,2}, 		//cam_output_width = 640
{0xC86E, 0x01E0,2}, 		//cam_output_height = 480
{0xC870, 0x4010,2}, 		//cam_output_format = 16592
{0xC87C, 0x00,1}, 		//cam_aet_aemode = 0
{0xC88E, 0x1B00,2}, 		//cam_aet_max_frame_rate = 6912
{0xC890, 0x1B00,2}, 		//cam_aet_min_frame_rate = 6912
{0xC94C, 0x0000,2}, 		//cam_stat_awb_clip_window_xstart = 0
{0xC94E, 0x0000,2}, 		//cam_stat_awb_clip_window_ystart = 0
{0xC950, 0x027F,2}, 		//cam_stat_awb_clip_window_xend = 639
{0xC952, 0x01DF,2}, 		//cam_stat_awb_clip_window_yend = 479
{0xC954, 0x0000,2}, 		//cam_stat_ae_initial_window_xstart = 0
{0xC956, 0x0000,2}, 		//cam_stat_ae_initial_window_ystart = 0
{0xC958, 0x007F,2}, 		//cam_stat_ae_initial_window_xend = 127
{0xC95A, 0x005F,2}, 		//cam_stat_ae_initial_window_yend = 95
};

struct as0260_reg as0260_320[] =
{
{0x098E, 0xCA12,2},      // LOGICAL_ADDRESS_ACCESS [CAM_SYSCTL_PLL_ENABLE]
{0xCA12, 0x01,1},      //cam_sysctl_pll_enable = 1
{0xCA13, 0x00,0},      //cam_sysctl_pll_enable = 1
	
{0xCA14, 0x0229,2}, 		//cam_sysctl_pll_divider_m_n = 553
{0xCA16, 0x0070,2}, 		//cam_sysctl_pll_divider_p = 112
{0xCA18, 0x7F7C,2}, 		//cam_sysctl_pll_divider_p4_p5_p6 = 32636
{0xCA1C, 0x8043,2}, 		//cam_port_output_control = 32835
{0xCA1E, 0x0004,2}, 		//cam_port_porch = 8
{0xCA20, 0x0B00,2}, 		//cam_port_mipi_timing_t_hs_zero = 2816
{0xCA22, 0x0005,2}, 		//cam_port_mipi_timing_t_hs_exit_hs_trail = 5
{0xCA24, 0x0A01,2}, 		//cam_port_mipi_timing_t_clk_post_clk_pre = 2561
{0xCA26, 0x0514,2}, 		//cam_port_mipi_timing_t_clk_trail_clk_zero = 1300
{0xCA28, 0x0004,2}, 		//cam_port_mipi_timing_t_lpx = 4
{0xCA2A, 0x0709,2}, 		//cam_port_mipi_timing_init_timing = 1801
{0xCA2C, 0x0000,2}, 		//cam_port_mipi_timing_t_hs_pre = 0


//[Timing_settings]
{0xC800, 0x0020,2}, 		//cam_sensor_cfg_y_addr_start = 32
{0xC802, 0x0100,2}, 		//cam_sensor_cfg_x_addr_start = 256
{0xC804, 0x045D,2}, 		//cam_sensor_cfg_y_addr_end = 1117
{0xC806, 0x06AD,2}, 		//cam_sensor_cfg_x_addr_end = 1709
{0xC808, 0x0301FB62,4}, 		//cam_sensor_cfg_pixclk = 50461538
{0xC80C, 0x0001,2}, 		//cam_sensor_cfg_row_speed = 1
{0xC80E, 0x06A4,2}, 		//cam_sensor_cfg_fine_integ_time_min = 1700
{0xC810, 0x077A,2}, 		//cam_sensor_cfg_fine_integ_time_max = 1914
{0xC812, 0x0560,2}, 		//cam_sensor_cfg_frame_length_lines = 1376
{0xC814, 0x0A9C,2}, 		//cam_sensor_cfg_line_length_pck = 2716
{0xC816, 0x01D9,2}, 		//cam_sensor_cfg_fine_correction = 473
{0xC818, 0x021B,2}, 		//cam_sensor_cfg_cpipe_last_row = 539
{0xC820, 0x0010,2}, 		//cam_sensor_cfg_reg_0_data = 16
{0xC830, 0x0012,2}, 		//cam_sensor_control_read_mode = 18
{0xC858, 0x0000,2}, 		//cam_crop_window_xoffset = 0
{0xC85A, 0x0000,2}, 		//cam_crop_window_yoffset = 0
{0xC85C, 0x02D0,2}, 		//cam_crop_window_width = 720
{0xC85E, 0x0218,2}, 		//cam_crop_window_height = 536
{0xC86C, 0x0140,2}, 		//cam_output_width = 320
{0xC86E, 0x00F0,2}, 		//cam_output_height = 240
{0xC870, 0x4010,2}, 		//cam_output_format = 16592
{0xC87C, 0x00,1}, 		//cam_aet_aemode = 0
{0xC88E, 0x1B00,2}, 		//cam_aet_max_frame_rate = 6912
{0xC890, 0x1B00,2}, 		//cam_aet_min_frame_rate = 6912
{0xC94C, 0x0000,2}, 		//cam_stat_awb_clip_window_xstart = 0
{0xC94E, 0x0000,2}, 		//cam_stat_awb_clip_window_ystart = 0
{0xC950, 0x013F,2}, 		//cam_stat_awb_clip_window_xend = 319
{0xC952, 0x00EF,2}, 		//cam_stat_awb_clip_window_yend = 239
{0xC954, 0x0000,2}, 		//cam_stat_ae_initial_window_xstart = 0
{0xC956, 0x0000,2}, 		//cam_stat_ae_initial_window_ystart = 0
{0xC958, 0x003F,2}, 		//cam_stat_ae_initial_window_xend = 63
{0xC95A, 0x002F,2}, 		//cam_stat_ae_initial_window_yend = 47

};
// config resetting 
struct as0260_reg as0260_176[] =
{
	{0x098E, 0xCA12,2},      // LOGICAL_ADDRESS_ACCESS [CAM_SYSCTL_PLL_ENABLE]
	{0xCA12, 0x01,1},      //cam_sysctl_pll_enable = 1
	{0xCA13, 0x00,0},      //cam_sysctl_pll_enable = 1
	
{0xCA14, 0x0A98,2}, 		//cam_sysctl_pll_divider_m_n = 2712
{0xCA16, 0x0070,2}, 		//cam_sysctl_pll_divider_p = 112
{0xCA18, 0x7E9C,2}, 		//cam_sysctl_pll_divider_p4_p5_p6 = 32412
{0xCA1C, 0x8043,2}, 		//cam_port_output_control = 32835
{0xCA1E, 0x0004,2}, 		//cam_port_porch = 8
{0xCA20, 0x0F00,2}, 		//cam_port_mipi_timing_t_hs_zero = 3840
{0xCA22, 0x0B07,2}, 		//cam_port_mipi_timing_t_hs_exit_hs_trail = 2823
{0xCA24, 0x0D01,2}, 		//cam_port_mipi_timing_t_clk_post_clk_pre = 3329
{0xCA26, 0x071D,2}, 		//cam_port_mipi_timing_t_clk_trail_clk_zero = 1821
{0xCA28, 0x0006,2}, 		//cam_port_mipi_timing_t_lpx = 6
{0xCA2A, 0x0A0C,2}, 		//cam_port_mipi_timing_init_timing = 2572
{0xCA2C, 0x0000,2}, 		//cam_port_mipi_timing_t_hs_pre = 0


//[Timing_settings]
{0xC800, 0x0020,2}, 		//cam_sensor_cfg_y_addr_start = 32
{0xC802, 0x0100,2}, 		//cam_sensor_cfg_x_addr_start = 256
{0xC804, 0x045D,2}, 		//cam_sensor_cfg_y_addr_end = 1117
{0xC806, 0x06AD,2}, 		//cam_sensor_cfg_x_addr_end = 1709
{0xC808, 0x02C9A44E,4}, 		//cam_sensor_cfg_pixclk = 46769230
{0xC80C, 0x0001,2}, 		//cam_sensor_cfg_row_speed = 1
{0xC80E, 0x06A4,2}, 		//cam_sensor_cfg_fine_integ_time_min = 1700
{0xC810, 0x221B,2}, 		//cam_sensor_cfg_fine_integ_time_max = 8731
{0xC812, 0x026D,2}, 		//cam_sensor_cfg_frame_length_lines = 621
{0xC814, 0x253D,2}, 		//cam_sensor_cfg_line_length_pck = 9533
{0xC816, 0x01D9,2}, 		//cam_sensor_cfg_fine_correction = 473
{0xC818, 0x021B,2}, 		//cam_sensor_cfg_cpipe_last_row = 539
{0xC820, 0x0010,2}, 		//cam_sensor_cfg_reg_0_data = 16
{0xC830, 0x0012,2}, 		//cam_sensor_control_read_mode = 18
{0xC858, 0x0000,2}, 		//cam_crop_window_xoffset = 0
{0xC85A, 0x0000,2}, 		//cam_crop_window_yoffset = 0
{0xC85C, 0x02D0,2}, 		//cam_crop_window_width = 720
{0xC85E, 0x0218,2}, 		//cam_crop_window_height = 536
{0xC86C, 0x0140,2}, 		//cam_output_width = 320
{0xC86E, 0x00F0,2}, 		//cam_output_height = 240
{0xc870,OUT_FORMAT,2},
{0xC87C, 0x00,1}, 		//cam_aet_aemode = 0
{0xC88E, 0x0FCD,2}, 		//cam_aet_max_frame_rate = 4045
{0xC890, 0x0FCD,2}, 		//cam_aet_min_frame_rate = 4045
{0xC94C, 0x0000,2}, 		//cam_stat_awb_clip_window_xstart = 0
{0xC94E, 0x0000,2}, 		//cam_stat_awb_clip_window_ystart = 0
{0xC950, 0x013F,2}, 		//cam_stat_awb_clip_window_xend = 319
{0xC952, 0x00EF,2}, 		//cam_stat_awb_clip_window_yend = 239
{0xC954, 0x0000,2}, 		//cam_stat_ae_initial_window_xstart = 0
{0xC956, 0x0000,2}, 		//cam_stat_ae_initial_window_ystart = 0
{0xC958, 0x003F,2}, 		//cam_stat_ae_initial_window_xend = 63
{0xC95A, 0x002F,2}, 		//cam_stat_ae_initial_window_yend = 47
};

#endif

/* --------------------------------------------------------------------------*/
struct as0260_reg as0260_low_init_regs[] =
{
/*  POLL  MCU_BOOT_MODE::MCU_INFO_CODE =>  0x05, 0x0F, 0x24, 0x30 (4 reads) */
	{0x098E, 0x3E00,2},      // LOGICAL_ADDRESS_ACCESS [CAM_SYSCTL_PLL_ENABLE]
	 {0x3E00, 0x042D,2},      // RESERVED_CORE_3E00
    {0x3E02, 0x39FF,2},      // RESERVED_CORE_3E02
    {0x3E04, 0x49FF,2},      // RESERVED_CORE_3E04
    {0x3E06, 0xFFFF,2},      // RESERVED_CORE_3E06
    {0x3E08, 0x8071,2},      // RESERVED_CORE_3E08
    {0x3E0A, 0x7211,2},      // RESERVED_CORE_3E0A
    {0x3E0C, 0xE040,2},      // RESERVED_CORE_3E0C
    {0x3E0E, 0xA840,2},      // RESERVED_CORE_3E0E
    {0x3E10, 0x4100,2},      // RESERVED_CORE_3E10
    {0x3E12, 0x1846,2},      // RESERVED_CORE_3E12
    {0x3E14, 0xA547,2},      // RESERVED_CORE_3E14
    {0x3E16, 0xAD57,2},      // RESERVED_CORE_3E16
    {0x3E18, 0x8149,2},      // RESERVED_CORE_3E18
    {0x3E1A, 0x9D49,2},      // RESERVED_CORE_3E1A
    {0x3E1C, 0x9F46,2},      // RESERVED_CORE_3E1C
    {0x3E1E, 0x8000,2},      // RESERVED_CORE_3E1E
    {0x3E20, 0x1842,2},      // RESERVED_CORE_3E20
    {0x3E22, 0x4180,2},      // RESERVED_CORE_3E22
    {0x3E24, 0x0018,2},      // RESERVED_CORE_3E24
    {0x3E26, 0x8149,2},      // RESERVED_CORE_3E26
    {0x3E28, 0x9C49,2},      // RESERVED_CORE_3E28
    {0x3E2A, 0x9347,2},      // RESERVED_CORE_3E2A
    {0x3E2C, 0x804D,2},      // RESERVED_CORE_3E2C
    {0x3E2E, 0x804A,2},      // RESERVED_CORE_3E2E
    {0x3E30, 0x100C,2},      // RESERVED_CORE_3E30
    {0x3E32, 0x8000,2},      // RESERVED_CORE_3E32
    {0x3E34, 0x1841,2},      // RESERVED_CORE_3E34
    {0x3E36, 0x4280,2},      // RESERVED_CORE_3E36
    {0x3E38, 0x0018,2},      // RESERVED_CORE_3E38
    {0x3E3A, 0x9710,2},      // RESERVED_CORE_3E3A
    {0x3E3C, 0x0C80,2},      // RESERVED_CORE_3E3C
    {0x3E3E, 0x4DA2,2},      // RESERVED_CORE_3E3E
    {0x3E40, 0x4BA0,2},      // RESERVED_CORE_3E40
    {0x3E42, 0x4A00,2},      // RESERVED_CORE_3E42
    {0x3E44, 0x1880,2},      // RESERVED_CORE_3E44
    {0x3E46, 0x4241,2},      // RESERVED_CORE_3E46
    {0x3E48, 0x0018,2},      // RESERVED_CORE_3E48
    {0x3E4A, 0xB54B,2},      // RESERVED_CORE_3E4A
    {0x3E4C, 0x1C00,2},      // RESERVED_CORE_3E4C
    {0x3E4E, 0x8000,2},      // RESERVED_CORE_3E4E
    {0x3E50, 0x1C10,2},      // RESERVED_CORE_3E50
    {0x3E52, 0x6081,2},      // RESERVED_CORE_3E52
    {0x3E54, 0x1580,2},      // RESERVED_CORE_3E54
    {0x3E56, 0x7C09,2},      // RESERVED_CORE_3E56
    {0x3E58, 0x7000,2},      // RESERVED_CORE_3E58
    {0x3E5A, 0x8082,2},      // RESERVED_CORE_3E5A
    {0x3E5C, 0x7281,2},      // RESERVED_CORE_3E5C
    {0x3E5E, 0x4C40,2},      // RESERVED_CORE_3E5E
    {0x3E60, 0x8E4D,2},      // RESERVED_CORE_3E60
    {0x3E62, 0x8110,2},      // RESERVED_CORE_3E62
    {0x3E64, 0x0CAF,2},      // RESERVED_CORE_3E64
    {0x3E66, 0x4D80,2},      // RESERVED_CORE_3E66
    {0x3E68, 0x100C,2},      // RESERVED_CORE_3E68
    {0x3E6A, 0x8440,2},      // RESERVED_CORE_3E6A
    {0x3E6C, 0x4C81,2},      // RESERVED_CORE_3E6C
    {0x3E6E, 0x7C5B,2},      // RESERVED_CORE_3E6E
    {0x3E70, 0x7000,2},      // RESERVED_CORE_3E70
    {0x3E72, 0x8054,2},      // RESERVED_CORE_3E72
    {0x3E74, 0x924C,2},      // RESERVED_CORE_3E74
    {0x3E76, 0x4078,2},      // RESERVED_CORE_3E76
    {0x3E78, 0x4D4F,2},      // RESERVED_CORE_3E78
    {0x3E7A, 0x4E98,2},      // RESERVED_CORE_3E7A
    {0x3E7C, 0x504E,2},      // RESERVED_CORE_3E7C
    {0x3E7E, 0x4F97,2},      // RESERVED_CORE_3E7E
    {0x3E80, 0x4F4E,2},      // RESERVED_CORE_3E80
    {0x3E82, 0x507C,2},      // RESERVED_CORE_3E82
    {0x3E84, 0x7B8D,2},      // RESERVED_CORE_3E84
    {0x3E86, 0x4D88,2},      // RESERVED_CORE_3E86
    {0x3E88, 0x4E10,2},      // RESERVED_CORE_3E88
    {0x3E8A, 0x0940,2},      // RESERVED_CORE_3E8A
    {0x3E8C, 0x8879,2},      // RESERVED_CORE_3E8C
    {0x3E8E, 0x5481,2},      // RESERVED_CORE_3E8E
    {0x3E90, 0x7000,2},      // RESERVED_CORE_3E90
    {0x3E92, 0x8082,2},      // RESERVED_CORE_3E92
    {0x3E94, 0x7281,2},      // RESERVED_CORE_3E94
    {0x3E96, 0x4C40,2},      // RESERVED_CORE_3E96
    {0x3E98, 0x8E4D,2},      // RESERVED_CORE_3E98
    {0x3E9A, 0x8110,2},      // RESERVED_CORE_3E9A
    {0x3E9C, 0x0CAF,2},      // RESERVED_CORE_3E9C
    {0x3E9E, 0x4D80,2},      // RESERVED_CORE_3E9E
    {0x3EA0, 0x100C,2},      // RESERVED_CORE_3EA0
    {0x3EA2, 0x8440,2},      // RESERVED_CORE_3EA2
    {0x3EA4, 0x4C81,2},      // RESERVED_CORE_3EA4
    {0x3EA6, 0x7C93,2},      // RESERVED_CORE_3EA6
    {0x3EA8, 0x7000,2},      // RESERVED_CORE_3EA8
    {0x3EAA, 0x0000,2},      // RESERVED_CORE_3EAA
    {0x3EAC, 0x0000,2},      // RESERVED_CORE_3EAC
    {0x3EAE, 0x0000,2},      // RESERVED_CORE_3EAE
    {0x3EB0, 0x0000,2},      // RESERVED_CORE_3EB0
    {0x3EB2, 0x0000,2},      // RESERVED_CORE_3EB2
    {0x3EB4, 0x0000,2},      // RESERVED_CORE_3EB4
    {0x3EB6, 0x0000,2},      // RESERVED_CORE_3EB6
    {0x3EB8, 0x0000,2},      // RESERVED_CORE_3EB8
    {0x3EBA, 0x0000,2},      // RESERVED_CORE_3EBA
    {0x3EBC, 0x0000,2},      // RESERVED_CORE_3EBC
    {0x3EBE, 0x0000,2},      // RESERVED_CORE_3EBE
    {0x3EC0, 0x0000,2},      // RESERVED_CORE_3EC0
    {0x3EC2, 0x0000,2},      // RESERVED_CORE_3EC2
    {0x3EC4, 0x0000,2},      // RESERVED_CORE_3EC4
    {0x3EC6, 0x0000,2},      // RESERVED_CORE_3EC6
    {0x3EC8, 0x0000,2},      // RESERVED_CORE_3EC8
    {0x3ECA, 0x0000,2},      // RESERVED_CORE_3ECA
    {0x30B2, 0xC000,2},      // RESERVED_CORE_30B2
    {0x30D4, 0x9400,2},      // RESERVED_CORE_30D4
    {0x31C0, 0x0000,2},      // RESERVED_CORE_31C0
    {0x316A, 0x8200,2},      // RESERVED_CORE_316A
    {0x316C, 0x8200,2},      // RESERVED_CORE_316C
    {0x3EFE, 0x2808,2},      // RESERVED_CORE_3EFE
    {0x3EFC, 0x2868,2},      // RESERVED_CORE_3EFC
    {0x3ED2, 0xD165,2},      // RESERVED_CORE_3ED2
    {0x3EF2, 0xD165,2},      // RESERVED_CORE_3EF2
    {0x3ED8, 0x7F1A,2},      // RESERVED_CORE_3ED8
    {0x3EDA, 0x2828,2},      // RESERVED_CORE_3EDA
    {0x3EE2, 0x0058,2},      // RESERVED_CORE_3EE2
    {0x3EFE, 0x280A,2},      // RESERVED_CORE_3EFE
    {0x3170, 0x000A,2},      // RESERVED_CORE_3170
    {0x3174, 0x8060,2},      // RESERVED_CORE_3174
    {0x317A, 0x000A,2},      // RESERVED_CORE_317A
    {0x3ECC, 0x22B0,2},      // RESERVED_CORE_3ECC
	{0xFFFF,INIT_DELAY,1}, // add
	
    {0x098E, 0x0000,2},      // LOGICAL_ADDRESS_ACCESS
    {0xD1BC, 0x0150,2},      // PGA_TABLE_A_DATA_0
    {0xD1BE, 0xC2AB,2},      // PGA_TABLE_A_DATA_1
    {0xD1C0, 0x15D2,2},      // PGA_TABLE_A_DATA_2
    {0xD1C2, 0xA0EA,2},      // PGA_TABLE_A_DATA_3
    {0xD1C4, 0x12F1,2},      // PGA_TABLE_A_DATA_4
    {0xD1C6, 0x0250,2},      // PGA_TABLE_A_DATA_5
    {0xD1C8, 0x2B2F,2},      // PGA_TABLE_A_DATA_6
    {0xD1CA, 0x1153,2},      // PGA_TABLE_A_DATA_7
    {0xD1CC, 0x22B0,2},      // PGA_TABLE_A_DATA_8
    {0xD1CE, 0x89F1,2},      // PGA_TABLE_A_DATA_9
    {0xD1D0, 0x0250,2},      // PGA_TABLE_A_DATA_10
    {0xD1D2, 0x934D,2},      // PGA_TABLE_A_DATA_11
    {0xD1D4, 0x0AB1,2},      // PGA_TABLE_A_DATA_12
    {0xD1D6, 0x916E,2},      // PGA_TABLE_A_DATA_13
    {0xD1D8, 0x6D51,2},      // PGA_TABLE_A_DATA_14
    {0xD1DA, 0x0730,2},      // PGA_TABLE_A_DATA_15
    {0xD1DC, 0x724E,2},      // PGA_TABLE_A_DATA_16
    {0xD1DE, 0x4D92,2},      // PGA_TABLE_A_DATA_17
    {0xD1E0, 0x0C0F,2},      // PGA_TABLE_A_DATA_18
    {0xD1E2, 0x3EEC,2},      // PGA_TABLE_A_DATA_19
    {0xD1E4, 0xED4C,2},      // PGA_TABLE_A_DATA_20
    {0xD1E6, 0xBAAF,2},      // PGA_TABLE_A_DATA_21
    {0xD1E8, 0x4B30,2},      // PGA_TABLE_A_DATA_22
    {0xD1EA, 0x0D0F,2},      // PGA_TABLE_A_DATA_23
    {0xD1EC, 0xC8B2,2},      // PGA_TABLE_A_DATA_24
    {0xD1EE, 0x868C,2},      // PGA_TABLE_A_DATA_25
    {0xD1F0, 0x0770,2},      // PGA_TABLE_A_DATA_26
    {0xD1F2, 0x1171,2},      // PGA_TABLE_A_DATA_27
    {0xD1F4, 0xFCAF,2},      // PGA_TABLE_A_DATA_28
    {0xD1F6, 0xE6B2,2},      // PGA_TABLE_A_DATA_29
    {0xD1F8, 0xED0C,2},      // PGA_TABLE_A_DATA_30
    {0xD1FA, 0x5D2F,2},      // PGA_TABLE_A_DATA_31
    {0xD1FC, 0x1291,2},      // PGA_TABLE_A_DATA_32
    {0xD1FE, 0xCDF1,2},      // PGA_TABLE_A_DATA_33
    {0xD200, 0x8F93,2},      // PGA_TABLE_A_DATA_34
    {0xD202, 0x966E,2},      // PGA_TABLE_A_DATA_35
    {0xD204, 0xBB0F,2},      // PGA_TABLE_A_DATA_36
    {0xD206, 0x1BF1,2},      // PGA_TABLE_A_DATA_37
    {0xD208, 0x9130,2},      // PGA_TABLE_A_DATA_38
    {0xD20A, 0x8BB3,2},      // PGA_TABLE_A_DATA_39
    {0xD20C, 0x26B2,2},      // PGA_TABLE_A_DATA_40
    {0xD20E, 0x2AEE,2},      // PGA_TABLE_A_DATA_41
    {0xD210, 0x2173,2},      // PGA_TABLE_A_DATA_42
    {0xD212, 0x1693,2},      // PGA_TABLE_A_DATA_43
    {0xD214, 0x8496,2},      // PGA_TABLE_A_DATA_44
    {0xD216, 0x0B13,2},      // PGA_TABLE_A_DATA_45
    {0xD218, 0x4811,2},      // PGA_TABLE_A_DATA_46
    {0xD21A, 0x1B94,2},      // PGA_TABLE_A_DATA_47
    {0xD21C, 0xBE92,2},      // PGA_TABLE_A_DATA_48
    {0xD21E, 0xE4B6,2},      // PGA_TABLE_A_DATA_49
    {0xD220, 0x3031,2},      // PGA_TABLE_A_DATA_50
    {0xD222, 0xA50E,2},      // PGA_TABLE_A_DATA_51
    {0xD224, 0x0E93,2},      // PGA_TABLE_A_DATA_52
    {0xD226, 0x2494,2},      // PGA_TABLE_A_DATA_53
    {0xD228, 0x9AB0,2},      // PGA_TABLE_A_DATA_54
    {0xD22A, 0x01F2,2},      // PGA_TABLE_A_DATA_55
    {0xD22C, 0x7A8E,2},      // PGA_TABLE_A_DATA_56
    {0xD22E, 0x0FD3,2},      // PGA_TABLE_A_DATA_57
    {0xD230, 0x1ED2,2},      // PGA_TABLE_A_DATA_58
    {0xD232, 0xB6B5,2},      // PGA_TABLE_A_DATA_59
    {0xD234, 0x0151,2},      // PGA_TABLE_A_DATA_60
    {0xD236, 0xF651,2},      // PGA_TABLE_A_DATA_61
    {0xD238, 0xA3F4,2},      // PGA_TABLE_A_DATA_62
    {0xD23A, 0x6734,2},      // PGA_TABLE_A_DATA_63
    {0xD23C, 0x0E16,2},      // PGA_TABLE_A_DATA_64
    {0xD23E, 0x1471,2},      // PGA_TABLE_A_DATA_65
    {0xD240, 0xB670,2},      // PGA_TABLE_A_DATA_66
    {0xD242, 0xA034,2},      // PGA_TABLE_A_DATA_67
    {0xD244, 0x6ACB,2},      // PGA_TABLE_A_DATA_68
    {0xD246, 0x6E34,2},      // PGA_TABLE_A_DATA_69
    {0xD248, 0xC950,2},      // PGA_TABLE_A_DATA_70
    {0xD24A, 0xE7B0,2},      // PGA_TABLE_A_DATA_71
    {0xD24C, 0x9A73,2},      // PGA_TABLE_A_DATA_72
    {0xD24E, 0x0074,2},      // PGA_TABLE_A_DATA_73
    {0xD250, 0x2556,2},      // PGA_TABLE_A_DATA_74
    {0xD252, 0x586E,2},      // PGA_TABLE_A_DATA_75
    {0xD254, 0xAED1,2},      // PGA_TABLE_A_DATA_76
    {0xD256, 0xF533,2},      // PGA_TABLE_A_DATA_77
    {0xD258, 0x1BF5,2},      // PGA_TABLE_A_DATA_78
    {0xD25A, 0x7756,2},      // PGA_TABLE_A_DATA_79
    {0xD25C, 0x17D4,2},      // PGA_TABLE_A_DATA_80
    {0xD25E, 0x88F4,2},      // PGA_TABLE_A_DATA_81
    {0xD260, 0xE538,2},      // PGA_TABLE_A_DATA_82
    {0xD262, 0x3AB8,2},      // PGA_TABLE_A_DATA_83
    {0xD264, 0x69FB,2},      // PGA_TABLE_A_DATA_84
    {0xD266, 0x10D4,2},      // PGA_TABLE_A_DATA_85
    {0xD268, 0xBED5,2},      // PGA_TABLE_A_DATA_86
    {0xD26A, 0xAD79,2},      // PGA_TABLE_A_DATA_87
    {0xD26C, 0x47B8,2},      // PGA_TABLE_A_DATA_88
    {0xD26E, 0x16DC,2},      // PGA_TABLE_A_DATA_89
    {0xD270, 0x3C73,2},      // PGA_TABLE_A_DATA_90
    {0xD272, 0x3F12,2},      // PGA_TABLE_A_DATA_91
    {0xD274, 0x8CB8,2},      // PGA_TABLE_A_DATA_92
    {0xD276, 0x5EF6,2},      // PGA_TABLE_A_DATA_93
    {0xD278, 0x0E5B,2},      // PGA_TABLE_A_DATA_94
    {0xD27A, 0x4514,2},      // PGA_TABLE_A_DATA_95
    {0xD27C, 0x83B4,2},      // PGA_TABLE_A_DATA_96
    {0xD27E, 0xEE18,2},      // PGA_TABLE_A_DATA_97
    {0xD280, 0x75B7,2},      // PGA_TABLE_A_DATA_98
    {0xD282, 0x675B,2},      // PGA_TABLE_A_DATA_99
    
    {0xD286, 0x03EC,2},      // PGA_TABLE_A_CENTER_COLUMN
    {0xD284, 0x020C,2},      // PGA_TABLE_A_CENTER_ROW
    {0xC9F4, 0x0AF0,2},      // CAM_PGA_L_CONFIG_COLOUR_TEMP
    {0xC9F6, 0x8000,2},      // CAM_PGA_L_CONFIG_GREEN_RED_Q14
    {0xC9F8, 0x8000,2},      // CAM_PGA_L_CONFIG_RED_Q14
    {0xC9FA, 0x8000,2},      // CAM_PGA_L_CONFIG_GREEN_BLUE_Q14
    {0xC9FC, 0x8000,2},      // CAM_PGA_L_CONFIG_BLUE_Q14

    {0xD0F0, 0x0130,2},      // PGA_TABLE_CWF_DATA_0
    {0xD0F2, 0x42CA,2},      // PGA_TABLE_CWF_DATA_1
    {0xD0F4, 0x21F2,2},      // PGA_TABLE_CWF_DATA_2
    {0xD0F6, 0x3E2B,2},      // PGA_TABLE_CWF_DATA_3
    {0xD0F8, 0x0010,2},      // PGA_TABLE_CWF_DATA_4
    {0xD0FA, 0x0230,2},      // PGA_TABLE_CWF_DATA_5
    {0xD0FC, 0x7E4E,2},      // PGA_TABLE_CWF_DATA_6
    {0xD0FE, 0x3E52,2},      // PGA_TABLE_CWF_DATA_7
    {0xD100, 0x45F0,2},      // PGA_TABLE_CWF_DATA_8
    {0xD102, 0x812F,2},      // PGA_TABLE_CWF_DATA_9
    {0xD104, 0x01F0,2},      // PGA_TABLE_CWF_DATA_10
    {0xD106, 0xE2AC,2},      // PGA_TABLE_CWF_DATA_11
    {0xD108, 0x1491,2},      // PGA_TABLE_CWF_DATA_12
    {0xD10A, 0xC2CB,2},      // PGA_TABLE_CWF_DATA_13
    {0xD10C, 0x4DB1,2},      // PGA_TABLE_CWF_DATA_14
    {0xD10E, 0x0310,2},      // PGA_TABLE_CWF_DATA_15
    {0xD110, 0x724E,2},      // PGA_TABLE_CWF_DATA_16
    {0xD112, 0x3DB2,2},      // PGA_TABLE_CWF_DATA_17
    {0xD114, 0x438F,2},      // PGA_TABLE_CWF_DATA_18
    {0xD116, 0x69CE,2},      // PGA_TABLE_CWF_DATA_19
    {0xD118, 0xAE8D,2},      // PGA_TABLE_CWF_DATA_20
    {0xD11A, 0xB80F,2},      // PGA_TABLE_CWF_DATA_21
    {0xD11C, 0x2E10,2},      // PGA_TABLE_CWF_DATA_22
    {0xD11E, 0x210E,2},      // PGA_TABLE_CWF_DATA_23
    {0xD120, 0xAA72,2},      // PGA_TABLE_CWF_DATA_24
    {0xD122, 0xD5AC,2},      // PGA_TABLE_CWF_DATA_25
    {0xD124, 0x5BAF,2},      // PGA_TABLE_CWF_DATA_26
    {0xD126, 0x0D51,2},      // PGA_TABLE_CWF_DATA_27
    {0xD128, 0xAA70,2},      // PGA_TABLE_CWF_DATA_28
    {0xD12A, 0x9D53,2},      // PGA_TABLE_CWF_DATA_29
    {0xD12C, 0xB98D,2},      // PGA_TABLE_CWF_DATA_30
    {0xD12E, 0x74AF,2},      // PGA_TABLE_CWF_DATA_31
    {0xD130, 0x03F1,2},      // PGA_TABLE_CWF_DATA_32
    {0xD132, 0xBEF1,2},      // PGA_TABLE_CWF_DATA_33
    {0xD134, 0xD072,2},      // PGA_TABLE_CWF_DATA_34
    {0xD136, 0x9C8E,2},      // PGA_TABLE_CWF_DATA_35
    {0xD138, 0xC6EF,2},      // PGA_TABLE_CWF_DATA_36
    {0xD13A, 0x08F1,2},      // PGA_TABLE_CWF_DATA_37
    {0xD13C, 0xA64C,2},      // PGA_TABLE_CWF_DATA_38
    {0xD13E, 0xCB92,2},      // PGA_TABLE_CWF_DATA_39
    {0xD140, 0x1672,2},      // PGA_TABLE_CWF_DATA_40
    {0xD142, 0x692D,2},      // PGA_TABLE_CWF_DATA_41
    {0xD144, 0x68D2,2},      // PGA_TABLE_CWF_DATA_42
    {0xD146, 0x2DF3,2},      // PGA_TABLE_CWF_DATA_43
    {0xD148, 0xC135,2},      // PGA_TABLE_CWF_DATA_44
    {0xD14A, 0x4692,2},      // PGA_TABLE_CWF_DATA_45
    {0xD14C, 0x38D1,2},      // PGA_TABLE_CWF_DATA_46
    {0xD14E, 0x2932,2},      // PGA_TABLE_CWF_DATA_47
    {0xD150, 0x8013,2},      // PGA_TABLE_CWF_DATA_48
    {0xD152, 0xE714,2},      // PGA_TABLE_CWF_DATA_49
    {0xD154, 0x3D11,2},      // PGA_TABLE_CWF_DATA_50
    {0xD156, 0x132F,2},      // PGA_TABLE_CWF_DATA_51
    {0xD158, 0x6B71,2},      // PGA_TABLE_CWF_DATA_52
    {0xD15A, 0x0FF3,2},      // PGA_TABLE_CWF_DATA_53
    {0xD15C, 0x49D3,2},      // PGA_TABLE_CWF_DATA_54
    {0xD15E, 0x0C72,2},      // PGA_TABLE_CWF_DATA_55
    {0xD160, 0x052E,2},      // PGA_TABLE_CWF_DATA_56
    {0xD162, 0x5DD2,2},      // PGA_TABLE_CWF_DATA_57
    {0xD164, 0x4A71,2},      // PGA_TABLE_CWF_DATA_58
    {0xD166, 0xCCF5,2},      // PGA_TABLE_CWF_DATA_59
    {0xD168, 0x3E30,2},      // PGA_TABLE_CWF_DATA_60
    {0xD16A, 0x84F1,2},      // PGA_TABLE_CWF_DATA_61
    {0xD16C, 0x96D3,2},      // PGA_TABLE_CWF_DATA_62
    {0xD16E, 0x30D3,2},      // PGA_TABLE_CWF_DATA_63
    {0xD170, 0x9894,2},      // PGA_TABLE_CWF_DATA_64
    {0xD172, 0x1FB1,2},      // PGA_TABLE_CWF_DATA_65
    {0xD174, 0xE02F,2},      // PGA_TABLE_CWF_DATA_66
    {0xD176, 0xD054,2},      // PGA_TABLE_CWF_DATA_67
    {0xD178, 0xF5D0,2},      // PGA_TABLE_CWF_DATA_68
    {0xD17A, 0x1716,2},      // PGA_TABLE_CWF_DATA_69
    {0xD17C, 0x96B0,2},      // PGA_TABLE_CWF_DATA_70
    {0xD17E, 0xD0B0,2},      // PGA_TABLE_CWF_DATA_71
    {0xD180, 0x0412,2},      // PGA_TABLE_CWF_DATA_72
    {0xD182, 0xB890,2},      // PGA_TABLE_CWF_DATA_73
    {0xD184, 0xD055,2},      // PGA_TABLE_CWF_DATA_74
    {0xD186, 0xD26A,2},      // PGA_TABLE_CWF_DATA_75
    {0xD188, 0xB730,2},      // PGA_TABLE_CWF_DATA_76
    {0xD18A, 0xBE10,2},      // PGA_TABLE_CWF_DATA_77
    {0xD18C, 0x64D3,2},      // PGA_TABLE_CWF_DATA_78
    {0xD18E, 0x9493,2},      // PGA_TABLE_CWF_DATA_79
    {0xD190, 0x1CF4,2},      // PGA_TABLE_CWF_DATA_80
    {0xD192, 0xFE13,2},      // PGA_TABLE_CWF_DATA_81
    {0xD194, 0xD638,2},      // PGA_TABLE_CWF_DATA_82
    {0xD196, 0x40D8,2},      // PGA_TABLE_CWF_DATA_83
    {0xD198, 0x5EBB,2},      // PGA_TABLE_CWF_DATA_84
    {0xD19A, 0x5012,2},      // PGA_TABLE_CWF_DATA_85
    {0xD19C, 0xA6F5,2},      // PGA_TABLE_CWF_DATA_86
    {0xD19E, 0xC898,2},      // PGA_TABLE_CWF_DATA_87
    {0xD1A0, 0x43F8,2},      // PGA_TABLE_CWF_DATA_88
    {0xD1A2, 0x4FFB,2},      // PGA_TABLE_CWF_DATA_89
    {0xD1A4, 0x2F93,2},      // PGA_TABLE_CWF_DATA_90
    {0xD1A6, 0xDE33,2},      // PGA_TABLE_CWF_DATA_91
    {0xD1A8, 0x90D8,2},      // PGA_TABLE_CWF_DATA_92
    {0xD1AA, 0x4838,2},      // PGA_TABLE_CWF_DATA_93
    {0xD1AC, 0x249B,2},      // PGA_TABLE_CWF_DATA_94
    {0xD1AE, 0x25D4,2},      // PGA_TABLE_CWF_DATA_95
    {0xD1B0, 0xEC73,2},      // PGA_TABLE_CWF_DATA_96
    {0xD1B2, 0xDAD8,2},      // PGA_TABLE_CWF_DATA_97
    {0xD1B4, 0x1878,2},      // PGA_TABLE_CWF_DATA_98
    {0xD1B6, 0x64DB,2},      // PGA_TABLE_CWF_DATA_99
    {0xD1BA, 0x03EC,2},      // PGA_TABLE_CWF_CENTER_COLUMN
    {0xD1B8, 0x0200,2},      // PGA_TABLE_CWF_CENTER_ROW
    {0xC9FE, 0x0FA0,2},      // CAM_PGA_M_CONFIG_COLOUR_TEMP
    {0xCA00, 0x8000,2},      // CAM_PGA_M_CONFIG_GREEN_RED_Q14
    {0xCA02, 0x8000,2},      // CAM_PGA_M_CONFIG_RED_Q14
    {0xCA04, 0x8000,2},      // CAM_PGA_M_CONFIG_GREEN_BLUE_Q14
    {0xCA06, 0x8000,2},      // CAM_PGA_M_CONFIG_BLUE_Q14

    {0xD024, 0x0110,2},      // PGA_TABLE_D65_DATA_0
    {0xD026, 0xC1AD,2},      // PGA_TABLE_D65_DATA_1
    {0xD028, 0x2792,2},      // PGA_TABLE_D65_DATA_2
    {0xD02A, 0xEB2B,2},      // PGA_TABLE_D65_DATA_3
    {0xD02C, 0x1630,2},      // PGA_TABLE_D65_DATA_4
    {0xD02E, 0x0190,2},      // PGA_TABLE_D65_DATA_5
    {0xD030, 0x22EE,2},      // PGA_TABLE_D65_DATA_6
    {0xD032, 0x74F2,2},      // PGA_TABLE_D65_DATA_7
    {0xD034, 0x5750,2},      // PGA_TABLE_D65_DATA_8
    {0xD036, 0x9FB1,2},      // PGA_TABLE_D65_DATA_9
    {0xD038, 0x01B0,2},      // PGA_TABLE_D65_DATA_10
    {0xD03A, 0xD14D,2},      // PGA_TABLE_D65_DATA_11
    {0xD03C, 0x4E71,2},      // PGA_TABLE_D65_DATA_12
    {0xD03E, 0xC56E,2},      // PGA_TABLE_D65_DATA_13
    {0xD040, 0x1D70,2},      // PGA_TABLE_D65_DATA_14
    {0xD042, 0x0230,2},      // PGA_TABLE_D65_DATA_15
    {0xD044, 0x60ED,2},      // PGA_TABLE_D65_DATA_16
    {0xD046, 0x3A32,2},      // PGA_TABLE_D65_DATA_17
    {0xD048, 0x372F,2},      // PGA_TABLE_D65_DATA_18
    {0xD04A, 0x26AE,2},      // PGA_TABLE_D65_DATA_19
    {0xD04C, 0xA18D,2},      // PGA_TABLE_D65_DATA_20
    {0xD04E, 0xC1AF,2},      // PGA_TABLE_D65_DATA_21
    {0xD050, 0x3910,2},      // PGA_TABLE_D65_DATA_22
    {0xD052, 0x026F,2},      // PGA_TABLE_D65_DATA_23
    {0xD054, 0xCFB2,2},      // PGA_TABLE_D65_DATA_24
    {0xD056, 0x9F6D,2},      // PGA_TABLE_D65_DATA_25
    {0xD058, 0x5D4F,2},      // PGA_TABLE_D65_DATA_26
    {0xD05A, 0x16B1,2},      // PGA_TABLE_D65_DATA_27
    {0xD05C, 0xAC0E,2},      // PGA_TABLE_D65_DATA_28
    {0xD05E, 0xEA92,2},      // PGA_TABLE_D65_DATA_29
    {0xD060, 0x818E,2},      // PGA_TABLE_D65_DATA_30
    {0xD062, 0x5DAF,2},      // PGA_TABLE_D65_DATA_31
    {0xD064, 0x4591,2},      // PGA_TABLE_D65_DATA_32
    {0xD066, 0x9151,2},      // PGA_TABLE_D65_DATA_33
    {0xD068, 0x9D13,2},      // PGA_TABLE_D65_DATA_34
    {0xD06A, 0x978E,2},      // PGA_TABLE_D65_DATA_35
    {0xD06C, 0xDAEF,2},      // PGA_TABLE_D65_DATA_36
    {0xD06E, 0x1971,2},      // PGA_TABLE_D65_DATA_37
    {0xD070, 0x2CAD,2},      // PGA_TABLE_D65_DATA_38
    {0xD072, 0xFFB2,2},      // PGA_TABLE_D65_DATA_39
    {0xD074, 0x1952,2},      // PGA_TABLE_D65_DATA_40
    {0xD076, 0x878F,2},      // PGA_TABLE_D65_DATA_41
    {0xD078, 0x2373,2},      // PGA_TABLE_D65_DATA_42
    {0xD07A, 0x2594,2},      // PGA_TABLE_D65_DATA_43
    {0xD07C, 0x8B76,2},      // PGA_TABLE_D65_DATA_44
    {0xD07E, 0x7212,2},      // PGA_TABLE_D65_DATA_45
    {0xD080, 0x2F31,2},      // PGA_TABLE_D65_DATA_46
    {0xD082, 0x78F3,2},      // PGA_TABLE_D65_DATA_47
    {0xD084, 0xB5EE,2},      // PGA_TABLE_D65_DATA_48
    {0xD086, 0xAF36,2},      // PGA_TABLE_D65_DATA_49
    {0xD088, 0x7351,2},      // PGA_TABLE_D65_DATA_50
    {0xD08A, 0x1A46,2},      // PGA_TABLE_D65_DATA_51
    {0xD08C, 0xCE71,2},      // PGA_TABLE_D65_DATA_52
    {0xD08E, 0x00D4,2},      // PGA_TABLE_D65_DATA_53
    {0xD090, 0x1B55,2},      // PGA_TABLE_D65_DATA_54
    {0xD092, 0x0D12,2},      // PGA_TABLE_D65_DATA_55
    {0xD094, 0x0DEB,2},      // PGA_TABLE_D65_DATA_56
    {0xD096, 0x49D2,2},      // PGA_TABLE_D65_DATA_57
    {0xD098, 0x1533,2},      // PGA_TABLE_D65_DATA_58
    {0xD09A, 0xD7B4,2},      // PGA_TABLE_D65_DATA_59
    {0xD09C, 0x58F0,2},      // PGA_TABLE_D65_DATA_60
    {0xD09E, 0xA1B1,2},      // PGA_TABLE_D65_DATA_61
    {0xD0A0, 0xD5F3,2},      // PGA_TABLE_D65_DATA_62
    {0xD0A2, 0x0B74,2},      // PGA_TABLE_D65_DATA_63
    {0xD0A4, 0x22D3,2},      // PGA_TABLE_D65_DATA_64
    {0xD0A6, 0x3291,2},      // PGA_TABLE_D65_DATA_65
    {0xD0A8, 0x0BC9,2},      // PGA_TABLE_D65_DATA_66
    {0xD0AA, 0x87B4,2},      // PGA_TABLE_D65_DATA_67
    {0xD0AC, 0xE4B2,2},      // PGA_TABLE_D65_DATA_68
    {0xD0AE, 0x8BD2,2},      // PGA_TABLE_D65_DATA_69
    {0xD0B0, 0xCDAE,2},      // PGA_TABLE_D65_DATA_70
    {0xD0B2, 0xBB10,2},      // PGA_TABLE_D65_DATA_71
    {0xD0B4, 0xED52,2},      // PGA_TABLE_D65_DATA_72
    {0xD0B6, 0x3B92,2},      // PGA_TABLE_D65_DATA_73
    {0xD0B8, 0x3013,2},      // PGA_TABLE_D65_DATA_74
    {0xD0BA, 0x100E,2},      // PGA_TABLE_D65_DATA_75
    {0xD0BC, 0x8830,2},      // PGA_TABLE_D65_DATA_76
    {0xD0BE, 0x9193,2},      // PGA_TABLE_D65_DATA_77
    {0xD0C0, 0x0334,2},      // PGA_TABLE_D65_DATA_78
    {0xD0C2, 0x42F5,2},      // PGA_TABLE_D65_DATA_79
    {0xD0C4, 0x1A54,2},      // PGA_TABLE_D65_DATA_80
    {0xD0C6, 0x59D3,2},      // PGA_TABLE_D65_DATA_81
    {0xD0C8, 0x8319,2},      // PGA_TABLE_D65_DATA_82
    {0xD0CA, 0x9ED4,2},      // PGA_TABLE_D65_DATA_83
    {0xD0CC, 0x7B1B,2},      // PGA_TABLE_D65_DATA_84
    {0xD0CE, 0x67D3,2},      // PGA_TABLE_D65_DATA_85
    {0xD0D0, 0xB534,2},      // PGA_TABLE_D65_DATA_86
    {0xD0D2, 0xA5B9,2},      // PGA_TABLE_D65_DATA_87
    {0xD0D4, 0x2390,2},      // PGA_TABLE_D65_DATA_88
    {0xD0D6, 0x0BFC,2},      // PGA_TABLE_D65_DATA_89
    {0xD0D8, 0x4F52,2},      // PGA_TABLE_D65_DATA_90
    {0xD0DA, 0x1EF3,2},      // PGA_TABLE_D65_DATA_91
    {0xD0DC, 0x8CD8,2},      // PGA_TABLE_D65_DATA_92
    {0xD0DE, 0x3FB1,2},      // PGA_TABLE_D65_DATA_93
    {0xD0E0, 0x191B,2},      // PGA_TABLE_D65_DATA_94
    {0xD0E2, 0x1C74,2},      // PGA_TABLE_D65_DATA_95
    {0xD0E4, 0x3890,2},      // PGA_TABLE_D65_DATA_96
    {0xD0E6, 0xD138,2},      // PGA_TABLE_D65_DATA_97
    {0xD0E8, 0xA375,2},      // PGA_TABLE_D65_DATA_98
    {0xD0EA, 0x44DB,2},      // PGA_TABLE_D65_DATA_99
    
    {0xD0EE, 0x03C0,2},      // PGA_TABLE_D65_CENTER_COLUMN
    {0xD0EC, 0x0200,2},      // PGA_TABLE_D65_CENTER_ROW
    {0xCA08, 0x1964,2},      // CAM_PGA_R_CONFIG_COLOUR_TEMP
    {0xCA0A, 0x8000,2},      // CAM_PGA_R_CONFIG_GREEN_RED_Q14
    {0xCA0C, 0x8000,2},      // CAM_PGA_R_CONFIG_RED_Q14
    {0xCA0E, 0x8000,2},      // CAM_PGA_R_CONFIG_GREEN_BLUE_Q14
    {0xCA10, 0x8000,2},      // CAM_PGA_R_CONFIG_BLUE_Q14
    {0x098E, 0x0000,2},      // LOGICAL_ADDRESS_ACCESS
    {0xD1BC, 0x0110,2},      // PGA_TABLE_A_DATA_0
    {0xD1BE, 0xC4EC,2},      // PGA_TABLE_A_DATA_1
    {0xD1C0, 0x0C72,2},      // PGA_TABLE_A_DATA_2
    {0xD1C2, 0x0B70,2},      // PGA_TABLE_A_DATA_3
    {0xD1C4, 0x8E90,2},      // PGA_TABLE_A_DATA_4
    {0xD1C6, 0x0270,2},      // PGA_TABLE_A_DATA_5
    {0xD1C8, 0x2C8F,2},      // PGA_TABLE_A_DATA_6
    {0xD1CA, 0x04B3,2},      // PGA_TABLE_A_DATA_7
    {0xD1CC, 0x1FB0,2},      // PGA_TABLE_A_DATA_8
    {0xD1CE, 0xA832,2},      // PGA_TABLE_A_DATA_9
    {0xD1D0, 0x0290,2},      // PGA_TABLE_A_DATA_10
    {0xD1D2, 0xE80C,2},      // PGA_TABLE_A_DATA_11
    {0xD1D4, 0x73D0,2},      // PGA_TABLE_A_DATA_12
    {0xD1D6, 0x080D,2},      // PGA_TABLE_A_DATA_13
    {0xD1D8, 0xCCEF,2},      // PGA_TABLE_A_DATA_14
    {0xD1DA, 0x07B0,2},      // PGA_TABLE_A_DATA_15
    {0xD1DC, 0x032F,2},      // PGA_TABLE_A_DATA_16
    {0xD1DE, 0x2CD2,2},      // PGA_TABLE_A_DATA_17
    {0xD1E0, 0xFA4A,2},      // PGA_TABLE_A_DATA_18
    {0xD1E2, 0x84B2,2},      // PGA_TABLE_A_DATA_19
    {0xD1E4, 0xC20B,2},      // PGA_TABLE_A_DATA_20
    {0xD1E6, 0xE36F,2},      // PGA_TABLE_A_DATA_21
    {0xD1E8, 0x9A10,2},      // PGA_TABLE_A_DATA_22
    {0xD1EA, 0x55D0,2},      // PGA_TABLE_A_DATA_23
    {0xD1EC, 0x34F1,2},      // PGA_TABLE_A_DATA_24
    {0xD1EE, 0xAA6A,2},      // PGA_TABLE_A_DATA_25
    {0xD1F0, 0x4E4F,2},      // PGA_TABLE_A_DATA_26
    {0xD1F2, 0xAAEF,2},      // PGA_TABLE_A_DATA_27
    {0xD1F4, 0xA2AF,2},      // PGA_TABLE_A_DATA_28
    {0xD1F6, 0x1C2F,2},      // PGA_TABLE_A_DATA_29
    {0xD1F8, 0x854C,2},      // PGA_TABLE_A_DATA_30
    {0xD1FA, 0x59CF,2},      // PGA_TABLE_A_DATA_31
    {0xD1FC, 0xCE4E,2},      // PGA_TABLE_A_DATA_32
    {0xD1FE, 0xB131,2},      // PGA_TABLE_A_DATA_33
    {0xD200, 0x2651,2},      // PGA_TABLE_A_DATA_34
    {0xD202, 0xEBCD,2},      // PGA_TABLE_A_DATA_35
    {0xD204, 0xE2CF,2},      // PGA_TABLE_A_DATA_36
    {0xD206, 0xE10F,2},      // PGA_TABLE_A_DATA_37
    {0xD208, 0x1051,2},      // PGA_TABLE_A_DATA_38
    {0xD20A, 0x0833,2},      // PGA_TABLE_A_DATA_39
    {0xD20C, 0x26B2,2},      // PGA_TABLE_A_DATA_40
    {0xD20E, 0x01B0,2},      // PGA_TABLE_A_DATA_41
    {0xD210, 0x3593,2},      // PGA_TABLE_A_DATA_42
    {0xD212, 0x5AF2,2},      // PGA_TABLE_A_DATA_43
    {0xD214, 0x9196,2},      // PGA_TABLE_A_DATA_44
    {0xD216, 0x0653,2},      // PGA_TABLE_A_DATA_45
    {0xD218, 0x07D2,2},      // PGA_TABLE_A_DATA_46
    {0xD21A, 0x7814,2},      // PGA_TABLE_A_DATA_47
    {0xD21C, 0x9E75,2},      // PGA_TABLE_A_DATA_48
    {0xD21E, 0xAB97,2},      // PGA_TABLE_A_DATA_49
    {0xD220, 0x2A31,2},      // PGA_TABLE_A_DATA_50
    {0xD222, 0x17CE,2},      // PGA_TABLE_A_DATA_51
    {0xD224, 0x7652,2},      // PGA_TABLE_A_DATA_52
    {0xD226, 0x56F2,2},      // PGA_TABLE_A_DATA_53
    {0xD228, 0xA435,2},      // PGA_TABLE_A_DATA_54
    {0xD22A, 0x0E92,2},      // PGA_TABLE_A_DATA_55
    {0xD22C, 0x2C70,2},      // PGA_TABLE_A_DATA_56
    {0xD22E, 0x3E32,2},      // PGA_TABLE_A_DATA_57
    {0xD230, 0xA4F4,2},      // PGA_TABLE_A_DATA_58
    {0xD232, 0xD3D5,2},      // PGA_TABLE_A_DATA_59
    {0xD234, 0x2251,2},      // PGA_TABLE_A_DATA_60
    {0xD236, 0x2552,2},      // PGA_TABLE_A_DATA_61
    {0xD238, 0x3A53,2},      // PGA_TABLE_A_DATA_62
    {0xD23A, 0xA734,2},      // PGA_TABLE_A_DATA_63
    {0xD23C, 0xE1D5,2},      // PGA_TABLE_A_DATA_64
    {0xD23E, 0x41B1,2},      // PGA_TABLE_A_DATA_65
    {0xD240, 0x6D72,2},      // PGA_TABLE_A_DATA_66
    {0xD242, 0x4354,2},      // PGA_TABLE_A_DATA_67
    {0xD244, 0xE675,2},      // PGA_TABLE_A_DATA_68
    {0xD246, 0x9737,2},      // PGA_TABLE_A_DATA_69
    {0xD248, 0xD68F,2},      // PGA_TABLE_A_DATA_70
    {0xD24A, 0x1231,2},      // PGA_TABLE_A_DATA_71
    {0xD24C, 0x6F74,2},      // PGA_TABLE_A_DATA_72
    {0xD24E, 0xDB93,2},      // PGA_TABLE_A_DATA_73
    {0xD250, 0xB4D6,2},      // PGA_TABLE_A_DATA_74
    {0xD252, 0x1B4F,2},      // PGA_TABLE_A_DATA_75
    {0xD254, 0x56F2,2},      // PGA_TABLE_A_DATA_76
    {0xD256, 0x29F5,2},      // PGA_TABLE_A_DATA_77
    {0xD258, 0x8F15,2},      // PGA_TABLE_A_DATA_78
    {0xD25A, 0x8777,2},      // PGA_TABLE_A_DATA_79
    {0xD25C, 0x9392,2},      // PGA_TABLE_A_DATA_80
    {0xD25E, 0xBF2F,2},      // PGA_TABLE_A_DATA_81
    {0xD260, 0x8E98,2},      // PGA_TABLE_A_DATA_82
    {0xD262, 0xC7B4,2},      // PGA_TABLE_A_DATA_83
    {0xD264, 0x61D9,2},      // PGA_TABLE_A_DATA_84
    {0xD266, 0x4B72,2},      // PGA_TABLE_A_DATA_85
    {0xD268, 0xA994,2},      // PGA_TABLE_A_DATA_86
    {0xD26A, 0xFF18,2},      // PGA_TABLE_A_DATA_87
    {0xD26C, 0x4E95,2},      // PGA_TABLE_A_DATA_88
    {0xD26E, 0x31DA,2},      // PGA_TABLE_A_DATA_89
    {0xD270, 0x1352,2},      // PGA_TABLE_A_DATA_90
    {0xD272, 0x3EB3,2},      // PGA_TABLE_A_DATA_91
    {0xD274, 0x8637,2},      // PGA_TABLE_A_DATA_92
    {0xD276, 0xB2B4,2},      // PGA_TABLE_A_DATA_93
    {0xD278, 0x1AF9,2},      // PGA_TABLE_A_DATA_94
    {0xD27A, 0x0EF0,2},      // PGA_TABLE_A_DATA_95
    {0xD27C, 0xF4F2,2},      // PGA_TABLE_A_DATA_96
    {0xD27E, 0xEF77,2},      // PGA_TABLE_A_DATA_97
    {0xD280, 0x0334,2},      // PGA_TABLE_A_DATA_98
    {0xD282, 0x28B9,2},      // PGA_TABLE_A_DATA_99
    
    {0xD286, 0x040C,2},      // PGA_TABLE_A_CENTER_COLUMN
    {0xD284, 0x022C,2},      // PGA_TABLE_A_CENTER_ROW
    {0xC9F4, 0x0AF0,2},      // CAM_PGA_L_CONFIG_COLOUR_TEMP
    {0xC9F6, 0x8000,2},      // CAM_PGA_L_CONFIG_GREEN_RED_Q14
    {0xC9F8, 0x8000,2},      // CAM_PGA_L_CONFIG_RED_Q14
    {0xC9FA, 0x8000,2},      // CAM_PGA_L_CONFIG_GREEN_BLUE_Q14
    {0xC9FC, 0x8000,2},      // CAM_PGA_L_CONFIG_BLUE_Q14

    {0xD0F0, 0x0130,2},      // PGA_TABLE_CWF_DATA_0
    {0xD0F2, 0x87EE,2},      // PGA_TABLE_CWF_DATA_1
    {0xD0F4, 0x1732,2},      // PGA_TABLE_CWF_DATA_2
    {0xD0F6, 0x04B0,2},      // PGA_TABLE_CWF_DATA_3
    {0xD0F8, 0xA9B1,2},      // PGA_TABLE_CWF_DATA_4
    {0xD0FA, 0x0170,2},      // PGA_TABLE_CWF_DATA_5
    {0xD0FC, 0x320E,2},      // PGA_TABLE_CWF_DATA_6
    {0xD0FE, 0x3BF2,2},      // PGA_TABLE_CWF_DATA_7
    {0xD100, 0x0D30,2},      // PGA_TABLE_CWF_DATA_8
    {0xD102, 0xAF72,2},      // PGA_TABLE_CWF_DATA_9
    {0xD104, 0x0230,2},      // PGA_TABLE_CWF_DATA_10
    {0xD106, 0xEF4D,2},      // PGA_TABLE_CWF_DATA_11
    {0xD108, 0x1AD1,2},      // PGA_TABLE_CWF_DATA_12
    {0xD10A, 0x214E,2},      // PGA_TABLE_CWF_DATA_13
    {0xD10C, 0xF150,2},      // PGA_TABLE_CWF_DATA_14
    {0xD10E, 0x0550,2},      // PGA_TABLE_CWF_DATA_15
    {0xD110, 0x164E,2},      // PGA_TABLE_CWF_DATA_16
    {0xD112, 0x2D32,2},      // PGA_TABLE_CWF_DATA_17
    {0xD114, 0x5DAD,2},      // PGA_TABLE_CWF_DATA_18
    {0xD116, 0x8D92,2},      // PGA_TABLE_CWF_DATA_19
    {0xD118, 0x268A,2},      // PGA_TABLE_CWF_DATA_20
    {0xD11A, 0xB62F,2},      // PGA_TABLE_CWF_DATA_21
    {0xD11C, 0xE56F,2},      // PGA_TABLE_CWF_DATA_22
    {0xD11E, 0x0E10,2},      // PGA_TABLE_CWF_DATA_23
    {0xD120, 0xED8D,2},      // PGA_TABLE_CWF_DATA_24
    {0xD122, 0x210C,2},      // PGA_TABLE_CWF_DATA_25
    {0xD124, 0x778F,2},      // PGA_TABLE_CWF_DATA_26
    {0xD126, 0x93AE,2},      // PGA_TABLE_CWF_DATA_27
    {0xD128, 0x92F1,2},      // PGA_TABLE_CWF_DATA_28
    {0xD12A, 0xB391,2},      // PGA_TABLE_CWF_DATA_29
    {0xD12C, 0xF04B,2},      // PGA_TABLE_CWF_DATA_30
    {0xD12E, 0x00F0,2},      // PGA_TABLE_CWF_DATA_31
    {0xD130, 0xD48D,2},      // PGA_TABLE_CWF_DATA_32
    {0xD132, 0xC111,2},      // PGA_TABLE_CWF_DATA_33
    {0xD134, 0x1D31,2},      // PGA_TABLE_CWF_DATA_34
    {0xD136, 0x978D,2},      // PGA_TABLE_CWF_DATA_35
    {0xD138, 0xAEEF,2},      // PGA_TABLE_CWF_DATA_36
    {0xD13A, 0xAEEF,2},      // PGA_TABLE_CWF_DATA_37
    {0xD13C, 0x5A8F,2},      // PGA_TABLE_CWF_DATA_38
    {0xD13E, 0x43D2,2},      // PGA_TABLE_CWF_DATA_39
    {0xD140, 0x2392,2},      // PGA_TABLE_CWF_DATA_40
    {0xD142, 0x002F,2},      // PGA_TABLE_CWF_DATA_41
    {0xD144, 0x2053,2},      // PGA_TABLE_CWF_DATA_42
    {0xD146, 0x2DB3,2},      // PGA_TABLE_CWF_DATA_43
    {0xD148, 0x8616,2},      // PGA_TABLE_CWF_DATA_44
    {0xD14A, 0x50B2,2},      // PGA_TABLE_CWF_DATA_45
    {0xD14C, 0x3271,2},      // PGA_TABLE_CWF_DATA_46
    {0xD14E, 0x1834,2},      // PGA_TABLE_CWF_DATA_47
    {0xD150, 0xE6D4,2},      // PGA_TABLE_CWF_DATA_48
    {0xD152, 0xE056,2},      // PGA_TABLE_CWF_DATA_49
    {0xD154, 0x34F1,2},      // PGA_TABLE_CWF_DATA_50
    {0xD156, 0x5BEE,2},      // PGA_TABLE_CWF_DATA_51
    {0xD158, 0x20F3,2},      // PGA_TABLE_CWF_DATA_52
    {0xD15A, 0x0C13,2},      // PGA_TABLE_CWF_DATA_53
    {0xD15C, 0xCF15,2},      // PGA_TABLE_CWF_DATA_54
    {0xD15E, 0x1452,2},      // PGA_TABLE_CWF_DATA_55
    {0xD160, 0x4B70,2},      // PGA_TABLE_CWF_DATA_56
    {0xD162, 0x01B3,2},      // PGA_TABLE_CWF_DATA_57
    {0xD164, 0xAB14,2},      // PGA_TABLE_CWF_DATA_58
    {0xD166, 0xE355,2},      // PGA_TABLE_CWF_DATA_59
    {0xD168, 0x0CF1,2},      // PGA_TABLE_CWF_DATA_60
    {0xD16A, 0x7A11,2},      // PGA_TABLE_CWF_DATA_61
    {0xD16C, 0x06CF,2},      // PGA_TABLE_CWF_DATA_62
    {0xD16E, 0x9E73,2},      // PGA_TABLE_CWF_DATA_63
    {0xD170, 0x6AD2,2},      // PGA_TABLE_CWF_DATA_64
    {0xD172, 0x2A71,2},      // PGA_TABLE_CWF_DATA_65
    {0xD174, 0x28F1,2},      // PGA_TABLE_CWF_DATA_66
    {0xD176, 0x7B11,2},      // PGA_TABLE_CWF_DATA_67
    {0xD178, 0xB054,2},      // PGA_TABLE_CWF_DATA_68
    {0xD17A, 0xA015,2},      // PGA_TABLE_CWF_DATA_69
    {0xD17C, 0xE06F,2},      // PGA_TABLE_CWF_DATA_70
    {0xD17E, 0x0810,2},      // PGA_TABLE_CWF_DATA_71
    {0xD180, 0x5E94,2},      // PGA_TABLE_CWF_DATA_72
    {0xD182, 0xFB91,2},      // PGA_TABLE_CWF_DATA_73
    {0xD184, 0x8256,2},      // PGA_TABLE_CWF_DATA_74
    {0xD186, 0x136F,2},      // PGA_TABLE_CWF_DATA_75
    {0xD188, 0x0312,2},      // PGA_TABLE_CWF_DATA_76
    {0xD18A, 0x0395,2},      // PGA_TABLE_CWF_DATA_77
    {0xD18C, 0x9B34,2},      // PGA_TABLE_CWF_DATA_78
    {0xD18E, 0xB316,2},      // PGA_TABLE_CWF_DATA_79
    {0xD190, 0x9E32,2},      // PGA_TABLE_CWF_DATA_80
    {0xD192, 0x1652,2},      // PGA_TABLE_CWF_DATA_81
    {0xD194, 0x9218,2},      // PGA_TABLE_CWF_DATA_82
    {0xD196, 0xC195,2},      // PGA_TABLE_CWF_DATA_83
    {0xD198, 0x6F99,2},      // PGA_TABLE_CWF_DATA_84
    {0xD19A, 0x8E52,2},      // PGA_TABLE_CWF_DATA_85
    {0xD19C, 0xF333,2},      // PGA_TABLE_CWF_DATA_86
    {0xD19E, 0xC078,2},      // PGA_TABLE_CWF_DATA_87
    {0xD1A0, 0x1C96,2},      // PGA_TABLE_CWF_DATA_88
    {0xD1A2, 0x209A,2},      // PGA_TABLE_CWF_DATA_89
    {0xD1A4, 0x1AB2,2},      // PGA_TABLE_CWF_DATA_90
    {0xD1A6, 0x6092,2},      // PGA_TABLE_CWF_DATA_91
    {0xD1A8, 0xAF17,2},      // PGA_TABLE_CWF_DATA_92
    {0xD1AA, 0x8B54,2},      // PGA_TABLE_CWF_DATA_93
    {0xD1AC, 0x4259,2},      // PGA_TABLE_CWF_DATA_94
    {0xD1AE, 0xE62F,2},      // PGA_TABLE_CWF_DATA_95
    {0xD1B0, 0xB9F3,2},      // PGA_TABLE_CWF_DATA_96
    {0xD1B2, 0xFED7,2},      // PGA_TABLE_CWF_DATA_97
    {0xD1B4, 0x5175,2},      // PGA_TABLE_CWF_DATA_98
    {0xD1B6, 0x42F9,2},      // PGA_TABLE_CWF_DATA_99
    
    {0xD1BA, 0x03F0,2},      // PGA_TABLE_CWF_CENTER_COLUMN
    {0xD1B8, 0x023C,2},      // PGA_TABLE_CWF_CENTER_ROW
    {0xC9FE, 0x0FA0,2},      // CAM_PGA_M_CONFIG_COLOUR_TEMP
    {0xCA00, 0x8000,2},      // CAM_PGA_M_CONFIG_GREEN_RED_Q14
    {0xCA02, 0x8000,2},      // CAM_PGA_M_CONFIG_RED_Q14
    {0xCA04, 0x8000,2},      // CAM_PGA_M_CONFIG_GREEN_BLUE_Q14
    {0xCA06, 0x8000,2},      // CAM_PGA_M_CONFIG_BLUE_Q14

    {0xD024, 0x0150,2},      // PGA_TABLE_D65_DATA_0
    {0xD026, 0xA02E,2},      // PGA_TABLE_D65_DATA_1
    {0xD028, 0x2352,2},      // PGA_TABLE_D65_DATA_2
    {0xD02A, 0x05D1,2},      // PGA_TABLE_D65_DATA_3
    {0xD02C, 0xAAF1,2},      // PGA_TABLE_D65_DATA_4
    {0xD02E, 0x0110,2},      // PGA_TABLE_D65_DATA_5
    {0xD030, 0x75ED,2},      // PGA_TABLE_D65_DATA_6
    {0xD032, 0x5B12,2},      // PGA_TABLE_D65_DATA_7
    {0xD034, 0x3191,2},      // PGA_TABLE_D65_DATA_8
    {0xD036, 0xDCB2,2},      // PGA_TABLE_D65_DATA_9
    {0xD038, 0x01D0,2},      // PGA_TABLE_D65_DATA_10
    {0xD03A, 0x81AE,2},      // PGA_TABLE_D65_DATA_11
    {0xD03C, 0x7E91,2},      // PGA_TABLE_D65_DATA_12
    {0xD03E, 0x40B0,2},      // PGA_TABLE_D65_DATA_13
    {0xD040, 0xDCF1,2},      // PGA_TABLE_D65_DATA_14
    {0xD042, 0x0310,2},      // PGA_TABLE_D65_DATA_15
    {0xD044, 0x760D,2},      // PGA_TABLE_D65_DATA_16
    {0xD046, 0x3712,2},      // PGA_TABLE_D65_DATA_17
    {0xD048, 0x26B0,2},      // PGA_TABLE_D65_DATA_18
    {0xD04A, 0xACD2,2},      // PGA_TABLE_D65_DATA_19
    {0xD04C, 0x96CB,2},      // PGA_TABLE_D65_DATA_20
    {0xD04E, 0xD28E,2},      // PGA_TABLE_D65_DATA_21
    {0xD050, 0xEDAC,2},      // PGA_TABLE_D65_DATA_22
    {0xD052, 0xDCEF,2},      // PGA_TABLE_D65_DATA_23
    {0xD054, 0xB871,2},      // PGA_TABLE_D65_DATA_24
    {0xD056, 0x9B0B,2},      // PGA_TABLE_D65_DATA_25
    {0xD058, 0x2B90,2},      // PGA_TABLE_D65_DATA_26
    {0xD05A, 0x6BAF,2},      // PGA_TABLE_D65_DATA_27
    {0xD05C, 0xE2D1,2},      // PGA_TABLE_D65_DATA_28
    {0xD05E, 0xECD1,2},      // PGA_TABLE_D65_DATA_29
    {0xD060, 0x9DAD,2},      // PGA_TABLE_D65_DATA_30
    {0xD062, 0x31B0,2},      // PGA_TABLE_D65_DATA_31
    {0xD064, 0x7B2F,2},      // PGA_TABLE_D65_DATA_32
    {0xD066, 0x9112,2},      // PGA_TABLE_D65_DATA_33
    {0xD068, 0x7E6E,2},      // PGA_TABLE_D65_DATA_34
    {0xD06A, 0xE1AD,2},      // PGA_TABLE_D65_DATA_35
    {0xD06C, 0xC98E,2},      // PGA_TABLE_D65_DATA_36
    {0xD06E, 0x0C8E,2},      // PGA_TABLE_D65_DATA_37
    {0xD070, 0x9D4F,2},      // PGA_TABLE_D65_DATA_38
    {0xD072, 0x58F1,2},      // PGA_TABLE_D65_DATA_39
    {0xD074, 0x11D2,2},      // PGA_TABLE_D65_DATA_40
    {0xD076, 0x4991,2},      // PGA_TABLE_D65_DATA_41
    {0xD078, 0x6253,2},      // PGA_TABLE_D65_DATA_42
    {0xD07A, 0x0D72,2},      // PGA_TABLE_D65_DATA_43
    {0xD07C, 0x9C96,2},      // PGA_TABLE_D65_DATA_44
    {0xD07E, 0x5412,2},      // PGA_TABLE_D65_DATA_45
    {0xD080, 0x3452,2},      // PGA_TABLE_D65_DATA_46
    {0xD082, 0x5BF4,2},      // PGA_TABLE_D65_DATA_47
    {0xD084, 0x8995,2},      // PGA_TABLE_D65_DATA_48
    {0xD086, 0x9517,2},      // PGA_TABLE_D65_DATA_49
    {0xD088, 0x5711,2},      // PGA_TABLE_D65_DATA_50
    {0xD08A, 0x44D1,2},      // PGA_TABLE_D65_DATA_51
    {0xD08C, 0x1A53,2},      // PGA_TABLE_D65_DATA_52
    {0xD08E, 0x304A,2},      // PGA_TABLE_D65_DATA_53
    {0xD090, 0xBBB5,2},      // PGA_TABLE_D65_DATA_54
    {0xD092, 0x0972,2},      // PGA_TABLE_D65_DATA_55
    {0xD094, 0x6891,2},      // PGA_TABLE_D65_DATA_56
    {0xD096, 0x5353,2},      // PGA_TABLE_D65_DATA_57
    {0xD098, 0xD9F4,2},      // PGA_TABLE_D65_DATA_58
    {0xD09A, 0x8E96,2},      // PGA_TABLE_D65_DATA_59
    {0xD09C, 0x3831,2},      // PGA_TABLE_D65_DATA_60
    {0xD09E, 0x4A50,2},      // PGA_TABLE_D65_DATA_61
    {0xD0A0, 0x5CD2,2},      // PGA_TABLE_D65_DATA_62
    {0xD0A2, 0x2413,2},      // PGA_TABLE_D65_DATA_63
    {0xD0A4, 0xF0F4,2},      // PGA_TABLE_D65_DATA_64
    {0xD0A6, 0x7D11,2},      // PGA_TABLE_D65_DATA_65
    {0xD0A8, 0x418E,2},      // PGA_TABLE_D65_DATA_66
    {0xD0AA, 0x2F54,2},      // PGA_TABLE_D65_DATA_67
    {0xD0AC, 0x79AF,2},      // PGA_TABLE_D65_DATA_68
    {0xD0AE, 0x9EF7,2},      // PGA_TABLE_D65_DATA_69
    {0xD0B0, 0x1BCC,2},      // PGA_TABLE_D65_DATA_70
    {0xD0B2, 0xA4F1,2},      // PGA_TABLE_D65_DATA_71
    {0xD0B4, 0x12D5,2},      // PGA_TABLE_D65_DATA_72
    {0xD0B6, 0x2254,2},      // PGA_TABLE_D65_DATA_73
    {0xD0B8, 0xE2B6,2},      // PGA_TABLE_D65_DATA_74
    {0xD0BA, 0x1FD0,2},      // PGA_TABLE_D65_DATA_75
    {0xD0BC, 0x386F,2},      // PGA_TABLE_D65_DATA_76
    {0xD0BE, 0x1955,2},      // PGA_TABLE_D65_DATA_77
    {0xD0C0, 0x64F1,2},      // PGA_TABLE_D65_DATA_78
    {0xD0C2, 0xD876,2},      // PGA_TABLE_D65_DATA_79
    {0xD0C4, 0x8D50,2},      // PGA_TABLE_D65_DATA_80
    {0xD0C6, 0xBB34,2},      // PGA_TABLE_D65_DATA_81
    {0xD0C8, 0xA8B8,2},      // PGA_TABLE_D65_DATA_82
    {0xD0CA, 0x4BD5,2},      // PGA_TABLE_D65_DATA_83
    {0xD0CC, 0x03DA,2},      // PGA_TABLE_D65_DATA_84
    {0xD0CE, 0x3230,2},      // PGA_TABLE_D65_DATA_85
    {0xD0D0, 0x92F5,2},      // PGA_TABLE_D65_DATA_86
    {0xD0D2, 0xE818,2},      // PGA_TABLE_D65_DATA_87
    {0xD0D4, 0x0277,2},      // PGA_TABLE_D65_DATA_88
    {0xD0D6, 0x487A,2},      // PGA_TABLE_D65_DATA_89
    {0xD0D8, 0x0213,2},      // PGA_TABLE_D65_DATA_90
    {0xD0DA, 0xA434,2},      // PGA_TABLE_D65_DATA_91
    {0xD0DC, 0xD457,2},      // PGA_TABLE_D65_DATA_92
    {0xD0DE, 0x0397,2},      // PGA_TABLE_D65_DATA_93
    {0xD0E0, 0x4639,2},      // PGA_TABLE_D65_DATA_94
    {0xD0E2, 0x3131,2},      // PGA_TABLE_D65_DATA_95
    {0xD0E4, 0x8195,2},      // PGA_TABLE_D65_DATA_96
    {0xD0E6, 0xA1B8,2},      // PGA_TABLE_D65_DATA_97
    {0xD0E8, 0x05D7,2},      // PGA_TABLE_D65_DATA_98
    {0xD0EA, 0x7CF9,2},      // PGA_TABLE_D65_DATA_99
    
    {0xD0EE, 0x03D0,2},      // PGA_TABLE_D65_CENTER_COLUMN
    {0xD0EC, 0x022C,2},      // PGA_TABLE_D65_CENTER_ROW
    {0xCA08, 0x1964,2},      // CAM_PGA_R_CONFIG_COLOUR_TEMP
    {0xCA0A, 0x8000,2},      // CAM_PGA_R_CONFIG_GREEN_RED_Q14
    {0xCA0C, 0x8000,2},      // CAM_PGA_R_CONFIG_RED_Q14
    {0xCA0E, 0x8000,2},      // CAM_PGA_R_CONFIG_GREEN_BLUE_Q14
    {0xCA10, 0x8000,2},      // CAM_PGA_R_CONFIG_BLUE_Q14
    {0xC9F2, 0x0011,2},      // CAM_PGA_PGA_CONTROL
    {0xD018, 0xFF,1},      // PGA_CURRENT_ZONE
    {0xD01E, 0x00,1},      // PGA_ZONE_LO_2
    {0xD01D, 0x64,1},      // PGA_ZONE_LO_1
    {0xD01C, 0xB4,1},      // PGA_ZONE_LO_0
    {0xD021, 0x6E,1},      // PGA_ZONE_HI_2
    {0xD020, 0xB9,1},      // PGA_ZONE_HI_1
    {0xD01F, 0xFF,1},      // PGA_ZONE_HI_0
    {0xD01B, 0x03,1},      // PGA_NUMBER_ZONES
    {0xC9F2, 0x0011,2},      // CAM_PGA_PGA_CONTROL

    {0x098E, 0x0000,2},      // LOGICAL_ADDRESS_ACCESS
    {0xC894, 0x00FE,2},      // CAM_AWB_CCM_L_0
    {0xC896, 0xFFA6,2},      // CAM_AWB_CCM_L_1
    {0xC898, 0x005D,2},      // CAM_AWB_CCM_L_2
    {0xC89A, 0x000A,2},      // CAM_AWB_CCM_L_3
    {0xC89C, 0x00B5,2},      // CAM_AWB_CCM_L_4
    {0xC89E, 0x0041,2},      // CAM_AWB_CCM_L_5
    {0xC8A0, 0xFFF3,2},      // CAM_AWB_CCM_L_6
    {0xC8A2, 0xFF9F,2},      // CAM_AWB_CCM_L_7
    {0xC8A4, 0x016E,2},      // CAM_AWB_CCM_L_8
    {0xC8CA, 0x0056,2},      // CAM_AWB_CCM_L_RG_GAIN
    {0xC8CC, 0x01A8,2},      // CAM_AWB_CCM_L_BG_GAIN
    {0xC8A6, 0x01C8,2},      // CAM_AWB_CCM_M_0
    {0xC8A8, 0xFF01,2},      // CAM_AWB_CCM_M_1
    {0xC8AA, 0x0036,2},      // CAM_AWB_CCM_M_2
    {0xC8AC, 0xFFFE,2},      // CAM_AWB_CCM_M_3
    {0xC8AE, 0x00F0,2},      // CAM_AWB_CCM_M_4
    {0xC8B0, 0x0012,2},      // CAM_AWB_CCM_M_5
    {0xC8B2, 0xFFD7,2},      // CAM_AWB_CCM_M_6
    {0xC8B4, 0xFF23,2},      // CAM_AWB_CCM_M_7
    {0xC8B6, 0x0206,2},      // CAM_AWB_CCM_M_8
    {0xC8CE, 0x0087,2},      // CAM_AWB_CCM_M_RG_GAIN
    {0xC8D0, 0x0157,2},      // CAM_AWB_CCM_M_BG_GAIN
    {0xC8B8, 0x0182,2},      // CAM_AWB_CCM_R_0
    {0xC8BA, 0xFF58,2},      // CAM_AWB_CCM_R_1
    {0xC8BC, 0x0027,2},      // CAM_AWB_CCM_R_2
    {0xC8BE, 0xFFF9,2},      // CAM_AWB_CCM_R_3
    {0xC8C0, 0x00D3,2},      // CAM_AWB_CCM_R_4
    {0xC8C2, 0x0034,2},      // CAM_AWB_CCM_R_5
    {0xC8C4, 0xFFF4,2},      // CAM_AWB_CCM_R_6
    {0xC8C6, 0xFF60,2},      // CAM_AWB_CCM_R_7
    {0xC8C8, 0x01AC,2},      // CAM_AWB_CCM_R_8
    {0xC8D2, 0x00BF,2},      // CAM_AWB_CCM_R_RG_GAIN
    {0xC8D4, 0x00CD,2},      // CAM_AWB_CCM_R_BG_GAIN
    {0xC8D6, 0x09C4,2},      // CAM_AWB_CCM_L_CTEMP
    {0xC8D8, 0x0D67,2},      // CAM_AWB_CCM_M_CTEMP
    {0xC8DA, 0x1964,2},      // CAM_AWB_CCM_R_CTEMP
    {0xAC0A, 0x00,1},      // AWB_R_SCENE_RATIO_LOWER
    {0xAC0B, 0xFF,1},      // AWB_R_SCENE_RATIO_UPPER
    {0xAC0C, 0x22,1},      // AWB_B_SCENE_RATIO_LOWER
    {0xAC0D, 0xFF,1},      // AWB_B_SCENE_RATIO_UPPER
    {0xC8EE, 0x09C4,2},      // CAM_AWB_COLOR_TEMPERATURE_MIN
    {0xC8F0, 0x1964,2},      // CAM_AWB_COLOR_TEMPERATURE_MAX
    {0xAC06, 0x63,1},      // AWB_R_RATIO_LOWER
    {0xAC07, 0x65,1},      // AWB_R_RATIO_UPPER
    {0xAC08, 0x63,1},      // AWB_B_RATIO_LOWER
    {0xAC09, 0x65,1},      // AWB_B_RATIO_UPPER
    {0xC894, 0x01D4,2},      // CAM_AWB_CCM_L_0
    {0xC896, 0xFF4E,2},      // CAM_AWB_CCM_L_1
    {0xC898, 0xFFDD,2},      // CAM_AWB_CCM_L_2
    {0xC89A, 0xFFA5,2},      // CAM_AWB_CCM_L_3
    {0xC89C, 0x017D,2},      // CAM_AWB_CCM_L_4
    {0xC89E, 0xFFDD,2},      // CAM_AWB_CCM_L_5
    {0xC8A0, 0xFFA5,2},      // CAM_AWB_CCM_L_6
    {0xC8A2, 0xFF4E,2},      // CAM_AWB_CCM_L_7
    {0xC8A4, 0x020C,2},      // CAM_AWB_CCM_L_8
    {0xC8A6, 0x0216,2},      // CAM_AWB_CCM_M_0
    {0xC8A8, 0xFF17,2},      // CAM_AWB_CCM_M_1
    {0xC8AA, 0xFFD3,2},      // CAM_AWB_CCM_M_2
    {0xC8AC, 0xFF88,2},      // CAM_AWB_CCM_M_3
    {0xC8AE, 0x01A6,2},      // CAM_AWB_CCM_M_4
    {0xC8B0, 0xFFD2,2},      // CAM_AWB_CCM_M_5
    {0xC8B2, 0xFFD2,2},      // CAM_AWB_CCM_M_6
    {0xC8B4, 0xFFA6,2},      // CAM_AWB_CCM_M_7
    {0xC8B6, 0x0188,2},      // CAM_AWB_CCM_M_8
    {0xC8B8, 0x0144,2},      // CAM_AWB_CCM_R_0
    {0xC8BA, 0xFFA6,2},      // CAM_AWB_CCM_R_1
    {0xC8BC, 0x0016,2},      // CAM_AWB_CCM_R_2
    {0xC8BE, 0xFFFC,2},      // CAM_AWB_CCM_R_3
    {0xC8C0, 0x0116,2},      // CAM_AWB_CCM_R_4
    {0xC8C2, 0xFFEE,2},      // CAM_AWB_CCM_R_5
    {0xC8C4, 0xFFD6,2},      // CAM_AWB_CCM_R_6
    {0xC8C6, 0xFFAD,2},      // CAM_AWB_CCM_R_7
    {0xC8C8, 0x017D,2},      // CAM_AWB_CCM_R_8
    {0xC8CA, 0x007D,2},      // CAM_AWB_CCM_L_RG_GAIN
    {0xC8CC, 0x019C,2},      // CAM_AWB_CCM_L_BG_GAIN
    {0xC8CE, 0x0087,2},      // CAM_AWB_CCM_M_RG_GAIN
    {0xC8D0, 0x0135,2},      // CAM_AWB_CCM_M_BG_GAIN
    {0xC8D2, 0x00BF,2},      // CAM_AWB_CCM_R_RG_GAIN
    {0xC8D4, 0x00DD,2},      // CAM_AWB_CCM_R_BG_GAIN
    {0xC8D6, 0x09C4,2},      // CAM_AWB_CCM_L_CTEMP
    {0xC8D8, 0x0D67,2},      // CAM_AWB_CCM_M_CTEMP
    {0xC8DA, 0x1964,2},      // CAM_AWB_CCM_R_CTEMP
    {0xC94C, 0x0000,2},      // CAM_STAT_AWB_CLIP_WINDOW_XSTART
    {0xC94E, 0x0000,2},      // CAM_STAT_AWB_CLIP_WINDOW_YSTART
    {0xC950, 0x077F,2},      // CAM_STAT_AWB_CLIP_WINDOW_XEND
    {0xC952, 0x0437,2},      // CAM_STAT_AWB_CLIP_WINDOW_YEND
    {0xC95C, 0x0034,2},      // CAM_STAT_AWB_X_SHIFT
    {0xC95E, 0x000F,2},      // CAM_STAT_AWB_Y_SHIFT
    {0xC8F4, 0x0080,2},      // CAM_AWB_AWB_XSCALE
    {0xC8F6, 0x00AB,2},      // CAM_AWB_AWB_YSCALE
    {0xC8F8, 0x0000,2},      // CAM_AWB_AWB_ROT_CENTER_X
    {0xC8FA, 0x03D9,2},      // CAM_AWB_AWB_ROT_CENTER_Y
    {0xC8FD, 0x00,1},      // CAM_AWB_AWB_ROT_ANGLE_SIN
    {0xC8FC, 0x00,1},      // CAM_AWB_AWB_ROT_ANGLE_COS
    {0xC900, 0x0000,2},      // CAM_AWB_AWB_WEIGHTS_0
    {0xC902, 0x0000,2},      // CAM_AWB_AWB_WEIGHTS_1
    {0xC904, 0x0000,2},      // CAM_AWB_AWB_WEIGHTS_2
    {0xC906, 0x0000,2},      // CAM_AWB_AWB_WEIGHTS_3
    {0xC908, 0x0000,2},      // CAM_AWB_AWB_WEIGHTS_4
    {0xC90A, 0x0111,2},      // CAM_AWB_AWB_WEIGHTS_5
    {0xC90C, 0x1110,2},      // CAM_AWB_AWB_WEIGHTS_6
    {0xC90E, 0x0000,2},      // CAM_AWB_AWB_WEIGHTS_7
    {0xC910, 0x0000,2},      // CAM_AWB_AWB_WEIGHTS_8
    {0xC912, 0x1122,2},      // CAM_AWB_AWB_WEIGHTS_9
    {0xC914, 0x2222,2},      // CAM_AWB_AWB_WEIGHTS_10
    {0xC916, 0x1100,2},      // CAM_AWB_AWB_WEIGHTS_11
    {0xC918, 0x0712,2},      // CAM_AWB_AWB_WEIGHTS_12
    {0xC91A, 0x2333,2},      // CAM_AWB_AWB_WEIGHTS_13
    {0xC91C, 0x3333,2},      // CAM_AWB_AWB_WEIGHTS_14
    {0xC91E, 0x3211,2},      // CAM_AWB_AWB_WEIGHTS_15
    {0xC920, 0x1234,2},      // CAM_AWB_AWB_WEIGHTS_16
    {0xC922, 0x4554,2},      // CAM_AWB_AWB_WEIGHTS_17
    {0xC924, 0x3334,2},      // CAM_AWB_AWB_WEIGHTS_18
    {0xC926, 0x4321,2},      // CAM_AWB_AWB_WEIGHTS_19
    {0xC928, 0x1345,2},      // CAM_AWB_AWB_WEIGHTS_20
    {0xC92A, 0x5554,2},      // CAM_AWB_AWB_WEIGHTS_21
    {0xC92C, 0x3223,2},      // CAM_AWB_AWB_WEIGHTS_22
    {0xC92E, 0x3332,2},      // CAM_AWB_AWB_WEIGHTS_23
    {0xC930, 0x1234,2},      // CAM_AWB_AWB_WEIGHTS_24
    {0xC932, 0x4443,2},      // CAM_AWB_AWB_WEIGHTS_25
    {0xC934, 0x2111,2},      // CAM_AWB_AWB_WEIGHTS_26
    {0xC936, 0x2221,2},      // CAM_AWB_AWB_WEIGHTS_27
    {0xC938, 0x0112,2},      // CAM_AWB_AWB_WEIGHTS_28
    {0xC93A, 0x2221,2},      // CAM_AWB_AWB_WEIGHTS_29
    {0xC93C, 0x1100,2},      // CAM_AWB_AWB_WEIGHTS_30
    {0xC93E, 0x0010,2},      // CAM_AWB_AWB_WEIGHTS_31
    {0x33F4, 0x0517,2},      // KERNEL_CONFIG
    {0xC81A, 0x003D,2},      // CAM_SENSOR_CFG_MIN_ANALOG_GAIN
    {0xC81C, 0x01F8,2},      // CAM_SENSOR_CFG_MAX_ANALOG_GAIN
    {0xC96C, 0x06,1},      // CAM_LL_START_DEMOSAIC
    {0xC971, 0x2B,1},      // CAM_LL_STOP_DEMOSAIC
    {0xC96E, 0x18,1},      // CAM_LL_START_AP_NOISE_GAIN
    {0xC96D, 0x07,1},      // CAM_LL_START_AP_NOISE_KNEE
    {0xC973, 0x53,1},      // CAM_LL_STOP_AP_NOISE_GAIN
    {0xC972, 0x1C,1},      // CAM_LL_STOP_AP_NOISE_KNEE
    {0xC96F, 0x0C,1},      // CAM_LL_START_AP_GAIN_POS
    {0xC970, 0x0A,1},      // CAM_LL_START_AP_GAIN_NEG
    {0xC974, 0x00,1},      // CAM_LL_STOP_AP_GAIN_POS
    {0xC975, 0x00,1},      // CAM_LL_STOP_AP_GAIN_NEG
    {0xC976, 0x0E,1},      // CAM_LL_START_GRB_APOS
    {0xC977, 0x07,1},      // CAM_LL_START_GRB_BPOS
    {0xC978, 0x0A,1},      // CAM_LL_START_GRB_ANEG
    {0xC979, 0x32,1},      // CAM_LL_START_GRB_BNEG
    {0xC97A, 0xE3,1},      // CAM_LL_STOP_GRB_APOS
    {0xC97C, 0x3B,1},      // CAM_LL_STOP_GRB_ANEG
    {0xC97B, 0x9D,1},      // CAM_LL_STOP_GRB_BPOS
    {0xC97D, 0x64,1},      // CAM_LL_STOP_GRB_BNEG
    {0x33F4, 0x0515,2},      // KERNEL_CONFIG
    {0xC9CE, 0x0040,2},      // CAM_LL_START_MAX_GAIN_METRIC
    {0xC9D0, 0x0400,2},      // CAM_LL_MID_MAX_GAIN_METRIC
    {0xC9D2, 0x1000,2},      // CAM_LL_STOP_MAX_GAIN_METRIC
    {0xC984, 0x0136,2},      // CAM_LL_START_CDC_HI_THR_COMB
    {0xC988, 0x00D2,2},      // CAM_LL_START_CDC_HI_THR_SATUR
    {0xC98C, 0x00FC,2},      // CAM_LL_MID_CDC_HI_THR_COMB
    {0xC990, 0x0132,2},      // CAM_LL_MID_CDC_HI_THR_SATUR
    {0xC994, 0x07B6,2},      // CAM_LL_STOP_CDC_HI_THR_COMB
    {0xC998, 0x035E,2},      // CAM_LL_STOP_CDC_HI_THR_SATUR
    {0xC986, 0x00A9,2},      // CAM_LL_START_CDC_LO_THR_COMB
    {0xC98A, 0x00A6,2},      // CAM_LL_START_CDC_LO_THR_SATUR
    {0xC98E, 0x0119,2},      // CAM_LL_MID_CDC_LO_THR_COMB
    {0xC992, 0x0134,2},      // CAM_LL_MID_CDC_LO_THR_SATUR
    {0xC996, 0x062E,2},      // CAM_LL_STOP_CDC_LO_THR_COMB
    {0xC99A, 0x026F,2},      // CAM_LL_STOP_CDC_LO_THR_SATUR
    {0xC97E, 0x000F,2},      // CAM_LL_START_DC_SINGLE_PIXEL_THR
    {0xC980, 0x0005,2},      // CAM_LL_STOP_DC_SINGLE_PIXEL_THR
    {0xC99C, 0x0017,2},      // CAM_LL_START_CDC_CC_NOISE_SLOPE
    {0xC99E, 0x0008,2},      // CAM_LL_START_CDC_CC_NOISE_KNEE
    {0xC9A0, 0x0026,2},      // CAM_LL_MID_CDC_CC_NOISE_SLOPE
    {0xC9A2, 0x003E,2},      // CAM_LL_MID_CDC_CC_NOISE_KNEE
    {0xC9A4, 0x0055,2},      // CAM_LL_STOP_CDC_CC_NOISE_SLOPE
    {0xC9A6, 0x00FF,2},      // CAM_LL_STOP_CDC_CC_NOISE_KNEE
    {0x33F4, 0x0D15,2},      // KERNEL_CONFIG
    {0xC9A8, 0x0040,2},      // CAM_LL_ADACD_LUT_GAIN_0
    {0xC9AE, 0x03,1},      // CAM_LL_ADACD_LUT_SIGMA_0
    {0xC9B2, 0x00A8,2},      // CAM_LL_ADACD_LUT_K_0
    {0xC9AA, 0x0140,2},      // CAM_LL_ADACD_LUT_GAIN_1
    {0xC9AF, 0x07,1},      // CAM_LL_ADACD_LUT_SIGMA_1
    {0xC9B4, 0x00A8,2},      // CAM_LL_ADACD_LUT_K_1
    {0xC9AC, 0x0200,2},      // CAM_LL_ADACD_LUT_GAIN_2
    {0xC9B0, 0x0F,1},      // CAM_LL_ADACD_LUT_SIGMA_2
    {0xC9B6, 0x0064,2},      // CAM_LL_ADACD_LUT_K_2
    {0xBC02, 0x0013,2},      // LL_MODE
    {0xC9BE, 0x00,1},      // CAM_LL_ADACD_LL_MODE_EN
    {0x3398, 0x0060,2},      // ADACD_LOWLIGHT_CONTROL
    {0x3398, 0x0260,2},      // ADACD_LOWLIGHT_CONTROL
    {0x3398, 0x0260,2},      // ADACD_LOWLIGHT_CONTROL
    {0xC9B1, 0x01,1},      // CAM_LL_ADACD_PATCH
    {0xC9B8, 0x021F,2},      // CAM_LL_ADACD_TRT
    {0x326E, 0x0086,2},      // LOW_PASS_YUV_FILTER
    {0x3270, 0x0FAA,2},      // THRESHOLD_FOR_Y_FILTER_R_CHANNEL
    {0x3272, 0x0FE4,2},      // THRESHOLD_FOR_Y_FILTER_G_CHANNEL
    {0xC9CA, 0x0040,2},      // CAM_LL_START_GAIN_METRIC
    {0xC9CC, 0x1000,2},      // CAM_LL_STOP_GAIN_METRIC
    {0xC9CE, 0x0040,2},      // CAM_LL_START_MAX_GAIN_METRIC
    {0xC9D2, 0x1000,2},      // CAM_LL_STOP_MAX_GAIN_METRIC
    {0xC944, 0x80,1},      // CAM_AWB_K_R_L
    {0xC945, 0x80,1},      // CAM_AWB_K_G_L
    {0xC946, 0x80,1},      // CAM_AWB_K_B_L
    {0xC947, 0x80,1},      // CAM_AWB_K_R_R
    {0xC948, 0x80,1},      // CAM_AWB_K_G_R
    {0xC949, 0x80,1},      // CAM_AWB_K_B_R
    {0xC962, 0x0032,2},      // CAM_LL_START_BRIGHTNESS
    {0xC964, 0x024D,2},      // CAM_LL_STOP_BRIGHTNESS
    {0xC88A, 0x0180,2},      // CAM_AET_AE_MAX_VIRT_DGAIN
    {0xC87E, 0x3D,1},      // CAM_AET_TARGET_AVERAGE_LUMA
    {0xC87F, 0x3D,1},      // CAM_AET_TARGET_AVERAGE_LUMA_DARK
    {0x32B2, 0x2314,2},      // DKDELTA_CCM_CTL
    {0xB402, 0x0002,2},      // CCM_MODE
    {0xC968, 0x80,1},      // CAM_LL_START_DESATURATION
    {0xC969, 0x50,1},      // CAM_LL_END_DESATURATION
    {0xC96A, 0x18,1},      // CAM_LL_START_DARK_DELTA_CCM_THR
    {0xC96B, 0x0F,1},      // CAM_LL_STOP_DARK_DELTA_CCM_THR
    {0xB42A, 0x05,1},      // CCM_DELTA_GAIN
    {0xC8DC, 0x0100,2},      // CAM_AWB_LL_CCM_0
    {0xC8DE, 0x0000,2},      // CAM_AWB_LL_CCM_1
    {0xC8E0, 0x0000,2},      // CAM_AWB_LL_CCM_2
    {0xC8E2, 0x0000,2},      // CAM_AWB_LL_CCM_3
    {0xC8E4, 0x0100,2},      // CAM_AWB_LL_CCM_4
    {0xC8E6, 0x0000,2},      // CAM_AWB_LL_CCM_5
    {0xC8E8, 0x0000,2},      // CAM_AWB_LL_CCM_6
    {0xC8EA, 0x0000,2},      // CAM_AWB_LL_CCM_7
    {0xC8EC, 0x0100,2},      // CAM_AWB_LL_CCM_8
    {0xC9EF, 0x26,1},      // RESERVED_CAM_1EF
    {0xC9D4, 0x0050,2},      // CAM_LL_START_FADE_TO_BLACK_LUMA
    {0xC9D6, 0x0030,2},      // CAM_LL_STOP_FADE_TO_BLACK_LUMA
{0xCA1C, 0x8043,2}, 		//cam_port_output_control = 32835
    {0x001E, 0x0777,2},      // PAD_SLEW
	{0xFFFF,INIT_DELAY,1}, //ADD
#if defined(FEATURE_TW_CAMERA_FPS28_MCLK24_PCLK92_MIPI1_H)
	{0xCA1C, 0x8041,2},
#else
    {0xCA1C, 0x8043,2},      // CAM_PORT_OUTPUT_CONTROL
#endif
	{0xC88A, 0x0100,2},      // CAM_AET_AE_MAX_VIRT_DGAIN
    {0xC882, 0x000A,2},      // CAM_AET_BLACK_CLIPPING_TARGET
	{0xFFFF,10,1},
    {0xDC00, 0x28,1},      // SYSMGR_NEXT_STATE
    {0x0080, 0x8002,2},      // COMMAND_REGISTER
/*  POLL  COMMAND_REGISTER::HOST_COMMAND_1 =>  0x00 */

//remark
    {0xBC02, 0x0013,2},      // LL_MODE
    {0xC960, 0x0003,2},      // CAM_LL_LLMODE
    {0xBC08, 0x00,1},      // LL_GAMMA_SELECT
    {0xC9C6, 0xC0,1},      // CAM_LL_START_CONTRAST_GRADIENT
    {0xC9C7, 0x60,1},      // CAM_LL_STOP_CONTRAST_GRADIENT
    {0xC9C8, 0x0F,1},      // CAM_LL_START_CONTRAST_LUMA_PERCENTAGE
    {0xC9C9, 0x2C,1},      // CAM_LL_STOP_CONTRAST_LUMA_PERCENTAGE
    {0xE400, 0x00,1},      // PATCHVARS_START_ORIGIN_GRADIENT
    {0xE401, 0x00,1},      // PATCHVARS_STOP_ORIGIN_GRADIENT
    {0xC9C0, 0x0012,2},      // CAM_LL_START_CONTRAST_BM
    {0xC9C2, 0x00B3,2},      // CAM_LL_STOP_CONTRAST_BM
    {0xffff, 200,2},
    //{0x3C00, 0x5003,2},  
    //{0x3C40, 0x7820,2},
    {0xffff, 200,2},
   //{0xc87c, 0x0024,2},
};

/* --------------------------------------------------------------------------*/
#if defined(FEATURE_TW_CAMERA_P_REG_WRITE)
struct as0260_reg as0260_init01_regs[] =
{
	{0x0982, 0x0001,2}, 		//ACCESS_CTL_STAT
	{0x098A, 0x6A44,2}, 		//PHYSICAL_ADDRESS_ACCESS
	{0xEA44, 0xC0F1,2},
	{0xEA46, 0x0A2E,2},
	{0xEA48, 0x0720,2},
	{0xEA4A, 0xD81D,2},
	{0xEA4C, 0x0876,2},
	{0xEA4E, 0x0740,2},
	{0xEA50, 0xE080,2},
	{0xEA52, 0x0066,2},
	{0xEA54, 0x0001,2},
	{0xEA56, 0x0ED2,2},
	{0xEA58, 0x07E0,2},
	{0xEA5A, 0xD802,2},
	{0xEA5C, 0xD900,2},
	{0xEA5E, 0x70CF,2},
	{0xEA60, 0xFF00,2},
	{0xEA62, 0x31B0,2},
	{0xEA64, 0xB038,2},
	{0xEA66, 0x1CFC,2},
	{0xEA68, 0xB388,2},
	{0xEA6A, 0x76CF,2},
	{0xEA6C, 0xFF00,2},
	{0xEA6E, 0x33CC,2},
	{0xEA70, 0x200A,2},
	{0xEA72, 0x0F80,2},
	{0xEA74, 0xFFFF,2},
	{0xEA76, 0xEA8C,2},
	{0xEA78, 0x1CFC,2},
	{0xEA7A, 0xB008,2},
	{0xEA7C, 0x2022,2},
	{0xEA7E, 0x0F80,2},
	{0xEA80, 0x0000,2},
	{0xEA82, 0xFC3C,2},
	{0xEA84, 0x2020,2},
	{0xEA86, 0x0F80,2},
	{0xEA88, 0x0000,2},
	{0xEA8A, 0xEA34,2},
	{0xEA8C, 0x1404,2},
	{0xEA8E, 0x340E,2},
	{0xEA90, 0x70CF,2},
	{0xEA92, 0xFF00,2},
	{0xEA94, 0x31B0,2},
	{0xEA96, 0xD901,2},
	{0xEA98, 0xB038,2},
	{0xEA9A, 0x70CF,2},
	{0xEA9C, 0xFF00,2},
	{0xEA9E, 0x0078,2},
	{0xEAA0, 0x9012,2},
	{0xEAA2, 0x0817,2},
	{0xEAA4, 0x035E,2},
	{0xEAA6, 0x09CE,2},
	{0xEAA8, 0x0720,2},
	{0xEAAA, 0xD83C,2},
	{0xEAAC, 0x0C16,2},
	{0xEAAE, 0x0840,2},
	{0xEAB0, 0x216F,2},
	{0xEAB2, 0x003F,2},
	{0xEAB4, 0xF1FC,2},
	{0xEAB6, 0x09BE,2},
	{0xEAB8, 0x0720,2},
	{0xEABA, 0xD81E,2},
	{0xEABC, 0xC0D1,2},
	{0xEABE, 0x7EE0,2},
	{0xEAC0, 0xC0F1,2},
	{0xEAC2, 0xE889,2},
	{0xEAC4, 0x70CF,2},
	{0xEAC6, 0xFF00,2},
	{0xEAC8, 0x0000,2},
	{0xEACA, 0x900E,2},
	{0xEACC, 0xB8E7,2},
	{0xEACE, 0x0F78,2},
	{0xEAD0, 0xFFC1,2},
	{0xEAD2, 0xD800,2},
	{0xEAD4, 0xF1F4,2},
	{0x098E, 0x0000,2}, 		//LOGICAL_ADDRESS_ACCESS
	{0x098A, 0x5F38,2}, 		//PHYSICAL_ADDRESS_ACCESS
	{0x0990, 0xFFFF,2},
	{0x0992, 0xEAC0,2},
	{0x001C, 0x0600,2}, 		//MCU_BOOT_MODE
	{0xFFFF, POLL_DELAY,2},

	//==> resolution Setting Start [
	{0x098E, 0xCA12,2},      // LOGICAL_ADDRESS_ACCESS [CAM_SYSCTL_PLL_ENABLE]
	{0xCA12, 0x01,1},      // CAM_SYSCTL_PLL_ENABLE
	{0xCA13, 0x00,1},      // CAM_SYSCTL_SENSOR_CLK_DIV2_EN
	{0xCA14, 0x025C,2}, 		//cam_sysctl_pll_divider_m_n = 604
	{0xCA16, 0x0070,2}, 		//cam_sysctl_pll_divider_p = 112
	{0xCA18, 0x7F7C,2}, 		//cam_sysctl_pll_divider_p4_p5_p6 = 32636
	{0xCA1C, 0x8043,2}, 		//cam_port_output_control = 32835

	//mcnex 121218
	//{0xCA1E, 0x0008,2}, 		//cam_port_porch = 8

	{0xCA20, 0x0C00,2}, 		//cam_port_mipi_timing_t_hs_zero = 3072
	{0xCA22, 0x0006,2}, 		//cam_port_mipi_timing_t_hs_exit_hs_trail = 6
	{0xCA24, 0x0B01,2}, 		//cam_port_mipi_timing_t_clk_post_clk_pre = 2817
	{0xCA26, 0x0517,2}, 		//cam_port_mipi_timing_t_clk_trail_clk_zero = 1303
	{0xCA28, 0x0005,2}, 		//cam_port_mipi_timing_t_lpx = 5
	{0xCA2A, 0x0809,2}, 		//cam_port_mipi_timing_init_timing = 2057
	{0xCA2C, 0x0000,2}, 		//cam_port_mipi_timing_t_hs_pre = 0

	//[Timing_settings]
	{0xC800, 0x0020,2}, 		//cam_sensor_cfg_y_addr_start = 32
	{0xC802, 0x0020,2}, 		//cam_sensor_cfg_x_addr_start = 32
	{0xC804, 0x045F,2}, 		//cam_sensor_cfg_y_addr_end = 1119
	{0xC806, 0x07A7,2}, 		//cam_sensor_cfg_x_addr_end = 1959
	{0xC808, 0x035FE1D8,4}, 		//cam_sensor_cfg_pixclk = 56615384
	{0xC80C, 0x0001,2}, 		//cam_sensor_cfg_row_speed = 1
	{0xC80E, 0x0336,2}, 		//cam_sensor_cfg_fine_integ_time_min = 822
	{0xC810, 0x0CCF,2}, 		//cam_sensor_cfg_fine_integ_time_max = 3279
	{0xC812, 0x0491,2}, 		//cam_sensor_cfg_frame_length_lines = 1169
	{0xC814, 0x0E8D,2}, 		//cam_sensor_cfg_line_length_pck = 3725
	{0xC816, 0x00D4,2}, 		//cam_sensor_cfg_fine_correction = 212
	{0xC818, 0x043B,2}, 		//cam_sensor_cfg_cpipe_last_row = 1083
	{0xC820, 0x0010,2}, 		//cam_sensor_cfg_reg_0_data = 16
	{0xC830, 0x0002,2}, 		//cam_sensor_control_read_mode = 2
	{0xC858, 0x0000,2}, 		//cam_crop_window_xoffset = 0
	{0xC85A, 0x0000,2}, 		//cam_crop_window_yoffset = 0
	{0xC85C, 0x0780,2}, 		//cam_crop_window_width = 1920
	{0xC85E, 0x0438,2}, 		//cam_crop_window_height = 1080
	{0xC86C, 0x0780,2}, 		//cam_output_width = 1920
	{0xC86E, 0x0438,2}, 		//cam_output_height = 1080
	{0xC870, 0x4010,2}, 		//cam_output_format = 16592
	{0xC87C, 0x00,1}, 			//cam_aet_aemode = 0
	{0xC88E, 0x1A00,2}, 		//cam_aet_max_frame_rate = 6656
	{0xC890, 0x1A00,2}, 		//cam_aet_min_frame_rate = 6656
	{0xC94C, 0x0000,2}, 		//cam_stat_awb_clip_window_xstart = 0
	{0xC94E, 0x0000,2}, 		//cam_stat_awb_clip_window_ystart = 0
	{0xC950, 0x077F,2}, 		//cam_stat_awb_clip_window_xend = 1919
	{0xC952, 0x0437,2}, 		//cam_stat_awb_clip_window_yend = 1079
	{0xC954, 0x0000,2}, 		//cam_stat_ae_initial_window_xstart = 0
	{0xC956, 0x0000,2}, 		//cam_stat_ae_initial_window_ystart = 0
	{0xC958, 0x017F,2}, 		//cam_stat_ae_initial_window_xend = 383
	{0xC95A, 0x00D7,2}, 		//cam_stat_ae_initial_window_yend = 215
	//==> resolution Setting End ]

	{0x3E00, 0x042D,2}, 		//DYNAMIC_SEQRAM_00
	{0x3E02, 0x39FF,2}, 		//DYNAMIC_SEQRAM_02
	{0x3E04, 0x49FF,2}, 		//DYNAMIC_SEQRAM_04
	{0x3E06, 0xFFFF,2}, 		//DYNAMIC_SEQRAM_06
	{0x3E08, 0x8071,2}, 		//DYNAMIC_SEQRAM_08
	{0x3E0A, 0x7211,2}, 		//DYNAMIC_SEQRAM_0A
	{0x3E0C, 0xE040,2}, 		//DYNAMIC_SEQRAM_0C
	{0x3E0E, 0xA840,2}, 		//DYNAMIC_SEQRAM_0E
	{0x3E10, 0x4100,2}, 		//DYNAMIC_SEQRAM_10
	{0x3E12, 0x1846,2}, 		//DYNAMIC_SEQRAM_12
	{0x3E14, 0xA547,2}, 		//DYNAMIC_SEQRAM_14
	{0x3E16, 0xAD57,2}, 		//DYNAMIC_SEQRAM_16
	{0x3E18, 0x8149,2}, 		//DYNAMIC_SEQRAM_18
	{0x3E1A, 0x9D49,2}, 		//DYNAMIC_SEQRAM_1A
	{0x3E1C, 0x9F46,2}, 		//DYNAMIC_SEQRAM_1C
	{0x3E1E, 0x8000,2}, 		//DYNAMIC_SEQRAM_1E
	{0x3E20, 0x1842,2}, 		//DYNAMIC_SEQRAM_20
	{0x3E22, 0x4180,2}, 		//DYNAMIC_SEQRAM_22
	{0x3E24, 0x0018,2}, 		//DYNAMIC_SEQRAM_24
	{0x3E26, 0x8149,2}, 		//DYNAMIC_SEQRAM_26
	{0x3E28, 0x9C49,2}, 		//DYNAMIC_SEQRAM_28
	{0x3E2A, 0x9347,2}, 		//DYNAMIC_SEQRAM_2A
	{0x3E2C, 0x804D,2}, 		//DYNAMIC_SEQRAM_2C
	{0x3E2E, 0x804A,2}, 		//DYNAMIC_SEQRAM_2E
	{0x3E30, 0x100C,2}, 		//DYNAMIC_SEQRAM_30
	{0x3E32, 0x8000,2}, 		//DYNAMIC_SEQRAM_32
	{0x3E34, 0x1841,2}, 		//DYNAMIC_SEQRAM_34
	{0x3E36, 0x4280,2}, 		//DYNAMIC_SEQRAM_36
	{0x3E38, 0x0018,2}, 		//DYNAMIC_SEQRAM_38
	{0x3E3A, 0x9710,2}, 		//DYNAMIC_SEQRAM_3A
	{0x3E3C, 0x0C80,2}, 		//DYNAMIC_SEQRAM_3C
	{0x3E3E, 0x4DA2,2}, 		//DYNAMIC_SEQRAM_3E
	{0x3E40, 0x4BA0,2}, 		//DYNAMIC_SEQRAM_40
	{0x3E42, 0x4A00,2}, 		//DYNAMIC_SEQRAM_42
	{0x3E44, 0x1880,2}, 		//DYNAMIC_SEQRAM_44
	{0x3E46, 0x4241,2}, 		//DYNAMIC_SEQRAM_46
	{0x3E48, 0x0018,2}, 		//DYNAMIC_SEQRAM_48
	{0x3E4A, 0xB54B,2}, 		//DYNAMIC_SEQRAM_4A
	{0x3E4C, 0x1C00,2}, 		//DYNAMIC_SEQRAM_4C
	{0x3E4E, 0x8000,2}, 		//DYNAMIC_SEQRAM_4E
	{0x3E50, 0x1C10,2}, 		//DYNAMIC_SEQRAM_50
	{0x3E52, 0x6081,2}, 		//DYNAMIC_SEQRAM_52
	{0x3E54, 0x1580,2}, 		//DYNAMIC_SEQRAM_54
	{0x3E56, 0x7C09,2}, 		//DYNAMIC_SEQRAM_56
	{0x3E58, 0x7000,2}, 		//DYNAMIC_SEQRAM_58
	{0x3E5A, 0x8082,2}, 		//DYNAMIC_SEQRAM_5A
	{0x3E5C, 0x7281,2}, 		//DYNAMIC_SEQRAM_5C
	{0x3E5E, 0x4C40,2}, 		//DYNAMIC_SEQRAM_5E
	{0x3E60, 0x8E4D,2}, 		//DYNAMIC_SEQRAM_60
	{0x3E62, 0x8110,2}, 		//DYNAMIC_SEQRAM_62
	{0x3E64, 0x0CAF,2}, 		//DYNAMIC_SEQRAM_64
	{0x3E66, 0x4D80,2}, 		//DYNAMIC_SEQRAM_66
	{0x3E68, 0x100C,2}, 		//DYNAMIC_SEQRAM_68
	{0x3E6A, 0x8440,2}, 		//DYNAMIC_SEQRAM_6A
	{0x3E6C, 0x4C81,2}, 		//DYNAMIC_SEQRAM_6C
	{0x3E6E, 0x7C5B,2}, 		//DYNAMIC_SEQRAM_6E
	{0x3E70, 0x7000,2}, 		//DYNAMIC_SEQRAM_70
	{0x3E72, 0x8054,2}, 		//DYNAMIC_SEQRAM_72
	{0x3E74, 0x924C,2}, 		//DYNAMIC_SEQRAM_74
	{0x3E76, 0x4078,2}, 		//DYNAMIC_SEQRAM_76
	{0x3E78, 0x4D4F,2}, 		//DYNAMIC_SEQRAM_78
	{0x3E7A, 0x4E98,2}, 		//DYNAMIC_SEQRAM_7A
	{0x3E7C, 0x504E,2}, 		//DYNAMIC_SEQRAM_7C
	{0x3E7E, 0x4F97,2}, 		//DYNAMIC_SEQRAM_7E
	{0x3E80, 0x4F4E,2}, 		//DYNAMIC_SEQRAM_80
	{0x3E82, 0x507C,2}, 		//DYNAMIC_SEQRAM_82
	{0x3E84, 0x7B8D,2}, 		//DYNAMIC_SEQRAM_84
	{0x3E86, 0x4D88,2}, 		//DYNAMIC_SEQRAM_86
	{0x3E88, 0x4E10,2}, 		//DYNAMIC_SEQRAM_88
	{0x3E8A, 0x0940,2}, 		//DYNAMIC_SEQRAM_8A
	{0x3E8C, 0x8879,2}, 		//DYNAMIC_SEQRAM_8C
	{0x3E8E, 0x5481,2}, 		//DYNAMIC_SEQRAM_8E
	{0x3E90, 0x7000,2}, 		//DYNAMIC_SEQRAM_90
	{0x3E92, 0x8082,2}, 		//DYNAMIC_SEQRAM_92
	{0x3E94, 0x7281,2}, 		//DYNAMIC_SEQRAM_94
	{0x3E96, 0x4C40,2}, 		//DYNAMIC_SEQRAM_96
	{0x3E98, 0x8E4D,2}, 		//DYNAMIC_SEQRAM_98
	{0x3E9A, 0x8110,2}, 		//DYNAMIC_SEQRAM_9A
	{0x3E9C, 0x0CAF,2}, 		//DYNAMIC_SEQRAM_9C
	{0x3E9E, 0x4D80,2}, 		//DYNAMIC_SEQRAM_9E
	{0x3EA0, 0x100C,2}, 		//DYNAMIC_SEQRAM_A0
	{0x3EA2, 0x8440,2}, 		//DYNAMIC_SEQRAM_A2
	{0x3EA4, 0x4C81,2}, 		//DYNAMIC_SEQRAM_A4
	{0x3EA6, 0x7C93,2}, 		//DYNAMIC_SEQRAM_A6
	{0x3EA8, 0x7000,2}, 		//DYNAMIC_SEQRAM_A8
	{0x3EAA, 0x0000,2}, 		//DYNAMIC_SEQRAM_AA
	{0x3EAC, 0x0000,2}, 		//DYNAMIC_SEQRAM_AC
	{0x3EAE, 0x0000,2}, 		//DYNAMIC_SEQRAM_AE
	{0x3EB0, 0x0000,2}, 		//DYNAMIC_SEQRAM_B0
	{0x3EB2, 0x0000,2}, 		//DYNAMIC_SEQRAM_B2
	{0x3EB4, 0x0000,2}, 		//DYNAMIC_SEQRAM_B4
	{0x3EB6, 0x0000,2}, 		//DYNAMIC_SEQRAM_B6
	{0x3EB8, 0x0000,2}, 		//DYNAMIC_SEQRAM_B8
	{0x3EBA, 0x0000,2}, 		//DYNAMIC_SEQRAM_BA
	{0x3EBC, 0x0000,2}, 		//DYNAMIC_SEQRAM_BC
	{0x3EBE, 0x0000,2}, 		//DYNAMIC_SEQRAM_BE
	{0x3EC0, 0x0000,2}, 		//DYNAMIC_SEQRAM_C0
	{0x3EC2, 0x0000,2}, 		//DYNAMIC_SEQRAM_C2
	{0x3EC4, 0x0000,2}, 		//DYNAMIC_SEQRAM_C4
	{0x3EC6, 0x0000,2}, 		//DYNAMIC_SEQRAM_C6
	{0x3EC8, 0x0000,2}, 		//DYNAMIC_SEQRAM_C8
	{0x3ECA, 0x0000,2}, 		//DYNAMIC_SEQRAM_CA

	{0x30B2, 0xC000,2}, 		//CALIB_TIED_OFFSET
	{0x30D4, 0x9400,2}, 		//COLUMN_CORRECTION
	{0x31C0, 0x0000,2}, 		//FUSE_REF_ADDR
	{0x316A, 0x8200,2}, 		//DAC_FBIAS
	{0x316C, 0x8200,2}, 		//DAC_TXLO
	{0x3EFE, 0x2808,2}, 		//DAC_LD_TXLO
	{0x3EFC, 0x2868,2}, 		//DAC_LD_FBIAS
	{0x3ED2, 0xD165,2}, 		//DAC_LD_6_7
	{0x3EF2, 0xD165,2}, 		//DAC_LP_6_7
	{0x3ED8, 0x7F1A,2}, 		//DAC_LD_12_13
	{0x3EDA, 0x2828,2}, 		//DAC_LD_14_15
	{0x3EE2, 0x0058,2}, 		//DAC_LD_22_23
	{0x3EFE, 0x280A,2}, 		//DAC_LD_TXLO
	{0x3170, 0x000A,2}, 		//ANALOG_CONTROL
	{0x3174, 0x8060,2}, 		//ANALOG_CONTROL3
	{0x317A, 0x000A,2}, 		//ANALOG_CONTROL6
	{0x3ECC, 0x22B0,2}, 		//DAC_LD_0_1

	//[AutoPGA Settings 80% 2012/11/26 02:51:35]
	{0x098E, 0x0000,2}, 		// LOGICAL addressing
	{0xD1BC, 0x0170,2}, 		// PGA_TABLE_A_DATA_0
	{0xD1BE, 0xA32D,2}, 		// PGA_TABLE_A_DATA_1
	{0xD1C0, 0x6F51,2}, 		// PGA_TABLE_A_DATA_2
	{0xD1C2, 0x5F4C,2}, 		// PGA_TABLE_A_DATA_3
	{0xD1C4, 0xF091,2}, 		// PGA_TABLE_A_DATA_4
	{0xD1C6, 0x0150,2}, 		// PGA_TABLE_A_DATA_5
	{0xD1C8, 0x392E,2}, 		// PGA_TABLE_A_DATA_6
	{0xD1CA, 0x7AB2,2}, 		// PGA_TABLE_A_DATA_7
	{0xD1CC, 0x71AE,2}, 		// PGA_TABLE_A_DATA_8
	{0xD1CE, 0xBB93,2}, 		// PGA_TABLE_A_DATA_9
	{0xD1D0, 0x01F0,2}, 		// PGA_TABLE_A_DATA_10
	{0xD1D2, 0xCCEC,2}, 		// PGA_TABLE_A_DATA_11
	{0xD1D4, 0x680F,2}, 		// PGA_TABLE_A_DATA_12
	{0xD1D6, 0xB0AF,2}, 		// PGA_TABLE_A_DATA_13
	{0xD1D8, 0x8ECD,2}, 		// PGA_TABLE_A_DATA_14
	{0xD1DA, 0x1010,2}, 		// PGA_TABLE_A_DATA_15
	{0xD1DC, 0x67CD,2}, 		// PGA_TABLE_A_DATA_16
	{0xD1DE, 0x1BD2,2}, 		// PGA_TABLE_A_DATA_17
	{0xD1E0, 0x9C0E,2}, 		// PGA_TABLE_A_DATA_18
	{0xD1E2, 0xFD92,2}, 		// PGA_TABLE_A_DATA_19
	{0xD1E4, 0x552C,2}, 		// PGA_TABLE_A_DATA_20
	{0xD1E6, 0xDECE,2}, 		// PGA_TABLE_A_DATA_21
	{0xD1E8, 0xFE2F,2}, 		// PGA_TABLE_A_DATA_22
	{0xD1EA, 0x9E8D,2}, 		// PGA_TABLE_A_DATA_23
	{0xD1EC, 0x0E32,2}, 		// PGA_TABLE_A_DATA_24
	{0xD1EE, 0x01AE,2}, 		// PGA_TABLE_A_DATA_25
	{0xD1F0, 0x1D90,2}, 		// PGA_TABLE_A_DATA_26
	{0xD1F2, 0x85EE,2}, 		// PGA_TABLE_A_DATA_27
	{0xD1F4, 0xF7F1,2}, 		// PGA_TABLE_A_DATA_28
	{0xD1F6, 0x8AB0,2}, 		// PGA_TABLE_A_DATA_29
	{0xD1F8, 0xE9A8,2}, 		// PGA_TABLE_A_DATA_30
	{0xD1FA, 0x1FB0,2}, 		// PGA_TABLE_A_DATA_31
	{0xD1FC, 0x8790,2}, 		// PGA_TABLE_A_DATA_32
	{0xD1FE, 0x8992,2}, 		// PGA_TABLE_A_DATA_33
	{0xD200, 0x7652,2}, 		// PGA_TABLE_A_DATA_34
	{0xD202, 0xD9CB,2}, 		// PGA_TABLE_A_DATA_35
	{0xD204, 0x802F,2}, 		// PGA_TABLE_A_DATA_36
	{0xD206, 0x0CEF,2}, 		// PGA_TABLE_A_DATA_37
	{0xD208, 0x1A0F,2}, 		// PGA_TABLE_A_DATA_38
	{0xD20A, 0x6731,2}, 		// PGA_TABLE_A_DATA_39
	{0xD20C, 0x1C12,2}, 		// PGA_TABLE_A_DATA_40
	{0xD20E, 0x0131,2}, 		// PGA_TABLE_A_DATA_41
	{0xD210, 0xE553,2}, 		// PGA_TABLE_A_DATA_42
	{0xD212, 0xE3F3,2}, 		// PGA_TABLE_A_DATA_43
	{0xD214, 0x33B5,2}, 		// PGA_TABLE_A_DATA_44
	{0xD216, 0x1353,2}, 		// PGA_TABLE_A_DATA_45
	{0xD218, 0x4EB1,2}, 		// PGA_TABLE_A_DATA_46
	{0xD21A, 0xD213,2}, 		// PGA_TABLE_A_DATA_47
	{0xD21C, 0xC215,2}, 		// PGA_TABLE_A_DATA_48
	{0xD21E, 0x9FB3,2}, 		// PGA_TABLE_A_DATA_49
	{0xD220, 0x0FD1,2}, 		// PGA_TABLE_A_DATA_50
	{0xD222, 0x63B0,2}, 		// PGA_TABLE_A_DATA_51
	{0xD224, 0xD173,2}, 		// PGA_TABLE_A_DATA_52
	{0xD226, 0xB413,2}, 		// PGA_TABLE_A_DATA_53
	{0xD228, 0x20B6,2}, 		// PGA_TABLE_A_DATA_54
	{0xD22A, 0x0A12,2}, 		// PGA_TABLE_A_DATA_55
	{0xD22C, 0x69CF,2}, 		// PGA_TABLE_A_DATA_56
	{0xD22E, 0xAC34,2}, 		// PGA_TABLE_A_DATA_57
	{0xD230, 0xECB4,2}, 		// PGA_TABLE_A_DATA_58
	{0xD232, 0x2156,2}, 		// PGA_TABLE_A_DATA_59
	{0xD234, 0x7B90,2}, 		// PGA_TABLE_A_DATA_60
	{0xD236, 0x02D0,2}, 		// PGA_TABLE_A_DATA_61
	{0xD238, 0xA611,2}, 		// PGA_TABLE_A_DATA_62
	{0xD23A, 0x0914,2}, 		// PGA_TABLE_A_DATA_63
	{0xD23C, 0x4AF5,2}, 		// PGA_TABLE_A_DATA_64
	{0xD23E, 0x5971,2}, 		// PGA_TABLE_A_DATA_65
	{0xD240, 0x2670,2}, 		// PGA_TABLE_A_DATA_66
	{0xD242, 0x9274,2}, 		// PGA_TABLE_A_DATA_67
	{0xD244, 0x91B1,2}, 		// PGA_TABLE_A_DATA_68
	{0xD246, 0x48B6,2}, 		// PGA_TABLE_A_DATA_69
	{0xD248, 0x6F8F,2}, 		// PGA_TABLE_A_DATA_70
	{0xD24A, 0xDB31,2}, 		// PGA_TABLE_A_DATA_71
	{0xD24C, 0x7C33,2}, 		// PGA_TABLE_A_DATA_72
	{0xD24E, 0x49B4,2}, 		// PGA_TABLE_A_DATA_73
	{0xD250, 0xAAB4,2}, 		// PGA_TABLE_A_DATA_74
	{0xD252, 0x1950,2}, 		// PGA_TABLE_A_DATA_75
	{0xD254, 0x2831,2}, 		// PGA_TABLE_A_DATA_76
	{0xD256, 0x2293,2}, 		// PGA_TABLE_A_DATA_77
	{0xD258, 0x34B2,2}, 		// PGA_TABLE_A_DATA_78
	{0xD25A, 0x1454,2}, 		// PGA_TABLE_A_DATA_79
	{0xD25C, 0xC673,2}, 		// PGA_TABLE_A_DATA_80
	{0xD25E, 0x9534,2}, 		// PGA_TABLE_A_DATA_81
	{0xD260, 0xFAD6,2}, 		// PGA_TABLE_A_DATA_82
	{0xD262, 0x78F7,2}, 		// PGA_TABLE_A_DATA_83
	{0xD264, 0x7DF8,2}, 		// PGA_TABLE_A_DATA_84
	{0xD266, 0xEE33,2}, 		// PGA_TABLE_A_DATA_85
	{0xD268, 0xD674,2}, 		// PGA_TABLE_A_DATA_86
	{0xD26A, 0xB478,2}, 		// PGA_TABLE_A_DATA_87
	{0xD26C, 0x3178,2}, 		// PGA_TABLE_A_DATA_88
	{0xD26E, 0x25FA,2}, 		// PGA_TABLE_A_DATA_89
	{0xD270, 0x9DB3,2}, 		// PGA_TABLE_A_DATA_90
	{0xD272, 0x8293,2}, 		// PGA_TABLE_A_DATA_91
	{0xD274, 0x5835,2}, 		// PGA_TABLE_A_DATA_92
	{0xD276, 0x4B37,2}, 		// PGA_TABLE_A_DATA_93
	{0xD278, 0x8B78,2}, 		// PGA_TABLE_A_DATA_94
	{0xD27A, 0x8D93,2}, 		// PGA_TABLE_A_DATA_95
	{0xD27C, 0xC5D2,2}, 		// PGA_TABLE_A_DATA_96
	{0xD27E, 0xCA16,2}, 		// PGA_TABLE_A_DATA_97
	{0xD280, 0x6B17,2}, 		// PGA_TABLE_A_DATA_98
	{0xD282, 0x22D8,2}, 		// PGA_TABLE_A_DATA_99
	{0xD286, 0x0400,2}, 		// PGA_TABLE_A_CENTER_COLUMN
	{0xD284, 0x024C,2}, 		// PGA_TABLE_A_CENTER_ROW
	{0xC9F4, 0x0AF0,2}, 		// CAM_PGA_L_CONFIG_COLOUR_TEMP
	{0xC9F6, 0x8000,2}, 		// CAM_PGA_L_CONFIG_GREEN_RED_Q14
	{0xC9F8, 0x8000,2}, 		// CAM_PGA_L_CONFIG_RED_Q14
	{0xC9FA, 0x8000,2}, 		// CAM_PGA_L_CONFIG_GREEN_BLUE_Q14
	{0xC9FC, 0x8000,2}, 		// CAM_PGA_L_CONFIG_BLUE_Q14
	{0xD0F0, 0x00F0,2}, 		// PGA_TABLE_CWF_DATA_0
	{0xD0F2, 0xA48D,2}, 		// PGA_TABLE_CWF_DATA_1
	{0xD0F4, 0x0652,2}, 		// PGA_TABLE_CWF_DATA_2
	{0xD0F6, 0xB0AC,2}, 		// PGA_TABLE_CWF_DATA_3
	{0xD0F8, 0xA512,2}, 		// PGA_TABLE_CWF_DATA_4
	{0xD0FA, 0x01F0,2}, 		// PGA_TABLE_CWF_DATA_5
	{0xD0FC, 0x28CE,2}, 		// PGA_TABLE_CWF_DATA_6
	{0xD0FE, 0x4072,2}, 		// PGA_TABLE_CWF_DATA_7
	{0xD100, 0x2C2E,2}, 		// PGA_TABLE_CWF_DATA_8
	{0xD102, 0xA273,2}, 		// PGA_TABLE_CWF_DATA_9
	{0xD104, 0x0130,2}, 		// PGA_TABLE_CWF_DATA_10
	{0xD106, 0xB4ED,2}, 		// PGA_TABLE_CWF_DATA_11
	{0xD108, 0x0911,2}, 		// PGA_TABLE_CWF_DATA_12
	{0xD10A, 0x8CEF,2}, 		// PGA_TABLE_CWF_DATA_13
	{0xD10C, 0x9E11,2}, 		// PGA_TABLE_CWF_DATA_14
	{0xD10E, 0x0C50,2}, 		// PGA_TABLE_CWF_DATA_15
	{0xD110, 0x500D,2}, 		// PGA_TABLE_CWF_DATA_16
	{0xD112, 0x1472,2}, 		// PGA_TABLE_CWF_DATA_17
	{0xD114, 0xCFCE,2}, 		// PGA_TABLE_CWF_DATA_18
	{0xD116, 0xFE72,2}, 		// PGA_TABLE_CWF_DATA_19
	{0xD118, 0x396B,2}, 		// PGA_TABLE_CWF_DATA_20
	{0xD11A, 0x988F,2}, 		// PGA_TABLE_CWF_DATA_21
	{0xD11C, 0x88F0,2}, 		// PGA_TABLE_CWF_DATA_22
	{0xD11E, 0x6ECE,2}, 		// PGA_TABLE_CWF_DATA_23
	{0xD120, 0x1B92,2}, 		//  PGA_TABLE_CWF_DATA_24
	{0xD122, 0x13AD,2}, 		//  PGA_TABLE_CWF_DATA_25
	{0xD124, 0x734F,2}, 		//  PGA_TABLE_CWF_DATA_26
	{0xD126, 0xAC0E,2}, 		//  PGA_TABLE_CWF_DATA_27
	{0xD128, 0xB391,2}, 		//  PGA_TABLE_CWF_DATA_28
	{0xD12A, 0x560E,2}, 		//  PGA_TABLE_CWF_DATA_29
	{0xD12C, 0x8D0A,2}, 		//  PGA_TABLE_CWF_DATA_30
	{0xD12E, 0x556F,2}, 		//  PGA_TABLE_CWF_DATA_31
	{0xD130, 0xFD0B,2}, 		//  PGA_TABLE_CWF_DATA_32
	{0xD132, 0xA2F1,2}, 		//  PGA_TABLE_CWF_DATA_33
	{0xD134, 0x2CF1,2}, 		//  PGA_TABLE_CWF_DATA_34
	{0xD136, 0xA52C,2}, 		//  PGA_TABLE_CWF_DATA_35
	{0xD138, 0xAD2F,2}, 		//  PGA_TABLE_CWF_DATA_36
	{0xD13A, 0x3B0F,2}, 		//  PGA_TABLE_CWF_DATA_37
	{0xD13C, 0x6550,2}, 		//  PGA_TABLE_CWF_DATA_38
	{0xD13E, 0x2571,2}, 		//  PGA_TABLE_CWF_DATA_39
	{0xD140, 0x15D2,2}, 		//  PGA_TABLE_CWF_DATA_40
	{0xD142, 0x22AF,2}, 		//  PGA_TABLE_CWF_DATA_41
	{0xD144, 0xC213,2}, 		//  PGA_TABLE_CWF_DATA_42
	{0xD146, 0x1411,2}, 		//  PGA_TABLE_CWF_DATA_43
	{0xD148, 0x29F4,2}, 		//  PGA_TABLE_CWF_DATA_44
	{0xD14A, 0x7C52,2}, 		//  PGA_TABLE_CWF_DATA_45
	{0xD14C, 0x31D0,2}, 		//  PGA_TABLE_CWF_DATA_46
	{0xD14E, 0xF873,2}, 		//  PGA_TABLE_CWF_DATA_47
	{0xD150, 0xC2D4,2}, 		//  PGA_TABLE_CWF_DATA_48
	{0xD152, 0xD532,2}, 		//  PGA_TABLE_CWF_DATA_49
	{0xD154, 0x4011,2}, 		//  PGA_TABLE_CWF_DATA_50
	{0xD156, 0x6B4D,2}, 		//  PGA_TABLE_CWF_DATA_51
	{0xD158, 0xAF92,2}, 		//  PGA_TABLE_CWF_DATA_52
	{0xD15A, 0x16F2,2}, 		//  PGA_TABLE_CWF_DATA_53
	{0xD15C, 0x5F33,2}, 		//  PGA_TABLE_CWF_DATA_54
	{0xD15E, 0x0FF2,2}, 		//  PGA_TABLE_CWF_DATA_55
	{0xD160, 0x854F,2}, 		//  PGA_TABLE_CWF_DATA_56
	{0xD162, 0x8454,2}, 		//  PGA_TABLE_CWF_DATA_57
	{0xD164, 0xCF13,2}, 		//  PGA_TABLE_CWF_DATA_58
	{0xD166, 0x1DF5,2}, 		//  PGA_TABLE_CWF_DATA_59
	{0xD168, 0x59B0,2}, 		//  PGA_TABLE_CWF_DATA_60
	{0xD16A, 0x60CF,2}, 		//  PGA_TABLE_CWF_DATA_61
	{0xD16C, 0x4673,2}, 		//  PGA_TABLE_CWF_DATA_62
	{0xD16E, 0x5DD1,2}, 		//  PGA_TABLE_CWF_DATA_63
	{0xD170, 0x9135,2}, 		//  PGA_TABLE_CWF_DATA_64
	{0xD172, 0x58B1,2}, 		//  PGA_TABLE_CWF_DATA_65
	{0xD174, 0x1E4F,2}, 		//  PGA_TABLE_CWF_DATA_66
	{0xD176, 0x0F31,2}, 		//  PGA_TABLE_CWF_DATA_67
	{0xD178, 0x99F4,2}, 		//  PGA_TABLE_CWF_DATA_68
	{0xD17A, 0xC753,2}, 		//  PGA_TABLE_CWF_DATA_69
	{0xD17C, 0x1F8F,2}, 		//  PGA_TABLE_CWF_DATA_70
	{0xD17E, 0xFFCF,2}, 		//  PGA_TABLE_CWF_DATA_71
	{0xD180, 0x2494,2}, 		//  PGA_TABLE_CWF_DATA_72
	{0xD182, 0xF330,2}, 		//  PGA_TABLE_CWF_DATA_73
	{0xD184, 0xC2F5,2}, 		//  PGA_TABLE_CWF_DATA_74
	{0xD186, 0x15CF,2}, 		//  PGA_TABLE_CWF_DATA_75
	{0xD188, 0x1311,2}, 		//  PGA_TABLE_CWF_DATA_76
	{0xD18A, 0x3934,2}, 		//  PGA_TABLE_CWF_DATA_77
	{0xD18C, 0xBF93,2}, 		//  PGA_TABLE_CWF_DATA_78
	{0xD18E, 0xC355,2}, 		//  PGA_TABLE_CWF_DATA_79
	{0xD190, 0xE293,2}, 		//  PGA_TABLE_CWF_DATA_80
	{0xD192, 0xD773,2}, 		//  PGA_TABLE_CWF_DATA_81
	{0xD194, 0x9777,2}, 		//  PGA_TABLE_CWF_DATA_82
	{0xD196, 0x38B6,2}, 		//  PGA_TABLE_CWF_DATA_83
	{0xD198, 0x5DB9,2}, 		//  PGA_TABLE_CWF_DATA_84
	{0xD19A, 0xA354,2}, 		//  PGA_TABLE_CWF_DATA_85
	{0xD19C, 0x9334,2}, 		//  PGA_TABLE_CWF_DATA_86
	{0xD19E, 0xFD97,2}, 		//  PGA_TABLE_CWF_DATA_87
	{0xD1A0, 0x1A57,2}, 		//  PGA_TABLE_CWF_DATA_88
	{0xD1A2, 0x409A,2}, 		//  PGA_TABLE_CWF_DATA_89
	{0xD1A4, 0xA0B3,2}, 		//  PGA_TABLE_CWF_DATA_90
	{0xD1A6, 0xA3F1,2}, 		//  PGA_TABLE_CWF_DATA_91
	{0xD1A8, 0xC296,2}, 		//  PGA_TABLE_CWF_DATA_92
	{0xD1AA, 0x1636,2}, 		//  PGA_TABLE_CWF_DATA_93
	{0xD1AC, 0x5159,2}, 		//  PGA_TABLE_CWF_DATA_94
	{0xD1AE, 0xB693,2}, 		//  PGA_TABLE_CWF_DATA_95
	{0xD1B0, 0x4330,2}, 		//  PGA_TABLE_CWF_DATA_96
	{0xD1B2, 0x9437,2}, 		//  PGA_TABLE_CWF_DATA_97
	{0xD1B4, 0x0996,2}, 		//  PGA_TABLE_CWF_DATA_98
	{0xD1B6, 0x5E79,2}, 		//  PGA_TABLE_CWF_DATA_99
	{0xD1BA, 0x040C,2}, 		// PGA_TABLE_CWF_CENTER_COLUMN
	{0xD1B8, 0x024C,2}, 		// PGA_TABLE_CWF_CENTER_ROW
	{0xC9FE, 0x0FA0,2}, 		// CAM_PGA_M_CONFIG_COLOUR_TEMP
	{0xCA00, 0x8000,2}, 		// CAM_PGA_M_CONFIG_GREEN_RED_Q14
	{0xCA02, 0x8000,2}, 		// CAM_PGA_M_CONFIG_RED_Q14
	{0xCA04, 0x8000,2}, 		// CAM_PGA_M_CONFIG_GREEN_BLUE_Q14
	{0xCA06, 0x8000,2}, 		// CAM_PGA_M_CONFIG_BLUE_Q14
	{0xD024, 0x00F0,2}, 		// PGA_TABLE_D65_DATA_0
	{0xD026, 0xA48D,2}, 		// PGA_TABLE_D65_DATA_1
	{0xD028, 0x0652,2}, 		// PGA_TABLE_D65_DATA_2
	{0xD02A, 0xB0AC,2}, 		// PGA_TABLE_D65_DATA_3
	{0xD02C, 0xA512,2}, 		// PGA_TABLE_D65_DATA_4
	{0xD02E, 0x01F0,2}, 		// PGA_TABLE_D65_DATA_5
	{0xD030, 0x28CE,2}, 		// PGA_TABLE_D65_DATA_6
	{0xD032, 0x4072,2}, 		// PGA_TABLE_D65_DATA_7
	{0xD034, 0x2C2E,2}, 		// PGA_TABLE_D65_DATA_8
	{0xD036, 0xA273,2}, 		// PGA_TABLE_D65_DATA_9
	{0xD038, 0x0130,2}, 		// PGA_TABLE_D65_DATA_10
	{0xD03A, 0xB4ED,2}, 		// PGA_TABLE_D65_DATA_11
	{0xD03C, 0x0911,2}, 		// PGA_TABLE_D65_DATA_12
	{0xD03E, 0x8CEF,2}, 		// PGA_TABLE_D65_DATA_13
	{0xD040, 0x9E11,2}, 		// PGA_TABLE_D65_DATA_14
	{0xD042, 0x0C50,2}, 		// PGA_TABLE_D65_DATA_15
	{0xD044, 0x500D,2}, 		// PGA_TABLE_D65_DATA_16
	{0xD046, 0x1472,2}, 		// PGA_TABLE_D65_DATA_17
	{0xD048, 0xCFCE,2}, 		// PGA_TABLE_D65_DATA_18
	{0xD04A, 0xFE72,2}, 		// PGA_TABLE_D65_DATA_19
	{0xD04C, 0x396B,2}, 		// PGA_TABLE_D65_DATA_20
	{0xD04E, 0x988F,2}, 		// PGA_TABLE_D65_DATA_21
	{0xD050, 0x88F0,2}, 		// PGA_TABLE_D65_DATA_22
	{0xD052, 0x6ECE,2}, 		// PGA_TABLE_D65_DATA_23
	{0xD054, 0x1B92,2}, 		// PGA_TABLE_D65_DATA_24
	{0xD056, 0x13AD,2}, 		// PGA_TABLE_D65_DATA_25
	{0xD058, 0x734F,2}, 		// PGA_TABLE_D65_DATA_26
	{0xD05A, 0xAC0E,2}, 		// PGA_TABLE_D65_DATA_27
	{0xD05C, 0xB391,2}, 		// PGA_TABLE_D65_DATA_28
	{0xD05E, 0x560E,2}, 		// PGA_TABLE_D65_DATA_29
	{0xD060, 0x8D0A,2}, 		// PGA_TABLE_D65_DATA_30
	{0xD062, 0x556F,2}, 		// PGA_TABLE_D65_DATA_31
	{0xD064, 0xFD0B,2}, 		// PGA_TABLE_D65_DATA_32
	{0xD066, 0xA2F1,2}, 		// PGA_TABLE_D65_DATA_33
	{0xD068, 0x2CF1,2}, 		// PGA_TABLE_D65_DATA_34
	{0xD06A, 0xA52C,2}, 		// PGA_TABLE_D65_DATA_35
	{0xD06C, 0xAD2F,2}, 		// PGA_TABLE_D65_DATA_36
	{0xD06E, 0x3B0F,2}, 		// PGA_TABLE_D65_DATA_37
	{0xD070, 0x6550,2}, 		// PGA_TABLE_D65_DATA_38
	{0xD072, 0x2571,2}, 		// PGA_TABLE_D65_DATA_39
	{0xD074, 0x15D2,2}, 		// PGA_TABLE_D65_DATA_40
	{0xD076, 0x22AF,2}, 		// PGA_TABLE_D65_DATA_41
	{0xD078, 0xC213,2}, 		// PGA_TABLE_D65_DATA_42
	{0xD07A, 0x1411,2}, 		// PGA_TABLE_D65_DATA_43
	{0xD07C, 0x29F4,2}, 		// PGA_TABLE_D65_DATA_44
	{0xD07E, 0x7C52,2}, 		// PGA_TABLE_D65_DATA_45
	{0xD080, 0x31D0,2}, 		// PGA_TABLE_D65_DATA_46
	{0xD082, 0xF873,2}, 		// PGA_TABLE_D65_DATA_47
	{0xD084, 0xC2D4,2}, 		// PGA_TABLE_D65_DATA_48
	{0xD086, 0xD532,2}, 		// PGA_TABLE_D65_DATA_49
	{0xD088, 0x4011,2}, 		// PGA_TABLE_D65_DATA_50
	{0xD08A, 0x6B4D,2}, 		// PGA_TABLE_D65_DATA_51
	{0xD08C, 0xAF92,2}, 		// PGA_TABLE_D65_DATA_52
	{0xD08E, 0x16F2,2}, 		// PGA_TABLE_D65_DATA_53
	{0xD090, 0x5F33,2}, 		// PGA_TABLE_D65_DATA_54
	{0xD092, 0x0FF2,2}, 		// PGA_TABLE_D65_DATA_55
	{0xD094, 0x854F,2}, 		// PGA_TABLE_D65_DATA_56
	{0xD096, 0x8454,2}, 		// PGA_TABLE_D65_DATA_57
	{0xD098, 0xCF13,2}, 		// PGA_TABLE_D65_DATA_58
	{0xD09A, 0x1DF5,2}, 		// PGA_TABLE_D65_DATA_59
	{0xD09C, 0x59B0,2}, 		// PGA_TABLE_D65_DATA_60
	{0xD09E, 0x60CF,2}, 		// PGA_TABLE_D65_DATA_61
	{0xD0A0, 0x4673,2}, 		// PGA_TABLE_D65_DATA_62
	{0xD0A2, 0x5DD1,2}, 		// PGA_TABLE_D65_DATA_63
	{0xD0A4, 0x9135,2}, 		// PGA_TABLE_D65_DATA_64
	{0xD0A6, 0x58B1,2}, 		// PGA_TABLE_D65_DATA_65
	{0xD0A8, 0x1E4F,2}, 		// PGA_TABLE_D65_DATA_66
	{0xD0AA, 0x0F31,2}, 		// PGA_TABLE_D65_DATA_67
	{0xD0AC, 0x99F4,2}, 		// PGA_TABLE_D65_DATA_68
	{0xD0AE, 0xC753,2}, 		// PGA_TABLE_D65_DATA_69
	{0xD0B0, 0x1F8F,2}, 		// PGA_TABLE_D65_DATA_70
	{0xD0B2, 0xFFCF,2}, 		// PGA_TABLE_D65_DATA_71
	{0xD0B4, 0x2494,2}, 		// PGA_TABLE_D65_DATA_72
	{0xD0B6, 0xF330,2}, 		// PGA_TABLE_D65_DATA_73
	{0xD0B8, 0xC2F5,2}, 		// PGA_TABLE_D65_DATA_74
	{0xD0BA, 0x15CF,2}, 		// PGA_TABLE_D65_DATA_75
	{0xD0BC, 0x1311,2}, 		// PGA_TABLE_D65_DATA_76
	{0xD0BE, 0x3934,2}, 		// PGA_TABLE_D65_DATA_77
	{0xD0C0, 0xBF93,2}, 		// PGA_TABLE_D65_DATA_78
	{0xD0C2, 0xC355,2}, 		// PGA_TABLE_D65_DATA_79
	{0xD0C4, 0xE293,2}, 		// PGA_TABLE_D65_DATA_80
	{0xD0C6, 0xD773,2}, 		// PGA_TABLE_D65_DATA_81
	{0xD0C8, 0x9777,2}, 		// PGA_TABLE_D65_DATA_82
	{0xD0CA, 0x38B6,2}, 		// PGA_TABLE_D65_DATA_83
	{0xD0CC, 0x5DB9,2}, 		// PGA_TABLE_D65_DATA_84
	{0xD0CE, 0xA354,2}, 		// PGA_TABLE_D65_DATA_85
	{0xD0D0, 0x9334,2}, 		// PGA_TABLE_D65_DATA_86
	{0xD0D2, 0xFD97,2}, 		// PGA_TABLE_D65_DATA_87
	{0xD0D4, 0x1A57,2}, 		// PGA_TABLE_D65_DATA_88
	{0xD0D6, 0x409A,2}, 		// PGA_TABLE_D65_DATA_89
	{0xD0D8, 0xA0B3,2}, 		// PGA_TABLE_D65_DATA_90
	{0xD0DA, 0xA3F1,2}, 		// PGA_TABLE_D65_DATA_91
	{0xD0DC, 0xC296,2}, 		// PGA_TABLE_D65_DATA_92
	{0xD0DE, 0x1636,2}, 		// PGA_TABLE_D65_DATA_93
	{0xD0E0, 0x5159,2}, 		// PGA_TABLE_D65_DATA_94
	{0xD0E2, 0xB693,2}, 		// PGA_TABLE_D65_DATA_95
	{0xD0E4, 0x4330,2}, 		// PGA_TABLE_D65_DATA_96
	{0xD0E6, 0x9437,2}, 		// PGA_TABLE_D65_DATA_97
	{0xD0E8, 0x0996,2}, 		// PGA_TABLE_D65_DATA_98
	{0xD0EA, 0x5E79,2}, 		// PGA_TABLE_D65_DATA_99
	{0xD0EE, 0x0400,2},	 	//  PGA_TABLE_D65_CENTER_COLUMN
	{0xD0EC, 0x023C,2},	 	//  PGA_TABLE_D65_CENTER_ROW
	{0xCA08, 0x1964,2},	 	//  CAM_PGA_R_CONFIG_COLOUR_TEMP
	{0xCA0A, 0x8000,2},	 	//  CAM_PGA_R_CONFIG_GREEN_RED_Q14
	{0xCA0C, 0x8000,2},	 	//  CAM_PGA_R_CONFIG_RED_Q14
	{0xCA0E, 0x8000,2},	 	//  CAM_PGA_R_CONFIG_GREEN_BLUE_Q14
	{0xCA10, 0x8000,2},	 	//  CAM_PGA_R_CONFIG_BLUE_Q14
	{0xC9F2, 0x0011,2},	 	//  CAM_PGA_PGA_CONTROL
	{0xD018, 0xFF,1},	 	//  PGA_CURRENT_ZONE

	{0xD01C, 0x8C,1},	 	// PGA_ZONE_LO_0
	{0xD01D, 0x82,1},	 	// PGA_ZONE_LO_1
	{0xD01E, 0x00,1},	 	// PGA_ZONE_LO_2
	{0xD01F, 0xFF,1},	 	// PGA_ZONE_HI_0
	{0xD020, 0x90,1},	 	// PGA_ZONE_HI_1
	{0xD021, 0x87,1},	 	// PGA_ZONE_HI_2
	{0xD01B, 0x03,1},	 	// PGA_NUMBER_ZONES
	{0xC9F2, 0x0011,2},	 	// CAM_PGA_PGA_CONTROL

	//==> Color gain Setting Start [
	{0xAC0A, 0x00,1},	 	// AWB_R_SCENE_RATIO_LOWER
	{0xAC0B, 0x46,1},	 	// AWB_R_SCENE_RATIO_UPPER
	{0xAC0C, 0x29,1},	 	// AWB_B_SCENE_RATIO_LOWER
	{0xAC0D, 0xFF,1},	 	// AWB_B_SCENE_RATIO_UPPER
	{0xC8EE, 0x09C4,2},	 	// CAM_AWB_COLOR_TEMPERATURE_MIN
	{0xC8F0, 0x1964,2},	 	// CAM_AWB_COLOR_TEMPERATURE_MAX
	{0xAC06, 0x63,1},	 	// AWB_R_RATIO_LOWER
	{0xAC07, 0x65,1},	 	// AWB_R_RATIO_UPPER
	{0xAC08, 0x63,1},	 	// AWB_B_RATIO_LOWER
	{0xAC09, 0x65,1},	 	// AWB_B_RATIO_UPPER
	{0xC94A, 0x08,1},	 	// CAM_STAT_LUMA_THRESH_LOW
	{0xC94B, 0xF7,1},	 	// CAM_STAT_LUMA_THRESH_HIGH

	//[AWB and CCMs 11/14/12 15:19:39]
	{0xC894, 0x0165,2}, 	// CAM_AWB_CCM_L_0
	{0xC896, 0xFFA3,2}, 	// CAM_AWB_CCM_L_1
	{0xC898, 0xFFEE,2}, 	// CAM_AWB_CCM_L_2
	{0xC89A, 0xFFAE,2}, 	// CAM_AWB_CCM_L_3
	{0xC89C, 0x0163,2}, 	// CAM_AWB_CCM_L_4
	{0xC89E, 0xFFD9,2}, 	// CAM_AWB_CCM_L_5
	{0xC8A0, 0xFFCB,2}, 	// CAM_AWB_CCM_L_6
	{0xC8A2, 0xFF9B,2}, 	// CAM_AWB_CCM_L_7
	{0xC8A4, 0x01A0,2}, 	// CAM_AWB_CCM_L_8
	{0xC8A6, 0x019C,2}, 	// CAM_AWB_CCM_M_0
	{0xC8A8, 0xFF7B,2}, 	// CAM_AWB_CCM_M_1
	{0xC8AA, 0xFFEA,2}, 	// CAM_AWB_CCM_M_2
	{0xC8AC, 0xFFC8,2}, 	// CAM_AWB_CCM_M_3
	{0xC8AE, 0x014F,2}, 	// CAM_AWB_CCM_M_4
	{0xC8B0, 0xFFED,2}, 	// CAM_AWB_CCM_M_5
	{0xC8B2, 0xFFC4,2}, 	// CAM_AWB_CCM_M_6
	{0xC8B4, 0xFF89,2}, 	// CAM_AWB_CCM_M_7
	{0xC8B6, 0x0186,2}, 	// CAM_AWB_CCM_M_8
	{0xC8B8, 0x015C,2}, 	// CAM_AWB_CCM_R_0
	{0xC8BA, 0xFF8B,2}, 	// CAM_AWB_CCM_R_1
	{0xC8BC, 0x0014,2}, 	// CAM_AWB_CCM_R_2
	{0xC8BE, 0xFFC7,2}, 	// CAM_AWB_CCM_R_3
	{0xC8C0, 0x0171,2}, 	// CAM_AWB_CCM_R_4
	{0xC8C2, 0xFFAB,2}, 	// CAM_AWB_CCM_R_5
	{0xC8C4, 0xFFB7,2}, 	// CAM_AWB_CCM_R_6
	{0xC8C6, 0xFFD9,2}, 	// CAM_AWB_CCM_R_7
	{0xC8C8, 0x0196,2}, 	// CAM_AWB_CCM_R_8
	{0xC8CA, 0x007D,2}, 	// CAM_AWB_CCM_L_RG_GAIN
	{0xC8CC, 0x018F,2}, 	// CAM_AWB_CCM_L_BG_GAIN
	{0xC8CE, 0x008C,2}, 	// CAM_AWB_CCM_M_RG_GAIN
	{0xC8D0, 0x011B,2}, 	// CAM_AWB_CCM_M_BG_GAIN
	{0xC8D2, 0x00CC,2}, 	// CAM_AWB_CCM_R_RG_GAIN
	{0xC8D4, 0x00DD,2}, 	// CAM_AWB_CCM_R_BG_GAIN
	{0xC8D6, 0x09C4,2}, 	// CAM_AWB_CCM_L_CTEMP
	{0xC8D8, 0x0D67,2}, 	// CAM_AWB_CCM_M_CTEMP
	{0xC8DA, 0x1964,2}, 	// CAM_AWB_CCM_R_CTEMP
	{0xC94C, 0x0000,2}, 	// CAM_STAT_AWB_CLIP_WINDOW_XSTA
	{0xC94E, 0x0000,2}, 	// CAM_STAT_AWB_CLIP_WINDOW_YSTA
	{0xC950, 0x077F,2}, 	// CAM_STAT_AWB_CLIP_WINDOW_XEND
	{0xC952, 0x0437,2}, 	// CAM_STAT_AWB_CLIP_WINDOW_YEND
	{0xC95C, 0x0020,2}, 	// CAM_STAT_AWB_X_SHIFT
	{0xC95E, 0x000C,2}, 	// CAM_STAT_AWB_Y_SHIFT
	{0xC8F4, 0x0080,2}, 	// CAM_AWB_AWB_XSCALE
	{0xC8F6, 0x00AB,2}, 	// CAM_AWB_AWB_YSCALE
	{0xC8F8, 0x03F8,2}, 	// CAM_AWB_AWB_ROT_CENTER_X
	{0xC8FA, 0x03DB,2}, 	// CAM_AWB_AWB_ROT_CENTER_Y
	{0xC8FC, 0x00,1}, 	// CAM_AWB_AWB_ROT_ANGLE_COS
	{0xC8FD, 0x00,1}, 	// CAM_AWB_AWB_ROT_ANGLE_SIN
	{0xC900, 0x0000,2}, 	// CAM_AWB_AWB_WEIGHTS_0
	{0xC902, 0x0001,2}, 	// CAM_AWB_AWB_WEIGHTS_1
	{0xC904, 0x1000,2}, 	// CAM_AWB_AWB_WEIGHTS_2
	{0xC906, 0x0000,2}, 	// CAM_AWB_AWB_WEIGHTS_3
	{0xC908, 0x0000,2}, 	// CAM_AWB_AWB_WEIGHTS_4
	{0xC90A, 0x0011,2}, 	// CAM_AWB_AWB_WEIGHTS_5
	{0xC90C, 0x1110,2}, 	// CAM_AWB_AWB_WEIGHTS_6
	{0xC90E, 0x0000,2}, 	// CAM_AWB_AWB_WEIGHTS_7
	{0xC910, 0x0122,2}, 	// CAM_AWB_AWB_WEIGHTS_8
	{0xC912, 0x2222,2}, 	// CAM_AWB_AWB_WEIGHTS_9
	{0xC914, 0x2332,2}, 	// CAM_AWB_AWB_WEIGHTS_10
	{0xC916, 0x2211,2}, 	// CAM_AWB_AWB_WEIGHTS_11
	{0xC918, 0x1244,2}, 	// CAM_AWB_AWB_WEIGHTS_12
	{0xC91A, 0x4455,2}, 	// CAM_AWB_AWB_WEIGHTS_13
	{0xC91C, 0x4444,2}, 	// CAM_AWB_AWB_WEIGHTS_14
	{0xC91E, 0x4432,2}, 	// CAM_AWB_AWB_WEIGHTS_15
	{0xC920, 0x1245,2}, 	// CAM_AWB_AWB_WEIGHTS_16
	{0xC922, 0x5555,2}, 	// CAM_AWB_AWB_WEIGHTS_17
	{0xC924, 0x4445,2}, 	// CAM_AWB_AWB_WEIGHTS_18
	{0xC926, 0x5442,2}, 	// CAM_AWB_AWB_WEIGHTS_19
	{0xC928, 0x1233,2}, 	// CAM_AWB_AWB_WEIGHTS_20
	{0xC92A, 0x3444,2}, 	// CAM_AWB_AWB_WEIGHTS_21
	{0xC92C, 0x3223,2}, 	// CAM_AWB_AWB_WEIGHTS_22
	{0xC92E, 0x3322,2}, 	// CAM_AWB_AWB_WEIGHTS_23
	{0xC930, 0x0111,2}, 	// CAM_AWB_AWB_WEIGHTS_24
	{0xC932, 0x1222,2}, 	// CAM_AWB_AWB_WEIGHTS_25
	{0xC934, 0x1111,2}, 	// CAM_AWB_AWB_WEIGHTS_26
	{0xC936, 0x1111,2}, 	// CAM_AWB_AWB_WEIGHTS_27
	{0xC938, 0x0000,2}, 	// CAM_AWB_AWB_WEIGHTS_28
	{0xC93A, 0x0011,2}, 	// CAM_AWB_AWB_WEIGHTS_29
	{0xC93C, 0x0000,2}, 	// CAM_AWB_AWB_WEIGHTS_30
	{0xC93E, 0x0000,2}, 	// CAM_AWB_AWB_WEIGHTS_31
	//==> Color gain Setting End ]

	{0x33F4, 0x0517,2}, 		//KERNEL_CONFIG
	{0xC81A, 0x003D,2}, 		//CAM_SENSOR_CFG_MIN_ANALOG_GAIN
	{0xC81C, 0x00F4,2}, 		//CAM_SENSOR_CFG_MAX_ANALOG_GAIN
	{0xC96C, 0x2b,1}, 		//CAM_LL_START_DEMOSAIC
	{0xC971, 0x2B,1}, 		//CAM_LL_STOP_DEMOSAIC
	{0xC96E, 0x03,1}, 		//CAM_LL_START_AP_NOISE_GAIN
	{0xC96D, 0x01,1}, 		//CAM_LL_START_AP_NOISE_KNEE
	{0xC973, 0x53,1}, 		//CAM_LL_STOP_AP_NOISE_GAIN
	{0xC972, 0x1C,1}, 		//CAM_LL_STOP_AP_NOISE_KNEE
	{0xC96F, 0x09,1}, 		//CAM_LL_START_AP_GAIN_POS
	{0xC970, 0x0f,1}, 		//CAM_LL_START_AP_GAIN_NEG
	{0xC974, 0x00,1}, 		//CAM_LL_STOP_AP_GAIN_POS
	{0xC975, 0x00,1}, 		//CAM_LL_STOP_AP_GAIN_NEG
	{0xC976, 0x0E,1}, 		//CAM_LL_START_GRB_APOS
	{0xC977, 0x07,1}, 		//CAM_LL_START_GRB_BPOS
	{0xC978, 0x0A,1}, 		//CAM_LL_START_GRB_ANEG
	{0xC979, 0x32,1}, 		//CAM_LL_START_GRB_BNEG
	{0xC97A, 0xE3,1}, 		//CAM_LL_STOP_GRB_APOS
	{0xC97C, 0x3B,1}, 		//CAM_LL_STOP_GRB_ANEG
	{0xC97B, 0x9D,1}, 		//CAM_LL_STOP_GRB_BPOS
	{0xC97D, 0x64,1}, 		//CAM_LL_STOP_GRB_BNEG
	{0x33F4, 0x0515,2}, 		//KERNEL_CONFIG
	{0xC9CE, 0x0040,2}, 		//CAM_LL_START_MAX_GAIN_METRIC
	{0xC9D0, 0x0400,2}, 		//CAM_LL_MID_MAX_GAIN_METRIC
	{0xC9D2, 0x1000,2}, 		//CAM_LL_STOP_MAX_GAIN_METRIC
	{0xC984, 0x0136,2}, 		//CAM_LL_START_CDC_HI_THR_COMB
	{0xC988, 0x00D2,2}, 		//CAM_LL_START_CDC_HI_THR_SATUR
	{0xC98C, 0x00FC,2}, 		//CAM_LL_MID_CDC_HI_THR_COMB
	{0xC990, 0x0132,2}, 		//CAM_LL_MID_CDC_HI_THR_SATUR
	{0xC994, 0x07B6,2}, 		//CAM_LL_STOP_CDC_HI_THR_COMB
	{0xC998, 0x035E,2}, 		//CAM_LL_STOP_CDC_HI_THR_SATUR
	{0xC986, 0x00A9,2}, 		//CAM_LL_START_CDC_LO_THR_COMB
	{0xC98A, 0x00A6,2}, 		//CAM_LL_START_CDC_LO_THR_SATUR
	{0xC98E, 0x0119,2}, 		//CAM_LL_MID_CDC_LO_THR_COMB
	{0xC992, 0x0134,2}, 		//CAM_LL_MID_CDC_LO_THR_SATUR
	{0xC996, 0x062E,2}, 		//CAM_LL_STOP_CDC_LO_THR_COMB
	{0xC99A, 0x026F,2}, 		//CAM_LL_STOP_CDC_LO_THR_SATUR
	{0xC97E, 0x000F,2}, 		//CAM_LL_START_DC_SINGLE_PIXEL_THR
	{0xC980, 0x0005,2}, 		//CAM_LL_STOP_DC_SINGLE_PIXEL_THR
	{0xC99C, 0x0017,2}, 		//CAM_LL_START_CDC_CC_NOISE_SLOPE
	{0xC99E, 0x0008,2}, 		//CAM_LL_START_CDC_CC_NOISE_KNEE
	{0xC9A0, 0x0026,2}, 		//CAM_LL_MID_CDC_CC_NOISE_SLOPE
	{0xC9A2, 0x003E,2}, 		//CAM_LL_MID_CDC_CC_NOISE_KNEE
	{0xC9A4, 0x0055,2}, 		//CAM_LL_STOP_CDC_CC_NOISE_SLOPE
	{0xC9A6, 0x00FF,2}, 		//CAM_LL_STOP_CDC_CC_NOISE_KNEE
	{0x33F4, 0x0D15,2}, 		//KERNEL_CONF	IG
	{0xC9A8, 0x003A,2}, 		//CAM_LL_ADACD_LUT_GAIN_0
	{0xC9AE, 0x03,1}, 		//CAM_LL_ADACD_LUT_SIGMA_0
	{0xC9B2, 0x0034,2}, 		//CAM_LL_ADACD_LUT_K_0
	{0xC9AA, 0x00B0,2}, 		//CAM_LL_ADACD_LUT_GAIN_1
	{0xC9AF, 0x04,1}, 		//CAM_LL_ADACD_LUT_SIGMA_1
	{0xC9B4, 0x004c,2}, 		//CAM_LL_ADACD_LUT_K_1
	{0xC9AC, 0x00e0,2}, 		//CAM_LL_ADACD_LUT_GAIN_2
	{0xC9B0, 0x07,1}, 		//CAM_LL_ADACD_LUT_SIGMA_2
	{0xC9B6, 0x004a,2}, 		//CAM_LL_ADACD_LUT_K_2
	{0xBC02, 0x0013,2}, 		//LL_MODE
	{0xC9BE, 0x00,1}, 		//CAM_LL_ADACD_LL_MODE_EN
	//{0x3398, 0x0060,2}, 		//ADACD_LOWLIGHT_CONTROL
	//{0x3398, 0x0260,2}, 		//ADACD_LOWLIGHT_CONTROL
	//{0x3398, 0x0030,2}, 		//ADACD_LOWLIGHT_CONTROL
	{0x3398, 0x2230,2},
	{0xC9B1, 0x01,1}, 		//CAM_LL_ADACD_PATCH
	{0xC9B8, 0x021F,2}, 		//CAM_LL_ADACD_TRT
	{0x326E, 0x0086,2}, 		//LOW_PASS_YUV_FILTER
	{0x3270, 0x0FAA,2}, 		//THRESHOLD_FOR_Y_FILTER_R_CHANNEL
	{0x3272, 0x0FE4,2}, 		//THRESHOLD_FOR_Y_FILTER_G_CHANNEL
	{0xC9CA, 0x0040,2}, 		//CAM_LL_START_GAIN_METRIC
	{0xC9CC, 0x1000,2}, 		//CAM_LL_STOP_GAIN_METRIC
	{0xC9CE, 0x0040,2}, 		//CAM_LL_START_MAX_GAIN_METRIC
	{0xC9D2, 0x1000,2}, 		//CAM_LL_STOP_MAX_GAIN_METRIC
	{0xC944, 0x80,1}, 		//CAM_AWB_K_R_L
	{0xC945, 0x80,1}, 		//CAM_AWB_K_G_L
	{0xC946, 0x80,1}, 		//CAM_AWB_K_B_L
	{0xC947, 0x80,1}, 		//CAM_AWB_K_R_R
	{0xC948, 0x80,1}, 		//CAM_AWB_K_G_R
	{0xC949, 0x7D,1}, 		//CAM_AWB_K_B_R
	{0xC962, 0x0032,2}, 		//CAM_LL_START_BRIGHTNESS
	{0xC964, 0x024D,2}, 		//CAM_LL_STOP_BRIGHTNESS
	//{0xC88A, 0x0180,2}, 		//CAM_AET_AE_MAX_VIRT_DGAIN
	{0xC87E, 0x38,1}, 		//CAM_AET_TARGET_AVERAGE_LUMA
	{0xC87F, 0x51,1}, 		//CAM_AET_TARGET_AVERAGE_LUMA_DARK
	{0x32B2, 0x2314,2}, 		//DKDELTA_CCM_CTL
	{0xB402, 0x0002,2}, 		//CCM_MODE
	{0xC967, 0x3c,1}, 		//CAM_LL_END_SATURATION
	{0xC968, 0x80,1}, 		//CAM_LL_START_DESATURATION
	{0xC969, 0x28,1}, 		//CAM_LL_END_DESATURATION
	{0xC96A, 0x18,1}, 		//CAM_LL_START_DARK_DELTA_CCM_THR
	{0xC96B, 0x0F,1}, 		//CAM_LL_STOP_DARK_DELTA_CCM_THR
	{0xB42A, 0x05,1}, 		//CCM_DELTA_GAIN
	{0xC8DC, 0x0100,2}, 		//CAM_AWB_LL_CCM_0
	{0xC8DE, 0x0000,2}, 		//CAM_AWB_LL_CCM_1
	{0xC8E0, 0x0000,2}, 		//CAM_AWB_LL_CCM_2
	{0xC8E2, 0x0000,2}, 		//CAM_AWB_LL_CCM_3
	{0xC8E4, 0x0100,2}, 		//CAM_AWB_LL_CCM_4
	{0xC8E6, 0x0000,2}, 		//CAM_AWB_LL_CCM_5
	{0xC8E8, 0x0000,2}, 		//CAM_AWB_LL_CCM_6
	{0xC8EA, 0x0000,2}, 		//CAM_AWB_LL_CCM_7
	{0xC8EC, 0x0100,2}, 		//CAM_AWB_LL_CCM_8
	{0xC9EF, 0x25,1}, 		//CAM_SEQ_DARK_COLOR_KILL
	{0xC9D4, 0x0050,2}, 		//CAM_LL_START_FADE_TO_BLACK_LUMA
	{0xC9D6, 0x0030,2}, 		//CAM_LL_STOP_FADE_TO_BLACK_LUMA
	{0xCA1C, 0x8040,2}, 		//CAM_PORT_OUTPUT_CONTROL
	{0x001E, 0x0777,2}, 		//PAD_SLEW
	{0xCA1C, 0x8043,2}, 		//CAM_PORT_OUTPUT_CONTROL
	{0xC88A, 0x0100,2}, 		//CAM_AET_AE_MAX_VIRT_DGAIN
	{0xC882, 0x000A,2}, 		//CAM_AET_BLACK_CLIPPING_TARGET
	{0xC892, 0x0083,2}, 		//CAM_AET_TARGET_GAIN
	{0xA408, 0x01,1}, 		// AE_RULE_AE_WEIGHT_TABLE_0_0
	{0xA409, 0x01,1}, 		// AE_RULE_AE_WEIGHT_TABLE_0_1
	{0xA40A, 0x01,1}, 		// AE_RULE_AE_WEIGHT_TABLE_0_2
	{0xA40B, 0x01,1}, 		// AE_RULE_AE_WEIGHT_TABLE_0_3
	{0xA40C, 0x01,1}, 		// AE_RULE_AE_WEIGHT_TABLE_0_4
	{0xA40D, 0x01,1}, 		// AE_RULE_AE_WEIGHT_TABLE_1_0
	{0xA40E, 0x64,1}, 		// AE_RULE_AE_WEIGHT_TABLE_1_1
	{0xA40F, 0x64,1}, 		// AE_RULE_AE_WEIGHT_TABLE_1_2
	{0xA410, 0x64,1}, 		// AE_RULE_AE_WEIGHT_TABLE_1_3
	{0xA411, 0x01,1}, 		// AE_RULE_AE_WEIGHT_TABLE_1_4
	{0xA412, 0x01,1}, 		// AE_RULE_AE_WEIGHT_TABLE_2_0
	{0xA413, 0x64,1}, 		// AE_RULE_AE_WEIGHT_TABLE_2_1
	{0xA414, 0x64,1}, 		// AE_RULE_AE_WEIGHT_TABLE_2_2
	{0xA415, 0x64,1}, 		// AE_RULE_AE_WEIGHT_TABLE_2_3
	{0xA416, 0x01,1}, 		// AE_RULE_AE_WEIGHT_TABLE_2_4
	{0xA417, 0x01,1}, 		// AE_RULE_AE_WEIGHT_TABLE_3_0
	{0xA418, 0x64,1}, 		// AE_RULE_AE_WEIGHT_TABLE_3_1
	{0xA419, 0x64,1}, 		// AE_RULE_AE_WEIGHT_TABLE_3_2
	{0xA41A, 0x64,1}, 		// AE_RULE_AE_WEIGHT_TABLE_3_3
	{0xA41B, 0x01,1}, 		// AE_RULE_AE_WEIGHT_TABLE_3_4
	{0xA41C, 0x01,1}, 		// AE_RULE_AE_WEIGHT_TABLE_4_0
	{0xA41D, 0x01,1}, 		// AE_RULE_AE_WEIGHT_TABLE_4_1
	{0xA41E, 0x01,1}, 		// AE_RULE_AE_WEIGHT_TABLE_4_2
	{0xA41F, 0x01,1}, 		// AE_RULE_AE_WEIGHT_TABLE_4_3
	{0xA420, 0x01,1}, 		// AE_RULE_AE_WEIGHT_TABLE_4_4
	{0xC874, 0x08,1}, 		//CAM_OUTPUT_Y_OFFSET
	{0xDC00, 0x28,1}, 		//SYSMGR_NEXT_STATE
	{0x0080, 0x8002,2}, 		//COMMAND_REGISTER
#if USE_INIT_REG_POLL_DELAY
	{0xFFFF, POLL_DELAY,2},      // delay
#endif
	//  POLL  COMMAND_REGISTER::HOST_COMMAND_1 =>  0x00
	{0x0982, 0x0001,2}, 		//ACCESS_CTL_STAT
	{0x098A, 0x6000,2}, 		//PHYSICAL_ADDRESS_ACCESS
	{0xE000, 0xC0F1,2},
	{0xE002, 0x0C3E,2},
	{0xE004, 0x08C0,2},
	{0xE006, 0xC1A3,2},
	{0xE008, 0x7508,2},
	{0xE00A, 0x90C3,2},
	{0xE00C, 0x71CF,2},
	{0xE00E, 0x0000,2},
	{0xE010, 0x0E5E,2},
	{0xE012, 0x7960,2},
	{0xE014, 0x90E1,2},
	{0xE016, 0x70CF,2},
	{0xE018, 0xFFFF,2},
	{0xE01A, 0xD078,2},
	{0xE01C, 0x9036,2},
	{0xE01E, 0x9019,2},
	{0xE020, 0x091F,2},
	{0xE022, 0x0003,2},
	{0xE024, 0xD915,2},
	{0xE026, 0x70CF,2},
	{0xE028, 0xFFFF,2},
	{0xE02A, 0xD2B8,2},
	{0xE02C, 0xA826,2},
	{0xE02E, 0xB5C3,2},
	{0xE030, 0xB5E1,2},
	{0xE032, 0xD800,2},
	{0xE034, 0x1C00,2},
	{0xE036, 0x3004,2},
	{0xE038, 0x0CB6,2},
	{0xE03A, 0x06E0,2},
	{0xE03C, 0x708B,2},
	{0xE03E, 0x0411,2},
	{0xE040, 0x08E0,2},
	{0xE042, 0xC0A3,2},
	{0xE044, 0xD900,2},
	{0xE046, 0xF00A,2},
	{0xE048, 0x70CF,2},
	{0xE04A, 0xFFFF,2},
	{0xE04C, 0xE060,2},
	{0xE04E, 0x7835,2},
	{0xE050, 0x8041,2},
	{0xE052, 0x8000,2},
	{0xE054, 0xE102,2},
	{0xE056, 0xA040,2},
	{0xE058, 0x09F1,2},
	{0xE05A, 0x8094,2},
	{0xE05C, 0x7FE0,2},
	{0xE05E, 0xD800,2},
	{0xE060, 0xFFFF,2},
	{0xE062, 0xDA84,2},
	{0xE064, 0xFFFF,2},
	{0xE066, 0xE000,2},
	{0x098E, 0x0000,2}, 		//LOGICAL_ADDRESS_ACCESS
	{0xE000, 0x0044,2}, 		//PATCHLDR_LOADER_ADDRESS
	{0xE002, 0x0005,2}, 		//PATCHLDR_PATCH_ID
	{0xE004, 0x51000000,4}, 		//PATCHLDR_FIRMWARE_ID
	{0x0080, 0xFFF0,2}, 		//COMMAND_REGISTER
#if USE_INIT_REG_POLL_DELAY
	{0xFFFF, POLL_DELAY,2},      // delay
#endif
	//  POLL  COMMAND_REGISTER::HOST_COMMAND_0 =>  0x00
	{0x0080, 0xFFF1,2}, 		//COMMAND_REGISTER
	//  POLL  COMMAND_REGISTER::HOST_COMMAND_0 =>  0x00
#if USE_INIT_REG_POLL_DELAY
	{0xFFFF, POLL_DELAY,2},      // delay
#endif
	{0x0982, 0x0001,2}, 		//ACCESS_CTL_STAT
	{0x098A, 0x6568,2}, 		//PHYSICAL_ADDRESS_ACCESS
	{0xE568, 0xC0F1,2},
	{0xE56A, 0xC5E1,2},
	{0xE56C, 0x75CF,2},
	{0xE56E, 0xFFFF,2},
	{0xE570, 0xD5C8,2},
	{0xE572, 0x0E42,2},
	{0xE574, 0x0820,2},
	{0xE576, 0x156C,2},
	{0xE578, 0x1100,2},
	{0xE57A, 0x71CF,2},
	{0xE57C, 0xFF00,2},
	{0xE57E, 0x41DC,2},
	{0xE580, 0x913B,2},
	{0xE582, 0x09FB,2},
	{0xE584, 0x821E,2},
	{0xE586, 0xE896,2},
	{0xE588, 0x9500,2},
	{0xE58A, 0xB882,2},
	{0xE58C, 0xB500,2},
	{0xE58E, 0x1568,2},
	{0xE590, 0x1100,2},
	{0xE592, 0xE001,2},
	{0xE594, 0x1D68,2},
	{0xE596, 0x1004,2},
	{0xE598, 0x1568,2},
	{0xE59A, 0x1100,2},
	{0xE59C, 0xE884,2},
	{0xE59E, 0xD801,2},
	{0xE5A0, 0x1D68,2},
	{0xE5A2, 0x1004,2},
	{0xE5A4, 0x208A,2},
	{0xE5A6, 0x0010,2},
	{0xE5A8, 0x0B1E,2},
	{0xE5AA, 0x06E0,2},
	{0xE5AC, 0xD901,2},
	{0xE5AE, 0xF004,2},
	{0xE5B0, 0x1D70,2},
	{0xE5B2, 0x1002,2},
	{0xE5B4, 0x06A9,2},
	{0xE5B6, 0x0880,2},
	{0xE5B8, 0xD900,2},
	{0xE5BA, 0xF00A,2},
	{0xE5BC, 0x70CF,2},
	{0xE5BE, 0xFFFF,2},
	{0xE5C0, 0xE5D4,2},
	{0xE5C2, 0x7835,2},
	{0xE5C4, 0x8041,2},
	{0xE5C6, 0x8000,2},
	{0xE5C8, 0xE102,2},
	{0xE5CA, 0xA040,2},
	{0xE5CC, 0x09F1,2},
	{0xE5CE, 0x8094,2},
	{0xE5D0, 0x7FE0,2},
	{0xE5D2, 0xD800,2},
	{0xE5D4, 0xFFFF,2},
	{0xE5D6, 0xDCA8,2},
	{0xE5D8, 0xFFFF,2},
	{0xE5DA, 0xE568,2},
	{0x098E, 0x0000,2}, 		//LOGICAL_ADDRESS_ACCESS
	{0xE000, 0x05B8,2}, 		//PATCHLDR_LOADER_ADDRESS
	{0xE002, 0x0205,2}, 		//PATCHLDR_PATCH_ID
	{0xE004, 0x51000000,4}, 		//PATCHLDR_FIRMWARE_ID
	{0x0080, 0xFFF0,2}, 		//COMMAND_REGISTER
#if USE_INIT_REG_POLL_DELAY
	{0xFFFF, POLL_DELAY,2},      // delay
#endif
	//  POLL  COMMAND_REGISTER::HOST_COMMAND_0 =>  0x00
	{0x0080, 0xFFF1,2}, 		//COMMAND_REGISTER
	//  POLL  COMMAND_REGISTER::HOST_COMMAND_0 =>  0x00
#if USE_INIT_REG_POLL_DELAY
	{0xFFFF, POLL_DELAY,2},      // delay
#endif
	{0x0982, 0x0001,2}, 		//ACCESS_CTL_STAT
	{0x098A, 0x65DC,2}, 		//PHYSICAL_ADDRESS_ACCESS
	{0xE5DC, 0xC0F1,2},
	{0xE5DE, 0x0E62,2},
	{0xE5E0, 0x0880,2},
	{0xE5E2, 0x75CF,2},
	{0xE5E4, 0xFFFF,2},
	{0xE5E6, 0xD78C,2},
	{0xE5E8, 0x85EF,2},
	{0xE5EA, 0x95C8,2},
	{0xE5EC, 0x70E9,2},
	{0xE5EE, 0x0E86,2},
	{0xE5F0, 0x08A0,2},
	{0xE5F2, 0x71C9,2},
	{0xE5F4, 0xE084,2},
	{0xE5F6, 0x20CA,2},
	{0xE5F8, 0x0125,2},
	{0xE5FA, 0xF783,2},
	{0xE5FC, 0x7810,2},
	{0xE5FE, 0x7A30,2},
	{0xE600, 0xB52E,2},
	{0xE602, 0xD980,2},
	{0xE604, 0xA52E,2},
	{0xE606, 0xD900,2},
	{0xE608, 0xB52B,2},
	{0xE60A, 0x9522,2},
	{0xE60C, 0xB50F,2},
	{0xE60E, 0x093D,2},
	{0xE610, 0x015E,2},
	{0xE612, 0x208C,2},
	{0xE614, 0x8002,2},
	{0xE616, 0xF65A,2},
	{0xE618, 0x73CF,2},
	{0xE61A, 0xFFFF,2},
	{0xE61C, 0xD03C,2},
	{0xE61E, 0x9327,2},
	{0xE620, 0x0A15,2},
	{0xE622, 0x0043,2},
	{0xE624, 0x0811,2},
	{0xE626, 0x0152,2},
	{0xE628, 0x6829,2},
	{0xE62A, 0x7830,2},
	{0xE62C, 0xB52F,2},
	{0xE62E, 0x62D9,2},
	{0xE630, 0x7A30,2},
	{0xE632, 0xB52E,2},
	{0xE634, 0x9328,2},
	{0xE636, 0x0915,2},
	{0xE638, 0x0083,2},
	{0xE63A, 0xB52E,2},
	{0xE63C, 0x78CC,2},
	{0xE63E, 0x7127,2},
	{0xE640, 0x0E32,2},
	{0xE642, 0x08A0,2},
	{0xE644, 0x6F17,2},
	{0xE646, 0xA50E,2},
	{0xE648, 0x0605,2},
	{0xE64A, 0x0880,2},
	{0xE64C, 0xD900,2},
	{0xE64E, 0xF00A,2},
	{0xE650, 0x70CF,2},
	{0xE652, 0xFFFF,2},
	{0xE654, 0xE668,2},
	{0xE656, 0x7835,2},
	{0xE658, 0x8041,2},
	{0xE65A, 0x8000,2},
	{0xE65C, 0xE102,2},
	{0xE65E, 0xA040,2},
	{0xE660, 0x09F1,2},
	{0xE662, 0x8094,2},
	{0xE664, 0x7FE0,2},
	{0xE666, 0xD800,2},
	{0xE668, 0xFFFF,2},
	{0xE66A, 0xDD18,2},
	{0xE66C, 0xFFFF,2},
	{0xE66E, 0xE5DC,2},
	{0x098E, 0x0000,2}, 		//LOGICAL_ADDRESS_ACCESS
	{0xE000, 0x064C,2}, 		//PATCHLDR_LOADER_ADDRESS
	{0xE002, 0x0305,2}, 		//PATCHLDR_PATCH_ID
	{0xE004, 0x51000000,4}, 		//PATCHLDR_FIRMWARE_ID
	{0x0080, 0xFFF0,2}, 		//COMMAND_REGISTER
#if USE_INIT_REG_POLL_DELAY
	{0xFFFF, POLL_DELAY,2},      // delay
#endif
	//  POLL  COMMAND_REGISTER::HOST_COMMAND_0 =>  0x00
	{0x0080, 0xFFF1,2}, 		//COMMAND_REGISTER
	//  POLL  COMMAND_REGISTER::HOST_COMMAND_0 =>  0x00
#if USE_INIT_REG_POLL_DELAY
	{0xFFFF, POLL_DELAY,2},      // delay
#endif
	{0x0982, 0x0001,2}, 		//ACCESS_CTL_STAT
	{0x098A, 0x6670,2}, 		//PHYSICAL_ADDRESS_ACCESS
	{0xE670, 0xC0F1,2},
	{0xE672, 0x0DCE,2},
	{0xE674, 0x08A0,2},
	{0xE676, 0x2256,2},
	{0xE678, 0x0802,2},
	{0xE67A, 0xC1A3,2},
	{0xE67C, 0x73CF,2},
	{0xE67E, 0xFFFF,2},
	{0xE680, 0xD2F0,2},
	{0xE682, 0x7F50,2},
	{0xE684, 0x220A,2},
	{0xE686, 0x1F80,2},
	{0xE688, 0xFFFF,2},
	{0xE68A, 0xD825,2},
	{0xE68C, 0x72CF,2},
	{0xE68E, 0xFFFF,2},
	{0xE690, 0xD1FC,2},
	{0xE692, 0xC040,2},
	{0xE694, 0x080D,2},
	{0xE696, 0x0281,2},
	{0xE698, 0x8B00,2},
	{0xE69A, 0xC041,2},
	{0xE69C, 0x8AA8,2},
	{0xE69E, 0xF005,2},
	{0xE6A0, 0x8B01,2},
	{0xE6A2, 0x8AA9,2},
	{0xE6A4, 0xC041,2},
	{0xE6A6, 0xD819,2},
	{0xE6A8, 0x0DDA,2},
	{0xE6AA, 0x08A0,2},
	{0xE6AC, 0xB80A,2},
	{0xE6AE, 0x7810,2},
	{0xE6B0, 0xC042,2},
	{0xE6B2, 0xDE00,2},
	{0xE6B4, 0xF006,2},
	{0xE6B6, 0xD800,2},
	{0xE6B8, 0xC100,2},
	{0xE6BA, 0x61D9,2},
	{0xE6BC, 0xE601,2},
	{0xE6BE, 0xA900,2},
	{0xE6C0, 0x0EB3,2},
	{0xE6C2, 0x14D5,2},
	{0xE6C4, 0x70CF,2},
	{0xE6C6, 0xFFFF,2},
	{0xE6C8, 0xE794,2},
	{0xE6CA, 0x8000,2},
	{0xE6CC, 0x60CB,2},
	{0xE6CE, 0x2302,2},
	{0xE6D0, 0x0340,2},
	{0xE6D2, 0x780E,2},
	{0xE6D4, 0x0B2F,2},
	{0xE6D6, 0x0363,2},
	{0xE6D8, 0x2D40,2},
	{0xE6DA, 0x1202,2},
	{0xE6DC, 0x71E9,2},
	{0xE6DE, 0x2182,2},
	{0xE6E0, 0x0008,2},
	{0xE6E2, 0x790C,2},
	{0xE6E4, 0x7227,2},
	{0xE6E6, 0x7A6C,2},
	{0xE6E8, 0xC201,2},
	{0xE6EA, 0x712F,2},
	{0xE6EC, 0x784C,2},
	{0xE6EE, 0x2904,2},
	{0xE6F0, 0x703E,2},
	{0xE6F2, 0x7127,2},
	{0xE6F4, 0x796C,2},
	{0xE6F6, 0x702F,2},
	{0xE6F8, 0x0D8A,2},
	{0xE6FA, 0x08A0,2},
	{0xE6FC, 0x71A9,2},
	{0xE6FE, 0x71A9,2},
	{0xE700, 0xF012,2},
	{0xE702, 0x250E,2},
	{0xE704, 0x1F81,2},
	{0xE706, 0x0000,2},
	{0xE708, 0x0100,2},
	{0xE70A, 0x792E,2},
	{0xE70C, 0x270E,2},
	{0xE70E, 0x1F83,2},
	{0xE710, 0x0000,2},
	{0xE712, 0x0100,2},
	{0xE714, 0x7B0C,2},
	{0xE716, 0x732F,2},
	{0xE718, 0x79EC,2},
	{0xE71A, 0x7327,2},
	{0xE71C, 0x7B0C,2},
	{0xE71E, 0x702F,2},
	{0xE720, 0x7A2C,2},
	{0xE722, 0x7027,2},
	{0xE724, 0x0D5E,2},
	{0xE726, 0x0880,2},
	{0xE728, 0xE080,2},
	{0xE72A, 0x0040,2},
	{0xE72C, 0x002C,2},
	{0xE72E, 0x20CA,2},
	{0xE730, 0x002C,2},
	{0xE732, 0x0B82,2},
	{0xE734, 0x0320,2},
	{0xE736, 0xD908,2},
	{0xE738, 0xC102,2},
	{0xE73A, 0x2099,2},
	{0xE73C, 0x0008,2},
	{0xE73E, 0xDC80,2},
	{0xE740, 0x782C,2},
	{0xE742, 0x210E,2},
	{0xE744, 0xF300,2},
	{0xE746, 0xDCFF,2},
	{0xE748, 0x20C0,2},
	{0xE74A, 0x0304,2},
	{0xE74C, 0xB848,2},
	{0xE74E, 0xD908,2},
	{0xE750, 0x0AF6,2},
	{0xE752, 0x0320,2},
	{0xE754, 0x7228,2},
	{0xE756, 0x7108,2},
	{0xE758, 0xB83F,2},
	{0xE75A, 0x6038,2},
	{0xE75C, 0x781C,2},
	{0xE75E, 0x70C7,2},
	{0xE760, 0x0001,2},
	{0xE762, 0x0000,2},
	{0xE764, 0x0D1E,2},
	{0xE766, 0x0880,2},
	{0xE768, 0x084F,2},
	{0xE76A, 0x8012,2},
	{0xE76C, 0x2089,2},
	{0xE76E, 0x0FC3,2},
	{0xE770, 0xF1A4,2},
	{0xE772, 0x04DD,2},
	{0xE774, 0x08A0,2},
	{0xE776, 0xC0A3,2},
	{0xE778, 0xD900,2},
	{0xE77A, 0xF00A,2},
	{0xE77C, 0x70CF,2},
	{0xE77E, 0xFFFF,2},
	{0xE780, 0xE798,2},
	{0xE782, 0x7835,2},
	{0xE784, 0x8041,2},
	{0xE786, 0x8000,2},
	{0xE788, 0xE102,2},
	{0xE78A, 0xA040,2},
	{0xE78C, 0x09F1,2},
	{0xE78E, 0x8094,2},
	{0xE790, 0x7FE0,2},
	{0xE792, 0xD800,2},
	{0xE794, 0x0001,2},
	{0xE796, 0x03A8,2},
	{0xE798, 0xFFFF,2},
	{0xE79A, 0xDD68,2},
	{0xE79C, 0xFFFF,2},
	{0xE79E, 0xE670,2},
	{0x098E, 0x0000,2}, 		//LOGICAL_ADDRESS_ACCESS
	{0xE000, 0x0778,2}, 		//PATCHLDR_LOADER_ADDRESS
	{0xE002, 0x0405,2}, 		//PATCHLDR_PATCH_ID
	{0xE004, 0x51000000,4}, 		//PATCHLDR_FIRMWARE_ID
	{0x0080, 0xFFF0,2}, 		//COMMAND_REGISTER
	//  POLL  COMMAND_REGISTER::HOST_COMMAND_0 =>  0x00
#if USE_INIT_REG_POLL_DELAY
	{0xFFFF, POLL_DELAY,2},      // delay
#endif
	{0x0080, 0xFFF1,2}, 		//COMMAND_REGISTER
	//  POLL  COMMAND_REGISTER::HOST_COMMAND_0 =>  0x00
#if USE_INIT_REG_POLL_DELAY
	{0xFFFF, POLL_DELAY,2},      // delay
#endif
	//LOAD=Patch 0805; FEATURE RECOMMENDED ; blacklevel patch 
	{0xBC02, 0x0013,2}, 		//LL_MODE
	{0xC960, 0x0003,2}, 		//CAM_LL_LLMODE
	{0xBC08, 0x00,1}, 		//LL_GAMMA_SELECT
	{0xC9C4, 0x00e3,2}, 		//CAM_LL_GAMMA
	{0xC9C6, 0x52,1}, 		//CAM_LL_START_CONTRAST_GRADIENT
	{0xC9C7, 0x64,1}, 		//CAM_LL_STOP_CONTRAST_GRADIENT
	{0xC9C8, 0x38,1}, 		//CAM_LL_START_CONTRAST_LUMA_PERCENTAGE
	{0xC9C9, 0x11,1}, 		//CAM_LL_STOP_CONTRAST_LUMA_PERCENTAGE
	{0xE400, 0x00,1}, 		//PATCHVARS_START_ORIGIN_GRADIENT
	{0xE401, 0x00,1}, 		//PATCHVARS_STOP_ORIGIN_GRADIENT
	{0xC9C0, 0x0266,2}, 		//CAM_LL_START_CONTRAST_BM
	{0xC9C2, 0x029A,2}, 		//CAM_LL_STOP_CONTRAST_BM
	{0x3C40, 0x7820,2},	// Non-continuous mode 
	{0xC860, 0x00,1},   // CAM_CROP_CROPMODE
	{0xA808, 0x004B,2}, 		//AE_TRACK_GATE

	//mcnex 121218
	{0xA807, 0x1B, 1}, 	// AE_TRACK_AE_TRACKING_SPEED
	{0xC877, 0xFE, 1}, 	//CAM_HUE_ANGLE
};
#endif

struct as0260_reg as0260_init_regs[] =
{
/*  POLL  MCU_BOOT_MODE::MCU_INFO_CODE =>  0x05, 0x0F, 0x24, 0x30 (4 reads) */
	{0x098E, 0x3E00,2},      // LOGICAL_ADDRESS_ACCESS [CAM_SYSCTL_PLL_ENABLE]

    {0x3E00, 0x042D,2},      // RESERVED_CORE_3E00
    {0x3E02, 0x39FF,2},      // RESERVED_CORE_3E02
    {0x3E04, 0x49FF,2},      // RESERVED_CORE_3E04
    {0x3E06, 0xFFFF,2},      // RESERVED_CORE_3E06
    {0x3E08, 0x8071,2},      // RESERVED_CORE_3E08
    {0x3E0A, 0x7211,2},      // RESERVED_CORE_3E0A
    {0x3E0C, 0xE040,2},      // RESERVED_CORE_3E0C
    {0x3E0E, 0xA840,2},      // RESERVED_CORE_3E0E

    {0x3E10, 0x4100,2},      // RESERVED_CORE_3E10
    {0x3E12, 0x1846,2},      // RESERVED_CORE_3E12
    {0x3E14, 0xA547,2},      // RESERVED_CORE_3E14
    {0x3E16, 0xAD57,2},      // RESERVED_CORE_3E16
    {0x3E18, 0x8149,2},      // RESERVED_CORE_3E18
    {0x3E1A, 0x9D49,2},      // RESERVED_CORE_3E1A
    {0x3E1C, 0x9F46,2},      // RESERVED_CORE_3E1C
    {0x3E1E, 0x8000,2},      // RESERVED_CORE_3E1E

    {0x3E20, 0x1842,2},      // RESERVED_CORE_3E20
    {0x3E22, 0x4180,2},      // RESERVED_CORE_3E22
    {0x3E24, 0x0018,2},      // RESERVED_CORE_3E24
    {0x3E26, 0x8149,2},      // RESERVED_CORE_3E26
    {0x3E28, 0x9C49,2},      // RESERVED_CORE_3E28
    {0x3E2A, 0x9347,2},      // RESERVED_CORE_3E2A
    {0x3E2C, 0x804D,2},      // RESERVED_CORE_3E2C
    {0x3E2E, 0x804A,2},      // RESERVED_CORE_3E2E

    {0x3E30, 0x100C,2},      // RESERVED_CORE_3E30
    {0x3E32, 0x8000,2},      // RESERVED_CORE_3E32
    {0x3E34, 0x1841,2},      // RESERVED_CORE_3E34
    {0x3E36, 0x4280,2},      // RESERVED_CORE_3E36
    {0x3E38, 0x0018,2},      // RESERVED_CORE_3E38
    {0x3E3A, 0x9710,2},      // RESERVED_CORE_3E3A
    {0x3E3C, 0x0C80,2},      // RESERVED_CORE_3E3C
    {0x3E3E, 0x4DA2,2},      // RESERVED_CORE_3E3E

    {0x3E40, 0x4BA0,2},      // RESERVED_CORE_3E40
    {0x3E42, 0x4A00,2},      // RESERVED_CORE_3E42
    {0x3E44, 0x1880,2},      // RESERVED_CORE_3E44
    {0x3E46, 0x4241,2},      // RESERVED_CORE_3E46
    {0x3E48, 0x0018,2},      // RESERVED_CORE_3E48
    {0x3E4A, 0xB54B,2},      // RESERVED_CORE_3E4A
    {0x3E4C, 0x1C00,2},      // RESERVED_CORE_3E4C
    {0x3E4E, 0x8000,2},      // RESERVED_CORE_3E4E

    {0x3E50, 0x1C10,2},      // RESERVED_CORE_3E50
    {0x3E52, 0x6081,2},      // RESERVED_CORE_3E52
    {0x3E54, 0x1580,2},      // RESERVED_CORE_3E54
    {0x3E56, 0x7C09,2},      // RESERVED_CORE_3E56
    {0x3E58, 0x7000,2},      // RESERVED_CORE_3E58
    {0x3E5A, 0x8082,2},      // RESERVED_CORE_3E5A
    {0x3E5C, 0x7281,2},      // RESERVED_CORE_3E5C
    {0x3E5E, 0x4C40,2},      // RESERVED_CORE_3E5E

	{0x3E60, 0x8E4D,2},      // RESERVED_CORE_3E60
    {0x3E62, 0x8110,2},      // RESERVED_CORE_3E62
    {0x3E64, 0x0CAF,2},      // RESERVED_CORE_3E64
    {0x3E66, 0x4D80,2},      // RESERVED_CORE_3E66
    {0x3E68, 0x100C,2},      // RESERVED_CORE_3E68
    {0x3E6A, 0x8440,2},      // RESERVED_CORE_3E6A
    {0x3E6C, 0x4C81,2},      // RESERVED_CORE_3E6C
    {0x3E6E, 0x7C5B,2},      // RESERVED_CORE_3E6E
    {0x3E70, 0x7000,2},      // RESERVED_CORE_3E70

    {0x3E72, 0x8054,2},      // RESERVED_CORE_3E72
    {0x3E74, 0x924C,2},      // RESERVED_CORE_3E74
    {0x3E76, 0x4078,2},      // RESERVED_CORE_3E76
    {0x3E78, 0x4D4F,2},      // RESERVED_CORE_3E78
    {0x3E7A, 0x4E98,2},      // RESERVED_CORE_3E7A
    {0x3E7C, 0x504E,2},      // RESERVED_CORE_3E7C
    {0x3E7E, 0x4F97,2},      // RESERVED_CORE_3E7E

    {0x3E80, 0x4F4E,2},      // RESERVED_CORE_3E80
    {0x3E82, 0x507C,2},      // RESERVED_CORE_3E82
    {0x3E84, 0x7B8D,2},      // RESERVED_CORE_3E84
    {0x3E86, 0x4D88,2},      // RESERVED_CORE_3E86
    {0x3E88, 0x4E10,2},      // RESERVED_CORE_3E88
    {0x3E8A, 0x0940,2},      // RESERVED_CORE_3E8A
    {0x3E8C, 0x8879,2},      // RESERVED_CORE_3E8C
    {0x3E8E, 0x5481,2},      // RESERVED_CORE_3E8E

    {0x3E90, 0x7000,2},      // RESERVED_CORE_3E90
    {0x3E92, 0x8082,2},      // RESERVED_CORE_3E92
    {0x3E94, 0x7281,2},      // RESERVED_CORE_3E94
    {0x3E96, 0x4C40,2},      // RESERVED_CORE_3E96
    {0x3E98, 0x8E4D,2},      // RESERVED_CORE_3E98
    {0x3E9A, 0x8110,2},      // RESERVED_CORE_3E9A
    {0x3E9C, 0x0CAF,2},      // RESERVED_CORE_3E9C
    {0x3E9E, 0x4D80,2},      // RESERVED_CORE_3E9E

    {0x3EA0, 0x100C,2},      // RESERVED_CORE_3EA0
    {0x3EA2, 0x8440,2},      // RESERVED_CORE_3EA2
    {0x3EA4, 0x4C81,2},      // RESERVED_CORE_3EA4
    {0x3EA6, 0x7C93,2},      // RESERVED_CORE_3EA6
    {0x3EA8, 0x7000,2},      // RESERVED_CORE_3EA8
    {0x3EAA, 0x0000,2},      // RESERVED_CORE_3EAA
    {0x3EAC, 0x0000,2},      // RESERVED_CORE_3EAC
    {0x3EAE, 0x0000,2},      // RESERVED_CORE_3EAE

	{0x3EB0, 0x0000,2},      // RESERVED_CORE_3EB0
    {0x3EB2, 0x0000,2},      // RESERVED_CORE_3EB2
    {0x3EB4, 0x0000,2},      // RESERVED_CORE_3EB4
    {0x3EB6, 0x0000,2},      // RESERVED_CORE_3EB6
    {0x3EB8, 0x0000,2},      // RESERVED_CORE_3EB8
    {0x3EBA, 0x0000,2},      // RESERVED_CORE_3EBA
    {0x3EBC, 0x0000,2},      // RESERVED_CORE_3EBC
    {0x3EBE, 0x0000,2},      // RESERVED_CORE_3EBE

    {0x3EC0, 0x0000,2},      // RESERVED_CORE_3EC0
    {0x3EC2, 0x0000,2},      // RESERVED_CORE_3EC2
    {0x3EC4, 0x0000,2},      // RESERVED_CORE_3EC4
    {0x3EC6, 0x0000,2},      // RESERVED_CORE_3EC6
    {0x3EC8, 0x0000,2},      // RESERVED_CORE_3EC8
    {0x3ECA, 0x0000,2},      // RESERVED_CORE_3ECA

    {0x30B2, 0xC000,2},      // RESERVED_CORE_30B2
    {0x30D4, 0x9400,2},      // RESERVED_CORE_30D4
    {0x31C0, 0x0000,2},      // RESERVED_CORE_31C0
    {0x316A, 0x8200,2},      // RESERVED_CORE_316A
    {0x316C, 0x8200,2},      // RESERVED_CORE_316C
    {0x3EFE, 0x2808,2},      // RESERVED_CORE_3EFE
    {0x3EFC, 0x2868,2},      // RESERVED_CORE_3EFC
    {0x3ED2, 0xD165,2},      // RESERVED_CORE_3ED2
    {0x3EF2, 0xD165,2},      // RESERVED_CORE_3EF2
    {0x3ED8, 0x7F1A,2},      // RESERVED_CORE_3ED8
    {0x3EDA, 0x2828,2},      // RESERVED_CORE_3EDA
    {0x3EE2, 0x0058,2},      // RESERVED_CORE_3EE2
//	{0xFFFF,INIT_DELAY,1}, // add
    {0x3EFE, 0x280A,2},      // RESERVED_CORE_3EFE
    {0x3170, 0x000A,2},      // RESERVED_CORE_3170
    {0x3174, 0x8060,2},      // RESERVED_CORE_3174
    {0x317A, 0x000A,2},      // RESERVED_CORE_317A
    {0x3ECC, 0x22B0,2},      // RESERVED_CORE_3ECC
    {0x098E, 0x0000,2},      // LOGICAL_ADDRESS_ACCESS

    {0xD1BC, 0x0150,2},      // PGA_TABLE_A_DATA_0
    {0xD1BE, 0xC2AB,2},      // PGA_TABLE_A_DATA_1
    {0xD1C0, 0x15D2,2},      // PGA_TABLE_A_DATA_2
    {0xD1C2, 0xA0EA,2},      // PGA_TABLE_A_DATA_3
    {0xD1C4, 0x12F1,2},      // PGA_TABLE_A_DATA_4
    {0xD1C6, 0x0250,2},      // PGA_TABLE_A_DATA_5
    {0xD1C8, 0x2B2F,2},      // PGA_TABLE_A_DATA_6
    {0xD1CA, 0x1153,2},      // PGA_TABLE_A_DATA_7
    {0xD1CC, 0x22B0,2},      // PGA_TABLE_A_DATA_8
    {0xD1CE, 0x89F1,2},      // PGA_TABLE_A_DATA_9
    {0xD1D0, 0x0250,2},      // PGA_TABLE_A_DATA_10
    {0xD1D2, 0x934D,2},      // PGA_TABLE_A_DATA_11
    {0xD1D4, 0x0AB1,2},      // PGA_TABLE_A_DATA_12
    {0xD1D6, 0x916E,2},      // PGA_TABLE_A_DATA_13
    {0xD1D8, 0x6D51,2},      // PGA_TABLE_A_DATA_14
    {0xD1DA, 0x0730,2},      // PGA_TABLE_A_DATA_15
    {0xD1DC, 0x724E,2},      // PGA_TABLE_A_DATA_16
    {0xD1DE, 0x4D92,2},      // PGA_TABLE_A_DATA_17
    {0xD1E0, 0x0C0F,2},      // PGA_TABLE_A_DATA_18
    {0xD1E2, 0x3EEC,2},      // PGA_TABLE_A_DATA_19
    {0xD1E4, 0xED4C,2},      // PGA_TABLE_A_DATA_20
    {0xD1E6, 0xBAAF,2},      // PGA_TABLE_A_DATA_21
    {0xD1E8, 0x4B30,2},      // PGA_TABLE_A_DATA_22
    {0xD1EA, 0x0D0F,2},      // PGA_TABLE_A_DATA_23
    {0xD1EC, 0xC8B2,2},      // PGA_TABLE_A_DATA_24
    {0xD1EE, 0x868C,2},      // PGA_TABLE_A_DATA_25
    {0xD1F0, 0x0770,2},      // PGA_TABLE_A_DATA_26
    {0xD1F2, 0x1171,2},      // PGA_TABLE_A_DATA_27
    {0xD1F4, 0xFCAF,2},      // PGA_TABLE_A_DATA_28
    {0xD1F6, 0xE6B2,2},      // PGA_TABLE_A_DATA_29
    {0xD1F8, 0xED0C,2},      // PGA_TABLE_A_DATA_30
    {0xD1FA, 0x5D2F,2},      // PGA_TABLE_A_DATA_31
    {0xD1FC, 0x1291,2},      // PGA_TABLE_A_DATA_32
    {0xD1FE, 0xCDF1,2},      // PGA_TABLE_A_DATA_33
    {0xD200, 0x8F93,2},      // PGA_TABLE_A_DATA_34
    {0xD202, 0x966E,2},      // PGA_TABLE_A_DATA_35
    {0xD204, 0xBB0F,2},      // PGA_TABLE_A_DATA_36
    {0xD206, 0x1BF1,2},      // PGA_TABLE_A_DATA_37
    {0xD208, 0x9130,2},      // PGA_TABLE_A_DATA_38
    {0xD20A, 0x8BB3,2},      // PGA_TABLE_A_DATA_39
    {0xD20C, 0x26B2,2},      // PGA_TABLE_A_DATA_40
    {0xD20E, 0x2AEE,2},      // PGA_TABLE_A_DATA_41
    {0xD210, 0x2173,2},      // PGA_TABLE_A_DATA_42
    {0xD212, 0x1693,2},      // PGA_TABLE_A_DATA_43
    {0xD214, 0x8496,2},      // PGA_TABLE_A_DATA_44
    {0xD216, 0x0B13,2},      // PGA_TABLE_A_DATA_45
    {0xD218, 0x4811,2},      // PGA_TABLE_A_DATA_46
    {0xD21A, 0x1B94,2},      // PGA_TABLE_A_DATA_47
    {0xD21C, 0xBE92,2},      // PGA_TABLE_A_DATA_48
    {0xD21E, 0xE4B6,2},      // PGA_TABLE_A_DATA_49
    {0xD220, 0x3031,2},      // PGA_TABLE_A_DATA_50
    {0xD222, 0xA50E,2},      // PGA_TABLE_A_DATA_51
    {0xD224, 0x0E93,2},      // PGA_TABLE_A_DATA_52
    {0xD226, 0x2494,2},      // PGA_TABLE_A_DATA_53
    {0xD228, 0x9AB0,2},      // PGA_TABLE_A_DATA_54
    {0xD22A, 0x01F2,2},      // PGA_TABLE_A_DATA_55
    {0xD22C, 0x7A8E,2},      // PGA_TABLE_A_DATA_56
    {0xD22E, 0x0FD3,2},      // PGA_TABLE_A_DATA_57
    {0xD230, 0x1ED2,2},      // PGA_TABLE_A_DATA_58
    {0xD232, 0xB6B5,2},      // PGA_TABLE_A_DATA_59
    {0xD234, 0x0151,2},      // PGA_TABLE_A_DATA_60
    {0xD236, 0xF651,2},      // PGA_TABLE_A_DATA_61
    {0xD238, 0xA3F4,2},      // PGA_TABLE_A_DATA_62
    {0xD23A, 0x6734,2},      // PGA_TABLE_A_DATA_63
    {0xD23C, 0x0E16,2},      // PGA_TABLE_A_DATA_64
    {0xD23E, 0x1471,2},      // PGA_TABLE_A_DATA_65
    {0xD240, 0xB670,2},      // PGA_TABLE_A_DATA_66
    {0xD242, 0xA034,2},      // PGA_TABLE_A_DATA_67
    {0xD244, 0x6ACB,2},      // PGA_TABLE_A_DATA_68
    {0xD246, 0x6E34,2},      // PGA_TABLE_A_DATA_69
    {0xD248, 0xC950,2},      // PGA_TABLE_A_DATA_70
    {0xD24A, 0xE7B0,2},      // PGA_TABLE_A_DATA_71
    {0xD24C, 0x9A73,2},      // PGA_TABLE_A_DATA_72
    {0xD24E, 0x0074,2},      // PGA_TABLE_A_DATA_73
    {0xD250, 0x2556,2},      // PGA_TABLE_A_DATA_74
    {0xD252, 0x586E,2},      // PGA_TABLE_A_DATA_75
    {0xD254, 0xAED1,2},      // PGA_TABLE_A_DATA_76
    {0xD256, 0xF533,2},      // PGA_TABLE_A_DATA_77
    {0xD258, 0x1BF5,2},      // PGA_TABLE_A_DATA_78
    {0xD25A, 0x7756,2},      // PGA_TABLE_A_DATA_79
    {0xD25C, 0x17D4,2},      // PGA_TABLE_A_DATA_80
    {0xD25E, 0x88F4,2},      // PGA_TABLE_A_DATA_81
    {0xD260, 0xE538,2},      // PGA_TABLE_A_DATA_82
    {0xD262, 0x3AB8,2},      // PGA_TABLE_A_DATA_83
    {0xD264, 0x69FB,2},      // PGA_TABLE_A_DATA_84
    {0xD266, 0x10D4,2},      // PGA_TABLE_A_DATA_85
    {0xD268, 0xBED5,2},      // PGA_TABLE_A_DATA_86
    {0xD26A, 0xAD79,2},      // PGA_TABLE_A_DATA_87
    {0xD26C, 0x47B8,2},      // PGA_TABLE_A_DATA_88
    {0xD26E, 0x16DC,2},      // PGA_TABLE_A_DATA_89
    {0xD270, 0x3C73,2},      // PGA_TABLE_A_DATA_90
    {0xD272, 0x3F12,2},      // PGA_TABLE_A_DATA_91
    {0xD274, 0x8CB8,2},      // PGA_TABLE_A_DATA_92
    {0xD276, 0x5EF6,2},      // PGA_TABLE_A_DATA_93
    {0xD278, 0x0E5B,2},      // PGA_TABLE_A_DATA_94
    {0xD27A, 0x4514,2},      // PGA_TABLE_A_DATA_95
    {0xD27C, 0x83B4,2},      // PGA_TABLE_A_DATA_96
    {0xD27E, 0xEE18,2},      // PGA_TABLE_A_DATA_97
    {0xD280, 0x75B7,2},      // PGA_TABLE_A_DATA_98
    {0xD282, 0x675B,2},      // PGA_TABLE_A_DATA_99
    
    {0xD286, 0x03EC,2},      // PGA_TABLE_A_CENTER_COLUMN
    {0xD284, 0x020C,2},      // PGA_TABLE_A_CENTER_ROW
    {0xC9F4, 0x0AF0,2},      // CAM_PGA_L_CONFIG_COLOUR_TEMP
    {0xC9F6, 0x8000,2},      // CAM_PGA_L_CONFIG_GREEN_RED_Q14
    {0xC9F8, 0x8000,2},      // CAM_PGA_L_CONFIG_RED_Q14
    {0xC9FA, 0x8000,2},      // CAM_PGA_L_CONFIG_GREEN_BLUE_Q14
    {0xC9FC, 0x8000,2},      // CAM_PGA_L_CONFIG_BLUE_Q14
    {0xD0F0, 0x0130,2},      // PGA_TABLE_CWF_DATA_0
    {0xD0F2, 0x42CA,2},      // PGA_TABLE_CWF_DATA_1
    {0xD0F4, 0x21F2,2},      // PGA_TABLE_CWF_DATA_2
    {0xD0F6, 0x3E2B,2},      // PGA_TABLE_CWF_DATA_3
    {0xD0F8, 0x0010,2},      // PGA_TABLE_CWF_DATA_4
    {0xD0FA, 0x0230,2},      // PGA_TABLE_CWF_DATA_5
    {0xD0FC, 0x7E4E,2},      // PGA_TABLE_CWF_DATA_6
    {0xD0FE, 0x3E52,2},      // PGA_TABLE_CWF_DATA_7
    {0xD100, 0x45F0,2},      // PGA_TABLE_CWF_DATA_8
    {0xD102, 0x812F,2},      // PGA_TABLE_CWF_DATA_9
    {0xD104, 0x01F0,2},      // PGA_TABLE_CWF_DATA_10
    {0xD106, 0xE2AC,2},      // PGA_TABLE_CWF_DATA_11
    {0xD108, 0x1491,2},      // PGA_TABLE_CWF_DATA_12
    {0xD10A, 0xC2CB,2},      // PGA_TABLE_CWF_DATA_13
    {0xD10C, 0x4DB1,2},      // PGA_TABLE_CWF_DATA_14
    {0xD10E, 0x0310,2},      // PGA_TABLE_CWF_DATA_15
    {0xD110, 0x724E,2},      // PGA_TABLE_CWF_DATA_16
    {0xD112, 0x3DB2,2},      // PGA_TABLE_CWF_DATA_17
    {0xD114, 0x438F,2},      // PGA_TABLE_CWF_DATA_18
    {0xD116, 0x69CE,2},      // PGA_TABLE_CWF_DATA_19
    {0xD118, 0xAE8D,2},      // PGA_TABLE_CWF_DATA_20
    {0xD11A, 0xB80F,2},      // PGA_TABLE_CWF_DATA_21
    {0xD11C, 0x2E10,2},      // PGA_TABLE_CWF_DATA_22
    {0xD11E, 0x210E,2},      // PGA_TABLE_CWF_DATA_23
    {0xD120, 0xAA72,2},      // PGA_TABLE_CWF_DATA_24
    {0xD122, 0xD5AC,2},      // PGA_TABLE_CWF_DATA_25
    {0xD124, 0x5BAF,2},      // PGA_TABLE_CWF_DATA_26
    {0xD126, 0x0D51,2},      // PGA_TABLE_CWF_DATA_27
    {0xD128, 0xAA70,2},      // PGA_TABLE_CWF_DATA_28
    {0xD12A, 0x9D53,2},      // PGA_TABLE_CWF_DATA_29
    {0xD12C, 0xB98D,2},      // PGA_TABLE_CWF_DATA_30
    {0xD12E, 0x74AF,2},      // PGA_TABLE_CWF_DATA_31
    {0xD130, 0x03F1,2},      // PGA_TABLE_CWF_DATA_32
    {0xD132, 0xBEF1,2},      // PGA_TABLE_CWF_DATA_33
    {0xD134, 0xD072,2},      // PGA_TABLE_CWF_DATA_34
    {0xD136, 0x9C8E,2},      // PGA_TABLE_CWF_DATA_35
    {0xD138, 0xC6EF,2},      // PGA_TABLE_CWF_DATA_36
    {0xD13A, 0x08F1,2},      // PGA_TABLE_CWF_DATA_37
    {0xD13C, 0xA64C,2},      // PGA_TABLE_CWF_DATA_38
    {0xD13E, 0xCB92,2},      // PGA_TABLE_CWF_DATA_39
    {0xD140, 0x1672,2},      // PGA_TABLE_CWF_DATA_40
    {0xD142, 0x692D,2},      // PGA_TABLE_CWF_DATA_41
    {0xD144, 0x68D2,2},      // PGA_TABLE_CWF_DATA_42
    {0xD146, 0x2DF3,2},      // PGA_TABLE_CWF_DATA_43
    {0xD148, 0xC135,2},      // PGA_TABLE_CWF_DATA_44
    {0xD14A, 0x4692,2},      // PGA_TABLE_CWF_DATA_45
    {0xD14C, 0x38D1,2},      // PGA_TABLE_CWF_DATA_46
    {0xD14E, 0x2932,2},      // PGA_TABLE_CWF_DATA_47
    {0xD150, 0x8013,2},      // PGA_TABLE_CWF_DATA_48
    {0xD152, 0xE714,2},      // PGA_TABLE_CWF_DATA_49
    {0xD154, 0x3D11,2},      // PGA_TABLE_CWF_DATA_50
    {0xD156, 0x132F,2},      // PGA_TABLE_CWF_DATA_51
    {0xD158, 0x6B71,2},      // PGA_TABLE_CWF_DATA_52
    {0xD15A, 0x0FF3,2},      // PGA_TABLE_CWF_DATA_53
    {0xD15C, 0x49D3,2},      // PGA_TABLE_CWF_DATA_54
    {0xD15E, 0x0C72,2},      // PGA_TABLE_CWF_DATA_55
    {0xD160, 0x052E,2},      // PGA_TABLE_CWF_DATA_56
    {0xD162, 0x5DD2,2},      // PGA_TABLE_CWF_DATA_57
    {0xD164, 0x4A71,2},      // PGA_TABLE_CWF_DATA_58
    {0xD166, 0xCCF5,2},      // PGA_TABLE_CWF_DATA_59
    {0xD168, 0x3E30,2},      // PGA_TABLE_CWF_DATA_60
    {0xD16A, 0x84F1,2},      // PGA_TABLE_CWF_DATA_61
    {0xD16C, 0x96D3,2},      // PGA_TABLE_CWF_DATA_62
    {0xD16E, 0x30D3,2},      // PGA_TABLE_CWF_DATA_63
    {0xD170, 0x9894,2},      // PGA_TABLE_CWF_DATA_64
    {0xD172, 0x1FB1,2},      // PGA_TABLE_CWF_DATA_65
    {0xD174, 0xE02F,2},      // PGA_TABLE_CWF_DATA_66
    {0xD176, 0xD054,2},      // PGA_TABLE_CWF_DATA_67
    {0xD178, 0xF5D0,2},      // PGA_TABLE_CWF_DATA_68
    {0xD17A, 0x1716,2},      // PGA_TABLE_CWF_DATA_69
    {0xD17C, 0x96B0,2},      // PGA_TABLE_CWF_DATA_70
    {0xD17E, 0xD0B0,2},      // PGA_TABLE_CWF_DATA_71
    {0xD180, 0x0412,2},      // PGA_TABLE_CWF_DATA_72
    {0xD182, 0xB890,2},      // PGA_TABLE_CWF_DATA_73
    {0xD184, 0xD055,2},      // PGA_TABLE_CWF_DATA_74
    {0xD186, 0xD26A,2},      // PGA_TABLE_CWF_DATA_75
    {0xD188, 0xB730,2},      // PGA_TABLE_CWF_DATA_76
    {0xD18A, 0xBE10,2},      // PGA_TABLE_CWF_DATA_77
    {0xD18C, 0x64D3,2},      // PGA_TABLE_CWF_DATA_78
    {0xD18E, 0x9493,2},      // PGA_TABLE_CWF_DATA_79
    {0xD190, 0x1CF4,2},      // PGA_TABLE_CWF_DATA_80
    {0xD192, 0xFE13,2},      // PGA_TABLE_CWF_DATA_81
    {0xD194, 0xD638,2},      // PGA_TABLE_CWF_DATA_82
    {0xD196, 0x40D8,2},      // PGA_TABLE_CWF_DATA_83
    {0xD198, 0x5EBB,2},      // PGA_TABLE_CWF_DATA_84
    {0xD19A, 0x5012,2},      // PGA_TABLE_CWF_DATA_85
    {0xD19C, 0xA6F5,2},      // PGA_TABLE_CWF_DATA_86
    {0xD19E, 0xC898,2},      // PGA_TABLE_CWF_DATA_87
    {0xD1A0, 0x43F8,2},      // PGA_TABLE_CWF_DATA_88
    {0xD1A2, 0x4FFB,2},      // PGA_TABLE_CWF_DATA_89
    {0xD1A4, 0x2F93,2},      // PGA_TABLE_CWF_DATA_90
    {0xD1A6, 0xDE33,2},      // PGA_TABLE_CWF_DATA_91
    {0xD1A8, 0x90D8,2},      // PGA_TABLE_CWF_DATA_92
    {0xD1AA, 0x4838,2},      // PGA_TABLE_CWF_DATA_93
    {0xD1AC, 0x249B,2},      // PGA_TABLE_CWF_DATA_94
    {0xD1AE, 0x25D4,2},      // PGA_TABLE_CWF_DATA_95
    {0xD1B0, 0xEC73,2},      // PGA_TABLE_CWF_DATA_96
    {0xD1B2, 0xDAD8,2},      // PGA_TABLE_CWF_DATA_97
    {0xD1B4, 0x1878,2},      // PGA_TABLE_CWF_DATA_98
    {0xD1B6, 0x64DB,2},      // PGA_TABLE_CWF_DATA_99
    {0xD1BA, 0x03EC,2},      // PGA_TABLE_CWF_CENTER_COLUMN
    {0xD1B8, 0x0200,2},      // PGA_TABLE_CWF_CENTER_ROW
    {0xC9FE, 0x0FA0,2},      // CAM_PGA_M_CONFIG_COLOUR_TEMP
    {0xCA00, 0x8000,2},      // CAM_PGA_M_CONFIG_GREEN_RED_Q14
    {0xCA02, 0x8000,2},      // CAM_PGA_M_CONFIG_RED_Q14
    {0xCA04, 0x8000,2},      // CAM_PGA_M_CONFIG_GREEN_BLUE_Q14
    {0xCA06, 0x8000,2},      // CAM_PGA_M_CONFIG_BLUE_Q14
    {0xD024, 0x0110,2},      // PGA_TABLE_D65_DATA_0
    {0xD026, 0xC1AD,2},      // PGA_TABLE_D65_DATA_1
    {0xD028, 0x2792,2},      // PGA_TABLE_D65_DATA_2
    {0xD02A, 0xEB2B,2},      // PGA_TABLE_D65_DATA_3
    {0xD02C, 0x1630,2},      // PGA_TABLE_D65_DATA_4
    {0xD02E, 0x0190,2},      // PGA_TABLE_D65_DATA_5
    {0xD030, 0x22EE,2},      // PGA_TABLE_D65_DATA_6
    {0xD032, 0x74F2,2},      // PGA_TABLE_D65_DATA_7
    {0xD034, 0x5750,2},      // PGA_TABLE_D65_DATA_8
    {0xD036, 0x9FB1,2},      // PGA_TABLE_D65_DATA_9
    {0xD038, 0x01B0,2},      // PGA_TABLE_D65_DATA_10
    {0xD03A, 0xD14D,2},      // PGA_TABLE_D65_DATA_11
    {0xD03C, 0x4E71,2},      // PGA_TABLE_D65_DATA_12
    {0xD03E, 0xC56E,2},      // PGA_TABLE_D65_DATA_13
    {0xD040, 0x1D70,2},      // PGA_TABLE_D65_DATA_14
    {0xD042, 0x0230,2},      // PGA_TABLE_D65_DATA_15
    {0xD044, 0x60ED,2},      // PGA_TABLE_D65_DATA_16
    {0xD046, 0x3A32,2},      // PGA_TABLE_D65_DATA_17
    {0xD048, 0x372F,2},      // PGA_TABLE_D65_DATA_18
    {0xD04A, 0x26AE,2},      // PGA_TABLE_D65_DATA_19
    {0xD04C, 0xA18D,2},      // PGA_TABLE_D65_DATA_20
    {0xD04E, 0xC1AF,2},      // PGA_TABLE_D65_DATA_21
    {0xD050, 0x3910,2},      // PGA_TABLE_D65_DATA_22
    {0xD052, 0x026F,2},      // PGA_TABLE_D65_DATA_23
    {0xD054, 0xCFB2,2},      // PGA_TABLE_D65_DATA_24
    {0xD056, 0x9F6D,2},      // PGA_TABLE_D65_DATA_25
    {0xD058, 0x5D4F,2},      // PGA_TABLE_D65_DATA_26
    {0xD05A, 0x16B1,2},      // PGA_TABLE_D65_DATA_27
    {0xD05C, 0xAC0E,2},      // PGA_TABLE_D65_DATA_28
    {0xD05E, 0xEA92,2},      // PGA_TABLE_D65_DATA_29
    {0xD060, 0x818E,2},      // PGA_TABLE_D65_DATA_30
    {0xD062, 0x5DAF,2},      // PGA_TABLE_D65_DATA_31
    {0xD064, 0x4591,2},      // PGA_TABLE_D65_DATA_32
    {0xD066, 0x9151,2},      // PGA_TABLE_D65_DATA_33
    {0xD068, 0x9D13,2},      // PGA_TABLE_D65_DATA_34
    {0xD06A, 0x978E,2},      // PGA_TABLE_D65_DATA_35
    {0xD06C, 0xDAEF,2},      // PGA_TABLE_D65_DATA_36
    {0xD06E, 0x1971,2},      // PGA_TABLE_D65_DATA_37
    {0xD070, 0x2CAD,2},      // PGA_TABLE_D65_DATA_38
    {0xD072, 0xFFB2,2},      // PGA_TABLE_D65_DATA_39
    {0xD074, 0x1952,2},      // PGA_TABLE_D65_DATA_40
    {0xD076, 0x878F,2},      // PGA_TABLE_D65_DATA_41
    {0xD078, 0x2373,2},      // PGA_TABLE_D65_DATA_42
    {0xD07A, 0x2594,2},      // PGA_TABLE_D65_DATA_43
    {0xD07C, 0x8B76,2},      // PGA_TABLE_D65_DATA_44
    {0xD07E, 0x7212,2},      // PGA_TABLE_D65_DATA_45
    {0xD080, 0x2F31,2},      // PGA_TABLE_D65_DATA_46
    {0xD082, 0x78F3,2},      // PGA_TABLE_D65_DATA_47
    {0xD084, 0xB5EE,2},      // PGA_TABLE_D65_DATA_48
    {0xD086, 0xAF36,2},      // PGA_TABLE_D65_DATA_49
    {0xD088, 0x7351,2},      // PGA_TABLE_D65_DATA_50
    {0xD08A, 0x1A46,2},      // PGA_TABLE_D65_DATA_51
    {0xD08C, 0xCE71,2},      // PGA_TABLE_D65_DATA_52
    {0xD08E, 0x00D4,2},      // PGA_TABLE_D65_DATA_53
    {0xD090, 0x1B55,2},      // PGA_TABLE_D65_DATA_54
    {0xD092, 0x0D12,2},      // PGA_TABLE_D65_DATA_55
    {0xD094, 0x0DEB,2},      // PGA_TABLE_D65_DATA_56
    {0xD096, 0x49D2,2},      // PGA_TABLE_D65_DATA_57
    {0xD098, 0x1533,2},      // PGA_TABLE_D65_DATA_58
    {0xD09A, 0xD7B4,2},      // PGA_TABLE_D65_DATA_59
    {0xD09C, 0x58F0,2},      // PGA_TABLE_D65_DATA_60
    {0xD09E, 0xA1B1,2},      // PGA_TABLE_D65_DATA_61
    {0xD0A0, 0xD5F3,2},      // PGA_TABLE_D65_DATA_62
    {0xD0A2, 0x0B74,2},      // PGA_TABLE_D65_DATA_63
    {0xD0A4, 0x22D3,2},      // PGA_TABLE_D65_DATA_64
    {0xD0A6, 0x3291,2},      // PGA_TABLE_D65_DATA_65
    {0xD0A8, 0x0BC9,2},      // PGA_TABLE_D65_DATA_66
    {0xD0AA, 0x87B4,2},      // PGA_TABLE_D65_DATA_67
    {0xD0AC, 0xE4B2,2},      // PGA_TABLE_D65_DATA_68
    {0xD0AE, 0x8BD2,2},      // PGA_TABLE_D65_DATA_69
    {0xD0B0, 0xCDAE,2},      // PGA_TABLE_D65_DATA_70
    {0xD0B2, 0xBB10,2},      // PGA_TABLE_D65_DATA_71
    {0xD0B4, 0xED52,2},      // PGA_TABLE_D65_DATA_72
    {0xD0B6, 0x3B92,2},      // PGA_TABLE_D65_DATA_73
    {0xD0B8, 0x3013,2},      // PGA_TABLE_D65_DATA_74
    {0xD0BA, 0x100E,2},      // PGA_TABLE_D65_DATA_75
    {0xD0BC, 0x8830,2},      // PGA_TABLE_D65_DATA_76
    {0xD0BE, 0x9193,2},      // PGA_TABLE_D65_DATA_77
    {0xD0C0, 0x0334,2},      // PGA_TABLE_D65_DATA_78
    {0xD0C2, 0x42F5,2},      // PGA_TABLE_D65_DATA_79
    {0xD0C4, 0x1A54,2},      // PGA_TABLE_D65_DATA_80
    {0xD0C6, 0x59D3,2},      // PGA_TABLE_D65_DATA_81
    {0xD0C8, 0x8319,2},      // PGA_TABLE_D65_DATA_82
    {0xD0CA, 0x9ED4,2},      // PGA_TABLE_D65_DATA_83
    {0xD0CC, 0x7B1B,2},      // PGA_TABLE_D65_DATA_84
    {0xD0CE, 0x67D3,2},      // PGA_TABLE_D65_DATA_85
    {0xD0D0, 0xB534,2},      // PGA_TABLE_D65_DATA_86
    {0xD0D2, 0xA5B9,2},      // PGA_TABLE_D65_DATA_87
    {0xD0D4, 0x2390,2},      // PGA_TABLE_D65_DATA_88
    {0xD0D6, 0x0BFC,2},      // PGA_TABLE_D65_DATA_89
    {0xD0D8, 0x4F52,2},      // PGA_TABLE_D65_DATA_90
    {0xD0DA, 0x1EF3,2},      // PGA_TABLE_D65_DATA_91
    {0xD0DC, 0x8CD8,2},      // PGA_TABLE_D65_DATA_92
    {0xD0DE, 0x3FB1,2},      // PGA_TABLE_D65_DATA_93
    {0xD0E0, 0x191B,2},      // PGA_TABLE_D65_DATA_94
    {0xD0E2, 0x1C74,2},      // PGA_TABLE_D65_DATA_95
    {0xD0E4, 0x3890,2},      // PGA_TABLE_D65_DATA_96
    {0xD0E6, 0xD138,2},      // PGA_TABLE_D65_DATA_97
    {0xD0E8, 0xA375,2},      // PGA_TABLE_D65_DATA_98
    {0xD0EA, 0x44DB,2},      // PGA_TABLE_D65_DATA_99
    {0xD0EE, 0x03C0,2},      // PGA_TABLE_D65_CENTER_COLUMN
    {0xD0EC, 0x0200,2},      // PGA_TABLE_D65_CENTER_ROW
    {0xCA08, 0x1964,2},      // CAM_PGA_R_CONFIG_COLOUR_TEMP
    {0xCA0A, 0x8000,2},      // CAM_PGA_R_CONFIG_GREEN_RED_Q14
    {0xCA0C, 0x8000,2},      // CAM_PGA_R_CONFIG_RED_Q14
    {0xCA0E, 0x8000,2},      // CAM_PGA_R_CONFIG_GREEN_BLUE_Q14
    {0xCA10, 0x8000,2},      // CAM_PGA_R_CONFIG_BLUE_Q14
    {0x098E, 0x0000,2},      // LOGICAL_ADDRESS_ACCESS
    {0xD1BC, 0x0110,2},      // PGA_TABLE_A_DATA_0
    {0xD1BE, 0xC4EC,2},      // PGA_TABLE_A_DATA_1
    {0xD1C0, 0x0C72,2},      // PGA_TABLE_A_DATA_2
    {0xD1C2, 0x0B70,2},      // PGA_TABLE_A_DATA_3
    {0xD1C4, 0x8E90,2},      // PGA_TABLE_A_DATA_4
    {0xD1C6, 0x0270,2},      // PGA_TABLE_A_DATA_5
    {0xD1C8, 0x2C8F,2},      // PGA_TABLE_A_DATA_6
    {0xD1CA, 0x04B3,2},      // PGA_TABLE_A_DATA_7
    {0xD1CC, 0x1FB0,2},      // PGA_TABLE_A_DATA_8
    {0xD1CE, 0xA832,2},      // PGA_TABLE_A_DATA_9
    {0xD1D0, 0x0290,2},      // PGA_TABLE_A_DATA_10
    {0xD1D2, 0xE80C,2},      // PGA_TABLE_A_DATA_11
    {0xD1D4, 0x73D0,2},      // PGA_TABLE_A_DATA_12
    {0xD1D6, 0x080D,2},      // PGA_TABLE_A_DATA_13
    {0xD1D8, 0xCCEF,2},      // PGA_TABLE_A_DATA_14
    {0xD1DA, 0x07B0,2},      // PGA_TABLE_A_DATA_15
    {0xD1DC, 0x032F,2},      // PGA_TABLE_A_DATA_16
    {0xD1DE, 0x2CD2,2},      // PGA_TABLE_A_DATA_17
    {0xD1E0, 0xFA4A,2},      // PGA_TABLE_A_DATA_18
    {0xD1E2, 0x84B2,2},      // PGA_TABLE_A_DATA_19
    {0xD1E4, 0xC20B,2},      // PGA_TABLE_A_DATA_20
    {0xD1E6, 0xE36F,2},      // PGA_TABLE_A_DATA_21
    {0xD1E8, 0x9A10,2},      // PGA_TABLE_A_DATA_22
    {0xD1EA, 0x55D0,2},      // PGA_TABLE_A_DATA_23
    {0xD1EC, 0x34F1,2},      // PGA_TABLE_A_DATA_24
    {0xD1EE, 0xAA6A,2},      // PGA_TABLE_A_DATA_25
    {0xD1F0, 0x4E4F,2},      // PGA_TABLE_A_DATA_26
    {0xD1F2, 0xAAEF,2},      // PGA_TABLE_A_DATA_27
    {0xD1F4, 0xA2AF,2},      // PGA_TABLE_A_DATA_28
    {0xD1F6, 0x1C2F,2},      // PGA_TABLE_A_DATA_29
    {0xD1F8, 0x854C,2},      // PGA_TABLE_A_DATA_30
    {0xD1FA, 0x59CF,2},      // PGA_TABLE_A_DATA_31
    {0xD1FC, 0xCE4E,2},      // PGA_TABLE_A_DATA_32
    {0xD1FE, 0xB131,2},      // PGA_TABLE_A_DATA_33
    {0xD200, 0x2651,2},      // PGA_TABLE_A_DATA_34
    {0xD202, 0xEBCD,2},      // PGA_TABLE_A_DATA_35
    {0xD204, 0xE2CF,2},      // PGA_TABLE_A_DATA_36
    {0xD206, 0xE10F,2},      // PGA_TABLE_A_DATA_37
    {0xD208, 0x1051,2},      // PGA_TABLE_A_DATA_38
    {0xD20A, 0x0833,2},      // PGA_TABLE_A_DATA_39
    {0xD20C, 0x26B2,2},      // PGA_TABLE_A_DATA_40
    {0xD20E, 0x01B0,2},      // PGA_TABLE_A_DATA_41
    {0xD210, 0x3593,2},      // PGA_TABLE_A_DATA_42
    {0xD212, 0x5AF2,2},      // PGA_TABLE_A_DATA_43
    {0xD214, 0x9196,2},      // PGA_TABLE_A_DATA_44
    {0xD216, 0x0653,2},      // PGA_TABLE_A_DATA_45
    {0xD218, 0x07D2,2},      // PGA_TABLE_A_DATA_46
    {0xD21A, 0x7814,2},      // PGA_TABLE_A_DATA_47
    {0xD21C, 0x9E75,2},      // PGA_TABLE_A_DATA_48
    {0xD21E, 0xAB97,2},      // PGA_TABLE_A_DATA_49
    {0xD220, 0x2A31,2},      // PGA_TABLE_A_DATA_50
    {0xD222, 0x17CE,2},      // PGA_TABLE_A_DATA_51
    {0xD224, 0x7652,2},      // PGA_TABLE_A_DATA_52
    {0xD226, 0x56F2,2},      // PGA_TABLE_A_DATA_53
    {0xD228, 0xA435,2},      // PGA_TABLE_A_DATA_54
    {0xD22A, 0x0E92,2},      // PGA_TABLE_A_DATA_55
    {0xD22C, 0x2C70,2},      // PGA_TABLE_A_DATA_56
    {0xD22E, 0x3E32,2},      // PGA_TABLE_A_DATA_57
    {0xD230, 0xA4F4,2},      // PGA_TABLE_A_DATA_58
    {0xD232, 0xD3D5,2},      // PGA_TABLE_A_DATA_59
    {0xD234, 0x2251,2},      // PGA_TABLE_A_DATA_60
    {0xD236, 0x2552,2},      // PGA_TABLE_A_DATA_61
    {0xD238, 0x3A53,2},      // PGA_TABLE_A_DATA_62
    {0xD23A, 0xA734,2},      // PGA_TABLE_A_DATA_63
    {0xD23C, 0xE1D5,2},      // PGA_TABLE_A_DATA_64
    {0xD23E, 0x41B1,2},      // PGA_TABLE_A_DATA_65
    {0xD240, 0x6D72,2},      // PGA_TABLE_A_DATA_66
    {0xD242, 0x4354,2},      // PGA_TABLE_A_DATA_67
    {0xD244, 0xE675,2},      // PGA_TABLE_A_DATA_68
    {0xD246, 0x9737,2},      // PGA_TABLE_A_DATA_69
    {0xD248, 0xD68F,2},      // PGA_TABLE_A_DATA_70
    {0xD24A, 0x1231,2},      // PGA_TABLE_A_DATA_71
    {0xD24C, 0x6F74,2},      // PGA_TABLE_A_DATA_72
    {0xD24E, 0xDB93,2},      // PGA_TABLE_A_DATA_73
    {0xD250, 0xB4D6,2},      // PGA_TABLE_A_DATA_74
    {0xD252, 0x1B4F,2},      // PGA_TABLE_A_DATA_75
    {0xD254, 0x56F2,2},      // PGA_TABLE_A_DATA_76
    {0xD256, 0x29F5,2},      // PGA_TABLE_A_DATA_77
    {0xD258, 0x8F15,2},      // PGA_TABLE_A_DATA_78
    {0xD25A, 0x8777,2},      // PGA_TABLE_A_DATA_79
    {0xD25C, 0x9392,2},      // PGA_TABLE_A_DATA_80
    {0xD25E, 0xBF2F,2},      // PGA_TABLE_A_DATA_81
    {0xD260, 0x8E98,2},      // PGA_TABLE_A_DATA_82
    {0xD262, 0xC7B4,2},      // PGA_TABLE_A_DATA_83
    {0xD264, 0x61D9,2},      // PGA_TABLE_A_DATA_84
    {0xD266, 0x4B72,2},      // PGA_TABLE_A_DATA_85
    {0xD268, 0xA994,2},      // PGA_TABLE_A_DATA_86
    {0xD26A, 0xFF18,2},      // PGA_TABLE_A_DATA_87
    {0xD26C, 0x4E95,2},      // PGA_TABLE_A_DATA_88
    {0xD26E, 0x31DA,2},      // PGA_TABLE_A_DATA_89
    {0xD270, 0x1352,2},      // PGA_TABLE_A_DATA_90
    {0xD272, 0x3EB3,2},      // PGA_TABLE_A_DATA_91
    {0xD274, 0x8637,2},      // PGA_TABLE_A_DATA_92
    {0xD276, 0xB2B4,2},      // PGA_TABLE_A_DATA_93
    {0xD278, 0x1AF9,2},      // PGA_TABLE_A_DATA_94
    {0xD27A, 0x0EF0,2},      // PGA_TABLE_A_DATA_95
    {0xD27C, 0xF4F2,2},      // PGA_TABLE_A_DATA_96
    {0xD27E, 0xEF77,2},      // PGA_TABLE_A_DATA_97
    {0xD280, 0x0334,2},      // PGA_TABLE_A_DATA_98
    {0xD282, 0x28B9,2},      // PGA_TABLE_A_DATA_99
    {0xD286, 0x040C,2},      // PGA_TABLE_A_CENTER_COLUMN
    {0xD284, 0x022C,2},      // PGA_TABLE_A_CENTER_ROW
    {0xC9F4, 0x0AF0,2},      // CAM_PGA_L_CONFIG_COLOUR_TEMP
    {0xC9F6, 0x8000,2},      // CAM_PGA_L_CONFIG_GREEN_RED_Q14
    {0xC9F8, 0x8000,2},      // CAM_PGA_L_CONFIG_RED_Q14
    {0xC9FA, 0x8000,2},      // CAM_PGA_L_CONFIG_GREEN_BLUE_Q14
    {0xC9FC, 0x8000,2},      // CAM_PGA_L_CONFIG_BLUE_Q14
    {0xD0F0, 0x0130,2},      // PGA_TABLE_CWF_DATA_0
    {0xD0F2, 0x87EE,2},      // PGA_TABLE_CWF_DATA_1
    {0xD0F4, 0x1732,2},      // PGA_TABLE_CWF_DATA_2
    {0xD0F6, 0x04B0,2},      // PGA_TABLE_CWF_DATA_3
    {0xD0F8, 0xA9B1,2},      // PGA_TABLE_CWF_DATA_4
    {0xD0FA, 0x0170,2},      // PGA_TABLE_CWF_DATA_5
    {0xD0FC, 0x320E,2},      // PGA_TABLE_CWF_DATA_6
    {0xD0FE, 0x3BF2,2},      // PGA_TABLE_CWF_DATA_7
    {0xD100, 0x0D30,2},      // PGA_TABLE_CWF_DATA_8
    {0xD102, 0xAF72,2},      // PGA_TABLE_CWF_DATA_9
    {0xD104, 0x0230,2},      // PGA_TABLE_CWF_DATA_10
    {0xD106, 0xEF4D,2},      // PGA_TABLE_CWF_DATA_11
    {0xD108, 0x1AD1,2},      // PGA_TABLE_CWF_DATA_12
    {0xD10A, 0x214E,2},      // PGA_TABLE_CWF_DATA_13
    {0xD10C, 0xF150,2},      // PGA_TABLE_CWF_DATA_14
    {0xD10E, 0x0550,2},      // PGA_TABLE_CWF_DATA_15
    {0xD110, 0x164E,2},      // PGA_TABLE_CWF_DATA_16
    {0xD112, 0x2D32,2},      // PGA_TABLE_CWF_DATA_17
    {0xD114, 0x5DAD,2},      // PGA_TABLE_CWF_DATA_18
    {0xD116, 0x8D92,2},      // PGA_TABLE_CWF_DATA_19
    {0xD118, 0x268A,2},      // PGA_TABLE_CWF_DATA_20
    {0xD11A, 0xB62F,2},      // PGA_TABLE_CWF_DATA_21
    {0xD11C, 0xE56F,2},      // PGA_TABLE_CWF_DATA_22
    {0xD11E, 0x0E10,2},      // PGA_TABLE_CWF_DATA_23
    {0xD120, 0xED8D,2},      // PGA_TABLE_CWF_DATA_24
    {0xD122, 0x210C,2},      // PGA_TABLE_CWF_DATA_25
    {0xD124, 0x778F,2},      // PGA_TABLE_CWF_DATA_26
    {0xD126, 0x93AE,2},      // PGA_TABLE_CWF_DATA_27
    {0xD128, 0x92F1,2},      // PGA_TABLE_CWF_DATA_28
    {0xD12A, 0xB391,2},      // PGA_TABLE_CWF_DATA_29
    {0xD12C, 0xF04B,2},      // PGA_TABLE_CWF_DATA_30
    {0xD12E, 0x00F0,2},      // PGA_TABLE_CWF_DATA_31
    {0xD130, 0xD48D,2},      // PGA_TABLE_CWF_DATA_32
    {0xD132, 0xC111,2},      // PGA_TABLE_CWF_DATA_33
    {0xD134, 0x1D31,2},      // PGA_TABLE_CWF_DATA_34
    {0xD136, 0x978D,2},      // PGA_TABLE_CWF_DATA_35
    {0xD138, 0xAEEF,2},      // PGA_TABLE_CWF_DATA_36
    {0xD13A, 0xAEEF,2},      // PGA_TABLE_CWF_DATA_37
    {0xD13C, 0x5A8F,2},      // PGA_TABLE_CWF_DATA_38
    {0xD13E, 0x43D2,2},      // PGA_TABLE_CWF_DATA_39
    {0xD140, 0x2392,2},      // PGA_TABLE_CWF_DATA_40
    {0xD142, 0x002F,2},      // PGA_TABLE_CWF_DATA_41
    {0xD144, 0x2053,2},      // PGA_TABLE_CWF_DATA_42
    {0xD146, 0x2DB3,2},      // PGA_TABLE_CWF_DATA_43
    {0xD148, 0x8616,2},      // PGA_TABLE_CWF_DATA_44
    {0xD14A, 0x50B2,2},      // PGA_TABLE_CWF_DATA_45
    {0xD14C, 0x3271,2},      // PGA_TABLE_CWF_DATA_46
    {0xD14E, 0x1834,2},      // PGA_TABLE_CWF_DATA_47
    {0xD150, 0xE6D4,2},      // PGA_TABLE_CWF_DATA_48
    {0xD152, 0xE056,2},      // PGA_TABLE_CWF_DATA_49
    {0xD154, 0x34F1,2},      // PGA_TABLE_CWF_DATA_50
    {0xD156, 0x5BEE,2},      // PGA_TABLE_CWF_DATA_51
    {0xD158, 0x20F3,2},      // PGA_TABLE_CWF_DATA_52
    {0xD15A, 0x0C13,2},      // PGA_TABLE_CWF_DATA_53
    {0xD15C, 0xCF15,2},      // PGA_TABLE_CWF_DATA_54
    {0xD15E, 0x1452,2},      // PGA_TABLE_CWF_DATA_55
    {0xD160, 0x4B70,2},      // PGA_TABLE_CWF_DATA_56
    {0xD162, 0x01B3,2},      // PGA_TABLE_CWF_DATA_57
    {0xD164, 0xAB14,2},      // PGA_TABLE_CWF_DATA_58
    {0xD166, 0xE355,2},      // PGA_TABLE_CWF_DATA_59
    {0xD168, 0x0CF1,2},      // PGA_TABLE_CWF_DATA_60
    {0xD16A, 0x7A11,2},      // PGA_TABLE_CWF_DATA_61
    {0xD16C, 0x06CF,2},      // PGA_TABLE_CWF_DATA_62
    {0xD16E, 0x9E73,2},      // PGA_TABLE_CWF_DATA_63
    {0xD170, 0x6AD2,2},      // PGA_TABLE_CWF_DATA_64
    {0xD172, 0x2A71,2},      // PGA_TABLE_CWF_DATA_65
    {0xD174, 0x28F1,2},      // PGA_TABLE_CWF_DATA_66
    {0xD176, 0x7B11,2},      // PGA_TABLE_CWF_DATA_67
    {0xD178, 0xB054,2},      // PGA_TABLE_CWF_DATA_68
    {0xD17A, 0xA015,2},      // PGA_TABLE_CWF_DATA_69
    {0xD17C, 0xE06F,2},      // PGA_TABLE_CWF_DATA_70
    {0xD17E, 0x0810,2},      // PGA_TABLE_CWF_DATA_71
    {0xD180, 0x5E94,2},      // PGA_TABLE_CWF_DATA_72
    {0xD182, 0xFB91,2},      // PGA_TABLE_CWF_DATA_73
    {0xD184, 0x8256,2},      // PGA_TABLE_CWF_DATA_74
    {0xD186, 0x136F,2},      // PGA_TABLE_CWF_DATA_75
    {0xD188, 0x0312,2},      // PGA_TABLE_CWF_DATA_76
    {0xD18A, 0x0395,2},      // PGA_TABLE_CWF_DATA_77
    {0xD18C, 0x9B34,2},      // PGA_TABLE_CWF_DATA_78
    {0xD18E, 0xB316,2},      // PGA_TABLE_CWF_DATA_79
    {0xD190, 0x9E32,2},      // PGA_TABLE_CWF_DATA_80
    {0xD192, 0x1652,2},      // PGA_TABLE_CWF_DATA_81
    {0xD194, 0x9218,2},      // PGA_TABLE_CWF_DATA_82
    {0xD196, 0xC195,2},      // PGA_TABLE_CWF_DATA_83
    {0xD198, 0x6F99,2},      // PGA_TABLE_CWF_DATA_84
    {0xD19A, 0x8E52,2},      // PGA_TABLE_CWF_DATA_85
    {0xD19C, 0xF333,2},      // PGA_TABLE_CWF_DATA_86
    {0xD19E, 0xC078,2},      // PGA_TABLE_CWF_DATA_87
    {0xD1A0, 0x1C96,2},      // PGA_TABLE_CWF_DATA_88
    {0xD1A2, 0x209A,2},      // PGA_TABLE_CWF_DATA_89
    {0xD1A4, 0x1AB2,2},      // PGA_TABLE_CWF_DATA_90
    {0xD1A6, 0x6092,2},      // PGA_TABLE_CWF_DATA_91
    {0xD1A8, 0xAF17,2},      // PGA_TABLE_CWF_DATA_92
    {0xD1AA, 0x8B54,2},      // PGA_TABLE_CWF_DATA_93
    {0xD1AC, 0x4259,2},      // PGA_TABLE_CWF_DATA_94
    {0xD1AE, 0xE62F,2},      // PGA_TABLE_CWF_DATA_95
    {0xD1B0, 0xB9F3,2},      // PGA_TABLE_CWF_DATA_96
    {0xD1B2, 0xFED7,2},      // PGA_TABLE_CWF_DATA_97
    {0xD1B4, 0x5175,2},      // PGA_TABLE_CWF_DATA_98
    {0xD1B6, 0x42F9,2},      // PGA_TABLE_CWF_DATA_99
    {0xD1BA, 0x03F0,2},      // PGA_TABLE_CWF_CENTER_COLUMN
    {0xD1B8, 0x023C,2},      // PGA_TABLE_CWF_CENTER_ROW
    {0xC9FE, 0x0FA0,2},      // CAM_PGA_M_CONFIG_COLOUR_TEMP
    {0xCA00, 0x8000,2},      // CAM_PGA_M_CONFIG_GREEN_RED_Q14
    {0xCA02, 0x8000,2},      // CAM_PGA_M_CONFIG_RED_Q14
    {0xCA04, 0x8000,2},      // CAM_PGA_M_CONFIG_GREEN_BLUE_Q14
    {0xCA06, 0x8000,2},      // CAM_PGA_M_CONFIG_BLUE_Q14
    {0xD024, 0x0150,2},      // PGA_TABLE_D65_DATA_0
    {0xD026, 0xA02E,2},      // PGA_TABLE_D65_DATA_1
    {0xD028, 0x2352,2},      // PGA_TABLE_D65_DATA_2
    {0xD02A, 0x05D1,2},      // PGA_TABLE_D65_DATA_3
    {0xD02C, 0xAAF1,2},      // PGA_TABLE_D65_DATA_4
    {0xD02E, 0x0110,2},      // PGA_TABLE_D65_DATA_5
    {0xD030, 0x75ED,2},      // PGA_TABLE_D65_DATA_6
    {0xD032, 0x5B12,2},      // PGA_TABLE_D65_DATA_7
    {0xD034, 0x3191,2},      // PGA_TABLE_D65_DATA_8
    {0xD036, 0xDCB2,2},      // PGA_TABLE_D65_DATA_9
    {0xD038, 0x01D0,2},      // PGA_TABLE_D65_DATA_10
    {0xD03A, 0x81AE,2},      // PGA_TABLE_D65_DATA_11
    {0xD03C, 0x7E91,2},      // PGA_TABLE_D65_DATA_12
    {0xD03E, 0x40B0,2},      // PGA_TABLE_D65_DATA_13
    {0xD040, 0xDCF1,2},      // PGA_TABLE_D65_DATA_14
    {0xD042, 0x0310,2},      // PGA_TABLE_D65_DATA_15
    {0xD044, 0x760D,2},      // PGA_TABLE_D65_DATA_16
    {0xD046, 0x3712,2},      // PGA_TABLE_D65_DATA_17
    {0xD048, 0x26B0,2},      // PGA_TABLE_D65_DATA_18
    {0xD04A, 0xACD2,2},      // PGA_TABLE_D65_DATA_19
    {0xD04C, 0x96CB,2},      // PGA_TABLE_D65_DATA_20
    {0xD04E, 0xD28E,2},      // PGA_TABLE_D65_DATA_21
    {0xD050, 0xEDAC,2},      // PGA_TABLE_D65_DATA_22
    {0xD052, 0xDCEF,2},      // PGA_TABLE_D65_DATA_23
    {0xD054, 0xB871,2},      // PGA_TABLE_D65_DATA_24
    {0xD056, 0x9B0B,2},      // PGA_TABLE_D65_DATA_25
    {0xD058, 0x2B90,2},      // PGA_TABLE_D65_DATA_26
    {0xD05A, 0x6BAF,2},      // PGA_TABLE_D65_DATA_27
    {0xD05C, 0xE2D1,2},      // PGA_TABLE_D65_DATA_28
    {0xD05E, 0xECD1,2},      // PGA_TABLE_D65_DATA_29
    {0xD060, 0x9DAD,2},      // PGA_TABLE_D65_DATA_30
    {0xD062, 0x31B0,2},      // PGA_TABLE_D65_DATA_31
    {0xD064, 0x7B2F,2},      // PGA_TABLE_D65_DATA_32
    {0xD066, 0x9112,2},      // PGA_TABLE_D65_DATA_33
    {0xD068, 0x7E6E,2},      // PGA_TABLE_D65_DATA_34
    {0xD06A, 0xE1AD,2},      // PGA_TABLE_D65_DATA_35
    {0xD06C, 0xC98E,2},      // PGA_TABLE_D65_DATA_36
    {0xD06E, 0x0C8E,2},      // PGA_TABLE_D65_DATA_37
    {0xD070, 0x9D4F,2},      // PGA_TABLE_D65_DATA_38
    {0xD072, 0x58F1,2},      // PGA_TABLE_D65_DATA_39
    {0xD074, 0x11D2,2},      // PGA_TABLE_D65_DATA_40
    {0xD076, 0x4991,2},      // PGA_TABLE_D65_DATA_41
    {0xD078, 0x6253,2},      // PGA_TABLE_D65_DATA_42
    {0xD07A, 0x0D72,2},      // PGA_TABLE_D65_DATA_43
    {0xD07C, 0x9C96,2},      // PGA_TABLE_D65_DATA_44
    {0xD07E, 0x5412,2},      // PGA_TABLE_D65_DATA_45
    {0xD080, 0x3452,2},      // PGA_TABLE_D65_DATA_46
    {0xD082, 0x5BF4,2},      // PGA_TABLE_D65_DATA_47
    {0xD084, 0x8995,2},      // PGA_TABLE_D65_DATA_48
    {0xD086, 0x9517,2},      // PGA_TABLE_D65_DATA_49
    {0xD088, 0x5711,2},      // PGA_TABLE_D65_DATA_50
    {0xD08A, 0x44D1,2},      // PGA_TABLE_D65_DATA_51
    {0xD08C, 0x1A53,2},      // PGA_TABLE_D65_DATA_52
    {0xD08E, 0x304A,2},      // PGA_TABLE_D65_DATA_53
    {0xD090, 0xBBB5,2},      // PGA_TABLE_D65_DATA_54
    {0xD092, 0x0972,2},      // PGA_TABLE_D65_DATA_55
    {0xD094, 0x6891,2},      // PGA_TABLE_D65_DATA_56
    {0xD096, 0x5353,2},      // PGA_TABLE_D65_DATA_57
    {0xD098, 0xD9F4,2},      // PGA_TABLE_D65_DATA_58
    {0xD09A, 0x8E96,2},      // PGA_TABLE_D65_DATA_59
    {0xD09C, 0x3831,2},      // PGA_TABLE_D65_DATA_60
    {0xD09E, 0x4A50,2},      // PGA_TABLE_D65_DATA_61
    {0xD0A0, 0x5CD2,2},      // PGA_TABLE_D65_DATA_62
    {0xD0A2, 0x2413,2},      // PGA_TABLE_D65_DATA_63
    {0xD0A4, 0xF0F4,2},      // PGA_TABLE_D65_DATA_64
    {0xD0A6, 0x7D11,2},      // PGA_TABLE_D65_DATA_65
    {0xD0A8, 0x418E,2},      // PGA_TABLE_D65_DATA_66
    {0xD0AA, 0x2F54,2},      // PGA_TABLE_D65_DATA_67
    {0xD0AC, 0x79AF,2},      // PGA_TABLE_D65_DATA_68
    {0xD0AE, 0x9EF7,2},      // PGA_TABLE_D65_DATA_69
    {0xD0B0, 0x1BCC,2},      // PGA_TABLE_D65_DATA_70
    {0xD0B2, 0xA4F1,2},      // PGA_TABLE_D65_DATA_71
    {0xD0B4, 0x12D5,2},      // PGA_TABLE_D65_DATA_72
    {0xD0B6, 0x2254,2},      // PGA_TABLE_D65_DATA_73
    {0xD0B8, 0xE2B6,2},      // PGA_TABLE_D65_DATA_74
    {0xD0BA, 0x1FD0,2},      // PGA_TABLE_D65_DATA_75
    {0xD0BC, 0x386F,2},      // PGA_TABLE_D65_DATA_76
    {0xD0BE, 0x1955,2},      // PGA_TABLE_D65_DATA_77
    {0xD0C0, 0x64F1,2},      // PGA_TABLE_D65_DATA_78
    {0xD0C2, 0xD876,2},      // PGA_TABLE_D65_DATA_79
    {0xD0C4, 0x8D50,2},      // PGA_TABLE_D65_DATA_80
    {0xD0C6, 0xBB34,2},      // PGA_TABLE_D65_DATA_81
    {0xD0C8, 0xA8B8,2},      // PGA_TABLE_D65_DATA_82
    {0xD0CA, 0x4BD5,2},      // PGA_TABLE_D65_DATA_83
    {0xD0CC, 0x03DA,2},      // PGA_TABLE_D65_DATA_84
    {0xD0CE, 0x3230,2},      // PGA_TABLE_D65_DATA_85
    {0xD0D0, 0x92F5,2},      // PGA_TABLE_D65_DATA_86
    {0xD0D2, 0xE818,2},      // PGA_TABLE_D65_DATA_87
    {0xD0D4, 0x0277,2},      // PGA_TABLE_D65_DATA_88
    {0xD0D6, 0x487A,2},      // PGA_TABLE_D65_DATA_89
    {0xD0D8, 0x0213,2},      // PGA_TABLE_D65_DATA_90
    {0xD0DA, 0xA434,2},      // PGA_TABLE_D65_DATA_91
    {0xD0DC, 0xD457,2},      // PGA_TABLE_D65_DATA_92
    {0xD0DE, 0x0397,2},      // PGA_TABLE_D65_DATA_93
    {0xD0E0, 0x4639,2},      // PGA_TABLE_D65_DATA_94
    {0xD0E2, 0x3131,2},      // PGA_TABLE_D65_DATA_95
    {0xD0E4, 0x8195,2},      // PGA_TABLE_D65_DATA_96
    {0xD0E6, 0xA1B8,2},      // PGA_TABLE_D65_DATA_97
    {0xD0E8, 0x05D7,2},      // PGA_TABLE_D65_DATA_98
    {0xD0EA, 0x7CF9,2},      // PGA_TABLE_D65_DATA_99
    {0xD0EE, 0x03D0,2},      // PGA_TABLE_D65_CENTER_COLUMN
    {0xD0EC, 0x022C,2},      // PGA_TABLE_D65_CENTER_ROW
    {0xCA08, 0x1964,2},      // CAM_PGA_R_CONFIG_COLOUR_TEMP
    {0xCA0A, 0x8000,2},      // CAM_PGA_R_CONFIG_GREEN_RED_Q14
    {0xCA0C, 0x8000,2},      // CAM_PGA_R_CONFIG_RED_Q14
    {0xCA0E, 0x8000,2},      // CAM_PGA_R_CONFIG_GREEN_BLUE_Q14
    {0xCA10, 0x8000,2},      // CAM_PGA_R_CONFIG_BLUE_Q14
    {0xC9F2, 0x0011,2},      // CAM_PGA_PGA_CONTROL
    {0xD018, 0xFF,1},      // PGA_CURRENT_ZONE
    {0xD01E, 0x00,1},      // PGA_ZONE_LO_2
    {0xD01D, 0x64,1},      // PGA_ZONE_LO_1
    {0xD01C, 0xB4,1},      // PGA_ZONE_LO_0
    {0xD021, 0x6E,1},      // PGA_ZONE_HI_2
    {0xD020, 0xB9,1},      // PGA_ZONE_HI_1
    {0xD01F, 0xFF,1},      // PGA_ZONE_HI_0
    {0xD01B, 0x03,1},      // PGA_NUMBER_ZONES
    {0xC9F2, 0x0011,2},      // CAM_PGA_PGA_CONTROL
    {0x098E, 0x0000,2},      // LOGICAL_ADDRESS_ACCESS
    {0xC894, 0x00FE,2},      // CAM_AWB_CCM_L_0
    {0xC896, 0xFFA6,2},      // CAM_AWB_CCM_L_1
    {0xC898, 0x005D,2},      // CAM_AWB_CCM_L_2
    {0xC89A, 0x000A,2},      // CAM_AWB_CCM_L_3
    {0xC89C, 0x00B5,2},      // CAM_AWB_CCM_L_4
    {0xC89E, 0x0041,2},      // CAM_AWB_CCM_L_5
    {0xC8A0, 0xFFF3,2},      // CAM_AWB_CCM_L_6
    {0xC8A2, 0xFF9F,2},      // CAM_AWB_CCM_L_7
    {0xC8A4, 0x016E,2},      // CAM_AWB_CCM_L_8
    {0xC8CA, 0x0056,2},      // CAM_AWB_CCM_L_RG_GAIN
    {0xC8CC, 0x01A8,2},      // CAM_AWB_CCM_L_BG_GAIN
    {0xC8A6, 0x01C8,2},      // CAM_AWB_CCM_M_0
    {0xC8A8, 0xFF01,2},      // CAM_AWB_CCM_M_1
    {0xC8AA, 0x0036,2},      // CAM_AWB_CCM_M_2
    {0xC8AC, 0xFFFE,2},      // CAM_AWB_CCM_M_3
    {0xC8AE, 0x00F0,2},      // CAM_AWB_CCM_M_4
    {0xC8B0, 0x0012,2},      // CAM_AWB_CCM_M_5
    {0xC8B2, 0xFFD7,2},      // CAM_AWB_CCM_M_6
    {0xC8B4, 0xFF23,2},      // CAM_AWB_CCM_M_7
    {0xC8B6, 0x0206,2},      // CAM_AWB_CCM_M_8
    {0xC8CE, 0x0087,2},      // CAM_AWB_CCM_M_RG_GAIN
    {0xC8D0, 0x0157,2},      // CAM_AWB_CCM_M_BG_GAIN
    {0xC8B8, 0x0182,2},      // CAM_AWB_CCM_R_0
    {0xC8BA, 0xFF58,2},      // CAM_AWB_CCM_R_1
    {0xC8BC, 0x0027,2},      // CAM_AWB_CCM_R_2
    {0xC8BE, 0xFFF9,2},      // CAM_AWB_CCM_R_3
    {0xC8C0, 0x00D3,2},      // CAM_AWB_CCM_R_4
    {0xC8C2, 0x0034,2},      // CAM_AWB_CCM_R_5
    {0xC8C4, 0xFFF4,2},      // CAM_AWB_CCM_R_6
    {0xC8C6, 0xFF60,2},      // CAM_AWB_CCM_R_7
    {0xC8C8, 0x01AC,2},      // CAM_AWB_CCM_R_8
    {0xC8D2, 0x00BF,2},      // CAM_AWB_CCM_R_RG_GAIN
    {0xC8D4, 0x00CD,2},      // CAM_AWB_CCM_R_BG_GAIN
    {0xC8D6, 0x09C4,2},      // CAM_AWB_CCM_L_CTEMP
    {0xC8D8, 0x0D67,2},      // CAM_AWB_CCM_M_CTEMP
    {0xC8DA, 0x1964,2},      // CAM_AWB_CCM_R_CTEMP
    {0xAC0A, 0x00,1},      // AWB_R_SCENE_RATIO_LOWER
    {0xAC0B, 0xFF,1},      // AWB_R_SCENE_RATIO_UPPER
    {0xAC0C, 0x22,1},      // AWB_B_SCENE_RATIO_LOWER
    {0xAC0D, 0xFF,1},      // AWB_B_SCENE_RATIO_UPPER
    {0xC8EE, 0x09C4,2},      // CAM_AWB_COLOR_TEMPERATURE_MIN
    {0xC8F0, 0x1964,2},      // CAM_AWB_COLOR_TEMPERATURE_MAX
    {0xAC06, 0x63,1},      // AWB_R_RATIO_LOWER
    {0xAC07, 0x65,1},      // AWB_R_RATIO_UPPER
    {0xAC08, 0x63,1},      // AWB_B_RATIO_LOWER
    {0xAC09, 0x65,1},      // AWB_B_RATIO_UPPER
    {0xC894, 0x01D4,2},      // CAM_AWB_CCM_L_0
    {0xC896, 0xFF4E,2},      // CAM_AWB_CCM_L_1
    {0xC898, 0xFFDD,2},      // CAM_AWB_CCM_L_2
    {0xC89A, 0xFFA5,2},      // CAM_AWB_CCM_L_3
    {0xC89C, 0x017D,2},      // CAM_AWB_CCM_L_4
    {0xC89E, 0xFFDD,2},      // CAM_AWB_CCM_L_5
    {0xC8A0, 0xFFA5,2},      // CAM_AWB_CCM_L_6
    {0xC8A2, 0xFF4E,2},      // CAM_AWB_CCM_L_7
    {0xC8A4, 0x020C,2},      // CAM_AWB_CCM_L_8
    {0xC8A6, 0x0216,2},      // CAM_AWB_CCM_M_0
    {0xC8A8, 0xFF17,2},      // CAM_AWB_CCM_M_1
    {0xC8AA, 0xFFD3,2},      // CAM_AWB_CCM_M_2
    {0xC8AC, 0xFF88,2},      // CAM_AWB_CCM_M_3
    {0xC8AE, 0x01A6,2},      // CAM_AWB_CCM_M_4
    {0xC8B0, 0xFFD2,2},      // CAM_AWB_CCM_M_5
    {0xC8B2, 0xFFD2,2},      // CAM_AWB_CCM_M_6
    {0xC8B4, 0xFFA6,2},      // CAM_AWB_CCM_M_7
    {0xC8B6, 0x0188,2},      // CAM_AWB_CCM_M_8
    {0xC8B8, 0x0144,2},      // CAM_AWB_CCM_R_0
    {0xC8BA, 0xFFA6,2},      // CAM_AWB_CCM_R_1
    {0xC8BC, 0x0016,2},      // CAM_AWB_CCM_R_2
    {0xC8BE, 0xFFFC,2},      // CAM_AWB_CCM_R_3
    {0xC8C0, 0x0116,2},      // CAM_AWB_CCM_R_4
    {0xC8C2, 0xFFEE,2},      // CAM_AWB_CCM_R_5
    {0xC8C4, 0xFFD6,2},      // CAM_AWB_CCM_R_6
    {0xC8C6, 0xFFAD,2},      // CAM_AWB_CCM_R_7
    {0xC8C8, 0x017D,2},      // CAM_AWB_CCM_R_8
    {0xC8CA, 0x007D,2},      // CAM_AWB_CCM_L_RG_GAIN
    {0xC8CC, 0x019C,2},      // CAM_AWB_CCM_L_BG_GAIN
    {0xC8CE, 0x0087,2},      // CAM_AWB_CCM_M_RG_GAIN
    {0xC8D0, 0x0135,2},      // CAM_AWB_CCM_M_BG_GAIN
    {0xC8D2, 0x00BF,2},      // CAM_AWB_CCM_R_RG_GAIN
    {0xC8D4, 0x00DD,2},      // CAM_AWB_CCM_R_BG_GAIN
    {0xC8D6, 0x09C4,2},      // CAM_AWB_CCM_L_CTEMP
    {0xC8D8, 0x0D67,2},      // CAM_AWB_CCM_M_CTEMP
    {0xC8DA, 0x1964,2},      // CAM_AWB_CCM_R_CTEMP
    {0xC94C, 0x0000,2},      // CAM_STAT_AWB_CLIP_WINDOW_XSTART
    {0xC94E, 0x0000,2},      // CAM_STAT_AWB_CLIP_WINDOW_YSTART
    {0xC950, 0x077F,2},      // CAM_STAT_AWB_CLIP_WINDOW_XEND
    {0xC952, 0x0437,2},      // CAM_STAT_AWB_CLIP_WINDOW_YEND
    {0xC95C, 0x0034,2},      // CAM_STAT_AWB_X_SHIFT
    {0xC95E, 0x000F,2},      // CAM_STAT_AWB_Y_SHIFT
    {0xC8F4, 0x0080,2},      // CAM_AWB_AWB_XSCALE
    {0xC8F6, 0x00AB,2},      // CAM_AWB_AWB_YSCALE
    {0xC8F8, 0x0000,2},      // CAM_AWB_AWB_ROT_CENTER_X
    {0xC8FA, 0x03D9,2},      // CAM_AWB_AWB_ROT_CENTER_Y
    {0xC8FD, 0x00,1},      // CAM_AWB_AWB_ROT_ANGLE_SIN
    {0xC8FC, 0x00,1},      // CAM_AWB_AWB_ROT_ANGLE_COS
    {0xC900, 0x0000,2},      // CAM_AWB_AWB_WEIGHTS_0
    {0xC902, 0x0000,2},      // CAM_AWB_AWB_WEIGHTS_1
    {0xC904, 0x0000,2},      // CAM_AWB_AWB_WEIGHTS_2
    {0xC906, 0x0000,2},      // CAM_AWB_AWB_WEIGHTS_3
    {0xC908, 0x0000,2},      // CAM_AWB_AWB_WEIGHTS_4
    {0xC90A, 0x0111,2},      // CAM_AWB_AWB_WEIGHTS_5
    {0xC90C, 0x1110,2},      // CAM_AWB_AWB_WEIGHTS_6
    {0xC90E, 0x0000,2},      // CAM_AWB_AWB_WEIGHTS_7
    {0xC910, 0x0000,2},      // CAM_AWB_AWB_WEIGHTS_8
    {0xC912, 0x1122,2},      // CAM_AWB_AWB_WEIGHTS_9
    {0xC914, 0x2222,2},      // CAM_AWB_AWB_WEIGHTS_10
    {0xC916, 0x1100,2},      // CAM_AWB_AWB_WEIGHTS_11
    {0xC918, 0x0712,2},      // CAM_AWB_AWB_WEIGHTS_12
    {0xC91A, 0x2333,2},      // CAM_AWB_AWB_WEIGHTS_13
    {0xC91C, 0x3333,2},      // CAM_AWB_AWB_WEIGHTS_14
    {0xC91E, 0x3211,2},      // CAM_AWB_AWB_WEIGHTS_15
    {0xC920, 0x1234,2},      // CAM_AWB_AWB_WEIGHTS_16
    {0xC922, 0x4554,2},      // CAM_AWB_AWB_WEIGHTS_17
    {0xC924, 0x3334,2},      // CAM_AWB_AWB_WEIGHTS_18
    {0xC926, 0x4321,2},      // CAM_AWB_AWB_WEIGHTS_19
    {0xC928, 0x1345,2},      // CAM_AWB_AWB_WEIGHTS_20
    {0xC92A, 0x5554,2},      // CAM_AWB_AWB_WEIGHTS_21
    {0xC92C, 0x3223,2},      // CAM_AWB_AWB_WEIGHTS_22
    {0xC92E, 0x3332,2},      // CAM_AWB_AWB_WEIGHTS_23
    {0xC930, 0x1234,2},      // CAM_AWB_AWB_WEIGHTS_24
    {0xC932, 0x4443,2},      // CAM_AWB_AWB_WEIGHTS_25
    {0xC934, 0x2111,2},      // CAM_AWB_AWB_WEIGHTS_26
    {0xC936, 0x2221,2},      // CAM_AWB_AWB_WEIGHTS_27
    {0xC938, 0x0112,2},      // CAM_AWB_AWB_WEIGHTS_28
    {0xC93A, 0x2221,2},      // CAM_AWB_AWB_WEIGHTS_29
    {0xC93C, 0x1100,2},      // CAM_AWB_AWB_WEIGHTS_30
    {0xC93E, 0x0010,2},      // CAM_AWB_AWB_WEIGHTS_31
    {0x33F4, 0x0517,2},      // KERNEL_CONFIG
    {0xC81A, 0x003D,2},      // CAM_SENSOR_CFG_MIN_ANALOG_GAIN
    {0xC81C, 0x01F8,2},      // CAM_SENSOR_CFG_MAX_ANALOG_GAIN
    {0xC96C, 0x06,1},      // CAM_LL_START_DEMOSAIC
    {0xC971, 0x2B,1},      // CAM_LL_STOP_DEMOSAIC
    {0xC96E, 0x18,1},      // CAM_LL_START_AP_NOISE_GAIN
    {0xC96D, 0x07,1},      // CAM_LL_START_AP_NOISE_KNEE
    {0xC973, 0x53,1},      // CAM_LL_STOP_AP_NOISE_GAIN
    {0xC972, 0x1C,1},      // CAM_LL_STOP_AP_NOISE_KNEE
    {0xC96F, 0x0C,1},      // CAM_LL_START_AP_GAIN_POS
    {0xC970, 0x0A,1},      // CAM_LL_START_AP_GAIN_NEG
    {0xC974, 0x00,1},      // CAM_LL_STOP_AP_GAIN_POS
    {0xC975, 0x00,1},      // CAM_LL_STOP_AP_GAIN_NEG
    {0xC976, 0x0E,1},      // CAM_LL_START_GRB_APOS
    {0xC977, 0x07,1},      // CAM_LL_START_GRB_BPOS
    {0xC978, 0x0A,1},      // CAM_LL_START_GRB_ANEG
    {0xC979, 0x32,1},      // CAM_LL_START_GRB_BNEG
    {0xC97A, 0xE3,1},      // CAM_LL_STOP_GRB_APOS
    {0xC97C, 0x3B,1},      // CAM_LL_STOP_GRB_ANEG
    {0xC97B, 0x9D,1},      // CAM_LL_STOP_GRB_BPOS
    {0xC97D, 0x64,1},      // CAM_LL_STOP_GRB_BNEG
    {0x33F4, 0x0515,2},      // KERNEL_CONFIG
    {0xC9CE, 0x0040,2},      // CAM_LL_START_MAX_GAIN_METRIC
    {0xC9D0, 0x0400,2},      // CAM_LL_MID_MAX_GAIN_METRIC
    {0xC9D2, 0x1000,2},      // CAM_LL_STOP_MAX_GAIN_METRIC
    {0xC984, 0x0136,2},      // CAM_LL_START_CDC_HI_THR_COMB
    {0xC988, 0x00D2,2},      // CAM_LL_START_CDC_HI_THR_SATUR
    {0xC98C, 0x00FC,2},      // CAM_LL_MID_CDC_HI_THR_COMB
    {0xC990, 0x0132,2},      // CAM_LL_MID_CDC_HI_THR_SATUR
    {0xC994, 0x07B6,2},      // CAM_LL_STOP_CDC_HI_THR_COMB
    {0xC998, 0x035E,2},      // CAM_LL_STOP_CDC_HI_THR_SATUR
    {0xC986, 0x00A9,2},      // CAM_LL_START_CDC_LO_THR_COMB
    {0xC98A, 0x00A6,2},      // CAM_LL_START_CDC_LO_THR_SATUR
    {0xC98E, 0x0119,2},      // CAM_LL_MID_CDC_LO_THR_COMB
    {0xC992, 0x0134,2},      // CAM_LL_MID_CDC_LO_THR_SATUR
    {0xC996, 0x062E,2},      // CAM_LL_STOP_CDC_LO_THR_COMB
    {0xC99A, 0x026F,2},      // CAM_LL_STOP_CDC_LO_THR_SATUR
    {0xC97E, 0x000F,2},      // CAM_LL_START_DC_SINGLE_PIXEL_THR
    {0xC980, 0x0005,2},      // CAM_LL_STOP_DC_SINGLE_PIXEL_THR
    {0xC99C, 0x0017,2},      // CAM_LL_START_CDC_CC_NOISE_SLOPE
    {0xC99E, 0x0008,2},      // CAM_LL_START_CDC_CC_NOISE_KNEE
    {0xC9A0, 0x0026,2},      // CAM_LL_MID_CDC_CC_NOISE_SLOPE
    {0xC9A2, 0x003E,2},      // CAM_LL_MID_CDC_CC_NOISE_KNEE
    {0xC9A4, 0x0055,2},      // CAM_LL_STOP_CDC_CC_NOISE_SLOPE
    {0xC9A6, 0x00FF,2},      // CAM_LL_STOP_CDC_CC_NOISE_KNEE
    {0x33F4, 0x0D15,2},      // KERNEL_CONFIG
    {0xC9A8, 0x0040,2},      // CAM_LL_ADACD_LUT_GAIN_0
    {0xC9AE, 0x03,1},      // CAM_LL_ADACD_LUT_SIGMA_0
    {0xC9B2, 0x00A8,2},      // CAM_LL_ADACD_LUT_K_0
    {0xC9AA, 0x0140,2},      // CAM_LL_ADACD_LUT_GAIN_1
    {0xC9AF, 0x07,1},      // CAM_LL_ADACD_LUT_SIGMA_1
    {0xC9B4, 0x00A8,2},      // CAM_LL_ADACD_LUT_K_1
    {0xC9AC, 0x0200,2},      // CAM_LL_ADACD_LUT_GAIN_2
    {0xC9B0, 0x0F,1},      // CAM_LL_ADACD_LUT_SIGMA_2
    {0xC9B6, 0x0064,2},      // CAM_LL_ADACD_LUT_K_2
    {0xBC02, 0x0013,2},      // LL_MODE
    {0xC9BE, 0x00,1},      // CAM_LL_ADACD_LL_MODE_EN
    {0x3398, 0x0060,2},      // ADACD_LOWLIGHT_CONTROL
    {0x3398, 0x0260,2},      // ADACD_LOWLIGHT_CONTROL
    {0x3398, 0x0260,2},      // ADACD_LOWLIGHT_CONTROL
    {0xC9B1, 0x01,1},      // CAM_LL_ADACD_PATCH
    {0xC9B8, 0x021F,2},      // CAM_LL_ADACD_TRT
    {0x326E, 0x0086,2},      // LOW_PASS_YUV_FILTER
    {0x3270, 0x0FAA,2},      // THRESHOLD_FOR_Y_FILTER_R_CHANNEL
    {0x3272, 0x0FE4,2},      // THRESHOLD_FOR_Y_FILTER_G_CHANNEL
    {0xC9CA, 0x0040,2},      // CAM_LL_START_GAIN_METRIC
    {0xC9CC, 0x1000,2},      // CAM_LL_STOP_GAIN_METRIC
    {0xC9CE, 0x0040,2},      // CAM_LL_START_MAX_GAIN_METRIC
    {0xC9D2, 0x1000,2},      // CAM_LL_STOP_MAX_GAIN_METRIC
    {0xC944, 0x80,1},      // CAM_AWB_K_R_L
    {0xC945, 0x80,1},      // CAM_AWB_K_G_L
    {0xC946, 0x80,1},      // CAM_AWB_K_B_L
    {0xC947, 0x80,1},      // CAM_AWB_K_R_R
    {0xC948, 0x80,1},      // CAM_AWB_K_G_R
    {0xC949, 0x80,1},      // CAM_AWB_K_B_R
    {0xC962, 0x0032,2},      // CAM_LL_START_BRIGHTNESS
    {0xC964, 0x024D,2},      // CAM_LL_STOP_BRIGHTNESS
    {0xC88A, 0x0180,2},      // CAM_AET_AE_MAX_VIRT_DGAIN
    {0xC87E, 0x3D,1},      // CAM_AET_TARGET_AVERAGE_LUMA
    {0xC87F, 0x3D,1},      // CAM_AET_TARGET_AVERAGE_LUMA_DARK
    {0x32B2, 0x2314,2},      // DKDELTA_CCM_CTL
    {0xB402, 0x0002,2},      // CCM_MODE
    {0xC968, 0x80,1},      // CAM_LL_START_DESATURATION
    {0xC969, 0x50,1},      // CAM_LL_END_DESATURATION
    {0xC96A, 0x18,1},      // CAM_LL_START_DARK_DELTA_CCM_THR
    {0xC96B, 0x0F,1},      // CAM_LL_STOP_DARK_DELTA_CCM_THR
    {0xB42A, 0x05,1},      // CCM_DELTA_GAIN
    {0xC8DC, 0x0100,2},      // CAM_AWB_LL_CCM_0
    {0xC8DE, 0x0000,2},      // CAM_AWB_LL_CCM_1
    {0xC8E0, 0x0000,2},      // CAM_AWB_LL_CCM_2
    {0xC8E2, 0x0000,2},      // CAM_AWB_LL_CCM_3
    {0xC8E4, 0x0100,2},      // CAM_AWB_LL_CCM_4
    {0xC8E6, 0x0000,2},      // CAM_AWB_LL_CCM_5
    {0xC8E8, 0x0000,2},      // CAM_AWB_LL_CCM_6
    {0xC8EA, 0x0000,2},      // CAM_AWB_LL_CCM_7
    {0xC8EC, 0x0100,2},      // CAM_AWB_LL_CCM_8
    {0xC9EF, 0x26,1},      // RESERVED_CAM_1EF
    {0xC9D4, 0x0050,2},      // CAM_LL_START_FADE_TO_BLACK_LUMA
    {0xC9D6, 0x0030,2},      // CAM_LL_STOP_FADE_TO_BLACK_LUMA
    {0xCA1C, 0x8040,2},      // CAM_PORT_OUTPUT_CONTROL
    {0x001E, 0x0777,2},      // PAD_SLEW
	{0xFFFF,INIT_DELAY,1}, //ADD
#if defined(FEATURE_TW_CAMERA_FPS28_MCLK24_PCLK92_MIPI1_H)
	{0xCA1C, 0x8041,2},
#else
    {0xCA1C, 0x8043,2},      // CAM_PORT_OUTPUT_CONTROL
#endif
    {0xC88A, 0x0100,2},      // CAM_AET_AE_MAX_VIRT_DGAIN
    {0xC882, 0x000A,2},      // CAM_AET_BLACK_CLIPPING_TARGET
	{0xFFFF,10,1},
    {0xDC00, 0x28,1},      // SYSMGR_NEXT_STATE
    {0x0080, 0x8002,2},      // COMMAND_REGISTER
/*  POLL  COMMAND_REGISTER::HOST_COMMAND_1 =>  0x00 */

//remark
#if 1
    {0xffff, POLL_DELAY,2},      // delay 
    {0x0982, 0x0001,2},      // ACCESS_CTL_STAT
    {0x098A, 0x6000,2},      // PHYSICAL_ADDRESS_ACCESS
    {0xE000, 0xC0F1,2},
    {0xE002, 0x0C3E,2},
    {0xE004, 0x08C0,2},
    {0xE006, 0xC1A3,2},
    {0xE008, 0x7508,2},
    {0xE00A, 0x90C3,2},
    {0xE00C, 0x71CF,2},
    {0xE00E, 0x0000,2},
    {0xE010, 0x0E5E,2},
    {0xE012, 0x7960,2},
    {0xE014, 0x90E1,2},
    {0xE016, 0x70CF,2},
    {0xE018, 0xFFFF,2},
    {0xE01A, 0xD078,2},
    {0xE01C, 0x9036,2},
    {0xE01E, 0x9019,2},
    {0xE020, 0x091F,2},
    {0xE022, 0x0003,2},
    {0xE024, 0xD915,2},
    {0xE026, 0x70CF,2},
    {0xE028, 0xFFFF,2},
    {0xE02A, 0xD2B8,2},
    {0xE02C, 0xA826,2},
    {0xE02E, 0xB5C3,2},
    {0xE030, 0xB5E1,2},
    {0xE032, 0xD800,2},
    {0xE034, 0x1C00,2},
    {0xE036, 0x3004,2},
    {0xE038, 0x0CB6,2},
    {0xE03A, 0x06E0,2},
    {0xE03C, 0x708B,2},
    {0xE03E, 0x0411,2},
    {0xE040, 0x08E0,2},
    {0xE042, 0xC0A3,2},
    {0xE044, 0xD900,2},
    {0xE046, 0xF00A,2},
    {0xE048, 0x70CF,2},
    {0xE04A, 0xFFFF,2},
    {0xE04C, 0xE060,2},
    {0xE04E, 0x7835,2},
    {0xE050, 0x8041,2},
    {0xE052, 0x8000,2},
    {0xE054, 0xE102,2},
    {0xE056, 0xA040,2},
    {0xE058, 0x09F1,2},
    {0xE05A, 0x8094,2},
    {0xE05C, 0x7FE0,2},
    {0xE05E, 0xD800,2},
    {0xE060, 0xFFFF,2},
    {0xE062, 0xDA84,2},
    {0xE064, 0xFFFF,2},
    {0xE066, 0xE000,2},
    {0x098E, 0x0000,2},      // LOGICAL_ADDRESS_ACCESS
    {0xE000, 0x0044,2},      // PATCHLDR_LOADER_ADDRESS
    {0xE002, 0x0005,2},      // PATCHLDR_PATCH_ID
    {0xE004, 0x51000000,4},      // PATCHLDR_FIRMWARE_ID
    {0x0080, 0xFFF0,1},      // COMMAND_REGISTER
    {0xffff, POLL_DELAY,2},      // delay
/*  POLL  COMMAND_REGISTER::HOST_COMMAND_0 =>  0x00 */
    {0x0080, 0xFFF1,2},      // COMMAND_REGISTER
    {0xffff, POLL_DELAY,2},      // delay
/*  POLL  COMMAND_REGISTER::HOST_COMMAND_0 =>  0x00 */

    {0x0982, 0x0001,2},      // ACCESS_CTL_STAT
    {0x098A, 0x6068,2},      // PHYSICAL_ADDRESS_ACCESS
    {0xE068, 0xC0F1,2},
    {0xE06A, 0xC5E1,2},
    {0xE06C, 0x75CF,2},
    {0xE06E, 0xFFFF,2},
    {0xE070, 0xD864,2},
    {0xE072, 0x9501,2},
    {0xE074, 0x2052,2},
    {0xE076, 0x0040,2},
    {0xE078, 0xB501,2},
    {0xE07A, 0x70CF,2},
    {0xE07C, 0x0000,2},
    {0xE07E, 0x9EEE,2},
    {0xE080, 0x7840,2},
    {0xE082, 0x9501,2},
    {0xE084, 0x2052,2},
    {0xE086, 0x0040,2},
    {0xE088, 0x03D5,2},
    {0xE08A, 0x08E0,2},
    {0xE08C, 0xB501,2},
    {0xE08E, 0x78E0,2},
    {0xE090, 0xC0F1,2},
    {0xE092, 0x0BAE,2},
    {0xE094, 0x08C0,2},
    {0xE096, 0xC1A3,2},
    {0xE098, 0xC040,2},
    {0xE09A, 0xC141,2},
    {0xE09C, 0x141E,2},
    {0xE09E, 0x310E,2},
    {0xE0A0, 0x7148,2},
    {0xE0A2, 0x7ACF,2},
    {0xE0A4, 0xC508,2},
    {0xE0A6, 0xC242,2},
    {0xE0A8, 0x6E52,2},
    {0xE0AA, 0xAD41,2},
    {0xE0AC, 0xADC0,2},
    {0xE0AE, 0x0BD6,2},
    {0xE0B0, 0x08E0,2},
    {0xE0B2, 0x6B15,2},
    {0xE0B4, 0x7F10,2},
    {0xE0B6, 0xB501,2},
    {0xE0B8, 0xC000,2},
    {0xE0BA, 0x0813,2},
    {0xE0BC, 0x03C3,2},
    {0xE0BE, 0xD900,2},
    {0xE0C0, 0x70CF,2},
    {0xE0C2, 0xFFFF,2},
    {0xE0C4, 0xD964,2},
    {0xE0C6, 0xB023,2},
    {0xE0C8, 0xF02F,2},
    {0xE0CA, 0x0BBA,2},
    {0xE0CC, 0x08E0,2},
    {0xE0CE, 0x71E9,2},
    {0xE0D0, 0x790F,2},
    {0xE0D2, 0x0E0D,2},
    {0xE0D4, 0x1062,2},
    {0xE0D6, 0xAD04,2},
    {0xE0D8, 0xC142,2},
    {0xE0DA, 0xF003,2},
    {0xE0DC, 0xADC4,2},
    {0xE0DE, 0xC001,2},
    {0xE0E0, 0x0BA2,2},
    {0xE0E2, 0x08E0,2},
    {0xE0E4, 0x71E9,2},
    {0xE0E6, 0x790F,2},
    {0xE0E8, 0xAD05,2},
    {0xE0EA, 0xC002,2},
    {0xE0EC, 0xC202,2},
    {0xE0EE, 0x78EC,2},
    {0xE0F0, 0x202F,2},
    {0xE0F2, 0x0E48,2},
    {0xE0F4, 0x1D06,2},
    {0xE0F6, 0x1E44,2},
    {0xE0F8, 0x0E0B,2},
    {0xE0FA, 0x1081,2},
    {0xE0FC, 0xDA00,2},
    {0xE0FE, 0xB544,2},
    {0xE100, 0xF008,2},
    {0xE102, 0xE101,2},
    {0xE104, 0xC201,2},
    {0xE106, 0x79EC,2},
    {0xE108, 0x2102,2},
    {0xE10A, 0x7081,2},
    {0xE10C, 0x7A30,2},
    {0xE10E, 0xB524,2},
    {0xE110, 0xEA89,2},
    {0xE112, 0xC100,2},
    {0xE114, 0x2102,2},
    {0xE116, 0x8001,2},
    {0xE118, 0x21C0,2},
    {0xE11A, 0x0064,2},
    {0xE11C, 0x793C,2},
    {0xE11E, 0xB525,2},
    {0xE120, 0xF003,2},
    {0xE122, 0xD800,2},
    {0xE124, 0xB505,2},
    {0xE126, 0x0329,2},
    {0xE128, 0x08E0,2},
    {0xE12A, 0xC0A3,2},
    {0xE12C, 0x1020,2},
    {0xE12E, 0x000A,2},
    {0xE130, 0x1204,2},
    {0xE132, 0x1083,2},
    {0xE134, 0xEA0F,2},
    {0xE136, 0x1200,2},
    {0xE138, 0x1082,2},
    {0xE13A, 0x1826,2},
    {0xE13C, 0x0082,2},
    {0xE13E, 0xDA00,2},
    {0xE140, 0x1827,2},
    {0xE142, 0x0082,2},
    {0xE144, 0x1202,2},
    {0xE146, 0x1102,2},
    {0xE148, 0xBA22,2},
    {0xE14A, 0xB056,2},
    {0xE14C, 0x120A,2},
    {0xE14E, 0x1102,2},
    {0xE150, 0xB057,2},
    {0xE152, 0x1206,2},
    {0xE154, 0x1102,2},
    {0xE156, 0x102E,2},
    {0xE158, 0x010B,2},
    {0xE15A, 0x7261,2},
    {0xE15C, 0x1028,2},
    {0xE15E, 0x010B,2},
    {0xE160, 0x0B09,2},
    {0xE162, 0x1083,2},
    {0xE164, 0xBB61,2},
    {0xE166, 0x7B70,2},
    {0xE168, 0x1026,2},
    {0xE16A, 0x0082,2},
    {0xE16C, 0x2309,2},
    {0xE16E, 0x0083,2},
    {0xE170, 0x2B40,2},
    {0xE172, 0x008B,2},
    {0xE174, 0x7A62,2},
    {0xE176, 0x1826,2},
    {0xE178, 0x0082,2},
    {0xE17A, 0x9056,2},
    {0xE17C, 0x1825,2},
    {0xE17E, 0x02C2,2},
    {0xE180, 0xB140,2},
    {0xE182, 0x9057,2},
    {0xE184, 0xB141,2},
    {0xE186, 0x1025,2},
    {0xE188, 0x0082,2},
    {0xE18A, 0xB142,2},
    {0xE18C, 0x9037,2},
    {0xE18E, 0x1208,2},
    {0xE190, 0x1102,2},
    {0xE192, 0x623A,2},
    {0xE194, 0x7950,2},
    {0xE196, 0xB057,2},
    {0xE198, 0x1202,2},
    {0xE19A, 0x1102,2},
    {0xE19C, 0x0A09,2},
    {0xE19E, 0x0043,2},
    {0xE1A0, 0x7942,2},
    {0xE1A2, 0xB037,2},
    {0xE1A4, 0xE380,2},
    {0xE1A6, 0xD801,2},
    {0xE1A8, 0x7FE0,2},
    {0xE1AA, 0x20C2,2},
    {0xE1AC, 0x000C,2},
    {0xE1AE, 0x78E0,2},
    {0xE1B0, 0x1098,2},
    {0xE1B2, 0x0103,2},
    {0xE1B4, 0xE301,2},
    {0xE1B6, 0x1898,2},
    {0xE1B8, 0x00C4,2},
    {0xE1BA, 0x1098,2},
    {0xE1BC, 0x010A,2},
    {0xE1BE, 0x7151,2},
    {0xE1C0, 0xDB00,2},
    {0xE1C2, 0x20CA,2},
    {0xE1C4, 0x00C9,2},
    {0xE1C6, 0xF609,2},
    {0xE1C8, 0x1898,2},
    {0xE1CA, 0x00C4,2},
    {0xE1CC, 0x70CF,2},
    {0xE1CE, 0xFFFF,2},
    {0xE1D0, 0xD964,2},
    {0xE1D2, 0xB043,2},
    {0xE1D4, 0xD801,2},
    {0xE1D6, 0x7EE0,2},
    {0xE1D8, 0x70CF,2},
    {0xE1DA, 0xFFFF,2},
    {0xE1DC, 0xD96C,2},
    {0xE1DE, 0x7EE0,2},
    {0xE1E0, 0xC0F1,2},
    {0xE1E2, 0x0A5E,2},
    {0xE1E4, 0x08C0,2},
    {0xE1E6, 0xC1A4,2},
    {0xE1E8, 0x7628,2},
    {0xE1EA, 0xC242,2},
    {0xE1EC, 0xC343,2},
    {0xE1EE, 0xFFFB,2},
    {0xE1F0, 0x7508,2},
    {0xE1F2, 0xDF01,2},
    {0xE1F4, 0x70CF,2},
    {0xE1F6, 0xFFFF,2},
    {0xE1F8, 0xD964,2},
    {0xE1FA, 0xB0E3,2},
    {0xE1FC, 0x70A9,2},
    {0xE1FE, 0x0DCA,2},
    {0xE200, 0x08E0,2},
    {0xE202, 0xD9B0,2},
    {0xE204, 0xD804,2},
    {0xE206, 0xAD00,2},
    {0xE208, 0xAD01,2},
    {0xE20A, 0xD810,2},
    {0xE20C, 0xC102,2},
    {0xE20E, 0xB501,2},
    {0xE210, 0xB905,2},
    {0xE212, 0x7830,2},
    {0xE214, 0xC203,2},
    {0xE216, 0xB534,2},
    {0xE218, 0xBA05,2},
    {0xE21A, 0xB555,2},
    {0xE21C, 0x7950,2},
    {0xE21E, 0xDA05,2},
    {0xE220, 0xC240,2},
    {0xE222, 0x2540,2},
    {0xE224, 0x1202,2},
    {0xE226, 0xC241,2},
    {0xE228, 0xDA64,2},
    {0xE22A, 0x73C9,2},
    {0xE22C, 0xFF99,2},
    {0xE22E, 0xD806,2},
    {0xE230, 0xC040,2},
    {0xE232, 0x2540,2},
    {0xE234, 0x1500,2},
    {0xE236, 0xC041,2},
    {0xE238, 0x9514,2},
    {0xE23A, 0x9535,2},
    {0xE23C, 0xDA78,2},
    {0xE23E, 0x73C9,2},
    {0xE240, 0xFF94,2},
    {0xE242, 0x70E9,2},
    {0xE244, 0x0209,2},
    {0xE246, 0x08E0,2},
    {0xE248, 0xC0A4,2},
    {0xE24A, 0x78E0,2},
    {0xE24C, 0xC0F1,2},
    {0xE24E, 0x09F6,2},
    {0xE250, 0x08C0,2},
    {0xE252, 0x7608,2},
    {0xE254, 0xFFE1,2},
    {0xE256, 0x88A7,2},
    {0xE258, 0x7079,2},
    {0xE25A, 0xD800,2},
    {0xE25C, 0x1B07,2},
    {0xE25E, 0x1002,2},
    {0xE260, 0x71CF,2},
    {0xE262, 0xFFFF,2},
    {0xE264, 0xD964,2},
    {0xE266, 0x9143,2},
    {0xE268, 0x0A0B,2},
    {0xE26A, 0x0051,2},
    {0xE26C, 0xD802,2},
    {0xE26E, 0xB103,2},
    {0xE270, 0xF01C,2},
    {0xE272, 0x0A1D,2},
    {0xE274, 0x0091,2},
    {0xE276, 0x0D35,2},
    {0xE278, 0x101E,2},
    {0xE27A, 0x1300,2},
    {0xE27C, 0x1081,2},
    {0xE27E, 0x7069,2},
    {0xE280, 0xDA03,2},
    {0xE282, 0xFFCC,2},
    {0xE284, 0xE080,2},
    {0xE286, 0xD801,2},
    {0xE288, 0x78C0,2},
    {0xE28A, 0xF02F,2},
    {0xE28C, 0x0A0F,2},
    {0xE28E, 0x00D1,2},
    {0xE290, 0x0D57,2},
    {0xE292, 0x101E,2},
    {0xE294, 0xD804,2},
    {0xE296, 0xB103,2},
    {0xE298, 0xF008,2},
    {0xE29A, 0x0A15,2},
    {0xE29C, 0x0111,2},
    {0xE29E, 0x0D0D,2},
    {0xE2A0, 0x101E,2},
    {0xE2A2, 0xDA05,2},
    {0xE2A4, 0xB143,2},
    {0xE2A6, 0xF021,2},
    {0xE2A8, 0xD801,2},
    {0xE2AA, 0xF01F,2},
    {0xE2AC, 0x0A25,2},
    {0xE2AE, 0x0151,2},
    {0xE2B0, 0x0D37,2},
    {0xE2B2, 0x101E,2},
    {0xE2B4, 0x0D13,2},
    {0xE2B6, 0x105E,2},
    {0xE2B8, 0xD804,2},
    {0xE2BA, 0xB103,2},
    {0xE2BC, 0x7069,2},
    {0xE2BE, 0x1301,2},
    {0xE2C0, 0x1081,2},
    {0xE2C2, 0xDA06,2},
    {0xE2C4, 0xF1DF,2},
    {0xE2C6, 0x1B98,2},
    {0xE2C8, 0x1004,2},
    {0xE2CA, 0xDA01,2},
    {0xE2CC, 0xB143,2},
    {0xE2CE, 0xF00D,2},
    {0xE2D0, 0x0A15,2},
    {0xE2D2, 0x0191,2},
    {0xE2D4, 0x0D13,2},
    {0xE2D6, 0x101E,2},
    {0xE2D8, 0x1302,2},
    {0xE2DA, 0x1101,2},
    {0xE2DC, 0x7069,2},
    {0xE2DE, 0xDA01,2},
    {0xE2E0, 0xFFB4,2},
    {0xE2E2, 0xF003,2},
    {0xE2E4, 0xB103,2},
    {0xE2E6, 0xE808,2},
    {0xE2E8, 0x7069,2},
    {0xE2EA, 0x71C9,2},
    {0xE2EC, 0x2553,2},
    {0xE2EE, 0x1002,2},
    {0xE2F0, 0xFF8F,2},
    {0xE2F2, 0xF003,2},
    {0xE2F4, 0xD800,2},
    {0xE2F6, 0x0161,2},
    {0xE2F8, 0x08C0,2},
    {0xE2FA, 0x78E0,2},
    {0xE2FC, 0xC0F1,2},
    {0xE2FE, 0x7208,2},
    {0xE300, 0xFFB6,2},
    {0xE302, 0x1025,2},
    {0xE304, 0x008B,2},
    {0xE306, 0x73CF,2},
    {0xE308, 0xFFFF,2},
    {0xE30A, 0xD964,2},
    {0xE30C, 0x2040,2},
    {0xE30E, 0x0C0A,2},
    {0xE310, 0x0B0F,2},
    {0xE312, 0x10A2,2},
    {0xE314, 0x9363,2},
    {0xE316, 0x1027,2},
    {0xE318, 0x0082,2},
    {0xE31A, 0x0A09,2},
    {0xE31C, 0x0612,2},
    {0xE31E, 0xDB00,2},
    {0xE320, 0x0B31,2},
    {0xE322, 0x0091,2},
    {0xE324, 0x1090,2},
    {0xE326, 0x0003,2},
    {0xE328, 0x090D,2},
    {0xE32A, 0x00E3,2},
    {0xE32C, 0x1027,2},
    {0xE32E, 0x0082,2},
    {0xE330, 0x1890,2},
    {0xE332, 0x0040,2},
    {0xE334, 0x1094,2},
    {0xE336, 0x0003,2},
    {0xE338, 0x0B09,2},
    {0xE33A, 0x0043,2},
    {0xE33C, 0x1894,2},
    {0xE33E, 0x0040,2},
    {0xE340, 0x2015,2},
    {0xE342, 0x008A,2},
    {0xE344, 0x1230,2},
    {0xE346, 0x1003,2},
    {0xE348, 0x6179,2},
    {0xE34A, 0x1A30,2},
    {0xE34C, 0x1040,2},
    {0xE34E, 0xF02E,2},
    {0xE350, 0x0B63,2},
    {0xE352, 0x0111,2},
    {0xE354, 0x1090,2},
    {0xE356, 0x0003,2},
    {0xE358, 0x0911,2},
    {0xE35A, 0x00E2,2},
    {0xE35C, 0x1027,2},
    {0xE35E, 0x0082,2},
    {0xE360, 0x1094,2},
    {0xE362, 0x0003,2},
    {0xE364, 0x0B07,2},
    {0xE366, 0x0043,2},
    {0xE368, 0x7168,2},
    {0xE36A, 0x22F0,2},
    {0xE36C, 0x1083,2},
    {0xE36E, 0x7962,2},
    {0xE370, 0x109A,2},
    {0xE372, 0x0083,2},
    {0xE374, 0x797A,2},
    {0xE376, 0x109C,2},
    {0xE378, 0x0003,2},
    {0xE37A, 0x7B2C,2},
    {0xE37C, 0x109B,2},
    {0xE37E, 0x0083,2},
    {0xE380, 0x2902,2},
    {0xE382, 0x70C1,2},
    {0xE384, 0x10A8,2},
    {0xE386, 0x0003,2},
    {0xE388, 0x792C,2},
    {0xE38A, 0x2102,2},
    {0xE38C, 0x00CA,2},
    {0xE38E, 0x10A4,2},
    {0xE390, 0x0003,2},
    {0xE392, 0x18A0,2},
    {0xE394, 0x0280,2},
    {0xE396, 0x18A8,2},
    {0xE398, 0x00C0,2},
    {0xE39A, 0x10AC,2},
    {0xE39C, 0x0001,2},
    {0xE39E, 0x18A4,2},
    {0xE3A0, 0x0280,2},
    {0xE3A2, 0x7127,2},
    {0xE3A4, 0x18AC,2},
    {0xE3A6, 0x0040,2},
    {0xE3A8, 0xE201,2},
    {0xE3AA, 0x1827,2},
    {0xE3AC, 0x0082,2},
    {0xE3AE, 0xD800,2},
    {0xE3B0, 0xF002,2},
    {0xE3B2, 0xD801,2},
    {0xE3B4, 0xC0D1,2},
    {0xE3B6, 0x7EE0,2},
    {0xE3B8, 0xC0F1,2},
    {0xE3BA, 0xFF88,2},
    {0xE3BC, 0x1026,2},
    {0xE3BE, 0x0081,2},
    {0xE3C0, 0xE984,2},
    {0xE3C2, 0x8827,2},
    {0xE3C4, 0xB980,2},
    {0xE3C6, 0xA827,2},
    {0xE3C8, 0xF1F6,2},
    {0xE3CA, 0x78E0,2},
    {0xE3CC, 0xC0F1,2},
    {0xE3CE, 0x0872,2},
    {0xE3D0, 0x08C0,2},
    {0xE3D2, 0xC1A1,2},
    {0xE3D4, 0xFF81,2},
    {0xE3D6, 0x7508,2},
    {0xE3D8, 0x8008,2},
    {0xE3DA, 0x8801,2},
    {0xE3DC, 0xC040,2},
    {0xE3DE, 0x70CF,2},
    {0xE3E0, 0xFFFF,2},
    {0xE3E2, 0xD964,2},
    {0xE3E4, 0x9003,2},
    {0xE3E6, 0xDE00,2},
    {0xE3E8, 0x0845,2},
    {0xE3EA, 0x0051,2},
    {0xE3EC, 0x2540,2},
    {0xE3EE, 0x1C00,2},
    {0xE3F0, 0x0BD6,2},
    {0xE3F2, 0x08E0,2},
    {0xE3F4, 0xD960,2},
    {0xE3F6, 0x70CF,2},
    {0xE3F8, 0xFFFF,2},
    {0xE3FA, 0xD78C,2},
    {0xE3FC, 0x1D90,2},
    {0xE3FE, 0x1F80,2},
    {0xE400, 0x7FFF,2},
    {0xE402, 0xFFFF,2},
    {0xE404, 0x1D94,2},
    {0xE406, 0x1F80,2},
    {0xE408, 0x8000,2},
    {0xE40A, 0x0000,2},
    {0xE40C, 0x9026,2},
    {0xE40E, 0x9011,2},
    {0xE410, 0x0913,2},
    {0xE412, 0x0001,2},
    {0xE414, 0x2540,2},
    {0xE416, 0x1500,2},
    {0xE418, 0xA508,2},
    {0xE41A, 0xD83C,2},
    {0xE41C, 0x1D24,2},
    {0xE41E, 0x1002,2},
    {0xE420, 0xF06E,2},
    {0xE422, 0x2540,2},
    {0xE424, 0x1200,2},
    {0xE426, 0xA508,2},
    {0xE428, 0xD832,2},
    {0xE42A, 0xF1FA,2},
    {0xE42C, 0x0887,2},
    {0xE42E, 0x0090,2},
    {0xE430, 0x087F,2},
    {0xE432, 0x00D1,2},
    {0xE434, 0xF009,2},
    {0xE436, 0x2515,2},
    {0xE438, 0x138F,2},
    {0xE43A, 0x870C,2},
    {0xE43C, 0x0846,2},
    {0xE43E, 0x08E0,2},
    {0xE440, 0x8D20,2},
    {0xE442, 0xA70C,2},
    {0xE444, 0xE601,2},
    {0xE446, 0xC000,2},
    {0xE448, 0x0EEF,2},
    {0xE44A, 0x9002,2},
    {0xE44C, 0x1590,2},
    {0xE44E, 0x100E,2},
    {0xE450, 0x1594,2},
    {0xE452, 0x100F,2},
    {0xE454, 0x2702,2},
    {0xE456, 0x1380,2},
    {0xE458, 0x2088,2},
    {0xE45A, 0x0901,2},
    {0xE45C, 0x0826,2},
    {0xE45E, 0x08E0,2},
    {0xE460, 0xD90A,2},
    {0xE462, 0x7E02,2},
    {0xE464, 0x1D90,2},
    {0xE466, 0x1380,2},
    {0xE468, 0x671F,2},
    {0xE46A, 0x1D94,2},
    {0xE46C, 0x13C0,2},
    {0xE46E, 0x2702,2},
    {0xE470, 0x1381,2},
    {0xE472, 0xDE00,2},
    {0xE474, 0x1D9A,2},
    {0xE476, 0x1382,2},
    {0xE478, 0xD800,2},
    {0xE47A, 0x1D9C,2},
    {0xE47C, 0x1F80,2},
    {0xE47E, 0x0004,2},
    {0xE480, 0x0000,2},
    {0xE482, 0xB892,2},
    {0xE484, 0xF008,2},
    {0xE486, 0x781B,2},
    {0xE488, 0x1D9C,2},
    {0xE48A, 0x1000,2},
    {0xE48C, 0x6E41,2},
    {0xE48E, 0x7E4F,2},
    {0xE490, 0x1D9A,2},
    {0xE492, 0x1082,2},
    {0xE494, 0x08F3,2},
    {0xE496, 0x8044,2},
    {0xE498, 0xD800,2},
    {0xE49A, 0x0FEA,2},
    {0xE49C, 0x08A0,2},
    {0xE49E, 0xB89E,2},
    {0xE4A0, 0x1D9C,2},
    {0xE4A2, 0x1000,2},
    {0xE4A4, 0x264E,2},
    {0xE4A6, 0x1480,2},
    {0xE4A8, 0x1D9B,2},
    {0xE4AA, 0x1002,2},
    {0xE4AC, 0xF019,2},
    {0xE4AE, 0x080B,2},
    {0xE4B0, 0x0111,2},
    {0xE4B2, 0xD800,2},
    {0xE4B4, 0xF02A,2},
    {0xE4B6, 0x083F,2},
    {0xE4B8, 0x0151,2},
    {0xE4BA, 0x15A4,2},
    {0xE4BC, 0x1000,2},
    {0xE4BE, 0x15A8,2},
    {0xE4C0, 0x1001,2},
    {0xE4C2, 0x780C,2},
    {0xE4C4, 0x702F,2},
    {0xE4C6, 0x792C,2},
    {0xE4C8, 0x15AC,2},
    {0xE4CA, 0x1001,2},
    {0xE4CC, 0x7027,2},
    {0xE4CE, 0xC200,2},
    {0xE4D0, 0xB942,2},
    {0xE4D2, 0x794C,2},
    {0xE4D4, 0x090B,2},
    {0xE4D6, 0x7003,2},
    {0xE4D8, 0x8D07,2},
    {0xE4DA, 0xB881,2},
    {0xE4DC, 0xAD07,2},
    {0xE4DE, 0xD800,2},
    {0xE4E0, 0x1DAC,2},
    {0xE4E2, 0x1000,2},
    {0xE4E4, 0x1DA8,2},
    {0xE4E6, 0x1000,2},
    {0xE4E8, 0x1DA4,2},
    {0xE4EA, 0x1000,2},
    {0xE4EC, 0x1DA0,2},
    {0xE4EE, 0x1000,2},
    {0xE4F0, 0xF009,2},
    {0xE4F2, 0x080D,2},
    {0xE4F4, 0x0191,2},
    {0xE4F6, 0x1598,2},
    {0xE4F8, 0x1100,2},
    {0xE4FA, 0xE804,2},
    {0xE4FC, 0x70C9,2},
    {0xE4FE, 0xF003,2},
    {0xE500, 0xD801,2},
    {0xE502, 0x8D27,2},
    {0xE504, 0xB980,2},
    {0xE506, 0xAD27,2},
    {0xE508, 0x0745,2},
    {0xE50A, 0x08A0,2},
    {0xE50C, 0xC0A1,2},
    {0xE50E, 0x78E0,2},
    {0xE510, 0xD900,2},
    {0xE512, 0xF00A,2},
    {0xE514, 0x70CF,2},
    {0xE516, 0xFFFF,2},
    {0xE518, 0xE538,2},
    {0xE51A, 0x7835,2},
    {0xE51C, 0x8041,2},
    {0xE51E, 0x8000,2},
    {0xE520, 0xE102,2},
    {0xE522, 0xA040,2},
    {0xE524, 0x09F1,2},
    {0xE526, 0x8314,2},
    {0xE528, 0x71CF,2},
    {0xE52A, 0xFFFF,2},
    {0xE52C, 0xD964,2},
    {0xE52E, 0x9101,2},
    {0xE530, 0xB8A7,2},
    {0xE532, 0xB101,2},
    {0xE534, 0x7FE0,2},
    {0xE536, 0xD800,2},
    {0xE538, 0xFFFF,2},
    {0xE53A, 0xDDB4,2},
    {0xE53C, 0xFFFF,2},
    {0xE53E, 0xE068,2},
    {0xE540, 0xFFFF,2},
    {0xE542, 0xDE10,2},
    {0xE544, 0xFFFF,2},
    {0xE546, 0xE1E0,2},
    {0xE548, 0xFFFF,2},
    {0xE54A, 0xDE14,2},
    {0xE54C, 0xFFFF,2},
    {0xE54E, 0xE24C,2},
    {0xE550, 0xFFFF,2},
    {0xE552, 0xDE18,2},
    {0xE554, 0xFFFF,2},
    {0xE556, 0xE2FC,2},
    {0xE558, 0xFFFF,2},
    {0xE55A, 0xDE1C,2},
    {0xE55C, 0xFFFF,2},
    {0xE55E, 0xE3B8,2},
    {0xE560, 0xFFFF,2},
    {0xE562, 0xDE24,2},
    {0xE564, 0xFFFF,2},
    {0xE566, 0xE3CC,2},
    
    {0x098E, 0x0000,2},      // LOGICAL_ADDRESS_ACCESS
    {0xE000, 0x0510,2},      // PATCHLDR_LOADER_ADDRESS
    {0xE002, 0x0105,2},      // PATCHLDR_PATCH_ID
    {0xE004, 0x51000000,4},      // PATCHLDR_FIRMWARE_ID
    {0x0080, 0xFFF0,2},      // COMMAND_REGISTER
    {0xffff, POLL_DELAY,2},      // delay
/*  POLL  COMMAND_REGISTER::HOST_COMMAND_0 =>  0x00 */
    {0x0080, 0xFFF1,2},      // COMMAND_REGISTER
/*  POLL  COMMAND_REGISTER::HOST_COMMAND_0 =>  0x00 */
    {0xffff, POLL_DELAY,2},      // delay
    {0x0982, 0x0001,2},      // ACCESS_CTL_STAT
    {0x098A, 0x6568,2},      // PHYSICAL_ADDRESS_ACCESS
    {0xE568, 0xC0F1,2},
    {0xE56A, 0xC5E1,2},
    {0xE56C, 0x75CF,2},
    {0xE56E, 0xFFFF,2},
    {0xE570, 0xD5C8,2},
    {0xE572, 0x0E42,2},
    {0xE574, 0x0820,2},
    {0xE576, 0x156C,2},
    {0xE578, 0x1100,2},
    {0xE57A, 0x71CF,2},
    {0xE57C, 0xFF00,2},
    {0xE57E, 0x41DC,2},
    {0xE580, 0x913B,2},
    {0xE582, 0x09FB,2},
    {0xE584, 0x821E,2},
    {0xE586, 0xE896,2},
    {0xE588, 0x9500,2},
    {0xE58A, 0xB882,2},
    {0xE58C, 0xB500,2},
    {0xE58E, 0x1568,2},
    {0xE590, 0x1100,2},
    {0xE592, 0xE001,2},
    {0xE594, 0x1D68,2},
    {0xE596, 0x1004,2},
    {0xE598, 0x1568,2},
    {0xE59A, 0x1100,2},
    {0xE59C, 0xE884,2},
    {0xE59E, 0xD801,2},
    {0xE5A0, 0x1D68,2},
    {0xE5A2, 0x1004,2},
    {0xE5A4, 0x208A,2},
    {0xE5A6, 0x0010,2},
    {0xE5A8, 0x0B1E,2},
    {0xE5AA, 0x06E0,2},
    {0xE5AC, 0xD901,2},
    {0xE5AE, 0xF004,2},
    {0xE5B0, 0x1D70,2},
    {0xE5B2, 0x1002,2},
    {0xE5B4, 0x06A9,2},
    {0xE5B6, 0x0880,2},
    {0xE5B8, 0xD900,2},
    {0xE5BA, 0xF00A,2},
    {0xE5BC, 0x70CF,2},
    {0xE5BE, 0xFFFF,2},
    {0xE5C0, 0xE5D4,2},
    {0xE5C2, 0x7835,2},
    {0xE5C4, 0x8041,2},
    {0xE5C6, 0x8000,2},
    {0xE5C8, 0xE102,2},
    {0xE5CA, 0xA040,2},
    {0xE5CC, 0x09F1,2},
    {0xE5CE, 0x8094,2},
    {0xE5D0, 0x7FE0,2},
    {0xE5D2, 0xD800,2},
    {0xE5D4, 0xFFFF,2},
    {0xE5D6, 0xDCA8,2},
    {0xE5D8, 0xFFFF,2},
    {0xE5DA, 0xE568,2},
    {0x098E, 0x0000,2},      // LOGICAL_ADDRESS_ACCESS
    {0xE000, 0x05B8,2},      // PATCHLDR_LOADER_ADDRESS
    {0xE002, 0x0205,2},      // PATCHLDR_PATCH_ID
    {0xE004, 0x51000000,4},      // PATCHLDR_FIRMWARE_ID
    {0x0080, 0xFFF0,2},      // COMMAND_REGISTER
    {0xffff, POLL_DELAY,2},      // delay
/*  POLL  COMMAND_REGISTER::HOST_COMMAND_0 =>  0x00 */
    {0x0080, 0xFFF1,2},      // COMMAND_REGISTER
    {0xffff, POLL_DELAY,2},      // delay
/*  POLL  COMMAND_REGISTER::HOST_COMMAND_0 =>  0x00 */
    {0x0982, 0x0001,2},      // ACCESS_CTL_STAT
    {0x098A, 0x65DC,2},      // PHYSICAL_ADDRESS_ACCESS
    {0xE5DC, 0xC0F1,2},
    {0xE5DE, 0x0E62,2},
    {0xE5E0, 0x0880,2},
    {0xE5E2, 0x75CF,2},
    {0xE5E4, 0xFFFF,2},
    {0xE5E6, 0xD78C,2},
    {0xE5E8, 0x85EF,2},
    {0xE5EA, 0x95C8,2},
    {0xE5EC, 0x70E9,2},
    {0xE5EE, 0x0E86,2},
    {0xE5F0, 0x08A0,2},
    {0xE5F2, 0x71C9,2},
    {0xE5F4, 0xE084,2},
    {0xE5F6, 0x20CA,2},
    {0xE5F8, 0x0125,2},
    {0xE5FA, 0xF783,2},
    {0xE5FC, 0x7810,2},
    {0xE5FE, 0x7A30,2},
    {0xE600, 0xB52E,2},
    {0xE602, 0xD980,2},
    {0xE604, 0xA52E,2},
    {0xE606, 0xD900,2},
    {0xE608, 0xB52B,2},
    {0xE60A, 0x9522,2},
    {0xE60C, 0xB50F,2},
    {0xE60E, 0x093D,2},
    {0xE610, 0x015E,2},
    {0xE612, 0x208C,2},
    {0xE614, 0x8002,2},
    {0xE616, 0xF65A,2},
    {0xE618, 0x73CF,2},
    {0xE61A, 0xFFFF,2},
    {0xE61C, 0xD03C,2},
    {0xE61E, 0x9327,2},
    {0xE620, 0x0A15,2},
    {0xE622, 0x0043,2},
    {0xE624, 0x0811,2},
    {0xE626, 0x0152,2},
    {0xE628, 0x6829,2},
    {0xE62A, 0x7830,2},
    {0xE62C, 0xB52F,2},
    {0xE62E, 0x62D9,2},
    {0xE630, 0x7A30,2},
    {0xE632, 0xB52E,2},
    {0xE634, 0x9328,2},
    {0xE636, 0x0915,2},
    {0xE638, 0x0083,2},
    {0xE63A, 0xB52E,2},
    {0xE63C, 0x78CC,2},
    {0xE63E, 0x7127,2},
    {0xE640, 0x0E32,2},
    {0xE642, 0x08A0,2},
    {0xE644, 0x6F17,2},
    {0xE646, 0xA50E,2},
    {0xE648, 0x0605,2},
    {0xE64A, 0x0880,2},
    {0xE64C, 0xD900,2},
    {0xE64E, 0xF00A,2},
    {0xE650, 0x70CF,2},
    {0xE652, 0xFFFF,2},
    {0xE654, 0xE668,2},
    {0xE656, 0x7835,2},
    {0xE658, 0x8041,2},
    {0xE65A, 0x8000,2},
    {0xE65C, 0xE102,2},
    {0xE65E, 0xA040,2},
    {0xE660, 0x09F1,2},
    {0xE662, 0x8094,2},
    {0xE664, 0x7FE0,2},
    {0xE666, 0xD800,2},
    {0xE668, 0xFFFF,2},
    {0xE66A, 0xDD18,2},
    {0xE66C, 0xFFFF,2},
    {0xE66E, 0xE5DC,2},
    {0x098E, 0x0000,2},      // LOGICAL_ADDRESS_ACCESS
    {0xE000, 0x064C,2},      // PATCHLDR_LOADER_ADDRESS
    {0xE002, 0x0305,2},      // PATCHLDR_PATCH_ID
    {0xE004, 0x51000000,4},      // PATCHLDR_FIRMWARE_ID
    {0x0080, 0xFFF0,2},      // COMMAND_REGISTER
    {0xffff, POLL_DELAY,2},      // delay
/*  POLL  COMMAND_REGISTER::HOST_COMMAND_0 =>  0x00 */
    {0x0080, 0xFFF1,2},      // COMMAND_REGISTER
/*  POLL  COMMAND_REGISTER::HOST_COMMAND_0 =>  0x00 */
    {0xffff, POLL_DELAY,2},      // delay
    {0x0982, 0x0001,2},      // ACCESS_CTL_STAT
    {0x098A, 0x6670,2},      // PHYSICAL_ADDRESS_ACCESS
    {0xE670, 0xC0F1,2},
    {0xE672, 0x0DCE,2},
    {0xE674, 0x08A0,2},
    {0xE676, 0x2256,2},
    {0xE678, 0x0802,2},
    {0xE67A, 0xC1A3,2},
    {0xE67C, 0x73CF,2},
    {0xE67E, 0xFFFF,2},
    {0xE680, 0xD2F0,2},
    {0xE682, 0x7F50,2},
    {0xE684, 0x220A,2},
    {0xE686, 0x1F80,2},
    {0xE688, 0xFFFF,2},
    {0xE68A, 0xD825,2},
    {0xE68C, 0x72CF,2},
    {0xE68E, 0xFFFF,2},
    {0xE690, 0xD1FC,2},
    {0xE692, 0xC040,2},
    {0xE694, 0x080D,2},
    {0xE696, 0x0281,2},
    {0xE698, 0x8B00,2},
    {0xE69A, 0xC041,2},
    {0xE69C, 0x8AA8,2},
    {0xE69E, 0xF005,2},
    {0xE6A0, 0x8B01,2},
    {0xE6A2, 0x8AA9,2},
    {0xE6A4, 0xC041,2},
    {0xE6A6, 0xD819,2},
    {0xE6A8, 0x0DDA,2},
    {0xE6AA, 0x08A0,2},
    {0xE6AC, 0xB80A,2},
    {0xE6AE, 0x7810,2},
    {0xE6B0, 0xC042,2},
    {0xE6B2, 0xDE00,2},
    {0xE6B4, 0xF006,2},
    {0xE6B6, 0xD800,2},
    {0xE6B8, 0xC100,2},
    {0xE6BA, 0x61D9,2},
    {0xE6BC, 0xE601,2},
    {0xE6BE, 0xA900,2},
    {0xE6C0, 0x0EB3,2},
    {0xE6C2, 0x14D5,2},
    {0xE6C4, 0x70CF,2},
    {0xE6C6, 0xFFFF,2},
    {0xE6C8, 0xE794,2},
    {0xE6CA, 0x8000,2},
    {0xE6CC, 0x60CB,2},
    {0xE6CE, 0x2302,2},
    {0xE6D0, 0x0340,2},
    {0xE6D2, 0x780E,2},
    {0xE6D4, 0x0B2F,2},
    {0xE6D6, 0x0363,2},
    {0xE6D8, 0x2D40,2},
    {0xE6DA, 0x1202,2},
    {0xE6DC, 0x71E9,2},
    {0xE6DE, 0x2182,2},
    {0xE6E0, 0x0008,2},
    {0xE6E2, 0x790C,2},
    {0xE6E4, 0x7227,2},
    {0xE6E6, 0x7A6C,2},
    {0xE6E8, 0xC201,2},
    {0xE6EA, 0x712F,2},
    {0xE6EC, 0x784C,2},
    {0xE6EE, 0x2904,2},
    {0xE6F0, 0x703E,2},
    {0xE6F2, 0x7127,2},
    {0xE6F4, 0x796C,2},
    {0xE6F6, 0x702F,2},
    {0xE6F8, 0x0D8A,2},
    {0xE6FA, 0x08A0,2},
    {0xE6FC, 0x71A9,2},
    {0xE6FE, 0x71A9,2},
    {0xE700, 0xF012,2},
    {0xE702, 0x250E,2},
    {0xE704, 0x1F81,2},
    {0xE706, 0x0000,2},
    {0xE708, 0x0100,2},
    {0xE70A, 0x792E,2},
    {0xE70C, 0x270E,2},
    {0xE70E, 0x1F83,2},
    {0xE710, 0x0000,2},
    {0xE712, 0x0100,2},
    {0xE714, 0x7B0C,2},
    {0xE716, 0x732F,2},
    {0xE718, 0x79EC,2},
    {0xE71A, 0x7327,2},
    {0xE71C, 0x7B0C,2},
    {0xE71E, 0x702F,2},
    {0xE720, 0x7A2C,2},
    {0xE722, 0x7027,2},
    {0xE724, 0x0D5E,2},
    {0xE726, 0x0880,2},
    {0xE728, 0xE080,2},
    {0xE72A, 0x0040,2},
    {0xE72C, 0x002C,2},
    {0xE72E, 0x20CA,2},
    {0xE730, 0x002C,2},
    {0xE732, 0x0B82,2},
    {0xE734, 0x0320,2},
    {0xE736, 0xD908,2},
    {0xE738, 0xC102,2},
    {0xE73A, 0x2099,2},
    {0xE73C, 0x0008,2},
    {0xE73E, 0xDC80,2},
    {0xE740, 0x782C,2},
    {0xE742, 0x210E,2},
    {0xE744, 0xF300,2},
    {0xE746, 0xDCFF,2},
    {0xE748, 0x20C0,2},
    {0xE74A, 0x0304,2},
    {0xE74C, 0xB848,2},
    {0xE74E, 0xD908,2},
    {0xE750, 0x0AF6,2},
    {0xE752, 0x0320,2},
    {0xE754, 0x7228,2},
    {0xE756, 0x7108,2},
    {0xE758, 0xB83F,2},
    {0xE75A, 0x6038,2},
    {0xE75C, 0x781C,2},
    {0xE75E, 0x70C7,2},
    {0xE760, 0x0001,2},
    {0xE762, 0x0000,2},
    {0xE764, 0x0D1E,2},
    {0xE766, 0x0880,2},
    {0xE768, 0x084F,2},
    {0xE76A, 0x8012,2},
    {0xE76C, 0x2089,2},
    {0xE76E, 0x0FC3,2},
    {0xE770, 0xF1A4,2},
    {0xE772, 0x04DD,2},
    {0xE774, 0x08A0,2},
    {0xE776, 0xC0A3,2},
    {0xE778, 0xD900,2},
    {0xE77A, 0xF00A,2},
    {0xE77C, 0x70CF,2},
    {0xE77E, 0xFFFF,2},
    {0xE780, 0xE798,2},
    {0xE782, 0x7835,2},
    {0xE784, 0x8041,2},
    {0xE786, 0x8000,2},
    {0xE788, 0xE102,2},
    {0xE78A, 0xA040,2},
    {0xE78C, 0x09F1,2},
    {0xE78E, 0x8094,2},
    {0xE790, 0x7FE0,2},
    {0xE792, 0xD800,2},
    {0xE794, 0x0001,2},
    {0xE796, 0x03A8,2},
    {0xE798, 0xFFFF,2},
    {0xE79A, 0xDD68,2},
    {0xE79C, 0xFFFF,2},
    {0xE79E, 0xE670,2},

    {0x098E, 0x0000,2},      // LOGICAL_ADDRESS_ACCESS
    {0xE000, 0x0778,2},      // PATCHLDR_LOADER_ADDRESS
    {0xE002, 0x0405,2},      // PATCHLDR_PATCH_ID
    {0xE004, 0x51000000,4},      // PATCHLDR_FIRMWARE_ID
    {0x0080, 0xFFF0,2},      // COMMAND_REGISTER
    {0xffff, POLL_DELAY,2},      // delay
/*  POLL  COMMAND_REGISTER::HOST_COMMAND_0 =>  0x00 */
    {0x0080, 0xFFF1,2},      // COMMAND_REGISTER
    {0xffff, POLL_DELAY,2},      // delay    
/*  POLL  COMMAND_REGISTER::HOST_COMMAND_0 =>  0x00 */
    {0x0982, 0x0001,2},      // ACCESS_CTL_STAT
    {0x098A, 0x6854,2},      // PHYSICAL_ADDRESS_ACCESS
    {0xE854, 0xC0F1,2},
    {0xE856, 0x0BEE,2},
    {0xE858, 0x0880,2},
    {0xE85A, 0x881C,2},
    {0xE85C, 0x082F,2},
    {0xE85E, 0x0211,2},
    {0xE860, 0x75CF,2},
    {0xE862, 0xFFFF,2},
    {0xE864, 0xC00C,2},
    {0xE866, 0x8D00,2},
    {0xE868, 0xDE00,2},
    {0xE86A, 0xE807,2},
    {0xE86C, 0x0A4E,2},
    {0xE86E, 0x0500,2},
    {0xE870, 0x08AE,2},
    {0xE872, 0x01C0,2},
    {0xE874, 0xADC0,2},
    {0xE876, 0x0AEA,2},
    {0xE878, 0x04E0,2},
    {0xE87A, 0xD800,2},
    {0xE87C, 0x0BC2,2},
    {0xE87E, 0x0600,2},
    {0xE880, 0x0C9A,2},
    {0xE882, 0x00C0,2},
    {0xE884, 0x0A2E,2},
    {0xE886, 0x01E0,2},
    {0xE888, 0xD822,2},
    {0xE88A, 0x03CD,2},
    {0xE88C, 0x0880,2},
    {0xE88E, 0x78E0,2},
    {0xE890, 0x883C,2},
    {0xE892, 0x0921,2},
    {0xE894, 0x0211,2},
    {0xE896, 0x72CF,2},
    {0xE898, 0xFFFF,2},
    {0xE89A, 0xD034,2},
    {0xE89C, 0x8A20,2},
    {0xE89E, 0x0911,2},
    {0xE8A0, 0x001E,2},
    {0xE8A2, 0x8A20,2},
    {0xE8A4, 0xB9E1,2},
    {0xE8A6, 0x0424,2},
    {0xE8A8, 0x01C2,2},
    {0xE8AA, 0xF1D6,2},
    {0xE8AC, 0x0469,2},
    {0xE8AE, 0x01C0,2},
    {0xE8B0, 0x7EE0,2},
    {0xE8B2, 0x78E0,2},
    {0xE8B4, 0xD900,2},
    {0xE8B6, 0xF00A,2},
    {0xE8B8, 0x70CF,2},
    {0xE8BA, 0xFFFF,2},
    {0xE8BC, 0xE8D0,2},
    {0xE8BE, 0x7835,2},
    {0xE8C0, 0x8041,2},
    {0xE8C2, 0x8000,2},
    {0xE8C4, 0xE102,2},
    {0xE8C6, 0xA040,2},
    {0xE8C8, 0x09F1,2},
    {0xE8CA, 0x8094,2},
    {0xE8CC, 0x7FE0,2},
    {0xE8CE, 0xD800,2},
    {0xE8D0, 0xFFFF,2},
    {0xE8D2, 0xDB2C,2},
    {0xE8D4, 0xFFFF,2},
    {0xE8D6, 0xE890,2},
    {0x098E, 0x0000,2},      // LOGICAL_ADDRESS_ACCESS
    {0xE000, 0x08B4,2},      // PATCHLDR_LOADER_ADDRESS
    {0xE002, 0x0605,2},      // PATCHLDR_PATCH_ID
    {0xE004, 0x51000000,4},      // PATCHLDR_FIRMWARE_ID
    {0x0080, 0xFFF0,2},      // COMMAND_REGISTER
    {0xffff, POLL_DELAY,2},      // delay
/*  POLL  COMMAND_REGISTER::HOST_COMMAND_0 =>  0x00 */
    {0x0080, 0xFFF1,2},      // COMMAND_REGISTER
    {0xffff, POLL_DELAY,2},      // delay
/*  POLL  COMMAND_REGISTER::HOST_COMMAND_0 =>  0x00 */
    {0x0982, 0x0001,2},      // ACCESS_CTL_STAT
    {0x098A, 0x68D8,2},      // PHYSICAL_ADDRESS_ACCESS
    {0xE8D8, 0xC0F1,2},
    {0xE8DA, 0x0911,2},
    {0xE8DC, 0x0450,2},
    {0xE8DE, 0xDA00,2},
    {0xE8E0, 0x0911,2},
    {0xE8E2, 0x0A50,2},
    {0xE8E4, 0xD800,2},
    {0xE8E6, 0xF004,2},
    {0xE8E8, 0x0FFA,2},
    {0xE8EA, 0x0300,2},
    {0xE8EC, 0xC0D1,2},
    {0xE8EE, 0x7EE0,2},
    {0xE8F0, 0x70CF,2},
    {0xE8F2, 0xFFFF,2},
    {0xE8F4, 0xD21C,2},
    {0xE8F6, 0x9009,2},
    {0xE8F8, 0x2086,2},
    {0xE8FA, 0x8BBF,2},
    {0xE8FC, 0xF3F4,2},
    {0xE8FE, 0xD903,2},
    {0xE900, 0x70CF,2},
    {0xE902, 0xFFFF,2},
    {0xE904, 0xD340,2},
    {0xE906, 0xA839,2},
    {0xE908, 0x0A7A,2},
    {0xE90A, 0x0360,2},
    {0xE90C, 0xA858,2},
    {0xE90E, 0xF1EC,2},
    {0xE910, 0xD900,2},
    {0xE912, 0xF00A,2},
    {0xE914, 0x70CF,2},
    {0xE916, 0xFFFF,2},
    {0xE918, 0xE92C,2},
    {0xE91A, 0x7835,2},
    {0xE91C, 0x8041,2},
    {0xE91E, 0x8000,2},
    {0xE920, 0xE102,2},
    {0xE922, 0xA040,2},
    {0xE924, 0x09F1,2},
    {0xE926, 0x8094,2},
    {0xE928, 0x7FE0,2},
    {0xE92A, 0xD800,2},
    {0xE92C, 0xFFFF,2},
    {0xE92E, 0xDBF8,2},
    {0xE930, 0xFFFF,2},
    {0xE932, 0xE8D8,2},
    {0x098E, 0x0000,2},      // LOGICAL_ADDRESS_ACCESS
    {0xE000, 0x0910,2},      // PATCHLDR_LOADER_ADDRESS
    {0xE002, 0x0705,2},      // PATCHLDR_PATCH_ID
    {0xE004, 0x51000000,4},      // PATCHLDR_FIRMWARE_ID
    {0x0080, 0xFFF0,2},      // COMMAND_REGISTER
    {0xffff, POLL_DELAY,2},      // delay
/*  POLL  COMMAND_REGISTER::HOST_COMMAND_0 =>  0x00 */
    {0x0080, 0xFFF1,2},      // COMMAND_REGISTER
    {0xffff, POLL_DELAY,2},      // delay
/*  POLL  COMMAND_REGISTER::HOST_COMMAND_0 =>  0x00 */
    {0x0982, 0x0001,2},      // ACCESS_CTL_STAT
    {0x098A, 0x6934,2},      // PHYSICAL_ADDRESS_ACCESS
    {0xE934, 0xC0F1,2},
    {0xE936, 0x71CF,2},
    {0xE938, 0x0000,2},
    {0xE93A, 0x997E,2},
    {0xE93C, 0x7940,2},
    {0xE93E, 0xD900,2},
    {0xE940, 0x70CF,2},
    {0xE942, 0xFF00,2},
    {0xE944, 0x2274,2},
    {0xE946, 0xB03A,2},
    {0xE948, 0xC0D1,2},
    {0xE94A, 0x7EE0,2},
    {0xE94C, 0xD900,2},
    {0xE94E, 0xF00A,2},
    {0xE950, 0x70CF,2},
    {0xE952, 0xFFFF,2},
    {0xE954, 0xE970,2},
    {0xE956, 0x7835,2},
    {0xE958, 0x8041,2},
    {0xE95A, 0x8000,2},
    {0xE95C, 0xE102,2},
    {0xE95E, 0xA040,2},
    {0xE960, 0x09F1,2},
    {0xE962, 0x8094,2},
    {0xE964, 0xD800,2},
    {0xE966, 0x71CF,2},
    {0xE968, 0xFF00,2},
    {0xE96A, 0x2274,2},
    {0xE96C, 0xB11A,2},
    {0xE96E, 0x7EE0,2},
    {0xE970, 0xFFFF,2},
    {0xE972, 0xDD94,2},
    {0xE974, 0xFFFF,2},
    {0xE976, 0xE934,2},
    {0x098E, 0x0000,2},      // LOGICAL_ADDRESS_ACCESS
    {0xE000, 0x094C,2},      // PATCHLDR_LOADER_ADDRESS
    {0xE002, 0x0805,2},      // PATCHLDR_PATCH_ID
    {0xE004, 0x51000000,4},      // PATCHLDR_FIRMWARE_ID
    {0x0080, 0xFFF0,2},      // COMMAND_REGISTER
    {0xffff, POLL_DELAY,2},      // delay
/*  POLL  COMMAND_REGISTER::HOST_COMMAND_0 =>  0x00 */
    {0x0080, 0xFFF1,2},      // COMMAND_REGISTER
    {0xffff, POLL_DELAY,2},      // delay
/*  POLL  COMMAND_REGISTER::HOST_COMMAND_0 =>  0x00 */
    {0x0982, 0x0001,2},      // ACCESS_CTL_STAT
    {0x098A, 0x6978,2},      // PHYSICAL_ADDRESS_ACCESS
    {0xE978, 0xC0F1,2},
    {0xE97A, 0x0CBA,2},
    {0xE97C, 0x01E0,2},
    {0xE97E, 0xC5E1,2},
    {0xE980, 0x0D52,2},
    {0xE982, 0x0860,2},
    {0xE984, 0xDD01,2},
    {0xE986, 0x20AA,2},
    {0xE988, 0x00C1,2},
    {0xE98A, 0x2053,2},
    {0xE98C, 0x8040,2},
    {0xE98E, 0x25C2,2},
    {0xE990, 0x1341,2},
    {0xE992, 0xF40A,2},
    {0xE994, 0x0D3E,2},
    {0xE996, 0x0840,2},
    {0xE998, 0x71CF,2},
    {0xE99A, 0xFFFF,2},
    {0xE99C, 0xC000,2},
    {0xE99E, 0x8100,2},
    {0xE9A0, 0xE001,2},
    {0xE9A2, 0xA100,2},
    {0xE9A4, 0x20AA,2},
    {0xE9A6, 0x0F7F,2},
    {0xE9A8, 0x08FD,2},
    {0xE9AA, 0x819F,2},
    {0xE9AC, 0x70CF,2},
    {0xE9AE, 0xFFFF,2},
    {0xE9B0, 0xD864,2},
    {0xE9B2, 0x0CBE,2},
    {0xE9B4, 0x0760,2},
    {0xE9B6, 0x9005,2},
    {0xE9B8, 0xED8A,2},
    {0xE9BA, 0x71CF,2},
    {0xE9BC, 0xFFFF,2},
    {0xE9BE, 0xC000,2},
    {0xE9C0, 0x8100,2},
    {0xE9C2, 0x2080,2},
    {0xE9C4, 0x8FFF,2},
    {0xE9C6, 0xA100,2},
    {0xE9C8, 0x0D38,2},
    {0xE9CA, 0x0841,2},
    {0xE9CC, 0x0291,2},
    {0xE9CE, 0x0880,2},
    {0xE9D0, 0xC0F1,2},
    {0xE9D2, 0xC5E1,2},
    {0xE9D4, 0x0CFE,2},
    {0xE9D6, 0x0860,2},
    {0xE9D8, 0xDD01,2},
    {0xE9DA, 0x20AA,2},
    {0xE9DC, 0x00C1,2},
    {0xE9DE, 0x2053,2},
    {0xE9E0, 0x8040,2},
    {0xE9E2, 0x25C2,2},
    {0xE9E4, 0x1341,2},
    {0xE9E6, 0xF40A,2},
    {0xE9E8, 0x0CEA,2},
    {0xE9EA, 0x0840,2},
    {0xE9EC, 0x71CF,2},
    {0xE9EE, 0xFFFF,2},
    {0xE9F0, 0xC000,2},
    {0xE9F2, 0x8100,2},
    {0xE9F4, 0xE001,2},
    {0xE9F6, 0xA100,2},
    {0xE9F8, 0x20AA,2},
    {0xE9FA, 0x0F7F,2},
    {0xE9FC, 0x08FD,2},
    {0xE9FE, 0x819F,2},
    {0xEA00, 0x0D4E,2},
    {0xEA02, 0x0740,2},
    {0xEA04, 0xED8A,2},
    {0xEA06, 0x71CF,2},
    {0xEA08, 0xFFFF,2},
    {0xEA0A, 0xC000,2},
    {0xEA0C, 0x8100,2},
    {0xEA0E, 0x2080,2},
    {0xEA10, 0x8FFF,2},
    {0xEA12, 0xA100,2},
    {0xEA14, 0x0CEC,2},
    {0xEA16, 0x0841,2},
    {0xEA18, 0x0C32,2},
    {0xEA1A, 0x01C0,2},
    {0xEA1C, 0x0241,2},
    {0xEA1E, 0x0880,2},
    {0xEA20, 0xC0F1,2},
    {0xEA22, 0x71CF,2},
    {0xEA24, 0xFFFF,2},
    {0xEA26, 0xE978,2},
    {0xEA28, 0xD814,2},
    {0xEA2A, 0x0D4E,2},
    {0xEA2C, 0x06A0,2},
    {0xEA2E, 0xDA00,2},
    {0xEA30, 0x71CF,2},
    {0xEA32, 0xFFFF,2},
    {0xEA34, 0xE9D0,2},
    {0xEA36, 0xD813,2},
    {0xEA38, 0x0D3E,2},
    {0xEA3A, 0x06A0,2},
    {0xEA3C, 0xDA00,2},
    {0xEA3E, 0xD800,2},
    {0xEA40, 0xC0D1,2},
    {0xEA42, 0x7EE0,2},
    {0x098E, 0x0000,2},      // LOGICAL_ADDRESS_ACCESS
    {0xE000, 0x0A20,2},      // PATCHLDR_LOADER_ADDRESS
    {0xE002, 0x0905,2},      // PATCHLDR_PATCH_ID
    {0xE004, 0x51000000,4},      // PATCHLDR_FIRMWARE_ID
    {0x0080, 0xFFF0,2},      // COMMAND_REGISTER
    {0xffff, POLL_DELAY,2},      // delay
/*  POLL  COMMAND_REGISTER::HOST_COMMAND_0 =>  0x00 */
    {0x0080, 0xFFF1,2},      // COMMAND_REGISTER
    {0xffff, POLL_DELAY,2},      // delay
/*  POLL  COMMAND_REGISTER::HOST_COMMAND_0 =>  0x00 */
#endif
    {0xBC02, 0x0013,2},      // LL_MODE
    {0xC960, 0x0003,2},      // CAM_LL_LLMODE
    {0xBC08, 0x00,1},      // LL_GAMMA_SELECT
    {0xC9C6, 0xC0,1},      // CAM_LL_START_CONTRAST_GRADIENT
    {0xC9C7, 0x60,1},      // CAM_LL_STOP_CONTRAST_GRADIENT
    {0xC9C8, 0x0F,1},      // CAM_LL_START_CONTRAST_LUMA_PERCENTAGE
    {0xC9C9, 0x2C,1},      // CAM_LL_STOP_CONTRAST_LUMA_PERCENTAGE
    {0xE400, 0x00,1},      // PATCHVARS_START_ORIGIN_GRADIENT
    {0xE401, 0x00,1},      // PATCHVARS_STOP_ORIGIN_GRADIENT
    {0xC9C0, 0x0012,2},      // CAM_LL_START_CONTRAST_BM
    {0xC9C2, 0x00B3,2},      // CAM_LL_STOP_CONTRAST_BM
    {0xffff, 200,2},
};

struct as0260_reg const as0260_refresh_reg[] =
{
	{0xffff, 100},      // LOGICAL_ADDRESS_ACCESS 

	{0xDC00, 0x0028}, 	// SYSMGR_NEXT_STATE

	{0x098E, 0xDC00},      // LOGICAL_ADDRESS_ACCESS 
	{0xDC00, 0x0028}, 	// SYSMGR_NEXT_STATE
};


struct as0260_reg const as0260_configChange_regs[] =
{
//	{0x098E, 0xDC00},      // LOGICAL_ADDRESS_ACCESS 
	{0xDC00, 0x0028}, 	// SYSMGR_NEXT_STATE
};


struct as0260_reg const as0260_enterStreaming_regs[] =
{
	{0x098E, 0xDC00},      // LOGICAL_ADDRESS_ACCESS 
	{0xDC00, 0x0034}, 	// SYSMGR_NEXT_STATE
};

struct as0260_reg const as0260_enterSuspend_regs[] =
{
//	{0x098E, 0xDC00},      // LOGICAL_ADDRESS_ACCESS 
	{0xDC00, 0x0040}, 	// SYSMGR_NEXT_STATE
};

struct as0260_reg const as0260_enterStanby_regs[] =
{
//	{0x098E, 0xDC00},      // LOGICAL_ADDRESS_ACCESS 
	{0xDC00, 0x0050}, 	// SYSMGR_NEXT_STATE
};

struct as0260_reg const as0260_exitStanby_regs[] =
{
//	{0x098E, 0xDC00},      // LOGICAL_ADDRESS_ACCESS 
	{0xDC00, 0x0054}, 	// SYSMGR_NEXT_STATE
};

struct as0260_reg const as0260_preview_regs[] =
{
	{0x098E, 0xCA12 	},      // LOGICAL_ADDRESS_ACCESS [CAM_SYSCTL_PLL_ENABLE]
	{0xCA12, 0x01 	},      // CAM_SYSCTL_PLL_ENABLE
	{0xCA13, 0x00 	},      // CAM_SYSCTL_SENSOR_CLK_DIV2_EN
	{0xCA14, 0x0120 	},      // CAM_SYSCTL_PLL_DIVIDER_M_N
	{0xCA16, 0x0070 	},      // CAM_SYSCTL_PLL_DIVIDER_P
	{0xCA18, 0x7F7D 	},      // CAM_SYSCTL_PLL_DIVIDER_P4_P5_P6
	{0xCA1A, 0x1007 	},      // CAM_SYSCTL_PLL_DIVIDER_P7
	{0xC808, 0x03450DB6 	},      // CAM_SENSOR_CFG_PIXCLK
	{0xC800, 0x0020 	},      // CAM_SENSOR_CFG_Y_ADDR_START
	{0xC802, 0x0100 	},      // CAM_SENSOR_CFG_X_ADDR_START
	{0xC804, 0x045D 	},      // CAM_SENSOR_CFG_Y_ADDR_END
	{0xC806, 0x06AD 	},      // CAM_SENSOR_CFG_X_ADDR_END
	{0xC80E, 0x06A4 	},      // CAM_SENSOR_CFG_FINE_INTEG_TIME_MIN
	{0xC810, 0x085E 	},      // CAM_SENSOR_CFG_FINE_INTEG_TIME_MAX
	{0xC812, 0x026D 	},      // CAM_SENSOR_CFG_FRAME_LENGTH_LINES
	{0xC814, 0x0B80 	},      // CAM_SENSOR_CFG_LINE_LENGTH_PCK
	{0xC816, 0x01D9 	},      // CAM_SENSOR_CFG_FINE_CORRECTION
	{0xC818, 0x021B 	},      // CAM_SENSOR_CFG_CPIPE_LAST_ROW
	{0xC830, 0x0011 	},      // CAM_SENSOR_CONTROL_READ_MODE
	{0xC858, 0x0000 	},      // CAM_CROP_WINDOW_XOFFSET
	{0xC85A, 0x0000 	},      // CAM_CROP_WINDOW_YOFFSET
	{0xC85C, 0x02D0 	},      // CAM_CROP_WINDOW_WIDTH
	{0xC85E, 0x0218 	},      // CAM_CROP_WINDOW_HEIGHT
	{0xC86C, 0x0280 	},      // CAM_OUTPUT_WIDTH
	{0xC86E, 0x01E0 	},      // CAM_OUTPUT_HEIGHT
	{0xC88E, 0x3C02 	},      // CAM_AET_MAX_FRAME_RATE
	{0xC890, 0x3C00 	},      // CAM_AET_MIN_FRAME_RATE
	{0xC94C, 0x0000 	},      // CAM_STAT_AWB_CLIP_WINDOW_XSTART
	{0xC94E, 0x0000 	},      // CAM_STAT_AWB_CLIP_WINDOW_YSTART
	{0xC950, 0x027F 	},      // CAM_STAT_AWB_CLIP_WINDOW_XEND
	{0xC952, 0x01DF 	},      // CAM_STAT_AWB_CLIP_WINDOW_YEND
	{0xC954, 0x0000 	},      // CAM_STAT_AE_INITIAL_WINDOW_XSTART
	{0xC956, 0x0000 	},      // CAM_STAT_AE_INITIAL_WINDOW_YSTART
	{0xC958, 0x007F 	},      // CAM_STAT_AE_INITIAL_WINDOW_XEND
	{0xC95A, 0x005F 	},      // CAM_STAT_AE_INITIAL_WINDOW_YEND
	{0xDC00, 0x28 	},      // SYSMGR_NEXT_STATE
	{0x0080, 0x8002 	},      // COMMAND_REGISTER
	{0xffff, 100 	},      // COMMAND_REGISTER
};

#endif

