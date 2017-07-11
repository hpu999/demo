#include "mythread.h"
#include <unistd.h>
#include <fcntl.h>
#include <QDebug>
#include <QTime>

MyThread::MyThread() :
    QThread()
{
    qsrand(QTime(0, 0, 0).secsTo(QTime::currentTime()));
    m_flag = false;

    m_fd = open("/dev/mem", O_RDWR);

}

void MyThread::run()
{
    int i = 1;
    const int pointQty = 100;

    QByteArray data;
    while(m_flag) {
        qDebug("i = %d ... ...", i);
        for (int j = 0; j < pointQty; j++){
            data.append(qrand());
        }
        emit draw_data(data);
        usleep(1000);
        i += 10;
        data.clear();
    }
}

MyThread::~MyThread()
{
}
