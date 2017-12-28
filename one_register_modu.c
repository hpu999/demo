#include <linux/module.h>
#include <linux/uaccess.h>
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

#define DEVICE_NAME "gpio"
#define CLASS_NAME "gpio_test"

static int gMajor; /* major number of device */
static struct class *gpio_class;

struct cmd_data {
	unsigned int m_addr;
	unsigned int m_data;
};

unsigned int *wbuf, *rbuf;

struct cmd_data data;

ssize_t gpio_read (struct file *filp, char __user * buf, size_t count,
								loff_t * offset)
{
	copy_from_user (&data, buf, sizeof (struct cmd_data));
	rbuf = (unsigned int *)ioremap(data.m_addr, 4);
	data.m_data = *rbuf;
	copy_to_user (buf, &data, sizeof (struct cmd_data));
	iounmap(rbuf);
	return 0;
}

ssize_t gpio_write(struct file * filp, const char __user * buf, size_t count,
								loff_t * offset)
{
	copy_from_user (&data, buf, sizeof (struct cmd_data));
	wbuf = (unsigned int *)ioremap(data.m_addr, 4);
	*wbuf = data.m_data;
	iounmap(wbuf);
	return 0;
}

struct file_operations gpio_fops = {
	read:		gpio_read,
	write:		gpio_write,
};

int __init gpio_init_module(void)
{
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,26))
	struct device *temp_class;
#else
	struct class_device *temp_class;
#endif
	int error;

	/* register a character device */
	error = register_chrdev(0, DEVICE_NAME, &gpio_fops);
	if (error < 0) {
		printk("gpio test driver can't get major number\n");
		return error;
	}
	gMajor = error;
	printk("gpio test major number = %d\n",gMajor);

	gpio_class = class_create(THIS_MODULE, CLASS_NAME);
	if (IS_ERR(gpio_class)) {
		printk(KERN_ERR "Error creating register test module class.\n");
		unregister_chrdev(gMajor, DEVICE_NAME);
		return PTR_ERR(gpio_class);
	}

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,28))
	temp_class = device_create(gpio_class, NULL,
				   MKDEV(gMajor, 0), NULL, DEVICE_NAME);
#elif (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,26))
	temp_class = device_create(gpio_class, NULL,
				   MKDEV(gMajor, 0), DEVICE_NAME);
#else
	temp_class = class_device_create(gpio_class, NULL,
					     MKDEV(gMajor, 0), NULL,
					     DEVICE_NAME);
#endif
	if (IS_ERR(temp_class)) {
		printk(KERN_ERR "Error creating gpio test class device.\n");
		class_destroy(gpio_class);
		unregister_chrdev(gMajor, DEVICE_NAME);
		return -1;
	}

	printk("gpio test Driver Module loaded\n");
	return 0;
}

static void gpio_cleanup_module(void)
{
	unregister_chrdev(gMajor, DEVICE_NAME);
	device_destroy(gpio_class, MKDEV(gMajor, 0));
	class_destroy(gpio_class);

	printk("gpio test Driver Module Unloaded\n");
}


module_init(gpio_init_module);
module_exit(gpio_cleanup_module);

MODULE_DESCRIPTION("gpio test driver");
MODULE_LICENSE("GPL");
