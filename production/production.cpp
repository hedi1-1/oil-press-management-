#include "production.h"
#include "ui_production.h"
#include "productioneditdialog.h"
#include "qualityaiagent.h"
#include <QCoreApplication>
#include <QDateTime>
#include <QPixmap>
#include <QFileDialog>
#include <QPrinter>
#include <QPainter>
#include <QPageLayout>
#include <QPageSize>
#include <QFont>
#include <QFontMetrics>
#include <QRect>
#include <QColor>
#include <QLinearGradient>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QStandardPaths>
#include <QRegularExpression>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QLabel>
#include <QTextEdit>
#include <QSignalBlocker>
#include <QtCharts/QValueAxis>
#include <QtCharts/QChart>
#include <QSqlRecord>

Production::Production(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Production)
    , currentProductionId(0)
    , m_rendementProductionId(0)
    , m_qualityProductionId(0)
    , m_isDarkMode(false)
    , m_langIndex(0)
    , m_simTimer(new QTimer(this))
    , m_statsTimer(new QTimer(this))
    , m_simTicks(0)
    , m_totalDurationMin(60)
    , m_olivesKgTotal(500)
    , m_simProductionId(0)
    , m_isRunning(false)
    , m_isPaused(false)
    , m_oilProducedL(0.0)
    , m_comboQualityProduction(nullptr)
    , m_btnRefreshQualityProduction(nullptr)
    , m_spinAcidityPercent(nullptr)
    , m_lblAiQualityScore(nullptr)
    , m_lblAiRecommendations(nullptr)
    , m_comboReportPeriod(nullptr)
    , m_dateReportFrom(nullptr)
    , m_dateReportTo(nullptr)
    , m_comboReportQuality(nullptr)
    , m_btnApplyReportFilters(nullptr)
    , m_btnFullscreen(nullptr)
    , m_tabStatistiques(nullptr)
    , m_lblStatProdToday(nullptr)
    , m_lblStatRendAvg(nullptr)
    , m_lblStatAcidAvg(nullptr)
    , m_lblStatConformity(nullptr)
    , m_lineChartView(nullptr)
    , m_pieChartView(nullptr)
    , m_seriesOlives(nullptr)
    , m_seriesHuile(nullptr)
    , m_seriesRendement(nullptr)
    , m_axisStatsX(nullptr)
    , m_axisStatsYVolume(nullptr)
    , m_axisStatsYRendement(nullptr)
    , m_qualityPie(nullptr)
    , m_statsTick(0)
{
    ui->setupUi(this);
    {
        QPixmap logo;
        for (const QString &p : QStringList{":/logo.png", "logo.png", "../production/logo.png",
             QCoreApplication::applicationDirPath() + "/../../logo.png",
             QCoreApplication::applicationDirPath() + "/../../../production/logo.png"}) {
            logo = QPixmap(p);
            if (!logo.isNull()) break;
        }
        if (!logo.isNull())
            ui->lblLogo->setPixmap(logo.scaledToHeight(50, Qt::SmoothTransformation));
    }

    // Establish database connection via singleton
    if (!Connection::getInstance().createconnect()) {
        showErrorNotification("Impossible de se connecter à la base de données Oracle");
    }

    // Connect back button
    connect(ui->btnBackToMenu, &QPushButton::clicked, this, &Production::onBackButtonClicked);

    // Connect CRUD buttons
    connect(ui->btnPlanifier,       &QPushButton::clicked, this, &Production::onPlanifierClicked);
    connect(ui->btnStart,           &QPushButton::clicked, this, &Production::onStartClicked);
    connect(ui->btnStop,            &QPushButton::clicked, this, &Production::onStopClicked);
    connect(ui->btnFinishProduction,&QPushButton::clicked, this, &Production::onFinishProductionClicked);
    connect(ui->btnCalculateYield,  &QPushButton::clicked, this, &Production::onCalculateYieldClicked);
    connect(ui->btnValidateQuality, &QPushButton::clicked, this, &Production::onValidateQualityClicked);
    connect(ui->btnGenerateReport,  &QPushButton::clicked, this, &Production::onGenerateReportClicked);

    // Connect history buttons
    connect(ui->btnDeleteProduction, &QPushButton::clicked, this, &Production::onDeleteClicked);
    connect(ui->btnModifyProduction, &QPushButton::clicked, this, &Production::onModifyClicked);
    connect(ui->btnUpdateMonitoring, &QPushButton::clicked, this, &Production::onRefreshHistoryClicked);

    // Connect simulation controls
    connect(ui->btnRefreshPlanned, &QPushButton::clicked, this, &Production::onRefreshPlannedClicked);
    connect(m_simTimer, &QTimer::timeout, this, &Production::onSimTick);
    connect(m_statsTimer, &QTimer::timeout, this, &Production::refreshStatistics);

    // Connect rendement tab controls
    connect(ui->btnRefreshTerminated, &QPushButton::clicked, this, &Production::onRefreshTerminatedClicked);
    connect(ui->comboTerminatedProductions, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &Production::onTerminatedProductionSelected);

    // Connect theme & language buttons
    connect(ui->btnDarkMode,  &QPushButton::clicked, this, &Production::onToggleDarkMode);
    connect(ui->btnLanguage,  &QPushButton::clicked, this, &Production::onToggleLanguage);

    // Header fullscreen toggle button (inserted next to language/theme controls).
    if (ui->btnLanguage && ui->btnLanguage->parentWidget()) {
        auto *headerLayout = qobject_cast<QHBoxLayout*>(ui->btnLanguage->parentWidget()->layout());
        if (headerLayout) {
            m_btnFullscreen = new QPushButton("⛶ Plein écran", ui->btnLanguage->parentWidget());
            m_btnFullscreen->setObjectName("btnFullscreen");
            m_btnFullscreen->setMinimumSize(30, 34);
            m_btnFullscreen->setCursor(Qt::PointingHandCursor);
            m_btnFullscreen->setStyleSheet(
                "QPushButton { background: rgba(255,255,255,0.15); color: white; font-size: 12px; font-weight: 600; "
                "padding: 6px 14px; border-radius: 8px; border: 1px solid rgba(255,255,255,0.3); min-width: 0; } "
                "QPushButton:hover { background: rgba(255,255,255,0.28); border-color: rgba(255,255,255,0.55); } "
                "QPushButton:pressed { background: rgba(255,255,255,0.40); }");

            const int languageIndex = headerLayout->indexOf(ui->btnLanguage);
            headerLayout->insertWidget(languageIndex, m_btnFullscreen);
            connect(m_btnFullscreen, &QPushButton::clicked, this, &Production::onToggleFullscreen);
        }
    }

    // Capture light stylesheet for toggling
    m_lightStyleSheet = this->styleSheet();

    // Build advanced modules requested by user
    setupAdvancedQualityUI();
    setupAdvancedReportsUI();
    buildStatisticsTab();

    // 1 production hour = 30 real seconds → timer fires every 500ms = 1 prod minute
    m_simTimer->setInterval(500);
    // Higher cadence for a smoother live dashboard.
    m_statsTimer->setInterval(700);
    m_statsTimer->start();

    // Style the new Terminer button
    ui->btnFinishProduction->setStyleSheet(
        "QPushButton { background: #059669; color: white; font-size: 16px; font-weight: 700; "
        "border-radius: 10px; border: none; } "
        "QPushButton:hover { background: #047857; } "
        "QPushButton:disabled { background: #d1d5db; color: #9ca3af; }"
    );

    // Load planned productions into dropdown
    loadPlannedProductions();
    // Load terminated productions into rendement dropdown
    loadTerminatedProductions();
    loadQualityEvaluableProductions();

    // Load production history on startup
    loadProductionHistory();
    updateReportPreview();
    refreshStatistics();
}

Production::~Production()
{
    delete ui;
}

void Production::onBackButtonClicked()
{
    emit backToMenu();
    this->hide();
}

// ============================================================================
// NOTIFICATION HELPERS
// ============================================================================

void Production::showSuccessNotification(const QString &message)
{
    QMessageBox msgBox(this);
    msgBox.setWindowTitle("✓ Succès");
    msgBox.setText(message);
    msgBox.setIcon(QMessageBox::Information);
    msgBox.setStyleSheet(
        "QMessageBox { background-color: #f0fdf4; }"
        "QMessageBox QLabel { color: #166534; font-size: 14px; font-weight: 600; }"
        "QPushButton { background-color: #1B4332; color: white; padding: 8px 20px; "
        "border-radius: 6px; font-weight: 600; min-width: 80px; }"
        "QPushButton:hover { background-color: #234E3E; }"
    );
    msgBox.exec();
}

void Production::showErrorNotification(const QString &message)
{
    QMessageBox msgBox(this);
    msgBox.setWindowTitle("✗ Erreur");
    msgBox.setText(message);
    msgBox.setIcon(QMessageBox::Critical);
    msgBox.setStyleSheet(
        "QMessageBox { background-color: #fef2f2; }"
        "QMessageBox QLabel { color: #991b1b; font-size: 14px; font-weight: 600; }"
        "QPushButton { background-color: #dc2626; color: white; padding: 8px 20px; "
        "border-radius: 6px; font-weight: 600; min-width: 80px; }"
        "QPushButton:hover { background-color: #b91c1c; }"
    );
    msgBox.exec();
}

// ============================================================================
// CRUD: ADD PRODUCTION (Planifier)
// ============================================================================

void Production::onPlanifierClicked()
{
    ProductionModel prod;
    
    // Get values from UI
    prod.setQuantiteOlivesKg(ui->spinOlivesKg->value());
    prod.setTypePressage(ui->comboPressType->currentText());
    prod.setNotesPlanification(ui->txtNotesPlanification->toPlainText());
    prod.setDateProduction(QDate::currentDate());
    prod.setStatut("Planifie");
    prod.setHeureDemarrage(QTime::currentTime());
    
    // Set FK values to 0 (will become NULL in database - no FK constraints)
    prod.setIdClient(0);
    prod.setIdUser(0);
    prod.setIdMachine(0);
    prod.setIdStock(0);
    
    // Estimate duration based on quantity
    int estimatedMinutes = prod.getQuantiteOlivesKg() / 5; // ~5kg per minute
    prod.setDureeEstimee(estimatedMinutes);
    prod.setTempsEcoule(0);
    prod.setHuileProduiteL(0);
    prod.setRendement(0);
    
    qDebug() << "Attempting to add production...";
    qDebug() << "Quantity:" << prod.getQuantiteOlivesKg();
    qDebug() << "Type:" << prod.getTypePressage();
    
    if (prod.addProduction()) {
        showSuccessNotification("Production planifiée avec succès!\n\n"
                               "Quantité: " + QString::number(prod.getQuantiteOlivesKg()) + " kg\n"
                               "Type: " + prod.getTypePressage() + "\n"
                               "Durée estimée: " + QString::number(estimatedMinutes) + " min");
        
        // Update UI status
        ui->lblStatusPlanification->setText("✓  Production planifiée");
        ui->lblStatusPlanification->setStyleSheet("background-color: #DCFCE7; color: #166534; border: 2px solid #4ADE80; border-radius: 8px; font-size: 15px; padding: 10px;");
        ui->lblDuration->setText("⏱ Durée estimée : " + QString::number(estimatedMinutes) + " min");
        
        currentProductionId = prod.getIdProduction();
        qDebug() << "Production created with ID:" << currentProductionId;

        // Refresh history + planned list
        loadProductionHistory();
        loadPlannedProductions();
    } else {
        showErrorNotification("Échec de la planification!\n\nConsultez la console Application Output pour les détails.");
    }
}

// ============================================================================
// SIMULATION: LOAD PLANNED PRODUCTIONS INTO COMBO
// ============================================================================

void Production::loadPlannedProductions()
{
    ui->comboSelectProduction->clear();
    ui->comboSelectProduction->addItem("— Choisir une production planifiée —", -1);

    QSqlQuery q(QSqlDatabase::database("production_conn"));
    q.exec("SELECT IDPRODUCTION, QUANTITEOLIVESKG, TYPEPRESSAGE, DATEPRODUCTION, DUREEESTIMEE "
           "FROM PRODUCTION WHERE STATUT = 'Planifie' ORDER BY IDPRODUCTION DESC");
    int count = 0;
    while (q.next()) {
        int    id      = q.value(0).toInt();
        int    kg      = q.value(1).toInt();
        QString type   = q.value(2).toString();
        QString date   = q.value(3).toDate().toString("dd/MM/yyyy");
        int    duree   = q.value(4).toInt();
        QString label  = QString("ID #%1 — %2 kg — %3 — %4  (%5 min)")
                           .arg(id).arg(kg).arg(type).arg(date).arg(duree);
        ui->comboSelectProduction->addItem(label, id);
        count++;
    }

    if (count == 0) {
        ui->lblSelectedProductionInfo->setText(
            "Aucune production avec statut 'Planifie' trouvée. Planifiez d'abord.");
    } else {
        ui->lblSelectedProductionInfo->setText(
            QString("%1 production(s) planifiée(s) disponible(s)").arg(count));
    }
}

void Production::onRefreshPlannedClicked()
{
    loadPlannedProductions();
}

// ============================================================================
// RENDEMENT: LOAD TERMINATED PRODUCTIONS INTO COMBO
// ============================================================================

void Production::loadTerminatedProductions()
{
    ui->comboTerminatedProductions->clear();
    ui->comboTerminatedProductions->addItem("— Sélectionner une production terminée —", -1);

    QSqlQuery q(QSqlDatabase::database("production_conn"));
    q.exec("SELECT IDPRODUCTION, DATEPRODUCTION, QUANTITEOLIVESKG, HUILEPRODUITEL "
           "FROM PRODUCTION WHERE STATUT = 'Termine' ORDER BY IDPRODUCTION DESC");
    int count = 0;
    while (q.next()) {
        int     id    = q.value(0).toInt();
        QString date  = q.value(1).toDate().toString("dd/MM/yyyy");
        int     kg    = q.value(2).toInt();
        double  huile = q.value(3).toDouble();
        QString label = QString("ID #%1 — %2 — %3 kg — %4 L")
                            .arg(id).arg(date).arg(kg).arg(huile, 0, 'f', 2);
        ui->comboTerminatedProductions->addItem(label, id);
        count++;
    }

    ui->lblTerminatedInfo->setText(
        count > 0
        ? QString("%1 production(s) terminée(s) disponible(s) — sélectionnez pour charger").arg(count)
        : "Aucune production terminée trouvée."
    );
}

