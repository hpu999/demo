#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/errno.h>
#include <linux/kernel.h>

#include <linux/sched.h>
#include <linux/kthread.h>
#include <linux/err.h>

static struct task_struct *test_task;

int thread_fun (void *data) {
    int i = 0;
    for (i = 0; i < 1000; ++i)
        printk ("kthread is working ... i = %d \n" ,i);

    return 0;
}

static int __init kthread_test_init (void) {
    int err;

/* 创建启动线程，方法一 */
    /* kthread_run 相当于 kthread_create(创建线程) and wake_up_process(启动线程)
     * @thread_fun : int (*thread_fun)(void *)
     * @NULL 向thread_fun中传递的参数
     * @"threa_test" 内核线程的名字
     */
//  kthread_run (thread_fun, NULL, "thread_test");

/* 创建启动线程，方法二 */
    test_task = kthread_create (thread_fun, NULL, "test_task");
    if (IS_ERR (test_task)) {
        printk ("Unable to start kernel thread. \n");
        err = PTR_ERR(test_task);
        test_task = NULL;
        return err;
    }

    printk ("thread is waked before ... \n");
    wake_up_process (test_task);
    printk ("thread is waked after ... \n");

    return 0;

}

static void __exit kthread_test_exit (void) {

    return ;
}

MODULE_LICENSE("Dual BSD/GPL");
module_init(kthread_test_init);
module_exit(kthread_test_exit);
