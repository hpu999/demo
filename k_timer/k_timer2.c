#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/time.h>
#include <linux/timer.h>

static struct timer_list tm;
struct timeval oldtv;

void callback(unsigned long arg)
{
	struct timeval tv;
	char *strp = (char*)arg;

	printk("%s: %lu, %s\n", __func__, jiffies, strp);

	do_gettimeofday(&tv);
	printk("%s: %ld, %ld\n", __func__,
				tv.tv_sec - oldtv.tv_sec,        //与上次中断间隔 s
				tv.tv_usec- oldtv.tv_usec);        //与上次中断间隔 ms


	oldtv = tv;
	tm.expires = jiffies+1*HZ;    
	add_timer(&tm);        //重新开始计时
}

static int __init demo_init(void)
{
	printk(KERN_INFO "%s : %s : %d - ok.\n", __FILE__, __func__, __LINE__);

	init_timer(&tm);    //初始化内核定时器

	do_gettimeofday(&oldtv);        //获取当前时间
	tm.function= callback;            //指定定时时间到后的回调函数
	tm.data    = (unsigned long)"hello world";        //回调函数的参数
	tm.expires = jiffies+1*HZ;        //定时时间
	add_timer(&tm);        //注册定时器

	return 0;
}

static void __exit demo_exit(void)
{
	printk(KERN_INFO "%s : %s : %d - ok.\n", __FILE__, __func__, __LINE__);
	del_timer(&tm);        //注销定时器
}

module_init(demo_init);
module_exit(demo_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Farsight");
MODULE_DESCRIPTION("Demo for kernel module");
