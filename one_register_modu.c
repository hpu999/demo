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

static int gMajor; /* major number of device */
static struct class *dma_tm_class;

struct cmd_data {
	unsigned int m_addr;
	unsigned int m_data;
};

unsigned int *wbuf, *rbuf;

struct cmd_data data;

ssize_t register_read (struct file *filp, char __user * buf, size_t count,
								loff_t * offset)
{
	copy_from_user (&data, buf, sizeof (struct cmd_data));
	rbuf = (unsigned int *)ioremap(data.m_addr, 4);
	data.m_data = *rbuf;
	copy_to_user (buf, &data, sizeof (struct cmd_data));
	iounmap(rbuf);
	return 0;
}

ssize_t register_write(struct file * filp, const char __user * buf, size_t count,
								loff_t * offset)
{
	copy_from_user (&data, buf, sizeof (struct cmd_data));
	wbuf = (unsigned int *)ioremap(data.m_addr, 4);
	*wbuf = data.m_data;
	iounmap(wbuf);
	return 0;
}

struct file_operations dma_fops = {
	read:		register_read,
	write:		register_write,
};

int __init register_init_module(void)
{
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,26))
	struct device *temp_class;
#else
	struct class_device *temp_class;
#endif
	int error;

	/* register a character device */
	error = register_chrdev(0, "register_test", &dma_fops);
	if (error < 0) {
		printk("register test driver can't get major number\n");
		return error;
	}
	gMajor = error;
	printk("register test major number = %d\n",gMajor);

	dma_tm_class = class_create(THIS_MODULE, "register_test");
	if (IS_ERR(dma_tm_class)) {
		printk(KERN_ERR "Error creating register test module class.\n");
		unregister_chrdev(gMajor, "register_test");
		return PTR_ERR(dma_tm_class);
	}

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,28))
	temp_class = device_create(dma_tm_class, NULL,
				   MKDEV(gMajor, 0), NULL, "register_test");
#elif (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,26))
	temp_class = device_create(dma_tm_class, NULL,
				   MKDEV(gMajor, 0), "register_test");
#else
	temp_class = class_device_create(dma_tm_class, NULL,
					     MKDEV(gMajor, 0), NULL,
					     "register_test");
#endif
	if (IS_ERR(temp_class)) {
		printk(KERN_ERR "Error creating register test class device.\n");
		class_destroy(dma_tm_class);
		unregister_chrdev(gMajor, "register_test");
		return -1;
	}

	printk("register test Driver Module loaded\n");
	return 0;
}

static void register_cleanup_module(void)
{
	unregister_chrdev(gMajor, "register_test");
	device_destroy(dma_tm_class, MKDEV(gMajor, 0));
	class_destroy(dma_tm_class);

	printk("register test Driver Module Unloaded\n");
}


module_init(register_init_module);
module_exit(register_cleanup_module);

MODULE_DESCRIPTION("register test driver");
MODULE_LICENSE("GPL");
