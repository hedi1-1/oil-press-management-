#ifndef CONNECTION_H
#define CONNECTION_H

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
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
