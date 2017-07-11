#include "mythread.h"
#include <QDebug>

MyThread::MyThread(QPushButton *pb) :
    QThread()
{
    this->pb = pb;
}

void MyThread::run()
{
    int i = 0;
    while(true)
    {
        qDebug("The number is : %d", i);
        i += 10;
        emit change_postion(i); // 多线程间的通信
        this->msleep(1000);
        if(i == 400)
        {
            i = 0;
        }
    }
}

MyThread::~MyThread()
{
}
