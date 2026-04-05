#include "../Login/login.h"
#include "../gestionclients/clientconnection.h"
#include "../machine/connexionmachine.h"
#include "../production/connection.h"
#include "../stock/connexion.h"
#include "menu.h"

#include <QApplication>
#include <QMessageBox>
#include <QDebug>
#include <QDir>

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);

  qDebug() << "[main] Resources check - :/logo.png exists:" << QFile::exists(":/logo.png");
  qDebug() << "[main] App dir:" << QCoreApplication::applicationDirPath();

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

  // 4. Machine connection
  if (!ConnectionMachine::getInstance().createconnect()) {
    qDebug() << "[main] Machine DB: ECHEC";
  } else {
    qDebug() << "[main] Machine DB: OK";
  }

  // Create login and menu windows
  Login *loginWindow = new Login();
  menu *menuWindow = new menu();

  // Connect login to menu navigation
  QObject::connect(loginWindow, &Login::goToMenu,
                   [menuWindow]() { menuWindow->showFullScreen(); });

  // Show login first
  loginWindow->showFullScreen();

  return a.exec();
}
