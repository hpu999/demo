#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <termios.h>

int main(int argc, char *argv[])
{
    int fd = open("/dev/ttymxc2", O_RDWR);
    if (fd < 0) {
        perror ("open");
        return -1;
    }

    struct termios newtermios;
	/* 清除结构体以放入新的序列埠设定值 */
	bzero(&newtermios, sizeof(newtermios)); 
	tcgetattr(fd, &newtermios);
	/* control characters */
	newtermios.c_cc[VMIN] = 1;
	/* 输入模式标志 */
	newtermios.c_iflag = 0;
	/* 输出模式标志 */
	newtermios.c_oflag = 0;
	/* local mode flags */
	newtermios.c_lflag = 0; 
	newtermios.c_cflag	= B115200 | CS8 | PARENB | CLOCAL | CREAD;
	if (tcsetattr(fd, TCSANOW, &newtermios)) {
		perror("tcsetattr   error");  
		exit(1);  
	}

    unsigned char buf[8] = {0};
    buf[0] = 0xff;
    buf[1] = 0xff;
    buf[2] = 0x53;
    buf[3] = 0x01;
    buf[4] = 0x44;
    buf[5] = 0x01;
    buf[6] = 0xfe;
    buf[7] = 0xfe;

    int res = write (fd, buf, 8);
    if (res < 0) {
        perror ("write");
        return -1;
    }

    close(fd);
    return 0;
}






