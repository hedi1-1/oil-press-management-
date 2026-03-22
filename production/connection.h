#ifndef CONNECTION_H
#define CONNECTION_H

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <QDebug>

class Connection
{
public:
    static Connection& getInstance();
    bool createconnect();
    void closeconnect();
    QSqlDatabase getDatabase();

private:
    Connection();
    Connection(const Connection&) = delete;
    Connection& operator=(const Connection&) = delete;
    QSqlDatabase db;
};

#endif // CONNECTION_H
