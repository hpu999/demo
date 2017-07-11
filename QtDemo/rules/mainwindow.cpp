#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ruler_widget.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_pRulerWgt = new RulerWidget(this);
    m_pRulerWgt->setFixedSize(width(), 20);
    m_pRulerWgt->set_type(RulerWidget::BOTTOM);
//    this->setCentralWidget(m_pRulerWgt);
}

MainWindow::~MainWindow()
{
    delete ui;
}
