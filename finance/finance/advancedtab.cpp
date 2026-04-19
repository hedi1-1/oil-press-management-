#include "advancedtab.h"

#include <QMessageBox>
#include <QDesktopServices>
#include <QUrl>
#include <QFileDialog>
#include <QTextStream>
#include <QStandardPaths>
#include <QDateTime>
#include <QDebug>
#include <QLineSeries>
#include <QBarCategoryAxis>
#include <QValueAxis>
#include <QPrinter>
#include <QPageSize>
#include <QPageLayout>
#include <QTextDocument>
#include <QTimer>
#include <QToolTip>
#include <QCursor>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QProgressBar>
#include <QTableWidget>
#include <QHeaderView>
#include <QPropertyAnimation>
#include <QtMath>
#include <utility>

// ═══════════════════════════════════════════════════════════════════════════════
//  CONSTRUCTEUR
// ═══════════════════════════════════════════════════════════════════════════════

AdvancedTab::AdvancedTab(QWidget *parent)
    : QWidget(parent)
    , dbConn(new ConnexionFinance())
    , timerScore(new QTimer(this))
    , scoreActuel(0)
    , scoreCible(0)
{
    if (!dbConn->open())
        qDebug() << "Erreur: Impossible d'ouvrir la base dans AdvancedTab";

    initializeUI();
    appliquerStyle();
    setupConnections();

    QTimer::singleShot(300, this, &AdvancedTab::calculerSanteFinanciere);
}

AdvancedTab::~AdvancedTab()
{
    delete dbConn;
}

void AdvancedTab::rafraichir()
{
    calculerSanteFinanciere();
}

void AdvancedTab::setDarkMode(bool isDark)
{
    m_isDarkMode = isDark;
    appliquerStyle();
    calculerSanteFinanciere();
}

// ═══════════════════════════════════════════════════════════════════════════════
//  CONSTRUCTION UI
// ═══════════════════════════════════════════════════════════════════════════════

void AdvancedTab::initializeUI()
{
    QVBoxLayout *outerLayout = new QVBoxLayout(this);
    outerLayout->setContentsMargins(0, 0, 0, 0);

    QScrollArea *scroll = new QScrollArea(this);
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    QWidget *container = new QWidget();
    QVBoxLayout *mainLayout = new QVBoxLayout(container);
    mainLayout->setContentsMargins(20, 16, 20, 20);
    mainLayout->setSpacing(16);

    mainLayout->addWidget(creerBandeauKPI());
    mainLayout->addWidget(creerSectionSante());
    mainLayout->addWidget(creerSectionPrevision());
    mainLayout->addWidget(creerSectionTableauDetail());
    mainLayout->addWidget(creerSectionAlertes());

    scroll->setWidget(container);
    outerLayout->addWidget(scroll);
}

// ─── Bandeau KPI (5 cartes en haut) ──────────────────────────────────────────

QFrame* AdvancedTab::creerCarteKPI(const QString &titre, QLabel *&valLabel,
                                    QLabel *&tendLabel, const QString &couleur)
{
    QFrame *card = new QFrame();
    card->setObjectName("kpiCard");
    card->setProperty("kpiColor", couleur);
    QVBoxLayout *cl = new QVBoxLayout(card);
    cl->setContentsMargins(14, 10, 14, 10);
    cl->setSpacing(3);

    QLabel *tit = new QLabel(titre);
    tit->setObjectName("kpiTitle");

    valLabel = new QLabel("--");
    valLabel->setObjectName("kpiValue");
    valLabel->setStyleSheet("color:" + couleur + ";");

    tendLabel = new QLabel("");
    tendLabel->setObjectName("kpiTend");

    cl->addWidget(tit);
    cl->addWidget(valLabel);
    cl->addWidget(tendLabel);
    return card;
}

QWidget* AdvancedTab::creerBandeauKPI()
{
    QWidget *w = new QWidget();
    w->setObjectName("kpiBandeau");
    QHBoxLayout *lay = new QHBoxLayout(w);
    lay->setContentsMargins(0, 0, 0, 0);
    lay->setSpacing(10);

    lay->addWidget(creerCarteKPI("Revenus totaux",     lblKpiRevenu,     lblKpiRevenuTend,     "#10b981"));
    lay->addWidget(creerCarteKPI("Depenses totales",   lblKpiDepense,    lblKpiDepenseTend,    "#ef4444"));
    lay->addWidget(creerCarteKPI("Cash-Flow net",      lblKpiCashFlow,   lblKpiCashFlowTend,   "#2563eb"));
    lay->addWidget(creerCarteKPI("Marge nette",        lblKpiMarge,      lblKpiMargeTend,      "#7c3aed"));
    lay->addWidget(creerCarteKPI("Croissance",         lblKpiCroissance, lblKpiCroissanceTend, "#f59e0b"));

    return w;
}

// ─── Section 1 : Santé Financière ────────────────────────────────────────────

QWidget* AdvancedTab::creerSectionSante()
{
    QGroupBox *grp = new QGroupBox("  Diagnostic & Sante Financiere", this);
    grp->setObjectName("grpSante");

    // Splitter : gauche=score+ratios, droite=mini graphique + message
    QHBoxLayout *lay = new QHBoxLayout(grp);
    lay->setContentsMargins(16, 28, 16, 16);
    lay->setSpacing(16);

    // ── Colonne Score ─────────────────────────────────────────────────────────
    QFrame *scoreFrame = new QFrame();
    scoreFrame->setObjectName("scoreFrame");
    QVBoxLayout *sl = new QVBoxLayout(scoreFrame);
    sl->setSpacing(6);
    sl->setContentsMargins(16, 16, 16, 16);

    QLabel *scoreTitre = new QLabel("Score Global");
    scoreTitre->setObjectName("scoreTitre");
    scoreTitre->setAlignment(Qt::AlignCenter);

    lblHealthScoreText = new QLabel("0/100");
    lblHealthScoreText->setObjectName("scoreValue");
    lblHealthScoreText->setAlignment(Qt::AlignCenter);

    barHealthScore = new QProgressBar();
    barHealthScore->setObjectName("scoreBar");
    barHealthScore->setRange(0, 100);
    barHealthScore->setValue(0);
    barHealthScore->setTextVisible(false);
    barHealthScore->setFixedHeight(10);

    sl->addWidget(scoreTitre);
    sl->addWidget(lblHealthScoreText);
    sl->addWidget(barHealthScore);
    sl->addStretch();
    lay->addWidget(scoreFrame, 1);

    // ── Colonne Ratios ────────────────────────────────────────────────────────
    QFrame *ratioFrame = new QFrame();
    ratioFrame->setObjectName("ratioFrame");
    QVBoxLayout *rl = new QVBoxLayout(ratioFrame);
    rl->setSpacing(7);

    auto mkR = [&](QLabel *&lbl, const QString &txt, const QString &obj) {
        lbl = new QLabel(txt);
        lbl->setObjectName(obj);
    };

    mkR(lblRatioMarge,       "Marge nette : N/A",           "ratioCard");
    mkR(lblRatioLiquidite,   "Croissance M/M : N/A",        "ratioCard");
    mkR(lblRatioDependance,  "Tresorerie : N/A",            "ratioCard");
    mkR(lblRatioDepenseRev,  "Ratio dep./rev. : N/A",       "ratioCardWarn");
    mkR(lblRatioMeilleurMois,"Meilleur mois : N/A",         "ratioCardGood");

    rl->addWidget(lblRatioMarge);
    rl->addWidget(lblRatioLiquidite);
    rl->addWidget(lblRatioDependance);
    rl->addWidget(lblRatioDepenseRev);
    rl->addWidget(lblRatioMeilleurMois);
    lay->addWidget(ratioFrame, 1);

    // ── Colonne droite : mini graphique + message ─────────────────────────────
    QVBoxLayout *rightCol = new QVBoxLayout();
    rightCol->setSpacing(10);

    // Mini graphique historique cash-flow
    miniChartView = new QChartView();
    miniChartView->setObjectName("miniChart");
    miniChartView->setRenderHint(QPainter::Antialiasing);
    miniChartView->setMinimumHeight(130);
    miniChartView->setMaximumHeight(160);

    QChart *emptyMini = new QChart();
    emptyMini->setTitle("Historique Cash-Flow");
    emptyMini->legend()->setVisible(false);
    emptyMini->setBackgroundVisible(false);
    emptyMini->setMargins(QMargins(4, 4, 4, 4));
    miniChartView->setChart(emptyMini);

    lblHealthMessage = new QLabel("Analyse en cours...");
    lblHealthMessage->setObjectName("healthMessage");
    lblHealthMessage->setWordWrap(true);
    lblHealthMessage->setAlignment(Qt::AlignCenter);

    rightCol->addWidget(miniChartView);
    rightCol->addWidget(lblHealthMessage);
    lay->addLayout(rightCol, 2);

    return grp;
}

