#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

typedef struct _msgcontent{
	char szAction[512];   //USB产生的动作
	char szDevice[512];   //USB设备名称
	char mountPath[512];  //u盘挂载地址
}UMSGCONTENT;

int main (int argc, char *argv[])
{
	int fd;
	UMSGCONTENT umsg;
	strcpy (&umsg.szAction, "add");
	strcpy (&umsg.szDevice, "/dev/sda1");
	strcpy (&umsg.mountPath, "/mnt");

	mkfifo ("/tmp/.mountFifo", 0777);
	fd = open ("/tmp/.mountFifo", O_WRONLY);

	write (fd, &umsg, sizeof (umsg));

	close (fd);

	return 0;
}