void Production::onRefreshTerminatedClicked()
{
    loadTerminatedProductions();
}

void Production::setupAdvancedQualityUI()
{
    auto *qualityRoot = qobject_cast<QVBoxLayout*>(ui->tabQualite->layout());

    // Build a strict no-scroll structure:
    // root VBox -> top section (evaluation + notes) + bottom section (summary + action button)
    if (qualityRoot && !ui->tabQualite->findChild<QWidget*>("qualityTopSection")) {
        auto *topSection = new QWidget(ui->tabQualite);
        topSection->setObjectName("qualityTopSection");
        auto *topLayout = new QHBoxLayout(topSection);
        topLayout->setContentsMargins(0, 0, 0, 0);
        topLayout->setSpacing(16);

        ui->groupQualityEval->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        ui->groupQualityNotes->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        topLayout->addWidget(ui->groupQualityEval, 1);
        topLayout->addWidget(ui->groupQualityNotes, 1);

        auto *bottomSection = new QWidget(ui->tabQualite);
        bottomSection->setObjectName("qualityBottomSection");
        auto *bottomLayout = new QVBoxLayout(bottomSection);
        bottomLayout->setContentsMargins(0, 0, 0, 0);
        bottomLayout->setSpacing(12);

        ui->groupQualitySummary->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        bottomLayout->addWidget(ui->groupQualitySummary, 1);

        if (ui->qualityFooterDivider) {
            bottomLayout->addWidget(ui->qualityFooterDivider);
        }

        auto *footerRow = new QWidget(bottomSection);
        auto *footerLayout = new QHBoxLayout(footerRow);
        footerLayout->setContentsMargins(0, 0, 0, 0);
        footerLayout->setSpacing(12);
        footerLayout->addStretch(1);
        footerLayout->addWidget(ui->btnValidateQuality, 0, Qt::AlignRight | Qt::AlignVCenter);

        // Keep CTA anchored without creating an oversized empty area.
        bottomLayout->addWidget(footerRow);

        while (qualityRoot->count() > 0) {
            QLayoutItem *oldItem = qualityRoot->takeAt(0);
            delete oldItem;
        }

        qualityRoot->setContentsMargins(16, 16, 16, 16);
        qualityRoot->setSpacing(16);
        qualityRoot->addWidget(topSection, 4);
        qualityRoot->addWidget(bottomSection, 2);
        qualityRoot->setStretch(0, 4);
        qualityRoot->setStretch(1, 2);
    }

    auto *form = ui->groupQualityEval->findChild<QFormLayout*>("formQuality");
    if (form) {
        form->setFormAlignment(Qt::AlignTop);
        form->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
        form->setRowWrapPolicy(QFormLayout::DontWrapRows);
        form->setLabelAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        form->setHorizontalSpacing(14);
        form->setVerticalSpacing(14);

        // Row 0: production selection for automatic AI evaluation
        auto *lblProd = new QLabel("🏭 Production :", ui->groupQualityEval);
        lblProd->setStyleSheet("font-weight: 600;");
        lblProd->setMinimumHeight(40);
        lblProd->setMinimumWidth(140);
        auto *selectorWrap = new QWidget(ui->groupQualityEval);
        auto *selectorLayout = new QHBoxLayout(selectorWrap);
        selectorLayout->setContentsMargins(0, 0, 0, 0);
        selectorLayout->setSpacing(8);

        m_comboQualityProduction = new QComboBox(selectorWrap);
        m_comboQualityProduction->setMinimumHeight(40);
        m_comboQualityProduction->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLengthWithIcon);
        m_comboQualityProduction->setMinimumContentsLength(26);
        m_comboQualityProduction->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        m_comboQualityProduction->setStyleSheet(
            "QComboBox { background: #ffffff; color: #111827; border: 2px solid #1B4332; border-radius: 10px; padding: 8px 12px; font-size: 14px; font-weight: 600; }"
            "QComboBox:hover { border-color: #234E3E; background: #f8fffb; }"
            "QComboBox::drop-down { border: none; width: 30px; background: transparent; }"
            "QComboBox QAbstractItemView { background: #ffffff; color: #111827; border: 2px solid #1B4332; selection-background-color: #1B4332; selection-color: #ffffff; font-size: 13px; }");
        m_btnRefreshQualityProduction = new QPushButton("↻", selectorWrap);
        m_btnRefreshQualityProduction->setFixedSize(44, 44);
        m_btnRefreshQualityProduction->setToolTip("Actualiser la liste des productions");
        m_btnRefreshQualityProduction->setCursor(Qt::PointingHandCursor);

        selectorLayout->addWidget(m_comboQualityProduction, 1);
        selectorLayout->addWidget(m_btnRefreshQualityProduction, 0);
        form->insertRow(0, lblProd, selectorWrap);

        // Use the acidity field defined in the .ui form row to avoid layout detachment issues.
        ui->lblAcidity->setText("🧪 Acidité mesurée :");
        ui->lblAcidity->setMinimumWidth(140);
        m_spinAcidityPercent = ui->comboAcidity;
        m_spinAcidityPercent->setRange(0.10, 5.00);
        m_spinAcidityPercent->setDecimals(2);
        m_spinAcidityPercent->setSingleStep(0.05);
        m_spinAcidityPercent->setValue(0.80);
        m_spinAcidityPercent->setSuffix(" %");
        m_spinAcidityPercent->setMinimumHeight(40);
        m_spinAcidityPercent->setMinimumWidth(240);
        m_spinAcidityPercent->setButtonSymbols(QAbstractSpinBox::NoButtons);

        ui->comboOilQuality->setMinimumHeight(40);
        ui->lblOilQuality->setMinimumWidth(140);
        ui->checkConformity->setMinimumHeight(40);
    }

    ui->groupQualityEval->setMinimumHeight(0);
    ui->groupQualityNotes->setMinimumHeight(0);
    ui->groupQualityEval->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->groupQualityNotes->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    if (auto *evalLayout = qobject_cast<QVBoxLayout*>(ui->groupQualityEval->layout())) {
        evalLayout->setSpacing(14);
        evalLayout->setContentsMargins(16, 22, 16, 16);
    }

    if (auto *notesLayout = qobject_cast<QVBoxLayout*>(ui->groupQualityNotes->layout())) {
        notesLayout->setSpacing(14);
        notesLayout->setContentsMargins(16, 22, 16, 16);
    }

    if (auto *summaryLayout = qobject_cast<QVBoxLayout*>(ui->groupQualitySummary->layout())) {
        summaryLayout->setSpacing(12);
        summaryLayout->setContentsMargins(16, 16, 16, 16);
    }

    if (ui->lblQualitySummary) {
        ui->lblQualitySummary->setStyleSheet(
            "background-color: #edf7f1;"
            "border-left: 5px solid #1E5A45;"
            "padding: 14px 16px;"
            "border-radius: 0 10px 10px 0;"
            "color: #0f172a;"
            "font-size: 16px;"
            "font-weight: 700;");
        ui->lblQualitySummary->setWordWrap(true);
    }

    if (ui->txtQualityNotes) {
        ui->txtQualityNotes->setMinimumHeight(180);
        ui->txtQualityNotes->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    }

    if (ui->btnValidateQuality) {
        ui->btnValidateQuality->setMinimumWidth(300);
        ui->btnValidateQuality->setMinimumHeight(50);
    }

    auto *sumLayout = qobject_cast<QVBoxLayout*>(ui->groupQualitySummary->layout());
    if (sumLayout) {
        sumLayout->setAlignment(Qt::AlignTop);
        m_lblAiQualityScore = new QLabel("IA: score --/100 | confiance --%", ui->groupQualitySummary);
        m_lblAiQualityScore->setStyleSheet("background-color: #ffffff; color: #1e3a8a; border: 1px solid #bfdbfe; border-left: 5px solid #2563eb; padding: 12px; border-radius: 8px; font-size: 15px; font-weight: 700;");
        m_lblAiQualityScore->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        m_lblAiRecommendations = new QLabel("Remarques IA: en attente d'evaluation.", ui->groupQualitySummary);
        m_lblAiRecommendations->setWordWrap(true);
        m_lblAiRecommendations->setStyleSheet("background-color: #ffffff; color: #111827; border: 1px solid #dbeafe; border-left: 5px solid #0284c7; padding: 12px; border-radius: 8px; font-size: 15px; font-weight: 600;");
        m_lblAiRecommendations->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        sumLayout->addWidget(m_lblAiQualityScore);
        sumLayout->addWidget(m_lblAiRecommendations);
    }

    if (m_btnRefreshQualityProduction) {
        connect(m_btnRefreshQualityProduction, &QPushButton::clicked,
                this, &Production::onRefreshQualityProductions);
    }
    if (m_comboQualityProduction) {
        connect(m_comboQualityProduction, QOverload<int>::of(&QComboBox::currentIndexChanged),
                this, &Production::onQualityProductionSelected);
        connect(m_comboQualityProduction, QOverload<int>::of(&QComboBox::activated),
            this, &Production::onQualityProductionSelected);
    }
    if (m_spinAcidityPercent) {
        connect(m_spinAcidityPercent, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this,
            [this](double) { autoEvaluateQuality(false, false); });
    }

        connect(ui->comboOilQuality, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, [this](int) { autoEvaluateQuality(false, false); });
        connect(ui->checkConformity, &QCheckBox::toggled,
            this, [this](bool) { autoEvaluateQuality(false, false); });
}

void Production::loadQualityEvaluableProductions()
{
    if (!m_comboQualityProduction) return;

    m_comboQualityProduction->clear();
    m_comboQualityProduction->addItem("— Choisir une production terminée —", -1);

    QSqlQuery q(QSqlDatabase::database("production_conn"));
        q.exec("SELECT IDPRODUCTION, DATEPRODUCTION, QUANTITEOLIVESKG, RENDEMENT "
            "FROM PRODUCTION "
            "WHERE UPPER(STATUT) IN ('TERMINE', 'TERMINEE', 'TERMINÉ', 'TERMINÉE', 'QUALITE VALIDEE', 'QUALITÉ VALIDÉE') "
            "OR STATUT = 'Termine' OR STATUT = 'Terminee' OR STATUT = 'Terminé' OR STATUT = 'Terminée' "
            "OR STATUT = 'Qualité validée' OR STATUT = 'Qualite validée' "
            "ORDER BY IDPRODUCTION DESC");

    int count = 0;
    while (q.next()) {
        const int id = q.value(0).toInt();
        const QString date = q.value(1).toDate().toString("dd/MM/yyyy");
        const int kg = q.value(2).toInt();
        m_comboQualityProduction->addItem(
            QString("ID #%1 — %2 — %3 kg")
                .arg(id).arg(date).arg(kg),
            id);
        count++;
    }

    m_comboQualityProduction->setEnabled(count > 0);

    if (count == 0) {
        ui->lblQualitySummary->setText("ℹ  Aucune production terminée disponible pour évaluation.");
        if (m_lblAiQualityScore) {
            m_lblAiQualityScore->setText("IA: aucune production terminée trouvée.");
        }
        if (m_lblAiRecommendations) {
            m_lblAiRecommendations->setText("Remarques IA: vérifiez le statut des productions (Terminé / Qualité validée).");
        }
    } else {
        ui->lblQualitySummary->setText("ℹ  Sélectionnez une production pour lancer l'évaluation IA automatique.");
        m_comboQualityProduction->setCurrentIndex(0);
    }
}

void Production::onRefreshQualityProductions()
{
    loadQualityEvaluableProductions();
}

void Production::onQualityProductionSelected(int index)
{
    if (!m_comboQualityProduction || index <= 0) {
        m_qualityProductionId = 0;
        ui->lblQualitySummary->setText("ℹ  Sélectionnez une production pour lancer l'évaluation IA automatique.");
        ui->txtQualityNotes->clear();
        return;
    }

    const int prodId = m_comboQualityProduction->itemData(index).toInt();
    if (prodId <= 0) return;

    m_qualityProductionId = prodId;

    QSqlQuery q(QSqlDatabase::database("production_conn"));
    q.prepare("SELECT QUALITE, CONFORMENORMES, REMARQUESQUALITE, RENDEMENT FROM PRODUCTION WHERE IDPRODUCTION = :id");
    q.bindValue(":id", prodId);
    if (q.exec() && q.next()) {
        const QString qualite = q.value(0).toString();
        const bool conforme = q.value(1).toInt() == 1;
        const QString notes = q.value(2).toString();
        const double rend = q.value(3).toDouble();

        QSignalBlocker b1(ui->comboOilQuality);
        QSignalBlocker b2(ui->checkConformity);
        int idxQ = ui->comboOilQuality->findText(qualite, Qt::MatchContains);
        if (idxQ >= 0) ui->comboOilQuality->setCurrentIndex(idxQ);
        ui->checkConformity->setChecked(conforme);
        if (!notes.trimmed().isEmpty()) ui->txtQualityNotes->setPlainText(notes);
        ui->lblQualitySummary->setText(QString("ℹ  Production #%1 chargee. Rendement actuel: %2 %")
                                       .arg(prodId)
                                       .arg(rend, 0, 'f', 2));
    }

    QSqlQuery qa(QSqlDatabase::database("production_conn"));
    qa.prepare("SELECT ACIDITE FROM PRODUCTION WHERE IDPRODUCTION = :id");
    qa.bindValue(":id", prodId);
    if (qa.exec() && qa.next() && m_spinAcidityPercent) {
        const double acid = qa.value(0).toDouble();
        if (acid > 0.0) m_spinAcidityPercent->setValue(acid);
    }

    // Automatic AI preview immediately after selection
    autoEvaluateQuality(false, false);
}