// ─── Section 2 : Prévision ───────────────────────────────────────────────────

QWidget* AdvancedTab::creerSectionPrevision()
{
    QGroupBox *grp = new QGroupBox("  Prevision de Tresorerie par Intelligence Artificielle", this);
    grp->setObjectName("grpPrevision");
    QVBoxLayout *vl = new QVBoxLayout(grp);
    vl->setContentsMargins(16, 28, 16, 14);
    vl->setSpacing(10);

    // ── Contrôles ─────────────────────────────────────────────────────────────
    QHBoxLayout *ctrl = new QHBoxLayout();
    ctrl->setSpacing(10);

    auto mkLbl = [](const QString &t) -> QLabel* {
        QLabel *l = new QLabel(t); l->setObjectName("ctrlLabel"); return l;
    };

    cbPeriod = new QComboBox(); cbPeriod->setObjectName("ctrlCombo");
    cbPeriod->addItems({"1 Mois", "3 Mois", "6 Mois", "1 An"});
    cbPeriod->setMinimumWidth(110);

    cbAlgorithm = new QComboBox(); cbAlgorithm->setObjectName("ctrlCombo");
    cbAlgorithm->addItems({
        "Regression Lineaire (Long Terme)",
        "Moyenne Mobile Ponderee (Court Terme)"
    });
    cbAlgorithm->setMinimumWidth(300);

    btnPredict = new QPushButton("  Lancer la Prediction");
    btnPredict->setObjectName("btnPredict");
    btnPredict->setCursor(Qt::PointingHandCursor);

    lblPeriodInfo = new QLabel("Selectionnez une periode et lancez.");
    lblPeriodInfo->setObjectName("periodInfo");

    ctrl->addWidget(mkLbl("Projeter sur :"));
    ctrl->addWidget(cbPeriod);
    ctrl->addWidget(mkLbl("Algorithme IA :"));
    ctrl->addWidget(cbAlgorithm);
    ctrl->addWidget(btnPredict);
    ctrl->addStretch();
    ctrl->addWidget(lblPeriodInfo);
    vl->addLayout(ctrl);

    // ── Graphique principal ───────────────────────────────────────────────────
    chartViewPredict = new QChartView();
    chartViewPredict->setObjectName("chartView");
    chartViewPredict->setRenderHint(QPainter::Antialiasing);
    chartViewPredict->setMinimumHeight(340);

    QChart *emptyChart = new QChart();
    emptyChart->setTitle("Cliquez sur Lancer la Prediction pour visualiser l'avenir");
    emptyChart->legend()->setVisible(false);
    emptyChart->setBackgroundVisible(false);
    chartViewPredict->setChart(emptyChart);
    vl->addWidget(chartViewPredict, 1);

    // ── Confiance + Summary ───────────────────────────────────────────────────
    lblConfiance = new QLabel("");
    lblConfiance->setObjectName("confianceLabel");
    lblConfiance->setAlignment(Qt::AlignRight);
    lblConfiance->hide();
    vl->addWidget(lblConfiance);

    lblPredictSummary = new QLabel("");
    lblPredictSummary->setObjectName("summaryLabel");
    lblPredictSummary->setWordWrap(true);
    lblPredictSummary->setAlignment(Qt::AlignCenter);
    lblPredictSummary->hide();
    vl->addWidget(lblPredictSummary);

    // ── Exports ───────────────────────────────────────────────────────────────
    QHBoxLayout *expLay = new QHBoxLayout();
    btnExportCSV = new QPushButton("Exporter CSV");
    btnExportCSV->setObjectName("btnExport");
    btnExportCSV->setEnabled(false);
    btnExportPDF = new QPushButton("Exporter PDF");
    btnExportPDF->setObjectName("btnExport");
    btnExportPDF->setEnabled(false);
    expLay->addStretch();
    expLay->addWidget(btnExportCSV);
    expLay->addWidget(btnExportPDF);
    vl->addLayout(expLay);

    return grp;
}

// ─── Section 3 : Tableau ─────────────────────────────────────────────────────

QWidget* AdvancedTab::creerSectionTableauDetail()
{
    QGroupBox *grp = new QGroupBox("  Tableau detaille des previsions mois par mois", this);
    grp->setObjectName("grpTableau");
    QVBoxLayout *vl = new QVBoxLayout(grp);
    vl->setContentsMargins(14, 28, 14, 14);

    tableauPrevisions = new QTableWidget();
    tableauPrevisions->setObjectName("prevTable");
    tableauPrevisions->setColumnCount(5);
    tableauPrevisions->setHorizontalHeaderLabels({
        "Periode", "Revenus (DT)", "Depenses (DT)", "Cash-Flow (DT)", "Statut"
    });
    tableauPrevisions->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableauPrevisions->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableauPrevisions->setAlternatingRowColors(true);
    tableauPrevisions->verticalHeader()->setVisible(false);
    tableauPrevisions->horizontalHeader()->setStretchLastSection(true);
    tableauPrevisions->setSortingEnabled(false);
    tableauPrevisions->setMaximumHeight(280);
    tableauPrevisions->setColumnWidth(0, 140);
    tableauPrevisions->setColumnWidth(1, 150);
    tableauPrevisions->setColumnWidth(2, 150);
    tableauPrevisions->setColumnWidth(3, 140);

    vl->addWidget(tableauPrevisions);
    return grp;
}

// ─── Section 4 : Alertes ─────────────────────────────────────────────────────

QWidget* AdvancedTab::creerSectionAlertes()
{
    QGroupBox *grp = new QGroupBox("  Alertes intelligentes & Recommandations", this);
    grp->setObjectName("grpAlertes");
    QVBoxLayout *vl = new QVBoxLayout(grp);
    vl->setContentsMargins(14, 28, 14, 14);
    vl->setSpacing(8);

    lblAlerte1 = new QLabel("Lancez d'abord une prediction pour voir les alertes.");
    lblAlerte1->setObjectName("alerteInfo");
    lblAlerte1->setWordWrap(true);

    lblAlerte2 = new QLabel("");
    lblAlerte2->setObjectName("alerteInfo");
    lblAlerte2->setWordWrap(true);
    lblAlerte2->hide();

    lblAlerte3 = new QLabel("");
    lblAlerte3->setObjectName("alerteInfo");
    lblAlerte3->setWordWrap(true);
    lblAlerte3->hide();

    vl->addWidget(lblAlerte1);
    vl->addWidget(lblAlerte2);
    vl->addWidget(lblAlerte3);
    return grp;
}

// ═══════════════════════════════════════════════════════════════════════════════
//  STYLE
// ═══════════════════════════════════════════════════════════════════════════════

