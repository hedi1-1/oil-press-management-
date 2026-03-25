#include "connectionlogin.h"

#include <QtSql/QSqlDatabase>

static const char* kLoginConnName = "login_conn";

ConnectionLogin::ConnectionLogin()
{
    if (QSqlDatabase::contains(kLoginConnName)) {
        db = QSqlDatabase::database(kLoginConnName);
    } else {
        db = QSqlDatabase::addDatabase("QODBC", kLoginConnName);
    }

    const QString connectionString = "Driver={Oracle in XE};"
                                     "DBQ=127.0.0.1:1521/XE;"
                                     "UID=zitouna;"
                                     "PWD=zitouna;";
    db.setDatabaseName(connectionString);
}

ConnectionLogin& ConnectionLogin::getInstance()
{
    static ConnectionLogin instance;
    return instance;
}

bool ConnectionLogin::createconnect()
{
    if (db.isOpen()) {
        return true;
    }

    if (db.open()) {
        qDebug() << "[Login] Connexion à la base de données réussie!";
        return true;
    }

    qDebug() << "[Login] Échec de connexion:" << db.lastError().text();
    return false;
}

void ConnectionLogin::closeconnect()
{
    if (db.isOpen()) {
        db.close();
        qDebug() << "[Login] Connexion fermée.";
    }
}

QSqlDatabase ConnectionLogin::getDatabase()
{
    return db;
}
