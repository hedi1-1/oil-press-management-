#include "connectionjasser.h"

#include <QStringList>
#include "connection.h"
#include <QSerialPortInfo>
#include <QSqlQuery>
#include <QDebug>

static QString envOrDefault(const char *name, const QString &def)
{
  const QByteArray v = qgetenv(name);
  return v.isEmpty() ? def : QString::fromLocal8Bit(v);
}

static bool envBool(const char *name, bool def)
{
  const QByteArray v = qgetenv(name).trimmed().toLower();
  if (v.isEmpty()) return def;
  return v == "1" || v == "true" || v == "yes" || v == "on";
}

static QStringList buildOracleOdbcConnStrings()
{
  const QString dsn    = envOrDefault("PRESSIQ_ODBC_DSN", "project");
  QString altDsn = QStringLiteral("projet");
  if (dsn.compare("project", Qt::CaseInsensitive) != 0) {
    altDsn = QStringLiteral("project");
  }
  const QString driver = envOrDefault("PRESSIQ_ORACLE_ODBC_DRIVER", "Oracle in XE");
  const QString host   = envOrDefault("PRESSIQ_ORACLE_HOST", "localhost");
  const QString port   = envOrDefault("PRESSIQ_ORACLE_PORT", "1521");
  const QString service = envOrDefault("PRESSIQ_ORACLE_SERVICE",
                                       envOrDefault("PRESSIQ_ORACLE_SID", "XE"));
  const QString uid    = envOrDefault("PRESSIQ_ORACLE_UID",  "zitouna");
  const QString pwd    = envOrDefault("PRESSIQ_ORACLE_PWD",  "zitouna");

  const QString dbqSlashLower = QString("%1:%2/%3").arg(host, port, service);
  const QString dbqSlashUpper = QString("%1:%2/%3").arg(host, port, service.toUpper());
  const QString dbqColonLower = QString("%1:%2:%3").arg(host, port, service);
  const QString dbqColonUpper = QString("%1:%2:%3").arg(host, port, service.toUpper());

  auto mk = [&](const QString &dbq) {
    return QString("Driver={%1};DBQ=%2;UID=%3;PWD=%4;").arg(driver, dbq, uid, pwd);
  };

  const QString dsnConn = QString("DSN=%1;UID=%2;PWD=%3;").arg(dsn, uid, pwd);
  const QString dsnOnlyConn = QString("DSN=%1;").arg(dsn);
  const QString altDsnConn = QString("DSN=%1;UID=%2;PWD=%3;").arg(altDsn, uid, pwd);
  const QString altDsnOnlyConn = QString("DSN=%1;").arg(altDsn);

  const bool allowDirectDbqFallback = envBool("PRESSIQ_ALLOW_DIRECT_DBQ_FALLBACK", true);
  if (!allowDirectDbqFallback) {
    return { dsnConn, dsnOnlyConn, altDsnConn, altDsnOnlyConn };
  }

  return { mk(dbqSlashLower), mk(dbqSlashUpper), mk(dbqColonLower), mk(dbqColonUpper),
           dsnConn, dsnOnlyConn, altDsnConn, altDsnOnlyConn };
}

ConnectionJasser::ConnectionJasser() {
  db = QSqlDatabase::addDatabase("QODBC", "jasser_conn");
  // Connection string is applied in createconnect() to allow retries/fallbacks.
}

ConnectionJasser &ConnectionJasser::getInstance() {
  static ConnectionJasser instance;
  return instance;
}

bool ConnectionJasser::createconnect() {
  if (db.isOpen()) {
    return true;
  }

  QString lastErr;
  for (const QString &connStr : buildOracleOdbcConnStrings()) {
    db.setDatabaseName(connStr);
    if (db.open()) {
      qDebug() << "[Jasser] Connexion à la base de données réussie!";
      return true;
    }
    lastErr = db.lastError().text();
  }

  qDebug() << "[Jasser] Échec de connexion:" << lastErr;
  return false;
}

void ConnectionJasser::closeconnect() {
  if (db.isOpen()) {
    db.close();
    qDebug() << "[Jasser] Connexion fermée.";
  }
}

QSqlDatabase ConnectionJasser::getDatabase() { return db; }



Connection::Connection(QObject *parent) : QObject(parent)
{
}

void Connection::start()
{
    connectDatabase();
    setupSerial();
}

void Connection::setupSerial()
{
    serial = new QSerialPort(this);

    serial->setPortName("COM3"); // change this
    serial->setBaudRate(QSerialPort::Baud9600);

    if(serial->open(QIODevice::ReadOnly)) {
        connect(serial, &QSerialPort::readyRead,
                this, &Connection::readArduino);
    } else {
        qDebug() << "Serial not opened";
    }
}

void Connection::readArduino()
{
    QByteArray data = serial->readAll();
    QString value = QString(data).trimmed();

    saveToDatabase(value);
}

void Connection::connectDatabase()
{
    db = QSqlDatabase::addDatabase("PRESSIQ_ODBC_DSN");
    db.setDatabaseName("create_zitouna_db.sql");

    if(!db.open()) {
        qDebug() << "DB Error";
    }
}

void Connection::saveToDatabase(QString value)
{
    QSqlQuery query;
    query.prepare("INSERT INTO capteurs (valeur) VALUES (:val)");
    query.bindValue(":val", value);

    if(!query.exec()) {
        qDebug() << "Insert error";
    }
}