bool Production::autoEvaluateQuality(bool persistToDb, bool showSuccessNotification)
{
    const int targetId = (m_qualityProductionId > 0) ? m_qualityProductionId : currentProductionId;
    if (targetId == 0) {
        if (m_lblAiQualityScore) {
            m_lblAiQualityScore->setText("IA: score --/100 | confiance --%");
        }
        if (m_lblAiRecommendations) {
            m_lblAiRecommendations->setText("Remarques IA: sélectionnez une production pour lancer l'analyse.");
        }
        return false;
    }

    QSqlQuery q(QSqlDatabase::database("production_conn"));
    q.prepare("SELECT QUANTITEOLIVESKG, NVL(RENDEMENT,0) FROM PRODUCTION WHERE IDPRODUCTION = :id");
    q.bindValue(":id", targetId);
    int olives = 0;
    double rendement = 0.0;
    if (q.exec() && q.next()) {
        olives = q.value(0).toInt();
        rendement = q.value(1).toDouble();
    }

    QualityEvaluationInput input;
    input.acidityPercent = m_spinAcidityPercent ? m_spinAcidityPercent->value() : 0.8;
    input.rendementPercent = rendement;
    input.olivesKg = olives;
    input.selectedQuality = ui->comboOilQuality->currentText();
    input.userConformity = ui->checkConformity->isChecked();
    input.operatorNotes = ui->txtQualityNotes->toPlainText();

    const QualityEvaluationResult result = QualityAIAgent::evaluate(input);

    if (m_lblAiQualityScore) {
        m_lblAiQualityScore->setText(
            QString("IA: score %1/100 | confiance %2 % | risque %3")
                .arg(QString::number(result.score, 'f', 1))
                .arg(QString::number(result.confidence, 'f', 0))
                .arg(result.riskLevel));
    }
    if (m_lblAiRecommendations) {
        m_lblAiRecommendations->setText(result.smartRemarks);
    }

    ui->lblQualitySummary->setText(
        QString("🤖 Évaluation automatique prête pour production #%1\n"
                "Qualité IA: %2 | Risque: %3")
            .arg(targetId)
            .arg(result.qualityLabel)
            .arg(result.riskLevel));

    if (!persistToDb) {
        return true;
    }

    QString remarks = ui->txtQualityNotes->toPlainText().trimmed();
    if (!remarks.isEmpty()) remarks += "\n\n";
    remarks += "[Analyse IA]\n" + result.smartRemarks;

    QSqlQuery upd(QSqlDatabase::database("production_conn"));
    upd.prepare("UPDATE PRODUCTION SET QUALITE=:q, CONFORMENORMES=:c, REMARQUESQUALITE=:r, "
                "STATUT='Qualité validée', ACIDITE=:a, SCOREQUALITEIA=:s, CONFIANCEIA=:f "
                "WHERE IDPRODUCTION=:id");
    upd.bindValue(":q", result.qualityLabel);
    upd.bindValue(":c", result.isConforme ? 1 : 0);
    upd.bindValue(":r", remarks.left(500));
    upd.bindValue(":a", input.acidityPercent);
    upd.bindValue(":s", result.score);
    upd.bindValue(":f", result.confidence);
    upd.bindValue(":id", targetId);

    bool ok = upd.exec();
    if (!ok) {
        QSqlQuery fallback(QSqlDatabase::database("production_conn"));
        fallback.prepare("UPDATE PRODUCTION SET QUALITE=:q, CONFORMENORMES=:c, REMARQUESQUALITE=:r, "
                         "STATUT='Qualité validée' WHERE IDPRODUCTION=:id");
        fallback.bindValue(":q", result.qualityLabel);
        fallback.bindValue(":c", result.isConforme ? 1 : 0);
        fallback.bindValue(":r", remarks.left(500));
        fallback.bindValue(":id", targetId);
        ok = fallback.exec();
    }

    if (!ok) {
        showErrorNotification("Échec de la validation qualité:\n" + upd.lastError().text());
        return false;
    }

    ui->txtQualityNotes->setPlainText(remarks);

    if (showSuccessNotification) {
        this->showSuccessNotification(
            QString("✓ Qualité validée avec IA\n\nProduction #%1\nQualité: %2\nAcidité: %3 %\n"
                    "Score IA: %4/100\nConfiance: %5 %")
                .arg(targetId)
                .arg(result.qualityLabel)
                .arg(QString::number(input.acidityPercent, 'f', 2))
                .arg(QString::number(result.score, 'f', 1))
                .arg(QString::number(result.confidence, 'f', 0)));
    }

    loadProductionHistory();
    loadQualityEvaluableProductions();
    refreshStatistics();
    return true;
}

void Production::setupAdvancedReportsUI()
{
    auto *reportsLayout = qobject_cast<QVBoxLayout*>(ui->tabRapports->layout());
    if (!reportsLayout) return;

    auto *groupFilters = new QGroupBox("  Filtres du rapport", ui->tabRapports);
    auto *filters = new QHBoxLayout(groupFilters);
    filters->setSpacing(10);

    m_comboReportPeriod = new QComboBox(groupFilters);
    m_comboReportPeriod->addItems({"Ce mois", "Mois dernier", "Cette année", "Intervalle de dates"});
    m_comboReportPeriod->setMinimumHeight(38);

    m_dateReportFrom = new QDateEdit(QDate::currentDate().addDays(-30), groupFilters);
    m_dateReportFrom->setCalendarPopup(true);
    m_dateReportFrom->setDisplayFormat("dd/MM/yyyy");
    m_dateReportFrom->setMinimumHeight(38);

    m_dateReportTo = new QDateEdit(QDate::currentDate(), groupFilters);
    m_dateReportTo->setCalendarPopup(true);
    m_dateReportTo->setDisplayFormat("dd/MM/yyyy");
    m_dateReportTo->setMinimumHeight(38);

    m_comboReportQuality = new QComboBox(groupFilters);
    m_comboReportQuality->addItems({"Toutes qualités", "Extra Vierge", "Vierge", "Lampante"});
    m_comboReportQuality->setMinimumHeight(38);

    m_btnApplyReportFilters = new QPushButton("Appliquer", groupFilters);
    m_btnApplyReportFilters->setMinimumHeight(38);

    filters->addWidget(new QLabel("Période:"), 0);
    filters->addWidget(m_comboReportPeriod, 1);
    filters->addWidget(new QLabel("Du:"), 0);
    filters->addWidget(m_dateReportFrom, 0);
    filters->addWidget(new QLabel("Au:"), 0);
    filters->addWidget(m_dateReportTo, 0);
    filters->addWidget(new QLabel("Qualité:"), 0);
    filters->addWidget(m_comboReportQuality, 1);
    filters->addWidget(m_btnApplyReportFilters, 0);

    reportsLayout->insertWidget(0, groupFilters);

    connect(m_comboReportPeriod, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &Production::onReportPeriodChanged);
    connect(m_btnApplyReportFilters, &QPushButton::clicked,
            this, &Production::onApplyReportFilters);

    onReportPeriodChanged(m_comboReportPeriod->currentIndex());
}

void Production::onReportPeriodChanged(int index)
{
    const bool custom = (index == 3);
    if (m_dateReportFrom) m_dateReportFrom->setEnabled(custom);
    if (m_dateReportTo) m_dateReportTo->setEnabled(custom);
}

QString Production::buildReportWhereClause(QString *prettyPeriod) const
{
    QStringList clauses;
    QString periodLabel;

    const QDate today = QDate::currentDate();
    if (m_comboReportPeriod) {
        switch (m_comboReportPeriod->currentIndex()) {
        case 0: {
            QDate start(today.year(), today.month(), 1);
            QDate end = start.addMonths(1).addDays(-1);
            clauses << QString("DATEPRODUCTION BETWEEN TO_DATE('%1','YYYY-MM-DD') AND TO_DATE('%2','YYYY-MM-DD')")
                           .arg(start.toString("yyyy-MM-dd"), end.toString("yyyy-MM-dd"));
            periodLabel = "Ce mois";
            break;
        }
        case 1: {
            QDate start(today.year(), today.month(), 1);
            start = start.addMonths(-1);
            QDate end = start.addMonths(1).addDays(-1);
            clauses << QString("DATEPRODUCTION BETWEEN TO_DATE('%1','YYYY-MM-DD') AND TO_DATE('%2','YYYY-MM-DD')")
                           .arg(start.toString("yyyy-MM-dd"), end.toString("yyyy-MM-dd"));
            periodLabel = "Mois dernier";
            break;
        }
        case 2: {
            QDate start(today.year(), 1, 1);
            QDate end(today.year(), 12, 31);
            clauses << QString("DATEPRODUCTION BETWEEN TO_DATE('%1','YYYY-MM-DD') AND TO_DATE('%2','YYYY-MM-DD')")
                           .arg(start.toString("yyyy-MM-dd"), end.toString("yyyy-MM-dd"));
            periodLabel = "Cette année";
            break;
        }
        case 3: {
            const QDate start = m_dateReportFrom ? m_dateReportFrom->date() : today.addDays(-30);
            const QDate end = m_dateReportTo ? m_dateReportTo->date() : today;
            clauses << QString("DATEPRODUCTION BETWEEN TO_DATE('%1','YYYY-MM-DD') AND TO_DATE('%2','YYYY-MM-DD')")
                           .arg(start.toString("yyyy-MM-dd"), end.toString("yyyy-MM-dd"));
            periodLabel = QString("%1 → %2").arg(start.toString("dd/MM/yyyy"), end.toString("dd/MM/yyyy"));
            break;
        }
        default:
            break;
        }
    }

    if (m_comboReportQuality) {
        const QString q = m_comboReportQuality->currentText();
        if (!q.startsWith("Toutes")) {
            clauses << QString("QUALITE = '%1'").arg(q);
        }
    }

    if (prettyPeriod) {
        *prettyPeriod = periodLabel;
    }

    if (clauses.isEmpty()) return QString();
    return " WHERE " + clauses.join(" AND ");
}

void Production::updateReportPreview()
{
    QString periodLabel;
    const QString whereClause = buildReportWhereClause(&periodLabel);

    QSqlQuery q(QSqlDatabase::database("production_conn"));
    q.exec("SELECT COUNT(*), NVL(SUM(QUANTITEOLIVESKG),0), NVL(SUM(HUILEPRODUITEL),0), NVL(AVG(RENDEMENT),0) "
           "FROM PRODUCTION" + whereClause);

    int count = 0;
    double olives = 0.0;
    double huile = 0.0;
    double rend = 0.0;
    if (q.next()) {
        count = q.value(0).toInt();
        olives = q.value(1).toDouble();
        huile = q.value(2).toDouble();
        rend = q.value(3).toDouble();
    }

    const QString qualityFilter = m_comboReportQuality ? m_comboReportQuality->currentText() : "Toutes qualités";
    const QString html = QString(
        "<h1>📋 Rapport de Production</h1>"
        "<p><b>Période:</b> %1</p>"
        "<p><b>Filtre qualité:</b> %2</p>"
        "<ul>"
        "<li>Productions: <b>%3</b></li>"
        "<li>Olives traitées: <b>%4 kg</b></li>"
        "<li>Huile produite: <b>%5 L</b></li>"
        "<li>Rendement moyen: <b>%6 %%</b></li>"
        "</ul>"
        "<p style='color:#64748b'>Rapport enrichi avec indicateurs qualité et suivi opérationnel.</p>")
            .arg(periodLabel.isEmpty() ? "Global" : periodLabel)
            .arg(qualityFilter)
            .arg(count)
            .arg(QString::number(olives, 'f', 0))
            .arg(QString::number(huile, 'f', 2))
            .arg(QString::number(rend, 'f', 2));

    ui->textReport->setHtml(html);
    ui->lblProductionSummary->setText(
        QString("📋 %1 productions | %2 kg olives | %3 L huile | Rendement moyen %4 %")
            .arg(count)
            .arg(QString::number(olives, 'f', 0))
            .arg(QString::number(huile, 'f', 2))
            .arg(QString::number(rend, 'f', 2)));
    ui->lblReportDate->setText("📅  Dernière génération : " + QDateTime::currentDateTime().toString("dd/MM/yyyy HH:mm"));
}

void Production::onApplyReportFilters()
{
    updateReportPreview();
}

void Production::buildStatisticsTab()
{
    m_tabStatistiques = new QWidget(this);
    auto *main = new QVBoxLayout(m_tabStatistiques);
    main->setContentsMargins(15, 15, 15, 15);
    main->setSpacing(12);

    auto *kpiRow = new QHBoxLayout();
    m_lblStatProdToday = new QLabel("Productions jour: --", m_tabStatistiques);
    m_lblStatRendAvg = new QLabel("Rendement moyen: -- %", m_tabStatistiques);
    m_lblStatAcidAvg = new QLabel("Acidité moyenne: -- %", m_tabStatistiques);
    m_lblStatConformity = new QLabel("Conformité: -- %", m_tabStatistiques);
    for (QLabel *lbl : {m_lblStatProdToday, m_lblStatRendAvg, m_lblStatAcidAvg, m_lblStatConformity}) {
        lbl->setStyleSheet("background:#f8fafc; border:1px solid #dbeafe; border-radius:10px; padding:10px; font-weight:700;");
        kpiRow->addWidget(lbl, 1);
    }
    main->addLayout(kpiRow);

    auto *chartsRow = new QHBoxLayout();

    m_seriesOlives = new QLineSeries(this);
    m_seriesOlives->setName("Olives (kg)");
    m_seriesHuile = new QLineSeries(this);
    m_seriesHuile->setName("Huile (L)");
    m_seriesRendement = new QLineSeries(this);
    m_seriesRendement->setName("Rendement (%)");

    m_seriesOlives->setPen(QPen(QColor("#2563eb"), 2));
    m_seriesHuile->setPen(QPen(QColor("#16a34a"), 2));
    m_seriesRendement->setPen(QPen(QColor("#f59e0b"), 3));
    m_seriesOlives->setPointsVisible(true);
    m_seriesHuile->setPointsVisible(true);
    m_seriesRendement->setPointsVisible(true);

    auto *lineChart = new QChart();
    lineChart->addSeries(m_seriesOlives);
    lineChart->addSeries(m_seriesHuile);
    lineChart->addSeries(m_seriesRendement);
    lineChart->setTitle("Statistiques temps réel");
    lineChart->legend()->setVisible(true);
    lineChart->setAnimationOptions(QChart::SeriesAnimations);

    m_axisStatsX = new QValueAxis();
    m_axisStatsX->setRange(0, 30);
    m_axisStatsX->setTitleText("Ticks");

    m_axisStatsYVolume = new QValueAxis();
    m_axisStatsYVolume->setRange(0, 1000);
    m_axisStatsYVolume->setTitleText("Volume (kg/L)");

    m_axisStatsYRendement = new QValueAxis();
    m_axisStatsYRendement->setRange(0, 30);
    m_axisStatsYRendement->setTitleText("Rendement (%)");

    lineChart->addAxis(m_axisStatsX, Qt::AlignBottom);
    lineChart->addAxis(m_axisStatsYVolume, Qt::AlignLeft);
    lineChart->addAxis(m_axisStatsYRendement, Qt::AlignRight);

    m_seriesOlives->attachAxis(m_axisStatsX);
    m_seriesOlives->attachAxis(m_axisStatsYVolume);
    m_seriesHuile->attachAxis(m_axisStatsX);
    m_seriesHuile->attachAxis(m_axisStatsYVolume);
    m_seriesRendement->attachAxis(m_axisStatsX);
    m_seriesRendement->attachAxis(m_axisStatsYRendement);

    m_lineChartView = new QChartView(lineChart, m_tabStatistiques);
    m_lineChartView->setRenderHint(QPainter::Antialiasing);
    m_lineChartView->setRubberBand(QChartView::HorizontalRubberBand);

    m_qualityPie = new QPieSeries(this);
    auto *pieChart = new QChart();
    pieChart->addSeries(m_qualityPie);
    pieChart->setTitle("Répartition qualité");
    pieChart->legend()->setVisible(true);

    m_pieChartView = new QChartView(pieChart, m_tabStatistiques);
    m_pieChartView->setRenderHint(QPainter::Antialiasing);

    chartsRow->addWidget(m_lineChartView, 2);
    chartsRow->addWidget(m_pieChartView, 1);
    main->addLayout(chartsRow, 1);

    ui->tabWidgetProduction->addTab(m_tabStatistiques, "📈  Statistiques");
}

