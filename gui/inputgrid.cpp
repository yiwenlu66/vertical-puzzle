#include "inputgrid.h"
#include "inputitemdelegate.h"
#include "colorlut.h"
#include <cmath>
#include <QResizeEvent>
#include <QHeaderView>
#include <QMimeData>
#include <QTableWidgetItem>
#include <QDrag>
#include <QTextStream>

InputGrid::InputGrid(int n_rows, int n_cols, QWidget *parent)
    : QTableWidget(n_rows, n_cols, parent), _proto_item(new QTableWidgetItem())
{
    auto sizePolicy = this->sizePolicy();
    sizePolicy.setHeightForWidth(true);
    this->setSizePolicy(sizePolicy);
    this->setGridStyle(Qt::DashLine);
    this->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    this->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    this->setSelectionMode(QAbstractItemView::NoSelection);
    _proto_item->setTextAlignment(Qt::AlignCenter);
    this->setItemPrototype(_proto_item);
    for (auto i = 0; i < n_rows; ++i) {
        for (auto j = 0; j < n_cols; ++j) {
            this->setItem(i, j, new QTableWidgetItem(*_proto_item));
        }
    }
    this->setItemDelegate(new InputItemDelegate(this));
    connect(this, &QTableWidget::itemChanged, this, &InputGrid::itemChangedListener);
    this->setDragEnabled(true);
    this->setAcceptDrops(true);
}

InputGrid::InputGrid(QWidget *parent)
    : InputGrid(N_ROWS_DEFAULT, N_COLS_DEFAULT, parent)
{
}

void InputGrid::itemChangedListener(QTableWidgetItem *item)
{
    if (_state == SOLUTION) {
        return;
    }
    item->setText(item->text().toUpper());
    item->setBackgroundColor(ColorLUT::getColor(item->text()));
}

void InputGrid::resizeEvent(QResizeEvent *event)
{
    auto size = event->size();
    auto min_side = std::min(size.width() - this->verticalHeader()->size().width(), size.height() - this->horizontalHeader()->size().height());
    auto tile_side = static_cast<int>(std::round(1.0 * min_side / (std::max(this->rowCount(), this->columnCount()))));
    for (auto i = 0; i < this->rowCount(); ++i) {
        this->setRowHeight(i, tile_side);
    }
    for (auto j = 0; j < this->columnCount(); ++j) {
        this->setColumnWidth(j, tile_side);
    }
}

void InputGrid::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat("application/x-dnditemdata")) {
        if (event->source() == this) {
            event->setDropAction(Qt::MoveAction);
            event->accept();
        } else {
            event->acceptProposedAction();
        }
    } else {
        event->ignore();
    }
}

void InputGrid::dragMoveEvent(QDragMoveEvent *event)
{
    if (event->mimeData()->hasFormat("application/x-dnditemdata")) {
        if (event->source() == this) {
            event->setDropAction(Qt::MoveAction);
            event->accept();
        } else {
            event->acceptProposedAction();
        }
    } else {
        event->ignore();
    }
}

void InputGrid::dropEvent(QDropEvent *event)
{
    if (event->mimeData()->hasFormat("application/x-dnditemdata")) {
        QByteArray itemData = event->mimeData()->data("application/x-dnditemdata");
        QDataStream dataStream(&itemData, QIODevice::ReadOnly);

        QString text;
        dataStream >> text;

        if (!text.length()) {
            event->ignore();
            return;
        }

        auto dstItem = this->itemAt(event->pos());

        if (event->source() == this) {
            QPoint srcPos;
            dataStream >> srcPos;
            auto srcItem = this->itemAt(srcPos);
            if (srcItem == dstItem) {
                event->ignore();
                return;
            }
        }

        if (text.at(0).toLatin1() == '_' && event->source() != this) {
            // fill entire row
            auto row = this->rowAt(event->pos().y());
            for (auto col = 0; col < this->columnCount(); ++col) {
                this->item(row, col)->setText(text);
            }
        } else {
            dstItem->setText(text);
        }

        event->accept();

    } else {
        event->ignore();
    }
}

