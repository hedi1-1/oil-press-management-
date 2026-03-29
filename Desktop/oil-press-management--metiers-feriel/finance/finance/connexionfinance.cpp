#include "connexionfinance.h"

ConnexionFinance::ConnexionFinance() {
    // Connexion directe à Oracle - Base ZITOUNA
    if (QSqlDatabase::contains("finance_conn")) {
        db = QSqlDatabase::database("finance_conn");
    } else {
        db = QSqlDatabase::addDatabase("QODBC", "finance_conn");
        QString connectionString = "Driver={Oracle in XE};"
                                   "DBQ=127.0.0.1:1521/XE;"
                                   "UID=zitouna;"
                                   "PWD=zitouna;";
        db.setDatabaseName(connectionString);
    }
}

ConnexionFinance::~ConnexionFinance() {
    close();
}

bool ConnexionFinance::open() {
    if (db.isOpen()) {
        qDebug() << "[Finance] Connexion deja etablie.";
        return true;
    }

    if (db.open()) {
        qDebug() << "========================================";
        qDebug() << "[Finance] Connexion a la base de donnees reussie !";
        qDebug() << "[Finance] Host : 127.0.0.1:1521/XE | User : zitouna";
        qDebug() << "========================================";
        return true;
    } else {
        QString errorMsg = db.lastError().text();
        qDebug() << "========================================";
        qDebug() << "[Finance] ERREUR : Echec de la connexion !";
        qDebug() << "[Finance] Details :" << errorMsg;
        qDebug() << "========================================";
        return false;
    }
}

void ConnexionFinance::close() {
    if (db.isOpen()) {
        db.close();
        qDebug() << "[Finance] Connexion fermee.";
    }
}

QSqlDatabase ConnexionFinance::getDatabase() {
    return db;
}
