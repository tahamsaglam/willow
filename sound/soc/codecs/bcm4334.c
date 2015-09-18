/*
 * bcm4334.c
 *
 * test version
 */

#include <linux/init.h>
#include <sound/pcm.h>
#include <sound/soc.h>

static struct snd_soc_dai_driver bcm4334_dai = {
	.name = "bcm4334-pcm",
	.playback = {
		.stream_name = "Playback",
		.channels_min = 1,
		.channels_max = 2,
		.rate_min = 8000,
		.rate_max = 16000,
		.rates = SNDRV_PCM_RATE_8000 | SNDRV_PCM_RATE_16000,
		.formats = SNDRV_PCM_FMTBIT_S16_LE,
	},
	.capture = {
		.stream_name = "Capture",
		.channels_min = 1,
		.channels_max = 2,
		.rate_min = 8000,
		.rate_max = 16000,
		.rates = SNDRV_PCM_RATE_8000 | SNDRV_PCM_RATE_16000,
		.formats = SNDRV_PCM_FMTBIT_S16_LE,
	},
};

static int bcm4334_codec_probe(struct snd_soc_codec *codec)
{
	return 0;
}

static int  bcm4334_codec_remove(struct snd_soc_codec *codec)
{
	return 0;
}

static struct snd_soc_codec_driver soc_codec_dev_bcm4334 = {
	.probe =  bcm4334_codec_probe,
	.remove = bcm4334_codec_remove,
};

static struct device_driver bcm4334_driver = {
	.name = "bcm4334-pcm",
};

static struct device bcm4334_device = {
	.init_name = "bcm4334-pcm",
	.driver = &bcm4334_driver,
};

static __init int bcm4334_init(void)
{
	int ret;

	ret = device_register(&bcm4334_device);
	if (ret < 0)
		return ret;

	ret = snd_soc_register_codec(&bcm4334_device, &soc_codec_dev_bcm4334,
			&bcm4334_dai, 1);

	return ret;
}
module_init(bcm4334_init);

static __exit void bcm4334_exit(void)
{
	snd_soc_unregister_codec(&bcm4334_device);
	device_unregister(&bcm4334_device);
}
module_exit(bcm4334_exit);

MODULE_DESCRIPTION("BCM4334 PCM driver");
MODULE_LICENSE("GPL");
