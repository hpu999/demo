#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <fcntl.h>
#include <poll.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

#define GPIO_NAME "/dev/gpio"

struct demo {
	int num;
	int *pnum;
	int num1;
	int *pnum1;
	char ch1;
	char ch2;
	char ch3;
	char ch4;
};

#define IOCTL_BASE 'd'
#define DRM_IOWR(nr,type) _IOWR(IOCTL_BASE,nr,type)
#define DRM_IOCTL_CAP     DRM_IOWR(0x0c, struct demo) 

int main(int argc,char **argv) {
	int fd  = -1;
	int res = 0 ;

	struct demo demo1;
	memset (&demo1, 'A', sizeof demo1);

	fd = open (GPIO_NAME, O_RDWR);
	if (fd < 0) {
		perror ("open");
		return -1;
	}

	printf ("%u << %u \n", 3,  _IOC_DIRSHIFT);
	printf ("%u << %u \n", 'd', _IOC_TYPESHIFT);
	printf ("%u << %u \n", 0x0c, _IOC_NRSHIFT);
	printf ("%u << %u \n", 20, _IOC_SIZESHIFT);

	printf ("_IOC_DIRSHIFT[%d] \n", _IOC_DIRSHIFT);
	printf ("_IOC_TYPESHIFT[%d] \n", _IOC_TYPESHIFT);
	printf ("_IOC_NRSHIFT[%d] \n", _IOC_NRSHIFT);
	printf ("_IOC_SIZESHIFT[%d] \n", _IOC_SIZESHIFT);
	
	printf ("%s:%s[%d] DRM_IOCTL_CAP[%x] \n", 
		__FILE__, __func__, __LINE__, DRM_IOCTL_CAP);
	
	res = ioctl (fd, DRM_IOCTL_CAP, &demo1);

	close (fd);
	return 0;
}
