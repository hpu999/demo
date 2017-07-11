/**
 * @file a_scan_widget.cpp
 * @brief 画A扫
 * @author Jake Yang <yanghuanjie@cndoppler.cn>
 * @date 2017-06-28
 */
#include "a_scan_widget.h"

#include <QPainter>
#include <QDebug>

AscanWidget::AscanWidget(QWidget *parent):
    QWidget(parent),
    m_color("#ffff77")
{

}

void AscanWidget::show(const QByteArray &b)
{
    if (b.size() == 0) {
        return;
    }
    m_beam.setRawData(b.constData(), b.size());
    update();
}

QPainterPath AscanWidget::paint_wave()
{
    QPainterPath path;

    float xRatio1 = 1.0;
    float xRatio2 = 1.0;
    float yRatio = y_axis_length() / 255.0;

    int drawPoints = 0;
    if ( m_beam.size() < x_axis_length()) {
        xRatio1 = x_axis_length() / 1.0 / m_beam.size();
        drawPoints = m_beam.size();
    } else {
        xRatio2 = m_beam.size() / 1.0 / x_axis_length();
        drawPoints = x_axis_length();
    }

    for (int i = 0; i < drawPoints; ++i) {
        path.lineTo( i*xRatio1,
                     ((quint8)(m_beam.at((int)(i*xRatio2)))) * yRatio + 0.5);
    }

    return path;
}

/*
QPainterPath AscanWidget::paint_gate()
{

}
*/
