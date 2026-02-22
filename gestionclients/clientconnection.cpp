#include "clientconnection.h"

ClientConnection::ClientConnection() {
  db = QSqlDatabase::addDatabase("QODBC", "client_conn");
  db.setDatabaseName("Projet2A");
  db.setUserName("amal");
  db.setPassword("qwerty123456789");

  // Auto-open the connection
  if (!db.open()) {
    qDebug() << "[ClientConnection] Erreur de connexion:"
             << db.lastError().text();
  } else {
    qDebug()
        << "[ClientConnection] Connexion réussie à la base de données Oracle!";
  }
}

ClientConnection &ClientConnection::getInstance() {
  static ClientConnection instance;
  return instance;
}

bool ClientConnection::createconnect() {
  if (!db.open()) {
    qDebug() << "Erreur de connexion:" << db.lastError().text();
    return false;
  }
  qDebug() << "Connexion réussie à la base de données Oracle!";
  return true;
}

QSqlDatabase ClientConnection::getDatabase() { return db; }
