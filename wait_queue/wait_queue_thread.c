#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/kthread.h>
#include <linux/delay.h>

MODULE_AUTHOR("Jimmy");
MODULE_DESCRIPTION("wait queue example");
MODULE_LICENSE("GPL");

static int condition;
static struct task_struct *task_1;
static struct task_struct *task_2;
static struct task_struct *task_3;

DECLARE_WAIT_QUEUE_HEAD(wq);


static int thread_func_1(void *data)
{
    msleep(100);//延时100ms，使得这个进程的等待队列插入在整个链表的头部，最先被唤醒。所以先打印这个进程！
    
    wait_event_interruptible(wq, condition);
    condition = 0;
    printk(">>>>>this task 1\n");
    
    do {
        msleep(1000);
    }while(!kthread_should_stop());
    
    return 1;
}

static int thread_func_2(void *data)
{
        
    wait_event_interruptible(wq, condition);
    condition = 0;
    printk(">>>>>this task 2\n");
    
    do {
        msleep(1000);
    }while(!kthread_should_stop());
    
    return 2;
}

static int thread_func_3(void *data)
{    
    msleep(2000);
    printk(">>>>>this task 3\n");
    condition = 1;
    wake_up_interruptible(&wq);

    msleep(2000);
    condition = 1;
    wake_up_interruptible(&wq);
    
    do {
        msleep(1000);
    }while(!kthread_should_stop());
        
    return 3;
}

static int __init mod_init(void)
{
    condition = 0;

    task_1 = kthread_run(thread_func_1, NULL, "thread%d", 1);
    if (IS_ERR(task_1)) {
        printk("******create thread 1 failed\n");
    } else {
        printk("======success create thread 1\n");
    }
    
    task_2 = kthread_run(thread_func_2, NULL, "thread%d", 2);
    if (IS_ERR(task_2)) {
        printk("******create thread 2 failed\n");
    } else {
        printk("======success create thread 2\n");
    }

    task_3 = kthread_run(thread_func_3, NULL, "thread%d", 3);
    if (IS_ERR(task_2)) {
        printk("******create thread 3 failed\n");
    } else {
        printk("======success create thread 3\n");
    }
        
    return 0;
}

static void __exit mod_exit(void)
{
    int ret;
    
    if (!IS_ERR(task_1)) {
        ret = kthread_stop(task_1);
        printk("<<<<<<<<task 1 exit, ret = %d\n", ret);
    }
        
    if (!IS_ERR(task_2)) {
        ret = kthread_stop(task_2);
        printk("<<<<<<<<task 2 exit, ret = %d\n", ret);
    }

    if (!IS_ERR(task_3)) {
        ret = kthread_stop(task_3);
        printk("<<<<<<<<task 3 exit, ret = %d\n", ret);
    }
    
    return;
}

module_init(mod_init);
module_exit(mod_exit);