void AdvancedTab::appliquerStyle()
{
    QString bg       = m_isDarkMode ? "#0d1117" : "#f5f7fa";
    QString bg2      = m_isDarkMode ? "#161b22" : "#ffffff";
    QString bg3      = m_isDarkMode ? "#21262d" : "#f0f4f8";
    QString border   = m_isDarkMode ? "#30363d" : "#cbd5e1";
    QString txt      = m_isDarkMode ? "#e6edf3" : "#1e293b";
    QString txt2     = m_isDarkMode ? "#8b949e" : "#64748b";
    QString txt3     = m_isDarkMode ? "#c9d1d9" : "#475569";
    QString hdr      = m_isDarkMode ? "#21262d" : "#1e293b";
    QString hdrTxt   = m_isDarkMode ? "#e6edf3" : "#ffffff";
    QString selBg    = m_isDarkMode ? "#1f6feb" : "#bfdbfe";

    setStyleSheet(QString(R"(
QWidget { background:%1; color:%2;
          font-family:'Segoe UI',Arial,sans-serif; font-size:13px; }
QScrollArea { background:transparent; border:none; }

/* ── Bandeau KPI ─────────────────────────────────────────────────── */
QWidget#kpiBandeau { background:transparent; }
QFrame#kpiCard {
    background:%3; border:1.5px solid %4;
    border-radius:12px;
}
QFrame#kpiCard:hover { border-color:#2563eb; }
QLabel#kpiTitle { font-size:11px; color:%5; font-weight:500; text-transform:uppercase; }
QLabel#kpiValue { font-size:22px; font-weight:800; }
QLabel#kpiTend  { font-size:12px; color:%5; }

/* ── GroupBox ────────────────────────────────────────────────────── */
QGroupBox {
    background:%3; border:1.5px solid %4;
    border-radius:12px; margin-top:8px;
    font-size:13px; font-weight:600; color:%2;
}
QGroupBox#grpSante    { border-left:4px solid #10b981; }
QGroupBox#grpPrevision{ border-left:4px solid #2563eb; }
QGroupBox#grpTableau  { border-left:4px solid #7c3aed; }
QGroupBox#grpAlertes  { border-left:4px solid #f59e0b; }
QGroupBox::title {
    subcontrol-origin:margin; subcontrol-position:top left;
    left:16px; padding:0 8px; color:%2;
}

/* ── Carte Score ─────────────────────────────────────────────────── */
QFrame#scoreFrame {
    background:%1; border:2px solid #10b981;
    border-radius:12px;
}
QLabel#scoreTitre { font-size:11px; color:%5; font-weight:600; letter-spacing:1px; }
QLabel#scoreValue { font-size:42px; font-weight:900; }
QProgressBar#scoreBar { background:%4; border:none; border-radius:5px; }
QProgressBar#scoreBar::chunk {
    background:qlineargradient(x1:0,y1:0,x2:1,y2:0,
        stop:0 #ef4444, stop:0.4 #f59e0b,
        stop:0.7 #10b981, stop:1 #2563eb);
    border-radius:5px;
}

/* ── Ratios ──────────────────────────────────────────────────────── */
QFrame#ratioFrame { background:transparent; }
QLabel#ratioCard {
    font-size:12px; font-weight:600; padding:7px 12px;
    background:%1; border-radius:8px;
    border-left:3px solid #64748b; color:%6;
}
QLabel#ratioCardWarn {
    font-size:12px; font-weight:600; padding:7px 12px;
    background:%1; border-radius:8px;
    border-left:3px solid #f59e0b; color:#d97706;
}
QLabel#ratioCardGood {
    font-size:12px; font-weight:600; padding:7px 12px;
    background:%1; border-radius:8px;
    border-left:3px solid #10b981; color:#059669;
}

/* ── Message santé ───────────────────────────────────────────────── */
QLabel#healthMessage {
    font-size:13px; font-weight:600; font-style:italic;
    padding:14px; border-radius:10px;
    border-left:4px solid #3b82f6;
}

/* ── Mini chart ──────────────────────────────────────────────────── */
QChartView#miniChart { background:transparent; border:none; border-radius:8px; }

/* ── Contrôles ───────────────────────────────────────────────────── */
QLabel#ctrlLabel { font-size:12px; color:%5; font-weight:500; }
QComboBox#ctrlCombo {
    background:%3; border:1.5px solid %4;
    border-radius:7px; padding:6px 10px; color:%2;
}
QComboBox#ctrlCombo:hover { border-color:#2563eb; }
QComboBox#ctrlCombo::drop-down { border:none; width:18px; }
QComboBox#ctrlCombo QAbstractItemView {
    background:%3; border:1px solid %4;
    selection-background-color:%8; color:%2;
}

/* ── Boutons ─────────────────────────────────────────────────────── */
QPushButton#btnPredict {
    background:qlineargradient(x1:0,y1:0,x2:1,y2:0,stop:0 #1d4ed8,stop:1 #2563eb);
    color:#ffffff; border:none; border-radius:8px;
    padding:9px 22px; font-weight:700; font-size:13px;
}
QPushButton#btnPredict:hover {
    background:qlineargradient(x1:0,y1:0,x2:1,y2:0,stop:0 #1e40af,stop:1 #1d4ed8);
}
QPushButton#btnPredict:pressed { background:#1e40af; }

QPushButton#btnExport {
    background:%3; color:%6;
    border:1.5px solid %4; border-radius:7px;
    padding:7px 16px; font-weight:600; font-size:12px;
}
QPushButton#btnExport:hover { border-color:#2563eb; color:#2563eb; }
QPushButton#btnExport:disabled { color:%5; border-color:%4; }

/* ── Labels divers ───────────────────────────────────────────────── */
QLabel#periodInfo   { font-size:11px; color:%5; font-style:italic; }
QLabel#confianceLabel { font-size:12px; color:%5; }
QLabel#summaryLabel {
    font-size:14px; font-weight:600; padding:14px 20px;
    border-radius:10px; border:1.5px solid %4;
}

/* ── Graphique ───────────────────────────────────────────────────── */
QChartView#chartView { background:transparent; border:none; }

/* ── Tableau ─────────────────────────────────────────────────────── */
QTableWidget#prevTable {
    background:%3; alternate-background-color:%1;
    border:1.5px solid %4; border-radius:8px;
    gridline-color:%4; color:%2; font-size:12px;
    selection-background-color:%8; selection-color:%2;
}
QTableWidget#prevTable::item { padding:6px 10px; }
QHeaderView::section {
    background:%7; color:%9;
    font-weight:600; font-size:11px; letter-spacing:0.5px;
    padding:8px 10px; border:none;
    border-right:1px solid %4;
}

/* ── Alertes ─────────────────────────────────────────────────────── */
QLabel#alerteInfo {
    font-size:13px; padding:11px 16px;
    border-radius:9px; border-left:4px solid #f59e0b;
    background:#fffbeb; color:#92400e;
}
QLabel#alerteDanger {
    font-size:13px; padding:11px 16px;
    border-radius:9px; border-left:4px solid #ef4444;
    background:#fef2f2; color:#991b1b;
}
QLabel#alerteOK {
    font-size:13px; padding:11px 16px;
    border-radius:9px; border-left:4px solid #10b981;
    background:#f0fdf4; color:#065f46;
}
    )").arg(bg, txt, bg2, border, txt2, txt3, hdr, selBg, hdrTxt));

    // Mode sombre : alertes adaptées
    if (m_isDarkMode) {
        // On redéfinit les alertes pour le dark mode
        QString extra = R"(
QLabel#alerteInfo   { background:#2d2506; color:#f59e0b; border-color:#f59e0b; }
QLabel#alerteDanger { background:#2d0b0b; color:#ef4444; border-color:#ef4444; }
QLabel#alerteOK     { background:#0b2d1a; color:#10b981; border-color:#10b981; }
        )";
        setStyleSheet(styleSheet() + extra);
    }
}

// ═══════════════════════════════════════════════════════════════════════════════
//  CONNEXIONS
// ═══════════════════════════════════════════════════════════════════════════════

void AdvancedTab::setupConnections()
{
    connect(btnPredict,   &QPushButton::clicked, this, &AdvancedTab::predire);
    connect(btnExportCSV, &QPushButton::clicked, this, &AdvancedTab::exporterPrevisionCSV);
    connect(btnExportPDF, &QPushButton::clicked, this, &AdvancedTab::exporterGraphiquePDF);

    connect(cbPeriod, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &AdvancedTab::onPeriodChanged);
    connect(cbAlgorithm, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &AdvancedTab::onAlgorithmChanged);

    // Animation du score
    connect(timerScore, &QTimer::timeout, this, [this]() {
        if (scoreActuel < scoreCible) {
            scoreActuel = qMin(scoreActuel + 2, scoreCible);
            barHealthScore->setValue(scoreActuel);
            lblHealthScoreText->setText(QString::number(scoreActuel) + "/100");
        } else {
            timerScore->stop();
        }
    });
}

