#include <QtCore/QCoreApplication>
#include <QsharedPointer>

class A {
public:
    A (int i = 5, int j = 5):
        m_num (i), m_num1 (j) {}
    void print () {
        qDebug("The auto_ptr is printed.\n");
    }
private:
    int m_num;
    int m_num1;
};

typedef QSharedPointer<A> Pint;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    Pint p_A;

    Pint *p_p_A = &p_A;

    p_A->print();

    p_A.data()->print (); // data()返回类A的指针

    p_p_A->data()->print ();

    qDebug("This is a QsharePointer.\n");



    return a.exec();
}
