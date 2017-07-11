#include "mainwindow.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{    
    this->setGeometry(0, 0, 400, 400);
    pb = new QPushButton("AA", this);
    thread = new MyThread(pb);

    connect(thread, SIGNAL(change_postion(int)), this, SLOT(move_button(int)), Qt::QueuedConnection);

    thread->start();
}

MainWindow::~MainWindow()
{
    delete thread;
}

void MainWindow::move_button(int i)
{
    pb->move(i, i);
}