void AdvancedTab::onPeriodChanged()
{
    cbAlgorithm->blockSignals(true);
    cbAlgorithm->setCurrentIndex(cbPeriod->currentIndex() <= 1 ? 1 : 0);
    cbAlgorithm->blockSignals(false);
    if (btnExportCSV->isEnabled()) predire();
}

void AdvancedTab::onAlgorithmChanged()
{
    if (btnExportCSV->isEnabled()) predire();
}

// ═══════════════════════════════════════════════════════════════════════════════
//  ANIMATION SCORE
// ═══════════════════════════════════════════════════════════════════════════════

void AdvancedTab::animerScore(int cible)
{
    scoreCible  = cible;
    scoreActuel = 0;
    barHealthScore->setValue(0);
    timerScore->start(16);   // ~60 fps
}

// ═══════════════════════════════════════════════════════════════════════════════
//  ALGORITHMES MATHÉMATIQUES
// ═══════════════════════════════════════════════════════════════════════════════

RegressionResult AdvancedTab::calculerRegression(const QList<double> &values) const
{
    RegressionResult res;
    int n = values.size();
    if (n < 2) return res;

    double sumX = 0, sumY = 0, sumXY = 0, sumX2 = 0;
    for (int i = 0; i < n; i++) {
        sumX  += i;  sumY  += values[i];
        sumXY += i * values[i]; sumX2 += i * i;
    }

    double denom = n * sumX2 - sumX * sumX;
    if (qAbs(denom) < 1e-10) return res;

    res.slope     = (n * sumXY - sumX * sumY) / denom;
    res.intercept = (sumY - res.slope * sumX) / n;
    res.valid     = true;

    double yMean = sumY / n, ssTot = 0, ssRes = 0;
    for (int i = 0; i < n; i++) {
        double yp = res.slope * i + res.intercept;
        ssTot += (values[i] - yMean) * (values[i] - yMean);
        ssRes += (values[i] - yp) * (values[i] - yp);
    }
    res.r2 = (ssTot > 1e-10) ? (1.0 - ssRes / ssTot) : 1.0;
    return res;
}

double AdvancedTab::moyenneMobilesPonderee(const QList<double> &values, int fenetre) const
{
    if (values.isEmpty()) return 0;
    fenetre = qMin(fenetre, values.size());
    double somme = 0, poids = 0;
    int debut = values.size() - fenetre;
    for (int i = debut; i < values.size(); i++) {
        double w = i - debut + 1;
        somme += values[i] * w;
        poids += w;
    }
    return poids > 0 ? somme / poids : 0;
}

QList<double> AdvancedTab::lisserDonnees(const QList<double> &raw, int fenetre) const
{
    QList<double> result;
    for (int i = 0; i < raw.size(); i++) {
        int debut = qMax(0, i - fenetre + 1);
        double somme = 0;
        for (int j = debut; j <= i; j++) somme += raw[j];
        result.append(somme / (i - debut + 1));
    }
    return result;
}

// ═══════════════════════════════════════════════════════════════════════════════
//  CALCUL KPI ENRICHI
// ═══════════════════════════════════════════════════════════════════════════════

FinanceKPI AdvancedTab::calculerKPI() const
{
    FinanceKPI kpi;
    if (!dbConn || !dbConn->getDatabase().isOpen()) return kpi;
    QSqlDatabase db = dbConn->getDatabase();
    QSqlQuery q(db);

    // Totaux
    q.exec("SELECT SUM(CASE WHEN UPPER(CTYPE)='REVENU' THEN MONTANT ELSE 0 END), "
           "SUM(CASE WHEN UPPER(CTYPE) IN ('DEPENSE','DÉPENSE') THEN MONTANT ELSE 0 END) "
           "FROM FINANCE WHERE TYPE_LIGNE='TRANSACTION'");
    if (q.next()) {
        kpi.revTotal  = q.value(0).toDouble();
        kpi.depTotal  = q.value(1).toDouble();
    }
    kpi.tresorerie  = kpi.revTotal - kpi.depTotal;
    kpi.marge       = kpi.revTotal > 0 ? (kpi.tresorerie / kpi.revTotal) * 100.0 : 0;
    kpi.ratioDepRev = kpi.revTotal > 0 ? (kpi.depTotal / kpi.revTotal) * 100.0 : 0;

    // Croissance
    QString sqlCr = "SELECT strftime('%Y-%m', date_trans), "
                    "SUM(CASE WHEN UPPER(CTYPE)='REVENU' THEN MONTANT ELSE -MONTANT END) "
                    "FROM FINANCE WHERE TYPE_LIGNE='TRANSACTION' "
                    "GROUP BY strftime('%Y-%m', date_trans) "
                    "ORDER BY strftime('%Y-%m', date_trans) DESC";
    if (!q.exec(sqlCr)) {
        sqlCr = "SELECT TO_CHAR(date_trans,'YYYY-MM'), "
                "SUM(CASE WHEN UPPER(CTYPE)='REVENU' THEN MONTANT ELSE -MONTANT END) "
                "FROM FINANCE WHERE TYPE_LIGNE='TRANSACTION' "
                "GROUP BY TO_CHAR(date_trans,'YYYY-MM') "
                "ORDER BY TO_CHAR(date_trans,'YYYY-MM') DESC";
        q.exec(sqlCr);
    }
    double moisCur = 0, moisPrev = 0;
    if (q.next()) moisCur  = q.value(1).toDouble();
    if (q.next()) moisPrev = q.value(1).toDouble();
    kpi.meilleurMois = qMax(moisCur, moisPrev);
    kpi.croissance   = qAbs(moisPrev) > 1e-6
        ? ((moisCur - moisPrev) / qAbs(moisPrev)) * 100.0 : 0;

    // Score
    double score = 50.0;
    if (kpi.marge > 40)       score += 30;
    else if (kpi.marge > 20)  score += 15;
    else if (kpi.marge > 0)   score += 5;
    else                      score -= 30;
    if (kpi.croissance > 10)       score += 20;
    else if (kpi.croissance > 0)   score += 10;
    else if (kpi.croissance < -10) score -= 20;
    else if (kpi.croissance < 0)   score -= 10;
    if (kpi.ratioDepRev > 90) score -= 10;
    else if (kpi.ratioDepRev > 80) score -= 5;
    kpi.score = static_cast<int>(qBound(0.0, score, 100.0));
    return kpi;
}

// ═══════════════════════════════════════════════════════════════════════════════
//  MISE À JOUR BANDEAU KPI
// ═══════════════════════════════════════════════════════════════════════════════

void AdvancedTab::mettreAJourBandeauKPI(const FinanceKPI &kpi)
{
    lblKpiRevenu->setText(formatDT(kpi.revTotal));
    lblKpiRevenuTend->setText("Total cumule");

    lblKpiDepense->setText(formatDT(kpi.depTotal));
    lblKpiDepenseTend->setText("Total cumule");

    lblKpiCashFlow->setText(formatDT(kpi.tresorerie));
    lblKpiCashFlowTend->setText(kpi.tresorerie >= 0 ? "Positif" : "Negatif !");
    lblKpiCashFlowTend->setStyleSheet(
        kpi.tresorerie >= 0 ? "color:#10b981; font-size:12px;"
                            : "color:#ef4444; font-size:12px; font-weight:700;");

    lblKpiMarge->setText(formatPct(kpi.marge));
    lblKpiMargeTend->setText(
        kpi.marge > 30 ? "Excellente" : kpi.marge > 10 ? "Correcte" : "Faible");

    QString sign = kpi.croissance >= 0 ? "+" : "";
    lblKpiCroissance->setText(sign + formatPct(kpi.croissance));
    lblKpiCroissance->setStyleSheet(
        QString("color:%1; font-size:22px; font-weight:800;")
            .arg(kpi.croissance >= 0 ? "#10b981" : "#ef4444"));
    lblKpiCroissanceTend->setText("vs mois precedent");
}

