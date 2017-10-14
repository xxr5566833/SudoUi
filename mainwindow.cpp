#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "string"
#include "regex"
#include "iostream"
#include "fstream"
#include "QTimer"
#include "QFont"
#include "QComboBox"
#include "QMessageBox"
#include "QDebug"
#include "fstream"
// #define Btn(x,y) pushButton_##x_##y

const std::string rstrColor("rgb(210, 210, 210)");
const std::string none_rstrColor("white");
const std::string focusColor("rgb(0, 200, 0)");
const std::string relatedColor("rgb(255, 255, 0)");
const std::string errorColor("rgb(255,0,0)");
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
    // 将81个pushButton存到board数组中,9个pushButton存到bottom数组中，初始化对应的背景等
    for(int x = 0; x < 81; x++){
            int j = x % 9;
            int i = x / 9;
            char s[15];
            sprintf(s, "pushButton_%d_%d", i + 1, j + 1);
            board_[x] = ui->centralWidget->findChild<QPushButton*>(s);
            board_[x]->setStyleSheet(board_[x]->styleSheet() + "background-color:white;\n");
            board_[x]->setEnabled(false);
        }
    // 将9个tipButton存到bottom数组中
    for(int x = 0; x < 9; x++){
        char s[12];
        sprintf(s, "tipButton_%d", x + 1);
        bottom_[x] = ui->centralWidget->findChild<QPushButton*>(s);
        bottom_[x]->setEnabled(false);
    }
    //初始化erase和submit按钮
    submit_=ui->centralWidget->findChild<QPushButton*>("submit");
    submit_->setEnabled(false);
    erase_=ui->centralWidget->findChild<QPushButton*>("erase");
    erase_->setEnabled(false);
    pause_=ui->centralWidget->findChild<QPushButton*>("pause");
    pause_->setEnabled(false);
    ispause_=false;
    restart_=ui->centralWidget->findChild<QPushButton*>("restart");
    restart_->setEnabled(false);
    help_=ui->centralWidget->findChild<QPushButton*>("helpButton");

    //初始化 remindMe 按钮
    ui->remindMe->setEnabled(false);

    // 开始时将简单模式唯一解的题目摆在棋盘上，将题目存到origin和present里面
    /*initBoard(1, true);*/

    // 初始化时钟
    timer_ = new QTimer(this);
    timer_->setInterval(1000);
    //timer->start();

    //初始化 connect 连接
    connect(timer_, SIGNAL(timeout()), this, SLOT(timerUpdate()));
    connect(ui->restart, SIGNAL(clicked()), this, SLOT(restart()));
    connect(ui->pause, SIGNAL(clicked()), this, SLOT(pause()));
    connect(ui->newGame, SIGNAL(clicked()), this, SLOT(newGame()));
    for(int i = 0; i < 81; i++)
        connect(board_[i], SIGNAL(clicked()), this, SLOT(boardClicked()));
    for(int i = 0; i < 9; i++)
        connect(bottom_[i], SIGNAL(clicked()), this, SLOT(bottomClicked()));
    connect(ui->remindMe, SIGNAL(clicked()), this, SLOT(remindMe()));
    connect(ui->erase, SIGNAL(clicked()), this, SLOT(erase()));
    connect(ui->submit,SIGNAL(clicked()),this,SLOT(finish()));
    connect(ui->record, SIGNAL(clicked()), this, SLOT(showRecord()));
    connect(ui->helpButton,SIGNAL(clicked()),this,SLOT(help()));

    // 如果没有record文件，则创建它
    std::ifstream fin("record");
    if(!fin){
        fin.close();
        std::ofstream fout("record");
        for(int i = 0; i < 6; i++)
            fout << -1 << std::endl;
        fout.close();
    }
    else
        fin.close();
    //弹出操作窗口
}

