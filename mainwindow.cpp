#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <string>
#include <regex>
#include <iostream>
#include <QTimer>
#include <QFont>

// #define Btn(x,y) pushButton_##x_##y

const std::string rstrColor("rgb(210, 210, 210)");
const std::string none_rstrColor("white");
const std::string focusColor("rgb(150, 150, 150)");
const std::string relatedColor("rgb(200, 200, 200)");
const std::regex bg("(background-color:).+?;\\n");

const char *multiNumTemplete = "_ _ _\n_ _ _\n_ _ _";
const int singleNumFontSize = 18;
const int multiNumFontSize = 7;
const QFont singleNumFont("Comic Sans MS", 18, QFont::Normal);
const QFont multiNumFont("Comic sans MS", 7, QFont::Normal);


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    // 从DLL中导入三个函数
    coreDLL = LoadLibrary(TEXT("Core/SoduCore.dll"));
    generate_m = (GENERATE_M)GetProcAddress(coreDLL, "generate_m");
    generate_r = (GENERATE_R)GetProcAddress(coreDLL, "generate_r");
    solve_s = (SOLVE_S)GetProcAddress(coreDLL, "solve_s");
    // 为类变量申请空间
    originBoard = new int[81];
    presentBoard = new bool*[81];
    for(int i = 0; i < 81; i++){
        presentBoard[i] = new bool[9];
        for(int j = 0; j < 9; j++)
            presentBoard[i][j] = false;
    }
    // 不知道，一开始就有这一行
    ui->setupUi(this);
    // 将81个pushButton存到board数组中
    for(int x = 0; x < 81; x++){
            int j = x % 9;
            int i = x / 9;
            char s[15];
            sprintf(s, "pushButton_%d_%d", i + 1, j + 1);
            board[x] = ui->centralWidget->findChild<QPushButton*>(s);
            board[x]->setStyleSheet(board[x]->styleSheet() + "background-color:white;\n");
        }
    // 将9个tipButton存到bottom数组中
    for(int x = 0; x < 9; x++){
        char s[12];
        sprintf(s, "tipButton_%d", x + 1);
        bottom[x] = ui->centralWidget->findChild<QPushButton*>(s);

    }
    // 开始时将简单模式唯一解的题目摆在棋盘上，将题目存到origin和present里面
    initBoard(1, true);

    // 开始计时
    timer = new QTimer(this);
    timer->setInterval(1000);
    timer->start();

    // connect 连接
    connect(timer, SIGNAL(timeout()), this, SLOT(timerUpdate()));
    connect(ui->restart, SIGNAL(clicked()), this, SLOT(restart()));
    connect(ui->pause, SIGNAL(clicked()), this, SLOT(pause()));
    connect(ui->newGame, SIGNAL(clicked()), this, SLOT(newGame()));
    for(int i = 0; i < 81; i++)
        connect(board[i], SIGNAL(clicked()), this, SLOT(boardClicked()));
    for(int i = 0; i < 9; i++)
        connect(bottom[i], SIGNAL(clicked()), this, SLOT(bottomClicked()));
    connect(ui->remindMe, SIGNAL(clicked()), this, SLOT(remindMe()));
    connect(ui->erase, SIGNAL(clicked()), this, SLOT(erase()));
}

MainWindow::~MainWindow()
{
    FreeLibrary(coreDLL);
    delete ui;

}

void MainWindow::initBoard(int mode, bool unique){
    focus = -1;
    costTime = 0;
    if(unique){
        int difficultyDivide[4] = {20, 32, 44, 56};
        generate_r(1, difficultyDivide[mode - 1], difficultyDivide[mode] - 1, true, &originBoard);
    }
    else{
        generate_m(1, mode, &originBoard);
    }

    for(int i = 0; i < 81; ++i){
        QPushButton *pb = board[i];
        pb->setFont(singleNumFont);
        std::string styleSheet = pb->styleSheet().toStdString();
        if(originBoard[i]){
            pb->setText(QString(originBoard[i] + '0'));
            styleSheet = std::regex_replace(styleSheet, bg, "$1" + rstrColor + ";\n");
        }else{
            pb->setText(QString(""));
            styleSheet = std::regex_replace(styleSheet, bg, "$1" + none_rstrColor+ ";\n");
        }
        pb->setStyleSheet(styleSheet.c_str());
    }
}

void MainWindow::restart(){}

void MainWindow::pause(){}

void MainWindow::newGame(){}

void MainWindow::boardClicked(){}

void MainWindow::bottomClicked(){}

void MainWindow::remindMe(){}

void MainWindow::erase(){}

void MainWindow::timerUpdate(){
    ++costTime;
    char s[3];
    sprintf(s, "%02d", costTime % 60);
    ui->timer_sec->setText(s);
    sprintf(s, "%02d", costTime / 60);
    ui->timer_min->setText(s);
}
