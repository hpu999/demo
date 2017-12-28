#include <linux/module.h>
#include <linux/gpio.h>
#include <asm/io.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/mman.h>
#include <linux/init.h>
#include <linux/dma-mapping.h>
#include <linux/fs.h>
#include <linux/version.h>
#include <linux/delay.h>
#include <linux/miscdevice.h>

#include <linux/dmaengine.h>
#include <linux/device.h>

#include <linux/io.h>
#include <linux/delay.h>

#define DEVICE_NAME "gpio"

static long gpio_ioctl (struct file *filp, unsigned int cmd, unsigned long arg)
{
    /*
     *
     *
     *
     */

    return 0;
}

struct file_operations gpio_fops = {
    unlocked_ioctl: gpio_ioctl,
};

static struct miscdevice gpio_misc = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = DEVICE_NAME,
    .fops = &gpio_fops
};

int __init gpio_init_module(void)
{
    unsigned int *addr = NULL;
    addr = (unsigned int *)ioremap(0x020e010c, 8);
    *addr = 0x5;
    *(addr + 1) = 0x5;
    iounmap(addr);

    /* register a character device */
    misc_register(&gpio_misc);

    gpio_request (EIM_EB0, "GPIO2_IO28");
    gpio_request (EIM_EB1, "GPIO2_IO29");
    gpio_direction_output (EIM_EB0, 0);
    gpio_direction_output (EIM_EB1, 0);

    printk("gpio test Driver Module loaded\n");
	return 0;
}

static void gpio_cleanup_module(void)
{
    gpio_direction_output (EIM_EB0, 0);
    gpio_direction_output (EIM_EB1, 0);
    gpio_free (EIM_EB0);
    gpio_free (EIM_EB1);

    misc_deregister(&gpio_misc);
    printk("gpio test Driver Module Unloaded\n");
}


module_init(gpio_init_module);
module_exit(gpio_cleanup_module);

MODULE_DESCRIPTION("gpio test driver");
MODULE_LICENSE("GPL");
