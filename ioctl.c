#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#define LED_ON  0x100001
#define LED_OFF 0x100002

int main(int argc, char *argv[])
{
    int fd;
    int uindex; //用户缓冲区

    if (argc != 3) {
        printf("Usage: %s <on|off> <1|2>\n", argv[0]);
        return -1;
    }

    fd = open("/dev/myled", O_RDWR);
    if (fd < 0)
        return -1;
    
    //"1" - > 1
    uindex = strtoul(argv[2], NULL, 0);

    if (!strcmp(argv[1], "on"))
        ioctl(fd, LED_ON, &uindex);
    else
        ioctl(fd, LED_OFF, &uindex);

    close(fd);
    return 0;
}





