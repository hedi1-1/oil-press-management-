#ifndef CONNECTIONJASSER_H
#define CONNECTIONJASSER_H

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <QDebug>

class ConnectionJasser
{
public:
    static ConnectionJasser& getInstance();
    bool createconnect();
    void closeconnect();
    QSqlDatabase getDatabase();

private:
    ConnectionJasser();
    ConnectionJasser(const ConnectionJasser&) = delete;
    ConnectionJasser& operator=(const ConnectionJasser&) = delete;
    QSqlDatabase db;
};

#endif // CONNECTIONJASSER_H
