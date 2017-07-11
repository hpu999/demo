#ifndef MYTHREAD_H
#define MYTHREAD_H

#include <QThread>
#include <QPushButton>

class MyThread : public QThread
{
    Q_OBJECT
public:
    explicit MyThread(QPushButton *pb);
    ~MyThread();
    QPushButton *pb;
signals:
    void change_postion(int);

protected:
    void run();

public slots:

private:


};

#endif // MYTHREAD_H
