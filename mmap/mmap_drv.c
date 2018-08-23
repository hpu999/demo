#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/mm.h> //mmap

//vma指向内核创建的描述空闲虚拟内存区域的属性对象
static int led_mmap(struct file *file,
                        struct vm_area_struct *vma)
{
    //0.将cache功能关闭,保证数据能够及时到达寄存器
    vma->vm_page_prot = 
        pgprot_noncached(vma->vm_page_prot);
    
    //1.只做仅作一件事：地址映射
    remap_pfn_range(vma, //对象指针
                    vma->vm_start,//用户虚拟起始地址
                    0xE0200000>>12,//物理地址
                    0x1000, //映射的内存大小
                    vma->vm_page_prot //访问属性
                    );
    return 0;
}

static struct file_operations led_fops = {
    .owner = THIS_MODULE,
    .mmap = led_mmap //地址映射
};

static struct miscdevice led_misc = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = "myled",
    .fops = &led_fops
};

static int led_init(void)
{
    misc_register(&led_misc);
    return 0;
}

static void led_exit(void)
{
    misc_deregister(&led_misc);
}
module_init(led_init);
module_exit(led_exit);
MODULE_LICENSE("GPL");
