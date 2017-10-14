#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "QMainWindow"
#include "ui_mainwindow.h"
#include "windows.h"
#include "QMessageBox"
#include "main.h"
#include <QDialog>
#include <QGridLayout>
extern GENERATE_M generate_m;
extern GENERATE_R generate_r;
extern SOLVE_S solve_s;
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
    QPushButton *board_[81];
    QPushButton *bottom_[9];
    QPushButton *submit_;
    QPushButton *erase_;
    QPushButton *pause_;
    QPushButton *restart_;
    QPushButton *help_;
    QMessageBox *new_game_dialog_;
    QMessageBox *restart_dialog_;
    QTimer *timer_;
    bool ispause_;
    bool timerEnable_;
    bool **presentBoard;
    int *originBoard;
    int *tempBoard_;
    int focus;
    int costTime;
    int presentGameMode;

    void initBoard(int mode, bool uniue);
    void setBoard();

private slots:
    void restart();
    void pause();
    void newGame();
    void boardClicked();
    void bottomClicked();
    void remindMe();
    void erase();
    void timerUpdate();
    void finish();
    void showRecord();
    void help();
};

#endif // MAINWINDOW_H
