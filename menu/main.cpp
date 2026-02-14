#include "menu.h"
#include "../Login/login.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    
    a.setApplicationName("Smart Oil Press Management");
    a.setApplicationVersion("1.0");
    a.setOrganizationName("Smart Oil Systems");
    a.setStyle("Fusion");
    
    // Create login and menu windows
    Login *loginWindow = new Login();
    menu *menuWindow = new menu();
    
    // Connect login to menu navigation
    QObject::connect(loginWindow, &Login::goToMenu, [menuWindow]() {
        menuWindow->show();
    });
    
    // Show login first
    loginWindow->show();
    
    return a.exec();
}