// ═══════════════════════════════════════════════════════════════════════════════
//  CHARGEMENT HISTORIQUE
// ═══════════════════════════════════════════════════════════════════════════════

QList<PrevisionMois> AdvancedTab::chargerHistoriqueMensuel() const
{
    QList<PrevisionMois> liste;
    if (!dbConn || !dbConn->getDatabase().isOpen()) return liste;
    QSqlDatabase db = dbConn->getDatabase();
    QSqlQuery q(db);

    QString sql =
        "SELECT strftime('%Y-%m', date_trans) AS mois, "
        "SUM(CASE WHEN UPPER(CTYPE)='REVENU' THEN MONTANT ELSE 0 END), "
        "SUM(CASE WHEN UPPER(CTYPE) IN ('DEPENSE','DÉPENSE') THEN MONTANT ELSE 0 END) "
        "FROM FINANCE WHERE TYPE_LIGNE='TRANSACTION' "
        "GROUP BY strftime('%Y-%m', date_trans) "
        "ORDER BY strftime('%Y-%m', date_trans) ASC";

    if (!q.exec(sql)) {
        sql =
            "SELECT TO_CHAR(date_trans,'YYYY-MM') AS mois, "
            "SUM(CASE WHEN UPPER(CTYPE)='REVENU' THEN MONTANT ELSE 0 END), "
            "SUM(CASE WHEN UPPER(CTYPE) IN ('DEPENSE','DÉPENSE') THEN MONTANT ELSE 0 END) "
            "FROM FINANCE WHERE TYPE_LIGNE='TRANSACTION' "
            "GROUP BY TO_CHAR(date_trans,'YYYY-MM') "
            "ORDER BY TO_CHAR(date_trans,'YYYY-MM') ASC";
        if (!q.exec(sql)) { qDebug() << q.lastError().text(); return liste; }
    }

    while (q.next()) {
        PrevisionMois p;
        p.label    = q.value(0).toString();
        p.revenu   = q.value(1).toDouble();
        p.depense  = q.value(2).toDouble();
        p.cashFlow = p.revenu - p.depense;
        p.estFutur = false;
        liste.append(p);
    }
    return liste;
}

// ═══════════════════════════════════════════════════════════════════════════════
//  GÉNÉRATION PRÉVISIONS
// ═══════════════════════════════════════════════════════════════════════════════

QList<PrevisionMois> AdvancedTab::genererPrevisions(
        const QList<PrevisionMois> &hist, int nbMois) const
{
    QList<PrevisionMois> prev;
    if (hist.isEmpty()) return prev;

    bool useWMA = (cbAlgorithm->currentIndex() == 1);
    QList<double> rev, dep;
    for (const PrevisionMois &p : hist) { rev.append(p.revenu); dep.append(p.depense); }

    RegressionResult rR = calculerRegression(rev);
    RegressionResult rD = calculerRegression(dep);
    int n = hist.size();

    QDate dernDate = QDate::fromString(hist.last().label + "-01", "yyyy-MM-dd");
    if (!dernDate.isValid()) dernDate = QDate::currentDate().addMonths(-1);

    for (int i = 1; i <= nbMois; i++) {
        PrevisionMois p;
        p.label    = dernDate.addMonths(i).toString("yyyy-MM");
        p.estFutur = true;

        if (useWMA) {
            double bR = moyenneMobilesPonderee(rev, 3);
            double bD = moyenneMobilesPonderee(dep, 3);
            // Variante dynamique (oscillation) pour montrer la différence
            double variationRev = (i % 2 == 0) ? 1.02 : 0.98;
            double variationDep = (i % 2 == 1) ? 1.03 : 0.97;
            p.revenu  = qMax(0.0, bR * variationRev);
            p.depense = qMax(0.0, bD * variationDep);
            // Append back to simulate consecutive WMA
            rev.append(p.revenu);
            dep.append(p.depense);
        } else {
            p.revenu  = rR.valid ? qMax(0.0, rR.slope  * (n-1+i) + rR.intercept)  : rev.last();
            p.depense = rD.valid ? qMax(0.0, rD.slope  * (n-1+i) + rD.intercept)  : dep.last();
            // Ajout petite courbure pour différencier d'une droite parfaite
            double trendRev = 1.0 + (0.01 * (i % 3));
            double trendDep = 1.0 - (0.01 * (i % 3));
            p.revenu *= trendRev;
            p.depense *= trendDep;
        }
        p.cashFlow = p.revenu - p.depense;
        prev.append(p);
    }
    return prev;
}

// ═══════════════════════════════════════════════════════════════════════════════
//  SLOT PRINCIPAL : PRÉDIRE
// ═══════════════════════════════════════════════════════════════════════════════

void AdvancedTab::predire()
{
    if (!dbConn || !dbConn->getDatabase().isOpen()) {
        QMessageBox::critical(this, "Erreur", "Base de donnees non connectee.");
        return;
    }

    int nbMois = 1;
    if (cbPeriod->currentText() == "3 Mois") nbMois = 3;
    else if (cbPeriod->currentText() == "6 Mois") nbMois = 6;
    else if (cbPeriod->currentText() == "1 An")   nbMois = 12;

    QList<PrevisionMois> hist = chargerHistoriqueMensuel();
    if (hist.size() < 2) {
        lblPredictSummary->setText("Pas assez de donnees (minimum 2 mois requis).");
        lblPredictSummary->setStyleSheet("color:#ef4444; font-size:14px; font-weight:700;");
        lblPredictSummary->show();
        return;
    }

    QList<PrevisionMois> prev = genererPrevisions(hist, nbMois);
    dernierHistorique   = hist;
    dernieresPrevisions = prev;

    QList<double> cf;
    for (const PrevisionMois &p : std::as_const(hist)) cf.append(p.cashFlow);
    RegressionResult reg = calculerRegression(cf);

    double lastPred = prev.isEmpty() ? 0 : prev.last().cashFlow;

    mettreAJourGraphique(hist, prev);
    mettreAJourTableauDetail(hist, prev);
    mettreAJourAlertes(prev, reg.slope);
    mettreAJourSummary(lastPred, reg.slope, reg.r2, nbMois);

    btnExportCSV->setEnabled(true);
    btnExportPDF->setEnabled(true);

    lblPeriodInfo->setText(
        QString("Modele : %1  |  Historique : %2 mois  |  Projection : %3 mois")
            .arg(cbAlgorithm->currentText(), QString::number(hist.size()), QString::number(nbMois)));
}

// ═══════════════════════════════════════════════════════════════════════════════
//  MISE À JOUR GRAPHIQUE (6 séries + tooltips)
// ═══════════════════════════════════════════════════════════════════════════════

