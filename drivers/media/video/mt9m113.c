/* linux/drivers/media/video/mt9m113.c
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/i2c.h>
#include <linux/delay.h>
#include <linux/version.h>
#include <media/v4l2-device.h>
#include <media/v4l2-subdev.h>
#include <linux/slab.h>
#ifdef CONFIG_VIDEO_SAMSUNG_V4L2
#include <linux/videodev2_samsung.h>
#endif
#include <media/mt9m113_platform.h>

#include "mt9m113.h"

#ifdef MT9M113_DEBUG
#define mt9m113_info dev_info
#define mt9m113_err dev_err
#else
#define mt9m113_info(dev, format, arg...) do { } while (0)
#define mt9m113_err dev_err
#endif

static DEFINE_MUTEX(sensor_s_ctrl);

#if 0
static int mt9m113_i2c_read(struct v4l2_subdev *sd, unsigned char *rxdata, int length)
{
	struct i2c_client *client = v4l2_get_subdevdata(sd);
	struct i2c_msg msgs[] = {
		{
			.addr  = client->addr >> 1,
			.flags = 0,
			.len   = 2,
			.buf   = rxdata,
		},
		{
			.addr  = client->addr >> 1,
			.flags = I2C_M_RD,
			.len   = length,
			.buf   = rxdata,
		},
	};
	int I2C_RETRY = 0;

	for(; I2C_RETRY < 5; I2C_RETRY++) {
		if (i2c_transfer(client->adapter, msgs, 2) < 0) {
			if(4==I2C_RETRY) {
				mt9m113_err(&client->dev, "%s: failed to transfer to addr:0x%x\n", __func__, client->addr);
				return -EIO;
			}
			mdelay(10);
		} else {
			break;
		}
	}

	return 0;
}
#endif

static int mt9m113_i2c_write(struct v4l2_subdev *sd, unsigned char *txdata, int length)
{
	struct i2c_client *client = v4l2_get_subdevdata(sd);
	struct i2c_msg msg[] = {
		{
			.addr = client->addr >> 1,
			.flags = 0,
			.len = length,
			.buf = txdata,
		},
	};
	int I2C_RETRY = 0;

	for (; I2C_RETRY < 2; I2C_RETRY++) {
		if (i2c_transfer(client->adapter, msg, 1) < 0) {
			if (1 == I2C_RETRY) {
				mt9m113_err(&client->dev, "%s: failed to transfer to addr:0x%x\n", __func__, client->addr);
				return -EIO;
			}
			mdelay(10);
		} else {
			break;
		}
	}

	return 0;
}

static int mt9m113_i2c_write_regs(struct v4l2_subdev *sd,
	struct mt9m113_reg const *reg_tbl, int num_of_items_in_table) {
	int i;
	int err = -EIO;
	unsigned char buf[4];

	for (i = 0; i < num_of_items_in_table; i++) {
		buf[0] = (reg_tbl->waddr & 0xFF00) >> 8;
		buf[1] = (reg_tbl->waddr & 0x00FF);
		buf[2] = (reg_tbl->wdata & 0xFF00) >> 8;
		buf[3] = (reg_tbl->wdata & 0x00FF);

		err = mt9m113_i2c_write(sd, buf, 4);
		if (err < 0)
			break;

		if (reg_tbl->mdelay_time != 0)
			mdelay(reg_tbl->mdelay_time);

		reg_tbl++;
	}
	return err;
}

static int mt9m113_set_brightness(struct v4l2_subdev *sd, struct v4l2_control *ctrl)
{
	struct i2c_client *client = v4l2_get_subdevdata(sd);
	int err = -EIO;

	if (ctrl->value < EV_MINUS_4 || ctrl->value > EV_PLUS_4) {
		mt9m113_err(&client->dev, "%s : there's no brightness value with [%d]\n", __func__,ctrl->value);
		return -EINVAL;
	}

	mt9m113_info(&client->dev, "%s: value %d\n", __func__, ctrl->value);
	mt9m113_bright_regs[1].wdata = mt9m113_brightness[ctrl->value][0];
	mt9m113_bright_regs[3].wdata = mt9m113_brightness[ctrl->value][1];
	if(mt9m113_bright_regs[3].wdata == 0)
		err = mt9m113_i2c_write_regs(sd, &mt9m113_bright_regs[0], 2);
	else
		err = mt9m113_i2c_write_regs(sd, &mt9m113_bright_regs[0], 4);

	if (err < 0)
		mt9m113_err(&client->dev, "%s : i2c_write for set brightness\n", __func__);

	return err;
}

static int mt9m113_set_white_balance(struct v4l2_subdev *sd, struct v4l2_control *ctrl)
{
	struct i2c_client *client = v4l2_get_subdevdata(sd);
	int err = -EIO;
	int value = ctrl->value - 1;

	if (ctrl->value < WHITE_BALANCE_AUTO || ctrl->value > WHITE_BALANCE_FLUORESCENT) {
		mt9m113_err(&client->dev, "%s: unsupported wb(%d) value.\n", __func__, ctrl->value);
		return -EIO;
	}

	mt9m113_info(&client->dev, "%s: value %d\n", __func__, ctrl->value);
	if (mt9m113_wb_tbl[value].size != 0) {
	    err = mt9m113_i2c_write_regs(sd, &mt9m113_wb_tbl[value].mt9m113_regs[0], mt9m113_wb_tbl[value].size);
	}

	if (err < 0)
		mt9m113_err(&client->dev, "%s : i2c_write for set WB\n", __func__);

	return err;
}

static int mt9m113_set_effect(struct v4l2_subdev *sd, struct v4l2_control *ctrl)
{
	struct i2c_client *client = v4l2_get_subdevdata(sd);
	int err = -EIO;
	int value = ctrl->value - 1;

	if (ctrl->value < IMAGE_EFFECT_NONE || ctrl->value > IMAGE_EFFECT_SHARPEN) {
		mt9m113_err(&client->dev, "%s: unsupported effect(%d) value.\n", __func__, ctrl->value);
		return -EIO;
	}

	mt9m113_info(&client->dev, "%s: value %d\n", __func__, ctrl->value);
	if (mt9m113_effect_tbl[value].size != 0) {
	    err = mt9m113_i2c_write_regs(sd, &mt9m113_effect_tbl[value].mt9m113_regs[0], mt9m113_effect_tbl[value].size);
	}

	if (err < 0)
		mt9m113_err(&client->dev, "%s : i2c_write for set effect\n", __func__);

	return err;
}

static int mt9m113_set_blur(struct v4l2_subdev *sd, struct v4l2_control *ctrl)
{
	struct i2c_client *client = v4l2_get_subdevdata(sd);
	int err = 0;

	/* TBD */
	mt9m113_err(&client->dev, "%s : not supported [%d]\n", __func__,ctrl->value);

	if (err < 0)
		mt9m113_err(&client->dev, "%s : i2c_write for set blur\n", __func__);

	return err;
}

