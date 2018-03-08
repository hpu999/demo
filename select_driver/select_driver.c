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
     */
    printk ("%s:%s[%d] cmd[%u] \n", __FILE__, __func__, __LINE__, cmd);
    printk ("%s:%s[%d] _IOC_DIR(%u)[%d] \n", 
             __FILE__, __func__, __LINE__, cmd, _IOC_DIR(cmd));
    printk ("%s:%s[%d] _IOC_TYPE(%u)[%d] \n", 
             __FILE__, __func__, __LINE__, cmd, _IOC_TYPE(cmd));
    printk ("%s:%s[%d] _IOC_NR(%u)[%d] \n", 
             __FILE__, __func__, __LINE__, cmd, _IOC_NR(cmd));
    printk ("%s:%s[%d] _IOC_SIZE(%u)[%d] \n", 
             __FILE__, __func__, __LINE__, cmd, _IOC_SIZE(cmd));

    return 0;
}

static unsigned int gpio_poll(struct file *filp, 
			struct poll_table_struct *wait)  
{  
    unsigned int mask = 0;  
	 
	/* 注释的是poll()的真正的框架 */ 
//    poll_wait(filp, &event_wait, wait);  
//    // set mask  
//    if(/*数据可读条件*/)  {  
//        mask |= POLLIN | POLLRDNORM;  
//    }  

	/* 仅限于测试select()的调用是执行的poll()函数 */
	printk ("%s:%s[%d] \n", __FILE__, __func__, __LINE__);

	return mask;  
}  

struct file_operations gpio_fops = {
    unlocked_ioctl: gpio_ioctl,
	poll:gpio_poll,      // 系统调用select()的时候，对于底层的poll()响应
};

static struct miscdevice gpio_misc = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = DEVICE_NAME,
    .fops = &gpio_fops
};

int __init gpio_init_module(void)
{
    /* register a character device */
    misc_register(&gpio_misc);

    printk("gpio test Driver Module loaded\n");
    return 0;
}

static void gpio_cleanup_module(void)
{
    misc_deregister(&gpio_misc);
    printk("gpio test Driver Module Unloaded\n");
}


module_init(gpio_init_module);
module_exit(gpio_cleanup_module);

MODULE_DESCRIPTION("gpio test driver");
MODULE_LICENSE("GPL");
