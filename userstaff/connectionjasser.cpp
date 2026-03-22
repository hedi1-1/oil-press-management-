#include "connectionjasser.h"

ConnectionJasser::ConnectionJasser() {
  db = QSqlDatabase::addDatabase("QODBC", "jasser_conn");
  
  // Connexion directe à Oracle - Base ZITOUNA
  QString connectionString = "Driver={Oracle in XE};"
                             "DBQ=127.0.0.1:1521/XE;"
                             "UID=zitouna;"
                             "PWD=zitouna;";
  db.setDatabaseName(connectionString);
}

ConnectionJasser &ConnectionJasser::getInstance() {
  static ConnectionJasser instance;
  return instance;
}

bool ConnectionJasser::createconnect() {
  if (db.isOpen()) {
    return true;
  }

  if (db.open()) {
    qDebug() << "[Jasser] Connexion à la base de données réussie!";
    return true;
  } else {
    qDebug() << "[Jasser] Échec de connexion:" << db.lastError().text();
    return false;
  }
}

void ConnectionJasser::closeconnect() {
  if (db.isOpen()) {
    db.close();
    qDebug() << "[Jasser] Connexion fermée.";
  }
}

QSqlDatabase ConnectionJasser::getDatabase() { return db; }
