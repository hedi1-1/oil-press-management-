#ifndef FINANCE_H
#define FINANCE_H

// ═══════════════════════════════════════════════════════════════
// INCLUDES
// ═══════════════════════════════════════════════════════════════
#include <QMainWindow>
#include <QWidget>
#include <QTabWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QDateEdit>
#include <QTextEdit>
#include <QPushButton>
#include <QTableWidget>
#include <QHeaderView>
#include <QGroupBox>
#include <QLabel>
#include <QProgressBar>
#include <QScrollArea>
#include <QSpacerItem>

// ═══════════════════════════════════════════════════════════════
// FORWARD DECLARATIONS
// ═══════════════════════════════════════════════════════════════
QT_BEGIN_NAMESPACE
namespace Ui { class Finance; }
QT_END_NAMESPACE

// ═══════════════════════════════════════════════════════════════
// CLASSE 1: TransactionTab
// ═══════════════════════════════════════════════════════════════
class TransactionTab : public QWidget
{
    Q_OBJECT

public:
    explicit TransactionTab(QWidget *parent = nullptr);

private slots:
    void ajouterTransaction();
    void modifierTransaction();
    void supprimerTransaction();
    void chargerTransactions();
    void effacerFormulaire();

private:
    void initializeUI();
    void setupConnections();

    // Widgets - Zone Formulaire
    QLineEdit *txtId;
    QComboBox *cbType;
    QComboBox *cbCategorie;
    QDoubleSpinBox *spinMontant;
    QDateEdit *dateEdit;
    QTextEdit *txtDescription;
    QComboBox *cbClient;
    QPushButton *btnAdd;
    QPushButton *btnUpdate;
    QPushButton *btnClear;

    // Widgets - Zone Tableau
    QTableWidget *tableTransaction;
    QPushButton *btnDelete;
    QPushButton *btnRefresh;
};

// ═══════════════════════════════════════════════════════════════
// CLASSE 2: SearchTab
// ═══════════════════════════════════════════════════════════════
class SearchTab : public QWidget
{
    Q_OBJECT

public:
    explicit SearchTab(QWidget *parent = nullptr);

private slots:
    void rechercher();
    void trier();

private:
    void initializeUI();
    void setupConnections();

    // Groupe Recherche
    QComboBox *cbSearchType;
    QLineEdit *txtSearch;
    QDateEdit *dateFrom;
    QDateEdit *dateTo;
    QPushButton *btnSearch;
    QTableWidget *tableResult;

    // Groupe Tri
    QComboBox *cbSortBy;
    QComboBox *cbOrder;
    QPushButton *btnSort;
};

// ═══════════════════════════════════════════════════════════════
// CLASSE 3: StatsTab
// ═══════════════════════════════════════════════════════════════
class StatsTab : public QWidget
{
    Q_OBJECT

public:
    explicit StatsTab(QWidget *parent = nullptr);

private slots:
    void calculerStats();
    void afficherGraphique();

private:
    void initializeUI();
    void setupConnections();
    QGroupBox* createSummarySection();
    QGroupBox* createChartControlSection();
    QGroupBox* createChartSection();

    // Résumé
    QLabel *lblTotalRevenus;
    QLabel *lblTotalDepenses;
    QLabel *lblBenefice;

    // Graphiques
    QComboBox *cbChartType;
    QWidget *chartView;
    QPushButton *btnGenerate;
};

// ═══════════════════════════════════════════════════════════════
// CLASSE 4: JournalTab
// ═══════════════════════════════════════════════════════════════
class JournalTab : public QWidget
{
    Q_OBJECT

public:
    explicit JournalTab(QWidget *parent = nullptr);

private slots:
    void filtrerJournal();
    void afficherHistorique();

private:
    void initializeUI();
    void setupConnections();

    // Filtres
    QDateEdit *dateFilter;
    QComboBox *cbTypeFilter;
    QComboBox *cbCatFilter;
    QPushButton *btnFilter;

    // Tableau
    QTableWidget *tableJournal;
};

// ═══════════════════════════════════════════════════════════════
// CLASSE 5: BudgetTab
// ═══════════════════════════════════════════════════════════════
class BudgetTab : public QWidget
{
    Q_OBJECT

public:
    explicit BudgetTab(QWidget *parent = nullptr);

private slots:
    void enregistrerBudget();
    void comparerBudget();

private:
    void initializeUI();
    void setupConnections();

    // Budget
    QComboBox *cbMonth;
    QDoubleSpinBox *spinBudget;
    QPushButton *btnSaveBudget;

    // Résultat
    QLabel *lblDepenseReelle;
    QLabel *lblEcart;
    QProgressBar *progressBudget;
};

// ═══════════════════════════════════════════════════════════════
// CLASSE 6: AdvancedTab
// ═══════════════════════════════════════════════════════════════
class AdvancedTab : public QWidget
{
    Q_OBJECT

public:
    explicit AdvancedTab(QWidget *parent = nullptr);

private slots:
    void calculerRentabilite();
    void detecterAnomalies();
    void predire();
    void classerClients();
    void simuler();

private:
    void initializeUI();
    void setupConnections();

    // Rentabilité
    QComboBox *cbMonthRent;
    QPushButton *btnCalcRent;
    QLabel *lblRent;

    // Dépenses Anormales
    QDoubleSpinBox *spinSeuil;
    QPushButton *btnDetect;
    QTableWidget *tableAnomaly;

    // Prévision
    QComboBox *cbPeriod;
    QPushButton *btnPredict;
    QWidget *chartPredict;

    // Classement Clients
    QComboBox *cbRank;
    QTableWidget *tableClients;

    // Simulation
    QDoubleSpinBox *spinPrice;
    QDoubleSpinBox *spinCharge;
    QDoubleSpinBox *spinSales;
    QPushButton *btnSimulate;
    QLabel *lblResult;
};

// ═══════════════════════════════════════════════════════════════
// CLASSE 7: ExportTab
// ═══════════════════════════════════════════════════════════════
class ExportTab : public QWidget
{
    Q_OBJECT

public:
    explicit ExportTab(QWidget *parent = nullptr);

private slots:
    void exportPDF();
    void exportExcel();
    void exportWord();
    void exportTxt();

private:
    void initializeUI();
    void setupConnections();

    // Export
    QComboBox *cbFormat;
    QDateEdit *dateExportFrom;
    QDateEdit *dateExportTo;
    QPushButton *btnExport;
    QLabel *lblStatus;
};

// ═══════════════════════════════════════════════════════════════
// CLASSE PRINCIPALE: Finance
// ═══════════════════════════════════════════════════════════════
class Finance : public QMainWindow
{
    Q_OBJECT

public:
    Finance(QWidget *parent = nullptr);
    ~Finance();

signals:
    void backToMenu();

protected:
    void closeEvent(QCloseEvent *event) override {
        emit backToMenu();
        QMainWindow::closeEvent(event);
    }

private:
    void initializeUI();
    void applyStyles();

    Ui::Finance *ui;
    QTabWidget *mainTabWidget;

    // Tabs
    TransactionTab *transactionTab;
    SearchTab *searchTab;
    StatsTab *statsTab;
    JournalTab *journalTab;
    BudgetTab *budgetTab;
    AdvancedTab *advancedTab;
    ExportTab *exportTab;
};

#endif // FINANCE_H
