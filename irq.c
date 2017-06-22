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

#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/irqreturn.h>

#define IMX_GPIO_NR(bank, nr)            (((bank) - 1) * 32 + (nr))
#define GPIO_IRQ(bank, nr)               (gpio_to_irq (IMX_GPIO_NR (bank, nr)))

int cnt;

static irqreturn_t tranfer_data_from_fpga (int irq, void *dev_id)
{
	printk ("gpio_18 is trigger_falling \n");
	if (cnt % 2) 
	{
		gpio_set_value (IMX_GPIO_NR (1, 30), 1);
	} else 
	{
		gpio_set_value (IMX_GPIO_NR (1, 30), 0);
	}
	++cnt;
	return IRQ_HANDLED;
}

int __init gpio_init_module(void)
{
	int ret;
	// irq
	printk ("IRQ = %u \n", GPIO_IRQ (7, 13));
	gpio_request (IMX_GPIO_NR (7, 13), "GPIO_18");
	gpio_direction_input (IMX_GPIO_NR (7, 13));
	ret = request_irq (GPIO_IRQ (7, 13), tranfer_data_from_fpga, 
						IRQF_TRIGGER_RISING, "transfer_from_fpga", NULL);
	if (ret) 
	{
		printk ("request_irq error \n");
		return ret;
	}

	gpio_request (IMX_GPIO_NR (1, 30), "green");
	gpio_direction_output (IMX_GPIO_NR (1, 30), 0);

	return 0;
}

static void gpio_cleanup_module(void)
{
	gpio_set_value (IMX_GPIO_NR (1, 30), 0);
	free_irq (GPIO_IRQ (7, 13), NULL);	

	gpio_free (IMX_GPIO_NR (1, 30));
	gpio_free (IMX_GPIO_NR (7, 13));

}


module_init(gpio_init_module);
module_exit(gpio_cleanup_module);

MODULE_LICENSE("GPL");
