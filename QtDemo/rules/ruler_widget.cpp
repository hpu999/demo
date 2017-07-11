#include "ruler_widget.h"
#include <QPainter>
#include <QDebug>

RulerWidget::RulerWidget(QWidget *parent) : QWidget(parent)
{
    m_bgColor = QColor("#ff88ff");
    m_start = 0;
    m_end = 100;
    m_unitName = "(mm)";
    m_type = BOTTOM;
    m_direction = Up;
}

bool RulerWidget::set_range(double start, double end)
{
    if (start >= end) {
        return false;
    }

    m_start = start;
    m_end = end;

    return true;
}

void RulerWidget::paintEvent(QPaintEvent *e)
{
    Q_UNUSED(e);

    QPainter painter(this);

    double interval = 0.0;          /* 单位/刻度 */
    double m_pixelPerUnit = 0.0;    /* 像素/单位 */
    int markQty = 0;                /* 刻度数 */
    int length = 0;                 /* 标尺像素长度 */


    /* 设置背景 */
    painter.setBrush(m_bgColor);
    painter.drawRect(this->rect());

    painter.setPen(QColor(Qt::black));

    if (RulerWidget::BOTTOM == m_type) {
        m_pixelPerUnit = width() / (m_end - m_start);
        length = width();
    } else {
        m_pixelPerUnit = height() / (m_end - m_start);
        length = height();
    }

    if (length >= 400) {
        markQty = 100;
    } else if ( length > 300 && length < 400 ) {
        markQty = 80;
    } else if ((length > 200) && (length < 300)) {
        markQty = 50;
    } else if ((length > 100) && ( length<200 )) {
        markQty = 40;
    } else {
        markQty = 20;
    }

    interval = (m_end - m_start) / markQty;

    if (interval < 0.01 ) {
        interval = 0.01;
    } else if ( 0.01 <=interval && interval < 0.1 ) {
        interval = ((int)(interval * 100)) / 100.0 ;
    } else if ( 0.1 <=interval  && interval < 1.0 ) {
        interval = ((int)(interval * 10)) / 10.0;
    } else if (interval >= 1.0 && interval <= 5.0 ) {
        interval = (int)  interval;
    } else if (interval > 5  && interval <= 10 ) {
        interval = 10;
    } else if (interval > 10 && interval < 50) {
        interval = 10 * (1 + (int)(interval / 10));
    } else if (interval >= 50 && interval <= 100) {
        interval = 100;
    } else if (interval > 100 && interval <= 200) {
        interval = 200;
    } else if (interval > 200) {
        interval = 100 * (1 + (int)(interval / 100));
    }
    markQty = (int)((m_end - m_start) / interval);

    QFont font = painter.font();
    font.setPointSize(10);
    painter.setFont(font);

    if (RulerWidget::LEFT == m_type) {
        painter.rotate(90);
        painter.translate(0, -20);
    } else if (RulerWidget::RIGHT == m_type) {
        QTransform form = painter.transform();
        form.rotate(90);
        form.rotate(180, Qt::XAxis);
        painter.setTransform(form);
    }

    QLine line[markQty];

    painter.drawText(length/2, 19, m_unitName);

    if (RulerWidget::Down == m_direction) {

        for(int i = 0; i < markQty; ++i) {
            line[i].setLine( length - (int)(i * interval * m_pixelPerUnit), 0, length - (int)(i * interval * m_pixelPerUnit), 3);
        }

        for(int i = 5; i < markQty; i += 10) {
            line[i].setLine( length - (int)(i * interval * m_pixelPerUnit), 0, length - (int)(i * interval * m_pixelPerUnit), 7);
        }

        for(int i = 0; i < markQty; i += 10) {
            line[i].setLine( length - (int)(i * interval * m_pixelPerUnit), 0, length - (int)(i * interval * m_pixelPerUnit), 13);
            painter.drawText( length - (int)(i * interval * m_pixelPerUnit) - 15, 12, QString::number(i*interval+m_start));
        }
        painter.drawLines(line, markQty+1);
    } else {
        for(int i = 0; i < markQty; ++i) {
            line[i].setLine((int)(i * interval * m_pixelPerUnit), 0, (int)(i * interval * m_pixelPerUnit), 3);
        }

        for(int i = 5; i < markQty; i += 10) {
            line[i].setLine((int)(i * interval * m_pixelPerUnit), 0, (int)(i * interval * m_pixelPerUnit), 7);
        }

        for(int i = 0; i < markQty; i += 10) {
            line[i].setLine((int)(i * interval * m_pixelPerUnit), 0, (int)(i * interval * m_pixelPerUnit), 13);
            painter.drawText((int)(i * interval * m_pixelPerUnit)+2, 12, QString::number(i*interval+m_start));
        }
        painter.drawLines(line, markQty+1);
    }
}
