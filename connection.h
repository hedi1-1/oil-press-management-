#ifndef CONNECTION_H
#define CONNECTION_H

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QDebug>

class Connection
{
public:
    static Connection& getInstance();
    bool createconnect();
    QSqlDatabase getDatabase();

private:
    Connection();
    Connection(const Connection&) = delete;
    Connection& operator=(const Connection&) = delete;
    QSqlDatabase db;
};

#endif // CONNECTION_H
