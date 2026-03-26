#ifndef STOCK_H
#define STOCK_H

#include <QMainWindow>
#include <QTimer>
#include <QSqlQueryModel>
#include <QTableWidgetItem>
#include <QCalendarWidget>
#include <QDate>
#include <QMap>
#include <QComboBox>
#include <QTableWidget>
#include <QTextEdit>
#include <QLabel>
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
    void onAnalyserPredictifHuileClicked();
    
    // Rapports
    void onGenererRapportClicked();
    void onExportRapportPDFClicked();
    
    // Synchronisation
    void onSynchroniserClicked();

    // Calendrier interactif
    void onCalendarDateSelected(const QDate &date);
    void onCalendarShowMonth(int year, int month);
    
    // Selection dans la table
    void onTableStockSelectionChanged();

private:
    Ui::Stock *ui;
    QTimer *dateTimeTimer;
    QTimer *calendarRefreshTimer;
    Connexion* connexion;
    QMap<QDate, QString> dateStatutMap;
    
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
    void initialiserCalendrier();
    void chargerDatesStockCalendrier();
    void mettreEnCouleurCalendrier();
    void afficherStocksParDate(const QDate &date);
    void mettreAJourKpiCalendrier();
    void animerResumeCalendrier();
};

#endif // STOCK_H