//static int mt9m113_set_flip(struct v4l2_subdev *sd, struct v4l2_control *ctrl)
//{
//	struct i2c_client *client = v4l2_get_subdevdata(sd);
//	struct mt9m113_state *state = to_state(sd);
//	int err = 0;
//
//	if(state->runmode != MT9M113_RUNMODE_RUNNING)
//		return 0;
//
//	/* TBD */
//	mt9m113_err(&client->dev, "%s : not supported [%d]\n", __func__,ctrl->value);
//
//	if (err < 0)
//		mt9m113_err(&client->dev, "%s : i2c_write for set flip\n", __func__);
//
//	return err;
//}

static int mt9m113_set_frame_rate(struct v4l2_subdev *sd, struct v4l2_control *ctrl)
{
	struct i2c_client *client = v4l2_get_subdevdata(sd);
	int err = 0;

	/* TBD */
	mt9m113_err(&client->dev, "%s : not supported [%d]\n", __func__,ctrl->value);

	if (err < 0)
		mt9m113_err(&client->dev, "%s : i2c_write for checkdata\n", __func__);

	return err;
}

static int mt9m113_set_capture_start(struct v4l2_subdev *sd)
{
#ifdef MT9M113_DEBUG
	struct i2c_client *client = v4l2_get_subdevdata(sd);
#endif
	struct mt9m113_state *state = to_state(sd);

	int err = 0;
#ifdef MT9M113_DEBUG
	unsigned short lvalue = 0;
#endif

	mt9m113_info(&client->dev, "%s : light value is %x\n", __func__, lvalue);

	/* set initial regster value */
	err = mt9m113_i2c_write_regs(sd, &mt9m113_capture_regs[0], ARRAY_SIZE(mt9m113_capture_regs));

	if (unlikely(err)) {
		mt9m113_info(&client->dev, "%s: failed to make capture\n", __func__);
		return err;
	}

//	if (lvalue < 0x40) {
//		mt9m113_info(&client->dev, "\n----- low light -----\n\n");
//		msleep(300);
//	} else if(lvalue < 0x70) {
//		mt9m113_info(&client->dev, "\n----- mid light -----\n\n");
//		msleep(150);
//	} else {
//		mt9m113_info(&client->dev, "\n----- normal light -----\n\n");
//		msleep(100);
//	}

	state->runmode = MT9M113_RUNMODE_IDLE;

	return err;
}

