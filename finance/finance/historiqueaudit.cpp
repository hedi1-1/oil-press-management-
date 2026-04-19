#include "historiqueaudit.h"
#include "connexionfinance.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QSplitter>
#include <QScrollArea>
#include <QMessageBox>
#include <QFileDialog>
#include <QTextDocument>
#include <QPrinter>
#include <QPageSize>
#include <QPageLayout>
#include <QDesktopServices>
#include <QStandardPaths>
#include <QDateTime>
#include <QFile>
#include <QTextStream>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QHeaderView>
#include <QDebug>
#include <QClipboard>
#include <QApplication>
#include <QMenu>
#include <QShortcut>
#include <QListWidget>
#include <QSpinBox>

// QtCharts
#include <QtCharts/QChartView>
#include <QtCharts/QPieSeries>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QLineSeries>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QValueAxis>
#include <QtCharts/QDateTimeAxis>
#include <QtCharts/QChart>
// Qt5 : QtCharts est un namespace ; Qt6 : classes directement accessibles
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
using namespace QtCharts;
#endif

// ═══════════════════════════════════════════════════════════════════════════════
//  CONSTRUCTEUR / DESTRUCTEUR
// ═══════════════════════════════════════════════════════════════════════════════

HistoriqueAuditTab::HistoriqueAuditTab(QWidget *parent)
    : QWidget(parent)
    , currentSelectedRow(-1)
    , dbConn(nullptr)
    , timerAnimation(nullptr)
    , timerAlerte(nullptr)
    , dernierAuditIdConnu(0)
    , lblStatTotal(nullptr)
    , lblStatInserts(nullptr)
    , lblStatUpdates(nullptr)
    , lblStatDeletes(nullptr)
    , lblStatAnomalies(nullptr)
    , lblStatCritical(nullptr)
    , lblStatImpactMoyen(nullptr)
    , barAnomalieRatio(nullptr)
    , chartViewPie(nullptr)
    , chartViewBar(nullptr)
    , chartViewLine(nullptr)
    , chartViewImpact(nullptr)
    , timelineList(nullptr)
    , timelineLimitSpin(nullptr)
    , isInitializing(true)
{
    dbConn = new ConnexionFinance();
    if (!dbConn->getDatabase().isOpen())
        dbConn->open();

    initializeUI();
    appliquerStyle();
    setupConnections();
    setupRaccourcis();

    // Alertes temps réel toutes les 30 secondes
    timerAlerte = new QTimer(this);
    timerAlerte->setInterval(30000);
    connect(timerAlerte, &QTimer::timeout, this, &HistoriqueAuditTab::verifierNouvellesAlertes);
    timerAlerte->start();

    isInitializing = false;

    QTimer::singleShot(200, this, &HistoriqueAuditTab::chargerHistorique);
    QTimer::singleShot(300, this, &HistoriqueAuditTab::chargerListeUtilisateurs);
    QTimer::singleShot(500, this, &HistoriqueAuditTab::chargerNotes);
    QTimer::singleShot(600, this, &HistoriqueAuditTab::chargerStatutsRevision);
}

HistoriqueAuditTab::~HistoriqueAuditTab()
{
    delete dbConn;
}

// ═══════════════════════════════════════════════════════════════════════════════
//  CONSTRUCTION UI
// ═══════════════════════════════════════════════════════════════════════════════

void HistoriqueAuditTab::initializeUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(6, 6, 6, 6);

    mainTabs = new QTabWidget(this);
    mainTabs->setObjectName("mainAuditTabs");
    mainTabs->addTab(creerOngletJournal(),    "   Journal d'Audit   ");
    mainTabs->addTab(creerOngletAnalytique(), "   Analytique   ");
    mainTabs->addTab(creerOngletTimeline(),   "   Timeline   ");

    connect(mainTabs, &QTabWidget::currentChanged, this, [this](int idx){
        if (!isInitializing) {
            if (idx == 1) mettreAJourGraphiques();
            if (idx == 2) mettreAJourTimeline();
        }
    });

    mainLayout->addWidget(mainTabs);
}

// ─── Onglet Journal (contenu original) ───────────────────────────────────────────────────
QWidget* HistoriqueAuditTab::creerOngletJournal()
{
    QWidget *w = new QWidget();
    QVBoxLayout *lay = new QVBoxLayout(w);
    lay->setSpacing(8);
    lay->setContentsMargins(8, 8, 8, 8);

    lay->addWidget(creerBandeauStats());
    lay->addWidget(creerPanneauFiltres());

    QSplitter *splitter = new QSplitter(Qt::Horizontal, w);
    splitter->addWidget(creerTableauAudit());
    splitter->addWidget(creerPanneauDetail());
    splitter->setSizes({720, 300});
    splitter->setChildrenCollapsible(false);
    lay->addWidget(splitter, 1);

    lay->addWidget(creerBarreActions());
    return w;
}

// ─── Bandeau statistiques ─────────────────────────────────────────────────────

QWidget* HistoriqueAuditTab::creerBandeauStats()
{
    QWidget *w = new QWidget(this);
    w->setObjectName("statsBar");
    QHBoxLayout *lay = new QHBoxLayout(w);
    lay->setContentsMargins(6, 6, 6, 6);
    lay->setSpacing(8);

    auto makeCard = [&](const QString &icon, const QString &label,
                        QLabel *&valueLabel,
                        const QString &objName) -> QFrame*
    {
        QFrame *card = new QFrame();
        card->setObjectName(objName);
        QVBoxLayout *cl = new QVBoxLayout(card);
        cl->setContentsMargins(10, 6, 10, 6);
        cl->setSpacing(2);
        QLabel *ico = new QLabel(icon + "  " + label);
        ico->setObjectName("statLabel");
        valueLabel = new QLabel("0");
        valueLabel->setObjectName("statValue");
        valueLabel->setAlignment(Qt::AlignCenter);
        cl->addWidget(ico);
        cl->addWidget(valueLabel);
        return card;
    };

    lay->addWidget(makeCard("*",  "Total",        lblStatTotal,     "cardTotal"));
    lay->addWidget(makeCard("+",  "Insertions",   lblStatInserts,   "cardInsert"));
    lay->addWidget(makeCard("~",  "Modifications",lblStatUpdates,   "cardUpdate"));
    lay->addWidget(makeCard("x",  "Suppressions", lblStatDeletes,   "cardDelete"));
    lay->addWidget(makeCard("!",  "Anomalies",    lblStatAnomalies, "cardAnomaly"));
    lay->addWidget(makeCard("!!", "Critiques",    lblStatCritical,  "cardCritical"));

    QFrame *ratioCard = new QFrame();
    ratioCard->setObjectName("cardRatio");
    QVBoxLayout *rl = new QVBoxLayout(ratioCard);
    rl->setContentsMargins(10, 6, 10, 6);
    rl->setSpacing(4);
    QLabel *rlbl = new QLabel("Ratio Anomalies");
    rlbl->setObjectName("statLabel");
    barAnomalieRatio = new QProgressBar();
    barAnomalieRatio->setRange(0, 100);
    barAnomalieRatio->setValue(0);
    barAnomalieRatio->setTextVisible(true);
    barAnomalieRatio->setFormat("%v%");
    barAnomalieRatio->setFixedHeight(14);
    rl->addWidget(rlbl);
    rl->addWidget(barAnomalieRatio);
    lay->addWidget(ratioCard);

    lblStatImpactMoyen = new QLabel("0");

    return w;
}

// ─── Panneau filtres ──────────────────────────────────────────────────────────

QWidget* HistoriqueAuditTab::creerPanneauFiltres()
{
    QGroupBox *grp = new QGroupBox("  Filtres de recherche", this);
    grp->setObjectName("filterGroup");

    // Deux lignes de filtres
    QVBoxLayout *vlay = new QVBoxLayout(grp);
    vlay->setSpacing(6);

    // ── Ligne 1 ──────────────────────────────────────────────────────────────
    QHBoxLayout *lay1 = new QHBoxLayout();
    lay1->setSpacing(10);

    QLabel *lbA = new QLabel("Action :");
    lbA->setObjectName("filterLabel");
    cbActionFilter = new QComboBox();
    // IMPORTANT : texte FR + data EN pour que la requete SQL fonctionne
    cbActionFilter->addItem("Toutes les actions",  QString(""));
    cbActionFilter->addItem("Insertion",           QString("INSERT"));
    cbActionFilter->addItem("Modification",        QString("UPDATE"));
    cbActionFilter->addItem("Suppression",         QString("DELETE"));
    cbActionFilter->addItem("Restauration",        QString("ROLLBACK"));
    cbActionFilter->addItem("Alerte",              QString("ALERTE"));
    cbActionFilter->setObjectName("filterCombo");

    QLabel *lbS = new QLabel("Severite :");
    lbS->setObjectName("filterLabel");
    cbSeverityFilter = new QComboBox();
    cbSeverityFilter->addItem("Toutes les severites", QString(""));
    cbSeverityFilter->addItem("Critique",             QString("CRITICAL"));
    cbSeverityFilter->addItem("Elevee",               QString("HIGH"));
    cbSeverityFilter->addItem("Moyenne",              QString("MEDIUM"));
    cbSeverityFilter->addItem("Faible",               QString("LOW"));
    cbSeverityFilter->setObjectName("filterCombo");

    // Filtre utilisateur
    QLabel *lbU = new QLabel("Utilisateur :");
    lbU->setObjectName("filterLabel");
    cbUserFilter = new QComboBox();
    cbUserFilter->addItem("Tous les utilisateurs", -1);
    cbUserFilter->setObjectName("filterCombo");
    cbUserFilter->setMinimumWidth(90);

    chkAnomaliesOnly = new QCheckBox("Anomalies uniquement");
    chkAnomaliesOnly->setObjectName("filterCheck");

    // NOUVEAU : indicateur filtre actif
    lblFiltreActif = new QLabel("");
    lblFiltreActif->setObjectName("filtreActifLabel");
    lblFiltreActif->setVisible(false);

    lay1->addWidget(lbA);
    lay1->addWidget(cbActionFilter);
    lay1->addWidget(lbS);
    lay1->addWidget(cbSeverityFilter);
    lay1->addWidget(lbU);
    lay1->addWidget(cbUserFilter);
    lay1->addWidget(chkAnomaliesOnly);
    lay1->addStretch();
    lay1->addWidget(lblFiltreActif);

    // ── Ligne 2 ──────────────────────────────────────────────────────────────
    QHBoxLayout *lay2 = new QHBoxLayout();
    lay2->setSpacing(10);

    // Recherche par ID transaction
    txtSearchTransaction = new QLineEdit();
    txtSearchTransaction->setPlaceholderText("ID transaction...");
    txtSearchTransaction->setObjectName("filterSearch");
    txtSearchTransaction->setFixedWidth(130);

    // NOUVEAU : recherche texte libre (champ, valeur...)
    txtSearchLibre = new QLineEdit();
    txtSearchLibre->setPlaceholderText("Recherche dans champ / valeurs...");
    txtSearchLibre->setObjectName("filterSearch");
    txtSearchLibre->setMinimumWidth(200);

    // Bouton de recherche explicite
    btnSearch = new QPushButton("  Rechercher");
    btnSearch->setObjectName("btnAction");
    btnSearch->setCursor(Qt::PointingHandCursor);

    chkFiltreDate = new QCheckBox("Periode :");
    chkFiltreDate->setObjectName("filterCheck");

    dtDebut = new QDateEdit(QDate::currentDate().addMonths(-1));
    dtDebut->setCalendarPopup(true);
    dtDebut->setDisplayFormat("dd/MM/yyyy");
    dtDebut->setObjectName("filterDate");
    dtDebut->setEnabled(false);  // désactivé par défaut

    dtFin = new QDateEdit(QDate::currentDate());
    dtFin->setCalendarPopup(true);
    dtFin->setDisplayFormat("dd/MM/yyyy");
    dtFin->setObjectName("filterDate");
    dtFin->setEnabled(false);  // désactivé par défaut

    lblNbResultats = new QLabel("0 resultat(s)");
    lblNbResultats->setObjectName("resultCount");

    btnViderFiltres = new QPushButton("Effacer les filtres");
    btnViderFiltres->setObjectName("btnSecondary");

    btnRefresh = new QPushButton("Actualiser");
    btnRefresh->setObjectName("btnPrimary");

    lay2->addWidget(new QLabel("ID Trans. :"));
    lay2->addWidget(txtSearchTransaction);
    lay2->addWidget(txtSearchLibre);
    lay2->addWidget(btnSearch);
    lay2->addWidget(chkFiltreDate);
    lay2->addWidget(dtDebut);
    lay2->addWidget(new QLabel("au"));
    lay2->addWidget(dtFin);
    lay2->addStretch();
    lay2->addWidget(lblNbResultats);
    lay2->addWidget(btnViderFiltres);
    lay2->addWidget(btnRefresh);

    vlay->addLayout(lay1);
    vlay->addLayout(lay2);

    return grp;
}

// ─── Tableau principal ────────────────────────────────────────────────────────

QWidget* HistoriqueAuditTab::creerTableauAudit()
{
    QGroupBox *grp = new QGroupBox("  Journal d'audit complet", this);
    grp->setObjectName("tableGroup");
    QVBoxLayout *lay = new QVBoxLayout(grp);
    lay->setContentsMargins(6, 8, 6, 4);
    lay->setSpacing(4);

    tableAudit = new QTableWidget();
    tableAudit->setObjectName("auditTable");
    tableAudit->setColumnCount(13);
    tableAudit->setHorizontalHeaderLabels({
        "ID", "Trans.", "Action", "Type Op.", "Champ",
        "Ancienne valeur", "Nouvelle valeur",
        "Utilisateur", "Date / Heure",
        "Severite", "Version", "Anomalie", "Impact %"
    });

    tableAudit->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableAudit->setSelectionMode(QAbstractItemView::ExtendedSelection);
    tableAudit->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableAudit->setAlternatingRowColors(true);
    tableAudit->setSortingEnabled(true);
    tableAudit->setWordWrap(false);
    tableAudit->verticalHeader()->setVisible(false);
    tableAudit->setShowGrid(true);
    tableAudit->setGridStyle(Qt::DotLine);

    QHeaderView *hv = tableAudit->horizontalHeader();
    hv->setSectionResizeMode(QHeaderView::Interactive);
    hv->setStretchLastSection(false);

    tableAudit->setColumnWidth(0,  50);
    tableAudit->setColumnWidth(1,  55);
    tableAudit->setColumnWidth(2,  95);
    tableAudit->setColumnWidth(3,  110);  // élargi (était tronqué)
    tableAudit->setColumnWidth(4,  90);
    tableAudit->setColumnWidth(5,  130);
    tableAudit->setColumnWidth(6,  130);
    tableAudit->setColumnWidth(7,  80);
    tableAudit->setColumnWidth(8,  130);
    tableAudit->setColumnWidth(9,  85);
    tableAudit->setColumnWidth(10, 60);
    tableAudit->setColumnWidth(11, 75);
    tableAudit->setColumnWidth(12, 70);

    lay->addWidget(tableAudit);

    // NOUVEAU : barre de statut sous le tableau
    lblStatutTableau = new QLabel("Pret.");
    lblStatutTableau->setObjectName("statutTableau");
    lay->addWidget(lblStatutTableau);

    return grp;
}

