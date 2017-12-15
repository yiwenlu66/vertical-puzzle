#include "tile.h"
#include "colorlut.h"
#include <QPalette>
#include <QDebug>
#include <QMimeData>
#include <QDrag>
#include <QMouseEvent>
#include <QPixmap>

Tile::Tile(QWidget *parent)
    : QLabel(parent)
{
    auto sizePolicy = this->sizePolicy();
    sizePolicy.setHeightForWidth(true);
    sizePolicy.setHorizontalPolicy(QSizePolicy::Fixed);
    sizePolicy.setVerticalPolicy(QSizePolicy::Fixed);
    this->setSizePolicy(sizePolicy);
    this->setAlignment(Qt::AlignCenter);
    this->setAutoFillBackground(true);
}

void Tile::setText(const QString &text)
{
    QLabel::setText(text);
    auto palette = this->palette();
    palette.setColor(this->backgroundRole(), ColorLUT::getColor(text));
    this->setPalette(palette);
}

void Tile::mousePressEvent(QMouseEvent *ev)
{
    QByteArray itemData;
    QDataStream dataStream(&itemData, QIODevice::WriteOnly);
    dataStream << QString(this->text().at(0));
    auto mimeData = new QMimeData;
    mimeData->setData("application/x-dnditemdata", itemData);
    auto drag = new QDrag(this);
    drag->setMimeData(mimeData);
    QPixmap pixmap(20, 20);
    pixmap.fill(ColorLUT::getColor(this->text()));
    drag->setPixmap(pixmap);
    drag->setHotSpot(ev->pos());
    drag->exec(Qt::CopyAction);
}
