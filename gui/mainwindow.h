#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    ~MainWindow();

    void savePuzzle();
    void loadPuzzle(QString fileName = QString(""));
    void solve();
    void showSolutions();
    void showPuzzle();

private:
    Ui::MainWindow *ui;
    enum {
        PUZZLE,
        SOLUTION
    } _state = PUZZLE;
    int _solution_no = 0;
    std::vector<std::vector<int> > _solutions;
};

#endif // MAINWINDOW_H
