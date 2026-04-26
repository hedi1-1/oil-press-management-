#include "production.h"
#include "connection.h"

#include <QApplication>
#include <QMessageBox>
#include <QSqlDatabase>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    
    a.setApplicationName("PressIQ - Smart Oil Systems");
    a.setApplicationVersion("1.0");
    a.setOrganizationName("Smart Oil Systems");
    a.setStyle("Fusion");
    
    // Afficher les drivers disponibles
    qDebug() << "Drivers SQL disponibles:" << QSqlDatabase::drivers();
    
    // Test connexion Oracle
    Connection c;
    if (c.createconnect()) {
        QMessageBox::information(nullptr, "Connexion", "Connexion Oracle réussie!");
    } else {
        QString errorMsg = "Échec de connexion à Oracle!\n\n";
        errorMsg += "Drivers disponibles: " + QSqlDatabase::drivers().join(", ") + "\n\n";
        errorMsg += "Vérifiez:\n";
        errorMsg += "1. Oracle XE est démarré\n";
        errorMsg += "2. Le driver QOCI est installé\n";
        errorMsg += "3. oci.dll est dans le PATH";
        QMessageBox::critical(nullptr, "Erreur", errorMsg);
        return -1;
    }
    
    Production w;
    w.showFullScreen();
    return a.exec();
}
