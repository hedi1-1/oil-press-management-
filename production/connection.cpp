#include "connection.h"

Connection::Connection()
{
}

bool Connection::createconnect()
{
    bool test = false;
    
    // Utiliser QODBC avec le driver Oracle - Base ZITOUNA
    db = QSqlDatabase::addDatabase("QODBC", "production_conn");
    
    // Connexion via ODBC - Base de données ZITOUNA
    QString connectionString = "Driver={Oracle in XE};"
                               "DBQ=127.0.0.1:1521/XE;"
                               "UID=zitouna;"
                               "PWD=zitouna;";
    
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
    QString connName;
    if (db.isOpen()) {
        connName = db.connectionName();
        db.close();
        qDebug() << "Database connection closed.";
    }
    // Remove connection from Qt pool to avoid resource leak
    if (!connName.isEmpty()) {
        db = QSqlDatabase();  // release handle before removeDatabase
        QSqlDatabase::removeDatabase(connName);
    }
}

QSqlDatabase Connection::getDatabase()
{
    return db;
}
