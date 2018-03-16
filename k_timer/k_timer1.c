#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/irq.h>
#include <asm/uaccess.h>
#include <asm/irq.h>
#include <asm/io.h>
#include <linux/module.h>
#include <linux/device.h> 		//class_create
#include <mach/regs-gpio.h>		//S3C2410_GPF1
//#include <asm/arch/regs-gpio.h>  
#include <mach/hardware.h>
//#include <asm/hardware.h>
#include <linux/interrupt.h>  //wait_event_interruptible
#include <linux/poll.h>   //poll
#include <linux/fcntl.h>
#include <linux/input.h>

static struct pin_desc{
	int irq;
	unsigned char *name;
	unsigned int pin;
	unsigned int key_val;
};

static struct pin_desc pins_desc[4] = {
		{IRQ_EINT1,"K1",S3C2410_GPF1,KEY_L},
		{IRQ_EINT4,"K2",S3C2410_GPF4,KEY_S},
		{IRQ_EINT2,"K3",S3C2410_GPF2,KEY_ENTER},
		{IRQ_EINT0,"K4",S3C2410_GPF0,KEY_LEFTSHIFT},
}; 

static struct pin_desc *irq_pd;
static struct input_dev *buttons_dev;
static struct timer_list buttons_timer;

/* 用户中断处理函数 */
static irqreturn_t buttons_irq(int irq, void *dev_id)
{
	irq_pd = (struct pin_desc *)dev_id;
	
	/* 修改定时器定时时间，定时10ms，即10秒后启动定时器
	 * HZ 表示100个jiffies，jiffies的单位为10ms，即HZ = 100*10ms = 1s
	 * 这里HZ/100即定时10ms
	 */
	mod_timer(&buttons_timer, jiffies + (HZ /100));
	return IRQ_HANDLED;
}


/* 定时器处理函数 */
static void buttons_timer_function(unsigned long data)
{
	struct pin_desc *pindesc = irq_pd;
	unsigned int pinval;
	pinval = s3c2410_gpio_getpin(pindesc->pin);

	if(pinval)
	{
		/* 松开 最后一个参数: 0-松开, 1-按下 */
		input_event(buttons_dev,EV_KEY,pindesc->key_val,0);
		input_sync(buttons_dev);
	}
	else
	{
		/* 按下 */
		input_event(buttons_dev,EV_KEY,pindesc->key_val,1);
		input_sync(buttons_dev);
	}
}

/* 驱动入口函数 */
static int buttons_input_init(void)
{
	int i;
	
	/* 1.分配一个input_dev结构体 */
	buttons_dev = input_allocate_device();

	/* 2.设置 */
	/* 2.1 设置按键能产生哪类事件 */
	set_bit(EV_KEY,buttons_dev->evbit);
	set_bit(EV_REP,buttons_dev->evbit);

	/* 2.2 设置能产生这类操作的哪些事件 */
	set_bit(KEY_L,buttons_dev->keybit);
	set_bit(KEY_S,buttons_dev->keybit);
	set_bit(KEY_ENTER,buttons_dev->keybit);
	set_bit(KEY_LEFTSHIFT,buttons_dev->keybit);
	
	/* 3.注册 */
	input_register_device(buttons_dev);

	
	/* 4.硬件相关的设置 */
	/* 4.1 定时器相关的操作 */
	init_timer(&buttons_timer);
	buttons_timer.function = buttons_timer_function;
	add_timer(&buttons_timer);

	/* 4.2 申请中断 */  
	for(i = 0;i < sizeof(pins_desc)/sizeof(pins_desc[0]);i++)
	{
		request_irq(pins_desc[i].irq, buttons_irq, IRQ_TYPE_EDGE_BOTH, pins_desc[i].name, &pins_desc[i]);
	}
	
	return 0;
}

/* 驱动出口函数 */
static void buttons_input_exit(void)
{
	int i;
	for(i = 0;i < sizeof(pins_desc)/sizeof(pins_desc[0]);i++)
	{
		free_irq(pins_desc[i].irq, &pins_desc[i]);
	}
	del_timer(&buttons_timer);
	input_unregister_device(buttons_dev);
	input_free_device(buttons_dev);
}

module_init(buttons_input_init);  //用于修饰入口函数
module_exit(buttons_input_exit);  //用于修饰出口函数	

MODULE_AUTHOR("LWJ");
MODULE_DESCRIPTION("Just for Demon");
MODULE_LICENSE("GPL");  //遵循GPL协议