static int mt9m113_set_preview_start(struct v4l2_subdev *sd)
{
#ifdef MT9M113_DEBUG
	struct i2c_client *client = v4l2_get_subdevdata(sd);
#endif
	struct mt9m113_state *state = to_state(sd);

	int err = 0;

	mt9m113_info(&client->dev, "%s: set_Preview_start\n", __func__);

	if (!state->req_fmt.width || !state->req_fmt.height)
		return -EINVAL;

	if(state->check_dataline) {
//		err = mt9m113_write_regs(sd, mt9m113_pattern_on,
//				sizeof(mt9m113_pattern_on) / sizeof(mt9m113_pattern_on[0]));
//		msleep(100);
	} else {
//		if(state->set_vhflip == 1) {
//			err = mt9m113_write_regs(sd, mt9m113_vhflip_on,
//					sizeof(mt9m113_vhflip_on) / sizeof(mt9m113_vhflip_on[0]));
//		} else {
//			err = mt9m113_write_regs(sd, mt9m113_vhflip_off,
//					sizeof(mt9m113_vhflip_off) / sizeof(mt9m113_vhflip_off[0]));
//		}
		
		/* set initial regster value */
		err = mt9m113_i2c_write_regs(sd, &mt9m113_preview_regs[0], ARRAY_SIZE(mt9m113_preview_regs));

		if (unlikely(err)) {
			mt9m113_info(&client->dev, "%s: failed to make preview\n", __func__);
			return err;
		}

		state->runmode = MT9M113_RUNMODE_RUNNING;
	}
	
	return err;
}

static int mt9m113_set_preview_stop(struct v4l2_subdev *sd)
{
	struct mt9m113_state *state = to_state(sd);

	int err = 0;
	
	if (state->runmode == MT9M113_RUNMODE_RUNNING) {
		state->runmode = MT9M113_RUNMODE_IDLE;
		state->set_vhflip = 0;
	}

	return err;
}

static int mt9m113_check_dataline_stop(struct v4l2_subdev *sd)
{
	struct i2c_client *client = v4l2_get_subdevdata(sd);
	int err = 0;

	/* TBD */
	mt9m113_err(&client->dev, "%s : not supported\n", __func__);

	if (err < 0)
		mt9m113_err(&client->dev, "%s : i2c_write for check_dataline_stop\n", __func__);

	return err;
}

static int mt9m113_chip_ident(struct v4l2_subdev *sd,
				struct v4l2_dbg_chip_ident *id)
{
	struct i2c_client *client = v4l2_get_subdevdata(sd);
	struct mt9m113_platform_data *pdata;

	pdata = client->dev.platform_data;
	if (!pdata) {
		mt9m113_err(&client->dev, "%s: no platform data\n", __func__);
		return -ENODEV;
	}

	mt9m113_info(&client->dev, "%s: start\n", __func__);
	id->ident = 0x2480;

	return 0;
}

