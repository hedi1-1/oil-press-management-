#include "connexionmachine.h"

Connection::Connection()
{

}

bool Connection::createconnect()
{bool test=false;
QSqlDatabase db = QSqlDatabase::addDatabase("QODBC");
db.setDatabaseName("Projet2A");//inserer le nom de la source de données
db.setUserName("amal");//inserer nom de l'utilisateur
db.setPassword("qwerty123456789");//inserer mot de passe de cet utilisateur

if (db.open())
test=true;





    return  test;
}