void Production::refreshQualityPie()
{
    if (!m_qualityPie) return;

    m_qualityPie->clear();
    QSqlQuery q(QSqlDatabase::database("production_conn"));
    q.exec("SELECT NVL(QUALITE, 'Non classée') AS Q, COUNT(*) "
           "FROM PRODUCTION GROUP BY NVL(QUALITE, 'Non classée')");

    while (q.next()) {
        const QString label = q.value(0).toString();
        const qreal count = q.value(1).toDouble();
        if (count > 0.0) {
            m_qualityPie->append(label, count);
        }
    }
}

void Production::refreshStatistics()
{
    QSqlQuery kpi(QSqlDatabase::database("production_conn"));
    bool hasAcidity = kpi.exec(
        "SELECT "
        "NVL(SUM(CASE WHEN STATUT = 'En cours' THEN 1 ELSE 0 END),0), "
        "NVL(SUM(CASE WHEN STATUT = 'Termine' OR STATUT = 'Qualité validée' THEN 1 ELSE 0 END),0), "
        "NVL(AVG(RENDEMENT),0), "
        "NVL(AVG(ACIDITE),0), "
        "NVL(AVG(CASE WHEN CONFORMENORMES IS NOT NULL THEN CONFORMENORMES * 100 END),0) "
        "FROM PRODUCTION");

    if (!hasAcidity) {
        kpi.exec(
            "SELECT "
            "NVL(SUM(CASE WHEN STATUT = 'En cours' THEN 1 ELSE 0 END),0), "
            "NVL(SUM(CASE WHEN STATUT = 'Termine' OR STATUT = 'Qualité validée' THEN 1 ELSE 0 END),0), "
            "NVL(AVG(RENDEMENT),0), "
            "0, "
            "NVL(AVG(CASE WHEN CONFORMENORMES IS NOT NULL THEN CONFORMENORMES * 100 END),0) "
            "FROM PRODUCTION");
    }

    int runningCount = 0;
    int finishedCount = 0;
    double avgRend = 0.0;
    double avgAcid = 0.0;
    double conformityRate = 0.0;
    if (kpi.next()) {
        runningCount = kpi.value(0).toInt();
        finishedCount = kpi.value(1).toInt();
        avgRend = kpi.value(2).toDouble();
        avgAcid = kpi.value(3).toDouble();
        conformityRate = kpi.value(4).toDouble();
    }

    if (m_lblStatProdToday) {
        m_lblStatProdToday->setText(
            QString("Terminées: %1 | En cours: %2").arg(finishedCount).arg(runningCount));
    }
    if (m_lblStatRendAvg) m_lblStatRendAvg->setText(QString("Rendement moyen: %1 %").arg(avgRend, 0, 'f', 2));
    if (m_lblStatAcidAvg) m_lblStatAcidAvg->setText(QString("Acidité moyenne: %1 %").arg(avgAcid, 0, 'f', 2));
    if (m_lblStatConformity) m_lblStatConformity->setText(QString("Conformité: %1 %").arg(conformityRate, 0, 'f', 1));

    QSqlQuery totals(QSqlDatabase::database("production_conn"));
    bool hasToday = totals.exec(
        "SELECT NVL(SUM(QUANTITEOLIVESKG),0), NVL(SUM(HUILEPRODUITEL),0), NVL(AVG(RENDEMENT),0) "
        "FROM PRODUCTION WHERE TRUNC(DATEPRODUCTION) = TRUNC(SYSDATE)");

    double olives = 0.0;
    double huile = 0.0;
    double rend = 0.0;
    if (hasToday && totals.next()) {
        olives = totals.value(0).toDouble();
        huile = totals.value(1).toDouble();
        rend = totals.value(2).toDouble();
    }

    if (olives <= 0.0 && huile <= 0.0 && rend <= 0.0) {
        totals.exec("SELECT NVL(SUM(QUANTITEOLIVESKG),0), NVL(SUM(HUILEPRODUITEL),0), NVL(AVG(RENDEMENT),0) FROM PRODUCTION");
        if (totals.next()) {
            olives = totals.value(0).toDouble();
            huile = totals.value(1).toDouble();
            rend = totals.value(2).toDouble();
        }
    }

    m_statsTick++;
    if (m_seriesOlives && m_seriesHuile && m_seriesRendement) {
        m_seriesOlives->append(m_statsTick, olives);
        m_seriesHuile->append(m_statsTick, huile);
        m_seriesRendement->append(m_statsTick, rend);

        const int maxPoints = 60;
        if (m_seriesOlives->count() > maxPoints) m_seriesOlives->removePoints(0, m_seriesOlives->count() - maxPoints);
        if (m_seriesHuile->count() > maxPoints) m_seriesHuile->removePoints(0, m_seriesHuile->count() - maxPoints);
        if (m_seriesRendement->count() > maxPoints) m_seriesRendement->removePoints(0, m_seriesRendement->count() - maxPoints);

        if (m_axisStatsX) {
            m_axisStatsX->setRange(qMax(0, m_statsTick - maxPoints), qMax(maxPoints, m_statsTick));
        }
        if (m_axisStatsYVolume) {
            const double maxVolume = qMax(100.0, qMax(olives, huile));
            m_axisStatsYVolume->setRange(0.0, maxVolume * 1.15);
        }
        if (m_axisStatsYRendement) {
            const double maxRend = qMax(20.0, rend * 1.25);
            m_axisStatsYRendement->setRange(0.0, maxRend);
        }

        if (m_lineChartView && m_lineChartView->chart()) {
            m_lineChartView->chart()->update();
        }
    }

    refreshQualityPie();
}

// ============================================================================
// THEME: DARK / LIGHT MODE
// ============================================================================

void Production::applyTheme()
{
    // ── Dark stylesheet ──────────────────────────────────────────────────────
    static const QString DARK_SS =
        "QMainWindow { background: #0d1117; }\n"
        "#headerWidget { background: qlineargradient(x1:0,y1:0,x2:1,y2:0,"
        "stop:0 #0a1f12,stop:0.5 #0d2818,stop:1 #0a1f12); border-bottom: 4px solid #C9A227; }\n"
        "#lblAppTitle { color: #e6edf3; font-size: 26px; font-weight: 700; letter-spacing: 1px; }\n"
        "#lblAppSubtitle { color: #C9A227; font-size: 12px; font-weight: 600; letter-spacing: 3px; }\n"
        "#lblOnlineStatus { color: #4ADE80; font-size: 13px; font-weight: bold; padding: 6px 14px;"
        " background-color: rgba(74,222,128,0.15); border-radius: 15px; border: 1px solid rgba(74,222,128,0.3); }\n"
        "#btnBackToMenu { background-color: rgba(255,255,255,0.12); color: white;"
        " font-size: 13px; font-weight: 600; padding: 10px 20px; border-radius: 8px;"
        " border: 1px solid rgba(255,255,255,0.25); min-width: 100px; }\n"
        "#btnBackToMenu:hover { background-color: rgba(255,255,255,0.22); }\n"
        "#btnBackToMenu:pressed { background-color: rgba(255,255,255,0.08); }\n"
        "#dashboardWidget { background: transparent; }\n"
        "#dashCard1,#dashCard2,#dashCard3,#dashCard4 { background: #161b22; border-radius: 12px; border: none; }\n"
        "#dashCard1 { border-left: 4px solid #4ADE80; }\n"
        "#dashCard2 { border-left: 4px solid #3B82F6; }\n"
        "#dashCard3 { border-left: 4px solid #F59E0B; }\n"
        "#dashCard4 { border-left: 4px solid #EF4444; }\n"
        "QTabWidget::pane { border: none; background: transparent; border-radius: 12px; margin-top: 5px; }\n"
        "QTabBar { background: transparent; }\n"
        "QTabBar::tab { background: #161b22; color: #8b949e; padding: 12px 24px; margin-right: 4px;"
        " border: none; border-radius: 8px 8px 0 0; font-size: 13px; font-weight: 600; min-width: 120px; }\n"
        "QTabBar::tab:selected { background: #1B4332; color: white; font-weight: 700; }\n"
        "QTabBar::tab:hover:!selected { background: #21262d; color: #e6edf3; }\n"
        "QGroupBox { font-size: 14px; font-weight: 700; color: #e6edf3; background-color: #161b22;"
        " border: none; border-radius: 12px; margin-top: 20px; padding: 20px 15px 15px 15px; }\n"
        "QGroupBox::title { subcontrol-origin: margin; subcontrol-position: top left; left: 20px; top: 0px;"
        " padding: 8px 16px; background-color: #1B4332; color: white; border-radius: 8px; font-size: 13px; }\n"
        "QLineEdit,QSpinBox,QDoubleSpinBox,QDateEdit,QComboBox { padding: 10px 14px; border: 2px solid #30363d;"
        " border-radius: 8px; background-color: #0d1117; color: #e6edf3; font-size: 14px;"
        " min-height: 20px; selection-background-color: #1B4332; }\n"
        "QLineEdit:focus,QSpinBox:focus,QDoubleSpinBox:focus,QDateEdit:focus,QComboBox:focus"
        "{ border: 2px solid #4ADE80; background-color: #161b22; }\n"
        "QLineEdit:hover,QSpinBox:hover,QDoubleSpinBox:hover,QDateEdit:hover,QComboBox:hover"
        "{ border: 2px solid #4ADE80; }\n"
        "QComboBox::drop-down { border: none; padding-right: 15px; width: 30px; }\n"
        "QComboBox QAbstractItemView { border: 2px solid #1B4332; border-radius: 8px;"
        " background-color: #161b22; selection-background-color: #1B4332; selection-color: white;"
        " padding: 5px; color: #e6edf3; }\n"
        "QTextEdit,QTextBrowser { border: 2px solid #30363d; border-radius: 8px;"
        " background-color: #0d1117; color: #e6edf3; font-size: 14px; padding: 12px; }\n"
        "QTextEdit:focus,QTextBrowser:focus { border: 2px solid #4ADE80; background-color: #161b22; }\n"
        "QPushButton { padding: 12px 24px; border-radius: 8px; font-size: 14px;"
        " font-weight: 600; min-width: 120px; border: none; }\n"
        "QPushButton#btnPlanifier,QPushButton#btnStart,QPushButton#btnValidateQuality,"
        "QPushButton#btnGenerateReport,QPushButton#btnUpdateMonitoring,QPushButton#btnSearch,"
        "QPushButton#btnApplySort,QPushButton#btnCalculateYield"
        "{ background: qlineargradient(x1:0,y1:0,x2:0,y2:1,stop:0 #234E3E,stop:1 #1B4332);"
        " color: white; border: none; }\n"
        "QPushButton#btnPlanifier:hover,QPushButton#btnStart:hover,QPushButton#btnValidateQuality:hover,"
        "QPushButton#btnGenerateReport:hover,QPushButton#btnUpdateMonitoring:hover,QPushButton#btnSearch:hover,"
        "QPushButton#btnApplySort:hover,QPushButton#btnCalculateYield:hover"
        "{ background: qlineargradient(x1:0,y1:0,x2:0,y2:1,stop:0 #2D5A45,stop:1 #234E3E); }\n"
        "QPushButton#btnStop { background: qlineargradient(x1:0,y1:0,x2:0,y2:1,"
        "stop:0 #EF4444,stop:1 #DC2626); color: white; }\n"
        "QPushButton#btnStop:hover { background: qlineargradient(x1:0,y1:0,x2:0,y2:1,"
        "stop:0 #F87171,stop:1 #EF4444); }\n"
        "QPushButton#btnExportPDF { background: qlineargradient(x1:0,y1:0,x2:0,y2:1,"
        "stop:0 #D4A732,stop:1 #C9A227); color: white; }\n"
        "QPushButton#btnClearSearch { background-color: #30363d; color: #e6edf3; }\n"
        "QPushButton#btnClearSearch:hover { background-color: #484f58; }\n"
        "QPushButton#btnDeleteProduction { background-color: #dc2626; color: white; border-radius: 8px; }\n"
        "QPushButton#btnModifyProduction { background-color: #3B82F6; color: white; border-radius: 8px; }\n"
        "QProgressBar { border: none; border-radius: 10px; text-align: center; font-weight: 700;"
        " font-size: 13px; background-color: #21262d; min-height: 24px; color: #e6edf3; }\n"
        "QProgressBar::chunk { background: qlineargradient(x1:0,y1:0,x2:1,y2:0,"
        "stop:0 #1B4332,stop:0.5 #234E3E,stop:1 #1B4332); border-radius: 10px; }\n"
        "QTableWidget { border: none; border-radius: 8px; background-color: #161b22;"
        " gridline-color: #30363d; font-size: 13px; color: #e6edf3;"
        " selection-background-color: #1e3a5f; selection-color: #93c5fd; }\n"
        "QTableWidget::item { padding: 12px; border-bottom: 1px solid #21262d;"
        " color: #e6edf3; background-color: #161b22; }\n"
        "QTableWidget::item:selected { background-color: #1e3a5f; color: #93c5fd; }\n"
        "QTableWidget::item:hover { background-color: #21262d; }\n"
        "QTableWidget::item:alternate { background-color: #1c2128; }\n"
        "QHeaderView::section { background: qlineargradient(x1:0,y1:0,x2:0,y2:1,"
        "stop:0 #1B4332,stop:1 #163829); color: white; padding: 14px 12px; border: none;"
        " font-weight: 700; font-size: 12px; letter-spacing: 1px; }\n"
        "QHeaderView::section:first { border-top-left-radius: 8px; }\n"
        "QHeaderView::section:last { border-top-right-radius: 8px; }\n"
        "QLabel { color: #e6edf3; font-size: 14px; }\n"
        "QCheckBox { font-size: 14px; color: #e6edf3; spacing: 12px; padding: 8px; }\n"
        "QScrollBar:vertical { border: none; background: #0d1117; width: 8px; border-radius: 4px; margin: 0; }\n"
        "QScrollBar::handle:vertical { background: #30363d; border-radius: 4px; min-height: 40px; }\n"
        "QScrollBar::handle:vertical:hover { background: #484f58; }\n"
        "QScrollBar:horizontal { border: none; background: #0d1117; height: 8px; border-radius: 4px; margin: 0; }\n"
        "QScrollBar::handle:horizontal { background: #30363d; border-radius: 4px; min-width: 40px; }\n"
        "QScrollBar::add-line:vertical,QScrollBar::sub-line:vertical,"
        "QScrollBar::add-line:horizontal,QScrollBar::sub-line:horizontal { height: 0; width: 0; }\n"
        "QScrollArea { background: transparent; border: none; }\n";

    if (m_isDarkMode) {
        this->setStyleSheet(DARK_SS);
        // Patch inline-styled GroupBoxes (body bg + embedded child rules)
        for (auto *gb : findChildren<QGroupBox*>()) {
            QString s = gb->styleSheet();
            if (s.isEmpty()) continue;
            s.replace("background-color: #f8fafc", "background-color: #1a2030");
            s.replace("background: white",         "background: #1a2030");
            s.replace("background-color: white",   "background-color: #1a2030");
            s.replace("border: 2px solid #d1fae5", "border: 2px solid #2d3748");
            // Child widget overrides inside GroupBox stylesheets
            s.replace("background-color: #ffffff", "background-color: #111827");
            s.replace("color: #111827",            "color: #e6edf3");
            s.replace("color: #374151",            "color: #9ca3af");
            s.replace("color: #6b7280",            "color: #6b7280");  // keep subtle text
            gb->setStyleSheet(s);
        }
        // Scroll area contents go dark
        for (auto *w : findChildren<QWidget*>()) {
            const QString &name = w->objectName();
            if (name == "scrollContentsLancement")
                w->setStyleSheet("background: #0d1117;");
        }
    } else {
        this->setStyleSheet(m_lightStyleSheet);
        // Restore inline-styled GroupBoxes
        for (auto *gb : findChildren<QGroupBox*>()) {
            QString s = gb->styleSheet();
            if (s.isEmpty()) continue;
            s.replace("background-color: #1a2030", "background-color: #f8fafc");
            s.replace("background: #1a2030",       "background: white");
            s.replace("background-color: #1a2030", "background-color: white");
            s.replace("border: 2px solid #2d3748", "border: 2px solid #d1fae5");
            s.replace("background-color: #111827", "background-color: #ffffff");
            s.replace("color: #e6edf3",            "color: #111827");
            s.replace("color: #9ca3af",            "color: #374151");
            gb->setStyleSheet(s);
        }
        for (auto *w : findChildren<QWidget*>()) {
            if (w->objectName() == "scrollContentsLancement")
                w->setStyleSheet("background: transparent;");
        }
    }

    // Update dark mode button text based on current language
    applyTranslations();
}

