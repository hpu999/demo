#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <fcntl.h>

/* 视频卡的各个属性
 * struct fb_var_screeninfo {
 *     __u32 xres;            // visable resolution in the X axis
 *     __u32 yres;            // visable resolution in the Y axis
 *     __u32 bits_per_pixel;  // number of bits required to hold a pixel
 *     ... ...	
 * }
 */
/* 视频硬件信息，用户无法改变
 * struct fb_fix_screeninfo {
 *     ... ...
 *     unsigned long smem_start; // start of frame buffer memory
 *     __u32 smem_len;           // length of frame buffer memory
 *     ... ...
 * }
 */
/* 规定了颜色映射
 * struct fb_cmp {
 *     ... ...
 * }
 */
struct fb_var_screeninfo vinfo;
struct fb_fix_screeninfo finfo;

int main (int argc, char *argv[]) {

	int fbfd, fbsize, i;
	unsigned char *fbbuf;
	int ret;
	
	/* open video memory */
	if ((fbfd = open ("/dev/fb1", O_RDWR)) < 0) {
		perror ("open");
		exit (1);
	}
	printf ("open successed. \n");

	if (ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo)){
		printf("Error reading fixed information\n");
		exit(2);
	}
 
	/* get variable display parameters */
	if (ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo)) {
		printf ("Bad vscreeninfo ioctl. ret = %d \n", ret);
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
