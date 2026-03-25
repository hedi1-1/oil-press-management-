#include "userstaff.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    userstaff w("Admin", "Admin");
    w.show();
    return a.exec();
}