void InputGrid::clear()
{
    for (auto i = 0; i < this->rowCount(); ++i) {
        for (auto j = 0; j < this->columnCount(); ++j) {
            this->item(i, j)->setText(QString(""));
        }
    }
}

void InputGrid::mousePressEvent(QMouseEvent *ev)
{
    auto item = this->itemAt(ev->pos());
    QByteArray itemData;
    QDataStream dataStream(&itemData, QIODevice::WriteOnly);
    dataStream << item->text() << ev->pos();
    auto mimeData = new QMimeData;
    mimeData->setData("application/x-dnditemdata", itemData);
    auto drag = new QDrag(this);
    drag->setMimeData(mimeData);
    QPixmap pixmap(20, 20);
    pixmap.fill(ColorLUT::getColor(item->text()));
    drag->setPixmap(pixmap);
    auto row = this->rowAt(ev->y()), col = this->columnAt(ev->x());
    auto x_ref = this->columnViewportPosition(col), y_ref = this->rowViewportPosition(row);
    drag->setHotSpot(ev->pos() - QPoint(x_ref, y_ref));
    if (drag->exec(Qt::MoveAction) == Qt::MoveAction) {
        item->setText(QString(""));
    }
    QTableWidget::mousePressEvent(ev);
}

QString InputGrid::dump()
{
    QString string;
    QTextStream stream(&string, QIODevice::WriteOnly);
    for (auto i = 0; i < this->rowCount(); ++i) {
        for (auto j = 0; j < this->columnCount(); ++j) {
            if (this->item(i, j)->text().length()) {
                stream << this->item(i, j)->text();
            } else {
                stream << ' ';
            }
        }
        stream << '\n';
    }
    string.remove(QRegExp("\\s+$"));	// remove trailing blanks
    return string;
}

InputGridLoaderState InputGrid::load(QString text)
{
    this->clear();
    InputGridLoaderState result;
    QTextStream stream(&text, QIODevice::ReadOnly);
    int row = 0, col = 0;
    char c;
    while (!stream.atEnd()) {
        stream >> c;
        if (c == '\n') {
            ++row;
            col = 0;
        } else if (c == ' ') {
            ++col;
        } else {
            if (('A' <= c && c <= 'J') || ('a' <= c && c <= 'j') || ('0' <= c && c <= '9') || c == '+' || c == '-' || c == '*' || c == '_') {
                if (row >= this->rowCount() || col >= this->columnCount()) {
                    result.success = false;
                    result.msg = tr("The puzzle is too large to fit in GUI, please use CLI version instead. (Non-blank character at row %1, column %2)").arg(
                                QString::number(row + 1), QString::number(col + 1));
                    this->clear();
                    return result;
                }
                this->item(row, col++)->setText(QString(QChar(c)).toUpper());
            } else {
                result.success = false;
                result.msg = tr("Illegal character at row %1, column %2!").arg(QString::number(row + 1), QString::number(col + 1));
                this->clear();
                return result;
            }
        }
    }
    result.success = true;
    return result;
}

void InputGrid::showPuzzle()
{
    _state = PUZZLE;
    this->setDragEnabled(true);
    this->setAcceptDrops(true);
    for (auto i = 0; i < this->rowCount(); ++i) {
        for (auto j = 0; j < this->columnCount(); ++j) {
            auto item = this->item(i, j);
            item->setFlags(item->flags() | Qt::ItemIsEditable);
        }
    }
    this->load(_dumped_puzzle);
}

void InputGrid::showSolution(std::vector<int> solution)
{
    this->setDragEnabled(false);
    this->setAcceptDrops(false);
    solution;
    if (_state == PUZZLE) {
        _dumped_puzzle = this->dump();
        _state = SOLUTION;
    } else {
        this->load(_dumped_puzzle);
    }
    for (auto i = 0; i < this->rowCount(); ++i) {
        for (auto j = 0; j < this->columnCount(); ++j) {
            auto item = this->item(i, j);
            item->setFlags(item->flags() ^ Qt::ItemIsEditable);
            if (item->text().length()) {
                char c = item->text().at(0).toLatin1();
                if ('A' <= c && c <= 'J') {
                    item->setText(QString::number(solution[c - 'A']));
                }
            }
        }
    }
}
