#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    screen = QGuiApplication::primaryScreen();
    ui->label->setText("截屏");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    screen->grabWindow(0).save("123.jpg","jpg");
    ui->label->setText("Qscreen 截屏并保存 123.jpg");
}

void MainWindow::on_pushButton_2_clicked()
{
    QPixmap::grabWindow(QApplication::desktop()->winId()).save("456.jpg","jpg");
    ui->label->setText("Qpixmap 截屏并保存 456.jpg");
}
