#include "connexionmachine.h"

ConnectionMachine::ConnectionMachine() {
  db = QSqlDatabase::addDatabase("QODBC", "machine_conn");
  
  // Connexion directe à Oracle - Base ZITOUNA
  QString connectionString = "Driver={Oracle in XE};"
                             "DBQ=127.0.0.1:1521/XE;"
                             "UID=zitouna;"
                             "PWD=zitouna;";
  db.setDatabaseName(connectionString);
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
