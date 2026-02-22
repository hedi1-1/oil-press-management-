#ifndef STOCK_H
#define STOCK_H

#include <QMainWindow>
#include <QTimer>
#include <QSqlQueryModel>
#include <QTableWidgetItem>
#include "connexion.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class Stock;
}
QT_END_NAMESPACE

class Stock : public QMainWindow
{
    Q_OBJECT

public:
    Stock(QWidget *parent = nullptr);
    ~Stock();

signals:
    void backToMenu();

private slots:
    void onBackClicked();
    void updateDateTime();
    
    // CRUD Operations
    void onAjouterClicked();
    void onModifierClicked();
    void onSupprimerClicked();
    void onConsulterClicked();
    
    // Recherche et Filtres
    void onRechercherClicked();
    void onReinitialiserClicked();
    
    // Tri
    void onAppliquerTriClicked();
    
    // Export
    void onExporterClicked();
    
    // Statistiques
    void onCalculerClicked();
    
    // Alertes et Historique
    void onVerifierAlertesClicked();
    void onAfficherHistoriqueClicked();
    void onAnalyserTendancesClicked();
    
    // Rapports
    void onGenererRapportClicked();
    void onExportRapportPDFClicked();
    
    // Synchronisation
    void onSynchroniserClicked();
    
    // Selection dans la table
    void onTableStockSelectionChanged();

private:
    Ui::Stock *ui;
    QTimer *dateTimeTimer;
    Connexion* connexion;
    
    // Méthodes utilitaires
    void initialiserInterface();
    void activerScrolling();
    void chargerDonneesTable();
    void viderChamps();
    void remplirChamps(int row);
    bool validerChamps();
    void afficherMessage(const QString& titre, const QString& message, bool isError = false);
    void configurerTableStocks();
    QString getEtatStock(double quantite, double seuil);
};

#endif // STOCK_H
