#include "menu.h"
#include "../Login/login.h"
#include "../gestionclients/clientconnection.h"
#include "../stock/connexion.h"
#include "../production/connection.h"

#include <QApplication>
#include <QMessageBox>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    
    a.setApplicationName("Smart Oil Press Management");
    a.setApplicationVersion("1.0");
    a.setOrganizationName("Smart Oil Systems");
    a.setStyle("Fusion");
    
    // ── Initialize all database connections (Singleton) ──
    // 1. Client connection
    ClientConnection &clientConn = ClientConnection::getInstance();
    if (!clientConn.getDatabase().isOpen()) {
        qDebug() << "[main] Client DB: ECHEC";
    } else {
        qDebug() << "[main] Client DB: OK";
    }
    
    // 2. Stock connection
    Connexion *stockConn = Connexion::getInstance();
    if (!stockConn->createConnection()) {
        qDebug() << "[main] Stock DB: ECHEC";
    } else {
        qDebug() << "[main] Stock DB: OK";
    }
    
    // 3. Production connection
    if (!Connection::getInstance().createconnect()) {
        qDebug() << "[main] Production DB: ECHEC";
    } else {
        qDebug() << "[main] Production DB: OK";
    }
    
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
