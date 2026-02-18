#ifndef CLIENT_CONNECTION_H
#define CLIENT_CONNECTION_H

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QDebug>

class ClientConnection
{
public:
    static ClientConnection& getInstance();
    bool createconnect();
    QSqlDatabase getDatabase();

private:
    ClientConnection();
    ClientConnection(const ClientConnection&) = delete;
    ClientConnection& operator=(const ClientConnection&) = delete;
    QSqlDatabase db;
};

#endif // CLIENT_CONNECTION_H
