#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <string.h>


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
    char *pDevice,*pAction;
    UMSG umsg;
    int msgid;
    if((msgid = msgget(MSG_KEY,IPC_CREAT|0666))==-1) return 1; //取得IPC管道            
    pAction = getenv("ACTION"); //取得udev产生的动作，
    pDevice = getenv("DEVNAME");//取得设备名称如/dev/sda
  umsg.msgtype = 1;
    system("touch /opt/ACTION");
    system("touch /opt/DEVNAME");
    strcpy(umsg.content.szAction,pAction);
    strcpy(umsg.content.szDevice,pDevice);
    msgsnd(msgid,&umsg,sizeof(UMSGCONTENT),IPC_NOWAIT);
    return 0;
}