void Production::onToggleDarkMode()
{
    m_isDarkMode = !m_isDarkMode;
    applyTheme();
}

// ============================================================================
// LANGUAGE: FR → EN → AR (cycle)
// ============================================================================

void Production::applyTranslations()
{
    // Translation arrays: index 0=FR, 1=EN, 2=AR
    static const QStringList winTitle   = {"PressIQ - Gestion de la Production v2.0",
                                           "PressIQ - Production Management v2.0",
                                           "PressIQ - إدارة الإنتاج v2.0"};
    static const QStringList appTitle   = {"Gestion de la Production",
                                           "Production Management",
                                           "إدارة الإنتاج"};
    static const QStringList appSub     = {"MODULE DE PRODUCTION INTELLIGENT",
                                           "INTELLIGENT PRODUCTION MODULE",
                                           "وحدة الإنتاج الذكي"};
    static const QStringList dashL1     = {"STATUT PRODUCTION",  "PRODUCTION STATUS",   "حالة الإنتاج"};
    static const QStringList dashL2     = {"OLIVES PLANIFIÉES",  "PLANNED OLIVES",      "الزيتون المخطط"};
    static const QStringList dashL3     = {"RENDEMENT ESTIMÉ",   "ESTIMATED YIELD",     "المردودية المتوقعة"};
    static const QStringList dashL4     = {"ALERTES ACTIVES",    "ACTIVE ALERTS",       "التنبيهات النشطة"};
    static const QStringList tab0t      = {"📋  Planification",  "📋  Planning",        "📋  التخطيط"};
    static const QStringList tab1t      = {"▶  Lancement",       "▶  Launch",           "▶  الإطلاق"};
    static const QStringList tab2t      = {"📍  Suivi",          "📍  Monitoring",      "📍  المتابعة"};
    static const QStringList tab3t      = {"📊  Rendement",      "📊  Yield",           "📊  المردودية"};
    static const QStringList tab4t      = {"✓  Qualité",         "✓  Quality",          "✓  الجودة"};
    static const QStringList tab5t      = {"≡  Rapports",        "≡  Reports",          "≡  التقارير"};
    static const QStringList tab6t      = {"📈  Statistiques",    "📈  Statistics",       "📈  الإحصائيات"};
    static const QStringList btnBack    = {"← Retour au menu",   "← Back to Menu",      "← القائمة"};
    static const QStringList btnPlan    = {"✚  Planifier",       "✚  Plan",             "✚  تخطيط"};
    static const QStringList btnStrt    = {"▶  Démarrer",        "▶  Start",            "▶  ابدأ"};
    static const QStringList btnStp     = {"⏸  Pause",           "⏸  Pause",            "⏸  إيقاف"};
    static const QStringList btnFin     = {"✔  Terminer",        "✔  Finish",           "✔  إنهاء"};
    static const QStringList btnCalc    = {"📊  Calculer le rendement", "📊  Calculate Yield", "📊  احسب المردودية"};
    static const QStringList btnUpd     = {"🔄  Actualiser le suivi",   "🔄  Refresh",         "🔄  تحديث"};
    static const QStringList btnMod     = {"✏  Modifier",        "✏  Edit",             "✏  تعديل"};
    static const QStringList btnDel     = {"🗑  Supprimer",      "🗑  Delete",           "🗑  حذف"};
    static const QStringList grpRun     = {" Production en cours ",         " Running Production ",      " الإنتاج الجاري "};
    static const QStringList grpHist    = {" Historique des productions ",  " Production History ",      " تاريخ الإنتاج "};
    static const QStringList grpSel     = {" Sélectionner une production planifiée ", " Select Planned Production ", " اختيار إنتاج مخطط "};
    static const QStringList grpStat    = {" État de la production ",       " Production Status ",       " حالة الإنتاج "};
    static const QStringList grpProg    = {" Progression de la production "," Production Progress ",     " تقدم الإنتاج "};
    static const QStringList grpCtrl    = {" Contrôles de production ",     " Production Controls ",     " تحكم في الإنتاج "};
    static const QStringList grpTerm    = {" Sélectionner une production terminée ", " Select Completed Production ", " اختيار إنتاج منتهٍ "};
    // Dark mode button: shows what action will happen (i.e. current state inverse)
    static const QStringList btnDark    = {"🌙  Mode Sombre",    "🌙  Dark Mode",        "🌙  الوضع الداكن"};
    static const QStringList btnLight   = {"☀  Mode Clair",      "☀  Light Mode",        "☀  الوضع الفاتح"};
    static const QStringList btnLang    = {"🌐  EN",             "🌐  AR",               "🌐  FR"}; // shows NEXT
    static const QStringList onlineT    = {"● EN LIGNE",         "● ONLINE",             "● متصل"};

    const int L = m_langIndex;

    // Window & header
    setWindowTitle(winTitle[L]);
    ui->lblAppTitle->setText(appTitle[L]);
    ui->lblAppSubtitle->setText(appSub[L]);
    ui->lblOnlineStatus->setText(onlineT[L]);

    // Dashboard card labels
    ui->lblDashLabel1->setText(dashL1[L]);
    ui->lblDashLabel2->setText(dashL2[L]);
    ui->lblDashLabel3->setText(dashL3[L]);
    ui->lblDashLabel4->setText(dashL4[L]);

    // Tabs
    ui->tabWidgetProduction->setTabText(0, tab0t[L]);
    ui->tabWidgetProduction->setTabText(1, tab1t[L]);
    ui->tabWidgetProduction->setTabText(2, tab2t[L]);
    ui->tabWidgetProduction->setTabText(3, tab3t[L]);
    ui->tabWidgetProduction->setTabText(4, tab4t[L]);
    ui->tabWidgetProduction->setTabText(5, tab5t[L]);
    if (ui->tabWidgetProduction->count() > 6) {
        ui->tabWidgetProduction->setTabText(6, tab6t[L]);
    }

    // Buttons
    ui->btnBackToMenu->setText(btnBack[L]);
    ui->btnPlanifier->setText(btnPlan[L]);
    ui->btnStart->setText(btnStrt[L]);
    ui->btnStop->setText(btnStp[L]);
    ui->btnFinishProduction->setText(btnFin[L]);
    ui->btnCalculateYield->setText(btnCalc[L]);
    ui->btnUpdateMonitoring->setText(btnUpd[L]);
    ui->btnModifyProduction->setText(btnMod[L]);
    ui->btnDeleteProduction->setText(btnDel[L]);

    // Theme & language toggle buttons
    ui->btnDarkMode->setText(m_isDarkMode ? btnLight[L] : btnDark[L]);
    ui->btnLanguage->setText(btnLang[L]);  // shows the next language
    if (m_btnFullscreen) {
        static const QStringList btnFsEnter = {"⛶ Plein écran", "⛶ Fullscreen", "⛶ شاشة كاملة"};
        static const QStringList btnFsExit  = {"🗗 Fenêtre", "🗗 Windowed", "🗗 نافذة"};
        m_btnFullscreen->setText(isFullScreen() ? btnFsExit[L] : btnFsEnter[L]);
    }

    // GroupBox titles (badge-style ones with setTitle)
    ui->groupRunningProduction->setTitle(grpRun[L]);
    ui->groupHistory->setTitle(grpHist[L]);
    ui->groupSelectProduction->setTitle(grpSel[L]);
    ui->groupLancementStatus->setTitle(grpStat[L]);
    ui->groupProgress->setTitle(grpProg[L]);
    ui->groupControls->setTitle(grpCtrl[L]);
    ui->groupSelectTerminated->setTitle(grpTerm[L]);

    // Layout direction for Arabic (RTL)
    setLayoutDirection(L == 2 ? Qt::RightToLeft : Qt::LeftToRight);
}

void Production::onToggleLanguage()
{
    m_langIndex = (m_langIndex + 1) % 3;
    applyTranslations();
}

void Production::onToggleFullscreen()
{
    if (isFullScreen()) {
        showNormal();
        if (m_btnFullscreen) {
            m_btnFullscreen->setText("⛶ Plein écran");
        }
    } else {
        showFullScreen();
        if (m_btnFullscreen) {
            m_btnFullscreen->setText("🗗 Fenêtre");
        }
    }
}

void Production::onTerminatedProductionSelected(int index)
{
    if (index <= 0) {
        m_rendementProductionId = 0;
        ui->lblOlivesUsed->setText("-- kg");
        ui->spinOilProduced->setValue(0.0);
        ui->lblTerminatedInfo->setText("Sélectionnez une production pour charger ses données automatiquement");
        return;
    }

    int prodId = ui->comboTerminatedProductions->currentData().toInt();
    if (prodId <= 0) return;

    QSqlQuery q(QSqlDatabase::database("production_conn"));
    q.prepare("SELECT QUANTITEOLIVESKG, HUILEPRODUITEL, TYPEPRESSAGE, DATEPRODUCTION "
              "FROM PRODUCTION WHERE IDPRODUCTION = :id");
    q.bindValue(":id", prodId);
    if (!q.exec() || !q.next()) return;

    int     kg    = q.value(0).toInt();
    double  huile = q.value(1).toDouble();
    QString type  = q.value(2).toString();
    QString date  = q.value(3).toDate().toString("dd/MM/yyyy");

    m_rendementProductionId = prodId;
    ui->lblOlivesUsed->setText(QString("%1 kg").arg(kg));
    ui->spinOilProduced->setValue(huile);
    ui->lblTerminatedInfo->setText(
        QString("✓ Production #%1 chargée — %2 — %3 — %4 kg olives / %5 L huile")
            .arg(prodId).arg(date).arg(type).arg(kg).arg(huile, 0, 'f', 2)
    );
}

// ============================================================================
// CRUD: UPDATE STATUS - START  →  launches simulation
// ============================================================================

void Production::onStartClicked()
{
    if (m_isRunning) {
        showErrorNotification("Une production est déjà en cours!");
        return;
    }

    // Read selected production from combo
    int idx = ui->comboSelectProduction->currentIndex();
    int prodId = ui->comboSelectProduction->currentData().toInt();
    if (idx <= 0 || prodId <= 0) {
        showErrorNotification("Sélectionnez une production planifiée dans la liste!");
        return;
    }

    // Load full row from DB
    QSqlQuery q(QSqlDatabase::database("production_conn"));
    q.prepare("SELECT QUANTITEOLIVESKG, DUREEESTIMEE, TYPEPRESSAGE "
              "FROM PRODUCTION WHERE IDPRODUCTION = :id");
    q.bindValue(":id", prodId);
    if (!q.exec() || !q.next()) {
        showErrorNotification("Impossible de charger la production.");
        return;
    }
    m_olivesKgTotal    = q.value(0).toInt();
    m_totalDurationMin = qMax(q.value(1).toInt(), 1);
    m_simProductionId  = prodId;
    currentProductionId = prodId;

    // Update DB status
    QSqlQuery upd(QSqlDatabase::database("production_conn"));
    upd.prepare("UPDATE PRODUCTION SET STATUT = 'En cours', "
                "HEUREMARRAGE = :h WHERE IDPRODUCTION = :id");
    upd.bindValue(":h",  QTime::currentTime().toString("HH:mm:ss"));
    upd.bindValue(":id", prodId);
    upd.exec();

    // Reset simulation state
    m_simTicks    = 0;
    m_oilProducedL = 0.0;
    m_isRunning   = true;
    m_isPaused    = false;
    m_realStartTime = QTime::currentTime();

    // Update UI
    ui->btnStart->setEnabled(false);
    ui->btnStop->setEnabled(true);
    ui->btnFinishProduction->setEnabled(false);
    ui->comboSelectProduction->setEnabled(false);
    ui->btnRefreshPlanned->setEnabled(false);

    updateSimUI(0, 0);
    updateDashboard("En cours", m_olivesKgTotal, 0.0, 0);

    m_simTimer->start();
    loadProductionHistory();
}

