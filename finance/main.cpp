#include "finance.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Finance w;
    w.showFullScreen();
    return a.exec();
}
