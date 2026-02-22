#include "connexionmachine.h"

ConnectionMachine::ConnectionMachine() {
  db = QSqlDatabase::addDatabase("QODBC", "machine_conn");
  db.setDatabaseName("projet");
  db.setUserName("zitouna");
  db.setPassword("zitouna");
}

ConnectionMachine &ConnectionMachine::getInstance() {
  static ConnectionMachine instance;
  return instance;
}

bool ConnectionMachine::createconnect() {
  if (db.isOpen()) {
    return true;
  }

  if (db.open()) {
    qDebug() << "[Machine] Connexion à la base de données réussie!";
    return true;
  } else {
    qDebug() << "[Machine] Échec de connexion:" << db.lastError().text();
    return false;
  }
}

QSqlDatabase ConnectionMachine::getDatabase() { return db; }