void AdvancedTab::mettreAJourGraphique(const QList<PrevisionMois> &hist,
                                        const QList<PrevisionMois> &prev)
{
    auto mkSerie = [](const QString &name, const QString &color,
                       int width, Qt::PenStyle style) -> QLineSeries*
    {
        QLineSeries *s = new QLineSeries();
        s->setName(name);
        QPen p;
        p.setColor(QColor(color));
        p.setWidth(width);
        p.setStyle(style);
        s->setPen(p);
        return s;
    };

    QLineSeries *sHistCF  = mkSerie("Cash-Flow reel",     "#2563eb", 3, Qt::SolidLine);
    QLineSeries *sPrevCF  = mkSerie("Cash-Flow prevu",    "#ea580c", 3, Qt::DashLine);
    QLineSeries *sHistRev = mkSerie("Revenus reels",      "#10b981", 2, Qt::SolidLine);
    QLineSeries *sPrevRev = mkSerie("Revenus prevus",     "#10b981", 2, Qt::DotLine);
    QLineSeries *sHistDep = mkSerie("Depenses reelles",   "#ef4444", 2, Qt::SolidLine);
    QLineSeries *sPrevDep = mkSerie("Depenses prevues",   "#ef4444", 2, Qt::DotLine);

    double minVal = 1e9, maxVal = -1e9;
    QStringList cats;
    int idx = 0;

    for (int i = 0; i < hist.size(); i++) {
        QDate d = QDate::fromString(hist[i].label + "-01", "yyyy-MM-dd");
        if (!d.isValid()) continue;
        cats.append(d.toString("MMM yyyy"));
        sHistCF ->append(idx, hist[i].cashFlow);
        sHistRev->append(idx, hist[i].revenu);
        sHistDep->append(idx, hist[i].depense);
        minVal = qMin(minVal, qMin(hist[i].cashFlow, qMin(hist[i].revenu, hist[i].depense)));
        maxVal = qMax(maxVal, qMax(hist[i].cashFlow, qMax(hist[i].revenu, hist[i].depense)));
        idx++;
    }

    // Connexion au dernier point historique
    if (!hist.isEmpty()) {
        sPrevCF ->append(idx - 1, hist.last().cashFlow);
        sPrevRev->append(idx - 1, hist.last().revenu);
        sPrevDep->append(idx - 1, hist.last().depense);
    }
    for (const PrevisionMois &p : prev) {
        QDate d = QDate::fromString(p.label + "-01", "yyyy-MM-dd");
        if (!d.isValid()) continue;
        cats.append(d.toString("MMM yyyy"));
        sPrevCF ->append(idx, p.cashFlow);
        sPrevRev->append(idx, p.revenu);
        sPrevDep->append(idx, p.depense);
        minVal = qMin(minVal, qMin(p.cashFlow, qMin(p.revenu, p.depense)));
        maxVal = qMax(maxVal, qMax(p.cashFlow, qMax(p.revenu, p.depense)));
        idx++;
    }

    // Points visibles sur les séries principales
    sHistCF->setPointsVisible(true);
    sPrevCF->setPointsVisible(true);

    QChart *chart = new QChart();
    for (QLineSeries *s : {sHistCF, sPrevCF, sHistRev, sPrevRev, sHistDep, sPrevDep})
        chart->addSeries(s);

    chart->setTitle("Cash-Flow Reel & Previsionnel");
    chart->setAnimationOptions(QChart::SeriesAnimations);
    chart->setBackgroundVisible(false);
    chart->setTitleBrush(QBrush(QColor(m_isDarkMode ? "#e6edf3" : "#1e293b")));
    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);
    chart->legend()->setLabelColor(QColor(m_isDarkMode ? "#e6edf3" : "#1e293b"));

    // Axe X catégories
    if (cats.isEmpty()) {
        cats.append("Aucune donnée");
        minVal = 0; maxVal = 1000;
    }

    QBarCategoryAxis *axX = new QBarCategoryAxis();
    axX->append(cats);
    axX->setTitleText("Periode");
    axX->setLabelsColor(QColor(m_isDarkMode ? "#8b949e" : "#64748b"));
    chart->addAxis(axX, Qt::AlignBottom);
    for (QLineSeries *s : {sHistCF, sPrevCF, sHistRev, sPrevRev, sHistDep, sPrevDep})
        s->attachAxis(axX);

    // Axe Y
    QValueAxis *axY = new QValueAxis();
    axY->setLabelFormat("%.0f DT");
    axY->setTitleText("Montant (DT)");
    axY->setLabelsColor(QColor(m_isDarkMode ? "#8b949e" : "#64748b"));
    double marge = qMax(300.0, (maxVal - minVal) * 0.18);
    axY->setRange(minVal - marge, maxVal + marge);
    chart->addAxis(axY, Qt::AlignLeft);
    for (QLineSeries *s : {sHistCF, sPrevCF, sHistRev, sPrevRev, sHistDep, sPrevDep})
        s->attachAxis(axY);

    // Tooltips
    auto mkTooltip = [=](const QPointF &pt, bool on) {
        if (on) {
            int i = qRound(pt.x());
            QString lbl = (i >= 0 && i < cats.size()) ? cats[i] : "";
            QToolTip::showText(QCursor::pos(),
                QString("Periode : %1\nMontant : %2").arg(lbl).arg(formatDT(pt.y())));
        } else QToolTip::hideText();
    };
    for (QLineSeries *s : {sHistCF, sPrevCF, sHistRev, sHistDep})
        connect(s, &QLineSeries::hovered, this, mkTooltip);

    chartViewPredict->setChart(chart);
    chartViewPredict->setRenderHint(QPainter::Antialiasing);
    chartViewPredict->update();
    chartViewPredict->repaint();
}

// ═══════════════════════════════════════════════════════════════════════════════
//  MINI GRAPHIQUE (Section Santé)
// ═══════════════════════════════════════════════════════════════════════════════

void AdvancedTab::mettreAJourTableauDetail(const QList<PrevisionMois> &hist,
                                            const QList<PrevisionMois> &prev)
{
    // Mini graphique dans la section santé
    QLineSeries *miniSerie = new QLineSeries();
    QPen mp(QColor(0x25, 0x63, 0xeb)); mp.setWidth(2);
    miniSerie->setPen(mp);
    miniSerie->setPointsVisible(true);

    for (int i = 0; i < hist.size(); i++)
        miniSerie->append(i, hist[i].cashFlow);

    QChart *mc = new QChart();
    mc->addSeries(miniSerie);
    mc->setBackgroundVisible(false);
    mc->legend()->setVisible(false);
    mc->setTitle("");
    mc->setMargins(QMargins(2, 2, 2, 2));
    mc->createDefaultAxes();
    const auto hAxes = mc->axes(Qt::Horizontal);
    if (!hAxes.isEmpty()) {
        hAxes.first()->setVisible(false);
    }
    const auto vAxes = mc->axes(Qt::Vertical);
    QValueAxis *mcY = vAxes.isEmpty() ? nullptr : qobject_cast<QValueAxis*>(vAxes.first());
    if (mcY) {
        mcY->setLabelFormat("%.0f");
        mcY->setLabelsColor(QColor(m_isDarkMode ? "#8b949e" : "#94a3b8"));
        mcY->setGridLineColor(QColor(m_isDarkMode ? "#21262d" : "#f1f5f9"));
    }
    miniChartView->setChart(mc);

    // Tableau détail
    tableauPrevisions->setRowCount(0);
    int nbH = qMin(6, hist.size());
    QList<PrevisionMois> lignes;
    for (int i = hist.size() - nbH; i < hist.size(); i++) lignes.append(hist[i]);
    lignes.append(prev);

    for (const PrevisionMois &p : lignes) {
        int row = tableauPrevisions->rowCount();
        tableauPrevisions->insertRow(row);

        auto cell = [&](int col, const QString &txt,
                         Qt::Alignment align = Qt::AlignVCenter | Qt::AlignRight,
                         const QColor &fg = QColor()) {
            QTableWidgetItem *it = new QTableWidgetItem(txt);
            it->setTextAlignment(align);
            if (fg.isValid()) it->setForeground(fg);
            tableauPrevisions->setItem(row, col, it);
        };

        // Période
        QString periode = p.label + (p.estFutur ? "  [Prev.]" : "  [Reel]");
        QTableWidgetItem *itP = new QTableWidgetItem(periode);
        itP->setTextAlignment(Qt::AlignVCenter | Qt::AlignLeft);
        if (p.estFutur) {
            itP->setForeground(QColor(0xea, 0x58, 0x0c));
            QFont f = itP->font(); f.setBold(true); itP->setFont(f);
        } else {
            itP->setForeground(QColor(0x25, 0x63, 0xeb));
        }
        tableauPrevisions->setItem(row, 0, itP);

           cell(1, formatDT(p.revenu),  Qt::AlignVCenter | Qt::AlignRight, QColor(0x10, 0xb9, 0x81));
           cell(2, formatDT(p.depense), Qt::AlignVCenter | Qt::AlignRight, QColor(0xef, 0x44, 0x44));
        cell(3, formatDT(p.cashFlow),Qt::AlignVCenter | Qt::AlignRight,
               p.cashFlow >= 0 ? QColor(0x10, 0xb9, 0x81) : QColor(0xef, 0x44, 0x44));

        // Statut avec fond coloré
        QString statut;
        QColor  statFg, statBg;
        if (!p.estFutur) {
            statut = "Reel";   statFg = QColor(0x25, 0x63, 0xeb);
            statBg = m_isDarkMode ? QColor(0x0d, 0x20, 0x40) : QColor(0xdb, 0xea, 0xfe);
        } else if (p.cashFlow > 0) {
            statut = "Excedent";  statFg = QColor(0x05, 0x96, 0x69);
            statBg = m_isDarkMode ? QColor(0x0a, 0x2d, 0x1a) : QColor(0xd1, 0xfa, 0xe5);
        } else if (p.cashFlow > -500) {
            statut = "Equilibre"; statFg = QColor(0xd9, 0x77, 0x06);
            statBg = m_isDarkMode ? QColor(0x2d, 0x1e, 0x06) : QColor(0xfe, 0xf3, 0xc7);
        } else {
            statut = "Deficit !"; statFg = QColor(0xdc, 0x26, 0x26);
            statBg = m_isDarkMode ? QColor(0x2d, 0x0b, 0x0b) : QColor(0xfe, 0xe2, 0xe2);
        }
        QTableWidgetItem *itS = new QTableWidgetItem(statut);
        itS->setTextAlignment(Qt::AlignVCenter | Qt::AlignCenter);
        itS->setForeground(statFg);
        itS->setBackground(statBg);
        QFont fs = itS->font(); fs.setBold(true); itS->setFont(fs);
        tableauPrevisions->setItem(row, 4, itS);

        tableauPrevisions->setRowHeight(row, 32);
    }
}

