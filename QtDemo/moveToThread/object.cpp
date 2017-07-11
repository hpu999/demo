#include "object.h"
#include <unistd.h>
#include <QDebug>

Object::Object(QObject *parent) :
    QObject(parent)
{
    m_flag = true;
}

void Object::deal_data()
{
    int i = 0;
    while (m_flag) {
        qDebug("%s[%d] : i[%d]", __func__, __LINE__, i++);
        usleep (1000);
    }
}
