/*
 *  willow_dai.c
 *
 *  This program is free software; you can redistribute  it and/or modify it
 *  under  the terms of  the GNU General  Public License as published by the
 *  Free Software Foundation;  either version 2 of the  License, or (at your
 *  option) any later version.
 */

#include <linux/platform_device.h>
#include <linux/clk.h>
#include <linux/io.h>

#include <sound/soc.h>
#include <sound/soc-dapm.h>
#include <sound/pcm.h>
#include <sound/pcm_params.h>

#include <mach/regs-clock.h>

#include "i2s.h"
#include "pcm.h"
#include "s3c-i2s-v2.h"
#include "../codecs/wm8985.h"

static int set_epll_rate(unsigned long rate)
{
	struct clk *fout_epll;

	fout_epll = clk_get(NULL, "fout_epll");
	if (IS_ERR(fout_epll)) {
		printk(KERN_ERR "%s: failed to get fout_epll\n", __func__);
		return PTR_ERR(fout_epll);
	}

	if (rate == clk_get_rate(fout_epll))
		goto out;

	clk_set_rate(fout_epll, rate);
out:
	clk_put(fout_epll);

	return 0;
}

static int willow_pcm_hw_params(struct snd_pcm_substream *substream,
	struct snd_pcm_hw_params *params)
{
	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	struct snd_soc_dai *cpu_dai = rtd->cpu_dai;
	unsigned long epll_out_rate;
	int rfs, ret;

	switch (params_rate(params)) {
	case 8000:
	case 16000:
		epll_out_rate = 49152000;
		break;
	default:
		printk(KERN_ERR "%s:%d Sampling Rate %u not supported!\n",
			__func__, __LINE__, params_rate(params));
		return -EINVAL;
	}

	switch (params_rate(params)) {
	case 16000:
		rfs = 128;
		break;
	case 8000:
		rfs = 256;
		break;
		break;
	default:
		printk(KERN_ERR "%s:%d Sampling Rate %u not supported!\n",
			__func__, __LINE__, params_rate(params));
		return -EINVAL;
	}

	/* Set the cpu DAI configuration */
	ret = snd_soc_dai_set_fmt(cpu_dai, SND_SOC_DAIFMT_DSP_A
				| SND_SOC_DAIFMT_IB_NF
				| SND_SOC_DAIFMT_CBS_CFS);
	if (ret < 0)
		return ret;

	/* Set EPLL clock rate */
	ret = set_epll_rate(epll_out_rate);
	if (ret < 0)
		return ret;

	/* 8kHz * 256(16bits * 16slots, frame sync) = 2048kHz Clock Out */
	ret = snd_soc_dai_set_sysclk(cpu_dai, S3C_PCM_CLKSRC_MUX,
				params_rate(params)*rfs,
				SND_SOC_CLOCK_OUT);

	/* Set SCLK_DIV for making bclk */
	ret = snd_soc_dai_set_clkdiv(cpu_dai, S3C_PCM_SCLK_PER_FS, rfs);
	if (ret < 0)
		return ret;

	return 0;
}

static int willow_i2s_hw_params(struct snd_pcm_substream *substream,
	struct snd_pcm_hw_params *params)
{
	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	struct snd_soc_dai *codec_dai = rtd->codec_dai;
	struct snd_soc_dai *cpu_dai = rtd->cpu_dai;
	int bfs, psr, rfs, ret;
	unsigned long rclk;

	switch (params_format(params)) {
	case SNDRV_PCM_FORMAT_U24:
	case SNDRV_PCM_FORMAT_S24:
		bfs = 48;
		break;
	case SNDRV_PCM_FORMAT_U16_LE:
	case SNDRV_PCM_FORMAT_S16_LE:
		bfs = 32;
		break;
	default:
		return -EINVAL;
	}

	switch (params_rate(params)) {
	case 16000:
	case 22050:
	case 24000:
	case 32000:
	case 44100:
	case 48000:
	case 88200:
	case 96000:
		if (bfs == 48)
			rfs = 384;
		else
			rfs = 256;
		break;
	case 64000:
		rfs = 384;
		break;
	case 8000:
	case 11025:
	case 12000:
		if (bfs == 48)
			rfs = 768;
		else
			rfs = 512;
		break;
	default:
		return -EINVAL;
	}

	rclk = params_rate(params) * rfs;

	switch (rclk) {
	case 4096000:
	case 5644800:
	case 6144000:
	case 8467200:
	case 9216000:
		psr = 8;
		break;
	case 8192000:
	case 11289600:
	case 12288000:
	case 16934400:
	case 18432000:
		psr = 4;
		break;
	case 22579200:
	case 24576000:
	case 33868800:
	case 36864000:
		psr = 2;
		break;
	case 67737600:
	case 73728000:
		psr = 1;
		break;
	default:
		printk("Not yet supported!\n");
		return -EINVAL;
	}

	set_epll_rate(rclk * psr);

	ret = snd_soc_dai_set_fmt(codec_dai,
					SND_SOC_DAIFMT_I2S
					| SND_SOC_DAIFMT_NB_NF
					| SND_SOC_DAIFMT_CBS_CFS);
	if (ret < 0)
		return ret;