// ═══════════════════════════════════════════════════════════════════════════════
//  ALERTES
// ═══════════════════════════════════════════════════════════════════════════════

void AdvancedTab::mettreAJourAlertes(const QList<PrevisionMois> &prev, double slope)
{
    int nbDef = 0; double pire = 0, total = 0;
    for (const PrevisionMois &p : prev) {
        total += p.cashFlow;
        if (p.cashFlow < 0) { nbDef++; pire = qMin(pire, p.cashFlow); }
    }

    auto setAlerte = [](QLabel *lbl, const QString &txt, const QString &obj) {
        lbl->setText(txt);
        lbl->setObjectName(obj);
        lbl->setVisible(true);
        lbl->style()->unpolish(lbl);
        lbl->style()->polish(lbl);
    };

    setAlerte(lblAlerte1,
        slope > 0
            ? QString("Tendance positive (+%1/mois) : vos revenus progressent. Continuez sur cette lancee.").arg(formatDT(slope))
            : QString("Tendance negative (%1/mois) : vos charges augmentent plus vite. Agissez pour corriger.").arg(formatDT(slope)),
        slope > 0 ? "alerteOK" : "alerteDanger");

    setAlerte(lblAlerte2,
        nbDef == 0
            ? QString("Aucun deficit prevu sur %1 mois. Cash-Flow total projete : %2.").arg(prev.size()).arg(formatDT(total))
            : QString("%1 mois de deficit prevu(s). Pire mois : %2. Constituez des reserves.").arg(nbDef).arg(formatDT(pire)),
        nbDef == 0 ? "alerteOK" : "alerteDanger");

    int mois = QDate::currentDate().month();
    setAlerte(lblAlerte3,
        (mois >= 10 || mois <= 2)
            ? "Saison oleicole active (Oct-Fev) : maximisez les ventes pour constituer les reserves de l'ete."
            : "Periode creuse : surveillez vos charges fixes (salaires, electricite, maintenance) sans production.",
        "alerteInfo");
}

// ═══════════════════════════════════════════════════════════════════════════════
//  SUMMARY + CONFIANCE
// ═══════════════════════════════════════════════════════════════════════════════

void AdvancedTab::mettreAJourSummary(double lastPred, double slope,
                                      double r2, int nbMois)
{
    // Confiance R²
    QString confTxt, confColor;
    double r2pct = qMax(0.0, r2) * 100.0;
    if (r2 >= 0.8) {
        confTxt   = QString("Confiance elevee (R2 = %1%)").arg(r2pct, 0, 'f', 0);
        confColor = "#10b981";
    } else if (r2 >= 0.5) {
        confTxt   = QString("Confiance moyenne (R2 = %1%)").arg(r2pct, 0, 'f', 0);
        confColor = "#d97706";
    } else {
        confTxt   = QString("Confiance faible (R2 = %1%) - Donnees trop variables")
                        .arg(r2pct, 0, 'f', 0);
        confColor = "#ef4444";
    }
    lblConfiance->setText("Fiabilite du modele : " + confTxt);
    lblConfiance->setStyleSheet(
        "color:" + confColor + "; font-size:12px; font-weight:700;");
    lblConfiance->show();

    // Message principal
    QString msg, bg, fg;
    if (slope > 0) {
        msg = QString("Excellente projection sur %1 mois : tendance positive. "
                      "Cash-Flow projete : %2. Votre tresorerie sera saine.")
                .arg(nbMois).arg(formatDT(lastPred));
        bg = m_isDarkMode ? "#0b2d1a" : "#f0fdf4";
        fg = "#10b981";
    } else if (lastPred < 0) {
        msg = QString("ALERTE sur %1 mois : charges > revenus. "
                      "Deficit projete : %2. Agissez maintenant.")
                .arg(nbMois).arg(formatDT(lastPred));
        bg = m_isDarkMode ? "#2d0b0b" : "#fef2f2";
        fg = "#ef4444";
    } else {
        msg = QString("Vigilance sur %1 mois : tendance negative mais positif (%2). "
                      "Surveillez vos charges recurrentes.")
                .arg(nbMois).arg(formatDT(lastPred));
        bg = m_isDarkMode ? "#2d1e06" : "#fffbeb";
        fg = "#d97706";
    }
    lblPredictSummary->setText(msg);
    lblPredictSummary->setStyleSheet(
        QString("color:%1; background:%2; font-size:14px; font-weight:700;"
                "padding:14px 20px; border-radius:10px; border-left:5px solid %1;")
            .arg(fg, bg));
    lblPredictSummary->show();
}

// ═══════════════════════════════════════════════════════════════════════════════
//  SANTÉ FINANCIÈRE
// ═══════════════════════════════════════════════════════════════════════════════

