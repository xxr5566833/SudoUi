#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "ui_mainwindow.h"
#include <windows.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();



private:
    Ui::MainWindow *ui;
    QPushButton *board[9][9];

    typedef void(*GENERATE_M) (int, int, int**);
    typedef void(*GENERATE_R) (int, int, int, bool, int**);
    typedef bool(*SOLVE_S) (int *, int *);
    HMODULE coreDLL;
    GENERATE_M generate_m = NULL;
    GENERATE_R generate_r = NULL;
    SOLVE_S solve_s = NULL;
};

#endif // MAINWINDOW_H