// ============================================================================
// CRUD: UPDATE STATUS - STOP  →  pauses simulation
// ============================================================================

void Production::onStopClicked()
{
    if (!m_isRunning) {
        showErrorNotification("Aucune production en cours!");
        return;
    }

    m_simTimer->stop();
    m_isRunning = false;
    m_isPaused  = true;

    // Update DB
    QSqlQuery upd(QSqlDatabase::database("production_conn"));
    upd.prepare("UPDATE PRODUCTION SET STATUT = 'Arrete', "
                "TEMPSECOULE = :t WHERE IDPRODUCTION = :id");
    upd.bindValue(":t",  m_simTicks);
    upd.bindValue(":id", m_simProductionId);
    upd.exec();

    ui->btnStart->setEnabled(true);
    ui->btnStop->setEnabled(true);
    ui->btnFinishProduction->setEnabled(true);
    ui->comboSelectProduction->setEnabled(true);
    ui->btnRefreshPlanned->setEnabled(true);

    ui->lblCurrentStatus->setText("⏸  Production en pause");
    ui->lblCurrentStatus->setStyleSheet(
        "background-color: #FEF3C7; color: #92400E; border: 2px solid #FCD34D; "
        "font-size: 15px; font-weight: 700; padding: 10px 18px; border-radius: 10px;");

    // Update Suivi tab status badge
    ui->lblRunningStatus->setText("⏸  En pause");
    ui->lblRunningStatus->setStyleSheet(
        "background-color: #FEF3C7; color: #92400E; border: 2px solid #FCD34D; "
        "font-size: 14px; font-weight: 700; padding: 8px 16px; border-radius: 10px;");

    updateDashboard("En pause", m_olivesKgTotal, 0.0, 0);
    loadProductionHistory();
}

// ============================================================================
// SIMULATION: TICK (every 500ms real = 1 production minute)
// ============================================================================

void Production::onSimTick()
{
    m_simTicks++;

    // 1 real second = 2 prod minutes  (1h prod = 30s real = 60 ticks×500ms)
    int prodMinutesElapsed = m_simTicks;   // 1 tick = 1 prod minute

    int progressPct = qMin(prodMinutesElapsed * 100 / m_totalDurationMin, 100);

    // Simulate oil production with a realistic S-curve:
    // - Pressing phase (25-60%) produces 80% of oil
    // - Ramp up from 10%, peak at 60%, plateau at 90%
    double pctD = progressPct / 100.0;
    double oilFactor = 0.0;
    if (pctD < 0.10)      oilFactor = 0.0;
    else if (pctD < 0.25) oilFactor = (pctD - 0.10) / 0.15 * 0.15;
    else if (pctD < 0.60) oilFactor = 0.15 + (pctD - 0.25) / 0.35 * 0.65;
    else if (pctD < 0.85) oilFactor = 0.80 + (pctD - 0.60) / 0.25 * 0.18;
    else                   oilFactor = 0.98 + (pctD - 0.85) / 0.15 * 0.02;

    // Expected rendement: realistic range 14-20% for olive oil
    double expectedRendement = 0.16; // 16% average
    double targetOil = m_olivesKgTotal * expectedRendement * oilFactor;
    m_oilProducedL = targetOil;

    double rendementNow = (m_olivesKgTotal > 0)
                        ? (m_oilProducedL / m_olivesKgTotal * 100.0)
                        : 0.0;

    updateSimUI(progressPct, prodMinutesElapsed);
    updateDashboard("En cours", m_olivesKgTotal, rendementNow, 0);

    // Update DB periodically (every 10 ticks = 10 prod minutes)
    if (m_simTicks % 10 == 0) {
        QSqlQuery upd(QSqlDatabase::database("production_conn"));
        upd.prepare("UPDATE PRODUCTION SET HUILEPRODUITEL = :h, RENDEMENT = :r, "
                    "TEMPSECOULE = :t WHERE IDPRODUCTION = :id");
        upd.bindValue(":h",  m_oilProducedL);
        upd.bindValue(":r",  rendementNow);
        upd.bindValue(":t",  m_simTicks);
        upd.bindValue(":id", m_simProductionId);
        upd.exec();
        loadProductionHistory();
        refreshStatistics();
    }

    // Auto-finish at 100%
    if (progressPct >= 100) {
        m_simTimer->stop();
        m_isRunning = false;
        ui->btnFinishProduction->setEnabled(true);
        ui->btnStop->setEnabled(false);

        ui->lblCurrentStatus->setText("✅  Production terminée!");
        ui->lblCurrentStatus->setStyleSheet(
            "background-color: #D1FAE5; color: #065F46; border: 2px solid #34D399; "
            "font-size: 15px; font-weight: 700; padding: 10px 18px; border-radius: 10px;");

        // Final DB update
        QSqlQuery fin(QSqlDatabase::database("production_conn"));
        fin.prepare("UPDATE PRODUCTION SET STATUT = 'Termine', "
                    "HUILEPRODUITEL = :h, RENDEMENT = :r, "
                    "TEMPSECOULE = :t WHERE IDPRODUCTION = :id");
        fin.bindValue(":h",  m_oilProducedL);
        fin.bindValue(":r",  rendementNow);
        fin.bindValue(":t",  m_simTicks);
        fin.bindValue(":id", m_simProductionId);
        fin.exec();

        updateDashboard("Terminé", m_olivesKgTotal, rendementNow, 0);
        loadProductionHistory();
        loadPlannedProductions();
        loadTerminatedProductions();
        loadQualityEvaluableProductions();
        refreshStatistics();
        ui->comboSelectProduction->setEnabled(true);
        ui->btnRefreshPlanned->setEnabled(true);
    }
}

// ============================================================================
// SIMULATION: update the Lancement tab UI elements
// ============================================================================

void Production::updateSimUI(int progressPct, int prodMinutesElapsed)
{
    // ── Lancement tab ────────────────────────────────────────────────────────
    ui->progressProduction->setValue(progressPct);

    int kgDone = m_olivesKgTotal * progressPct / 100;
    ui->lblProgressEnd->setText(
        QString("%1 / %2 kg traités").arg(kgDone).arg(m_olivesKgTotal));

    int realSecs = m_realStartTime.secsTo(QTime::currentTime());
    QTime realElapsed = QTime(0, 0).addSecs(qMax(realSecs, 0));
    ui->lblElapsedTime->setText("⏱  " + realElapsed.toString("HH:mm:ss"));

    int prodHours = prodMinutesElapsed / 60;
    int prodMins  = prodMinutesElapsed % 60;
    ui->lblStartTime->setText(
        QString("🏭  %1h %2min").arg(prodHours).arg(prodMins, 2, 10, QChar('0')));

    QString phase;
    if      (progressPct < 10) phase = "⏳ Chargement des olives";
    else if (progressPct < 25) phase = "🚿 Lavage et nettoyage";
    else if (progressPct < 60) phase = "⚙  Pressage";
    else if (progressPct < 80) phase = "🔄 Centrifugation";
    else if (progressPct < 95) phase = "💧 Séparation huile/eau";
    else                        phase = "🫙 Stockage et finalisation";
    ui->lblPhase->setText("Phase : " + phase);

    if (progressPct < 100) {
        ui->lblCurrentStatus->setText("▶  Production en cours");
        ui->lblCurrentStatus->setStyleSheet(
            "background-color: #DBEAFE; color: #1E40AF; border: 2px solid #93C5FD; "
            "font-size: 15px; font-weight: 700; padding: 10px 18px; border-radius: 10px;");
    }

    QString pbColor = (progressPct >= 80) ? "#059669" :
                      (progressPct >= 40) ? "#16a34a" : "#22c55e";
    ui->progressProduction->setStyleSheet(
        QString("QProgressBar { background: #e5e7eb; border-radius: 8px; "
                "text-align: center; font-weight: 700; color: #1f2937; height: 35px; } "
                "QProgressBar::chunk { background: qlineargradient(x1:0,y1:0,x2:1,y2:0,"
                "stop:0 %1, stop:1 #1B4332); border-radius: 8px; }").arg(pbColor));

    // ── Suivi tab — synchronize running widget ───────────────────────────────
    double rendNow = (m_olivesKgTotal > 0) ? (m_oilProducedL / m_olivesKgTotal * 100.0) : 0.0;

    ui->lblRunningProduction->setVisible(false);
    ui->widgetRunningActive->setVisible(true);

    ui->progressRunning->setValue(progressPct);
    ui->progressRunning->setFormat(progressPct < 100
        ? QString("%1% — %2").arg(progressPct).arg(phase)
        : "100% — Terminé ✅");
    ui->progressRunning->setStyleSheet(
        QString("QProgressBar { background: #e5e7eb; border-radius: 8px; text-align: center; "
                "font-weight: 700; font-size: 12px; color: #1f2937; } "
                "QProgressBar::chunk { background: qlineargradient(x1:0,y1:0,x2:1,y2:0,"
                "stop:0 %1, stop:1 #1B4332); border-radius: 8px; }").arg(pbColor));

    ui->lblRunningPhase->setText("Phase : " + phase);
    ui->lblRunningOlives->setText(QString("🫒 Olives\n%1 kg").arg(kgDone));
    ui->lblRunningOil->setText(QString("🛢 Huile\n%1 L").arg(m_oilProducedL, 0, 'f', 2));
    ui->lblRunningRendement->setText(QString("📊 Rendement\n%1 %").arg(rendNow, 0, 'f', 2));
    ui->lblRunningSimTime->setText(
        QString("🏭 Simulé\n%1h %2min").arg(prodHours).arg(prodMins, 2, 10, QChar('0')));
    ui->lblRunningRealTime->setText("⏱ Réel\n" + realElapsed.toString("HH:mm:ss"));

    if (progressPct >= 100) {
        ui->lblRunningStatus->setText("✅  Terminée");
        ui->lblRunningStatus->setStyleSheet(
            "background-color: #D1FAE5; color: #065F46; border: 2px solid #34D399; "
            "font-size: 14px; font-weight: 700; padding: 8px 16px; border-radius: 10px;");
    } else {
        ui->lblRunningStatus->setText("▶  En cours");
        ui->lblRunningStatus->setStyleSheet(
            "background-color: #DBEAFE; color: #1E40AF; border: 2px solid #93C5FD; "
            "font-size: 14px; font-weight: 700; padding: 8px 16px; border-radius: 10px;");
    }
}

// ============================================================================
// SIMULATION: update top dashboard cards
// ============================================================================

void Production::updateDashboard(const QString &status, int olivesKg,
                                  double rendPct, int alertCount)
{
    ui->lblDashValue1->setText(status);
    ui->lblDashValue2->setText(QString::number(olivesKg) + " kg");
    ui->lblDashValue3->setText(rendPct > 0
        ? QString::number(rendPct, 'f', 1) + " %"
        : "-- %");
    ui->lblDashValue4->setText(QString::number(alertCount));
}

// ============================================================================
// FINISH PRODUCTION manually
// ============================================================================

void Production::onFinishProductionClicked()
{
    if (m_simProductionId <= 0) {
        showErrorNotification("Aucune production active à terminer.");
        return;
    }

    m_simTimer->stop();
    m_isRunning = false;

    double rendement = (m_olivesKgTotal > 0)
                     ? (m_oilProducedL / m_olivesKgTotal * 100.0) : 0.0;

    QSqlQuery fin(QSqlDatabase::database("production_conn"));
    fin.prepare("UPDATE PRODUCTION SET STATUT = 'Termine', "
                "HUILEPRODUITEL = :h, RENDEMENT = :r, "
                "TEMPSECOULE = :t, DATEGENERATIONRAPPORT = SYSDATE "
                "WHERE IDPRODUCTION = :id");
    fin.bindValue(":h",  m_oilProducedL);
    fin.bindValue(":r",  rendement);
    fin.bindValue(":t",  m_simTicks);
    fin.bindValue(":id", m_simProductionId);

    if (fin.exec()) {
        showSuccessNotification(
            "✅ Production terminée et enregistrée!\n\n"
            "ID  : " + QString::number(m_simProductionId) + "\n"
            "Huile produite : " + QString::number(m_oilProducedL, 'f', 2) + " L\n"
            "Rendement : " + QString::number(rendement, 'f', 2) + " %"
        );
    } else {
        showErrorNotification("Erreur lors de la finalisation: " + fin.lastError().text());
    }

    // Reset state
    m_simProductionId = 0;
    m_simTicks = 0;
    m_oilProducedL = 0.0;
    ui->progressProduction->setValue(100);
    ui->btnStart->setEnabled(true);
    ui->btnStop->setEnabled(true);
    ui->btnFinishProduction->setEnabled(false);
    ui->comboSelectProduction->setEnabled(true);
    ui->btnRefreshPlanned->setEnabled(true);
    ui->lblCurrentStatus->setText("🟢  Prêt à démarrer");
    ui->lblCurrentStatus->setStyleSheet(
        "background-color: #D1FAE5; color: #065F46; border: 2px solid #34D399; "
        "font-size: 15px; font-weight: 700; padding: 10px 18px; border-radius: 10px;");

    // Reset Suivi tab back to idle banner
    ui->widgetRunningActive->setVisible(false);
    ui->lblRunningProduction->setVisible(true);
    ui->lblRunningProduction->setText("ℹ  Production terminée. Sélectionnez et lancez une nouvelle production.");

    updateDashboard("En attente", 0, 0.0, 0);
    loadProductionHistory();
    loadPlannedProductions();
    loadTerminatedProductions();
    loadQualityEvaluableProductions();
    refreshStatistics();
}

// ============================================================================
// CRUD: CALCULATE YIELD
// ============================================================================