static int mt9m113_init(struct v4l2_subdev *sd, u32 val)
{
	struct i2c_client *client = v4l2_get_subdevdata(sd);
	struct mt9m113_state *state = to_state(sd);
	struct mt9m113_platform_data *pdata;
	int err = -EIO;

	pdata = client->dev.platform_data;
	if (!pdata) {
		mt9m113_err(&client->dev, "%s: no platform data\n", __func__);
		return -ENODEV;
	}

	mt9m113_info(&client->dev, "%s: start\n", __func__);

	state->default_width  = pdata->default_width;
	state->default_height = pdata->default_height;
	state->capture_width  = pdata->max_width;
	state->capture_height = pdata->max_height;

	err = mt9m113_i2c_write_regs(sd, &mt9m113_init_regs[0], ARRAY_SIZE(mt9m113_init_regs));

	if (unlikely(err)) {
		mt9m113_err(&client->dev, "%s: failed to init\n", __func__);
		return err;
	}

	state->set_fmt.width = state->capture_width;
	state->set_fmt.height = state->capture_height;

	return 0;
}

static int mt9m113_enum_framesizes(struct v4l2_subdev *sd, \
					struct v4l2_frmsizeenum *fsize)
{
#ifdef MT9M113_DEBUG
	struct i2c_client *client = v4l2_get_subdevdata(sd);
#endif
	struct mt9m113_state *state = to_state(sd);

	/*
	 * Return the actual output settings programmed to the camera
	 */
	if(state->req_fmt.width < state->capture_width) {
		fsize->discrete.width = state->default_width;
		fsize->discrete.height = state->default_height;
	} else if (state->req_fmt.width >= state->capture_width) {
		fsize->discrete.width = state->capture_width;
		fsize->discrete.height = state->capture_height;
	} else {
		fsize->discrete.width = state->req_fmt.width;
		fsize->discrete.height = state->req_fmt.height;
	}

	state->set_fmt.width = fsize->discrete.width;
	state->set_fmt.height = fsize->discrete.height;

	mt9m113_info(&client->dev, "%s : width - %d , height - %d\n", __func__, fsize->discrete.width, fsize->discrete.height);

	return 0;
}

static int mt9m113_g_fmt(struct v4l2_subdev *sd, struct v4l2_mbus_framefmt *fmt)
{
	int err = 0;
	return err;
}

static int mt9m113_s_fmt(struct v4l2_subdev *sd, struct v4l2_mbus_framefmt *fmt)
{
#ifdef MT9M113_DEBUG
	struct i2c_client *client = v4l2_get_subdevdata(sd);
#endif
	struct mt9m113_state *state = to_state(sd);

	/*
	 * Just copying the requested format as of now.
	 * We need to check here what are the formats the camera support, and
	 * set the most appropriate one according to the request from FIMC
	 */
	state->req_fmt.width = fmt->width;
	state->req_fmt.height = fmt->height;
	state->set_fmt.width = fmt->width;
	state->set_fmt.height = fmt->height;

	state->req_fmt.colorspace = fmt->colorspace;

	mt9m113_info(&client->dev, "%s : width - %d , height - %d\n", __func__, state->req_fmt.width, state->req_fmt.height);

	return 0;
}

static int mt9m113_g_parm(struct v4l2_subdev *sd, struct v4l2_streamparm *parms)
{
	int err = 0;
	return err;
}

static int mt9m113_s_parm(struct v4l2_subdev *sd, struct v4l2_streamparm *parms)
{
	int err = 0;
	return err;
}