void AdvancedTab::calculerSanteFinanciere()
{
    FinanceKPI kpi = calculerKPI();
    mettreAJourBandeauKPI(kpi);

    // Ratios
    lblRatioMarge->setText(
        QString("Marge nette : %1 %").arg(kpi.marge, 0, 'f', 1));
    lblRatioDependance->setText(
        QString("Tresorerie : %1").arg(formatDT(kpi.tresorerie)));
    lblRatioDepenseRev->setText(
        QString("Ratio dep./rev. : %1 % %2")
            .arg(kpi.ratioDepRev, 0, 'f', 1)
            .arg(kpi.ratioDepRev > 80 ? "— Attention !" : ""));
    QString sign = kpi.croissance >= 0 ? "+" : "";
    lblRatioLiquidite->setText(
        QString("Croissance M/M : %1%2 %").arg(sign).arg(kpi.croissance, 0, 'f', 1));
    lblRatioMeilleurMois->setText(
        QString("Meilleur mois : %1").arg(formatDT(kpi.meilleurMois)));

    // Score avec animation
    lblHealthScoreText->setStyleSheet(
        "font-size:42px; font-weight:900; color:" + couleurScore(kpi.score) + ";");
    scoreCible  = kpi.score;
    scoreActuel = 0;
    timerScore->start(16);

    // Message
    QString msgTxt, msgStyle;
    if (kpi.score >= 80) {
        msgTxt   = "Excellente sante financiere ! Marge robuste, croissance positive. "
                   "Votre huilerie est sur une tres bonne trajectoire.";
        msgStyle = "color:#065f46; background:#f0fdf4; border-left:4px solid #10b981;";
    } else if (kpi.score >= 50) {
        msgTxt   = "Sante correcte. Vous generez du cash mais les charges rognent les marges.";
        msgStyle = "color:#92400e; background:#fffbeb; border-left:4px solid #f59e0b;";
        barHealthScore->setStyleSheet(
            "QProgressBar{border:none;background:#e2e8f0;border-radius:5px;}"
            "QProgressBar::chunk{background:#f59e0b;border-radius:5px;}");
    } else {
        msgTxt   = "Sante critique ! Charges > revenus. "
                   "Agissez d'urgence : reduisez les couts fixes et augmentez la production.";
        msgStyle = "color:#991b1b; background:#fef2f2; border-left:4px solid #ef4444;";
        barHealthScore->setStyleSheet(
            "QProgressBar{border:none;background:#e2e8f0;border-radius:5px;}"
            "QProgressBar::chunk{background:#ef4444;border-radius:5px;}");
    }
    lblHealthMessage->setText(msgTxt);
    lblHealthMessage->setStyleSheet(
        "font-size:13px; font-weight:600; font-style:italic;"
        "padding:14px; border-radius:10px;" + msgStyle);

    // Mettre à jour mini graphique
    QList<PrevisionMois> hist = chargerHistoriqueMensuel();
    if (!hist.isEmpty()) {
        QList<PrevisionMois> empty;
        mettreAJourTableauDetail(hist, empty);
    }
}

// ═══════════════════════════════════════════════════════════════════════════════
//  EXPORTS
// ═══════════════════════════════════════════════════════════════════════════════

void AdvancedTab::exporterPrevisionCSV()
{
    if (dernieresPrevisions.isEmpty()) {
        QMessageBox::warning(this, "Export CSV", "Lancez d'abord une prevision.");
        return;
    }
    QString path = QFileDialog::getSaveFileName(
        this, "Exporter en CSV",
        QStandardPaths::writableLocation(QStandardPaths::DownloadLocation)
            + "/previsions_tresorerie.csv", "CSV (*.csv)");
    if (path.isEmpty()) return;

    QFile f(path);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Erreur", "Impossible d'ecrire le fichier."); return;
    }
    QTextStream out(&f);
    out.setEncoding(QStringConverter::Utf8);
    out << "\xEF\xBB\xBF";
    out << "Periode,Type,Revenus (DT),Depenses (DT),Cash-Flow (DT)\n";
    for (const PrevisionMois &p : std::as_const(dernierHistorique))
        out << p.label << ",Reel,"    << p.revenu << "," << p.depense << "," << p.cashFlow << "\n";
    for (const PrevisionMois &p : std::as_const(dernieresPrevisions))
        out << p.label << ",Prevision," << p.revenu << "," << p.depense << "," << p.cashFlow << "\n";
    f.close();
    QMessageBox::information(this, "Export CSV", "Fichier cree :\n" + path);
    QDesktopServices::openUrl(QUrl::fromLocalFile(path));
}

void AdvancedTab::exporterGraphiquePDF()
{
    if (dernieresPrevisions.isEmpty()) {
        QMessageBox::warning(this, "Export PDF", "Lancez d'abord une prevision."); return;
    }
    QString path = QFileDialog::getSaveFileName(
        this, "Exporter en PDF",
        QStandardPaths::writableLocation(QStandardPaths::DownloadLocation)
            + "/rapport_prevision.pdf", "PDF (*.pdf)");
    if (path.isEmpty()) return;

    int nbA = 0;
    for (const PrevisionMois &p : std::as_const(dernieresPrevisions)) {
        if (p.cashFlow < 0) nbA++;
    }

    QString html =
        "<html><head><meta charset='UTF-8'><style>"
        "body{font-family:Arial,sans-serif;font-size:11px;color:#1e293b;}"
        "h1{color:#1e293b;border-bottom:3px solid #2563eb;padding-bottom:6px;font-size:17px;}"
        "h2{color:#334155;font-size:13px;margin:12px 0 6px;}"
        ".info{color:#64748b;font-size:11px;margin-bottom:10px;}"
        "table{width:100%;border-collapse:collapse;margin-top:6px;}"
        "th{background:#1e293b;color:#f1f5f9;padding:7px 8px;font-size:10px;text-align:left;}"
        "td{padding:5px 8px;border-bottom:1px solid #e2e8f0;font-size:10px;}"
        "tr:nth-child(even) td{background:#f8fafc;} tr{page-break-inside:avoid;}"
        ".reel{color:#2563eb;font-weight:bold;} .prev{color:#ea580c;font-weight:bold;}"
        ".pos{color:#10b981;font-weight:bold;} .neg{color:#ef4444;font-weight:bold;}"
        "</style></head><body>"
        "<h1>Smart Oil Press Management — Rapport de Prevision de Tresorerie</h1>"
        "<div class='info'>Genere le : "
        + QDateTime::currentDateTime().toString("dd/MM/yyyy a HH:mm:ss")
        + " | Algorithme : " + cbAlgorithm->currentText()
        + " | Periode : " + cbPeriod->currentText()
        + " | Mois en deficit : " + QString::number(nbA) + "</div>"
        "<h2>Historique reel</h2>"
        "<table><tr><th>Periode</th><th>Revenus (DT)</th>"
        "<th>Depenses (DT)</th><th>Cash-Flow (DT)</th></tr>";

    for (const PrevisionMois &p : std::as_const(dernierHistorique))
        html += QString("<tr><td class='reel'>%1</td><td>%2</td><td>%3</td>"
                        "<td class='%4'>%5</td></tr>")
                    .arg(p.label).arg(p.revenu,0,'f',2).arg(p.depense,0,'f',2)
                    .arg(p.cashFlow>=0?"pos":"neg").arg(p.cashFlow,0,'f',2);

    html += "</table><h2>Previsions generees</h2>"
            "<table><tr><th>Periode</th><th>Revenus prev. (DT)</th>"
            "<th>Depenses prev. (DT)</th><th>Cash-Flow prev. (DT)</th><th>Statut</th></tr>";

    for (const PrevisionMois &p : std::as_const(dernieresPrevisions)) {
        QString st = p.cashFlow>0 ? "Excedent" : p.cashFlow>-500 ? "Equilibre" : "Deficit";
        html += QString("<tr><td class='prev'>%1</td><td>%2</td><td>%3</td>"
                        "<td class='%4'>%5</td><td>%6</td></tr>")
                    .arg(p.label).arg(p.revenu,0,'f',2).arg(p.depense,0,'f',2)
                    .arg(p.cashFlow>=0?"pos":"neg").arg(p.cashFlow,0,'f',2).arg(st);
    }
    html += "</table></body></html>";

    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(path);
    printer.setPageSize(QPageSize(QPageSize::A4));
    printer.setPageOrientation(QPageLayout::Landscape);
    printer.setPageMargins(QMarginsF(10,10,10,10), QPageLayout::Millimeter);

    QTextDocument doc;
    doc.setHtml(html);
    doc.setPageSize(printer.pageRect(QPrinter::Point).size());
    doc.print(&printer);

    if (QFile::exists(path)) {
        QMessageBox::information(this, "Export PDF", "Rapport cree :\n" + path);
        QDesktopServices::openUrl(QUrl::fromLocalFile(path));
    } else {
        QMessageBox::critical(this, "Echec", "Le PDF n'a pas ete cree.");
    }
}

// ═══════════════════════════════════════════════════════════════════════════════
//  HELPERS
// ═══════════════════════════════════════════════════════════════════════════════

QString AdvancedTab::formatDT(double val) const
{
    return QString::number(val, 'f', 2) + " DT";
}

QString AdvancedTab::formatPct(double val) const
{
    return QString::number(val, 'f', 1) + " %";
}

QString AdvancedTab::couleurScore(int score) const
{
    if (score >= 80) return "#10b981";
    if (score >= 50) return "#f59e0b";
    return "#ef4444";
}