void Production::onCalculateYieldClicked()
{
    // Read olives from the display label (set by combo selection)
    QString olivesText = ui->lblOlivesUsed->text().remove(" kg").trimmed();
    int olivesKg = olivesText.toInt();
    double oilLiters = ui->spinOilProduced->value();

    if (olivesKg <= 0) {
        showErrorNotification("Quantité d'olives invalide!\n\nSélectionnez d'abord une production terminée.");
        return;
    }
    if (oilLiters <= 0) {
        showErrorNotification("Quantité d'huile invalide!\n\nEntrez la quantité d'huile produite.");
        return;
    }

    double rendement = (oilLiters / static_cast<double>(olivesKg)) * 100.0;

    // Update UI
    ui->lblYield->setText(QString::number(rendement, 'f', 2) + " %");
    ui->progressYield->setValue(qMin(static_cast<int>(rendement * 5), 100)); // scale: 20% = 100%

    QString evaluation;
    QString evalStyle;
    if (rendement >= 18) {
        evaluation = "🌟 Excellent rendement ! Qualité supérieure.";
        evalStyle  = "background-color: #f0fdf4; padding: 18px; border-left: 5px solid #22c55e; font-size: 14px; border-radius: 0 8px 8px 0; color: #166534;";
    } else if (rendement >= 15) {
        evaluation = "✓ Bon rendement — dans la norme (15-22%).";
        evalStyle  = "background-color: #f0fdf4; padding: 18px; border-left: 5px solid #4ade80; font-size: 14px; border-radius: 0 8px 8px 0; color: #166534;";
    } else if (rendement >= 10) {
        evaluation = "⚠ Rendement moyen — en dessous de la norme optimale.";
        evalStyle  = "background-color: #fefce8; padding: 18px; border-left: 5px solid #C9A227; font-size: 14px; border-radius: 0 8px 8px 0; color: #713f12;";
    } else {
        evaluation = "⛔ Rendement faible — vérifiez la qualité des olives et le processus.";
        evalStyle  = "background-color: #fef2f2; padding: 18px; border-left: 5px solid #ef4444; font-size: 14px; border-radius: 0 8px 8px 0; color: #991b1b;";
    }
    ui->lblYieldMessage->setText(evaluation + "\n\nOlives : " + QString::number(olivesKg)
        + " kg   |   Huile : " + QString::number(oilLiters, 'f', 2)
        + " L   |   Rendement : " + QString::number(rendement, 'f', 2) + " %");
    ui->lblYieldMessage->setStyleSheet(evalStyle);

    // Save to DB for the selected terminated production
    int targetId = (m_rendementProductionId > 0) ? m_rendementProductionId : currentProductionId;
    if (targetId > 0) {
        QSqlQuery upd(QSqlDatabase::database("production_conn"));
        upd.prepare("UPDATE PRODUCTION SET RENDEMENT = :r, HUILEPRODUITEL = :h "
                    "WHERE IDPRODUCTION = :id");
        upd.bindValue(":r",  rendement);
        upd.bindValue(":h",  oilLiters);
        upd.bindValue(":id", targetId);
        if (upd.exec()) {
            showSuccessNotification(
                "📊 Rendement calculé et enregistré!\n\n"
                "Production #" + QString::number(targetId) + "\n"
                "Olives : " + QString::number(olivesKg) + " kg\n"
                "Huile  : " + QString::number(oilLiters, 'f', 2) + " L\n"
                "Rendement : " + QString::number(rendement, 'f', 2) + " %\n\n"
                + evaluation);
            loadProductionHistory();
            loadTerminatedProductions();
            loadQualityEvaluableProductions();
            updateReportPreview();
            refreshStatistics();
        } else {
            showErrorNotification("Calcul effectué mais échec de l'enregistrement:\n" + upd.lastError().text());
        }
    } else {
        showSuccessNotification("📊 Rendement calculé : " + QString::number(rendement, 'f', 2) + " %\n\n"
                                "Note : Sélectionnez une production pour enregistrer.");
    }
}

// ============================================================================
// CRUD: VALIDATE QUALITY
// ============================================================================

void Production::onValidateQualityClicked()
{
    if ((m_qualityProductionId <= 0) && (currentProductionId <= 0)) {
        showErrorNotification("Aucune production sélectionnée pour l'évaluation qualité.");
        return;
    }
    autoEvaluateQuality(true, true);
}

// ============================================================================
// GENERATE REPORT → update DB + export professional PDF
// ============================================================================

void Production::onGenerateReportClicked()
{
    QString periodLabel;
    const QString whereClause = buildReportWhereClause(&periodLabel);
    const QString filterText = QString("Période: %1 | Qualité: %2")
        .arg(periodLabel.isEmpty() ? "Global" : periodLabel)
        .arg(m_comboReportQuality ? m_comboReportQuality->currentText() : "Toutes qualités");

    updateReportPreview();

    // Ask where to save the PDF
    QString defaultName = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)
                        + "/Rapport_Production_" + QDate::currentDate().toString("yyyyMMdd") + ".pdf";
    QString filePath = QFileDialog::getSaveFileName(
        this,
        "Enregistrer le rapport PDF",
        defaultName,
        "Fichiers PDF (*.pdf)"
    );

    if (filePath.isEmpty()) return;

    generatePdf(filePath, whereClause, filterText);

    showSuccessNotification("📄 Rapport PDF généré avec succès!\n\n"
                            "Fichier : " + filePath + "\n"
                            "Date  : " + QDate::currentDate().toString("dd/MM/yyyy"));
    loadProductionHistory();
}

// ============================================================================
// GENERATE PROFESSIONAL PDF
// ============================================================================

void Production::generatePdf(const QString &filePath,
                             const QString &whereClause,
                             const QString &filterText)
{
    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(filePath);
    printer.setPageSize(QPageSize(QPageSize::A4));
    printer.setPageOrientation(QPageLayout::Portrait);
    printer.setPageMargins(QMarginsF(15, 15, 15, 15), QPageLayout::Millimeter);

    QPainter p;
    if (!p.begin(&printer)) {
        showErrorNotification("Impossible d'ouvrir le fichier PDF pour écriture.");
        return;
    }

    // ── Coordinates & helpers ────────────────────────────────────────────────
    QRectF page = QRectF(p.viewport());  // full page in device pixels
    const qreal W = page.width();
    const qreal H = page.height();

    auto scale = [&](qreal mm) -> qreal {
        return mm * printer.logicalDpiX() / 25.4;
    };

    // DPI-aware sizes
    qreal lm = scale(8);          // left margin
    qreal rm = W - scale(8);      // right limit
    qreal usableW = rm - lm;
    qreal y = scale(8);           // current Y cursor

    // ── Colours ──────────────────────────────────────────────────────────────
    QColor darkGreen(27, 67, 50);
    QColor medGreen(35, 78, 62);
    QColor accentGold(212, 172, 13);
    QColor lightGreen(220, 252, 231);
    QColor grayText(107, 114, 128);
    QColor rowAlt(249, 250, 251);
    QColor borderColor(229, 231, 235);

    // ── HEADER BANNER ────────────────────────────────────────────────────────
    qreal hdrH = scale(28);
    QLinearGradient hdrGrad(lm, y, rm, y);
    hdrGrad.setColorAt(0, darkGreen);
    hdrGrad.setColorAt(1, medGreen);
    p.setBrush(hdrGrad);
    p.setPen(Qt::NoPen);
    p.drawRoundedRect(QRectF(lm, y, usableW, hdrH), scale(3), scale(3));

    // Gold accent stripe
    p.setBrush(accentGold);
    p.drawRect(QRectF(lm, y + hdrH - scale(1.5), usableW, scale(1.5)));

    // Title text
    QFont titleFont("Arial", 0, QFont::Bold);
    titleFont.setPixelSize(static_cast<int>(scale(8)));
    p.setFont(titleFont);
    p.setPen(Qt::white);
    p.drawText(QRectF(lm + scale(6), y, usableW * 0.65, hdrH),
               Qt::AlignVCenter | Qt::AlignLeft, "PressIQ — Rapport de Production");

    // Subtitle
    QFont subFont("Arial");
    subFont.setPixelSize(static_cast<int>(scale(4)));
    p.setFont(subFont);
    p.setPen(QColor(134, 239, 172));
    p.drawText(QRectF(lm + scale(6), y + scale(11), usableW * 0.65, hdrH * 0.5),
               Qt::AlignTop | Qt::AlignLeft, "MODULE DE PRODUCTION INTELLIGENT");

    if (!filterText.trimmed().isEmpty()) {
        QFont filtFont("Arial");
        filtFont.setPixelSize(static_cast<int>(scale(3.2)));
        p.setFont(filtFont);
        p.setPen(QColor(191, 219, 254));
        p.drawText(QRectF(lm + scale(6), y + scale(16), usableW * 0.70, hdrH * 0.5),
                   Qt::AlignTop | Qt::AlignLeft, filterText);
    }

    // Date badge (top right)
    QString reportDate = "Généré le " + QDate::currentDate().toString("dd/MM/yyyy");
    QRectF dateBadge(rm - scale(46), y + scale(7), scale(44), scale(12));
    p.setBrush(accentGold);
    p.setPen(Qt::NoPen);
    p.drawRoundedRect(dateBadge, scale(2), scale(2));
    QFont dateFont("Arial", 0, QFont::Bold);
    dateFont.setPixelSize(static_cast<int>(scale(4)));
    p.setFont(dateFont);
    p.setPen(darkGreen);
    p.drawText(dateBadge, Qt::AlignCenter, reportDate);

    y += hdrH + scale(8);

    // ── INFO ROW ─────────────────────────────────────────────────────────────
    QFont labelFont("Arial");
    labelFont.setPixelSize(static_cast<int>(scale(3.5)));
    QFont valueFont("Arial", 0, QFont::Bold);
    valueFont.setPixelSize(static_cast<int>(scale(3.5)));

    // Fetch totals from DB
    QSqlQuery totals(QSqlDatabase::database("production_conn"));
    totals.exec(
        "SELECT COUNT(*), SUM(QUANTITEOLIVESKG), SUM(HUILEPRODUITEL), AVG(RENDEMENT) "
        "FROM PRODUCTION" + whereClause
    );
    int    totalProds  = 0;
    double totalOlives = 0, totalHuile = 0, avgRend = 0;
    if (totals.next()) {
        totalProds  = totals.value(0).toInt();
        totalOlives = totals.value(1).toDouble();
        totalHuile  = totals.value(2).toDouble();
        avgRend     = totals.value(3).toDouble();
    }

    // Draw 4 KPI cards
    struct KpiCard { QString icon; QString value; QString label; QColor bg; };
    QList<KpiCard> kpis = {
        {"🌿", QString::number(totalProds), "Productions totales", QColor(239,246,255)},
        {"⚖",  QString::number(totalOlives,'f',0) + " kg", "Olives traitées", lightGreen},
        {"💧", QString::number(totalHuile,'f',1) + " L",   "Huile produite",  QColor(254,249,195)},
        {"📊", QString::number(avgRend,'f',2) + " %",       "Rendement moyen", QColor(254,226,226)}
    };

    qreal cardW    = (usableW - scale(3) * 3) / 4.0;
    qreal cardH    = scale(20);
    qreal cardX    = lm;

    for (const auto &kpi : kpis) {
        // Card background
        p.setBrush(kpi.bg);
        p.setPen(borderColor);
        p.drawRoundedRect(QRectF(cardX, y, cardW, cardH), scale(2.5), scale(2.5));

        // Icon
        QFont iconFont("Segoe UI Emoji");
        iconFont.setPixelSize(static_cast<int>(scale(7)));
        p.setFont(iconFont);
        p.setPen(darkGreen);
        p.drawText(QRectF(cardX, y + scale(2), cardW, scale(9)),
                   Qt::AlignTop | Qt::AlignHCenter, kpi.icon);

        // Value
        QFont kpiValFont("Arial", 0, QFont::Bold);
        kpiValFont.setPixelSize(static_cast<int>(scale(5)));
        p.setFont(kpiValFont);
        p.setPen(darkGreen);
        p.drawText(QRectF(cardX, y + scale(11), cardW, scale(6)),
                   Qt::AlignTop | Qt::AlignHCenter, kpi.value);

        // Label
        QFont kpiLblFont("Arial");
        kpiLblFont.setPixelSize(static_cast<int>(scale(3)));
        p.setFont(kpiLblFont);
        p.setPen(grayText);
        p.drawText(QRectF(cardX, y + scale(16), cardW, scale(5)),
                   Qt::AlignTop | Qt::AlignHCenter, kpi.label);

        cardX += cardW + scale(3);
    }

    y += cardH + scale(8);

    // ── SECTION TITLE: Historique ─────────────────────────────────────────────
    QFont secFont("Arial", 0, QFont::Bold);
    secFont.setPixelSize(static_cast<int>(scale(5)));
    p.setFont(secFont);
    p.setPen(darkGreen);
    p.drawText(QRectF(lm, y, usableW, scale(7)), Qt::AlignVCenter | Qt::AlignLeft,
               "Historique des Productions");

    // Gold underline
    p.setBrush(accentGold);
    p.setPen(Qt::NoPen);
    p.drawRect(QRectF(lm, y + scale(6.5), scale(40), scale(1)));

    y += scale(9);

    // ── TABLE HEADER ─────────────────────────────────────────────────────────
    QList<QString> headers = {"ID", "Date", "Olives (kg)", "Huile (L)", "Rendement", "Statut"};
    QList<qreal>   colFrac = {0.05, 0.13, 0.14, 0.13, 0.13, 0.15};  // fractions of usableW

    // Normalise so they sum to <=1 using remaining proportion
    QList<qreal> colW;
    for (qreal f : colFrac) colW << f * usableW;

    qreal rowH = scale(8);

    // Header row background
    p.setBrush(darkGreen);
    p.setPen(Qt::NoPen);
    p.drawRoundedRect(QRectF(lm, y, usableW, rowH), scale(1.5), scale(1.5));

    QFont tblHdrFont("Arial", 0, QFont::Bold);
    tblHdrFont.setPixelSize(static_cast<int>(scale(3.5)));
    p.setFont(tblHdrFont);
    p.setPen(Qt::white);

    qreal cx = lm;
    for (int i = 0; i < headers.size(); i++) {
        p.drawText(QRectF(cx + scale(1), y, colW[i], rowH),
                   Qt::AlignVCenter | Qt::AlignLeft, headers[i]);
        cx += colW[i];
    }
    y += rowH;

    // ── TABLE ROWS ────────────────────────────────────────────────────────────
    QSqlQuery hist(QSqlDatabase::database("production_conn"));
    hist.exec(
        "SELECT IDPRODUCTION, DATEPRODUCTION, QUANTITEOLIVESKG, HUILEPRODUITEL, RENDEMENT, STATUT "
        "FROM PRODUCTION" + whereClause + " ORDER BY IDPRODUCTION DESC"
    );

    QFont rowFont("Arial");
    rowFont.setPixelSize(static_cast<int>(scale(3.5)));
    p.setFont(rowFont);

    int rowIdx = 0;
    while (hist.next()) {
        // Alternate background
        if (rowIdx % 2 == 0) {
            p.setBrush(rowAlt);
        } else {
            p.setBrush(Qt::white);
        }
        p.setPen(Qt::NoPen);
        p.drawRect(QRectF(lm, y, usableW, rowH));

        // Bottom line
        p.setPen(QPen(borderColor, 0.5));
        p.drawLine(QPointF(lm, y + rowH), QPointF(rm, y + rowH));

        QString statut = hist.value(5).toString();
        cx = lm;

        QList<QString> cells = {
            hist.value(0).toString(),
            hist.value(1).toDate().toString("dd/MM/yyyy"),
            QString::number(hist.value(2).toInt()) + " kg",
            QString::number(hist.value(3).toDouble(), 'f', 2) + " L",
            QString::number(hist.value(4).toDouble(), 'f', 2) + " %",
            statut
        };

        for (int i = 0; i < cells.size(); i++) {
            // Statut pill
            if (i == 5) {
                QColor pillBg, pillFg;
                if (statut.contains("Termin", Qt::CaseInsensitive)) {
                    pillBg = QColor(220,252,231); pillFg = QColor(22,101,52);
                } else if (statut == "En cours") {
                    pillBg = QColor(219,234,254); pillFg = QColor(30,64,175);
                } else if (statut.contains("Planifi", Qt::CaseInsensitive)) {
                    pillBg = QColor(254,243,199); pillFg = QColor(146,64,14);
                } else {
                    pillBg = QColor(254,226,226); pillFg = QColor(185,28,28);
                }
                qreal pillH = scale(5);
                qreal pillY = y + (rowH - pillH) / 2.0;
                p.setBrush(pillBg);
                p.setPen(Qt::NoPen);
                QFontMetrics fm(rowFont);
                qreal pillW = qMin(fm.horizontalAdvance(statut) + scale(5),
                                   (double)colW[i] - scale(2));
                p.drawRoundedRect(QRectF(cx + scale(1), pillY, pillW, pillH),
                                  scale(1.5), scale(1.5));
                p.setPen(pillFg);
                p.setFont(rowFont);
                p.drawText(QRectF(cx + scale(1), pillY, pillW, pillH),
                           Qt::AlignCenter, statut);
            } else {
                p.setPen(QColor(31, 41, 55));
                p.setFont(rowFont);
                p.drawText(QRectF(cx + scale(1), y, colW[i] - scale(2), rowH),
                           Qt::AlignVCenter | Qt::AlignLeft, cells[i]);
            }
            cx += colW[i];
        }

        y += rowH;
        rowIdx++;

        // Page break
        if (y + rowH > H - scale(20)) {
            printer.newPage();
            y = scale(8);
        }
    }

    // ── FOOTER ───────────────────────────────────────────────────────────────
    y = H - scale(14);
    p.setBrush(Qt::NoBrush);
    p.setPen(QPen(borderColor, 0.7));
    p.drawLine(QPointF(lm, y), QPointF(rm, y));

    QFont footerFont("Arial");
    footerFont.setPixelSize(static_cast<int>(scale(3)));
    p.setFont(footerFont);
    p.setPen(grayText);
    p.drawText(QRectF(lm, y + scale(2), usableW / 2, scale(8)),
               Qt::AlignVCenter | Qt::AlignLeft,
               "PressIQ — Gestion de la Production  |  Module Intelligent");
    p.drawText(QRectF(lm, y + scale(2), usableW, scale(8)),
               Qt::AlignVCenter | Qt::AlignRight,
               "Rapport généré automatiquement  •  " +
               QDateTime::currentDateTime().toString("dd/MM/yyyy HH:mm"));

    p.end();
}