	ret = snd_soc_dai_set_fmt(cpu_dai,
					SND_SOC_DAIFMT_I2S
					| SND_SOC_DAIFMT_NB_NF
					| SND_SOC_DAIFMT_CBS_CFS);
	if (ret < 0)
		return ret;

	ret = snd_soc_dai_set_sysclk(codec_dai, WM8985_CLKSRC_MCLK,
					rclk, SND_SOC_CLOCK_IN);
	if (ret < 0)
		return ret;

	ret = snd_soc_dai_set_sysclk(cpu_dai, SAMSUNG_I2S_CDCLK,
					0, SND_SOC_CLOCK_OUT);
	if (ret < 0)
		return ret;

	ret = snd_soc_dai_set_clkdiv(cpu_dai, SAMSUNG_I2S_DIV_BCLK, bfs);
	if (ret < 0)
		return ret;

	return 0;
}

/*
 * WILLOW DAI operations.
 */
static struct snd_soc_ops willow_i2s_ops = {
	.hw_params = willow_i2s_hw_params,
};

static struct snd_soc_ops willow_pcm_ops = {
	.hw_params = willow_pcm_hw_params,
};

/* WILLOW Widgets */
static const struct snd_soc_dapm_widget wm8985_dapm_widgets[] = {
	SND_SOC_DAPM_HP("HP-L/R", NULL),
	SND_SOC_DAPM_SPK("Speaker-L/R", NULL),
	SND_SOC_DAPM_MIC("MicIn", NULL),
};

/* WILLOW-PAIF Connections */
static const struct snd_soc_dapm_route audio_map[] = {
	/* HP Left/Right */
	{"HP-L/R", NULL, "HPL"},
	{"HP-L/R", NULL, "HPR"},

	/* SPK Left/Right */
	{"Speaker-L/R", NULL, "SPKL"},
	{"Speaker-L/R", NULL, "SPKR"},

	/* MicIn feeds AINL */
	{"RIN", NULL, "MicIn"},
	{"LIN", NULL, "MicIn"},
};

static int willow_wm8985_init_paif(struct snd_soc_pcm_runtime *rtd)
{
	struct snd_soc_codec *codec = rtd->codec;
	struct snd_soc_dapm_context *dapm = &codec->dapm;

	/* Add willow specific widgets */
	snd_soc_dapm_new_controls(dapm, wm8985_dapm_widgets,
				  ARRAY_SIZE(wm8985_dapm_widgets));

	/* Set up PAIF audio path */
	snd_soc_dapm_add_routes(dapm, audio_map, ARRAY_SIZE(audio_map));

	/* signal a DAPM event */
	snd_soc_dapm_sync(dapm);

	return 0;
}

static int willow_bcm4334_pcm_init(struct snd_soc_pcm_runtime *rtd)
{
	struct snd_soc_dai *cpu_dai = rtd->cpu_dai;

	/* default 256kHz Clock Out */
	snd_soc_dai_set_sysclk(cpu_dai, S3C_PCM_CLKSRC_MUX, 256000, SND_SOC_CLOCK_OUT);

	/* Set SCLK_DIV for making bclk, default 256 */
	snd_soc_dai_set_clkdiv(cpu_dai, S3C_PCM_SCLK_PER_FS, 256);

	return 0;
}

static struct snd_soc_dai_link willow_dai[] = {
#if defined(CONFIG_SND_SOC_WM8985) && defined(CONFIG_SND_SAMSUNG_I2S)
	{ /* Primary i/f */
		.name = "WM8985 PAIF",
		.stream_name = "Pri_Dai",
		.cpu_dai_name = "samsung-i2s.0",
		.codec_dai_name = "wm8985-hifi",
		.platform_name = "samsung-audio",
		.codec_name = "wm8985.1-001a",
		.init = willow_wm8985_init_paif,
		.ops = &willow_i2s_ops,
	},
#endif
#if defined(CONFIG_SND_SOC_BCM4334) && defined(CONFIG_SND_SAMSUNG_PCM)
	{ /* Secondary i/f */
		.name = "BCM4334 PAIF",
		.stream_name = "Sec_Dai",
		.cpu_dai_name = "samsung-pcm.1",
		.codec_dai_name = "bcm4334-pcm",
		.platform_name = "samsung-audio",
		.init = willow_bcm4334_pcm_init,
		.codec_name = "bcm4334-pcm",
		.ops = &willow_pcm_ops,
	},
#endif
};

static struct snd_soc_card willow = {
	.name = "willow",
	.dai_link = willow_dai,
	.num_links = ARRAY_SIZE(willow_dai),
};

static struct platform_device *willow_snd_device;

static int __init willow_audio_init(void)
{
	int ret;

	willow_snd_device = platform_device_alloc("soc-audio", -1);
	if (!willow_snd_device)
		return -ENOMEM;

	platform_set_drvdata(willow_snd_device, &willow);

	ret = platform_device_add(willow_snd_device);
	if (ret)
		platform_device_put(willow_snd_device);

	return ret;
}
module_init(willow_audio_init);

static void __exit willow_audio_exit(void)
{
	platform_device_unregister(willow_snd_device);
}
module_exit(willow_audio_exit);

MODULE_DESCRIPTION("ALSA SoC WILLOW DAI");
MODULE_LICENSE("GPL");
