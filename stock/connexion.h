#ifndef CONNEXION_H
#define CONNEXION_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QMessageBox>

class Connexion
{
private:
    // Instance statique privée (Singleton)
    static Connexion* instance;
    
    // Base de données
    QSqlDatabase db;
    
    // Constructeur privé (Singleton)
    Connexion();
    
    // Empêcher la copie et l'assignation
    Connexion(const Connexion&) = delete;
    Connexion& operator=(const Connexion&) = delete;

public:
    // Destructeur
    ~Connexion();
    
    // Méthode pour obtenir l'instance unique (Singleton)
    static Connexion* getInstance();
    
    // Méthodes de gestion de la connexion
    bool createConnection();
    void closeConnection();
    
    // Getter pour la base de données
    QSqlDatabase getDatabase();
    
    // Vérifier si la connexion est ouverte
    bool isOpen() const;
};

#endif // CONNEXION_H
