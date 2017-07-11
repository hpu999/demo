/**
 * @file a_scan_hwidget.h
 * @brief A扫横向显示
 * @author Jake Yang <yanghuanjie@cndoppler.cn>
 * @date 2017-06-28
 */
#ifndef __A_SCAN_HWIDGET_H__
#define __A_SCAN_HWIDGET_H__

#include "a_scan_widget.h"
#include "mythread.h"
#include <QTimer>

class AscanHWidget : public AscanWidget
{
    Q_OBJECT
public:
    AscanHWidget(QWidget *parent = 0);
    ~AscanHWidget();
protected:
    /**
     * @brief x_axis_length     获取X轴长度
     * @return                  长度(Pixel)
     */
    virtual int x_axis_length() const;

    /**
     * @brief y_axis_length     获取Y轴长度
     * @return                  长度(Pixel)
     */
    virtual int y_axis_length() const;

    virtual void paintEvent(QPaintEvent *e);

public slots:
    void recive_data(QByteArray waveData);

private:
    MyThread *m_draw_thread;
};

#endif // __A_SCAN_HWIDGET_H__
