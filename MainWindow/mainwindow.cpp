#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QLabel>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    listWidget = new QListWidget(this);
    listWidget->addItem(tr("Appearance"));
    listWidget->addItem(tr("Web Browser"));
    listWidget->addItem(tr("Mail & News"));
    listWidget->addItem(tr("Advanced"));

    QLabel *label = new QLabel(this);
    label->setText("0123456789");

    stackedLayout = new QStackedLayout(this);
    stackedLayout->addWidget(label);

    ui->horizontalLayout_2->addWidget(listWidget);
    ui->horizontalLayout_2->addLayout(stackedLayout);
    // qt an zhao bi li bu ju          lie  bi li
    ui->horizontalLayout_2->setStretch(0,   1);
    ui->horizontalLayout_2->setStretch(1,   3);
}

MainWindow::~MainWindow()
{
    delete ui;
}
