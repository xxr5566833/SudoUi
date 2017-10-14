#include "mainwindow.h"
#include "QApplication"
#include "main.h"
HMODULE coreDLL;
GENERATE_M generate_m = NULL;
GENERATE_R generate_r = NULL;
SOLVE_S solve_s = NULL;
int main(int argc, char *argv[])
{
    // 从DLL中导入三个函数
    coreDLL = LoadLibrary(TEXT("Core/SoduCore.dll"));
    generate_m = (GENERATE_M)GetProcAddress(coreDLL, "generate_m");
    generate_r = (GENERATE_R)GetProcAddress(coreDLL, "generate_r");
    solve_s = (SOLVE_S)GetProcAddress(coreDLL, "solve_s");
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
