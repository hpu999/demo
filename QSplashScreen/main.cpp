#include <QApplication>
#include <QTextEdit>
#include <QSplashScreen>
#include <QtTest>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QSplashScreen *splash = new QSplashScreen;
    splash->setPixmap(QPixmap(":/images/logo.jpg"));
    splash->show();
    Qt::Alignment topRight = Qt::AlignRight | Qt::AlignTop;
    splash->showMessage(QObject::tr("Setting up the main Window..."),
                        topRight,
                        Qt::red);
    QTest::qSleep(3000);
    QTextEdit *textEdit = new QTextEdit;
    splash->showMessage(QObject::tr("Loading modules..."),
                        topRight,
                        Qt::blue);
    QTest::qSleep(3000);
    textEdit->show();
    splash->finish(textEdit);
    delete splash;
    return app.exec();
}
