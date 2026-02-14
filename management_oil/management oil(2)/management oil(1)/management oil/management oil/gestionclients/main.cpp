#include "gestionclients.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    GestionClients w;
    w.show();
    return a.exec();
}