// ─── Panneau détail ───────────────────────────────────────────────────────────

QWidget* HistoriqueAuditTab::creerPanneauDetail()
{
    QScrollArea *scroll = new QScrollArea(this);
    scroll->setWidgetResizable(true);
    scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    frameDetail = new QFrame();
    frameDetail->setObjectName("detailPanel");
    QVBoxLayout *lay = new QVBoxLayout(frameDetail);
    lay->setSpacing(8);
    lay->setContentsMargins(12, 12, 12, 12);

    QLabel *title = new QLabel("Detail de l'enregistrement");
    title->setObjectName("detailTitle");
    lay->addWidget(title);

    QFrame *sep0 = new QFrame();
    sep0->setFrameShape(QFrame::HLine);
    sep0->setObjectName("separator");
    lay->addWidget(sep0);

    auto makeRow = [&](const QString &labelText, QLabel *&valueLabel,
                       const QString &objName = "detailValue")
    {
        QWidget *row = new QWidget();
        QHBoxLayout *rl = new QHBoxLayout(row);
        rl->setContentsMargins(0, 2, 0, 2);
        QLabel *lbl = new QLabel(labelText);
        lbl->setObjectName("detailKey");
        lbl->setFixedWidth(95);
        valueLabel = new QLabel("--");
        valueLabel->setObjectName(objName);
        valueLabel->setWordWrap(true);
        rl->addWidget(lbl);
        rl->addWidget(valueLabel, 1);
        lay->addWidget(row);
    };

    makeRow("Audit ID :",    lblDetailAuditId);
    makeRow("Transaction :", lblDetailTransId);
    makeRow("Action :",      lblDetailAction,  "detailAction");
    makeRow("Champ :",       lblDetailChamp);
    makeRow("Utilisateur :", lblDetailUser);
    makeRow("Date/Heure :",  lblDetailDate);
    makeRow("Version :",     lblDetailVersion);

    QFrame *sep1 = new QFrame();
    sep1->setFrameShape(QFrame::HLine);
    sep1->setObjectName("separator");
    lay->addWidget(sep1);

    makeRow("Ancienne val.:", lblOldValue, "detailOld");
    makeRow("Nouvelle val.:", lblNewValue, "detailNew");

    // NOUVEAU : Diff visuel variation
    QWidget *varRow = new QWidget();
    QHBoxLayout *vrl = new QHBoxLayout(varRow);
    vrl->setContentsMargins(0, 2, 0, 2);
    QLabel *varLbl = new QLabel("Variation :");
    varLbl->setObjectName("detailKey");
    varLbl->setFixedWidth(95);
    lblVariation = new QLabel("--");
    lblVariation->setObjectName("detailVariation");
    vrl->addWidget(varLbl);
    vrl->addWidget(lblVariation, 1);
    lay->addWidget(varRow);

    QFrame *sep2 = new QFrame();
    sep2->setFrameShape(QFrame::HLine);
    sep2->setObjectName("separator");
    lay->addWidget(sep2);

    makeRow("Severite :",  lblDetailSeverite,  "detailSeverity");
    makeRow("Anomalie :",  lblAnomalyStatus,   "detailAnomaly");
    makeRow("Impact :",    lblImpactScore,     "detailImpact");
    makeRow("Revision :",  lblStatutRevision,  "detailRevision");

    QLabel *noteLbl = new QLabel("Note :");
    noteLbl->setObjectName("detailKey");
    lay->addWidget(noteLbl);

    txtDetailNote = new QTextEdit();
    txtDetailNote->setPlaceholderText("Aucune note...");
    txtDetailNote->setReadOnly(true);
    txtDetailNote->setObjectName("detailNote");
    txtDetailNote->setFixedHeight(70);
    lay->addWidget(txtDetailNote);

    lay->addStretch();
    scroll->setWidget(frameDetail);
    return scroll;
}

// ─── Barre d'actions ─────────────────────────────────────────────────────────

QWidget* HistoriqueAuditTab::creerBarreActions()
{
    QWidget *w = new QWidget(this);
    w->setObjectName("actionBar");
    QVBoxLayout *vlay = new QVBoxLayout(w);
    vlay->setContentsMargins(6, 4, 6, 4);
    vlay->setSpacing(4);

    // Ligne 1 : actions principales
    QHBoxLayout *lay1 = new QHBoxLayout();
    lay1->setSpacing(6);

    btnComparison      = new QPushButton("Comparer (2 lignes)");
    btnRestore         = new QPushButton("Restaurer valeur");
    btnDetectAnomalies = new QPushButton("Detecter anomalies");
    btnDetailComplet   = new QPushButton("Detail complet");
    btnExportPDF       = new QPushButton("Export PDF");
    btnExportCSV       = new QPushButton("Export CSV");

    btnComparison->setEnabled(false);
    btnComparison->setObjectName("btnAction");
    btnRestore->setObjectName("btnDanger");
    btnDetectAnomalies->setObjectName("btnWarning");
    btnDetailComplet->setObjectName("btnSecondary");
    btnExportPDF->setObjectName("btnPrimary");
    btnExportCSV->setObjectName("btnSecondary");

    lay1->addWidget(btnComparison);
    lay1->addWidget(btnRestore);
    lay1->addWidget(btnDetectAnomalies);
    lay1->addWidget(btnDetailComplet);
    lay1->addStretch();
    lay1->addWidget(btnExportCSV);
    lay1->addWidget(btnExportPDF);

    vlay->addLayout(lay1);

    return w;
}

// ═══════════════════════════════════════════════════════════════════════════════
//  STYLE — LIGHT PROFESSIONAL GREEN THEME
// ═══════════════════════════════════════════════════════════════════════════════

void HistoriqueAuditTab::appliquerStyle()
{
    m_originalStyle = R"(
QWidget {
    background-color: #f5f7fa;
    color: #2c3e50;
    font-family: 'Segoe UI', Arial, sans-serif;
    font-size: 13px;
}

QWidget#statsBar {
    background: #ffffff;
    border-radius: 10px;
    border: 1px solid #d4e6d8;
}

QFrame#cardTotal    { background:#f9fafb; border:2px solid #1f5c3d; border-radius:8px; }
QFrame#cardInsert   { background:#f0fdf4; border:2px solid #1f5c3d; border-radius:8px; }
QFrame#cardUpdate   { background:#fffbeb; border:2px solid #ca8a04; border-radius:8px; }
QFrame#cardDelete   { background:#fef2f2; border:2px solid #dc2626; border-radius:8px; }
QFrame#cardAnomaly  { background:#fff7ed; border:2px solid #ea580c; border-radius:8px; }
QFrame#cardCritical { background:#fef2f2; border:2px solid #dc2626; border-radius:8px; }
QFrame#cardRatio    { background:#f0fdf4; border:2px solid #1f5c3d; border-radius:8px; min-width:130px; }

QLabel#statLabel { font-size:11px; color:#666666; font-weight:500; }
QLabel#statValue { font-size:22px; font-weight:700; color:#1f5c3d; }

QProgressBar {
    background:#eeeeee; border:1px solid #d4e6d8;
    border-radius:7px; text-align:center;
    color:#1f5c3d; font-size:11px; font-weight:600;
}
QProgressBar::chunk {
    background: qlineargradient(x1:0,y1:0,x2:1,y2:0,
        stop:0 #dc2626, stop:0.5 #ea580c, stop:1 #f59e0b);
    border-radius:7px;
}

QGroupBox#filterGroup {
    background:#ffffff; border:2px solid #1f5c3d; border-radius:10px;
    margin-top:6px; padding-top:16px; font-size:12px; font-weight:600; color:#1f5c3d;
}
QGroupBox#filterGroup::title {
    subcontrol-origin:margin; subcontrol-position:top left;
    left:12px; padding:0 6px; color:#1f5c3d;
}
QGroupBox#tableGroup {
    background:#ffffff; border:2px solid #1f5c3d; border-radius:10px;
    margin-top:6px; padding-top:14px; font-size:12px; font-weight:600; color:#1f5c3d;
}
QGroupBox#tableGroup::title {
    subcontrol-origin:margin; subcontrol-position:top left;
    left:12px; padding:0 6px; color:#1f5c3d;
}

QLabel#filterLabel { color:#555555; font-size:12px; }

QComboBox#filterCombo {
    background:#ffffff; border:1px solid #d4e6d8; border-radius:6px;
    padding:4px 10px; color:#2c3e50; min-width:90px; font-size:12px;
}
QComboBox#filterCombo:hover { border-color:#1f5c3d; }
QComboBox#filterCombo::drop-down { border:none; width:18px; }
QComboBox#filterCombo QAbstractItemView {
    background:#ffffff; border:1px solid #d4e6d8;
    selection-background-color:#d4e6d8; color:#2c3e50;
}

QLineEdit#filterSearch {
    background:#ffffff; border:1px solid #d4e6d8; border-radius:6px;
    padding:4px 10px; color:#2c3e50; font-size:12px;
}
QLineEdit#filterSearch:focus { border-color:#1f5c3d; }

QCheckBox#filterCheck { color:#555555; font-size:12px; spacing:6px; }
QCheckBox#filterCheck::indicator {
    width:14px; height:14px; border:1px solid #d4e6d8;
    border-radius:3px; background:#ffffff;
}
QCheckBox#filterCheck::indicator:checked { background:#1f5c3d; border-color:#1f5c3d; }

QDateEdit#filterDate {
    background:#ffffff; border:1px solid #d4e6d8; border-radius:6px;
    padding:4px 8px; color:#2c3e50; font-size:12px;
}
QDateEdit#filterDate:disabled { background:#f0f0f0; color:#aaaaaa; }

QLabel#resultCount { color:#1f5c3d; font-size:12px; font-weight:600; }

QLabel#filtreActifLabel {
    background:#fff3cd; color:#856404; border:1px solid #ffc107;
    border-radius:4px; padding:2px 8px; font-size:11px; font-weight:600;
}

QLabel#statutTableau {
    color:#666666; font-size:11px; padding:2px 4px;
    border-top:1px solid #e0e0e0;
}

QTableWidget#auditTable {
    background:#ffffff; alternate-background-color:#f9fafb;
    border:2px solid #1f5c3d; border-radius:8px;
    gridline-color:#d4e6d8; color:#2c3e50; font-size:12px;
    selection-background-color:#d4e6d8; selection-color:#1f5c3d;
}
QTableWidget#auditTable::item { padding:4px 8px; border-bottom:1px solid #f0f0f0; }
QTableWidget#auditTable::item:hover { background:#f0fdf4; }
QTableWidget#auditTable::item:selected { background:#d4e6d8; color:#1f5c3d; }

QHeaderView::section {
    background:#1f5c3d; color:#ffffff; font-weight:600; font-size:12px;
    padding:8px 6px; border:none;
    border-right:1px solid #2d7d4f; letter-spacing:0.5px;
}
QHeaderView::section:hover { background:#2d7d4f; }

QScrollBar:vertical { background:#f5f7fa; width:8px; border-radius:4px; }
QScrollBar::handle:vertical { background:#d4e6d8; border-radius:4px; min-height:30px; }
QScrollBar::handle:vertical:hover { background:#a8d4b9; }
QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height:0; }
QScrollBar:horizontal { background:#f5f7fa; height:8px; border-radius:4px; }
QScrollBar::handle:horizontal { background:#d4e6d8; border-radius:4px; }

QFrame#detailPanel { background:#ffffff; border:2px solid #1f5c3d; border-radius:10px; }
QScrollArea { background:transparent; border:none; }

QLabel#detailTitle { font-size:14px; font-weight:700; color:#1f5c3d; }
QFrame#separator { color:#d4e6d8; background:#d4e6d8; max-height:1px; }
QLabel#detailKey { color:#666666; font-size:12px; font-weight:500; }
QLabel#detailValue   { color:#2c3e50; font-size:12px; }
QLabel#detailOld     { color:#dc2626; font-size:12px; font-weight:600; }
QLabel#detailNew     { color:#16a34a; font-size:12px; font-weight:600; }
QLabel#detailAction  { font-size:13px; font-weight:700; }
QLabel#detailSeverity{ font-weight:700; font-size:12px; }
QLabel#detailAnomaly { font-weight:600; font-size:12px; }
QLabel#detailImpact  { font-weight:600; font-size:12px; color:#ea580c; }

QTextEdit#detailNote {
    background:#f9fafb; border:1px solid #d4e6d8;
    border-radius:6px; color:#555555; font-size:12px; padding:6px;
}

QWidget#actionBar { background:#ffffff; border:2px solid #1f5c3d; border-radius:10px; }

QPushButton#btnPrimary {
    background:qlineargradient(x1:0,y1:0,x2:1,y2:0,stop:0 #1f5c3d,stop:1 #2d7d4f);
    color:#ffffff; border:none; border-radius:7px; padding:8px 18px;
    font-weight:600; font-size:12px;
}
QPushButton#btnPrimary:hover {
    background:qlineargradient(x1:0,y1:0,x2:1,y2:0,stop:0 #2d7d4f,stop:1 #3d9d5f);
}
QPushButton#btnPrimary:pressed { background:#1f5c3d; }

QPushButton#btnSecondary {
    background:#f9fafb; color:#666666; border:2px solid #d4e6d8;
    border-radius:7px; padding:8px 18px; font-weight:600; font-size:12px;
}
QPushButton#btnSecondary:hover { background:#e8eef0; color:#1f5c3d; border-color:#1f5c3d; }
QPushButton#btnSecondary:pressed { background:#d4e6d8; }

QPushButton#btnAction {
    background:#f0fdf4; color:#1f5c3d; border:2px solid #a7d9b5;
    border-radius:7px; padding:8px 18px; font-weight:600; font-size:12px;
}
QPushButton#btnAction:hover   { background:#d4e6d8; border-color:#1f5c3d; }
QPushButton#btnAction:disabled{
    background:#f0f0f0; color:#aaaaaa; border-color:#dddddd;
}

QPushButton#btnDanger {
    background:#fef2f2; color:#dc2626; border:2px solid #fecaca;
    border-radius:7px; padding:8px 18px; font-weight:600; font-size:12px;
}
QPushButton#btnDanger:hover { background:#fee2e2; border-color:#dc2626; }

QPushButton#btnWarning {
    background:#fff7ed; color:#ea580c; border:2px solid #fed7aa;
    border-radius:7px; padding:8px 18px; font-weight:600; font-size:12px;
}
QPushButton#btnWarning:hover { background:#ffedd5; border-color:#ea580c; }