static int mt9m113_g_ctrl(struct v4l2_subdev *sd, struct v4l2_control *ctrl)
{
	struct i2c_client *client = v4l2_get_subdevdata(sd);
	int err = 0;

	mt9m113_info(&client->dev, "%s id:%d\n", __func__, ctrl->id - V4L2_CID_PRIVATE_BASE);

	switch (ctrl->id)
	{
		case V4L2_CID_EXPOSURE:
			break;

		case V4L2_CID_AUTO_WHITE_BALANCE:
			break;

		case V4L2_CID_WHITE_BALANCE_PRESET:
			break;

		case V4L2_CID_COLORFX:
			break;

		case V4L2_CID_CONTRAST:
			break;

		case V4L2_CID_SATURATION:
			break;

		case V4L2_CID_SHARPNESS:
			break;

		case V4L2_CID_CAM_JPEG_MAIN_SIZE:
			break;

		case V4L2_CID_CAM_JPEG_MAIN_OFFSET:
			break;

		case V4L2_CID_CAM_JPEG_THUMB_SIZE:
			break;

		case V4L2_CID_CAM_JPEG_THUMB_OFFSET:
			break;

		case V4L2_CID_CAM_JPEG_POSTVIEW_OFFSET:
			break; 

		case V4L2_CID_CAM_JPEG_MEMSIZE:
			break;

		case V4L2_CID_CAM_JPEG_QUALITY:
			break;

		case V4L2_CID_CAMERA_OBJ_TRACKING_STATUS:
			break;

		case V4L2_CID_CAMERA_SMART_AUTO_STATUS:
			break;

		case V4L2_CID_CAMERA_AUTO_FOCUS_RESULT:
			break;

		case V4L2_CID_CAM_DATE_INFO_YEAR:
			break; 

		case V4L2_CID_CAM_DATE_INFO_MONTH:
			break; 

		case V4L2_CID_CAM_DATE_INFO_DATE:
			break; 

		case V4L2_CID_CAM_SENSOR_VER:
			break; 

		case V4L2_CID_CAM_FW_MINOR_VER:
			break; 

		case V4L2_CID_CAM_FW_MAJOR_VER:
			break; 

		case V4L2_CID_CAM_PRM_MINOR_VER:
			break; 

		case V4L2_CID_CAM_PRM_MAJOR_VER:
			break; 

		default:
			mt9m113_err(&client->dev, "%s: no such control\n", __func__);
			break;
	}

	if (err < 0)
		mt9m113_err(&client->dev, "%s : err\n", __func__);

	return err;
}

static int mt9m113_s_ctrl(struct v4l2_subdev *sd, struct v4l2_control *ctrl)
{
	struct i2c_client *client = v4l2_get_subdevdata(sd);
	struct mt9m113_state *state = to_state(sd);

	int err = 0;

	if(state->check_dataline) {
		if( ( ctrl->id != V4L2_CID_CAM_PREVIEW_ONOFF ) &&
			( ctrl->id != V4L2_CID_CAMERA_CHECK_DATALINE_STOP ) &&
			( ctrl->id != V4L2_CID_CAMERA_CHECK_DATALINE ) )
				return 0;
	}

	mt9m113_info(&client->dev, "%s id:%d, value:%d\n", __func__, ctrl->id - V4L2_CID_PRIVATE_BASE, ctrl->value);

	mutex_lock(&sensor_s_ctrl);

	switch (ctrl->id) {
	case V4L2_CID_CAM_PREVIEW_ONOFF:
		if (ctrl->value)
			err = mt9m113_set_preview_start(sd);
		else
			err = mt9m113_set_preview_stop(sd);
		mt9m113_info(&client->dev, "V4L2_CID_CAM_PREVIEW_ONOFF [%d] \n", ctrl->value);
		break;

	case V4L2_CID_CAM_CAPTURE:
		err = mt9m113_set_capture_start(sd);
		mt9m113_info(&client->dev, "V4L2_CID_CAM_CAPTURE [%d] \n", ctrl->value);
		break;

	case V4L2_CID_CAMERA_BRIGHTNESS:
		err = mt9m113_set_brightness(sd, ctrl);
		mt9m113_info(&client->dev, "V4L2_CID_CAMERA_BRIGHTNESS [%d] \n", ctrl->value);
		break;

	case V4L2_CID_CAMERA_VGA_BLUR:
		err = mt9m113_set_blur(sd, ctrl);
		mt9m113_info(&client->dev, "V4L2_CID_CAMERA_VGA_BLUR [%d] \n", ctrl->value);
		break;

	case V4L2_CID_CAMERA_CHECK_DATALINE:
		state->check_dataline = ctrl->value;
		break;

	case V4L2_CID_CAMERA_CHECK_DATALINE_STOP:
		err = mt9m113_check_dataline_stop(sd);
		break;

	case V4L2_CID_CAMERA_FRAME_RATE:
		err = mt9m113_set_frame_rate(sd, ctrl);
		state->fps = ctrl->value;
		break;

//	case V4L2_CID_CAMERA_APP_CHECK:
//		break;

	case V4L2_CID_CAM_JPEG_QUALITY:
		break;

	case V4L2_CID_CAMERA_SENSOR_MODE:
		break;

//	case V4L2_CID_CAMERA_CHECK_FLIP:
//		state->set_vhflip = ctrl->value;
//		err = mt9m113_set_flip(sd, ctrl);
//		break;

	case V4L2_CID_CAMERA_WHITE_BALANCE:
		err = mt9m113_set_white_balance(sd, ctrl);
		break;

	case V4L2_CID_CAMERA_EFFECT:
		err = mt9m113_set_effect(sd, ctrl);
		break;

	default:
		mt9m113_err(&client->dev, "%s: no such control\n", __func__);
		break;
	}

	mutex_unlock(&sensor_s_ctrl);	
	
	return err;
}

