#include "connection.h"

Connection::Connection() {
  db = QSqlDatabase::addDatabase("QODBC", "production_conn");
  db.setDatabaseName("projet");
  db.setUserName("zitouna");
  db.setPassword("zitouna");
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
