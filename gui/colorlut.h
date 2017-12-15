#ifndef COLORLUT_H
#define COLORLUT_H

#include <QColor>

class ColorLUT
{
public:
    ColorLUT();
    static QColor getColor(QString);
};

#endif // COLORLUT_H
