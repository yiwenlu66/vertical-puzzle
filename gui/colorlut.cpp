#include "colorlut.h"

ColorLUT::ColorLUT()
{

}

QColor ColorLUT::getColor(QString s)
{
    if (!s.length()) {
        return Qt::white;
    }
    // https://en.wikipedia.org/wiki/Template:SHM_color [LoL]
    switch (s.at(0).toLatin1()) {
    case 'A':
        return QColor(0xE4, 0x00, 0x2B, 100);
    case 'B':
        return QColor(0x97, 0xD7, 0x00, 100);
    case 'C':
        return QColor(0xFF, 0xD1, 0x00, 100);
    case 'D':
        return QColor(0x5F, 0x25, 0x9F, 100);
    case 'E':
        return QColor(0xAC, 0x4F, 0xC6, 100);
    case 'F':
        return QColor(0xD7, 0x16, 0x71, 100);
    case 'G':
        return QColor(0xFF, 0x69, 0x00, 100);
    case 'H':
        return QColor(0x00, 0x9E, 0xDB, 100);
    case 'I':
        return QColor(0x71, 0xC5, 0xE8, 100);
    case 'J':
        return QColor(0xC1, 0xA7, 0xE2, 100);
    case '_':
        return QColor(0x66, 0x66, 0x66, 100);
    case '+':
    case '-':
    case '*':
        return QColor(0x76, 0x23, 0x2F, 100);
    default:
        return QColor(0x2C, 0xD5, 0xC4, 100);
    }
}
