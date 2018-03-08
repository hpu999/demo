#include <stdio.h>    
#include <stdlib.h>    
#include <linux/input.h>    
#include <fcntl.h>    
#include <sys/time.h>    
#include <sys/types.h>    
#include <sys/stat.h>    
#include <unistd.h>    

#define GPIO_NAME "/dev/gpio"

int main(int argc,char **argv) {
	int fd  = -1;

	fd = open (GPIO_NAME, O_RDWR);
	if (fd < 0) {
		perror ("open");
		return -1;
	}

	/* select() 使用例程 */
	int retval;
	fd_set readfds;
	struct timeval tv;
	char buf[6];
	// 设置最长等待时间
	tv.tv_sec = 5; 
	tv.tv_usec = 0;
	FD_ZERO (&readfds);
	FD_SET (fd, &readfds);

	retval = select (fd + 1, &readfds, NULL, NULL, &tv);
    if(retval==0) {    
        printf( "Time out!\n" );    
    }else if(retval<0){
        printf( "error!\n" ); 
    }else{
		if(FD_ISSET(fd,&readfds)) {    
 			// 读取鼠标设备中的数据    
 			if(read(fd, buf, 6) <= 0) {    
 				exit (1);    
 			}    
 		}  
     }	

	close (fd);
	return 0;
}
