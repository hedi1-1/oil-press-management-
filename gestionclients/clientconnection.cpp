#include "clientconnection.h"

ClientConnection::ClientConnection() {
  db = QSqlDatabase::addDatabase("QODBC", "client_conn");
  
  // Connexion directe à Oracle - Base ZITOUNA
  QString connectionString = "Driver={Oracle in XE};"
                             "DBQ=127.0.0.1:1521/XE;"
                             "UID=zitouna;"
                             "PWD=zitouna;";
  db.setDatabaseName(connectionString);

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
