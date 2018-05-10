#include "mainwindow.h"
#include <QPushButton>
#include <QVBoxLayout>
#include "myobject.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    my = new MyObject;
    QPushButton *firstButton = new QPushButton(tr("first"), 0);
    connect(firstButton, SIGNAL(clicked()), my, SLOT(first()), Qt::QueuedConnection);
    QPushButton *secondButton = new QPushButton(tr("second"), 0);
    connect(secondButton, SIGNAL(clicked()), my, SLOT(second()), Qt::QueuedConnection);
    QPushButton *threeButton = new QPushButton(tr("three"), 0);
    connect(threeButton, SIGNAL(clicked()), my, SLOT(three()), Qt::QueuedConnection);
    QPushButton *selfButton = new QPushButton(tr("self"), 0);
    connect(selfButton, SIGNAL(clicked()), this, SLOT(onSelfPushed()));
    QPushButton *exitButton = new QPushButton(tr("exit"), 0);
    connect(exitButton, SIGNAL(clicked()), this, SLOT(onExitPushed()));

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(firstButton);
    layout->addWidget(secondButton);
    layout->addWidget(threeButton);
    layout->addWidget(selfButton);
    layout->addWidget(exitButton);

    QWidget *p = new QWidget;
    p->setLayout(layout);

    QThread *thread = new QThread;
    my->moveToThread(thread);

    thread->start();
    connect(thread, SIGNAL(started()), my, SLOT(first()));

    setCentralWidget(p);
}

MainWindow::~MainWindow() {
}

void MainWindow::onFirstPushed() {
    my->first();
}

void MainWindow::onSecondPushed() {
    my->second();
}

void MainWindow::onThreePushed() {
    my->three();
}

void MainWindow::onSelfPushed() {
    qDebug() << QThread::currentThreadId();
}

void MainWindow::onExitPushed() {
    close();
}
