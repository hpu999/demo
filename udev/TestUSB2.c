#include <stdlib.h>
#include <stdio.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>

#define MSG_KEY     0x780078 //定义IPC消息key

//定义IPC消息传送的内容
typedef struct _msgcontent{
     char szAction[64];   //USB产生的动作
     char szDevice[64]; //USB设备名称
}UMSGCONTENT;

typedef struct _umsg{
     long msgtype;
     UMSGCONTENT content;
}UMSG;

int main(){
    int _nMessageId;
    UMSG _msg      ;
    _nMessageId = msgget((key_t)MSG_KEY, 0666 | IPC_CREAT);
    if (_nMessageId == -1)
    {
        printf("msgget failed with error: %d\n", errno);
        return ;
    }
    while(1)
    {
        if (msgrcv(_nMessageId, (void *)&_msg, sizeof(UMSGCONTENT),
                   1, 0) == -1)
        {
       	    printf ("msgrcv failed with error\n");
            return ;
        }
        else
        {
            printf("%s  %s\n" ,_msg.content.szAction ,  _msg.content.szDevice);
            if (!strcmp("add" ,_msg.content.szAction))
            {
                printf ("add\n");
                system ("mount /dev/usbStorageDoppler /opt/usbStorage");
			}
            if (!strcmp("remove" ,_msg.content.szAction))
            {
                printf ("remove\n");
                system ("umount /opt/usbStorage");
            }
        }
    }
    return 0;
}
