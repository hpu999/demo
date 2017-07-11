#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    thread = new QThread;
    obj = new Object;

    obj->moveToThread(thread);

    QObject::connect(thread, SIGNAL(started()),
                     obj, SLOT(deal_data()));

    thread->start();
}

MainWindow::~MainWindow()
{
    delete ui;
}
