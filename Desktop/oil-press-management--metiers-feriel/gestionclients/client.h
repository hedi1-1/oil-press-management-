#ifndef CLIENT_H
#define CLIENT_H

#include <QString>
#include <QDate>

// Structure Client simple
struct Client {
    int id_client;
    QString nom;
    QString prenom;
    QString telephone;
    QString adresse;
    QString email;
    QString type_client;
    double total_olives_livrees;
    QDate date_creation;
    QString statut;
    QString importance;
    
    QString getStatutFidelite() const;
    QString getImportance() const;
    double getScoreFidelite() const;
    
    bool operator==(const Client &other) const {
        return id_client == other.id_client;
    }
};

#endif // CLIENT_H
