#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void savePuzzle();
    void loadPuzzle();
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
