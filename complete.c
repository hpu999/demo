#include <linux/moduleparam.h>
#include <linux/module.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/errno.h>
#include <linux/kernel.h>
#include <linux/init.h>

#include <linux/timer.h>
#include <linux/sched.h>
#include <linux/interrupt.h>
#include <linux/pm.h>
#include <linux/spinlock.h>
#include <linux/delay.h>

#include <linux/device.h>
#include <linux/dma-mapping.h>

#include <linux/io.h>

#include <linux/workqueue.h> //

struct completion dma_m2m_ok; 

static int __init dmatest_init(void)
{
	printk ("1. xiaosheng-%s[%s]:%d \n", __FILE__, __func__, __LINE__);

	init_completion (&dma_m2m_ok);

	printk ("2. xiaosheng-%s[%s]:%d -- wiat ... \n", __FILE__, __func__, __LINE__);

	wait_for_completion (&dma_m2m_ok);

	printk ("3. xiaosheng-%s[%s]:%d \n", __FILE__, __func__, __LINE__);


    return -ENOMEM;
}

static void __exit dmatest_exit(void)
{
    return;
}

MODULE_AUTHOR("Doppler");
MODULE_DESCRIPTION("Phascan DMA module");
MODULE_LICENSE("Dual BSD/GPL");
module_init(dmatest_init);
module_exit(dmatest_exit);
