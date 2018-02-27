#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h> 
#include <linux/fb.h>
#include <time.h>
#include <sys/mman.h>

int main()
{
	int fbfd = 0;
	struct fb_var_screeninfo vinfo;
	struct fb_fix_screeninfo finfo;
	long int screensize = 0;
	char *fbp = 0;
	int x = 0, y = 0;
	long int location = 0;
	fbfd = open("/dev/fb0", O_RDWR);	// Open the file for reading and writing
	if (!fbfd)
	{
	printf("Error: cannot open framebuffer device.\n");
	exit(0);
	}
	printf("The framebuffer device was opened successfully.\n");

	// Get fixed screen information
	if (ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo))
	{
	printf("Error reading fixed information.\n");
		exit(0);
	}

	// Get variable screen information
	if (ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo)) 
	{            
	printf("Error reading variable information.\n");
		exit(0);
	}
	printf("%dx%d, %dbpp\n", vinfo.xres, vinfo.yres, vinfo.bits_per_pixel);
    // Figure out the size of the screen in bytes 
	screensize = vinfo.xres * vinfo.yres * vinfo.bits_per_pixel / 8;


    printf("xres %d\n",vinfo.xres);//可见解析度  320
    printf("yres %d\n",vinfo.yres);//            240
    printf("xres_virturl %d\n",vinfo.xres_virtual);//虚拟解析度  320
    printf("yres_virtual %d\n",vinfo.yres_virtual);//              240
    printf("xoffset %d\n",vinfo.xoffset);//虚拟到可见的偏移        0
    printf("yoffset %d\n",vinfo.yoffset);//                         0
    printf("bits_per_pixel %d\n",vinfo.bits_per_pixel);//每像素位数 bpp  16
    printf("grayscale %d\n",vinfo.grayscale);//非零时，指灰度

    printf("fb_bitfield red.offset %d\n",vinfo.red.offset);//     11  偏移11位
    printf("fb_bitfield red.length %d\n",vinfo.red.length);//         5
    printf("fb_bitfield red.msb_right %d\n",vinfo.red.msb_right);//    0
    printf("fb_bitfield green.offset %d\n",vinfo.green.offset);//  5 偏移5位
    printf("fb_bitfield green.length %d\n",vinfo.green.length);//        6
    printf("fb_bitfield green.msb_right %d\n",vinfo.green.msb_right);//  0
    printf("fb_bitfield blue.offset %d\n",vinfo.blue.offset);//
    printf("fb_bitfield blue.length %d\n",vinfo.blue.length);//
    printf("fb_bitfield blue.msb_right %d\n",vinfo.blue.msb_right);//
    printf("fb_bitfield transp.offset %d\n",vinfo.transp.offset);//
    printf("fb_bitfield transp.length %d\n",vinfo.transp.length);//
    printf("fb_bitfield transp.msb_right %d\n",vinfo.transp.msb_right);//

    printf("nonstd %d\n",vinfo.nonstd);//!=0 非标准像素格式
    printf("activate %d\n",vinfo.activate);
    printf("height %d\n",vinfo.height);//高度/  240
    printf("widht %d\n",vinfo.width);//             320
    printf("accel_flags %d\n",vinfo.accel_flags);//看 fb_info.flags

    //定时，除了 pixclock之外，其他的都以像素时钟为单位
    printf("pixclock %d\n",vinfo.pixclock);//像素时钟，皮秒   80000
    printf("left_margin %d\n",vinfo.left_margin);//行切换：从同步到绘图之间的延迟    28
    printf("right_margin %d\n",vinfo.right_margin);//行切换：从绘图到同步之间的延迟   24
    printf("upper_margin %d\n",vinfo.upper_margin);//帧切换：从同步到绘图之间的延迟   6
    printf("lower_margin %d\n",vinfo.lower_margin);//帧切换：从绘图到同步之间的延迟    2
    printf("width %d\n",vinfo.width);
    printf("height %d\n",vinfo.height);
    printf("hsync_len %d\n",vinfo.hsync_len); //hor 水平同步的长度         42
    printf("vsync_len %d\n",vinfo.vsync_len); //vir 垂直同步的长度         12

    printf("sync %d\n",vinfo.sync); //
    printf("vmode %d\n",vinfo.vmode);
    printf("rotate %d\n",vinfo.rotate);

    printf("id %s\n",finfo.id);//id
    printf("smem_start %lu\n",finfo.smem_start); //帧缓冲 内存开始地址,物理地址
    printf("smem_len %d\n",finfo.smem_len); // 帧缓冲 内存 长度
    printf("type %d\n",finfo.type);
    printf("type_aux %d\n",finfo.type_aux);//平面交织交替
    printf("visual %d\n",finfo.visual); //记录 色彩模式   2
    printf("xpanstep %d\n",finfo.xpanstep);//如果没有硬件panning，赋0
    printf("ypanstep %d\n",finfo.ypanstep);//
    printf("line_length %d\n",finfo.line_length);//line hength;
    printf("mmio_start %lu\n",finfo.mmio_start);//内存映射IO开始地址 物理地址
    printf("mmio_len %d\n",finfo.mmio_len);//内存映射IO 长度
    printf("accel %d\n\n",finfo.accel);


    // Map the device to memory
	fbp = (char *)mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);

	if((int)fbp == -1)
	{
	printf("Error: failed to map framebuffer device to memory.\n");
	exit(0);
	}

	printf("The framebuffer device was mapped to memory successfully.\n");
	x = 100;		y = 100;

// Where we are going to put the pixel   
// Figure out where in memory to put the pixel
	for (y = 100; y < 300; y++)
	{
		for (x = 100; x < 300; x++) 
		{
		location = (x+vinfo.xoffset) * (vinfo.bits_per_pixel/8) +(y+vinfo.yoffset) * finfo.line_length; 
		if (vinfo.bits_per_pixel == 32)
			{
			*(fbp + location) = 100; // Some blue 
			*(fbp + location + 1) = 15+(x-100)/2;// A little green
			*(fbp + location + 2) = 200-(y-100)/5;// A lot of red
			*(fbp + location + 3) = 0;// No transparency
			}
		else
			{ 
			int b = 10;//assume 16bpp
			int g = (x-100)/6;// A little green
			int r = 31-(y-100)/16;// A lot of red
			unsigned short int t = r<<11 | g << 5 | b;
			*((unsigned short int*)(fbp + location)) = t;
			}
		}
	}
	munmap(fbp, screensize);
	close(fbfd);
	return 0;
}
