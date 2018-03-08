#include <stdio.h>    
#include <stdlib.h>    
#include <linux/input.h>    
#include <fcntl.h>    
#include <sys/time.h>    
#include <sys/types.h>    
#include <sys/stat.h>    
#include <unistd.h>    

int main(int argc,char **argv)    
{    
    int fd, retval;    
    char buf[6];    
    fd_set readfds;    
    struct timeval tv;    

    // 打开鼠标设备    
    fd = open( "/dev/input/mice", O_RDONLY );    
    // 判断是否打开成功    
    if(fd<0) {    
        printf("Failed to open \"/dev/input/mice\".\n");    
        exit(1);    
    } else {    
        printf("open \"/dev/input/mice\" successfuly.\n");    
    }    
    
	int fd_kb;     
    struct input_event event_kb;  
    fd_kb = open("/dev/input/event1", O_RDONLY); //键盘输入  
	if(fd_kb <= 0)  
	{  
			printf("open device error\n");  
			return 0;  
	}
	
	int fd_mouse;    
	struct input_event event_mouse;  	  
	fd_mouse = open("/dev/input/event4", O_RDONLY); //鼠标输入  
	if(fd_mouse <= 0)  
	{  
			printf("open device error\n");  
			return 0;  
	}
	
	
    while(1) {    
        // 设置最长等待时间    
        tv.tv_sec = 5;    
        tv.tv_usec = 0;    
    
        FD_ZERO( &readfds );    
        FD_SET( fd, &readfds );    
        FD_SET( fd_kb, &readfds ); 
        FD_SET( fd_mouse, &readfds ); 
    
		int max=fd>fd_kb?(fd>fd_mouse?fd:fd_mouse):(fd_kb>fd_mouse?fd_kb:fd_mouse);
		
        retval = select( max+1, &readfds, NULL, NULL, &tv );//有文件描述符的状态发生变化（变为可读）或者超时和错误就返回，否则就阻塞    
        if(retval==0) {    
            printf( "Time out!\n" );    
        }else if(retval<0){
			printf( "error!\n" ); 
		}else{						//判断是哪个文件描述符的状态发生了变化，进行处理即可
		
			//鼠标位置
			if(FD_ISSET(fd,&readfds)) 
			{    
				// 读取鼠标设备中的数据    
				if(read(fd, buf, 6) <= 0) {    
					continue;    
				}    
				// 打印出从鼠标设备中读取到的数据    
				printf("Button type = %d, X = %d, Y = %d, Z = %d\n", (buf[0] & 0x07), buf[1], buf[2],   buf[3]);    
			}  
			
			//键盘点击
			if(FD_ISSET(fd_kb,&readfds)) 
			{    
				if(read(fd_kb, &event_kb, sizeof(event_kb)) == sizeof(event_kb))  
                {  
                    if (event_kb.type == EV_KEY)  
                    {  
                        //if (event_kb.value == 0 || event_kb.value == 1)//1表示按下，0表示释放，会检测到两次  
                        if (event_kb.value == 1)//键按下  
                        {  
                                //printf("key %d %s\n", event_kb.code, (event_kb.value) ? "Pressed" : "Released");  
                                if(event_kb.code == KEY_ESC)  
                                        break;  
                                if(event_kb.code == KEY_Q)  
                                    printf("q\n");                                   
                                if(event_kb.code == KEY_W)  
                                    printf("w\n");    
                                if(event_kb.code == KEY_E)  
                                    printf("e\n");    
                                if(event_kb.code == KEY_R)  
                                    printf("r\n");    
                        }  
  
                    }  
                  
                }     
			}  
			
			//鼠标点击
			if(FD_ISSET(fd_mouse,&readfds)) 
			{    
				if(read(fd_mouse, &event_mouse, sizeof(event_mouse)) == sizeof(event_mouse))  
                {                                     
                    if(event_mouse.type = EV_REL)  
                    {  
                        if(event_mouse.code == REL_WHEEL)         
                            printf("REL_WHEEL %d\n", event_mouse.value);//-1表示下滑，1表示上滑  
                    }     
       
                    if(event_mouse.code == BTN_LEFT && event_mouse.value==1)//左键按下，1表示按下，0表示释放。不然会检测到两次  
                        printf("left down: %d\n", event_mouse.code);  
                          
                    if(event_mouse.code == BTN_RIGHT && event_mouse.value==1)  
                        printf("right down: %d\n", event_mouse.code);  
                          
                    if(event_mouse.code == BTN_MIDDLE && event_mouse.value==1)  
                        printf("middle down: %d\n", event_mouse.code);  

                } 
			}  	
		
		}		
  
    }    
    close(fd);  
	close(fd_kb); 
	close(fd_mouse);  	
    return 0;    
} 




  



