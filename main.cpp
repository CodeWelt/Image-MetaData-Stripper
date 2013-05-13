#include <QtGui/QApplication>
#include "mainwindowstripper.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindowStripper w;
    w.show();

    return a.exec();
}