QPushButton#btnPrimaryActive {
    background:qlineargradient(x1:0,y1:0,x2:1,y2:0,stop:0 #ca8a04,stop:1 #d97706);
    color:#ffffff; border:none; border-radius:7px; padding:8px 18px;
    font-weight:600; font-size:12px;
}

/* ── Onglets principaux ──────────────────────────────────────────── */
QTabWidget#mainAuditTabs::pane {
    border:2px solid #1f5c3d; border-radius:8px; background:#f5f7fa;
}
QTabWidget#mainAuditTabs > QTabBar::tab {
    background:#e8f5ee; color:#1f5c3d; border:1px solid #d4e6d8;
    border-bottom:none; border-radius:6px 6px 0 0;
    padding:8px 18px; font-weight:600; font-size:12px; margin-right:3px;
}
QTabWidget#mainAuditTabs > QTabBar::tab:selected {
    background:#1f5c3d; color:#ffffff;
}
QTabWidget#mainAuditTabs > QTabBar::tab:hover:!selected {
    background:#c5e8d4;
}

/* ── Analytique ──────────────────────────────────────────────────── */
QLabel#analyticTitle {
    font-size:16px; font-weight:700; color:#1f5c3d;
    padding:6px 4px; border-bottom:2px solid #1f5c3d; margin-bottom:4px;
}
QFrame#chartFrame {
    background:#ffffff; border:2px solid #d4e6d8;
    border-radius:10px; padding:4px;
}
QLabel#chartTitle {
    font-size:12px; font-weight:600; color:#1f5c3d; padding:2px 4px;
}

/* ── Detail variation & revision ─────────────────────────────────── */
QLabel#detailVariation { font-size:13px; font-weight:700; }
QLabel#detailRevision  { font-size:12px; font-weight:600; }

/* ── Timeline ────────────────────────────────────────────────────── */
QListWidget#timelineList {
    background:#ffffff; border:2px solid #d4e6d8; border-radius:8px;
    font-size:12px; color:#2c3e50;
}
QListWidget#timelineList::item {
    padding:6px 10px; border-bottom:1px solid #f0f0f0; border-radius:4px;
}
QListWidget#timelineList::item:hover    { background:#f0fdf4; }
QListWidget#timelineList::item:selected { background:#d4e6d8; color:#1f5c3d; }
    )";
    setStyleSheet(m_originalStyle);
}

// ═══════════════════════════════════════════════════════════════════════════════
//  CONNEXIONS — FIX PRINCIPAL DES FILTRES
// ═══════════════════════════════════════════════════════════════════════════════

void HistoriqueAuditTab::setupConnections()
{
    // Boutons principaux
    connect(btnRefresh,         &QPushButton::clicked, this, &HistoriqueAuditTab::chargerHistorique);
    connect(btnViderFiltres,    &QPushButton::clicked, this, &HistoriqueAuditTab::viderFiltres);
    connect(btnComparison,      &QPushButton::clicked, this, &HistoriqueAuditTab::comparerValeurs);
    connect(btnRestore,         &QPushButton::clicked, this, &HistoriqueAuditTab::restaurerValeur);
    connect(btnExportPDF,       &QPushButton::clicked, this, &HistoriqueAuditTab::exporterPDF);
    connect(btnExportCSV,       &QPushButton::clicked, this, &HistoriqueAuditTab::exporterCSV);
    connect(btnDetectAnomalies, &QPushButton::clicked, this, &HistoriqueAuditTab::detecterAnomalies);
    connect(btnDetailComplet,   &QPushButton::clicked, this, &HistoriqueAuditTab::afficherDetailComplet);

    // FIX 1 : activated au lieu de currentIndexChanged
    // activated se déclenche UNIQUEMENT quand l'utilisateur clique, pas au démarrage
    connect(cbActionFilter,   QOverload<int>::of(&QComboBox::activated),
            this, &HistoriqueAuditTab::filtrerParAction);
    connect(cbSeverityFilter, QOverload<int>::of(&QComboBox::activated),
            this, &HistoriqueAuditTab::filtrerParSeverite);
    connect(cbUserFilter,     QOverload<int>::of(&QComboBox::activated),
            this, &HistoriqueAuditTab::filtrerParAction);

    // Checkboxes
    connect(chkAnomaliesOnly, &QCheckBox::stateChanged,
            this, &HistoriqueAuditTab::filtrerParAnomalie);
    connect(chkFiltreDate,    &QCheckBox::stateChanged,
            this, &HistoriqueAuditTab::filtrerParPeriode);

    // Dates — seulement si checkbox cochée
    connect(dtDebut, &QDateEdit::dateChanged, this, [this](){
        if (!isInitializing && chkFiltreDate->isChecked()) chargerHistorique();
    });
    connect(dtFin, &QDateEdit::dateChanged, this, [this](){
        if (!isInitializing && chkFiltreDate->isChecked()) chargerHistorique();
    });

    // Recherches texte
    connect(txtSearchTransaction, &QLineEdit::returnPressed,
            this, &HistoriqueAuditTab::rechercherTransaction);
    connect(txtSearchLibre, &QLineEdit::returnPressed,
            this, &HistoriqueAuditTab::rechercherTransaction);
    // Connecter le nouveau bouton Rechercher
    connect(btnSearch, &QPushButton::clicked,
            this, &HistoriqueAuditTab::rechercherTransaction);

    // Recherche en temps réel avec délai
    connect(txtSearchLibre, &QLineEdit::textChanged, this, [this](){
        if (!isInitializing)
            QTimer::singleShot(400, this, &HistoriqueAuditTab::chargerHistorique);
    });

    // FIX 3 : Sélection tableau — cellClicked + currentCellChanged + double-clic
    connect(tableAudit, &QTableWidget::cellClicked,
            this, &HistoriqueAuditTab::onAuditRowSelected);

    // FIX 3b : Navigation clavier et clic normal
    connect(tableAudit, &QTableWidget::currentCellChanged,
            this, [this](int row, int, int, int) {
                if (!isInitializing && row >= 0) {
                    currentSelectedRow = row;
                    mettreAJourDetail(row);
                }
            });

    // FIX 12 : Double-clic → détail complet directement
    connect(tableAudit, &QTableWidget::cellDoubleClicked,
            this, [this](int row, int) {
                currentSelectedRow = row;
                mettreAJourDetail(row);
                afficherDetailComplet();
            });

    // FIX 14 : Activer bouton Comparer seulement quand 2 lignes sélectionnées
    connect(tableAudit, &QTableWidget::itemSelectionChanged, this, [this](){
        int nbSel = tableAudit->selectionModel()->selectedRows().count();
        btnComparison->setEnabled(nbSel == 2);
    });

    // Indicateur filtre actif
    connect(cbActionFilter,   QOverload<int>::of(&QComboBox::activated),
            this, &HistoriqueAuditTab::mettreAJourBoutonActif);
    connect(cbSeverityFilter, QOverload<int>::of(&QComboBox::activated),
            this, &HistoriqueAuditTab::mettreAJourBoutonActif);
    connect(chkAnomaliesOnly, &QCheckBox::stateChanged,
            this, &HistoriqueAuditTab::mettreAJourBoutonActif);
    connect(chkFiltreDate,    &QCheckBox::stateChanged,
            this, &HistoriqueAuditTab::mettreAJourBoutonActif);

    // En-tête tableau : clic droit pour masquer colonnes
    tableAudit->horizontalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(tableAudit->horizontalHeader(), &QHeaderView::customContextMenuRequested,
            this, &HistoriqueAuditTab::onHeaderContextMenu);
}

// ═══════════════════════════════════════════════════════════════════════════════
//  CHARGEMENT DONNÉES
// ═══════════════════════════════════════════════════════════════════════════════

void HistoriqueAuditTab::chargerHistorique()
{
    if (isInitializing) return;  // FIX 1 : bloquer pendant construction

    tableAudit->setSortingEnabled(false);
    tableAudit->setRowCount(0);
    cachedRecords.clear();

    if (!dbConn) return;
    QSqlDatabase db = dbConn->getDatabase();
    if (!db.isOpen()) {
        QMessageBox::critical(this, "Erreur", "Base de donnees non connectee.");
        return;
    }

    QString sql = construireRequete();
    QSqlQuery query(db);

    if (!query.exec(sql)) {
        qDebug() << "SQL Error:" << query.lastError().text();
        QMessageBox::critical(this, "Erreur SQL", query.lastError().text());
        return;
    }

    remplirTableauAudit(query);
    tableAudit->setSortingEnabled(true);
    mettreAJourStats();
    mettreAJourBoutonActif();
}

// FIX 9 : Charger la liste des utilisateurs dans cbUserFilter
void HistoriqueAuditTab::chargerListeUtilisateurs()
{
    if (!dbConn || !dbConn->getDatabase().isOpen()) return;
    QSqlDatabase db = dbConn->getDatabase();
    QSqlQuery q(db);
    q.exec("SELECT DISTINCT ID_EMPLOYEE FROM FINANCE WHERE TYPE_LIGNE = 'AUDIT' ORDER BY ID_EMPLOYEE");
    while (q.next()) {
        int uid = q.value(0).toInt();
        cbUserFilter->addItem("User " + QString::number(uid), uid);
    }
}

QString HistoriqueAuditTab::construireRequete() const
{
    // CORRIGÉ : une seule liste de conditions, pas de prepend() en double
    QStringList conditions;
    conditions << "TYPE_LIGNE = 'AUDIT'";  // ← une seule fois ici

    // Filtre Action
    if (cbActionFilter->currentIndex() > 0) {
        QString dbVal = cbActionFilter->currentData().toString();
        if (!dbVal.isEmpty())
            conditions << QString("AUDIT_ACTION = '%1'").arg(dbVal);
    }

    // Filtre Severite
    if (cbSeverityFilter->currentIndex() > 0) {
        QString dbVal = cbSeverityFilter->currentData().toString();
        if (!dbVal.isEmpty())
            conditions << QString("AUDIT_SEVERITE = '%1'").arg(dbVal);
    }

    // Filtre Utilisateur
    if (cbUserFilter->currentIndex() > 0) {
        int uid = cbUserFilter->currentData().toInt();
        conditions << QString("ID_EMPLOYEE = %1").arg(uid);
    }

    // Filtre anomalies
    if (chkAnomaliesOnly->isChecked())
        conditions << "AUDIT_EST_ANOMALIE = 1";

    // Recherche par ID transaction (Audit ID ou Parent Transaction ID)
    QString searchId = txtSearchTransaction->text().trimmed();
    if (!searchId.isEmpty()) {
        bool ok;
        int idNum = searchId.toInt(&ok);
        if (ok) {
            conditions << QString("(ID_TRANSACTION = %1 OR PARENT_TRANSACTION_ID = %1)").arg(idNum);
        } else {
            // Sécurité si du texte est saisi (Oracle TO_CHAR ajoute un espace par défaut, FM l'enlève)
            conditions << QString("(TO_CHAR(ID_TRANSACTION, 'FM999999999') = '%1' OR TO_CHAR(PARENT_TRANSACTION_ID, 'FM999999999') = '%1')").arg(searchId);
        }
    }

    // Recherche texte libre
    QString searchLibre = txtSearchLibre->text().trimmed();
    if (!searchLibre.isEmpty()) {
        QString escaped = searchLibre;
        escaped.replace("'", "''");
        conditions << QString("(AUDIT_CHAMP LIKE '%%1%' OR AUDIT_OLD_VALUE LIKE '%%1%' OR AUDIT_NEW_VALUE LIKE '%%1%')")
                      .arg(escaped);
    }

    // Filtre période
    if (chkFiltreDate->isChecked()) {
        conditions << QString("CREATED_AT >= TO_TIMESTAMP('%1 00:00:00', 'YYYY-MM-DD HH24:MI:SS')")
                      .arg(dtDebut->date().toString("yyyy-MM-dd"));
        conditions << QString("CREATED_AT <= TO_TIMESTAMP('%1 23:59:59', 'YYYY-MM-DD HH24:MI:SS')")
                      .arg(dtFin->date().toString("yyyy-MM-dd"));
    }

    QString sql = "SELECT ID_TRANSACTION, PARENT_TRANSACTION_ID, AUDIT_ACTION, AUDIT_OPERATION_TYPE, AUDIT_CHAMP, "
                  "AUDIT_OLD_VALUE, AUDIT_NEW_VALUE, ID_EMPLOYEE, CREATED_AT, AUDIT_SEVERITE, "
                  "AUDIT_VERSION, AUDIT_EST_ANOMALIE, AUDIT_IMPACT_SCORE "
                  "FROM FINANCE ";

    if (!conditions.isEmpty())
        sql += " WHERE " + conditions.join(" AND ");

    sql += " ORDER BY CREATED_AT DESC";
    return sql;
}

bool HistoriqueAuditTab::filtreEstActif() const
{
    return (cbActionFilter->currentIndex() > 0)
        || (cbSeverityFilter->currentIndex() > 0)
        || (cbUserFilter->currentIndex() > 0)
        || chkAnomaliesOnly->isChecked()
        || chkFiltreDate->isChecked()
        || !txtSearchTransaction->text().trimmed().isEmpty()
        || !txtSearchLibre->text().trimmed().isEmpty();
}

// FIX 10 : Indicateur visuel filtre actif
void HistoriqueAuditTab::mettreAJourBoutonActif()
{
    bool actif = filtreEstActif();
    lblFiltreActif->setVisible(actif);
    lblFiltreActif->setText(actif ? "  Filtre actif  " : "");

    // Changer la couleur du bouton Actualiser si filtre actif
    if (actif) {
        btnRefresh->setObjectName("btnPrimaryActive");
        btnRefresh->setStyleSheet(
            "QPushButton { background: qlineargradient(x1:0,y1:0,x2:1,y2:0,"
            "stop:0 #ca8a04,stop:1 #d97706); color:#ffffff; border:none;"
            "border-radius:7px; padding:8px 18px; font-weight:600; font-size:12px; }"
            "QPushButton:hover { background:#b45309; }");
    } else {
        btnRefresh->setObjectName("btnPrimary");
        btnRefresh->setStyleSheet("");
    }
}

// ─── Remplissage du tableau ───────────────────────────────────────────────────

