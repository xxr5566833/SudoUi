#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <string>

// #define Btn(x,y) pushButton_##x_##y

const QString rstrColor("background-color: rgb(204, 204, 204);\n");
const QString none_rstrColor("background-color: white;\n");

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    coreDLL = LoadLibrary(TEXT("Core/SoduCore.dll"));
    generate_m = (GENERATE_M)GetProcAddress(coreDLL, "generate_m");
    generate_r = (GENERATE_R)GetProcAddress(coreDLL, "generate_r");
    solve_s = (SOLVE_S)GetProcAddress(coreDLL, "solve_s");

    ui->setupUi(this);
    // QPushButton *board[9][9];
    for(int i = 0; i < 9; i++)
        for(int j = 0; j < 9; j++){
            char s[15];
            sprintf(s, "pushButton_%d_%d", i + 1, j + 1);
            board[i][j] = ui->centralWidget->findChild<QPushButton*>(s);
        }
}

MainWindow::~MainWindow()
{
    FreeLibrary(coreDLL);
    delete ui;

}

