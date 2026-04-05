#include "stock.h"
#include "connexion.h"
#include <QApplication>
#include <QDebug>
#include <QMessageBox>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    
    qDebug() << "\n*** DÉMARRAGE DE L'APPLICATION STOCK ***\n";
    
    // Obtenir l'instance Singleton de Connexion
    Connexion* conn = Connexion::getInstance();
    
    // Tester la connexion
    if (!conn->createConnection()) {
        QMessageBox::critical(nullptr, "Erreur", 
            "Impossible de se connecter à la base de données.\n"
            "Vérifiez que le service Oracle est démarré.");
        return -1;
    }
    
    qDebug() << "✓ Connexion établie - Lancement de l'interface...\n";
    
    // Lancer l'interface graphique
    Stock w;
    w.showFullScreen();
    
    return a.exec();
}