// ============================================================================
// LOAD PRODUCTION HISTORY
// ============================================================================

void Production::loadProductionHistory()
{
    QSqlQuery query(QSqlDatabase::database("production_conn"));
    query.exec("SELECT IDPRODUCTION, DATEPRODUCTION, QUANTITEOLIVESKG, HUILEPRODUITEL, RENDEMENT, STATUT "
               "FROM PRODUCTION ORDER BY IDPRODUCTION DESC");
    
    // Clear existing rows
    ui->tableProductionHistory->setRowCount(0);
    
    // Hide vertical header (row numbers)
    ui->tableProductionHistory->verticalHeader()->setVisible(false);
    
    int row = 0;
    while (query.next()) {
        ui->tableProductionHistory->insertRow(row);
        
        // ID
        QTableWidgetItem* idItem = new QTableWidgetItem(query.value(0).toString());
        idItem->setTextAlignment(Qt::AlignCenter);
        idItem->setForeground(QColor("#1f2937"));
        ui->tableProductionHistory->setItem(row, 0, idItem);
        
        // Date
        QTableWidgetItem* dateItem = new QTableWidgetItem(query.value(1).toDate().toString("dd/MM/yyyy"));
        dateItem->setTextAlignment(Qt::AlignCenter);
        dateItem->setForeground(QColor("#1f2937"));
        ui->tableProductionHistory->setItem(row, 1, dateItem);
        
        // Olives (kg)
        QTableWidgetItem* olivesItem = new QTableWidgetItem(QString::number(query.value(2).toInt()) + " kg");
        olivesItem->setTextAlignment(Qt::AlignCenter);
        olivesItem->setForeground(QColor("#1f2937"));
        ui->tableProductionHistory->setItem(row, 2, olivesItem);
        
        // Huile (L)
        QTableWidgetItem* huileItem = new QTableWidgetItem(QString::number(query.value(3).toDouble(), 'f', 2) + " L");
        huileItem->setTextAlignment(Qt::AlignCenter);
        huileItem->setForeground(QColor("#1f2937"));
        ui->tableProductionHistory->setItem(row, 3, huileItem);
        
        // Rendement
        QTableWidgetItem* rendementItem = new QTableWidgetItem(QString::number(query.value(4).toDouble(), 'f', 2) + " %");
        rendementItem->setTextAlignment(Qt::AlignCenter);
        rendementItem->setForeground(QColor("#1f2937"));
        ui->tableProductionHistory->setItem(row, 4, rendementItem);
        
        // Statut with color badge
        QString statut = query.value(5).toString();
        QTableWidgetItem* statutItem = new QTableWidgetItem(statut);
        statutItem->setTextAlignment(Qt::AlignCenter);
        
        // Color code statut - background and text
        if (statut == "Termine" || statut.contains("Termin")) {
            statutItem->setBackground(QColor("#DCFCE7"));
            statutItem->setForeground(QColor("#166534"));
        } else if (statut == "En cours") {
            statutItem->setBackground(QColor("#DBEAFE"));
            statutItem->setForeground(QColor("#1E40AF"));
        } else if (statut == "Planifie" || statut.contains("Planifi")) {
            statutItem->setBackground(QColor("#FEF3C7"));
            statutItem->setForeground(QColor("#92400E"));
        } else {
            statutItem->setForeground(QColor("#1f2937"));
        }
        ui->tableProductionHistory->setItem(row, 5, statutItem);
        
        row++;
    }
    
    // Fixed column widths — prevent shrinking on reload
    QHeaderView *hdr = ui->tableProductionHistory->horizontalHeader();
    hdr->setSectionResizeMode(0, QHeaderView::Fixed);        // ID
    hdr->setSectionResizeMode(1, QHeaderView::Fixed);        // Date
    hdr->setSectionResizeMode(2, QHeaderView::Fixed);        // Olives
    hdr->setSectionResizeMode(3, QHeaderView::Fixed);        // Huile
    hdr->setSectionResizeMode(4, QHeaderView::Fixed);        // Rendement
    hdr->setSectionResizeMode(5, QHeaderView::Stretch);      // Statut — always fills remaining space

    ui->tableProductionHistory->setColumnWidth(0, 55);   // ID
    ui->tableProductionHistory->setColumnWidth(1, 100);  // Date
    ui->tableProductionHistory->setColumnWidth(2, 110);  // Olives
    ui->tableProductionHistory->setColumnWidth(3, 100);  // Huile
    ui->tableProductionHistory->setColumnWidth(4, 110);  // Rendement
    // Column 5 (Statut) stretches automatically

    qDebug() << "Loaded" << row << "productions into history table";
}

// ============================================================================
// CRUD: DELETE SELECTED PRODUCTION
// ============================================================================

void Production::onDeleteClicked()
{
    // Get selected row
    int selectedRow = ui->tableProductionHistory->currentRow();
    if (selectedRow < 0) {
        showErrorNotification("Sélectionnez une production dans le tableau!");
        return;
    }
    
    // Get ID from first column
    QTableWidgetItem* idItem = ui->tableProductionHistory->item(selectedRow, 0);
    if (!idItem) {
        showErrorNotification("Erreur: Impossible de récupérer l'ID!");
        return;
    }
    
    int productionId = idItem->text().toInt();
    
    QMessageBox::StandardButton reply = QMessageBox::question(this, 
        "Confirmer la suppression",
        "Êtes-vous sûr de vouloir supprimer la production ID: " + QString::number(productionId) + "?",
        QMessageBox::Yes | QMessageBox::No);
    
    if (reply == QMessageBox::Yes) {
        if (ProductionModel::deleteProduction(productionId)) {
            showSuccessNotification("Production supprimée!\n\nID: " + QString::number(productionId));
            loadProductionHistory(); // Refresh table
        } else {
            showErrorNotification("Échec de la suppression!");
        }
    }
}

// ============================================================================
// CRUD: MODIFY SELECTED PRODUCTION  →  popup dialog
// ============================================================================

void Production::onModifyClicked()
{
    // ── 1. Need a selected row ────────────────────────────────────────────────
    int selectedRow = ui->tableProductionHistory->currentRow();
    if (selectedRow < 0) {
        showErrorNotification("Sélectionnez une production dans le tableau!");
        return;
    }

    int productionId = ui->tableProductionHistory->item(selectedRow, 0)->text().toInt();

    // ── 2. Load full row from database ───────────────────────────────────────
    QSqlQuery q(QSqlDatabase::database("production_conn"));
    q.prepare("SELECT DATEPRODUCTION, QUANTITEOLIVESKG, HUILEPRODUITEL, RENDEMENT, "
              "STATUT, TYPEPRESSAGE, QUALITE, NOTESPLANIFICATION "
              "FROM PRODUCTION WHERE IDPRODUCTION = :id");
    q.bindValue(":id", productionId);
    if (!q.exec() || !q.next()) {
        showErrorNotification("Impossible de charger la production ID " +
                              QString::number(productionId));
        return;
    }

    // ── 3. Build dialog ──────────────────────────────────────────────────────
    ProductionEditDialog dlg(this);
    dlg.setProductionId(productionId);
    dlg.setDate(q.value(0).toDate());
    dlg.setOlivesKg(q.value(1).toInt());
    dlg.setHuileL(q.value(2).toDouble());
    dlg.setRendement(q.value(3).toDouble());
    dlg.setStatut(q.value(4).toString());
    dlg.setTypePressage(q.value(5).toString());
    dlg.setQualite(q.value(6).toString());
    dlg.setNotes(q.value(7).toString());

    // Center dialog over main window
    dlg.adjustSize();
    QPoint center = this->geometry().center();
    dlg.move(center.x() - dlg.width() / 2, center.y() - dlg.height() / 2);

    // ── 4. Execute ───────────────────────────────────────────────────────────
    if (dlg.exec() != QDialog::Accepted)
        return;

    // ── 5. Build UPDATE query with new values ────────────────────────────────
    double newOlives = dlg.getOlivesKg();
    double newHuile  = dlg.getHuileL();
    double newRend   = (newOlives > 0) ? (newHuile / newOlives * 100.0) : 0.0;

    // Strip non-ASCII chars (emojis) from type pressage for Oracle ODBC
    QString cleanType = dlg.getTypePressage();
    cleanType.remove(QRegularExpression("[^a-zA-Z0-9\\s\u00C0-\u017E]"));
    cleanType = cleanType.trimmed();
    if (cleanType.isEmpty()) cleanType = "Standard";

    QSqlQuery upd(QSqlDatabase::database("production_conn"));
    upd.prepare(
        "UPDATE PRODUCTION SET "
        "  DATEPRODUCTION     = TO_DATE(:date, 'YYYY-MM-DD'), "
        "  QUANTITEOLIVESKG   = :olives, "
        "  HUILEPRODUITEL     = :huile, "
        "  RENDEMENT          = :rend, "
        "  STATUT             = :statut, "
        "  TYPEPRESSAGE       = :type, "
        "  QUALITE            = :qualite, "
        "  NOTESPLANIFICATION = :notes "
        "WHERE IDPRODUCTION   = :id"
    );
    upd.bindValue(":date",   dlg.getDate().toString("yyyy-MM-dd"));
    upd.bindValue(":olives", dlg.getOlivesKg());
    upd.bindValue(":huile",  dlg.getHuileL());
    upd.bindValue(":rend",   newRend);
    upd.bindValue(":statut", dlg.getStatut());
    upd.bindValue(":type",   cleanType);
    QString qual = dlg.getQualite();
    upd.bindValue(":qualite", qual.isEmpty() ? QVariant() : QVariant(qual));
    upd.bindValue(":notes",  dlg.getNotes());
    upd.bindValue(":id",     productionId);

    if (upd.exec()) {
        showSuccessNotification(
            "✔  Production modifiée avec succès!\n\n"
            "ID : " + QString::number(productionId) + "\n"
            "Olives : " + QString::number(dlg.getOlivesKg()) + " kg\n"
            "Huile  : " + QString::number(dlg.getHuileL(), 'f', 2) + " L\n"
            "Rendement : " + QString::number(newRend, 'f', 2) + " %"
        );
        loadProductionHistory();
    } else {
        showErrorNotification("Échec de la mise à jour!\n\n" + upd.lastError().text());
    }
}

// ============================================================================
// REFRESH HISTORY
// ============================================================================

void Production::onRefreshHistoryClicked()
{
    loadProductionHistory();
    showSuccessNotification("Historique actualisé!");
}

void Production::refreshTable()
{
    loadProductionHistory();
}

void Production::clearForm()
{
    ui->spinOlivesKg->setValue(500);
    ui->spinOilProduced->setValue(0);
    ui->txtNotesPlanification->clear();
    ui->comboPressType->setCurrentIndex(0);
    currentProductionId = 0;
}
