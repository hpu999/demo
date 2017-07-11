/**
 * @file a_scan_widget.h
 * @brief 画A扫
 * @author Jake Yang <yanghuanjie@cndoppler.cn>
 * @date 2017-06-28
 */
#ifndef __A_SCAN_WIDGET_H__
#define __A_SCAN_WIDGET_H__

#include <QWidget>

class AscanWidget : public QWidget
{
    Q_OBJECT
public:
    explicit AscanWidget(QWidget *parent = 0);

    /**
     * @brief wave_color    获取波型显示颜色
     * @return              颜色
     */
    const QColor &wave_color() const;

    /**
     * @brief set_wave_color    设置波形显示颜色
     * @param color             颜色
     */
    void set_wave_color(const QColor &color);

    /**
     * @brief show  显示波形
     * @param b     波形数据
     */
    void show(const QByteArray &b);

protected:
    /**
     * @brief x_axis_length     获取X轴长度
     * @return                  长度(Pixel)
     */
    virtual int x_axis_length() const = 0;

    /**
     * @brief y_axis_length     获取Y轴长度
     * @return                  长度(Pixel)
     */
    virtual int y_axis_length() const = 0;

    /**
     * @brief paint_wave    画波形
     * @return              路径
     */
    QPainterPath paint_wave();

    /**
     * @brief paint_gate    画闸门
     * @return              路径
     */
//    QPainterPath paint_gate();

signals:

public slots:

private:
    QByteArray m_beam;
    QColor m_color;
};

inline const QColor &AscanWidget::wave_color() const
{
    return m_color;
}

inline void AscanWidget::set_wave_color(const QColor &color)
{
    m_color = color;
}

#endif // __A_SCAN_WIDGET_H__
