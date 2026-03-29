#include "finance.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Finance w(1);  // Default user ID = 1
    w.show();
    return a.exec();
}
