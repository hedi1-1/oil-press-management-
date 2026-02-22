#include "connexionfinance.h"

ConnexionFinance::ConnexionFinance() {
    // Utilisation du driver QODBC avec le DSN "Projet2A" (même config que les autres modules)
    if (QSqlDatabase::contains("finance_conn")) {
        db = QSqlDatabase::database("finance_conn");
    } else {
        db = QSqlDatabase::addDatabase("QODBC", "finance_conn");
        db.setDatabaseName("projet");
        db.setUserName("zitouna");
        db.setPassword("zitouna");
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
        qDebug() << "[Finance] DSN : Projet2A | User : amal";
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
