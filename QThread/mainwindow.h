#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MyObject;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void onFirstPushed();
    void onSecondPushed();
    void onThreePushed();
    void onSelfPushed();
    void onExitPushed();

private:
    Ui::MainWindow *ui;
    MyObject *my;
};

#endif // MAINWINDOW_H