void HistoriqueAuditTab::remplirTableauAudit(QSqlQuery &query)
{
    int row = 0;
    int nbAnomalies = 0, nbCritiques = 0;

    while (query.next()) {
        tableAudit->insertRow(row);

        auto cell = [&](int col, const QString &txt, const QString &tooltip = "") {
            QTableWidgetItem *item = new QTableWidgetItem(txt);
            item->setTextAlignment(Qt::AlignVCenter | Qt::AlignLeft);
            if (!tooltip.isEmpty()) item->setToolTip(tooltip);
            tableAudit->setItem(row, col, item);
        };

        QTableWidgetItem *item0 = new QTableWidgetItem(query.value(0).toString());
        item0->setData(Qt::UserRole, cachedRecords.size());
        item0->setTextAlignment(Qt::AlignVCenter | Qt::AlignLeft);
        tableAudit->setItem(row, 0, item0);

        cell(1, query.value(1).toString());

        // Action colorée
        QString action = query.value(2).toString();
        QTableWidgetItem *actItem = new QTableWidgetItem(getActionIcon(action) + " " + action);
        actItem->setForeground(QColor(getActionColor(action)));
        QFont fBold; fBold.setBold(true);
        actItem->setFont(fBold);
        actItem->setTextAlignment(Qt::AlignVCenter | Qt::AlignLeft);
        tableAudit->setItem(row, 2, actItem);

        // FIX 16 : Type Op. avec tooltip pour voir la valeur complète
        QString typeOp = query.value(3).toString();
        cell(3, typeOp.left(15), typeOp);  // tooltip = valeur complète

        cell(4, query.value(4).toString());

        // FIX 13 : Tooltip sur valeurs tronquées
        QString oldVal = query.value(5).toString();
        QString newVal = query.value(6).toString();
        cell(5, oldVal.left(60), oldVal);   // tooltip = valeur complète
        cell(6, newVal.left(60), newVal);

        cell(7, "User " + query.value(7).toString());

        QDateTime dt = query.value(8).toDateTime();
        QString dateStr = dt.isValid()
            ? dt.toString("dd/MM/yy HH:mm:ss")
            : query.value(8).toString();
        cell(8, dateStr);

        // Sévérité colorée
        QString severity = query.value(9).toString();
        QTableWidgetItem *sevItem = new QTableWidgetItem(" " + severity + " ");
        sevItem->setBackground(QColor(getSeveriteColor(severity)));
        sevItem->setForeground(QColor(getSeveriteTextColor(severity)));
        QFont fSev; fSev.setBold(true);
        sevItem->setFont(fSev);
        sevItem->setTextAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
        tableAudit->setItem(row, 9, sevItem);

        cell(10, "v" + query.value(10).toString());

        // Anomalie
        bool isAnomaly = (query.value(11).toInt() == 1);
        if (isAnomaly) nbAnomalies++;
        if (severity == "CRITICAL") nbCritiques++;

        QTableWidgetItem *anomItem = new QTableWidgetItem(isAnomaly ? " OUI" : " non");
        anomItem->setForeground(isAnomaly ? QColor("#dc2626") : QColor("#16a34a"));
        if (isAnomaly) {
            anomItem->setBackground(QColor("#fff0f0"));
        }
        QFont fAnom; fAnom.setBold(isAnomaly);
        anomItem->setFont(fAnom);
        anomItem->setTextAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
        tableAudit->setItem(row, 11, anomItem);

        // Impact
        double impact = query.value(12).toDouble();
        QTableWidgetItem *impItem = new QTableWidgetItem(
            QString::number(impact, 'f', 1) + " %");
        impItem->setForeground(couleurImpact(impact));
        QFont fImp; fImp.setBold(impact > 50);
        impItem->setFont(fImp);
        impItem->setTextAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
        tableAudit->setItem(row, 12, impItem);

        colorierLigne(row, severity, isAnomaly);

        // Cache
        AuditRecord rec;
        rec.auditId       = query.value(0).toInt();
        rec.transactionId = query.value(1).toInt();
        rec.action        = action;
        rec.operationType = query.value(3).toString();
        rec.fieldName     = query.value(4).toString();
        rec.oldValue      = oldVal;
        rec.newValue      = newVal;
        rec.changedBy     = query.value(7).toInt();
        rec.changedAt     = dateStr;
        rec.severity      = severity;
        rec.version       = query.value(10).toInt();
        rec.isAnomaly     = isAnomaly;
        rec.impactScore   = impact;
        cachedRecords.append(rec);

        tableAudit->setRowHeight(row, 30);
        row++;
    }

    lblNbResultats->setText(QString::number(row) + " resultat(s)");

    // FIX 15 : Barre de statut détaillée sous le tableau
    lblStatutTableau->setText(
        QString("%1 lignes affichees  |  %2 anomalie(s)  |  %3 critique(s)")
            .arg(row).arg(nbAnomalies).arg(nbCritiques));
}

void HistoriqueAuditTab::colorierLigne(int row, const QString &severity, bool isAnomaly)
{
    QColor bg;
    bool colorer = false;
    if (isAnomaly)                   { bg = QColor("#fff5f5"); colorer = true; }
    else if (severity == "CRITICAL") { bg = QColor("#fef2f2"); colorer = true; }
    else if (severity == "HIGH")     { bg = QColor("#fffbeb"); colorer = true; }

    if (!colorer) return;

    for (int c = 0; c < tableAudit->columnCount(); ++c) {
        if (c == 9) continue;  // ne pas écraser la colonne sévérité
        QTableWidgetItem *it = tableAudit->item(row, c);
        if (it) it->setBackground(bg);
    }
}

// ═══════════════════════════════════════════════════════════════════════════════
//  STATISTIQUES
// ═══════════════════════════════════════════════════════════════════════════════

void HistoriqueAuditTab::mettreAJourStats()
{
    if (!dbConn || !dbConn->getDatabase().isOpen()) return;
    QSqlDatabase db = dbConn->getDatabase();

    auto queryInt = [&](const QString &sql) -> int {
        QSqlQuery q(db);
        return (q.exec(sql) && q.next()) ? q.value(0).toInt() : 0;
    };

    AuditStats stats;
    stats.totalActions   = queryInt("SELECT COUNT(*) FROM FINANCE WHERE TYPE_LIGNE='AUDIT'");
    stats.totalInserts   = queryInt("SELECT COUNT(*) FROM FINANCE WHERE TYPE_LIGNE='AUDIT' AND AUDIT_ACTION='INSERT'");
    stats.totalUpdates   = queryInt("SELECT COUNT(*) FROM FINANCE WHERE TYPE_LIGNE='AUDIT' AND AUDIT_ACTION='UPDATE'");
    stats.totalDeletes   = queryInt("SELECT COUNT(*) FROM FINANCE WHERE TYPE_LIGNE='AUDIT' AND AUDIT_ACTION='DELETE'");
    stats.totalAnomalies = queryInt("SELECT COUNT(*) FROM FINANCE WHERE TYPE_LIGNE='AUDIT' AND AUDIT_EST_ANOMALIE=1");
    stats.totalCritical  = queryInt("SELECT COUNT(*) FROM FINANCE WHERE TYPE_LIGNE='AUDIT' AND AUDIT_SEVERITE='CRITICAL'");

    mettreAJourCartesStat(stats);
}

void HistoriqueAuditTab::mettreAJourCartesStat(const AuditStats &stats)
{
    if (lblStatTotal)     lblStatTotal->setText(QString::number(stats.totalActions));
    if (lblStatInserts)   lblStatInserts->setText(QString::number(stats.totalInserts));
    if (lblStatUpdates)   lblStatUpdates->setText(QString::number(stats.totalUpdates));
    if (lblStatDeletes)   lblStatDeletes->setText(QString::number(stats.totalDeletes));
    if (lblStatAnomalies) lblStatAnomalies->setText(QString::number(stats.totalAnomalies));
    if (lblStatCritical)  lblStatCritical->setText(QString::number(stats.totalCritical));

    int ratio = (stats.totalActions > 0)
        ? (stats.totalAnomalies * 100 / stats.totalActions) : 0;
    if (barAnomalieRatio) barAnomalieRatio->setValue(ratio);
}

AuditStats HistoriqueAuditTab::getStats() const { return AuditStats{}; }

// ═══════════════════════════════════════════════════════════════════════════════
//  SÉLECTION LIGNE
// ═══════════════════════════════════════════════════════════════════════════════

void HistoriqueAuditTab::onAuditRowSelected(int row, int)
{
    if (row < 0 || row >= tableAudit->rowCount()) return;
    QTableWidgetItem *item = tableAudit->item(row, 0);
    if (!item) return;
    currentSelectedRow = item->data(Qt::UserRole).toInt();
    mettreAJourDetail(currentSelectedRow);
}

void HistoriqueAuditTab::mettreAJourDetail(int row)
{
    if (row < 0 || row >= cachedRecords.size()) return;
    const AuditRecord &rec = cachedRecords[row];

    lblDetailAuditId->setText(QString::number(rec.auditId));
    lblDetailTransId->setText(QString::number(rec.transactionId));
    lblDetailChamp->setText(rec.fieldName.isEmpty() ? "--" : rec.fieldName);
    lblDetailUser->setText("Utilisateur " + QString::number(rec.changedBy));
    lblDetailDate->setText(rec.changedAt);
    lblDetailVersion->setText("v" + QString::number(rec.version));

    lblDetailAction->setText(getActionIcon(rec.action) + " " + rec.action);
    lblDetailAction->setStyleSheet(
        "color:" + getActionColor(rec.action) + "; font-weight:bold; font-size:13px;");

    // Valeurs complètes
    lblOldValue->setText(rec.oldValue.isEmpty() ? "--" : rec.oldValue);
    lblNewValue->setText(rec.newValue.isEmpty() ? "--" : rec.newValue);

    // NOUVEAU : Variation %
    QString var = calculerVariation(rec.oldValue, rec.newValue);
    lblVariation->setText(var);
    if (var.startsWith("+"))      lblVariation->setStyleSheet("color:#dc2626; font-weight:bold;");
    else if (var.startsWith("-")) lblVariation->setStyleSheet("color:#16a34a; font-weight:bold;");
    else                          lblVariation->setStyleSheet("color:#666666;");

    lblDetailSeverite->setText(rec.severity);
    lblDetailSeverite->setStyleSheet(
        "color:" + getSeveriteColor(rec.severity) + "; font-weight:bold;");

    lblAnomalyStatus->setText(rec.isAnomaly ? "OUI — Anomalie detectee" : "Non");
    lblAnomalyStatus->setStyleSheet(
        rec.isAnomaly ? "color:#dc2626; font-weight:bold;" : "color:#16a34a;");

    lblImpactScore->setText(QString::number(rec.impactScore, 'f', 2) + " %");
    lblImpactScore->setStyleSheet(
        "color:" + couleurImpact(rec.impactScore).name() + "; font-weight:bold;");

    // NOUVEAU : Statut révision
    QString revStat = reviewMap.value(rec.auditId, "En attente");
    lblStatutRevision->setText(revStat);
    if (revStat == "Verifié")
        lblStatutRevision->setStyleSheet("color:#16a34a; font-weight:bold;");
    else if (revStat == "Signalé")
        lblStatutRevision->setStyleSheet("color:#dc2626; font-weight:bold;");
    else
        lblStatutRevision->setStyleSheet("color:#ca8a04;");

    QString note;
    if (notesMap.contains(rec.auditId)) {
        note = notesMap[rec.auditId];
    } else {
        if (rec.isAnomaly)              note += "Anomalie detectee sur cette operation.\n";
        if (rec.severity == "CRITICAL") note += "Severite critique — verification recommandee.\n";
        if (rec.action == "DELETE")     note += "Suppression — rollback disponible.\n";
        if (rec.action == "ROLLBACK")   note += "Restauration precedemment effectuee.\n";
        if (note.isEmpty())             note = "Aucune remarque particuliere.";
    }
    txtDetailNote->setPlainText(note);
}

// ═══════════════════════════════════════════════════════════════════════════════
//  FILTRES (SLOTS)
// ═══════════════════════════════════════════════════════════════════════════════

void HistoriqueAuditTab::filtrerParAction()
{
    if (!isInitializing) chargerHistorique();
}
void HistoriqueAuditTab::filtrerParSeverite()
{
    if (!isInitializing) chargerHistorique();
}
void HistoriqueAuditTab::filtrerParAnomalie()
{
    if (!isInitializing) chargerHistorique();
}
void HistoriqueAuditTab::filtrerParPeriode()
{
    if (isInitializing) return;
    // FIX 2 : activer/désactiver les DateEdit selon checkbox
    dtDebut->setEnabled(chkFiltreDate->isChecked());
    dtFin->setEnabled(chkFiltreDate->isChecked());
    chargerHistorique();
}
void HistoriqueAuditTab::rechercherTransaction()
{
    if (!isInitializing) chargerHistorique();
}

void HistoriqueAuditTab::viderFiltres()
{
    isInitializing = true;  // bloquer les rechargements multiples pendant le reset
    cbActionFilter->setCurrentIndex(0);
    cbSeverityFilter->setCurrentIndex(0);
    cbUserFilter->setCurrentIndex(0);
    chkAnomaliesOnly->setChecked(false);
    chkFiltreDate->setChecked(false);
    dtDebut->setDate(QDate::currentDate().addMonths(-1));
    dtFin->setDate(QDate::currentDate());
    dtDebut->setEnabled(false);
    dtFin->setEnabled(false);
    txtSearchTransaction->clear();
    txtSearchLibre->clear();
    isInitializing = false;

    chargerHistorique();
}

// ═══════════════════════════════════════════════════════════════════════════════
//  ACTIONS
// ═══════════════════════════════════════════════════════════════════════════════

void HistoriqueAuditTab::comparerValeurs()
{
    QList<int> selectedIndices;
    auto selectedModelIndices = tableAudit->selectionModel()->selectedRows();
    for (const auto &idx : selectedModelIndices) {
        QTableWidgetItem *item = tableAudit->item(idx.row(), 0);
        if (item) selectedIndices.append(item->data(Qt::UserRole).toInt());
    }

    if (selectedIndices.size() != 2) {
        QMessageBox::warning(this, "Selection requise",
            "Selectionnez exactement 2 lignes (Ctrl+clic).");
        return;
    }

    if (selectedIndices[0] >= cachedRecords.size() ||
        selectedIndices[1] >= cachedRecords.size() ||
        selectedIndices[0] < 0 || selectedIndices[1] < 0) {
        QMessageBox::critical(this, "Erreur", "Enregistrements introuvables.");
        return;
    }

    // Comparaison simplifiée des deux enregistrements
    const AuditRecord &rec1 = cachedRecords[selectedIndices[0]];
    const AuditRecord &rec2 = cachedRecords[selectedIndices[1]];
    
    QString comparison = QString(
        "Comparaison des enregistrements:\n\n"
        "Enregistrement 1:\n"
        "Action: %1\nAncienne valeur: %2\nNouvelle valeur: %3\n\n"
        "Enregistrement 2:\n"
        "Action: %4\nAncienne valeur: %5\nNouvelle valeur: %6"
    ).arg(rec1.action, rec1.oldValue, rec1.newValue,
          rec2.action, rec2.oldValue, rec2.newValue);
    
    QMessageBox::information(this, "Comparaison", comparison);
}

