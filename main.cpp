#include "mainwindow.h"
#include "connection.h"

#include <QApplication>
#include <QMessageBox>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Connection &conn = Connection::getInstance();
    bool test = conn.createconnect();

    if (test) {
        QMessageBox::information(nullptr, "Connexion",
            "Connexion réussie à la base de données!");
    } else {
        QMessageBox::critical(nullptr, "Connexion",
            "Échec de la connexion à la base de données!");
    }

    MainWindow w;
    w.show();
    return a.exec();
}
