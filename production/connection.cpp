#include "connection.h"

Connection::Connection() {
  db = QSqlDatabase::addDatabase("QODBC", "production_conn");
  db.setDatabaseName("Projet2A");
  db.setUserName("amal");
  db.setPassword("qwerty123456789");
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
