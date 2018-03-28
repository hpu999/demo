#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QKeyEvent>
#include <QCursor>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->pushButtonDown->setStyleSheet("padding: 0");
    ui->pushButtonUp->setStyleSheet("padding: 0");
    ui->pushButtonLeft->setStyleSheet("padding: 0");
    ui->pushButtonRight->setStyleSheet("padding: 0");
    ui->pushButtonOk->setStyleSheet("padding: 0");

    ui->pushButtonDown->installEventFilter(this);
    ui->pushButtonUp->installEventFilter(this);
    ui->pushButtonLeft->installEventFilter(this);
    ui->pushButtonRight->installEventFilter(this);
    ui->pushButtonOk->installEventFilter(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButtonUp_clicked()
{
    qDebug() << "Up";
    QKeyEvent Up(QEvent::KeyPress, Qt::Key_Up, Qt::NoModifier);
    QCoreApplication::sendEvent(this, &Up);
}

void MainWindow::on_pushButtonDown_clicked()
{
    qDebug() << "Down";
    QKeyEvent Down(QEvent::KeyPress, Qt::Key_Down, Qt::NoModifier);
    QCoreApplication::sendEvent(this, &Down);
}

void MainWindow::on_pushButtonLeft_clicked()
{
    qDebug () << "Left";
    QKeyEvent Left(QEvent::KeyPress, Qt::Key_Left, Qt::NoModifier);
    QCoreApplication::sendEvent(this, &Left);
}

void MainWindow::on_pushButtonRight_clicked()
{
    qDebug () << "Right";
    QKeyEvent Right(QEvent::KeyPress, Qt::Key_Right, Qt::NoModifier);
    QCoreApplication::sendEvent(this, &Right);
}

void MainWindow::on_pushButtonOk_clicked()
{
    qDebug() << "Ok";
    QKeyEvent Ok(QEvent::KeyPress, Qt::Key_Enter, Qt::NoModifier);
    QCoreApplication::sendEvent(this, &Ok);
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    qDebug () << "00000";
    if (event->type() == QKeyEvent::KeyPress) {
        if (event->key() == Qt::Key_Up) {
            ui->textEdit->moveCursor(QTextCursor::Up);
        } else if (event->key() == Qt::Key_Down) {
            ui->textEdit->moveCursor(QTextCursor::Down);
        } else if (event->key() == Qt::Key_Left) {
            ui->textEdit->moveCursor(QTextCursor::Left);
        } else if (event->key() == Qt::Key_Right) {
            ui->textEdit->moveCursor(QTextCursor::Right);
        } else if (event->key() == Qt::Key_Enter) {
            ui->textEdit->selectAll();
        }
        ui->textEdit->setFocus();
    }
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    qDebug () << "111111111";
}