static int mt9m113_s_stream(struct v4l2_subdev *sd, int enable)
{
	int err = 0;

	switch(enable) {
		case 1: //Preview
			err = mt9m113_set_preview_start(sd);
			break;
		case 2: //Capture
		    err = mt9m113_set_preview_stop(sd);
			err = mt9m113_set_capture_start(sd);
			break;
		default:
			break;
	}

	return err;
}

static const struct v4l2_subdev_core_ops mt9m113_core_ops = {
	.g_chip_ident = mt9m113_chip_ident,
	.init = mt9m113_init,		/* initializing API */
	.g_ctrl = mt9m113_g_ctrl,
	.s_ctrl = mt9m113_s_ctrl,
};

static const struct v4l2_subdev_video_ops mt9m113_video_ops = {
	/*.s_crystal_freq = mt9m113_s_crystal_freq,*/
	.g_mbus_fmt = mt9m113_g_fmt,
	.s_mbus_fmt = mt9m113_s_fmt,
	.enum_framesizes = mt9m113_enum_framesizes,
	.g_parm = mt9m113_g_parm,
	.s_parm = mt9m113_s_parm,
	.s_stream = mt9m113_s_stream,
};

static const struct v4l2_subdev_ops mt9m113_ops = {
	.core = &mt9m113_core_ops,
	.video = &mt9m113_video_ops,
};

/*
 * mt9m113_probe
 * Fetching platform data is being done with s_config subdev call.
 * In probe routine, we just register subdev device
 */
static int mt9m113_probe(struct i2c_client *client,
		const struct i2c_device_id *id)
{
	struct mt9m113_state *state;
	struct v4l2_subdev *sd;

	state = kzalloc(sizeof(struct mt9m113_state), GFP_KERNEL);
	if (state == NULL)
		return -ENOMEM;

	state->runmode = MT9M113_RUNMODE_NOTREADY;
	state->default_width =  -1;
	state->default_height = -1;
	state->capture_width =  -1;
	state->capture_height = -1;

	sd = &state->sd;
	strcpy(sd->name, MT9M113_DRIVER_NAME);

	/* Registering subdev */
	v4l2_i2c_subdev_init(sd, client, &mt9m113_ops);

	mt9m113_info(&client->dev, "mt9m113 has been probed\n");

	return 0;
}

static int mt9m113_remove(struct i2c_client *client)
{
	struct v4l2_subdev *sd = i2c_get_clientdata(client);

	v4l2_device_unregister_subdev(sd);
	kfree(to_state(sd));

	return 0;
}

static const struct i2c_device_id mt9m113_id[] = {
	{ MT9M113_DRIVER_NAME, 0 },
	{ },
};
MODULE_DEVICE_TABLE(i2c, mt9m113_id);

static struct i2c_driver mt9m113_i2c_driver = {
	.driver = {
		.name = MT9M113_DRIVER_NAME,
	},
	.probe = mt9m113_probe,
	.remove = mt9m113_remove,
	.id_table = mt9m113_id,
};

static int __init mt9m113_mod_init(void)
{
	return i2c_add_driver(&mt9m113_i2c_driver);
}

static void __exit mt9m113_mod_exit(void)
{
	i2c_del_driver(&mt9m113_i2c_driver);
}
module_init(mt9m113_mod_init);
module_exit(mt9m113_mod_exit);

MODULE_DESCRIPTION("MT9M113 Camera driver");
MODULE_AUTHOR("DEV Team 1<h211@thinkware.co.kr>");
MODULE_LICENSE("GPL");
