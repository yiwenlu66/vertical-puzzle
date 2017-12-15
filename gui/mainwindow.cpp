#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QStandardPaths>
#include <QMessageBox>
#include <QTextStream>
#include <QFontDatabase>
#include <sstream>
#include "cli.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->solutionBar->setVisible(false);
    ui->maxSolutionsLineEdit->setValidator(new QIntValidator(1, 99, this));
    ui->consoleOutputTextEdit->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
    connect(ui->clearButton, &QPushButton::clicked, ui->inputGrid, &InputGrid::clear);
    connect(ui->actionSave_puzzle, &QAction::triggered, this, &MainWindow::savePuzzle);
    connect(ui->actionLoad_puzzle, &QAction::triggered, this, &MainWindow::loadPuzzle);
    connect(ui->solveButton, &QPushButton::clicked, this, &MainWindow::solve);
    connect(ui->solutionPrevButton, &QPushButton::clicked, this, [&](){ --_solution_no; showSolutions(); });
    connect(ui->solutionNextButton, &QPushButton::clicked, this, [&](){ ++_solution_no; showSolutions(); });
    connect(ui->solutionBackButton, &QPushButton::clicked, this, &MainWindow::showPuzzle);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::savePuzzle()
{
    showPuzzle();
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Puzzle"), QStandardPaths::writableLocation(QStandardPaths::DownloadLocation));
    if (fileName.isEmpty()) {
        return;
    }
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::information(this, tr("Unable to open file for writing"),
            file.errorString());
        return;
    }
    QTextStream str(&file);
    str << ui->inputGrid->dump();
    file.close();
}

void MainWindow::loadPuzzle()
{
    showPuzzle();
    QString fileName = QFileDialog::getOpenFileName(this, tr("Load Puzzle"), QStandardPaths::writableLocation(QStandardPaths::DownloadLocation));
    if (fileName.isEmpty()) {
        return;
    }
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::information(this, tr("Unable to open file for reading"),
                file.errorString());
        return;
    }
    QTextStream stream(&file);
    auto state = ui->inputGrid->load(stream.readAll());
    if (!state.success) {
        QMessageBox::information(this, tr("Failed to read file"), state.msg);
    }
}

void MainWindow::solve()
{
    std::istringstream iss(ui->inputGrid->dump().toStdString());
    std::istream istream(iss.rdbuf());
    std::ostringstream ostream;
    ui->consoleOutputTextEdit->clear();
    _solutions = vertical_puzzle_cli(ostream, istream, ui->maxSolutionsLineEdit->text().toInt(), ui->verboseCheckBox->isChecked());
    ui->consoleOutputTextEdit->append(QString::fromStdString(ostream.str()));
    if (!_solutions.empty()) {
        showSolutions();
    }
}

void MainWindow::showSolutions()
{
    _state = SOLUTION;
    ui->clearButton->setVisible(false);
    ui->solveButton->setEnabled(false);
    ui->solutionBar->setVisible(true);
    ui->solutionPrevButton->setEnabled(_solution_no);
    ui->solutionNextButton->setEnabled(_solution_no != _solutions.size() - 1);
    ui->solutionLabel->setText(tr("Solution %1 of %2").arg(QString::number(_solution_no + 1), QString::number(_solutions.size())));
    ui->inputGrid->showSolution(_solutions[_solution_no]);
}

void MainWindow::showPuzzle()
{
    _state = PUZZLE;
    ui->clearButton->setVisible(true);
    ui->solveButton->setEnabled(true);
    ui->solutionBar->setVisible(false);
    _solution_no = 0;
    ui->inputGrid->showPuzzle();
}
