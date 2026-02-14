#include "connection.h"

Connection::Connection()
{
}

bool Connection::createconnect()
{
    bool test = false;
    
    // Utiliser QODBC avec le driver Oracle
    db = QSqlDatabase::addDatabase("QODBC");
    
    // Connexion via ODBC - Driver "Oracle in XE" détecté sur ce système
    QString connectionString = "Driver={Oracle in XE};"
                               "DBQ=127.0.0.1:1521/XE;"
                               "UID=hr;"
                               "PWD=hr;";
    
    db.setDatabaseName(connectionString);
    
    if (db.open()) {
        test = true;
        qDebug() << "Database connection established successfully!";
    } else {
        qDebug() << "Database connection failed!";
        qDebug() << "Error:" << db.lastError().text();
    }
    
    return test;
}

void Connection::closeconnect()
{
    if (db.isOpen()) {
        db.close();
        qDebug() << "Database connection closed.";
    }
}

QSqlDatabase Connection::getDatabase()
{
    return db;
}
