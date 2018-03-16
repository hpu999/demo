#include <linux/module.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/mman.h>
#include <linux/init.h>
#include <linux/dma-mapping.h>
#include <linux/fs.h>
#include <linux/version.h>
#include <linux/delay.h>
#include <linux/platform_data/dma-imx.h>

#include <linux/dmaengine.h>
#include <linux/device.h>

#include <linux/io.h>
#include <linux/delay.h>

#include <linux/time.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/irqreturn.h>

unsigned long start_time;
unsigned long end_time ;

struct timeval start_tv;
struct timeval middle1_tv;
struct timeval middle2_tv;
struct timeval end_tv ;

struct timespec start_ts;
struct timespec end_ts ;

int __init gpio_init_module(void)
{
	printk ("HZ = %d \n", HZ);

	start_time = jiffies;
	delsy (1);
	end_time = jiffies;
	printk ("jiffies : end - start = %ld \n", end_time - start_time);

	getnstimeofday (&start_ts);
	ndelay (100);
	getnstimeofday (&middle1_ts);
	getnstimeofday (&middle2_ts);
	getnstimeofday (&end_ts);
	printk ("ns : end - middle2 = %ld \n", end_ts.tv_nsec - middle2_ts.tv_nsec);
	printk ("ns : middle2 - middle1 = %ld \n", middle2_ts.tv_nsec - middle1_ts.tv_nsec);
	printk ("ns : middle1 - start = %ld \n", middle1_ts.tv_nsec - start_ts.tv_nsec);

	do_gettimeofday (&start_tv);	
	udelay (100);
	do_gettimeofday (&end_tv);	
	printk ("us : end - start = %d \n", end_tv.tv_usec - start_tv.tv_usec);
	return 0;
}

static void gpio_cleanup_module(void)
{
}


module_init(gpio_init_module);
module_exit(gpio_cleanup_module);

MODULE_LICENSE("GPL");
