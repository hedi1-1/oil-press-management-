#include "stock.h"

Stock::Stock()
    : m_id(0), m_quantite(0.0), m_seuilAlerte(0)
{
}

Stock::Stock(int id, const QString &typeHuile, double quantite, int seuilAlerte,
             const QDate &dateMaj, const QString &emplacement, const QString &etat)
    : m_id(id)
    , m_typeHuile(typeHuile)
    , m_quantite(quantite)
    , m_seuilAlerte(seuilAlerte)
    , m_dateMaj(dateMaj)
    , m_emplacement(emplacement)
    , m_etat(etat)
{
}

MouvementStock::MouvementStock()
    : m_quantite(0.0)
{
}

MouvementStock::MouvementStock(const QDateTime &dateHeure, const QString &typeMouvement,
                               double quantite, const QString &produit)
    : m_dateHeure(dateHeure)
    , m_typeMouvement(typeMouvement)
    , m_quantite(quantite)
    , m_produit(produit)
{
}
