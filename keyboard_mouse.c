#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/input.h>
#include <linux/uinput.h>
#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

void simulate_key(int fd,int kval)
{
	struct input_event event;

	event.type = EV_KEY;
	event.value = 1;
	event.code = kval;
	gettimeofday(&event.time, 0);
	write(fd,&event, sizeof(event));
	event.type = EV_SYN;
	event.code = SYN_REPORT;
	event.value = 0;
	write(fd,&event, sizeof(event));
	memset(&event, 0, sizeof(event));
	gettimeofday(&event.time,NULL);
	event.type = EV_KEY;
	event.code = kval;
	event.value = 0;
	write(fd,&event, sizeof(event));
	event.type = EV_SYN;
	event.code = SYN_REPORT;
	event.value = 0;
	write(fd,&event, sizeof(event));
}

void simulate_mouse(int fd)
{
	struct input_event event;

	memset(&event, 0, sizeof(event));
	gettimeofday(&event.time,NULL);
	event.type = EV_REL;
	event.code = REL_X;
	event.value = 10;
	write(fd,&event, sizeof(event));
	event.type = EV_REL;
	event.code = REL_Y;
	event.value = 10;
	write(fd,&event, sizeof(event));
	event.type = EV_SYN;
	event.code = SYN_REPORT;
	event.value = 0;
	write(fd,&event, sizeof(event));
}


// /dev/input/mice是鼠标的抽象，代表的是鼠标，也许是/dev/input/mouse,
// /dev/input/mouse1，或者空，这个文件一直会存在。这里你也许会问，我怎
// 么知道/dev/input/eventX这些事件到底是什么事件阿，是鼠标还是键盘或者别的，
// eventX代表的是所有输入设备(input核心)的事件，比如按键按下，或者鼠标移动，
// 或者游戏遥控器等等，在系统查看的方法是 cat /proc/bus/input/devices
// 就可以看到每个eventX是什么设备的事件了。

int main()
{
	int fd_kbd;
	int fd_mouse;

	fd_kbd = open("/dev/input/event2", O_RDWR);
	if(fd_kbd <= 0)
	{
		printf("error open keyboard:\n");
		return -1;
	}

	fd_mouse = open("/dev/input/event4", O_RDWR);
	if(fd_mouse <= 0)
	{
		printf("error open mouse\n");
		return -2;
	}

	int i = 0;
	for(i = 0; i < 10; i++)
	{
		simulate_key(fd_kbd, KEY_A + i);
		simulate_mouse(fd_mouse);
		sleep(1);
	}
	close(fd_kbd);
}
