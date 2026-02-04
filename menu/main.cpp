#include "menu.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    
    a.setApplicationName("Smart Oil Press Management");
    a.setApplicationVersion("1.0");
    a.setOrganizationName("Smart Oil Systems");
    a.setStyle("Fusion");
    
    menu w;
    w.show();
    return a.exec();
}
