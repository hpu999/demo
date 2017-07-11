#include <QtGui/QApplication>
#include "a_scan_hwidget.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    AscanHWidget ascan ;


    ascan.QWidget::show();

    return a.exec();
}
