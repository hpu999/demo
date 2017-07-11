#ifndef OBJECT_H
#define OBJECT_H

#include <QObject>

class Object : public QObject
{
    Q_OBJECT
public:
    explicit Object(QObject *parent = 0);
    void set_flag (bool flag) {m_flag = flag;}

signals:

public slots:
    void deal_data();

private:
    bool m_flag;
};

#endif // OBJECT_H
