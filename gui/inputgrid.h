#ifndef INPUTGRID_H
#define INPUTGRID_H

#include <QTableWidget>
#include <QTableWidgetItem>

struct InputGridLoaderState {
    bool success;
    QString msg;
};

class InputGrid : public QTableWidget
{
    Q_OBJECT
public:
    static const int N_ROWS_DEFAULT = 15, N_COLS_DEFAULT = 15;
    InputGrid(int n_rows, int n_cols, QWidget *parent = __null);
    explicit InputGrid(QWidget *parent = __null);
    QSize sizeHint() const override { return QSize(500, 500); }
    int heightForWidth(int w) const override { return w; }
    void resizeEvent(QResizeEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void mousePressEvent(QMouseEvent *ev) override;
    QString dump();
    InputGridLoaderState load(QString text);
    void showPuzzle();
    void showSolution(std::vector<int> solution);
public slots:
    void itemChangedListener(QTableWidgetItem *item);
    void clear();
private:
    QTableWidgetItem *_proto_item;
    enum {
        PUZZLE,
        SOLUTION
    } _state = PUZZLE;
    QString _dumped_puzzle;
};

#endif // INPUTGRID_H
