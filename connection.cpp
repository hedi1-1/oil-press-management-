#include "connection.h"

Connection::Connection()
{
    db = QSqlDatabase::addDatabase("QODBC");
    db.setDatabaseName("Source_Projet2A");
    db.setUserName("Ilef");
    db.setPassword("esprit18");
}

Connection& Connection::getInstance()
{
    static Connection instance;
    return instance;
}

bool Connection::createconnect()
{
    if (!db.open()) {
        qDebug() << "Erreur de connexion:" << db.lastError().text();
        return false;
    }
    qDebug() << "Connexion réussie à la base de données Oracle!";
    return true;
}

QSqlDatabase Connection::getDatabase()
{
    return db;
}
