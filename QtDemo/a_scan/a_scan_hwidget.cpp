/**
 * @file a_scan_hwidget.cpp
 * @brief A扫横向显示
 * @author Jake Yang <yanghuanjie@cndoppler.cn>
 * @date 2017-06-28
 */
#include "a_scan_hwidget.h"
#include <QDebug>
#include <QPainter>
#include <QThread>
#include <QTime>

AscanHWidget::AscanHWidget(QWidget *parent) :
    AscanWidget(parent)
{
    setFixedSize(800, 600);
    setWindowTitle("ASCAN");
    setAutoFillBackground(true);

    QPalette palette;
    palette.setColor(QPalette::Background, QColor(0, 0, 0));
    setPalette(palette);

    m_draw_thread = new MyThread;
    connect(m_draw_thread, SIGNAL(draw_data(QByteArray)),
            this, SLOT(recive_data(QByteArray)), Qt::QueuedConnection);
    m_draw_thread->set_flag(true);
    m_draw_thread->start();

}

AscanHWidget::~AscanHWidget()
{
    m_draw_thread->set_flag(false);
    m_draw_thread->quit();
    m_draw_thread->wait();
    m_draw_thread->deleteLater();
}

int AscanHWidget::x_axis_length() const
{
    return width();
}

int AscanHWidget::y_axis_length() const
{
    return height();
}

void AscanHWidget::paintEvent(QPaintEvent *e)
{
//    qDebug("%s[%d]: ",__func__, __LINE__);

    QPainter painter(this);
    painter.translate(0, height());

    QTransform form = painter.transform();

    form.rotate(180, Qt::XAxis);

    painter.setTransform(form);

    painter.setPen( wave_color() );
    painter.drawPath( paint_wave() );
}

void AscanHWidget::recive_data(QByteArray waveData)
{
    this->AscanWidget::show(waveData);
}