void HistoriqueAuditTab::restaurerValeur()
{
    if (currentSelectedRow < 0 || currentSelectedRow >= cachedRecords.size()) {
        QMessageBox::warning(this, "Attention", "Selectionnez une ligne d'abord.");
        return;
    }

    const AuditRecord &rec = cachedRecords[currentSelectedRow];

    if (rec.action == "INSERT") {
        QMessageBox::information(this, "Rollback impossible",
            "Un INSERT ne peut pas etre annule par restauration.\n"
            "Utilisez une suppression directe.");
        return;
    }

    if (!confirmerRestauration(rec)) return;

    QSqlDatabase db = dbConn->getDatabase();
    QSqlQuery q(db);
    q.prepare("UPDATE FINANCE SET montant = :val WHERE id_transaction = :tid");
    q.bindValue(":val", rec.oldValue.toDouble());
    q.bindValue(":tid", rec.transactionId);

    if (q.exec()) {
        QSqlQuery ins(db);
        ins.prepare("INSERT INTO FINANCE "
                    "(TYPE_LIGNE, PARENT_TRANSACTION_ID, AUDIT_ACTION, AUDIT_CHAMP, AUDIT_OLD_VALUE, AUDIT_NEW_VALUE, "
                    "ID_EMPLOYEE, AUDIT_SEVERITE, AUDIT_EST_ANOMALIE, AUDIT_IMPACT_SCORE) "
                    "VALUES ('AUDIT', :tid, 'ROLLBACK', :field, :old, :new, :user, 'LOW', 0, 0)");
        ins.bindValue(":tid",   rec.transactionId);
        ins.bindValue(":field", rec.fieldName);
        ins.bindValue(":old",   rec.newValue);
        ins.bindValue(":new",   rec.oldValue);
        ins.bindValue(":user",  rec.changedBy);
        ins.exec();

        QMessageBox::information(this, "Succes",
            QString("Valeur restauree.\nTransaction #%1 : %2 -> %3")
                .arg(rec.transactionId).arg(rec.newValue, rec.oldValue));
        chargerHistorique();
    } else {
        QMessageBox::critical(this, "Echec",
            "Restauration echouee :\n" + q.lastError().text());
    }
}

bool HistoriqueAuditTab::confirmerRestauration(const AuditRecord &rec)
{
    return QMessageBox::warning(this, "Confirmer la restauration",
        QString("Restaurer transaction #%1 ?\n\n"
                "Champ : %2\n"
                "Valeur actuelle : %3\n"
                "Valeur a restaurer : %4\n\n"
                "L'operation sera enregistree dans le journal.")
                .arg(rec.transactionId)
                .arg(rec.fieldName, rec.newValue, rec.oldValue),
        QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::Yes;
}

void HistoriqueAuditTab::detecterAnomalies()
{
    if (!dbConn || !dbConn->getDatabase().isOpen()) return;
    QSqlDatabase db = dbConn->getDatabase();
    QSqlQuery q(db);
    q.exec("SELECT COUNT(*) FROM FINANCE WHERE TYPE_LIGNE='AUDIT' AND AUDIT_EST_ANOMALIE=1");
    int nb = q.next() ? q.value(0).toInt() : 0;

    chkAnomaliesOnly->setChecked(true);
    chargerHistorique();

    QMessageBox::information(this, "Detection anomalies",
        QString("%1 anomalie(s) detectee(s).\nFiltre applique automatiquement.").arg(nb));
}

void HistoriqueAuditTab::afficherDetailComplet()
{
    if (currentSelectedRow < 0 || currentSelectedRow >= cachedRecords.size()) {
        QMessageBox::warning(this, "Attention", "Selectionnez une ligne d'abord.");
        return;
    }

    const AuditRecord &rec = cachedRecords[currentSelectedRow];
    QString detail = QString(
        "<div style='font-family:Segoe UI,Arial;'>"
        "<h2 style='color:#1f5c3d;border-bottom:2px solid #1f5c3d;padding-bottom:6px'>"
        "Detail complet — Audit #%1</h2>"
        "<table style='font-size:13px;width:100%;border-collapse:collapse'>"
        "<tr style='background:#f0fdf4'><td style='padding:5px 8px;color:#666;width:140px'><b>Transaction ID</b></td><td style='padding:5px 8px'>%2</td></tr>"
        "<tr><td style='padding:5px 8px;color:#666'><b>Action</b></td>"
        "    <td style='padding:5px 8px;color:%3;font-weight:bold'>%4</td></tr>"
        "<tr style='background:#f0fdf4'><td style='padding:5px 8px;color:#666'><b>Champ</b></td><td style='padding:5px 8px'>%5</td></tr>"
        "<tr><td style='padding:5px 8px;color:#666'><b>Ancienne valeur</b></td>"
        "    <td style='padding:5px 8px;color:#dc2626;font-weight:bold'>%6</td></tr>"
        "<tr style='background:#f0fdf4'><td style='padding:5px 8px;color:#666'><b>Nouvelle valeur</b></td>"
        "    <td style='padding:5px 8px;color:#16a34a;font-weight:bold'>%7</td></tr>"
        "<tr><td style='padding:5px 8px;color:#666'><b>Utilisateur</b></td><td style='padding:5px 8px'>%8</td></tr>"
        "<tr style='background:#f0fdf4'><td style='padding:5px 8px;color:#666'><b>Date/Heure</b></td><td style='padding:5px 8px'>%9</td></tr>"
        "<tr><td style='padding:5px 8px;color:#666'><b>Severite</b></td>"
        "    <td style='padding:5px 8px;color:%10;font-weight:bold'>%11</td></tr>"
        "<tr style='background:#f0fdf4'><td style='padding:5px 8px;color:#666'><b>Version</b></td><td style='padding:5px 8px'>v%12</td></tr>"
        "<tr><td style='padding:5px 8px;color:#666'><b>Anomalie</b></td>"
        "    <td style='padding:5px 8px;color:%13;font-weight:bold'>%14</td></tr>"
        "<tr style='background:#f0fdf4'><td style='padding:5px 8px;color:#666'><b>Impact</b></td>"
        "    <td style='padding:5px 8px;color:#ea580c;font-weight:bold'>%15 %%</td></tr>"
        "</table></div>"
    ).arg(rec.auditId).arg(rec.transactionId)
     .arg(getActionColor(rec.action), rec.action.toHtmlEscaped())
     .arg(rec.fieldName.isEmpty() ? "--" : rec.fieldName.toHtmlEscaped())
     .arg(rec.oldValue.isEmpty() ? "--" : rec.oldValue.toHtmlEscaped())
     .arg(rec.newValue.isEmpty() ? "--" : rec.newValue.toHtmlEscaped())
     .arg("Utilisateur " + QString::number(rec.changedBy))
     .arg(rec.changedAt)
     .arg(getSeveriteColor(rec.severity), rec.severity)
     .arg(rec.version)
     .arg(rec.isAnomaly ? "#dc2626" : "#16a34a")
     .arg(rec.isAnomaly ? "OUI — Anomalie" : "Non")
     .arg(QString::number(rec.impactScore, 'f', 2));

    QDialog *dlg = new QDialog(this);
    dlg->setWindowTitle("Detail — Audit #" + QString::number(rec.auditId));
    dlg->resize(520, 420);
    dlg->setStyleSheet("background:#ffffff; color:#2c3e50;");
    QVBoxLayout *l = new QVBoxLayout(dlg);
    QLabel *lbl = new QLabel(detail);
    lbl->setWordWrap(true);
    lbl->setStyleSheet("padding:16px;");
    l->addWidget(lbl);
    QPushButton *btnClose = new QPushButton("Fermer");
    btnClose->setStyleSheet(
        "background:#1f5c3d; color:#ffffff; border:none;"
        "border-radius:6px; padding:8px 22px; font-weight:600;");
    connect(btnClose, &QPushButton::clicked, dlg, &QDialog::accept);
    l->addWidget(btnClose, 0, Qt::AlignRight);
    dlg->exec();
    delete dlg;
}

// ═══════════════════════════════════════════════════════════════════════════════
//  EXPORT PDF — CORRIGÉ COMPLET
// ═══════════════════════════════════════════════════════════════════════════════

void HistoriqueAuditTab::exporterPDF()
{
    // FIX 6 : garde si aucune donnée
    if (cachedRecords.isEmpty()) {
        QMessageBox::warning(this, "Export PDF",
            "Aucune donnee a exporter.\nChargez d'abord les donnees.");
        return;
    }

    QString path = QFileDialog::getSaveFileName(
        this, "Exporter en PDF",
        QStandardPaths::writableLocation(QStandardPaths::DownloadLocation)
            + "/audit_historique.pdf",
        "PDF (*.pdf)");
    if (path.isEmpty()) return;

    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(path);
    printer.setPageSize(QPageSize(QPageSize::A4));
    printer.setPageOrientation(QPageLayout::Landscape);  // FIX 4 : paysage
    // FIX 4 : marges propres
    printer.setPageMargins(QMarginsF(10, 10, 10, 10), QPageLayout::Millimeter);

    QTextDocument doc;
    doc.setHtml(genererHTMLRapport());
    // FIX 4 : CRITIQUE — définir la taille avant d'imprimer
    doc.setPageSize(printer.pageRect(QPrinter::Point).size());
    doc.print(&printer);

    // FIX 6 : vérifier que le fichier existe
    if (QFile::exists(path)) {
        QMessageBox::information(this, "Export PDF reussi",
            QString("Rapport exporte : %1 ligne(s)\nFichier : %2")
                .arg(cachedRecords.size()).arg(path));
        QDesktopServices::openUrl(QUrl::fromLocalFile(path));
    } else {
        QMessageBox::critical(this, "Echec export",
            "Le fichier PDF n'a pas ete cree.\nVerifiez les droits d'ecriture.");
    }
}

QString HistoriqueAuditTab::genererHTMLRapport() const
{
    // Calculer les stats pour le résumé (FIX 18)
    int nbAnomalies = 0, nbCritical = 0, nbInserts = 0, nbUpdates = 0, nbDeletes = 0;
    for (const AuditRecord &r : cachedRecords) {
        if (r.isAnomaly)            nbAnomalies++;
        if (r.severity == "CRITICAL") nbCritical++;
        if (r.action == "INSERT")   nbInserts++;
        if (r.action == "UPDATE")   nbUpdates++;
        if (r.action == "DELETE")   nbDeletes++;
    }

    // FIX 17 : en-tête complet + FIX 19 : page-break-inside:avoid
    QString html =
        "<html><head><meta charset='UTF-8'><style>"
        "body{font-family:Arial,sans-serif;font-size:11px;color:#1a202c;margin:0;padding:0;}"
        "h1{color:#1f5c3d;border-bottom:3px solid #1f5c3d;padding-bottom:6px;"
        "   font-size:16px;margin-bottom:4px;}"
        ".info{color:#4a5568;font-size:11px;margin-bottom:10px;}"
        ".resume{border:1px solid #d4e6d8;border-radius:6px;padding:10px;"
        "        margin-bottom:14px;background:#f0fdf4;}"
        ".resume h2{color:#1f5c3d;font-size:13px;margin:0 0 8px;}"
        ".resume table{border-collapse:collapse;}"
        ".resume td{padding:4px 16px;text-align:center;border-right:1px solid #d4e6d8;}"
        ".resume td:last-child{border-right:none;}"
        ".resume .val{font-size:18px;font-weight:bold;display:block;}"
        ".resume .lbl{font-size:10px;color:#666;}"
        "table.main{width:100%;border-collapse:collapse;margin-top:8px;}"
        "table.main th{background:#1f5c3d;color:white;padding:6px 5px;"
        "              font-size:10px;text-align:left;border-right:1px solid #2d7d4f;}"
        "table.main td{padding:4px 5px;border-bottom:1px solid #e2e8f0;font-size:10px;}"
        "table.main tr:nth-child(even) td{background:#f7fafc;}"
        "table.main tr{page-break-inside:avoid;}"       // FIX 19
        ".critical{color:#e53e3e;font-weight:bold;}"
        ".high{color:#dd6b20;font-weight:bold;}"
        ".medium{color:#d69e2e;}"
        ".low{color:#38a169;}"
        ".anomaly td{background:#fff5f5!important;}"
        "</style></head><body>"

        // FIX 17 : En-tête
        "<h1>Smart Oil Press Management — Rapport d'Audit Financier</h1>"
        "<div class='info'>"
        "Genere le : <b>" + QDateTime::currentDateTime().toString("dd/MM/yyyy a HH:mm:ss") + "</b>"
        " &nbsp;|&nbsp; Total lignes exportees : <b>" + QString::number(cachedRecords.size()) + "</b>"
        + (filtreEstActif() ? " &nbsp;|&nbsp; <i>Filtre actif</i>" : "") +
        "</div>"

        // FIX 18 : Tableau résumé statistiques
        "<div class='resume'><h2>Resume statistiques</h2>"
        "<table><tr>"
        "<td><span class='val'>" + QString::number(cachedRecords.size()) + "</span><span class='lbl'>Total</span></td>"
        "<td><span class='val' style='color:#16a34a'>" + QString::number(nbInserts) + "</span><span class='lbl'>Inserts</span></td>"
        "<td><span class='val' style='color:#ca8a04'>" + QString::number(nbUpdates) + "</span><span class='lbl'>Updates</span></td>"
        "<td><span class='val' style='color:#e53e3e'>" + QString::number(nbDeletes) + "</span><span class='lbl'>Deletes</span></td>"
        "<td><span class='val' style='color:#ea580c'>" + QString::number(nbAnomalies) + "</span><span class='lbl'>Anomalies</span></td>"
        "<td><span class='val' style='color:#e53e3e'>" + QString::number(nbCritical) + "</span><span class='lbl'>Critiques</span></td>"
        "</tr></table></div>"

        "<table class='main'><tr>"
        "<th>ID</th><th>Trans.</th><th>Action</th><th>Champ</th>"
        "<th>Ancienne valeur</th><th>Nouvelle valeur</th>"
        "<th>Utilisateur</th><th>Date/Heure</th>"
        "<th>Severite</th><th>Anomalie</th><th>Impact</th>"
        "</tr>";

    for (const AuditRecord &rec : cachedRecords) {
        // FIX 5 : toHtmlEscaped() sur toutes les valeurs
        QString rowClass = rec.isAnomaly ? " class='anomaly'" : "";
        QString sevClass = rec.severity.toLower();
        html += QString("<tr%1>"
                "<td>%2</td><td>%3</td>"
                "<td><b>%4</b></td>"
                "<td>%5</td>"
                "<td>%6</td>"
                "<td>%7</td>"
                "<td>U%8</td>"
                "<td>%9</td>"
                "<td class='%10'>%11</td>"
                "<td>%12</td>"
                "<td>%13%%</td>"
                "</tr>")
            .arg(rowClass)
            .arg(rec.auditId)
            .arg(rec.transactionId)
            .arg(rec.action.toHtmlEscaped())
            .arg(rec.fieldName.toHtmlEscaped())
            .arg(rec.oldValue.left(35).toHtmlEscaped())   // FIX 5
            .arg(rec.newValue.left(35).toHtmlEscaped())   // FIX 5
            .arg(rec.changedBy)
            .arg(rec.changedAt.toHtmlEscaped())
            .arg(sevClass)
            .arg(rec.severity.toHtmlEscaped())
            .arg(rec.isAnomaly ? "OUI" : "non")
            .arg(QString::number(rec.impactScore, 'f', 1));
    }

    html += "</table></body></html>";
    return html;
}

// ═══════════════════════════════════════════════════════════════════════════════
//  EXPORT CSV
// ═══════════════════════════════════════════════════════════════════════════════

void HistoriqueAuditTab::exporterCSV()
{
    if (cachedRecords.isEmpty()) {
        QMessageBox::warning(this, "Export CSV", "Aucune donnee a exporter.");
        return;
    }

    QString path = QFileDialog::getSaveFileName(
        this, "Exporter en CSV",
        QStandardPaths::writableLocation(QStandardPaths::DownloadLocation)
            + "/audit_historique.csv",
        "CSV (*.csv)");
    if (path.isEmpty()) return;
    ecrireCSV(path);
}

void HistoriqueAuditTab::ecrireCSV(const QString &chemin) const
{
    QFile file(chemin);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "Erreur ouverture CSV";
        return;
    }
    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);

    // BOM UTF-8 pour Excel
    out << "\xEF\xBB\xBF";
    out << "ID,Transaction,Action,Champ,Ancienne valeur,Nouvelle valeur,"
           "Utilisateur,Date,Severite,Version,Anomalie,Impact\n";

    for (const AuditRecord &rec : cachedRecords) {
        // Use local copies so replace() can be called without discarding const qualifiers
        QString fieldName = rec.fieldName;
        QString oldValue  = rec.oldValue;
        QString newValue  = rec.newValue;
        out << rec.auditId         << ","
            << rec.transactionId   << ","
            << rec.action          << ","
            << "\"" << fieldName.replace('"', '\'') << "\","
            << "\"" << oldValue .replace('"', '\'') << "\","
            << "\"" << newValue .replace('"', '\'') << "\","
            << rec.changedBy       << ","
            << "\"" << rec.changedAt << "\","
            << rec.severity        << ","
            << rec.version         << ","
            << (rec.isAnomaly ? "OUI" : "non") << ","
            << QString::number(rec.impactScore, 'f', 2) << "\n";
    }
    file.close();

    QMessageBox::information(nullptr, "Export CSV reussi",
        "Fichier exporte :\n" + chemin);
    QDesktopServices::openUrl(QUrl::fromLocalFile(chemin));
}