MainWindow::~MainWindow()
{
    //FreeLibrary(coreDLL);
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
        QPushButton *pb = board_[i];
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

void MainWindow::restart(){
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(new MainWindow(), tr("重新开始？"),
                                        tr("您确定要重新开始这局游戏？"),
                                        QMessageBox::Ok | QMessageBox::Cancel );
    switch(reply){
    case QMessageBox::Ok:
    {
        for(int i = 0; i < 81; ++i){
            QPushButton *pb = board_[i];
            pb->setEnabled(true);
            std::string styleSheet = pb->styleSheet().toStdString();
            if(originBoard[i]){
                styleSheet = std::regex_replace(styleSheet, bg, "$1" + rstrColor + ";\n");
            }else{
                pb->setText(QString(""));
                styleSheet = std::regex_replace(styleSheet, bg, "$1" + none_rstrColor+ ";\n");
            }
            pb->setStyleSheet(styleSheet.c_str());
        }
        for(int i=0;i<9;i++)
            bottom_[i]->setEnabled(false);
        erase_->setEnabled(false);
        timerEnable_=true;
        costTime=-1;
    }
        break;
    default:
        break;
    }
}

void MainWindow::pause(){
    if(!ispause_)
    {
        timerEnable_=false;
        tempBoard_=new int[81];
        for(int i=0;i<81;++i)
        {
            QString s=board_[i]->text();
            if(s=="")
                tempBoard_[i] = 0;
            else{
                tempBoard_[i] =  s.toStdString()[0]-'0';
            }
        }
        for(int i = 0; i < 81; ++i){
            QPushButton *pb = board_[i];
            pb->setEnabled(false);
            std::string styleSheet = pb->styleSheet().toStdString();
            pb->setText(QString(""));
            styleSheet = std::regex_replace(styleSheet, bg, "$1" + none_rstrColor+ ";\n");
            pb->setStyleSheet(styleSheet.c_str());
        }
        for(int i=0;i<9;++i)
        {
            bottom_[i]->setEnabled(false);
        }
        erase_->setEnabled(false);
        submit_->setEnabled(false);
        pause_->setText("continue");
        ispause_=true;
    }
    else{
        timerEnable_=true;
        for(int i = 0; i < 81; ++i){
            QPushButton *pb = board_[i];
            pb->setEnabled(true);
            std::string styleSheet = pb->styleSheet().toStdString();
            if(tempBoard_[i])
            {
                pb->setText(QString(tempBoard_[i]+'0'));
            }
            else
                pb->setText("");
            if(originBoard[i]){
                styleSheet = std::regex_replace(styleSheet, bg, "$1" + rstrColor + ";\n");
            }else{
                styleSheet = std::regex_replace(styleSheet, bg, "$1" + none_rstrColor+ ";\n");
            }
            pb->setStyleSheet(styleSheet.c_str());
        }
        pause_->setText("pause");
        submit_->setEnabled(true);
        ispause_=false;
        delete tempBoard_;
    }
}

void MainWindow::newGame(){
    QString mode=ui->centralWidget->findChild<QComboBox*>("modeChooseBox")->currentText();
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(new MainWindow(), tr("开始一局新游戏？"),
                                        "您选择了  "+mode+" 模式,确定要开始一局新的游戏？",
                                        QMessageBox::Ok | QMessageBox::Cancel );
    switch(reply)
    {
    case QMessageBox::Ok:
    {
        //1.根据combobox的结果获得用户所选模式
        QComboBox *select=new QComboBox();
        char *s="modeChooseBox";
        select=ui->centralWidget->findChild<QComboBox*>(s);
        int currentindex=select->currentIndex();

        presentGameMode = currentindex;

        //2.根据模式调用对应的函数产生数组
        if(currentindex<3){
            int difficultyDivide[4] = {20, 32, 44, 56};
            generate_r(1, difficultyDivide[currentindex ], difficultyDivide[currentindex+1] - 1, true, &originBoard);
        }
        else{
            generate_m(1, (currentindex%3)+1, &originBoard);
        }
        //3.根据数组元素初始化棋盘，区分不同的颜色
        for(int i = 0; i < 81; ++i){
            QPushButton *pb = board_[i];
            pb->setEnabled(true);
            pb->setFont(singleNumFont);
            std::string styleSheet = pb->styleSheet().toStdString();
            if(originBoard[i]){
                pb->setText(QString(originBoard[i] +'0'));
                styleSheet = std::regex_replace(styleSheet, bg, "$1" + rstrColor + ";\n");
            }else{
                pb->setText(QString(""));
                styleSheet = std::regex_replace(styleSheet, bg, "$1" + none_rstrColor+ ";\n");
            }
            pb->setStyleSheet(styleSheet.c_str());
        }
        //4.遍历整个bottombutton和erase按钮，先把它们都设置为false，pause也设置为enable
        for(int i=0;i<9;i++)
            bottom_[i]->setEnabled(false);
        erase_->setEnabled(false);
        pause_->setEnabled(true);
        restart_->setEnabled(true);
        submit_->setEnabled(true);
        //5.时钟开始运行,timer对应的label清零
        timer_->start();
        timerEnable_=true;
        costTime=0;
        ui->timer_sec->setText("00");
        ui->timer_min->setText("00");
    }
        break;
    default:
        break;
    }
    /*if (reply == QMessageBox::Abort)
            criticalLabel->setText(tr("Abort"));
    else if (reply == QMessageBox::Retry)
            criticalLabel->setText(tr("Retry"));*/

}

void MainWindow::boardClicked(){
    //一开始先回复为一开始的样式
    setBoard();
    //获得信号发出者，获得要突出显示的button并让他们突出显示,但是对于空白框点了后仅仅设置focus
    QPushButton *pb = qobject_cast<QPushButton*>(sender());
    std::string s = pb->objectName().toStdString();
    //设置好当前的focus
    QString num=pb->text();
    focus = (s[11]-'0'-1)*9 + (s[13]-'0'-1);
    if(num=="")
    {
        for(int i=0;i<9;i++)
            bottom_[i]->setEnabled(true);
        erase_->setEnabled(true);
        ui->remindMe->setEnabled(true);
    }
    else {
        ui->remindMe->setEnabled(false);
        //设置相同数字突出
        for(int i=0;i<81;i++)
        {
            QPushButton *temp=board_[i];
            if(temp->text()==num){
                //设置突出样式
                std::string stylesheet = temp->styleSheet().toStdString();
                stylesheet = std::regex_replace(stylesheet, bg, "$1" + relatedColor + ";\n");
                temp->setStyleSheet(stylesheet.c_str());
            }
        }
        if(originBoard[focus]){
            for(int i=0;i<9;i++)
                bottom_[i]->setEnabled(false);
            erase_->setEnabled(false);
        }
        else
        {
            int number=num.toStdString()[0]-'1';
            for(int i=0;i<9;i++)
                if(i!=number)
                    bottom_[i]->setEnabled(true);
                else
                    bottom_[i]->setEnabled(false);
            erase_->setEnabled(true);
        }
    }
    std::string stylesheet =pb->styleSheet().toStdString();
    stylesheet = std::regex_replace(stylesheet, bg, "$1" + focusColor + ";\n");
    pb->setStyleSheet(stylesheet.c_str());

}

void MainWindow::bottomClicked(){
    ui->remindMe->setEnabled(false);
    setBoard();
    for(int i=0;i<9;i++)
        bottom_[i]->setEnabled(true);
    erase_->setEnabled(true);
    QPushButton *pb = board_[focus];
    QPushButton *bottombutton=qobject_cast<QPushButton*>(sender());
    pb->setText(bottombutton->text());
    bottombutton->setEnabled(false);
    for(int i=0;i<81;i++)
    {
        QPushButton *temp=board_[i];
        if(temp->text()==pb->text()){
            //设置突出样式
            std::string stylesheet = temp->styleSheet().toStdString();
            stylesheet = std::regex_replace(stylesheet, bg, "$1" + relatedColor + ";\n");
            temp->setStyleSheet(stylesheet.c_str());
        }
    }
    std::string stylesheet =pb->styleSheet().toStdString();
    stylesheet = std::regex_replace(stylesheet, bg, "$1" + focusColor + ";\n");
    pb->setStyleSheet(stylesheet.c_str());
}

void MainWindow::remindMe(){
    // 检查已经填上的数是否合法
    int result[81] = {0};
    for(int i = 0; i < 81; i++){
        std::string num = board_[i]->text().toStdString();
        if(num != "")
            result[i] = num[0] - '0';
    }
    for(int i = 0; i < 81; i++){
        if(result[i]){
            // row
            for(int j = i / 9 * 9; j < (i / 9 + 1) * 9; j++)
                if(i != j && result[i] == result[j]){
                    std::string stylesheet = board_[i]->styleSheet().toStdString();
                    stylesheet = std::regex_replace(stylesheet, bg, "$1" + errorColor + ";\n");
                    board_[i]->setStyleSheet(stylesheet.c_str());
                    stylesheet = board_[j]->styleSheet().toStdString();
                    stylesheet = std::regex_replace(stylesheet, bg, "$1" + errorColor + ";\n");
                    board_[j]->setStyleSheet(stylesheet.c_str());
                    return;
                }
            for(int j = i % 9; j < 81; j += 9)
                if(i != j && result[i] == result[j]){
                    std::string stylesheet = board_[i]->styleSheet().toStdString();
                    stylesheet = std::regex_replace(stylesheet, bg, "$1" + errorColor + ";\n");
                    board_[i]->setStyleSheet(stylesheet.c_str());
                    stylesheet = board_[j]->styleSheet().toStdString();
                    stylesheet = std::regex_replace(stylesheet, bg, "$1" + errorColor + ";\n");
                    board_[j]->setStyleSheet(stylesheet.c_str());
                    return;
                }
            for(int k = i / 9 / 3 * 3; k < (i / 9 / 3 + 1) * 3; k++)
                for(int j = k * 9 + i % 9 / 3 * 3; j < k * 9 + (i % 9 / 3 + 1) * 3; j++)
                    if(i != j && result[i] == result[j]){
                        std::string stylesheet = board_[i]->styleSheet().toStdString();
                        stylesheet = std::regex_replace(stylesheet, bg, "$1" + errorColor + ";\n");
                        board_[i]->setStyleSheet(stylesheet.c_str());
                        stylesheet = board_[j]->styleSheet().toStdString();
                        stylesheet = std::regex_replace(stylesheet, bg, "$1" + errorColor + ";\n");
                        board_[j]->setStyleSheet(stylesheet.c_str());
                        return;
                    }
        }
    }
    int solution[81];
    if(solve_s(result, solution))
        board_[focus]->setText(QString(solution[focus] + '0'));
    else
        QMessageBox::StandardButton warning = QMessageBox::warning(new MainWindow(), tr("warning!"),
                                                                   tr("有一个错误藏得很深，提示也无能为力，再找找吧！"),
                                                                   QMessageBox::Ok);
}

void MainWindow::erase(){
    ui->remindMe->setEnabled(true);
    setBoard();
    for(int i=0;i<9;i++)
        bottom_[i]->setEnabled(true);
    board_[focus]->setText("");
    std::string stylesheet =board_[focus]->styleSheet().toStdString();
    stylesheet = std::regex_replace(stylesheet, bg, "$1" + focusColor + ";\n");
    board_[focus]->setStyleSheet(stylesheet.c_str());
}

void MainWindow::timerUpdate(){
    if(timerEnable_)
        ++costTime;
    char s[3];
    sprintf(s, "%02d", costTime % 60);
    ui->timer_sec->setText(s);
    sprintf(s, "%02d", costTime / 60);
    ui->timer_min->setText(s);
}
void MainWindow::finish(){
    int *result=new int[81];
    for(int i=0;i<81;i++){
        if(board_[i]->text()==""){
            QMessageBox::StandardButton warning= QMessageBox::warning(new MainWindow(),tr("warning!"),
                                                                     tr("貌似还没填完？？"),
                                                                     QMessageBox::Ok);
            std::string stylesheet =board_[i]->styleSheet().toStdString();
            stylesheet = std::regex_replace(stylesheet, bg, "$1" + errorColor + ";\n");
            board_[i]->setStyleSheet(stylesheet.c_str());
            return ;
        }
        result[i]=board_[i]->text().toStdString()[0]-'0';
    }
    for(int i = 0; i < 81; i++){
        if(result[i]){
            // row
            for(int j = i / 9 * 9; j < (i / 9 + 1) * 9; j++)
                if(i != j && result[i] == result[j]){
                    QMessageBox::StandardButton warning=QMessageBox::warning(new MainWindow(),tr("warning!"),
                                                                             tr("貌似有错？？"),
                                                                             QMessageBox::Ok);
                    std::string stylesheet = board_[i]->styleSheet().toStdString();
                    stylesheet = std::regex_replace(stylesheet, bg, "$1" + errorColor + ";\n");
                    board_[i]->setStyleSheet(stylesheet.c_str());
                    stylesheet = board_[j]->styleSheet().toStdString();
                    stylesheet = std::regex_replace(stylesheet, bg, "$1" + errorColor + ";\n");
                    board_[j]->setStyleSheet(stylesheet.c_str());
                    return;
                }
            for(int j = i % 9; j < 81; j += 9)
                if(i != j && result[i] == result[j]){
                    QMessageBox::StandardButton warning=QMessageBox::warning(new MainWindow(),tr("warning!"),
                                                                             tr("貌似有错？？"),
                                                                             QMessageBox::Ok);
                    std::string stylesheet = board_[i]->styleSheet().toStdString();
                    stylesheet = std::regex_replace(stylesheet, bg, "$1" + errorColor + ";\n");
                    board_[i]->setStyleSheet(stylesheet.c_str());
                    stylesheet = board_[j]->styleSheet().toStdString();
                    stylesheet = std::regex_replace(stylesheet, bg, "$1" + errorColor + ";\n");
                    board_[j]->setStyleSheet(stylesheet.c_str());
                    return;
                }
            for(int k = i / 9 / 3 * 3; k < (i / 9 / 3 + 1) * 3; k++)
                for(int j = k * 9 + i % 9 / 3 * 3; j < k * 9 + (i % 9 / 3 + 1) * 3; j++)
                    if(i != j && result[i] == result[j]){
                        QMessageBox::StandardButton warning=QMessageBox::warning(new MainWindow(),tr("warning!"),
                                                                                 tr("貌似有错？？"),
                                                                                 QMessageBox::Ok);
                        std::string stylesheet = board_[i]->styleSheet().toStdString();
                        stylesheet = std::regex_replace(stylesheet, bg, "$1" + errorColor + ";\n");
                        board_[i]->setStyleSheet(stylesheet.c_str());
                        stylesheet = board_[j]->styleSheet().toStdString();
                        stylesheet = std::regex_replace(stylesheet, bg, "$1" + errorColor + ";\n");
                        board_[j]->setStyleSheet(stylesheet.c_str());
                        return;
                    }
        }
    }

    timerEnable_=false;
    QMessageBox::StandardButton finished=QMessageBox::information(new MainWindow(),tr("填完了"),
                                                             tr("牛逼牛逼！！完全正确，您的用时：自己看吧"),
                                                             QMessageBox::Ok);
    std::ifstream recordin("record");
    int time[6];
    for(int i = 0; i < 6; i++)recordin >> time[i];
    recordin.close();
    if(time[presentGameMode] == -1 || time[presentGameMode] > costTime){
        time[presentGameMode] = costTime;
        std::fstream recordout("record", std::ios::out);
        for(int i = 0; i < 6; i++){
            recordout << time[i] << std::endl;
        }
        recordout << "abcdedf";
        recordout.close();
    }

    for(int i=0;i<9;i++)
    {
        bottom_[i]->setEnabled(false);
    }
    erase_->setEnabled(false);
    pause_->setEnabled(false);


}
void MainWindow::setBoard(){
    for(int i = 0; i < 81; ++i){
        QPushButton *pb = board_[i];
        pb->setFont(singleNumFont);
        std::string styleSheet = pb->styleSheet().toStdString();
        if(originBoard[i]){
            styleSheet = std::regex_replace(styleSheet, bg, "$1" + rstrColor + ";\n");
        }else{
            styleSheet = std::regex_replace(styleSheet, bg, "$1" + none_rstrColor+ ";\n");
        }
        pb->setStyleSheet(styleSheet.c_str());
    }
}
void MainWindow::help()
{
    if(helpdialog_)
        helpdialog_->reject();
    QDialog *dialog=new QDialog(this);
    QLabel *q=new QLabel();
    q->setFont(QFont("Comic Sans MS", 13, QFont::Normal));
    q->setText(tr("sudoku version 0.0\n"
                  "感谢您可以试玩我们的数独游戏!\n"
                  "填/删数字是先选定要填的框，然后通过下面的10个按钮（1-9和erase）来填或者删除对应格子上的数字\n"
                  "可以点击remind提醒您的错误或者帮您决定这里该填什么数字\n"
                  "您填完后可以点submit，如果有错系统会提示您，如果都对了，系统会记录下您所用的时间\n"
                  "您可以暂停游戏然后继续\n"
                  "右下方可以选择模式，分为easy normal hard三个级别，唯一解或者多解\n"
                  "祝您游戏愉快，如果有什么意见可以联系我们"));
    QGridLayout *layout=new QGridLayout(dialog);
    layout->addWidget(q,0,0);
    helpdialog_=dialog;
    dialog->show();

}
void MainWindow::showRecord(){
    std::ifstream recordFile("record");
    int time[6];
    for(int i = 0; i < 6; i++)
        recordFile >> time[i];
    recordFile.close();
    char outMessage[500];
    int p = 0;
    char *mode[] = {"easy", "normal", "hard"};
    char *unique[] = {"unique   solution  ", "multiple solutions"};
    for(int i = 0; i < 6; i++){
        if(time[i] == -1)
            sprintf(outMessage + p, "%s \t%s: no record\n", mode[i % 3], unique[i / 3]);
        else
            sprintf(outMessage + p, "%s \t%s: %d min %d sec\n", mode[i % 3], unique[i / 3], time[i] / 60, time[i] % 60);
        p += strlen(outMessage + p);
    }
    QMessageBox::StandardButton record=QMessageBox::information(new MainWindow(),tr("record"),
                                                             tr(outMessage),
                                                             QMessageBox::Ok);

}
