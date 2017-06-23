#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>

int main(int argc, char *argv[])
{
    int fd;
    void *addr;
    unsigned long *gpiocon, *gpiodata;

    if(argc != 2) {
        printf("Usage: %s <on|off>\n", argv[0]);
        return -1;
    }

    fd = open("/dev/myled", O_RDWR);
    if (fd < 0)
        return -1;

    //将LED的物理地址映射用户的虚拟地址上
    //应用mmap->...->驱动mmap
    //addr(vm_start)<----->0xE0200000
    addr = mmap(0, 0x1000, PROT_READ|PROT_WRITE,
                    MAP_SHARED, fd, 0);
    //计算寄存器物理地址对应的用户虚拟地址
    gpiocon = (unsigned long *)(addr + 0x80);
    gpiodata = (unsigned long *)(addr + 0x84);

    //配置为输出口
    *gpiocon &= ~((0xf << 12) | (0xf << 16));
    *gpiocon |= ((1 << 12) | (1 << 16));
   
    //设置状态
    if(!strcmp(argv[1], "on"))
        *gpiodata |= ((1 << 3) | (1 << 4));
    else
        *gpiodata &= ~((1 << 3) | (1 << 4));

    munmap(addr, 0x1000);
    close(fd);
    return 0;
}






