#ifndef HISTORIQUEAUDIT_H
#define HISTORIQUEAUDIT_H

#include <QWidget>
#include <QTableWidget>
#include <QComboBox>
#include <QCheckBox>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QDateEdit>
#include <QListWidget>
#include <QSpinBox>
#include <QProgressBar>
#include <QSplitter>
#include <QTabWidget>
#include <QTextEdit>
#include <QFrame>
#include <QTimer>
#include <QHash>
#include <QKeySequence>
#include <QShortcut>
#include <QtSql>

// QtCharts
#include <QtCharts/QChartView>
#include <QtCharts/QPieSeries>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QLineSeries>
#include <QtCharts/QAreaSeries>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QValueAxis>
#include <QtCharts/QDateTimeAxis>
#include <QtCharts/QChart>

// Qt5 : QtCharts est un namespace ; Qt6 : classes directement accessibles
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
using namespace QtCharts;
#endif

class ConnexionFinance;

// ─── Enregistrement d'audit ───────────────────────────────────────────────────
struct AuditRecord {
    int     auditId = 0;
    int     transactionId = 0;
    QString operationType;
    QString action;
    QString fieldName;
    QString oldValue;
    QString newValue;
    int     changedBy = 0;
    QString changedAt;
    int     version = 0;
    QString severity;
    double  impactScore = 0.0;
    bool    isAnomaly = false;
};

// ─── Statistiques d'audit ─────────────────────────────────────────────────────
struct AuditStats {
    int    totalActions   = 0;
    int    totalInserts   = 0;
    int    totalUpdates   = 0;
    int    totalDeletes   = 0;
    int    totalAnomalies = 0;
    int    totalCritical  = 0;
    double avgImpact      = 0.0;
};

// ─── Classe principale ────────────────────────────────────────────────────────
class HistoriqueAuditTab : public QWidget
{
    Q_OBJECT

public:
    explicit HistoriqueAuditTab(QWidget *parent = nullptr);
    ~HistoriqueAuditTab();

    void setDarkMode(bool isDark);
    void refreshData();
    AuditStats getStats() const;

    bool m_isDarkMode = false;
    QString m_originalStyle;

private slots:
    // ── Chargement & filtrage ─────────────────────────────────────────────────
    void chargerHistorique();
    void chargerListeUtilisateurs(); // charge les utilisateurs dans cbUserFilter
    void filtrerParAction();
    void filtrerParSeverite();
    void filtrerParAnomalie();
    void filtrerParPeriode();
    void rechercherTransaction();
    void viderFiltres();
    void mettreAJourBoutonActif();
    void mettreAJourTimeline();

    // ── Actions ───────────────────────────────────────────────────────────────
    void onAuditRowSelected(int row, int col);
    void comparerValeurs();
    void restaurerValeur();
    void afficherDetailComplet();

    // ── Export ────────────────────────────────────────────────────────────────
    void exporterPDF();
    void exporterCSV();

    // ── Intelligence ──────────────────────────────────────────────────────────
    void mettreAJourStats();
    void detecterAnomalies();

    // ── Nouvelles fonctionnalités ─────────────────────────────────────────────
    void verifierNouvellesAlertes();
    void copierLignePressePapier();
    void onHeaderContextMenu(const QPoint &pos);
    void rechercheRapide(const QString &text);
    void mettreAJourGraphiques();

private:
    // ── Construction UI ───────────────────────────────────────────────────────
    void     initializeUI();
    void     setupConnections();
    void     appliquerStyle();
    void     setupRaccourcis();

    // ── Onglets principaux ────────────────────────────────────────────────────
    QWidget* creerOngletJournal();
    QWidget* creerOngletAnalytique();
    QWidget* creerOngletTimeline();

    // ── Sous-widgets de l'onglet Journal ─────────────────────────────────────
    QWidget* creerBandeauStats();
    QWidget* creerPanneauFiltres();
    QWidget* creerTableauAudit();
    QWidget* creerPanneauDetail();
    QWidget* creerBarreActions();

    // ── Métier ────────────────────────────────────────────────────────────────
    void    remplirTableauAudit(QSqlQuery &query);
    void    mettreAJourDetail(int row);
    void    mettreAJourCartesStat(const AuditStats &stats);
    void    colorierLigne(int row, const QString &severity, bool isAnomaly);
    bool    confirmerRestauration(const AuditRecord &rec);
    QString construireRequete() const;
    bool    filtreEstActif() const;
    void    chargerNotes();
    void    chargerStatutsRevision();

    // ── Helpers visuels ───────────────────────────────────────────────────────
    QString getSeveriteColor(const QString &severity) const;
    QString getSeveriteTextColor(const QString &severity) const;
    QString getActionIcon(const QString &action) const;
    QString getActionColor(const QString &action) const;
    QString formaterMontant(const QString &valeur) const;
    QColor  couleurImpact(double score) const;
    QString calculerVariation(const QString &oldVal, const QString &newVal) const;
    QString normaliseDateISO(const QString &changedAt) const;
    QString extraireHeure(const QString &changedAt) const;

