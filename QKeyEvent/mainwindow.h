#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_pushButtonUp_clicked();

    void on_pushButtonDown_clicked();

    void on_pushButtonLeft_clicked();

    void on_pushButtonRight_clicked();

    void on_pushButtonOk_clicked();

    void keyPressEvent(QKeyEvent *event);

    bool eventFilter(QObject *object, QEvent *event);

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