// ═══════════════════════════════════════════════════════════════════════════════
//  HELPERS VISUELS
// ═══════════════════════════════════════════════════════════════════════════════

QString HistoriqueAuditTab::getSeveriteColor(const QString &s) const
{
    if (s == "CRITICAL") return "#dc2626";
    if (s == "HIGH")     return "#ea580c";
    if (s == "MEDIUM")   return "#ca8a04";
    return "#16a34a";
}

QString HistoriqueAuditTab::getSeveriteTextColor(const QString &s) const
{
    if (s == "MEDIUM") return "#1a1200";
    return "#ffffff";
}

QString HistoriqueAuditTab::getActionIcon(const QString &a) const
{
    if (a == "INSERT")   return "[+]";
    if (a == "UPDATE")   return "[~]";
    if (a == "DELETE")   return "[x]";
    if (a == "ROLLBACK") return "[<]";
    if (a == "ALERTE")   return "[!]";
    return "[ ]";
}

QString HistoriqueAuditTab::getActionColor(const QString &a) const
{
    if (a == "INSERT")   return "#16a34a";
    if (a == "UPDATE")   return "#2563eb";
    if (a == "DELETE")   return "#dc2626";
    if (a == "ROLLBACK") return "#ea580c";
    if (a == "ALERTE")   return "#ca8a04";
    return "#2c3e50";
}

QString HistoriqueAuditTab::formaterMontant(const QString &v) const
{
    bool ok;
    double d = v.toDouble(&ok);
    return ok ? QString::number(d, 'f', 3) + " DT" : v;
}

QColor HistoriqueAuditTab::couleurImpact(double score) const
{
    if (score >= 75) return QColor("#dc2626");
    if (score >= 50) return QColor("#ea580c");
    if (score >= 25) return QColor("#ca8a04");
    return QColor("#16a34a");
}

void HistoriqueAuditTab::refreshData() { chargerHistorique(); }

// ═══════════════════════════════════════════════════════════════════════════════
//  RACCOURCIS CLAVIER
// ═══════════════════════════════════════════════════════════════════════════════
void HistoriqueAuditTab::setupRaccourcis()
{
    new QShortcut(QKeySequence("Ctrl+R"), this, [this](){ chargerHistorique(); });
    new QShortcut(QKeySequence(Qt::Key_F5), this, [this](){ chargerHistorique(); });
    new QShortcut(QKeySequence("Ctrl+E"), this, [this](){ exporterCSV(); });
    new QShortcut(QKeySequence("Ctrl+P"), this, [this](){ exporterPDF(); });
    new QShortcut(QKeySequence(Qt::Key_Escape), this, [this](){ viderFiltres(); });
    new QShortcut(QKeySequence("Ctrl+C"), this, [this](){ copierLignePressePapier(); });
}

// ═══════════════════════════════════════════════════════════════════════════════
//  ONGLET ANALYTIQUE (CHARTS)
// ═══════════════════════════════════════════════════════════════════════════════
QWidget* HistoriqueAuditTab::creerOngletAnalytique()
{
    QWidget *w = new QWidget();
    QVBoxLayout *lay = new QVBoxLayout(w);
    lay->setSpacing(10);
    lay->setContentsMargins(12, 12, 12, 12);

    QLabel *title = new QLabel("  Analytique & Graphiques");
    title->setObjectName("analyticTitle");
    lay->addWidget(title);

    auto makeFrame = [](const QString &label) -> QFrame* {
        QFrame *f = new QFrame();
        f->setObjectName("chartFrame");
        QVBoxLayout *fl = new QVBoxLayout(f);
        fl->setContentsMargins(8, 6, 8, 8);
        QLabel *lbl = new QLabel(label);
        lbl->setObjectName("chartTitle");
        fl->addWidget(lbl);
        return f;
    };

    QGridLayout *grid = new QGridLayout();
    grid->setSpacing(10);

    QFrame *pieFrame = makeFrame("Repartition des Actions");
    chartViewPie = new QChartView();
    chartViewPie->setRenderHint(QPainter::Antialiasing);
    chartViewPie->setMinimumHeight(230);
    pieFrame->layout()->addWidget(chartViewPie);
    grid->addWidget(pieFrame, 0, 0);

    QFrame *barFrame = makeFrame("Activite par Heure de la Journee");
    chartViewBar = new QChartView();
    chartViewBar->setRenderHint(QPainter::Antialiasing);
    chartViewBar->setMinimumHeight(230);
    barFrame->layout()->addWidget(chartViewBar);
    grid->addWidget(barFrame, 0, 1);

    // ── Frame "Anomalies sur N jours" avec barre de contrôle organisée ────────
    QFrame *lineFrame = new QFrame();
    lineFrame->setObjectName("chartFrame");
    QVBoxLayout *lineLayout = new QVBoxLayout(lineFrame);
    lineLayout->setContentsMargins(8, 6, 8, 8);
    lineLayout->setSpacing(6);

    // ── En-tête : titre à gauche, contrôles à droite ─────────────────────────
    QWidget *lineHeader = new QWidget();
    QHBoxLayout *lineHdr = new QHBoxLayout(lineHeader);
    lineHdr->setContentsMargins(0, 0, 0, 0);
    lineHdr->setSpacing(10);

    QLabel *lineTitleLbl = new QLabel("  Activite & Anomalies");
    lineTitleLbl->setObjectName("chartTitle");

    // Filtre Période
    QLabel *lPer = new QLabel("Periode :");
    lPer->setObjectName("filterLabel");
    cbLineFilterPeriod = new QComboBox();
    cbLineFilterPeriod->setObjectName("filterCombo");
    cbLineFilterPeriod->addItem(" 7 jours",  7);
    cbLineFilterPeriod->addItem("15 jours",  15);
    cbLineFilterPeriod->addItem("30 jours",  30);
    cbLineFilterPeriod->addItem("90 jours",  90);
    cbLineFilterPeriod->setCurrentIndex(2);  // 30j par defaut
    cbLineFilterPeriod->setMinimumWidth(90);
    cbLineFilterPeriod->setToolTip("Nombre de jours a afficher");

    // Filtre Type
    QLabel *lTyp = new QLabel("Vue :");
    lTyp->setObjectName("filterLabel");
    cbLineFilterType = new QComboBox();
    cbLineFilterType->setObjectName("filterCombo");
    cbLineFilterType->addItem("Tout afficher",     0);
    cbLineFilterType->addItem("Anomalies seules",  1);
    cbLineFilterType->addItem("Critiques seules",  2);
    cbLineFilterType->setMinimumWidth(140);
    cbLineFilterType->setToolTip("Filtrer les evenements affiches");

    // Bouton Refresh
    btnLineRefresh = new QPushButton("  Rafraichir");
    btnLineRefresh->setObjectName("btnPrimary");
    btnLineRefresh->setCursor(Qt::PointingHandCursor);
    btnLineRefresh->setToolTip("Recharger le graphique (F5)");
    btnLineRefresh->setMinimumWidth(110);

    // Label horodatage
    lblLineLastUpdate = new QLabel("Jamais actualise");
    lblLineLastUpdate->setObjectName("resultCount");
    lblLineLastUpdate->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    lineHdr->addWidget(lineTitleLbl);
    lineHdr->addStretch();
    lineHdr->addWidget(lPer);
    lineHdr->addWidget(cbLineFilterPeriod);
    lineHdr->addWidget(lTyp);
    lineHdr->addWidget(cbLineFilterType);
    lineHdr->addWidget(btnLineRefresh);
    lineHdr->addWidget(lblLineLastUpdate);
    lineLayout->addWidget(lineHeader);

    // Séparateur visuel
    QFrame *lineSep = new QFrame();
    lineSep->setFrameShape(QFrame::HLine);
    lineSep->setObjectName("separator");
    lineLayout->addWidget(lineSep);

    // Zone graphique
    chartViewLine = new QChartView();
    chartViewLine->setRenderHint(QPainter::Antialiasing);
    chartViewLine->setMinimumHeight(200);
    lineLayout->addWidget(chartViewLine);

    // Connexions : filtres → refresh automatique du graphique
    connect(btnLineRefresh,
            &QPushButton::clicked,
            this, &HistoriqueAuditTab::construireGraphiqueLine);
    connect(cbLineFilterPeriod,
            QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &HistoriqueAuditTab::construireGraphiqueLine);
    connect(cbLineFilterType,
            QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &HistoriqueAuditTab::construireGraphiqueLine);

    grid->addWidget(lineFrame, 1, 0);

    QFrame *impFrame = makeFrame("Distribution des Scores d'Impact");
    chartViewImpact = new QChartView();
    chartViewImpact->setRenderHint(QPainter::Antialiasing);
    chartViewImpact->setMinimumHeight(200);
    impFrame->layout()->addWidget(chartViewImpact);
    grid->addWidget(impFrame, 1, 1);

    lay->addLayout(grid, 1);

    QPushButton *btnR = new QPushButton("  Actualiser les graphiques");
    btnR->setObjectName("btnPrimary");
    connect(btnR, &QPushButton::clicked, this, &HistoriqueAuditTab::mettreAJourGraphiques);
    QHBoxLayout *bl = new QHBoxLayout();
    bl->addStretch();
    bl->addWidget(btnR);
    lay->addLayout(bl);

    return w;
}

// ─── Onglet Timeline ──────────────────────────────────────────────────────────
QWidget* HistoriqueAuditTab::creerOngletTimeline()
{
    QWidget *w = new QWidget();
    QVBoxLayout *lay = new QVBoxLayout(w);
    lay->setSpacing(8);
    lay->setContentsMargins(12, 12, 12, 12);

    QLabel *title = new QLabel("  Timeline des Evenements d'Audit");
    title->setObjectName("analyticTitle");
    lay->addWidget(title);

    QHBoxLayout *fl = new QHBoxLayout();
    QLabel *l1 = new QLabel("Afficher les");
    l1->setObjectName("filterLabel");
    timelineLimitSpin = new QSpinBox();
    timelineLimitSpin->setRange(10, 500);
    timelineLimitSpin->setValue(50);
    timelineLimitSpin->setObjectName("filterSearch");
    QLabel *l2 = new QLabel("derniers evenements");
    l2->setObjectName("filterLabel");
    QPushButton *btnTl = new QPushButton("Actualiser");
    btnTl->setObjectName("btnPrimary");
    fl->addWidget(l1);
    fl->addWidget(timelineLimitSpin);
    fl->addWidget(l2);
    fl->addStretch();
    fl->addWidget(btnTl);
    lay->addLayout(fl);

    timelineList = new QListWidget();
    timelineList->setObjectName("timelineList");
    timelineList->setSpacing(2);
    lay->addWidget(timelineList, 1);

    connect(btnTl, &QPushButton::clicked, this, &HistoriqueAuditTab::mettreAJourTimeline);
    return w;
}

// ═══════════════════════════════════════════════════════════════════════════════
//  GRAPHIQUES
// ═══════════════════════════════════════════════════════════════════════════════
void HistoriqueAuditTab::mettreAJourGraphiques()
{
    construireGraphiquePie();
    construireGraphiqueBar();
    construireGraphiqueLine();
    construireGraphiqueImpact();
}

void HistoriqueAuditTab::construireGraphiquePie()
{
    if (!chartViewPie || !dbConn || !dbConn->getDatabase().isOpen()) return;
    QSqlDatabase db = dbConn->getDatabase();
    QPieSeries *series = new QPieSeries();
    series->setHoleSize(0.38);  // donut style

    // Donut style avec labels exterieurs
    struct Act { QString dbName; QString frName; QString color; };
    QList<Act> acts = {
        {"INSERT",   "Insertion",   "#16a34a"},
        {"UPDATE",   "Modification","#2563eb"},
        {"DELETE",   "Suppression", "#dc2626"},
        {"ROLLBACK", "Restauration","#ea580c"},
        {"ALERTE",   "Alerte",      "#ca8a04"}
    };
    int total = 0;
    for (auto &a : acts) {
        QSqlQuery q(db);
        q.exec(QString("SELECT COUNT(*) FROM FINANCE WHERE TYPE_LIGNE='AUDIT' AND AUDIT_ACTION='%1'").arg(a.dbName));
        if (q.next() && q.value(0).toInt() > 0) {
            int cnt = q.value(0).toInt();
            total += cnt;
            auto *sl = series->append(a.frName, cnt);
            sl->setColor(QColor(a.color));
            sl->setBorderColor(Qt::white);
        }
    }
    for (auto *sl : series->slices()) {
        int pct = total > 0 ? qRound(sl->value() * 100.0 / total) : 0;
        // Met le nom, la valeur et le pourcentage dans le Label (qui sera affiché proprement dans la légende)
        sl->setLabel(QString("%1 %2 (%3%)").arg(sl->label()).arg((int)sl->value()).arg(pct));
        
        // On cache le label sur le Pie lui-même pour ne plus avoir ces lignes moches qui se superposent
        sl->setLabelVisible(false); 
    }
    QChart *c = new QChart();
    c->addSeries(series);
    c->setTitle("");
    c->setBackgroundBrush(QBrush(Qt::white));
    
    // On s'assure que la légende est bien affichée en bas avec une bonne police
    c->legend()->setVisible(true);
    c->legend()->setAlignment(Qt::AlignBottom);
    c->legend()->setFont(QFont("Segoe UI", 10, QFont::Bold));
    c->setAnimationOptions(QChart::AllAnimations);
    c->setAnimationDuration(700);
    c->legend()->setAlignment(Qt::AlignBottom);
    c->legend()->setFont(QFont("Segoe UI", 9));
    c->setMargins(QMargins(4, 4, 4, 4));
    chartViewPie->setChart(c);
    chartViewPie->setBackgroundBrush(QBrush(Qt::white));
}

