#include "production.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    
    a.setApplicationName("PressIQ - Smart Oil Systems");
    a.setApplicationVersion("1.0");
    a.setOrganizationName("Smart Oil Systems");
    a.setStyle("Fusion");
    
    Production w;
    w.show();
    return a.exec();
}
