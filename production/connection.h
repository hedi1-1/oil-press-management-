#ifndef CONNECTION_H
#define CONNECTION_H

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <QDebug>

class Connection
{
public:
    Connection();
    bool createconnect();
    void closeconnect();
    QSqlDatabase getDatabase();

private:
    QSqlDatabase db;
};

#endif // CONNECTION_H