void HistoriqueAuditTab::construireGraphiqueBar()
{
    if (!chartViewBar) return;
    QBarSet *sI = new QBarSet("Insertion");    sI->setColor(QColor("#16a34a")); sI->setBorderColor(Qt::white);
    QBarSet *sU = new QBarSet("Modification"); sU->setColor(QColor("#2563eb")); sU->setBorderColor(Qt::white);
    QBarSet *sD = new QBarSet("Suppression");  sD->setColor(QColor("#dc2626")); sD->setBorderColor(Qt::white);
    QStringList hours;
    for (int h = 0; h < 24; h += 4)
        hours << QString("%1-%2h").arg(h,2,10,QChar('0')).arg(h+3,2,10,QChar('0'));

    QVector<int> cntI(6,0), cntU(6,0), cntD(6,0);
    for (const AuditRecord &r : cachedRecords) {
        int hour = -1; bool ok = false;
        if (r.changedAt.length() >= 11) {
            hour = r.changedAt.mid(9, 2).toInt(&ok);
            if (!ok || hour < 0 || hour > 23)
                hour = r.changedAt.mid(11, 2).toInt(&ok);
        }
        if (!ok || hour < 0 || hour > 23) continue;
        int b = hour / 4;
        if      (r.action == "INSERT") cntI[b]++;
        else if (r.action == "UPDATE") cntU[b]++;
        else if (r.action == "DELETE") cntD[b]++;
    }
    // Fallback DB si cachedRecords vide
    if (cachedRecords.isEmpty() && dbConn && dbConn->getDatabase().isOpen()) {
        QSqlDatabase db = dbConn->getDatabase();
        QStringList acts = {"INSERT","UPDATE","DELETE"};
        for (int h = 0; h < 24; h += 4) {
            int b = h/4;
            for (int k = 0; k < 3; ++k) {
                QSqlQuery q(db);
                q.exec(QString("SELECT COUNT(*) FROM FINANCE WHERE TYPE_LIGNE='AUDIT' AND AUDIT_ACTION='%1' "
                               "AND CAST(TO_CHAR(CREATED_AT, 'HH24') AS INTEGER) BETWEEN %2 AND %3")
                       .arg(acts[k]).arg(h).arg(h+3));
                int v = q.next() ? q.value(0).toInt() : 0;
                if (k==0) cntI[b]=v; else if(k==1) cntU[b]=v; else cntD[b]=v;
            }
        }
    }
    for (int b = 0; b < 6; b++) { *sI << cntI[b]; *sU << cntU[b]; *sD << cntD[b]; }

    QBarSeries *series = new QBarSeries();
    series->append(sI); series->append(sU); series->append(sD);
    series->setBarWidth(0.7);
    QChart *c = new QChart();
    c->addSeries(series);
    c->setTitle("");
    c->setBackgroundBrush(QBrush(Qt::white));
    c->setAnimationOptions(QChart::SeriesAnimations);
    c->setAnimationDuration(600);
    QBarCategoryAxis *axX = new QBarCategoryAxis(); axX->append(hours);
    axX->setLabelsFont(QFont("Segoe UI", 8));
    c->addAxis(axX, Qt::AlignBottom); series->attachAxis(axX);
    QValueAxis *axY = new QValueAxis(); axY->setLabelFormat("%d");
    c->addAxis(axY, Qt::AlignLeft); series->attachAxis(axY);
    c->legend()->setAlignment(Qt::AlignBottom);
    c->legend()->setFont(QFont("Segoe UI", 9));
    c->setMargins(QMargins(4,4,4,4));
    chartViewBar->setChart(c);
    chartViewBar->setBackgroundBrush(QBrush(Qt::white));
}

void HistoriqueAuditTab::construireGraphiqueLine()
{
    if (!chartViewLine) return;

    // ── 1. Trouver la date max par rapport aux donnees ────────────────────
    QDate referenceDate = QDate::currentDate(); // Date par defaut sil n'y a pas de date ou si db vide
    if (!cachedRecords.isEmpty()) {
        QDate maxDate;
        for (const AuditRecord &r : cachedRecords) {
            QString key = normaliseDateISO(r.changedAt);
            if (!key.isEmpty()) {
                QDate d = QDate::fromString(key, "yyyy-MM-dd");
                if (d.isValid() && (!maxDate.isValid() || d > maxDate)) {
                    maxDate = d;
                }
            }
        }
        if (maxDate.isValid()) {
            referenceDate = maxDate; // On s'appuie sur la vraie derniere date existante dans la BD
        }
    }

    // ── 2. Compter par jour en utilisant normaliseDateISO() ────────────────
    QMap<QString,int> totPerDay, anomPerDay;

    if (!cachedRecords.isEmpty()) {
        for (const AuditRecord &r : cachedRecords) {
            QString key = normaliseDateISO(r.changedAt);
            if (!key.isEmpty()) {
                totPerDay[key]++;
                if (r.isAnomaly) anomPerDay[key]++;
            }
        }
    } else if (dbConn && dbConn->getDatabase().isOpen()) {
        // Fallback DB Oracle
        QSqlDatabase db = dbConn->getDatabase();
        for (int i = 29; i >= 0; i--) {
            QString iso = referenceDate.addDays(-i).toString("yyyy-MM-dd");
            QSqlQuery q(db);
            q.exec(QString("SELECT COUNT(*) FROM FINANCE WHERE TYPE_LIGNE='AUDIT' AND "
                           "TRUNC(CREATED_AT) = TO_DATE('%1','YYYY-MM-DD')").arg(iso));
            if (q.next()) totPerDay[iso] = q.value(0).toInt();
            QSqlQuery q2(db);
            q2.exec(QString("SELECT COUNT(*) FROM FINANCE WHERE TYPE_LIGNE='AUDIT' "
                            "AND AUDIT_EST_ANOMALIE=1 AND "
                            "TRUNC(CREATED_AT) = TO_DATE('%1','YYYY-MM-DD')").arg(iso));
            if (q2.next()) anomPerDay[iso] = q2.value(0).toInt();
        }
    }

    // ── 3. Séries QLineSeries (données en millisecondes pour QDateTimeAxis) ─
    QLineSeries *sAll = new QLineSeries(); sAll->setName("Total");
    QLineSeries *sAno = new QLineSeries(); sAno->setName("Anomalies");

    QPen p1(QColor("#2563eb")); p1.setWidth(2); p1.setCapStyle(Qt::RoundCap);
    sAll->setPen(p1);
    QPen p2(QColor("#dc2626")); p2.setWidth(2); p2.setCapStyle(Qt::RoundCap);
    sAno->setPen(p2);
    sAll->setPointsVisible(true);
    sAno->setPointsVisible(true);

    // ── 4. Remplir les séries avec l'axe X en epoch-ms ────────────────────
    // QDateTimeAxis exige que les X soient en millisecondes depuis l'epoch.
    int maxVal = 0;
    for (int i = 0; i < 30; i++) {
        QDate day = referenceDate.addDays(i - 29);
        QString iso = day.toString("yyyy-MM-dd");
        int t = totPerDay.value(iso, 0);
        int a = anomPerDay.value(iso, 0);

        // Midi pour éviter les décalages DST
        qint64 ms = QDateTime(day, QTime(12, 0)).toMSecsSinceEpoch();
        sAll->append(ms, static_cast<double>(t));
        sAno->append(ms, static_cast<double>(a));
        if (t > maxVal) maxVal = t;
    }

    // ── 5. Graphique ───────────────────────────────────────────────────────
    QChart *c = new QChart();
    c->addSeries(sAll);
    c->addSeries(sAno);
    c->setTitle("Activite des 30 derniers jours");
    c->setTitleFont(QFont("Segoe UI", 10, QFont::Bold));
    c->setTitleBrush(QBrush(QColor("#1f5c3d")));
    c->setBackgroundBrush(QBrush(Qt::white));
    c->setAnimationOptions(QChart::SeriesAnimations);
    c->setAnimationDuration(700);
    c->setMargins(QMargins(4, 4, 4, 4));

    // ── 6. Axe X : QDateTimeAxis — SEUL axe compatible avec QLineSeries ───
    QDateTimeAxis *axX = new QDateTimeAxis();
    axX->setFormat("dd/MM");           // Affiche : 21/03, 26/03...
    axX->setTickCount(7);              // ~1 tick tous les 5 jours sur 30 jours
    axX->setLabelsFont(QFont("Segoe UI", 8));
    axX->setGridLineVisible(true);
    // Plage : du jour J-29 à J (midi pour éviter DST)
    axX->setRange(
        QDateTime(referenceDate.addDays(-29), QTime(12, 0)),
        QDateTime(referenceDate,              QTime(12, 0))
    );
    c->addAxis(axX, Qt::AlignBottom);
    sAll->attachAxis(axX);
    sAno->attachAxis(axX);

    // ── 7. Axe Y : QValueAxis — entiers, labels lisibles ──────────────────
    QValueAxis *axY = new QValueAxis();
    axY->setLabelFormat("%.0f");  // Entier sans virgule : 0, 1, 2, 3...
    axY->setMin(0.0);
    int maxLimit = maxVal > 0 ? (maxVal + qMax(1, maxVal / 5)) : 5;
    axY->setMax(static_cast<double>(maxLimit));
    int tickCount = qBound(2, maxLimit + 1, 6); // Entre 2 et 6 graduations
    axY->setTickCount(tickCount);
    axY->setMinorTickCount(0);
    axY->setLabelsFont(QFont("Segoe UI", 8));
    axY->setGridLineVisible(true);
    c->addAxis(axY, Qt::AlignLeft);
    sAll->attachAxis(axY);
    sAno->attachAxis(axY);

    c->legend()->setAlignment(Qt::AlignBottom);
    c->legend()->setFont(QFont("Segoe UI", 9));

    chartViewLine->setChart(c);
    chartViewLine->setBackgroundBrush(QBrush(Qt::white));
}

void HistoriqueAuditTab::construireGraphiqueImpact()
{
    if (!chartViewImpact || !dbConn || !dbConn->getDatabase().isOpen()) return;
    QSqlDatabase db = dbConn->getDatabase();
    QBarSet *bs = new QBarSet("Enregistrements"); bs->setColor(QColor("#1f5c3d"));
    QStringList cats = {"0-25%","25-50%","50-75%","75-100%"};
    int ranges[][2] = {{0,25},{25,50},{50,75},{75,100}};
    for (auto &r : ranges) {
        QSqlQuery q(db);
        q.exec(QString("SELECT COUNT(*) FROM FINANCE WHERE TYPE_LIGNE='AUDIT' AND AUDIT_IMPACT_SCORE>=%1 AND AUDIT_IMPACT_SCORE<%2").arg(r[0]).arg(r[1]));
        *bs << (q.next() ? q.value(0).toInt() : 0);
    }
    QBarSeries *series = new QBarSeries(); series->append(bs);
    QChart *c = new QChart();
    c->addSeries(series);
    c->setTitle("Distribution Impact");
    c->setTitleFont(QFont("Segoe UI", 10, QFont::Bold));
    c->setBackgroundBrush(QBrush(Qt::white));
    c->setTitleBrush(QBrush(QColor("#1f5c3d")));
    QBarCategoryAxis *axX = new QBarCategoryAxis(); axX->append(cats);
    c->addAxis(axX, Qt::AlignBottom); series->attachAxis(axX);
    QValueAxis *axY = new QValueAxis(); c->addAxis(axY, Qt::AlignLeft); series->attachAxis(axY);
    c->legend()->hide();
    chartViewImpact->setChart(c);
    chartViewImpact->setBackgroundBrush(QBrush(Qt::white));
}

