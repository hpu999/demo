#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/irq.h>
#include <linux/interrupt.h>
#include <linux/sched.h> //TASK_*
#include <linux/input.h> //KEY_* 标准按键值
#include <linux/uaccess.h>

#include <linux/moduleparam.h>
#include <linux/module.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/errno.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/gpio.h>
#include <linux/mman.h>
#include <linux/fs.h>

#include <linux/timer.h>
#include <linux/sched.h>
#include <linux/interrupt.h>
#include <linux/pm.h>
#include <linux/spinlock.h>
#include <linux/delay.h>

#include <linux/device.h>
#include <linux/dma-mapping.h>
#include <linux/uaccess.h>
#include <linux/io.h>

#include <linux/slab.h>
#include <linux/mman.h>
#include <linux/kthread.h>
#include <linux/dmaengine.h>
#include <linux/platform_data/dma-imx.h>
#include <linux/platform_data/dma-imx-sdma.h>

#include <linux/workqueue.h> //

//定义初始化等待队列头对象(构造鸡妈妈)
static wait_queue_head_t rwq; //每一个节点存放读进程

static int wait_queue_test(void *data)
{
    wait_queue_t wait;
    init_waitqueue_entry(&wait, current);

    add_wait_queue(&rwq, &wait);

    set_current_state(TASK_INTERRUPTIBLE);

    printk ("wait here ... \n");
    schedule();

    set_current_state(TASK_RUNNING);
    remove_wait_queue(&rwq, &wait);

    if (signal_pending(current)) {
        printk("读进程[%s][%d]由于接收到了信号引起的唤醒!\n", current->comm, current->pid);
        return -ERESTARTSYS;
    } else {
        printk ("driver wake up ... \n");
    }

    return 0;
}

static int btn_init(void)
{   
    init_waitqueue_head (&rwq);

    kthread_run (wait_queue_test, NULL, "wait_queue_test");
    printk ("wait_queue_test thread is runing ... \n");

    ssleep (5);

    wake_up (&rwq);

    return 0;
}

static void btn_exit(void)
{
}
module_init(btn_init);
module_exit(btn_exit);
MODULE_LICENSE("GPL");
