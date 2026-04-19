#ifndef CONNECTIONLOGIN_H
#define CONNECTIONLOGIN_H

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QDebug>

class ConnectionLogin
{
public:
    static ConnectionLogin& getInstance();

    bool createconnect();
    void closeconnect();
    QSqlDatabase getDatabase();

private:
    ConnectionLogin();
    ConnectionLogin(const ConnectionLogin&) = delete;
    ConnectionLogin& operator=(const ConnectionLogin&) = delete;

    QSqlDatabase db;
};

#endif // CONNECTIONLOGIN_H
