#ifndef STOCKMANAGEMENT_H
#define STOCKMANAGEMENT_H

#include <QWidget>
#include <QTabWidget>
#include <QTableWidget>
#include <QTableView>
#include <QLineEdit>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QDateEdit>
#include <QPushButton>
#include <QGroupBox>
#include <QRadioButton>
#include <QCheckBox>
#include <QTextEdit>
#include <QLabel>
#include <QProgressBar>
#include <QListWidget>
#include <QLCDNumber>
#include <QTimer>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGridLayout>
#include <QStandardItemModel>
#include <QMessageBox>
#include <QList>
#include "stock.h"

class StockManagement : public QWidget
{
    Q_OBJECT

public:
    explicit StockManagement(QWidget *parent = nullptr);
    ~StockManagement();

private slots:
    // Onglet 1 - CRUD
    void onAjouterStock();
    void onModifierStock();
    void onSupprimerStock();
    void onConsulterStock();

    // Onglet 2 - Tri
    void onAppliquerTri();

    // Onglet 3 - Recherche
    void onRechercher();
    void onReinitialiserRecherche();

    // Onglet 4 - Export
    void onExporterStock();

    // Onglet 5 - Statistiques
    void onCalculerStatistiques();

    // Onglet 6 - Historique
    void onAfficherHistorique();

    // Onglet 7 - État du Stock
    void updateEtatStock();

    // Onglet 8 - Temps Réel
    void onUpdateTempsReel();

    // Onglet 9 - Alertes
    void verifierAlertes();

    // Onglet 10 - Tendances
    void onAnalyserTendances();

    // Onglet 11 - Rapport
    void onGenererRapport();
    void onExportRapportPDF();

    // Onglet 12 - Arduino
    void onSynchroniserArduino();

    // Utilitaire
    void onTableSelectionChanged();

private:
    // Création des onglets
    QWidget* createOnglet1_GestionStock();
    QWidget* createOnglet2_TriStock();
    QWidget* createOnglet3_RechercheStock();
    QWidget* createOnglet4_ExportStock();
    QWidget* createOnglet5_Statistiques();
    QWidget* createOnglet6_Historique();
    QWidget* createOnglet7_EtatStock();
    QWidget* createOnglet8_TempsReel();
    QWidget* createOnglet9_AlerteStock();
    QWidget* createOnglet10_Tendances();
    QWidget* createOnglet11_RapportAuto();
    QWidget* createOnglet12_Arduino();

    // Utilitaires
    void setupTable(QTableWidget *table);
    void populateTableWithStocks(QTableWidget *table);
    void loadSampleData();
    QString getStyleSheet();

    // Widget principal
    QTabWidget *m_tabWidget;

    // === Onglet 1 - CRUD ===
    QLineEdit *m_lineIdStock;
    QComboBox *m_comboTypeHuile;
    QDoubleSpinBox *m_spinQuantite;
    QSpinBox *m_spinSeuilAlerte;
    QDateEdit *m_dateEditMaj;
    QLineEdit *m_lineEmplacement;
    QComboBox *m_comboEtatStock;
    QTableWidget *m_tableStocks;
    QPushButton *m_btnAjouter;
    QPushButton *m_btnModifier;
    QPushButton *m_btnSupprimer;
    QPushButton *m_btnConsulter;

    // === Onglet 2 - Tri ===
    QComboBox *m_comboTriCritere;
    QTableWidget *m_tableTriee;

    // === Onglet 3 - Recherche ===
    QComboBox *m_comboRechercheType;
    QComboBox *m_comboRechercheEtat;
    QLineEdit *m_lineRechercheEmplacement;
    QTableWidget *m_tableResultats;

    // === Onglet 4 - Export ===
    QRadioButton *m_radioPDF;
    QRadioButton *m_radioExcel;
    QCheckBox *m_checkStats;
    QCheckBox *m_checkDateTime;
    QLineEdit *m_lineNomFichier;

    // === Onglet 5 - Statistiques ===
    QLabel *m_labelTotalArticles;
    QLabel *m_labelValeurTotale;
    QComboBox *m_comboStatsPar;
    QTextEdit *m_textResumeStats;

    // === Onglet 6 - Historique ===
    QDateEdit *m_dateDebut;
    QDateEdit *m_dateFin;
    QTableWidget *m_tableHistorique;

    // === Onglet 7 - État ===
    QLabel *m_labelEtatGlobal;
    QProgressBar *m_progressStock;
    QListWidget *m_listProduitsCritiques;

    // === Onglet 8 - Temps Réel ===
    QLabel *m_labelQuantiteActuelle;
    QLCDNumber *m_lcdVolume;
    QLabel *m_labelDerniereMaj;
    QTimer *m_timerTempsReel;

    // === Onglet 9 - Alertes ===
    QLabel *m_labelSeuilDefini;
    QListWidget *m_listStocksSousSeuil;

    // === Onglet 10 - Tendances ===
    QComboBox *m_comboPeriode;
    QTextEdit *m_textAnalyseTendances;

    // === Onglet 11 - Rapport ===
    QTableWidget *m_tableRapportSelection;
    QTextEdit *m_textApercuRapport;

    // === Onglet 12 - Arduino ===
    QLabel *m_labelEtatConnexion;
    QLabel *m_labelValeurCapteur;
    QTextEdit *m_textLogsArduino;

    // Données
    QList<Stock> m_stocks;
    QList<MouvementStock> m_mouvements;
};

#endif // STOCKMANAGEMENT_H
