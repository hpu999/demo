#include <QtCore/QCoreApplication>
#include <QSettings>
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QSettings settings("E:/term/QSettings/test.ini", QSettings::IniFormat);
    settings.setValue("Student/name", "ChengKeKe");
    settings.setValue("Student/sex", "man");
    settings.setValue("Student/age", 24);
    settings.setValue("Address/ShengHui", "Xi'an");
    settings.setValue("Phone", 159088888888);
    QString name = settings.value("Student/name").toString();
    QString sex = settings.value("Student/sex").toString();
    return a.exec();
}
