#ifndef STOCK_H
#define STOCK_H

#include <QString>
#include <QDate>

// Classe représentant un stock d'huile
class Stock
{
public:
    Stock();
    Stock(int id, const QString &typeHuile, double quantite, int seuilAlerte,
          const QDate &dateMaj, const QString &emplacement, const QString &etat);

    // Getters
    int getId() const { return m_id; }
    QString getTypeHuile() const { return m_typeHuile; }
    double getQuantite() const { return m_quantite; }
    int getSeuilAlerte() const { return m_seuilAlerte; }
    QDate getDateMaj() const { return m_dateMaj; }
    QString getEmplacement() const { return m_emplacement; }
    QString getEtat() const { return m_etat; }

    // Setters
    void setId(int id) { m_id = id; }
    void setTypeHuile(const QString &type) { m_typeHuile = type; }
    void setQuantite(double quantite) { m_quantite = quantite; }
    void setSeuilAlerte(int seuil) { m_seuilAlerte = seuil; }
    void setDateMaj(const QDate &date) { m_dateMaj = date; }
    void setEmplacement(const QString &emplacement) { m_emplacement = emplacement; }
    void setEtat(const QString &etat) { m_etat = etat; }

    // Méthodes utilitaires
    bool estSousSeuil() const { return m_quantite <= m_seuilAlerte; }
    bool estCritique() const { return m_quantite <= m_seuilAlerte / 2; }

private:
    int m_id;
    QString m_typeHuile;      // Extra vierge, Vierge, Lampante
    double m_quantite;         // en Litres
    int m_seuilAlerte;
    QDate m_dateMaj;
    QString m_emplacement;
    QString m_etat;            // En stock, Stock faible, Rupture
};

// Classe pour les mouvements de stock
class MouvementStock
{
public:
    MouvementStock();
    MouvementStock(const QDateTime &dateHeure, const QString &typeMouvement,
                   double quantite, const QString &produit);

    QDateTime getDateHeure() const { return m_dateHeure; }
    QString getTypeMouvement() const { return m_typeMouvement; }
    double getQuantite() const { return m_quantite; }
    QString getProduit() const { return m_produit; }

private:
    QDateTime m_dateHeure;
    QString m_typeMouvement;  // Entrée, Sortie
    double m_quantite;
    QString m_produit;
};

#endif // STOCK_H
