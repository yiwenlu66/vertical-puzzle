#ifndef TILE_H
#define TILE_H

#include <QLabel>
#include <QString>

class Tile : public QLabel
{
    Q_OBJECT
public:
    explicit Tile(QWidget *parent = __null);
    int heightForWidth(int w) const override { return w; }
    QSize sizeHint() const override { return QSize(25, 25); }
    void setText(const QString &);
    void mousePressEvent(QMouseEvent *ev) override;
};

#endif // TILE_H
