#ifndef CONNEXIONFINANCE_H
#define CONNEXIONFINANCE_H

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QDebug>

class ConnexionFinance {
private:
    QSqlDatabase db;

public:
    ConnexionFinance();
    ~ConnexionFinance();
    
    bool open();
    void close();
    QSqlDatabase getDatabase();
};

#endif // CONNEXIONFINANCE_H
