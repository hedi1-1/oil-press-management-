#include "connection.h"

Connection::Connection() {
  db = QSqlDatabase::addDatabase("QODBC", "production_conn");
  
  // Connexion directe à Oracle - Base ZITOUNA
  QString connectionString = "Driver={Oracle in XE};"
                             "DBQ=127.0.0.1:1521/XE;"
                             "UID=zitouna;"
                             "PWD=zitouna;";
  db.setDatabaseName(connectionString);
}

Connection &Connection::getInstance() {
  static Connection instance;
  return instance;
}

bool Connection::createconnect() {
  if (db.isOpen()) {
    return true;
  }

  if (db.open()) {
    qDebug() << "[Production] Connexion à la base de données réussie!";
    return true;
  } else {
    qDebug() << "[Production] Échec de connexion:" << db.lastError().text();
    return false;
  }
}

void Connection::closeconnect() {
  if (db.isOpen()) {
    db.close();
    qDebug() << "[Production] Connexion fermée.";
  }
}

QSqlDatabase Connection::getDatabase() { return db; }
