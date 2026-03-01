#ifndef CONNECTIONMACHINE_H
#define CONNECTIONMACHINE_H
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>


class ConnectionMachine {
public:
  static ConnectionMachine &getInstance();
  bool createconnect();
  QSqlDatabase getDatabase();

private:
  ConnectionMachine();
  QSqlDatabase db;
};

#endif // CONNECTIONMACHINE_H
