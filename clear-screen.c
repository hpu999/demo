#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <fcntl.h>

struct fb_var_screeninfo vinfo;

int main (int argc, char *argv[]) {

	int fbfd, fbsize, i;
	unsigned char *fbbuf;
	
	/* open video memory */
	if (open ("/dev/fb0", O_RDWR) < 0) {
		perror ("open");
		exit (1);
	}
	printf ("open successed. \n");

	/* get variable display parameters */
	if (ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo)) {
		printf ("Bad vscreeninfo ioctl \n");
		exit (2);
	}
	printf ("ioctl successed. \n");

	/* size of frame buffer = (x-resolution * y-resoulation * bytes per pixel) */
	fbsize = vinfo.xres * vinfo.yres * (vinfo.bits_per_pixel / 8);

	/* map video memory */
	if ((fbbuf = mmap (0, fbsize, PROT_READ | PROT_WRITE, MAP_SHARED, 
							fbfd, 0)) == (void *)-1) {
		perror ("mmap");
		exit (3);
	}
	printf ("mmap successed. \n");

	/* clear the screen */
	for (i = 0; i < fbsize; ++i) {
		*(fbbuf + i) = 0x0;
	}

	munmap (fbbuf, fbsize);
	close (fbfd);

	return 0;
}
