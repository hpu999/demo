#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "object.h"
#include <QMainWindow>
#include <QThread>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QThread *thread;
    Object  *obj;
};

#endif // MAINWINDOW_H
