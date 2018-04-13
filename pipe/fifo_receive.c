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

	fd = open ("/tmp/.mountFifo", O_RDONLY);
	if (fd < 0) {
		printf ("open failed\n");
		return -1;
	}
	
	printf ("open succes\n");
	
	while (1) {
		read (fd, &umsg, sizeof (umsg));

		printf ("szAction  [%s] \n", umsg.szAction);
		printf ("szDevice  [%s] \n", umsg.szDevice);
		printf ("mountPath [%s] \n", umsg.mountPath);
	}
	close (fd);

	return 0;
}