// ═══════════════════════════════════════════════════════════════════════════════
//  TIMELINE
// ═══════════════════════════════════════════════════════════════════════════════
void HistoriqueAuditTab::mettreAJourTimeline()
{
    if (!timelineList) return;
    timelineList->clear();

    int limit = timelineLimitSpin ? timelineLimitSpin->value() : 50;

    struct TLItem { QString action, field, changedAt, severity; bool isAnomaly; double impactScore; int auditId; };
    QList<TLItem> items;

    if (!cachedRecords.isEmpty()) {
        for (int i = 0; i < qMin(limit, (int)cachedRecords.size()); ++i) {
            const AuditRecord &r = cachedRecords[i];
            items.append({r.action, r.fieldName, r.changedAt, r.severity, r.isAnomaly, r.impactScore, r.auditId});
        }
    } else if (dbConn && dbConn->getDatabase().isOpen()) {
        QSqlQuery q(dbConn->getDatabase());
        q.exec(QString("SELECT ID_TRANSACTION, AUDIT_ACTION, AUDIT_CHAMP, CREATED_AT, AUDIT_SEVERITE, AUDIT_EST_ANOMALIE, AUDIT_IMPACT_SCORE "
                       "FROM FINANCE WHERE TYPE_LIGNE='AUDIT' ORDER BY CREATED_AT DESC FETCH FIRST %1 ROWS ONLY").arg(limit));
        while (q.next()) {
            items.append({q.value(1).toString(), q.value(2).toString(), q.value(3).toString(),
                          q.value(4).toString(), q.value(5).toInt() == 1, q.value(6).toDouble(), q.value(0).toInt()});
        }
    }

    if (items.isEmpty()) {
        QListWidgetItem *msg = new QListWidgetItem("  Aucune donnee. Chargez d'abord le Journal d'Audit.");
        msg->setForeground(QColor("#888888"));
        timelineList->addItem(msg);
        return;
    }

    QString currentDate;
    int total = items.size(), nbAno = 0, nbCrit = 0;

    for (const TLItem &it : items) {
        if (it.isAnomaly) nbAno++;
        if (it.severity == "CRITICAL") nbCrit++;

        // Utilisation du helper normaliseDateISO !
        QString dateStr = normaliseDateISO(it.changedAt);
        
        if (dateStr != currentDate && !dateStr.isEmpty()) {
            currentDate = dateStr;
            QDate d = QDate::fromString(dateStr, "yyyy-MM-dd");
            QString isToday = (d == QDate::currentDate()) ? "  — Aujourd'hui" : "";
            QString dayLabel = d.isValid()
                ? ("   " + d.toString("dddd dd MMMM yyyy") + isToday)
                : ("   " + dateStr);
            QListWidgetItem *sep = new QListWidgetItem(dayLabel);
            sep->setBackground(QColor("#1f5c3d"));
            sep->setForeground(QColor("#ffffff"));
            QFont fs("Segoe UI", 10, QFont::Bold); sep->setFont(fs);
            sep->setFlags(sep->flags() & ~Qt::ItemIsSelectable);
            sep->setSizeHint(QSize(-1, 28));
            timelineList->addItem(sep);
        }

        // Utilisation du helper extraireHeure !
        QString time = extraireHeure(it.changedAt);
        QString icon = getActionIcon(it.action);
        QString anom = it.isAnomaly ? "  [ANOMALIE]" : "";

        // Formater proprement sans bords pipes excessifs
        QString text = QString("  [%1]   %2 %3   ➔   %4   •   Sévérité: %5   •   Impact: %6%%7")
               .arg(time)
               .arg(icon)
               .arg(it.action.leftJustified(6))
               .arg((it.field.isEmpty() ? "Général" : it.field).leftJustified(15))
               .arg(it.severity.leftJustified(6))
               .arg(QString::number(it.impactScore, 'f', 1).rightJustified(4))
               .arg(anom);

        QListWidgetItem *item = new QListWidgetItem(text);
        
        // Couleur de fond par défault transparente pour respecter le CSS
        if (it.isAnomaly) {
            item->setBackground(QColor("#fee2e2")); // Rouge très clair
            item->setForeground(QColor("#991b1b")); // Texte rouge foncé
        } else if (it.severity == "CRITICAL") {
            item->setBackground(QColor("#ffedd5")); // Orange/alerte clair
            item->setForeground(QColor("#9a3412")); // Texte orange foncé
        } else {
            item->setBackground(Qt::transparent);
            // Couleur de la police selon l'action (plus foncée pour lisibilité sur fond clair)
            if (it.action == "INSERT") item->setForeground(QColor("#166534")); // Vert foncé
            else if (it.action == "UPDATE") item->setForeground(QColor("#b45309")); // Orange-marron foncé
            else if (it.action == "DELETE") item->setForeground(QColor("#991b1b")); // Rouge foncé
            else item->setForeground(QColor("#374151")); // Gris foncé
        }

        QFont f("Consolas", 10);
        f.setBold(true); // Toujours bold pour mieux ressortir
        item->setFont(f);
        item->setSizeHint(QSize(-1, 30));
        timelineList->addItem(item);
    }

    QListWidgetItem *footer = new QListWidgetItem(
        QString("   Total: %1 evenements  |  Anomalies: %2  |  Critiques: %3")
        .arg(total).arg(nbAno).arg(nbCrit));
    footer->setBackground(QColor("#238636")); // Vert succés github
    footer->setForeground(QColor("#ffffff"));
    QFont ff("Segoe UI", 9, QFont::Bold); footer->setFont(ff);
    footer->setFlags(footer->flags() & ~Qt::ItemIsSelectable);
    footer->setSizeHint(QSize(-1, 28));
    timelineList->addItem(footer);
}

// ═══════════════════════════════════════════════════════════════════════════════
//  REVISION & VALIDATION
// ═══════════════════════════════════════════════════════════════════════════════
//  ALERTES TEMPS REEL
// ═══════════════════════════════════════════════════════════════════════════════
void HistoriqueAuditTab::verifierNouvellesAlertes()
{
    if (!dbConn || !dbConn->getDatabase().isOpen()) return;
    QSqlQuery q(dbConn->getDatabase());
    q.exec("SELECT MAX(ID_TRANSACTION) FROM FINANCE WHERE TYPE_LIGNE='AUDIT'");
    if (!q.next()) return;
    int maxId = q.value(0).toInt();
    if (dernierAuditIdConnu == 0) { dernierAuditIdConnu = maxId; return; }
    if (maxId <= dernierAuditIdConnu) return;
    q.exec(QString("SELECT COUNT(*) FROM FINANCE WHERE TYPE_LIGNE='AUDIT' AND ID_TRANSACTION>%1 AND AUDIT_SEVERITE='CRITICAL'")
           .arg(dernierAuditIdConnu));
    if (q.next() && q.value(0).toInt() > 0)
        QMessageBox::warning(this, "Alerte Temps Reel",
            QString("%1 nouvel(s) enregistrement(s) CRITICAL detecte(s) !").arg(q.value(0).toInt()));
    dernierAuditIdConnu = maxId;
    chargerHistorique();
}

// ═══════════════════════════════════════════════════════════════════════════════
//  PRESSE-PAPIER & COLONNES
// ═══════════════════════════════════════════════════════════════════════════════
void HistoriqueAuditTab::copierLignePressePapier()
{
    if (currentSelectedRow < 0 || currentSelectedRow >= cachedRecords.size()) return;
    const AuditRecord &rec = cachedRecords[currentSelectedRow];
    QString text = QString("ID:%1|Trans:%2|Action:%3|Champ:%4|Avant:%5|Apres:%6|User:%7|Date:%8|Sev:%9|Impact:%10%")
        .arg(rec.auditId).arg(rec.transactionId).arg(rec.action).arg(rec.fieldName)
        .arg(rec.oldValue).arg(rec.newValue).arg(rec.changedBy)
        .arg(rec.changedAt).arg(rec.severity)
        .arg(QString::number(rec.impactScore,'f',1));
    QApplication::clipboard()->setText(text);
    if (lblStatutTableau)
        lblStatutTableau->setText("  Ligne copiee dans le presse-papier (Ctrl+C).");
}

void HistoriqueAuditTab::onHeaderContextMenu(const QPoint &pos)
{
    QHeaderView *hv = tableAudit->horizontalHeader();
    QMenu menu(this);
    for (int i = 0; i < tableAudit->columnCount(); ++i) {
        QAction *act = menu.addAction(tableAudit->horizontalHeaderItem(i)->text());
        act->setCheckable(true);
        act->setChecked(!hv->isSectionHidden(i));
        connect(act, &QAction::toggled, this, [hv, i](bool checked){
            hv->setSectionHidden(i, !checked);
        });
    }
    menu.exec(hv->mapToGlobal(pos));
}

void HistoriqueAuditTab::rechercheRapide(const QString &text)
{
    QString t = text.trimmed().toLower();
    for (int r = 0; r < tableAudit->rowCount(); ++r) {
        bool match = t.isEmpty();
        if (!match) {
            for (int c = 0; c < tableAudit->columnCount(); ++c) {
                QTableWidgetItem *it = tableAudit->item(r, c);
                if (it && it->text().toLower().contains(t)) { match = true; break; }
            }
        }
        tableAudit->setRowHidden(r, !match);
    }
    int visible = 0;
    for (int r = 0; r < tableAudit->rowCount(); ++r)
        if (!tableAudit->isRowHidden(r)) visible++;
    if (lblNbResultats)
        lblNbResultats->setText(QString::number(visible) + " resultat(s)");
}

// ═══════════════════════════════════════════════════════════════════════════════
//  NOTES & STATUTS REVISION
// ═══════════════════════════════════════════════════════════════════════════════
void HistoriqueAuditTab::chargerNotes()
{
    if (!dbConn || !dbConn->getDatabase().isOpen()) return;
    QSqlQuery q(dbConn->getDatabase());
    if (!q.exec("SELECT AUDIT_ID, NOTE FROM AUDIT_NOTES")) return;
    notesMap.clear();
    while (q.next()) notesMap[q.value(0).toInt()] = q.value(1).toString();
}

void HistoriqueAuditTab::chargerStatutsRevision()
{
    if (!dbConn || !dbConn->getDatabase().isOpen()) return;
    QSqlQuery q(dbConn->getDatabase());
    if (!q.exec("SELECT AUDIT_ID, STATUS FROM AUDIT_REVIEW")) return;
    reviewMap.clear();
    while (q.next()) reviewMap[q.value(0).toInt()] = q.value(1).toString();
}

void HistoriqueAuditTab::setDarkMode(bool isDark) {
    m_isDarkMode = isDark;
    if (m_isDarkMode) {
        setStyleSheet(R"(
            QWidget { background-color: #0d1117; color: #e6edf3; font-family: 'Segoe UI', Arial, sans-serif; }
            QFrame#mainCard { background: #161b22; border: 1px solid #30363d; border-radius: 10px; }
            
            QFrame#cardTotal    { background:#21262d; border:1px solid #30363d; border-radius:8px; }
            QFrame#cardInsert   { background:#21262d; border:1px solid #238636; border-radius:8px; }
            QFrame#cardUpdate   { background:#21262d; border:1px solid #d29922; border-radius:8px; }
            QFrame#cardDelete   { background:#21262d; border:1px solid #da3633; border-radius:8px; }
            QFrame#cardAnomaly  { background:#21262d; border:1px solid #d29922; border-radius:8px; }
            QFrame#cardCritical { background:#21262d; border:1px solid #da3633; border-radius:8px; }
            QFrame#cardRatio    { background:#21262d; border:1px solid #30363d; border-radius:8px; min-width:130px; }
            
            QFrame#iconFrame { background:#30363d; border:1px solid #484f58; border-radius:7px; text-align:center; }
            QFrame#iconFrameTotal { background: qlineargradient(x1:0,y1:0,x2:1,y2:0, stop:0 #1f6feb, stop:0.5 #388bfd, stop:1 #238636); }
            QFrame#iconFrameCritical { background: qlineargradient(x1:0,y1:0,x2:1,y2:0, stop:0 #da3633, stop:0.5 #f59e0b, stop:1 #d29922); }
            
            QLabel { color:#e6edf3; }
            QLabel#titleTotal { background:transparent; font-size:12px; font-weight:600; color:#8b949e; }
            QLabel#valTotal { background:transparent; font-size:24px; font-weight:900; color:#e6edf3; }
            
            QTabWidget::pane { border:1px solid #30363d; border-radius:8px; background:#0d1117;}
            QTabBar::tab { background:#161b22; border:1px solid #30363d; padding:8px 16px; color:#8b949e; }
            QTabBar::tab:selected { background:#1f6feb; color:#ffffff; font-weight:bold; }
            
            QLineEdit, QComboBox, QDateEdit { background:#0d1117; border:1px solid #30363d; border-radius:4px; color:#e6edf3; padding:4px 8px; }
            
            QTableWidget { background:#0d1117; alternate-background-color:#161b22; border:1px solid #30363d; color:#e6edf3; gridline-color:#30363d; }
            QHeaderView::section { background:#21262d; color:#e6edf3; font-weight:bold; border-right:1px solid #30363d; border-bottom:1px solid #30363d; padding:6px; }
            QTableWidget::item:selected { background:#1f6feb; color:#ffffff; }
            
            QListWidget { background:#0d1117; border:1px solid #30363d; color:#e6edf3; }
            QListWidget::item:selected { background:#1f6feb; color:#ffffff; }
            
            QFrame#detailPanel { background:#161b22; border:1px solid #30363d; border-radius:10px; }
            QLabel#detailKey { color:#8b949e; font-weight:bold; }
            QLabel#detailVal { color:#e6edf3; }
            QTextEdit { background:#0d1117; color:#e6edf3; border:1px solid #30363d; }
            
            QWidget#actionBar { background:#161b22; border:1px solid #30363d; }
            
            QPushButton { background:#21262d; color:#c9d1d9; border:1px solid #30363d; border-radius:6px; padding:6px 12px; font-weight:bold; }
            QPushButton:hover { border-color:#8b949e; }
            
            QPushButton#btnPrimary { background:#238636; color:#ffffff; border:none; }
            QPushButton#btnAction { background:#1f6feb; color:#ffffff; border:none; }
            QPushButton#btnDanger { background:#da3633; color:#ffffff; border:none; }
            QPushButton#btnVerifie { background:#238636; color:#ffffff; border:none; }
            
            QScrollBar:vertical { background:#0d1117; width:8px; border-radius:4px; }
            QScrollBar::handle:vertical { background:#30363d; border-radius:4px; min-height:30px; }
        )");
        
        chartViewPie->setBackgroundBrush(QBrush(QColor("#0d1117")));
        chartViewBar->setBackgroundBrush(QBrush(QColor("#0d1117")));
        chartViewLine->setBackgroundBrush(QBrush(QColor("#0d1117")));
        chartViewImpact->setBackgroundBrush(QBrush(QColor("#0d1117")));
        
    } else {
        setStyleSheet(m_originalStyle);
        chartViewPie->setBackgroundBrush(QBrush(Qt::white));
        chartViewBar->setBackgroundBrush(QBrush(Qt::white));
        chartViewLine->setBackgroundBrush(QBrush(Qt::white));
        chartViewImpact->setBackgroundBrush(QBrush(Qt::white));
    }
}

QString HistoriqueAuditTab::calculerVariation(const QString &oldVal, const QString &newVal) const
{
    bool ok1, ok2;
    double oldNum = oldVal.toDouble(&ok1);
    double newNum = newVal.toDouble(&ok2);
    if (ok1 && ok2 && oldNum != 0) {
        double pct = ((newNum - oldNum) / qAbs(oldNum)) * 100.0;
        QString sign = pct > 0 ? "+" : "";
        return QString("%1%2 %").arg(sign).arg(pct, 0, 'f', 1);
    }
    return "--";
}

QString HistoriqueAuditTab::normaliseDateISO(const QString &changedAt) const
{
    if (changedAt.isEmpty()) return "";
    // Format "dd/MM/yy HH:mm:ss" → "yyyy-MM-dd"
    if (changedAt.length() >= 8 && changedAt[2] == '/') {
        QString day  = changedAt.mid(0, 2);
        QString mon  = changedAt.mid(3, 2);
        QString yr   = changedAt.mid(6, 2);
        return "20" + yr + "-" + mon + "-" + day;
    }
    // Format "yyyy-MM-dd HH:mm:ss" ou ISO complet
    if (changedAt.length() >= 10 && changedAt[4] == '-')
        return changedAt.left(10);
    // QDateTime auto-parse en dernier recours
    QDateTime dt = QDateTime::fromString(changedAt, Qt::ISODate);
    if (dt.isValid()) return dt.date().toString("yyyy-MM-dd");
    return changedAt.left(10);
}

QString HistoriqueAuditTab::extraireHeure(const QString &changedAt) const
{
    if (changedAt.length() >= 17 && changedAt[2] == '/')
        return changedAt.mid(9, 8);   // "dd/MM/yy HH:mm:ss" → offset 9
    if (changedAt.length() >= 19 && changedAt[4] == '-')
        return changedAt.mid(11, 8);  // "yyyy-MM-dd HH:mm:ss" → offset 11
    return "--:--:--";
}
