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
#include <QPushButton>
#include <QProgressBar>
#include <QDoubleSpinBox>
#include "connexion.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class Stock;
}
QT_END_NAMESPACE

class QSerialPort;

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
    void onRefreshPortsClicked();
    void onConnectArduinoClicked();
    void onSerialReadyRead();
    void onManualPlusOneClicked();
    void onManualConfirmClicked();
    void onManualWeightChanged(double value);
    void onValidatePoidsClicked();

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
    QSerialPort *m_serial;
    QString m_serialBuffer;
    double m_currentWeightL;
    QString m_currentStatusRaw;
    qlonglong m_currentStockId;
    bool m_hasStockId;
    bool m_freezeValue;
    qint64 m_lastValidationMs;

    QComboBox *m_comboPorts;
    QPushButton *m_btnConnectArduino;
    QPushButton *m_btnRefreshPorts;
    QLabel *m_lblPoidsTitre;
    QLabel *m_lblPoidsValeur;
    QLabel *m_lblStockIdValeur;
    QProgressBar *m_progressArduino;
    QLabel *m_lblStatusArduino;
    QDoubleSpinBox *m_spinManualWeight;
    QPushButton *m_btnPlusOneL;
    QPushButton *m_btnConfirmWeight;
    QPushButton *m_btnValiderPoids;
    const double m_minWeightThresholdL = 10.0;
    
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
    void setupArduinoTabUi();
    void refreshAvailablePorts();
    void parseArduinoLine(const QString &line);
    void applyArduinoStateUi();
    void updateProgressBarColor(const QString &status);
    void persistArduinoToStock(bool validated);
    void initialiserCalendrier();
    void chargerDatesStockCalendrier();
    void mettreEnCouleurCalendrier();
    void afficherStocksParDate(const QDate &date);
    void mettreAJourKpiCalendrier();
    void animerResumeCalendrier();
};

#endif // STOCK_H
