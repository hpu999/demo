#ifndef MYOBJECT_H
#define MYOBJECT_H

#include <QObject>
#include <QDebug>
#include <QThread>

class MyObject : public QObject {
    Q_OBJECT
    public:
        MyObject() {};
        ~MyObject() {}

    public slots:
        void first() {
            qDebug() << QThread::currentThreadId();
        }
        void second() {
            qDebug() << QThread::currentThreadId();
        }
        void three() {
            qDebug() << QThread::currentThreadId();
        }
};

#endif // MYOBJECT_H
