#ifndef ADVANCEDTAB_H
#define ADVANCEDTAB_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QProgressBar>
#include <QComboBox>
#include <QChartView>
#include <QChart>
#include <QLineSeries>
#include <QBarSeries>
#include <QBarSet>
#include <QBarCategoryAxis>
#include <QValueAxis>
#include <QDateTimeAxis>
#include <QTableWidget>
#include <QTimer>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QFrame>
#include <QSplitter>
#include <QToolTip>
#include <QCursor>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

#include "connexionfinance.h"

// ─── Données de prévision mensuelle ──────────────────────────────────────────
struct PrevisionMois {
    QString label;      // "2025-03"
    double  revenu   = 0;
    double  depense  = 0;
    double  cashFlow = 0;
    bool    estFutur = false;
};

// ─── Résultat de régression linéaire ─────────────────────────────────────────
struct RegressionResult {
    double slope     = 0;
    double intercept = 0;
    double r2        = 0;
    bool   valid     = false;
};

// ─── KPI enrichi pour le bandeau santé ───────────────────────────────────────
struct FinanceKPI {
    double revTotal    = 0;
    double depTotal    = 0;
    double tresorerie  = 0;
    double marge       = 0;
    double ratioDepRev = 0;
    double croissance  = 0;
    double meilleurMois= 0;
    double pireDepense = 0;
    int    score       = 0;
};

// ═════════════════════════════════════════════════════════════════════════════
class AdvancedTab : public QWidget
{
    Q_OBJECT

public:
    explicit AdvancedTab(QWidget *parent = nullptr);
    ~AdvancedTab();

    void rafraichir();
    void setDarkMode(bool isDark);

private slots:
    void predire();
    void onPeriodChanged();
    void exporterPrevisionCSV();
    void exporterGraphiquePDF();
    void calculerSanteFinanciere();
    void onAlgorithmChanged();

private:
    // ── Construction UI ───────────────────────────────────────────────────────
    void     initializeUI();
    void     appliquerStyle();
    void     setupConnections();

    QWidget* creerBandeauKPI();
    QWidget* creerSectionSante();
    QWidget* creerSectionPrevision();
    QWidget* creerSectionTableauDetail();
    QWidget* creerSectionAlertes();

    QFrame*  creerCarteKPI(const QString &titre, QLabel *&valLabel,
                            QLabel *&tendLabel, const QString &couleur);

    // ── Algorithmes ───────────────────────────────────────────────────────────
    RegressionResult calculerRegression(const QList<double> &values) const;
    double           moyenneMobilesPonderee(const QList<double> &v, int f = 3) const;
    QList<double>    lisserDonnees(const QList<double> &raw, int f = 3) const;

    // ── Métier ────────────────────────────────────────────────────────────────
    FinanceKPI           calculerKPI() const;
    QList<PrevisionMois> chargerHistoriqueMensuel() const;
    QList<PrevisionMois> genererPrevisions(const QList<PrevisionMois> &hist,
                                           int nbMois) const;
    void mettreAJourBandeauKPI(const FinanceKPI &kpi);
    void mettreAJourGraphique(const QList<PrevisionMois> &hist,
                              const QList<PrevisionMois> &prev);
    void mettreAJourTableauDetail(const QList<PrevisionMois> &hist,
                                  const QList<PrevisionMois> &prev);
    void mettreAJourAlertes(const QList<PrevisionMois> &prev, double slope);
    void mettreAJourSummary(double lastPredVal, double slope,
                            double r2, int nbMois);

    // ── Helpers ───────────────────────────────────────────────────────────────
    QString formatDT(double val) const;
    QString formatPct(double val) const;
    QString couleurScore(int score) const;
    void    animerScore(int cible);

    // ════════════════════════════════════════════════════════════════════════
    // WIDGETS — Bandeau KPI (NOUVEAU)
    // ════════════════════════════════════════════════════════════════════════
    QLabel *lblKpiRevenu;       QLabel *lblKpiRevenuTend;
    QLabel *lblKpiDepense;      QLabel *lblKpiDepenseTend;
    QLabel *lblKpiCashFlow;     QLabel *lblKpiCashFlowTend;
    QLabel *lblKpiMarge;        QLabel *lblKpiMargeTend;
    QLabel *lblKpiCroissance;   QLabel *lblKpiCroissanceTend;

    // ════════════════════════════════════════════════════════════════════════
    // WIDGETS — Section Santé
    // ════════════════════════════════════════════════════════════════════════
    QLabel       *lblHealthScoreText;
    QProgressBar *barHealthScore;
    QLabel       *lblHealthMessage;
    QLabel       *lblRatioMarge;
    QLabel       *lblRatioLiquidite;
    QLabel       *lblRatioDependance;
    QLabel       *lblRatioDepenseRev;
    QLabel       *lblRatioMeilleurMois;
    QChartView   *miniChartView;    // NOUVEAU : mini graphique historique

    // ════════════════════════════════════════════════════════════════════════
    // WIDGETS — Section Prévision
    // ════════════════════════════════════════════════════════════════════════
    QComboBox    *cbPeriod;
    QComboBox    *cbAlgorithm;
    QPushButton  *btnPredict;
    QPushButton  *btnExportCSV;
    QPushButton  *btnExportPDF;
    QChartView   *chartViewPredict;
    QLabel       *lblPeriodInfo;
    QLabel       *lblConfiance;
    QLabel       *lblPredictSummary;

    // ════════════════════════════════════════════════════════════════════════
    // WIDGETS — Tableau + Alertes
    // ════════════════════════════════════════════════════════════════════════
    QTableWidget *tableauPrevisions;
    QLabel       *lblAlerte1;
    QLabel       *lblAlerte2;
    QLabel       *lblAlerte3;

    // ════════════════════════════════════════════════════════════════════════
    // ÉTAT INTERNE
    // ════════════════════════════════════════════════════════════════════════
    ConnexionFinance     *dbConn;
    bool                  m_isDarkMode = false;
    QList<PrevisionMois>  dernierHistorique;
    QList<PrevisionMois>  dernieresPrevisions;
    QTimer               *timerScore;
    int                   scoreActuel = 0;
    int                   scoreCible  = 0;
};

#endif // ADVANCEDTAB_H