    // ── Helpers export ────────────────────────────────────────────────────────
    QString genererHTMLRapport() const;
    void    ecrireCSV(const QString &chemin) const;

    // ── Graphiques (QtCharts) ─────────────────────────────────────────────────
    QChartView *chartViewPie;
    QChartView *chartViewBar;
    QChartView *chartViewLine;
    QChartView *chartViewImpact;
    void        construireGraphiquePie();
    void        construireGraphiqueBar();
    void        construireGraphiqueLine();
    void        construireGraphiqueImpact();

    // ── Contrôles filtre graphique ligne ──────────────────────────────────────
    QComboBox   *cbLineFilterPeriod;   // 7j / 15j / 30j / 90j
    QComboBox   *cbLineFilterType;     // Tout / Anomalies / Critiques
    QPushButton *btnLineRefresh;       // Bouton Refresh graphique ligne
    QLabel      *lblLineLastUpdate;    // Horodatage dernier refresh

    // ════════════════════════════════════════════════════════════════════════
    // WIDGETS — onglet principal
    // ════════════════════════════════════════════════════════════════════════
    QTabWidget *mainTabs;

    // ════════════════════════════════════════════════════════════════════════
    // WIDGETS — bandeau statistiques
    // ════════════════════════════════════════════════════════════════════════
    QLabel       *lblStatTotal;
    QLabel       *lblStatInserts;
    QLabel       *lblStatUpdates;
    QLabel       *lblStatDeletes;
    QLabel       *lblStatAnomalies;
    QLabel       *lblStatCritical;
    QLabel       *lblStatImpactMoyen;
    QProgressBar *barAnomalieRatio;

    // ════════════════════════════════════════════════════════════════════════
    // WIDGETS — filtres
    // ════════════════════════════════════════════════════════════════════════
    QComboBox   *cbActionFilter;
    QComboBox   *cbSeverityFilter;
    QCheckBox   *chkAnomaliesOnly;
    QCheckBox   *chkNonRevise;          // NOUVEAU : filtre non révisé
    QLineEdit   *txtSearchTransaction;
    QLineEdit   *txtSearchLibre;
    QPushButton *btnSearch;             // NOUVEAU : bouton Rechercher dédié
    QLineEdit   *txtRechercheRapide;    // NOUVEAU : barre Ctrl+F
    QComboBox   *cbUserFilter;
    QDateEdit   *dtDebut;
    QDateEdit   *dtFin;
    QCheckBox   *chkFiltreDate;
    QPushButton *btnRefresh;
    QPushButton *btnViderFiltres;
    QLabel      *lblFiltreActif;

    // ════════════════════════════════════════════════════════════════════════
    // WIDGETS — tableau + statut
    // ════════════════════════════════════════════════════════════════════════
    QTableWidget *tableAudit;
    QLabel       *lblNbResultats;
    QLabel       *lblStatutTableau;

    // ════════════════════════════════════════════════════════════════════════
    // WIDGETS — panneau détail amélioré
    // ════════════════════════════════════════════════════════════════════════
    QLabel    *lblDetailAuditId;
    QLabel    *lblDetailTransId;
    QLabel    *lblDetailAction;
    QLabel    *lblDetailChamp;
    QLabel    *lblOldValue;
    QLabel    *lblNewValue;
    QLabel    *lblVariation;            // NOUVEAU : % variation
    QLabel    *lblImpactScore;
    QLabel    *lblAnomalyStatus;
    QLabel    *lblDetailSeverite;
    QLabel    *lblDetailVersion;
    QLabel    *lblDetailUser;
    QLabel    *lblDetailDate;
    QLabel    *lblStatutRevision;       // NOUVEAU : statut révision
    QTextEdit *txtDetailNote;
    QFrame    *frameDetail;

    // ════════════════════════════════════════════════════════════════════════
    // WIDGETS — barre d'actions
    // ════════════════════════════════════════════════════════════════════════
    QPushButton *btnComparison;
    QPushButton *btnRestore;
    QPushButton *btnExportPDF;
    QPushButton *btnExportCSV;
    QPushButton *btnDetectAnomalies;
    QPushButton *btnDetailComplet;

    // ════════════════════════════════════════════════════════════════════════
    // ÉTAT INTERNE
    // ════════════════════════════════════════════════════════════════════════
    int                currentSelectedRow;
    ConnexionFinance  *dbConn = nullptr;
    QTimer            *timerAnimation;
    QTimer            *timerAlerte;         // NOUVEAU : alertes temps réel
    QList<AuditRecord> cachedRecords;
    bool               isInitializing;
    int                dernierAuditIdConnu; // NOUVEAU : pour détecter nouveaux enregs
    QHash<int,QString> notesMap;            // NOUVEAU : notes par audit_id
    QHash<int,QString> reviewMap;           // NOUVEAU : statuts révision
    QListWidget        *timelineList;       // NOUVEAU : liste timeline
    QSpinBox           *timelineLimitSpin;  // NOUVEAU : limite timeline
};

#endif // HISTORIQUEAUDIT_H

