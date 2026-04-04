#ifndef FINANCE_H
#define FINANCE_H

// ========================================================================
// INCLUDES
// ========================================================================
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
#include <QFormLayout>
#include <QtSql>
#include <QShowEvent>

// QtCharts
#include <QtCharts/QChartView>
#include <QtCharts/QPieSeries>
#include <QtCharts/QPieSlice>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QLegend>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QValueAxis>
#include <QtCharts/QLineSeries>

QT_USE_NAMESPACE

class ConnexionFinance; // Forward declaration pour éviter les conflits avec d'autres modules

#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QTimer>
#include <QPrinter>
#include <QPrintDialog>
#include <QTextDocument>
#include <QStandardPaths>
#include <QDesktopServices>
#include <QUrl>
#include <QDir>

// ========================================================================
// FORWARD DECLARATIONS
// ========================================================================
QT_BEGIN_NAMESPACE
namespace Ui { class Finance; }
QT_END_NAMESPACE

// ========================================================================
// CLASSE 1: TransactionTab (Style GESTION STOCK)
// ========================================================================
class TransactionTab : public QWidget
{
    Q_OBJECT

public:
    explicit TransactionTab(int userId = 1, QWidget *parent = nullptr);
    ~TransactionTab();

signals:
    void dataChanged();

private slots:
    void ajouterTransaction();
    void modifierTransaction();
    void supprimerTransaction();
    void chargerTransactions();
    void effacerFormulaire();
    void chargerData();
    void onTypeChanged(const QString &type);
    void rechercher();
    void trier();
    void exporter(const QString &format);

private:
    void initializeUI();
    void setupConnections();

    // Widgets - Zone Formulaire
    QLineEdit *txtId;
    QComboBox *cbType;
    QComboBox *cbCategorie;
    QDoubleSpinBox *spinMontant;
    QDateEdit *dateEdit;
    
    QLabel *lblEmployee;  // Afficher l'employee qui enregistre
    QLineEdit *txtEmployee;
    
    QLabel *lblMachine;
    QComboBox *cbMachine;
    
    QTextEdit *txtDescription;

    // Boutons Actions
    QPushButton *btnAdd;
    QPushButton *btnUpdate;
    QPushButton *btnDelete;
    QPushButton *btnConsult;

    // Widgets - Zone Tableau
    QTableWidget *tableTransaction;

    // Widgets - Zone Recherche (Côté droit)
    QComboBox *cbSearchType;
    QComboBox *cbSearchCategory;
    QPushButton *btnSearch;
    QComboBox *cbSortBy;
    QPushButton *btnSort;
    QPushButton *btnExportPDF;
    QPushButton *btnExportExcel;
    QPushButton *btnExportTXT;

    ConnexionFinance *dbConn;
    int currentUserId;
};

// ========================================================================
// CLASSE 2: SearchTab (Recherche, Tri & Export)
// ========================================================================
class SearchTab : public QWidget
{
    Q_OBJECT

public:
    explicit SearchTab(QWidget *parent = nullptr);

public slots:
    void chargerTransactions();

private slots:
    void rechercher();
    void trier();
    void exportTransaction();

private:
    void initializeUI();
    void setupConnections();
    
    // Filtres
    QLineEdit *txtSearch;
    QComboBox *cbSearchType;
    QDateEdit *dateFrom;
    QDateEdit *dateTo;
    QPushButton *btnSearch;

    // Tri
    QComboBox *cbSortBy;
    QComboBox *cbOrder;
    QPushButton *btnSort;

    // Export
    QComboBox *cbExportFormat;
    QPushButton *btnExport;
    QLabel *lblExportStatus;

    // Tableau de visualisation (Lecture seule)
    QTableWidget *tableSearch;

    // Logic
    void exportPDF();
    void exportCSV();
    void exportExcel();
    void exportWord();

    ConnexionFinance *dbConn;
};

// ========================================================================
// CLASSE 3: StatsTab
// ========================================================================
class StatsTab : public QWidget
{
    Q_OBJECT

public:
    explicit StatsTab(QWidget *parent = nullptr);
    void setDarkMode(bool isDark);

public slots:
    void calculerStats();
    void afficherGraphique();

protected:
    void showEvent(QShowEvent *event) override;

private:
    void initializeUI();
    void setupConnections();
    void applyDarkModeStyles();
    QGroupBox* createSummarySection();
    QGroupBox* createChartControlSection();
    QGroupBox* createChartSection();

    // Résumé
    QLabel *lblTotalRevenus;
    QLabel *lblTotalDepenses;
    QLabel *lblBenefice;

    // Dark mode
    bool m_isDarkMode{false};
    QGroupBox *summaryGroup = nullptr;
    QGroupBox *controlGroup = nullptr;
    QGroupBox *chartGroup = nullptr;
    QWidget *revCard = nullptr;
    QWidget *depCard = nullptr;
    QWidget *benCard = nullptr;
    QLabel *lblRevenuTitle = nullptr;
    QLabel *lblDepenseTitle = nullptr;
    QLabel *lblBeneficeTitle = nullptr;

    // Graphiques
    QComboBox *cbChartType;
    QChartView *chartView;
    QPushButton *btnGenerate;

    ConnexionFinance *dbConn;
};

// ========================================================================
// CLASSE 4: AdvancedTab
// ========================================================================
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
    QChartView *chartPredict;

    // Classement Clients
    QComboBox *cbRank;
    QTableWidget *tableClients;

    // Simulation
    QDoubleSpinBox *spinPrice;
    QDoubleSpinBox *spinCharge;
    QDoubleSpinBox *spinSales;
    QPushButton *btnSimulate;
    QLabel *lblResult;

    ConnexionFinance *dbConn;
};

// ========================================================================
// CLASSE PRINCIPALE: Finance
// ========================================================================
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

private slots:
    void updateDateTime();
    void onToggleDarkMode();

private:
    void initializeUI();
    void applyStyles();
    void applyTheme();

    Ui::Finance *ui;
    QTabWidget *mainTabWidget;

    // Tabs
    TransactionTab *transactionTab;
    StatsTab *statsTab;
    AdvancedTab *advancedTab;

    // DateTime
    QTimer *dateTimeTimer;
    QLabel *lblDateTime;

    // Dark Mode
    bool m_isDarkMode;
    QString m_lightStyleSheet;
    QPushButton *btnDarkMode;
};

#endif // FINANCE_H
