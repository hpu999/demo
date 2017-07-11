#ifndef A_SCAN_H
#define A_SCAN_H
#include <QWidget>
#include "a_scan_widget.h"
class A_scan:public AscanWidget
{
public:
    A_scan(QWidget *parent = 0);
    /**
     * @brief x_axis_length     鑾峰彇X杞撮暱搴     * @return                  闀垮害(Pixel)
     */
    virtual int x_axis_length() const{};

    /**
     * @brief y_axis_length     鑾峰彇Y杞撮暱搴     * @return                  闀垮害(Pixel)
     */
    virtual int y_axis_length() const {};
};

#endif // A_SCAN_H
