#include "stock.h"
#include "ui_stock.h"
#include <QCoreApplication>
#include <QDate>
#include <QPixmap>
#include <QTime>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QTextDocument>
#include <QFileDialog>
#include <QPdfWriter>
#include <QAbstractSpinBox>
#include <QHeaderView>
#include <QScrollArea>
#include <QScrollBar>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QBoxLayout>
#include <QSplitter>
#include <QTabWidget>
#include <QFrame>
#include <QTextCharFormat>
#include <QBrush>
#include <QGraphicsOpacityEffect>
#include <QGraphicsDropShadowEffect>
#include <QPropertyAnimation>
#include <QEasingCurve>
#include <QDateTime>
#include <QPainter>
#include <QSignalBlocker>
#include <QStringList>
#include <QMetaType>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QCategoryAxis>
#include <QtCharts/QLineSeries>
#include <QtCharts/QLegend>
#include <QtCharts/QPieSeries>
#include <QtCharts/QValueAxis>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <algorithm>

Stock::Stock(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Stock)
    , dateTimeTimer(nullptr)
    , calendarRefreshTimer(nullptr)
    , historiqueRefreshTimer(nullptr)
    , connexion(nullptr)
    , m_serial(nullptr)
    , m_serialBuffer()
    , m_currentWeightL(0.0)
    , m_currentStatusRaw()
    , m_currentStockId(0)
    , m_hasStockId(false)
    , m_freezeValue(false)
    , m_lastValidationMs(0)
    , m_comboPorts(nullptr)
    , m_btnConnectArduino(nullptr)
    , m_btnRefreshPorts(nullptr)
    , m_lblPoidsTitre(nullptr)
    , m_lblPoidsValeur(nullptr)
    , m_lblStockIdValeur(nullptr)
    , m_progressArduino(nullptr)
    , m_lblStatusArduino(nullptr)
    , m_spinManualWeight(nullptr)
    , m_btnPlusOneL(nullptr)
    , m_btnConfirmWeight(nullptr)
    , m_btnValiderPoids(nullptr)
    , m_btnToggleFullScreen(nullptr)
{
    ui->setupUi(this);
    {
        QPixmap logo;
        for (const QString &p : QStringList{":/logo.png", ":/images/logo.png", "logo.png",
             "../production/logo.png", "../stock/images/logo.png",
             QCoreApplication::applicationDirPath() + "/../../logo.png",
             QCoreApplication::applicationDirPath() + "/../../../production/logo.png"}) {
            logo = QPixmap(p);
            if (!logo.isNull()) break;
        }
        if (!logo.isNull())
            ui->labelLogoImage->setPixmap(logo.scaledToHeight(50, Qt::SmoothTransformation));
    }

    // Obtenir la connexion
    connexion = Connexion::getInstance();
    
    // Vérifier la connexion
    if (!connexion->isOpen()) {
        connexion->createConnection();
    }

    ensureProcurementSchema();
    
    m_serial = new QSerialPort(this);
    m_serial->setBaudRate(QSerialPort::Baud9600);
    connect(m_serial, &QSerialPort::readyRead, this, &Stock::onSerialReadyRead);

    // Initialiser l'interface
    initialiserInterface();
    setupArduinoTabUi();
    refreshAvailablePorts();
    
    // Activer le scrolling sur tous les onglets
    activerScrolling();
    
    // Charger les données
    chargerDonneesTable();
    
    // Connect back button
    connect(ui->btnBackToMenu, &QPushButton::clicked, this, &Stock::onBackClicked);

    if (ui->btnBackToMenu && ui->btnBackToMenu->parentWidget()) {
        QBoxLayout *topButtonsLayout = qobject_cast<QBoxLayout*>(ui->btnBackToMenu->parentWidget()->layout());
        if (topButtonsLayout) {
            m_btnToggleFullScreen = new QPushButton("Plein ecran", this);
            m_btnToggleFullScreen->setObjectName("btnToggleFullScreen");
            m_btnToggleFullScreen->setToolTip("Basculer en plein ecran / fenetre normale");

            const int backButtonIndex = topButtonsLayout->indexOf(ui->btnBackToMenu);
            if (backButtonIndex >= 0) {
                topButtonsLayout->insertWidget(backButtonIndex + 1, m_btnToggleFullScreen);
            } else {
                topButtonsLayout->addWidget(m_btnToggleFullScreen);
            }

            connect(m_btnToggleFullScreen, &QPushButton::clicked, this, &Stock::onToggleFullScreen);
        }
    }
    
    // Connect CRUD buttons
    connect(ui->btnAjouter, &QPushButton::clicked, this, &Stock::onAjouterClicked);
    connect(ui->btnModifier, &QPushButton::clicked, this, &Stock::onModifierClicked);
    connect(ui->btnSupprimer, &QPushButton::clicked, this, &Stock::onSupprimerClicked);
    connect(ui->btnConsulter, &QPushButton::clicked, this, &Stock::onConsulterClicked);
    
    // Connect recherche buttons
    connect(ui->btnRechercher, &QPushButton::clicked, this, &Stock::onRechercherClicked);
    connect(ui->btnReinitialiser, &QPushButton::clicked, this, &Stock::onReinitialiserClicked);
    
    // Connect tri button
    connect(ui->btnAppliquerTri, &QPushButton::clicked, this, &Stock::onAppliquerTriClicked);
    
    // Connect export button
    connect(ui->btnExporter, &QPushButton::clicked, this, &Stock::onExporterClicked);
    
    // Connect statistiques button
    connect(ui->btnCalculer, &QPushButton::clicked, this, &Stock::onCalculerClicked);
    
    // Connect alertes buttons
    connect(ui->btnVerifierAlertes, &QPushButton::clicked, this, &Stock::onVerifierAlertesClicked);
    connect(ui->btnAfficherHistorique, &QPushButton::clicked, this, &Stock::onAfficherHistoriqueClicked);
    connect(ui->btnAnalyserTendances, &QPushButton::clicked, this, &Stock::onAnalyserTendancesClicked);
    connect(ui->btnAnalyserPredictifHuile, &QPushButton::clicked, this, &Stock::onAnalyserPredictifHuileClicked);
    
    // Connect rapport buttons
    connect(ui->btnGenererRapport, &QPushButton::clicked, this, &Stock::onGenererRapportClicked);
    connect(ui->btnExportRapportPDF, &QPushButton::clicked, this, &Stock::onExportRapportPDFClicked);
    connect(ui->btnAjouterFournisseur, &QPushButton::clicked, this, &Stock::onAjouterFournisseurClicked);
    connect(ui->btnLierHuileFournisseur, &QPushButton::clicked, this, &Stock::onLierHuileFournisseurClicked);
    connect(ui->btnGenererCommandeAlerte, &QPushButton::clicked, this, &Stock::onGenererCommandeAlerteClicked);
    
    // Connect synchronisation button
    connect(ui->btnSynchroniser, &QPushButton::clicked, this, &Stock::onSynchroniserClicked);

    // Connect calendrier interactif
    connect(ui->calendarWidgetStock, &QCalendarWidget::selectionChanged, this, [this]() {
        onCalendarDateSelected(ui->calendarWidgetStock->selectedDate());
    });
    connect(ui->calendarWidgetStock, &QCalendarWidget::currentPageChanged,
            this, &Stock::onCalendarShowMonth);
    connect(ui->btnTodayCalendar, &QPushButton::clicked, this, [this]() {
        const QDate today = QDate::currentDate();
        ui->calendarWidgetStock->setSelectedDate(today);
        ui->calendarWidgetStock->showSelectedDate();
        onCalendarDateSelected(today);
    });
    
    // Connect table selection
    connect(ui->tableStocks, &QTableWidget::itemSelectionChanged, this, &Stock::onTableStockSelectionChanged);
    
    // Setup date/time timer
    dateTimeTimer = new QTimer(this);
    connect(dateTimeTimer, &QTimer::timeout, this, &Stock::updateDateTime);
    dateTimeTimer->start(1000);
    updateDateTime();

    // Actualisation automatique du calendrier
    calendarRefreshTimer = new QTimer(this);
    connect(calendarRefreshTimer, &QTimer::timeout, this, &Stock::chargerDatesStockCalendrier);
    calendarRefreshTimer->start(30000);

    historiqueRefreshTimer = new QTimer(this);
    connect(historiqueRefreshTimer, &QTimer::timeout, this, [this]() {
        refreshHistoriqueEtatTemps(false);
    });
    historiqueRefreshTimer->start(5000);

    initialiserCalendrier();
    refreshHistoriqueEtatTemps(false);
    refreshProcurementUi();
}

Stock::~Stock()
{
    if (m_serial && m_serial->isOpen()) {
        m_serial->close();
    }
    if (dateTimeTimer) {
        dateTimeTimer->stop();
        delete dateTimeTimer;
    }
    if (calendarRefreshTimer) {
        calendarRefreshTimer->stop();
        delete calendarRefreshTimer;
    }
    if (historiqueRefreshTimer) {
        historiqueRefreshTimer->stop();
        delete historiqueRefreshTimer;
    }
    delete ui;
}

void Stock::initialiserInterface()
{
    // Configurer la date actuelle
    ui->dateEditMaj->setDate(QDate::currentDate());
    
    // Remplir les combobox
    ui->comboTypeHuile->clear();
    ui->comboTypeHuile->addItem("Huile d'olive extra vierge");
    ui->comboTypeHuile->addItem("Huile d'olive vierge");
    ui->comboTypeHuile->addItem("Huile d'arachide");
    ui->comboTypeHuile->addItem("Huile de tournesol");
    ui->comboTypeHuile->addItem("Huile de colza");
    ui->comboTypeHuile->addItem("Huile de sésame");
    ui->comboTypeHuile->addItem("Huile de noix");
    ui->comboTypeHuile->addItem("Huile de mais");

    if (ui->comboHuileFournisseur) {
        ui->comboHuileFournisseur->clear();
        for (int i = 0; i < ui->comboTypeHuile->count(); ++i) {
            ui->comboHuileFournisseur->addItem(ui->comboTypeHuile->itemText(i));
        }
    }
    
    ui->comboEtatStock->clear();
    ui->comboEtatStock->addItem("Disponible");
    ui->comboEtatStock->addItem("Faible");
    ui->comboEtatStock->addItem("Rupture");
    ui->comboEtatStock->addItem("En commande");
    
    // Combobox de recherche
    ui->comboRechercheType->clear();
    ui->comboRechercheType->addItem("Tous");
    ui->comboRechercheType->addItems({"Huile d'olive extra vierge", "Huile d'olive vierge", 
                                       "Huile d'arachide", "Huile de tournesol", 
                                       "Huile de colza", "Huile de sésame",
                                       "Huile de noix", "Huile de mais"});
    
    ui->comboRechercheEtat->clear();
    ui->comboRechercheEtat->addItem("Tous");
    ui->comboRechercheEtat->addItems({"Disponible", "Faible", "Rupture", "En commande"});
    
    // Combobox de tri
    ui->comboTriCritere->clear();
    ui->comboTriCritere->addItem("ID Stock");
    ui->comboTriCritere->addItem("Type d'huile");
    ui->comboTriCritere->addItem("Quantité actuelle");
    ui->comboTriCritere->addItem("Seuil d'alerte");
    ui->comboTriCritere->addItem("Date de mise à jour");
    
    // Combobox de statistiques
    ui->comboStatsPar->clear();
    ui->comboStatsPar->addItem("Type d'huile");
    ui->comboStatsPar->addItem("État du stock");
    ui->comboStatsPar->addItem("Emplacement");

    if (ui->comboTypeGraphique) {
        ui->comboTypeGraphique->clear();
        ui->comboTypeGraphique->addItem("Barres");
        ui->comboTypeGraphique->addItem("Courbe");
        ui->comboTypeGraphique->addItem("Histogramme");
        ui->comboTypeGraphique->addItem("Camembert");
    }
    
    // Combobox de période
    ui->comboPeriode->clear();
    ui->comboPeriode->addItem("7 derniers jours");
    ui->comboPeriode->addItem("30 derniers jours");
    ui->comboPeriode->addItem("90 derniers jours");

    if (ui->comboPeriodePredictifHuile) {
        ui->comboPeriodePredictifHuile->clear();
        ui->comboPeriodePredictifHuile->addItem("7 derniers jours");
        ui->comboPeriodePredictifHuile->addItem("30 derniers jours");
        ui->comboPeriodePredictifHuile->addItem("90 derniers jours");
    }
    
    // Configurer la table
    configurerTableStocks();
    
    // Configuration des spin boxes
    ui->spinQuantite->setRange(0, 100000.00);
    ui->spinQuantite->setDecimals(2);
    ui->spinQuantite->setSingleStep(1.0);
    ui->spinQuantite->setValue(0);
    
    ui->spinSeuilAlerte->setRange(0, 10000);
    ui->spinSeuilAlerte->setSingleStep(1);
    ui->spinSeuilAlerte->setValue(50);
    
    // Activer les boutons des spinbox
    ui->spinQuantite->setButtonSymbols(QAbstractSpinBox::UpDownArrows);
    ui->spinSeuilAlerte->setButtonSymbols(QAbstractSpinBox::UpDownArrows);
    ui->spinQuantite->setEnabled(true);
    ui->spinSeuilAlerte->setEnabled(true);
    
    // Connecter les changements de valeur pour mettre à jour l'état automatiquement
    connect(ui->spinQuantite, QOverload<double>::of(&QDoubleSpinBox::valueChanged), 
            [this]() {
        double quantite = ui->spinQuantite->value();
        double seuil = ui->spinSeuilAlerte->value();
        QString etat = getEtatStock(quantite, seuil);
        ui->comboEtatStock->setCurrentText(etat);
    });
    
    connect(ui->spinSeuilAlerte, QOverload<int>::of(&QSpinBox::valueChanged), 
            [this]() {
        double quantite = ui->spinQuantite->value();
        double seuil = ui->spinSeuilAlerte->value();
        QString etat = getEtatStock(quantite, seuil);
        ui->comboEtatStock->setCurrentText(etat);
    });
    
    // ID en lecture seule
    ui->lineEditIdStock->setReadOnly(true);
    ui->lineEditIdStock->setPlaceholderText("Auto-généré");

    // Rehausser visuellement les onglets Gestion/Tri pour une interface plus moderne
    const QString stockModernStyle =
        "QWidget#tabGestionStock, QWidget#tabTriRechercheExport {"
        "  background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #f7fbf8, stop:1 #edf6f0);"
        "}"
        "QWidget#tabGestionStock QGroupBox, QWidget#tabTriRechercheExport QGroupBox {"
        "  background: #ffffff;"
        "  border: 1px solid #d7e8db;"
        "  border-radius: 14px;"
        "  margin-top: 14px;"
        "  padding-top: 16px;"
        "  font-weight: 700;"
        "  color: #163a2d;"
        "}"
        "QWidget#tabGestionStock QGroupBox::title, QWidget#tabTriRechercheExport QGroupBox::title {"
        "  subcontrol-origin: margin;"
        "  left: 14px;"
        "  padding: 0 8px;"
        "  color: #14532d;"
        "}"
        "QWidget#tabGestionStock QLabel, QWidget#tabTriRechercheExport QLabel {"
        "  color: #1e3a32;"
        "  font-size: 15px;"
        "  font-weight: 600;"
        "}"
        "QWidget#tabGestionStock QLineEdit, QWidget#tabGestionStock QComboBox, QWidget#tabGestionStock QDateEdit,"
        "QWidget#tabGestionStock QSpinBox, QWidget#tabGestionStock QDoubleSpinBox,"
        "QWidget#tabTriRechercheExport QLineEdit, QWidget#tabTriRechercheExport QComboBox,"
        "QWidget#tabTriRechercheExport QDateEdit, QWidget#tabTriRechercheExport QSpinBox, QWidget#tabTriRechercheExport QDoubleSpinBox {"
        "  background: #fbfefc;"
        "  border: 1px solid #c9ddd0;"
        "  border-radius: 10px;"
        "  min-height: 38px;"
        "  padding: 6px 12px;"
        "  color: #102a22;"
        "  selection-background-color: #1f6f55;"
        "}"
        "QWidget#tabGestionStock QLineEdit:focus, QWidget#tabGestionStock QComboBox:focus, QWidget#tabGestionStock QDateEdit:focus,"
        "QWidget#tabGestionStock QSpinBox:focus, QWidget#tabGestionStock QDoubleSpinBox:focus,"
        "QWidget#tabTriRechercheExport QLineEdit:focus, QWidget#tabTriRechercheExport QComboBox:focus,"
        "QWidget#tabTriRechercheExport QDateEdit:focus, QWidget#tabTriRechercheExport QSpinBox:focus, QWidget#tabTriRechercheExport QDoubleSpinBox:focus {"
        "  border: 2px solid #1f6f55;"
        "  background: #ffffff;"
        "}"
        "QWidget#tabGestionStock QPushButton, QWidget#tabTriRechercheExport QPushButton {"
        "  border: none;"
        "  border-radius: 10px;"
        "  min-height: 40px;"
        "  padding: 8px 16px;"
        "  font-size: 15px;"
        "  font-weight: 700;"
        "}"
        "QWidget#tabGestionStock QPushButton#btnAjouter, QWidget#tabGestionStock QPushButton#btnModifier,"
        "QWidget#tabGestionStock QPushButton#btnConsulter, QWidget#tabTriRechercheExport QPushButton#btnRechercher,"
        "QWidget#tabTriRechercheExport QPushButton#btnAppliquerTri, QWidget#tabTriRechercheExport QPushButton#btnExporter {"
        "  background-color: #18543f;"
        "  color: #ffffff;"
        "}"
        "QWidget#tabGestionStock QPushButton#btnAjouter:hover, QWidget#tabGestionStock QPushButton#btnModifier:hover,"
        "QWidget#tabGestionStock QPushButton#btnConsulter:hover, QWidget#tabTriRechercheExport QPushButton#btnRechercher:hover,"
        "QWidget#tabTriRechercheExport QPushButton#btnAppliquerTri:hover, QWidget#tabTriRechercheExport QPushButton#btnExporter:hover {"
        "  background-color: #216b50;"
        "}"
        "QWidget#tabGestionStock QPushButton#btnReinitialiser, QWidget#tabTriRechercheExport QPushButton#btnReinitialiser {"
        "  background-color: #6b7280;"
        "  color: #ffffff;"
        "}"
        "QWidget#tabGestionStock QPushButton#btnReinitialiser:hover, QWidget#tabTriRechercheExport QPushButton#btnReinitialiser:hover {"
        "  background-color: #4b5563;"
        "}"
        "QWidget#tabGestionStock QPushButton#btnSupprimer {"
        "  background-color: #c62828;"
        "  color: #ffffff;"
        "}"
        "QWidget#tabGestionStock QPushButton#btnSupprimer:hover {"
        "  background-color: #a91f1f;"
        "}"
        "QWidget#tabGestionStock QTableWidget, QWidget#tabTriRechercheExport QTableWidget {"
        "  background: #ffffff;"
        "  alternate-background-color: #f3faf6;"
        "  border: 1px solid #d5e7dc;"
        "  border-radius: 12px;"
        "  gridline-color: #e6f1ea;"
        "  color: #1e3a32;"
        "  selection-background-color: #d8efe2;"
        "  selection-color: #0f2d23;"
        "}"
        "QWidget#tabGestionStock QHeaderView::section, QWidget#tabTriRechercheExport QHeaderView::section {"
        "  background-color: #184f3c;"
        "  color: white;"
        "  padding: 10px 8px;"
        "  border: none;"
        "  font-size: 14px;"
        "  font-weight: 700;"
        "}"
        "QWidget#tabGestionStock QScrollBar:vertical, QWidget#tabTriRechercheExport QScrollBar:vertical {"
        "  background: #edf3ef;"
        "  width: 10px;"
        "  border-radius: 5px;"
        "}"
        "QWidget#tabGestionStock QScrollBar::handle:vertical, QWidget#tabTriRechercheExport QScrollBar::handle:vertical {"
        "  background: #7aa88f;"
        "  min-height: 28px;"
        "  border-radius: 5px;"
        "}"
        "QWidget#tabGestionStock QScrollBar:horizontal {"
        "  height: 0px;"
        "}"
        "QWidget#tabGestionStock QScrollBar:vertical {"
        "  width: 0px;"
        "}";

    ui->tabGestionStock->setStyleSheet(stockModernStyle);
    ui->tabTriRechercheExport->setStyleSheet(stockModernStyle);

    const QString historiqueStyle =
        "QWidget#tabHistoriqueEtatTemps {"
        "  background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #f7fbf8, stop:1 #ecf6f0);"
        "}"
        "QWidget#tabHistoriqueEtatTemps QGroupBox {"
        "  background: #ffffff;"
        "  border: 1px solid #d4e8da;"
        "  border-radius: 14px;"
        "  margin-top: 14px;"
        "  padding-top: 16px;"
        "  color: #163a2d;"
        "  font-weight: 800;"
        "}"
        "QWidget#tabHistoriqueEtatTemps QGroupBox::title {"
        "  subcontrol-origin: margin;"
        "  left: 14px;"
        "  padding: 0 8px;"
        "  color: #14532d;"
        "}"
        "QWidget#tabHistoriqueEtatTemps QLabel {"
        "  color: #1e3a32;"
        "}"
        "QWidget#tabHistoriqueEtatTemps QLCDNumber {"
        "  border: 1px solid #0f5132;"
        "  border-radius: 12px;"
        "  background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #0b2f26, stop:1 #0a2135);"
        "  color: #4ade80;"
        "}"
        "QWidget#tabHistoriqueEtatTemps QProgressBar {"
        "  border: 1px solid #c7dbcf;"
        "  border-radius: 9px;"
        "  text-align: center;"
        "  min-height: 30px;"
        "  background: #f6fbf8;"
        "  color: #123327;"
        "  font-weight: 700;"
        "}"
        "QWidget#tabHistoriqueEtatTemps QProgressBar::chunk {"
        "  border-radius: 8px;"
        "  background-color: #22c55e;"
        "}"
        "QWidget#tabHistoriqueEtatTemps QDateEdit {"
        "  background: #fbfefc;"
        "  border: 1px solid #c9ddd0;"
        "  border-radius: 10px;"
        "  min-height: 34px;"
        "  padding: 6px 10px;"
        "  color: #123327;"
        "}"
        "QWidget#tabHistoriqueEtatTemps QDateEdit:focus {"
        "  border: 2px solid #1f6f55;"
        "  background: #ffffff;"
        "}"
        "QWidget#tabHistoriqueEtatTemps QPushButton#btnAfficherHistorique {"
        "  border: none;"
        "  border-radius: 10px;"
        "  min-height: 36px;"
        "  padding: 8px 16px;"
        "  color: #ffffff;"
        "  font-weight: 800;"
        "  background-color: #18543f;"
        "}"
        "QWidget#tabHistoriqueEtatTemps QPushButton#btnAfficherHistorique:hover {"
        "  background-color: #216b50;"
        "}"
        "QWidget#tabHistoriqueEtatTemps QListWidget {"
        "  border: 1px solid #d7e8db;"
        "  border-radius: 12px;"
        "  background: #fbfefc;"
        "  color: #163a2d;"
        "}"
        "QWidget#tabHistoriqueEtatTemps QListWidget::item {"
        "  border-bottom: 1px dashed #deebe3;"
        "  padding: 8px 10px;"
        "}"
        "QWidget#tabHistoriqueEtatTemps QTableWidget {"
        "  background: #ffffff;"
        "  alternate-background-color: #f3faf6;"
        "  border: 1px solid #d5e7dc;"
        "  border-radius: 12px;"
        "  gridline-color: #e6f1ea;"
        "  color: #1e3a32;"
        "  selection-background-color: #d8efe2;"
        "  selection-color: #0f2d23;"
        "}"
        "QWidget#tabHistoriqueEtatTemps QHeaderView::section {"
        "  background-color: #184f3c;"
        "  color: white;"
        "  padding: 9px 8px;"
        "  border: none;"
        "  font-size: 13px;"
        "  font-weight: 800;"
        "}";

    ui->tabHistoriqueEtatTemps->setStyleSheet(historiqueStyle);

    if (ui->dateDebut && ui->dateFin) {
        const QDate today = QDate::currentDate();
        ui->dateFin->setDate(today);
        ui->dateDebut->setDate(today.addDays(-30));
    }

    const QString alertesStyle =
        "QWidget#tabAlertesTendancesRapports {"
        "  background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #f8fbf9, stop:1 #edf7f1);"
        "}"
        "QWidget#tabAlertesTendancesRapports QGroupBox {"
        "  background: #ffffff;"
        "  border: 1px solid #d4e8da;"
        "  border-radius: 14px;"
        "  margin-top: 14px;"
        "  padding-top: 16px;"
        "  color: #163a2d;"
        "  font-weight: 800;"
        "}"
        "QWidget#tabAlertesTendancesRapports QGroupBox::title {"
        "  subcontrol-origin: margin;"
        "  left: 14px;"
        "  padding: 0 8px;"
        "  color: #14532d;"
        "}"
        "QWidget#tabAlertesTendancesRapports QLabel {"
        "  color: #1e3a32;"
        "}"
        "QWidget#tabAlertesTendancesRapports QComboBox {"
        "  background: #fbfefc;"
        "  border: 1px solid #c9ddd0;"
        "  border-radius: 10px;"
        "  min-height: 34px;"
        "  padding: 6px 10px;"
        "  color: #123327;"
        "}"
        "QWidget#tabAlertesTendancesRapports QComboBox:focus {"
        "  border: 2px solid #1f6f55;"
        "  background: #ffffff;"
        "}"
        "QWidget#tabAlertesTendancesRapports QPushButton {"
        "  border: none;"
        "  border-radius: 10px;"
        "  min-height: 36px;"
        "  padding: 8px 16px;"
        "  color: #ffffff;"
        "  font-weight: 800;"
        "}"
        "QWidget#tabAlertesTendancesRapports QPushButton#btnVerifierAlertes {"
        "  background-color: #b45309;"
        "}"
        "QWidget#tabAlertesTendancesRapports QPushButton#btnVerifierAlertes:hover {"
        "  background-color: #c96a1d;"
        "}"
        "QWidget#tabAlertesTendancesRapports QPushButton#btnAnalyserTendances,"
        "QWidget#tabAlertesTendancesRapports QPushButton#btnGenererRapport {"
        "  background-color: #18543f;"
        "}"
        "QWidget#tabAlertesTendancesRapports QPushButton#btnAnalyserTendances:hover,"
        "QWidget#tabAlertesTendancesRapports QPushButton#btnGenererRapport:hover {"
        "  background-color: #216b50;"
        "}"
        "QWidget#tabAlertesTendancesRapports QPushButton#btnExportRapportPDF {"
        "  background-color: #1554b8;"
        "}"
        "QWidget#tabAlertesTendancesRapports QPushButton#btnExportRapportPDF:hover {"
        "  background-color: #1d65d4;"
        "}"
        "QWidget#tabAlertesTendancesRapports QListWidget,"
        "QWidget#tabAlertesTendancesRapports QTextEdit,"
        "QWidget#tabAlertesTendancesRapports QTableWidget {"
        "  border: 1px solid #d7e8db;"
        "  border-radius: 12px;"
        "  background: #fbfefc;"
        "  color: #163a2d;"
        "}"
        "QWidget#tabAlertesTendancesRapports QTableWidget {"
        "  alternate-background-color: #f3faf6;"
        "  gridline-color: #e6f1ea;"
        "  selection-background-color: #d8efe2;"
        "  selection-color: #0f2d23;"
        "}"
        "QWidget#tabAlertesTendancesRapports QHeaderView::section {"
        "  background-color: #184f3c;"
        "  color: white;"
        "  padding: 9px 8px;"
        "  border: none;"
        "  font-size: 13px;"
        "  font-weight: 800;"
        "}";

    ui->tabAlertesTendancesRapports->setStyleSheet(alertesStyle);

    // Organiser l'onglet Alertes/Tendances/Rapports en 2 colonnes pour clarifier les metiers.
    if (ui->layoutScrollAlertesTendances && ui->groupConfigAlertes && ui->groupStocksSousSeuil &&
        ui->groupOptionsTendances && ui->groupGraphique && ui->groupKpiTendances && ui->groupAnalyseAuto &&
        ui->groupFournisseursERP && ui->groupSelectionRapport && ui->layoutBtnRapport && ui->groupApercuRapport) {
        QSplitter *existingSplitter = ui->scrollContentAlertesTendances->findChild<QSplitter*>("splitAlertesMain");
        if (!existingSplitter) {
            ui->layoutScrollAlertesTendances->removeWidget(ui->groupConfigAlertes);
            ui->layoutScrollAlertesTendances->removeWidget(ui->groupStocksSousSeuil);
            ui->layoutScrollAlertesTendances->removeWidget(ui->groupOptionsTendances);
            ui->layoutScrollAlertesTendances->removeWidget(ui->groupGraphique);
            ui->layoutScrollAlertesTendances->removeWidget(ui->groupKpiTendances);
            ui->layoutScrollAlertesTendances->removeWidget(ui->groupAnalyseAuto);
            ui->layoutScrollAlertesTendances->removeWidget(ui->groupFournisseursERP);
            ui->layoutScrollAlertesTendances->removeWidget(ui->groupSelectionRapport);
            ui->layoutScrollAlertesTendances->removeWidget(ui->groupApercuRapport);

            for (int i = 0; i < ui->layoutScrollAlertesTendances->count(); ++i) {
                QLayoutItem *item = ui->layoutScrollAlertesTendances->itemAt(i);
                if (item && item->layout() == ui->layoutBtnRapport) {
                    ui->layoutScrollAlertesTendances->takeAt(i);
                    break;
                }
            }

            auto *splitAlertes = new QSplitter(Qt::Horizontal, ui->scrollContentAlertesTendances);
            splitAlertes->setObjectName("splitAlertesMain");
            splitAlertes->setChildrenCollapsible(false);
            splitAlertes->setHandleWidth(8);

            auto *leftPane = new QWidget(splitAlertes);
            auto *leftLayout = new QVBoxLayout(leftPane);
            leftLayout->setContentsMargins(0, 0, 0, 0);
            leftLayout->setSpacing(12);
            leftLayout->addWidget(ui->groupConfigAlertes);
            leftLayout->addWidget(ui->groupStocksSousSeuil);
            leftLayout->addWidget(ui->groupOptionsTendances);
            leftLayout->addWidget(ui->groupGraphique, 1);

            auto *rightPane = new QWidget(splitAlertes);
            auto *rightLayout = new QVBoxLayout(rightPane);
            rightLayout->setContentsMargins(0, 0, 0, 0);
            rightLayout->setSpacing(12);
            rightLayout->addWidget(ui->groupKpiTendances);
            rightLayout->addWidget(ui->groupAnalyseAuto, 1);
            rightLayout->addWidget(ui->groupSelectionRapport);
            rightLayout->addLayout(ui->layoutBtnRapport);
            rightLayout->addWidget(ui->groupApercuRapport, 1);

            splitAlertes->addWidget(leftPane);
            splitAlertes->addWidget(rightPane);
            splitAlertes->setStretchFactor(0, 5);
            splitAlertes->setStretchFactor(1, 6);
            splitAlertes->setSizes({620, 720});

            ui->layoutScrollAlertesTendances->insertWidget(0, splitAlertes, 1);
            ui->layoutScrollAlertesTendances->setStretch(0, 1);

            ui->groupGraphique->setMinimumHeight(330);
            ui->groupAnalyseAuto->setMinimumHeight(220);
            ui->groupApercuRapport->setMinimumHeight(220);
            ui->groupSelectionRapport->setMinimumHeight(260);
            if (ui->tableRapportSelection) {
                ui->tableRapportSelection->setMaximumHeight(16777215);
                ui->tableRapportSelection->setMinimumHeight(220);
            }
        }
    }

    // Deplacer la partie Fournisseurs dans un onglet independant (comme Gestion Stock / Calendrier).
    if (ui->tabWidgetStock && ui->groupFournisseursERP) {
        QWidget *tabFournisseurs = ui->tabWidgetStock->findChild<QWidget*>("tabFournisseursERP");
        if (!tabFournisseurs) {
            tabFournisseurs = new QWidget(ui->tabWidgetStock);
            tabFournisseurs->setObjectName("tabFournisseursERP");

            auto *tabLayout = new QVBoxLayout(tabFournisseurs);
            tabLayout->setContentsMargins(12, 12, 12, 12);
            tabLayout->setSpacing(0);

            auto *scroll = new QScrollArea(tabFournisseurs);
            scroll->setWidgetResizable(true);
            scroll->setFrameShape(QFrame::NoFrame);

            auto *scrollContent = new QWidget(scroll);
            auto *scrollLayout = new QVBoxLayout(scrollContent);
            scrollLayout->setContentsMargins(8, 8, 8, 8);
            scrollLayout->setSpacing(10);
            scrollLayout->addWidget(ui->groupFournisseursERP);
            scrollLayout->addStretch();

            scroll->setWidget(scrollContent);
            tabLayout->addWidget(scroll);

            tabFournisseurs->setStyleSheet(
                "QWidget#tabFournisseursERP {"
                "  background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #f8fbf9, stop:1 #edf7f1);"
                "}"
                "QWidget#tabFournisseursERP QGroupBox {"
                "  background: #ffffff;"
                "  border: 1px solid #d4e8da;"
                "  border-radius: 14px;"
                "  margin-top: 14px;"
                "  padding-top: 16px;"
                "  color: #163a2d;"
                "  font-weight: 800;"
                "}"
                "QWidget#tabFournisseursERP QGroupBox::title {"
                "  subcontrol-origin: margin;"
                "  left: 14px;"
                "  padding: 0 8px;"
                "  color: #14532d;"
                "}"
                "QWidget#tabFournisseursERP QLabel {"
                "  color: #1e3a32;"
                "  font-size: 14px;"
                "  font-weight: 650;"
                "}"
                "QWidget#tabFournisseursERP QLineEdit, QWidget#tabFournisseursERP QComboBox {"
                "  background: #fbfefc;"
                "  border: 1px solid #bfd9ca;"
                "  border-radius: 10px;"
                "  min-height: 36px;"
                "  padding: 6px 10px;"
                "  color: #123327;"
                "  selection-background-color: #1f6f55;"
                "  selection-color: #ffffff;"
                "}"
                "QWidget#tabFournisseursERP QLineEdit:focus, QWidget#tabFournisseursERP QComboBox:focus {"
                "  border: 2px solid #1f6f55;"
                "  background: #ffffff;"
                "}"
                "QWidget#tabFournisseursERP QPushButton {"
                "  border: none;"
                "  border-radius: 10px;"
                "  min-height: 36px;"
                "  padding: 8px 14px;"
                "  color: #ffffff;"
                "  font-weight: 800;"
                "}"
                "QWidget#tabFournisseursERP QPushButton#btnAjouterFournisseur {"
                "  background-color: #155e75;"
                "}"
                "QWidget#tabFournisseursERP QPushButton#btnAjouterFournisseur:hover {"
                "  background-color: #0e7490;"
                "}"
                "QWidget#tabFournisseursERP QPushButton#btnLierHuileFournisseur {"
                "  background-color: #18543f;"
                "}"
                "QWidget#tabFournisseursERP QPushButton#btnLierHuileFournisseur:hover {"
                "  background-color: #216b50;"
                "}"
                "QWidget#tabFournisseursERP QPushButton#btnGenererCommandeAlerte {"
                "  background-color: #b45309;"
                "}"
                "QWidget#tabFournisseursERP QPushButton#btnGenererCommandeAlerte:hover {"
                "  background-color: #d97706;"
                "}"
                "QWidget#tabFournisseursERP QTableWidget {"
                "  background: #ffffff;"
                "  alternate-background-color: #f4fbf7;"
                "  border: 1px solid #cfe3d6;"
                "  border-radius: 12px;"
                "  gridline-color: #e2efe8;"
                "  color: #1e3a32;"
                "  selection-background-color: #d8efe2;"
                "  selection-color: #0f2d23;"
                "}"
                "QWidget#tabFournisseursERP QHeaderView::section {"
                "  background-color: #0f5132;"
                "  color: #ffffff;"
                "  padding: 9px 8px;"
                "  border: none;"
                "  font-size: 13px;"
                "  font-weight: 800;"
                "}");

            ui->tabWidgetStock->addTab(tabFournisseurs, "🏭 Fournisseurs ERP");
        }
    }

    // Recomposer l'onglet Historique en 2 parties: pilotage a gauche, tableau a droite.
    if (ui->layoutScrollHistoriqueEtat && ui->groupTempsReel && ui->groupEtatGlobal &&
        ui->groupProduitsCritiques && ui->groupFiltrePeriode && ui->groupHistorique) {
        QSplitter *existingSplitter = ui->scrollContentHistoriqueEtat->findChild<QSplitter*>("splitHistoriqueMain");
        if (!existingSplitter) {
            ui->layoutScrollHistoriqueEtat->removeWidget(ui->groupTempsReel);
            ui->layoutScrollHistoriqueEtat->removeWidget(ui->groupEtatGlobal);
            ui->layoutScrollHistoriqueEtat->removeWidget(ui->groupProduitsCritiques);
            ui->layoutScrollHistoriqueEtat->removeWidget(ui->groupFiltrePeriode);
            ui->layoutScrollHistoriqueEtat->removeWidget(ui->groupHistorique);

            auto *splitHistorique = new QSplitter(Qt::Horizontal, ui->scrollContentHistoriqueEtat);
            splitHistorique->setObjectName("splitHistoriqueMain");
            splitHistorique->setChildrenCollapsible(false);
            splitHistorique->setHandleWidth(8);

            auto *leftPane = new QWidget(splitHistorique);
            auto *leftLayout = new QVBoxLayout(leftPane);
            leftLayout->setContentsMargins(0, 0, 0, 0);
            leftLayout->setSpacing(12);
            leftLayout->addWidget(ui->groupTempsReel);
            leftLayout->addWidget(ui->groupEtatGlobal);
            leftLayout->addWidget(ui->groupProduitsCritiques);
            leftLayout->addWidget(ui->groupFiltrePeriode);
            leftLayout->addStretch();

            auto *rightPane = new QWidget(splitHistorique);
            auto *rightLayout = new QVBoxLayout(rightPane);
            rightLayout->setContentsMargins(0, 0, 0, 0);
            rightLayout->setSpacing(12);
            rightLayout->addWidget(ui->groupHistorique, 1);

            splitHistorique->addWidget(leftPane);
            splitHistorique->addWidget(rightPane);
            splitHistorique->setStretchFactor(0, 4);
            splitHistorique->setStretchFactor(1, 7);
            splitHistorique->setSizes({430, 900});

            ui->layoutScrollHistoriqueEtat->insertWidget(0, splitHistorique, 1);
            ui->layoutScrollHistoriqueEtat->setStretch(0, 1);

            ui->groupHistorique->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
            ui->groupHistorique->setMinimumHeight(430);
            if (ui->tableHistorique) {
                ui->tableHistorique->setMinimumHeight(380);
                ui->tableHistorique->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
            }
            if (ui->scrollAreaHistoriqueEtat) {
                ui->scrollAreaHistoriqueEtat->setWidgetResizable(true);
                ui->scrollAreaHistoriqueEtat->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
            }
        }
    }

    const auto setCompactButton = [](QPushButton *btn) {
        if (!btn) {
            return;
        }
        btn->setMinimumHeight(38);
        btn->setMaximumHeight(44);
    };

    setCompactButton(ui->btnAjouter);
    setCompactButton(ui->btnModifier);
    setCompactButton(ui->btnSupprimer);
    setCompactButton(ui->btnConsulter);
    setCompactButton(ui->btnRechercher);
    setCompactButton(ui->btnReinitialiser);
    setCompactButton(ui->btnAppliquerTri);
    setCompactButton(ui->btnExporter);

    if (ui->btnAjouter) ui->btnAjouter->setMaximumWidth(320);
    if (ui->btnModifier) ui->btnModifier->setMaximumWidth(320);
    if (ui->btnSupprimer) ui->btnSupprimer->setMaximumWidth(320);
    if (ui->btnConsulter) ui->btnConsulter->setMaximumWidth(320);

    // Recomposer l'onglet Gestion en 2 colonnes: formulaire a gauche, tableau a droite
    if (ui->verticalLayout_1 && ui->groupInfoStock && ui->groupTableStocks && ui->layoutBtnCRUD) {
        ui->verticalLayout_1->removeWidget(ui->groupInfoStock);
        ui->verticalLayout_1->removeWidget(ui->groupTableStocks);

        for (int i = 0; i < ui->verticalLayout_1->count(); ++i) {
            QLayoutItem *item = ui->verticalLayout_1->itemAt(i);
            if (item && item->layout() == ui->layoutBtnCRUD) {
                ui->verticalLayout_1->takeAt(i);
                break;
            }
        }

        auto *mainRow = new QHBoxLayout();
        mainRow->setSpacing(18);

        auto *leftColumn = new QVBoxLayout();
        leftColumn->setSpacing(12);
        leftColumn->addWidget(ui->groupInfoStock);
        leftColumn->addLayout(ui->layoutBtnCRUD);
        leftColumn->addStretch();

        ui->groupInfoStock->setMaximumWidth(680);
        ui->groupTableStocks->setMinimumWidth(0);
        ui->groupTableStocks->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

        mainRow->addLayout(leftColumn, 5);
        mainRow->addWidget(ui->groupTableStocks, 7);

        ui->verticalLayout_1->insertLayout(0, mainRow);
    }

    const auto applySoftShadow = [](QWidget *widget) {
        if (!widget) {
            return;
        }
        auto *shadow = new QGraphicsDropShadowEffect(widget);
        shadow->setBlurRadius(22);
        shadow->setOffset(0, 4);
        shadow->setColor(QColor(18, 73, 52, 38));
        widget->setGraphicsEffect(shadow);
    };

    applySoftShadow(ui->groupInfoStock);
    applySoftShadow(ui->groupTableStocks);
    applySoftShadow(ui->groupCriteres);
    applySoftShadow(ui->groupResultatsCombined);
}

void Stock::setupArduinoTabUi()
{
    const int tabIndex = ui->tabWidgetStock->indexOf(ui->tabArduino);
    if (tabIndex >= 0) {
        ui->tabWidgetStock->setTabText(tabIndex, "Suivi Production Arduino");
    }

    ui->tabArduino->setStyleSheet(
        "QWidget { background-color: #0F172A; color: white; }"
        "QGroupBox { border: 1px solid #273247; border-radius: 12px; margin-top: 12px; "
        "padding-top: 14px; color: #00C896; font-weight: 700; background-color: #111827; }"
        "QGroupBox::title { subcontrol-origin: margin; left: 12px; padding: 0 8px; }"
        "QLabel { color: white; }"
        "QPushButton { background-color: #00C896; color: #0b1220; border: none; "
        "border-radius: 8px; padding: 8px 14px; font-weight: 700; min-height: 34px; }"
        "QPushButton:hover { background-color: #16d6a5; }"
        "QComboBox { background-color: #0b1220; color: white; border: 1px solid #334155; "
        "border-radius: 8px; padding: 6px 10px; min-height: 34px; }"
        "QTextEdit { background-color: #0b1220; color: #e5e7eb; border: 1px solid #334155; border-radius: 8px; }"
    );

    if (ui->verticalLayout_12) {
        ui->verticalLayout_12->setSpacing(14);
        ui->verticalLayout_12->setContentsMargins(16, 16, 16, 16);
    }

    const QString groupStyle =
        "QGroupBox { border: 1px solid #2f3e56; border-radius: 12px; margin-top: 14px; "
        "padding-top: 14px; color: #D4AF37; font-weight: 800; background-color: #111827; }"
        "QGroupBox::title { subcontrol-origin: margin; left: 12px; right: 12px; padding: 0 10px; }";

    if (ui->groupConnexion) {
        ui->groupConnexion->setTitle("Connexion Arduino");
        ui->groupConnexion->setMinimumHeight(96);
        ui->groupConnexion->setAlignment(Qt::AlignHCenter);
        ui->groupConnexion->setStyleSheet(groupStyle);
    }
    if (ui->groupCapteur) {
        ui->groupCapteur->setTitle("Suivi Poids en Temps Reel");
        ui->groupCapteur->setMinimumHeight(255);
        ui->groupCapteur->setAlignment(Qt::AlignHCenter);
        ui->groupCapteur->setStyleSheet(groupStyle);
    }
    if (ui->groupLogs) {
        ui->groupLogs->setTitle("Logs Serie");
        ui->groupLogs->setMinimumHeight(220);
        ui->groupLogs->setAlignment(Qt::AlignHCenter);
        ui->groupLogs->setStyleSheet(groupStyle);
    }

    if (ui->labelEtatConnexion) {
        ui->labelEtatConnexion->setText("Non connecte");
        ui->labelEtatConnexion->setStyleSheet(
            "QLabel { color: #fecaca; font-size: 15px; font-weight: 800; padding: 6px 12px; "
            "background-color: rgba(239,68,68,0.18); border: 1px solid rgba(239,68,68,0.42); "
            "border-radius: 8px; }");
    }

    m_comboPorts = new QComboBox(this);
    m_comboPorts->setMinimumWidth(190);
    m_btnRefreshPorts = new QPushButton("Actualiser Ports", this);
    m_btnConnectArduino = new QPushButton("Connecter", this);
    m_btnConnectArduino->setMinimumWidth(120);
    m_btnRefreshPorts->setMinimumWidth(140);

    if (ui->layoutConnexion) {
        ui->layoutConnexion->setSpacing(10);
        ui->layoutConnexion->setContentsMargins(12, 10, 12, 10);
        ui->layoutConnexion->insertWidget(1, m_comboPorts);
        ui->layoutConnexion->insertWidget(2, m_btnRefreshPorts);
        ui->layoutConnexion->insertWidget(3, m_btnConnectArduino);
    }

    m_lblPoidsTitre = new QLabel("Poids actuel", this);
    m_lblPoidsTitre->setAlignment(Qt::AlignCenter);
    m_lblPoidsTitre->setStyleSheet(
        "QLabel { color: #d1d5db; font-size: 28px; font-weight: 700; }");

    m_lblPoidsValeur = new QLabel("-- L", this);
    m_lblPoidsValeur->setAlignment(Qt::AlignCenter);
    m_lblPoidsValeur->setStyleSheet(
        "QLabel { color: #3df5cb; font-size: 64px; font-weight: 900; letter-spacing: 1px; "
        "background-color: #0b1933; border: 1px solid #2f3e56; border-radius: 10px; padding: 6px 10px; }");
    auto *poidsGlow = new QGraphicsDropShadowEffect(m_lblPoidsValeur);
    poidsGlow->setBlurRadius(24);
    poidsGlow->setOffset(0, 0);
    poidsGlow->setColor(QColor(0, 200, 150, 160));
    m_lblPoidsValeur->setGraphicsEffect(poidsGlow);

    m_lblStockIdValeur = new QLabel("STOCK_ID: --", this);
    m_lblStockIdValeur->setAlignment(Qt::AlignCenter);
    m_lblStockIdValeur->setStyleSheet(
        "QLabel { color: #93c5fd; font-size: 16px; font-weight: 800; letter-spacing: 1px; "
        "padding: 6px 10px; background-color: rgba(59,130,246,0.15); border: 1px solid rgba(59,130,246,0.4); "
        "border-radius: 8px; }");

    m_progressArduino = new QProgressBar(this);
    m_progressArduino->setRange(0, 20);
    m_progressArduino->setValue(0);
    m_progressArduino->setFormat("%v / %m L");
    m_progressArduino->setAlignment(Qt::AlignCenter);
    m_progressArduino->setMinimumHeight(30);

    m_lblStatusArduino = new QLabel("En attente de donnees...", this);
    m_lblStatusArduino->setAlignment(Qt::AlignCenter);
    m_lblStatusArduino->setStyleSheet(
        "QLabel { color: #f8fafc; font-size: 18px; font-weight: 800; "
        "padding: 8px 12px; background-color: #1f2937; border: 1px solid #334155; border-radius: 8px; }");
    auto *statusGlow = new QGraphicsDropShadowEffect(m_lblStatusArduino);
    statusGlow->setBlurRadius(16);
    statusGlow->setOffset(0, 0);
    statusGlow->setColor(QColor(255, 255, 255, 110));
    m_lblStatusArduino->setGraphicsEffect(statusGlow);

    m_btnValiderPoids = new QPushButton("Valider Poids", this);
    m_btnValiderPoids->setStyleSheet(
        "QPushButton { background-color: #D4AF37; color: #111827; font-size: 15px; "
        "font-weight: 800; border-radius: 10px; padding: 10px 20px; }"
        "QPushButton:hover { background-color: #e2c45d; }");

    auto *manualRow = new QHBoxLayout();
    manualRow->setSpacing(10);
    auto *lblManual = new QLabel("Mode test sans capteur", this);
    lblManual->setStyleSheet("QLabel { color: #cbd5e1; font-size: 14px; font-weight: 700; }");

    m_spinManualWeight = new QDoubleSpinBox(this);
    m_spinManualWeight->setRange(0.0, 20.0);
    m_spinManualWeight->setDecimals(1);
    m_spinManualWeight->setSingleStep(0.1);
    m_spinManualWeight->setSuffix(" L");
    m_spinManualWeight->setValue(0.0);
    m_spinManualWeight->setMinimumWidth(120);
    m_spinManualWeight->setStyleSheet(
        "QDoubleSpinBox { background-color: #0b1220; color: #f8fafc; border: 1px solid #334155; "
        "border-radius: 8px; padding: 6px 10px; min-height: 34px; font-weight: 700; }"
        "QDoubleSpinBox::up-button, QDoubleSpinBox::down-button { width: 18px; }");

    m_btnPlusOneL = new QPushButton("+ 1L", this);
    m_btnPlusOneL->setMinimumWidth(80);
    m_btnConfirmWeight = new QPushButton("Confirmer Poids", this);
    m_btnConfirmWeight->setMinimumWidth(150);
    m_btnConfirmWeight->setStyleSheet(
        "QPushButton { background-color: #D4AF37; color: #111827; font-size: 14px; "
        "font-weight: 800; border-radius: 10px; padding: 9px 16px; }"
        "QPushButton:hover { background-color: #e2c45d; }");

    auto *manualHint = new QLabel("Seuil min: 10L", this);
    manualHint->setStyleSheet("QLabel { color: #fbbf24; font-size: 13px; font-weight: 700; }");

    manualRow->addWidget(lblManual);
    manualRow->addWidget(m_spinManualWeight);
    manualRow->addWidget(m_btnPlusOneL);
    manualRow->addWidget(m_btnConfirmWeight);
    manualRow->addWidget(manualHint);
    manualRow->addStretch();

    if (ui->layoutCapteur) {
        ui->layoutCapteur->addWidget(m_lblPoidsTitre);
        ui->layoutCapteur->addWidget(m_lblPoidsValeur);
        ui->layoutCapteur->addWidget(m_lblStockIdValeur, 0, Qt::AlignHCenter);
        ui->layoutCapteur->addWidget(m_progressArduino);
        ui->layoutCapteur->addWidget(m_lblStatusArduino);
        ui->layoutCapteur->addLayout(manualRow);
        ui->layoutCapteur->addWidget(m_btnValiderPoids, 0, Qt::AlignHCenter);
    }

    if (ui->labelValeurCapteur) {
        ui->labelValeurCapteur->hide();
    }

    if (ui->btnSynchroniser) {
        ui->btnSynchroniser->setText("Rafraichir Data/DB");
        ui->btnSynchroniser->setMinimumWidth(160);
    }

    if (ui->textLogsArduino) {
        ui->textLogsArduino->setPlaceholderText("Les trames serie apparaitront ici...");
        ui->textLogsArduino->document()->setMaximumBlockCount(350);
    }

    connect(m_btnRefreshPorts, &QPushButton::clicked, this, &Stock::onRefreshPortsClicked);
    connect(m_btnConnectArduino, &QPushButton::clicked, this, &Stock::onConnectArduinoClicked);
        connect(m_spinManualWeight, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &Stock::onManualWeightChanged);
        connect(m_btnPlusOneL, &QPushButton::clicked, this, &Stock::onManualPlusOneClicked);
        connect(m_btnConfirmWeight, &QPushButton::clicked, this, &Stock::onManualConfirmClicked);
    connect(m_btnValiderPoids, &QPushButton::clicked, this, &Stock::onValidatePoidsClicked);

    updateProgressBarColor(QStringLiteral("REMPLISSAGE"));
}

void Stock::refreshAvailablePorts()
{
    if (!m_comboPorts) {
        return;
    }

    m_comboPorts->clear();
    const auto ports = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &info : ports) {
        QString desc = info.description().trimmed();
        if (desc.isEmpty()) {
            desc = "Arduino/Serial";
        }
        m_comboPorts->addItem(QString("%1 (%2)").arg(info.portName(), desc), info.portName());
    }

    if (m_comboPorts->count() == 0) {
        m_comboPorts->addItem("Aucun port detecte", QString());
    }
}

void Stock::onRefreshPortsClicked()
{
    refreshAvailablePorts();
}

void Stock::onConnectArduinoClicked()
{
    if (!m_serial) {
        return;
    }

    if (m_serial->isOpen()) {
        m_serial->close();
        if (ui->labelEtatConnexion) {
            ui->labelEtatConnexion->setText("Non connecte");
            ui->labelEtatConnexion->setStyleSheet(
                "QLabel { color: #fecaca; font-size: 15px; font-weight: 800; padding: 6px 12px; "
                "background-color: rgba(239,68,68,0.18); border: 1px solid rgba(239,68,68,0.42); "
                "border-radius: 8px; }");
        }
        if (m_btnConnectArduino) {
            m_btnConnectArduino->setText("Connecter");
        }
        if (ui->textLogsArduino) {
            ui->textLogsArduino->append("Connexion serie fermee");
        }
        return;
    }

    if (!m_comboPorts || m_comboPorts->count() == 0) {
        afficherMessage("Arduino", "Aucun port serie disponible", true);
        return;
    }

    const QString portName = m_comboPorts->currentData().toString();
    if (portName.isEmpty()) {
        afficherMessage("Arduino", "Selectionnez un port serie valide", true);
        return;
    }

    m_serial->setPortName(portName);
    m_serial->setBaudRate(QSerialPort::Baud9600);
    m_serial->setDataBits(QSerialPort::Data8);
    m_serial->setParity(QSerialPort::NoParity);
    m_serial->setStopBits(QSerialPort::OneStop);
    m_serial->setFlowControl(QSerialPort::NoFlowControl);

    if (!m_serial->open(QIODevice::ReadOnly)) {
        afficherMessage("Arduino", "Impossible d'ouvrir " + portName + " : " + m_serial->errorString(), true);
        return;
    }

    m_serialBuffer.clear();
    m_freezeValue = false;

    if (ui->labelEtatConnexion) {
        ui->labelEtatConnexion->setText("Connecte: " + portName);
        ui->labelEtatConnexion->setStyleSheet(
            "QLabel { color: #d1fae5; font-size: 15px; font-weight: 800; padding: 6px 12px; "
            "background-color: rgba(0,200,150,0.20); border: 1px solid rgba(0,200,150,0.45); "
            "border-radius: 8px; }");
    }
    if (m_btnConnectArduino) {
        m_btnConnectArduino->setText("Deconnecter");
    }
    if (ui->textLogsArduino) {
        ui->textLogsArduino->append("Connexion serie etablie sur " + portName + " (9600)");
    }
}

void Stock::onSerialReadyRead()
{
    if (!m_serial) {
        return;
    }

    m_serialBuffer += QString::fromUtf8(m_serial->readAll());
    m_serialBuffer.replace('\r', '\n');
    int newLineIndex = -1;
    while ((newLineIndex = m_serialBuffer.indexOf('\n')) >= 0) {
        const QString line = m_serialBuffer.left(newLineIndex).trimmed();
        m_serialBuffer.remove(0, newLineIndex + 1);
        if (!line.isEmpty()) {
            parseArduinoLine(line);
        }
    }
}

void Stock::onManualPlusOneClicked()
{
    if (!m_spinManualWeight) {
        return;
    }

    const double nextValue = std::min(20.0, m_spinManualWeight->value() + 1.0);
    m_spinManualWeight->setValue(nextValue);
}

void Stock::onManualWeightChanged(double value)
{
    if (m_freezeValue) {
        return;
    }

    m_currentWeightL = value;
    m_currentStatusRaw = (value < m_minWeightThresholdL) ? "FAIBLE" : "STABLE";
    applyArduinoStateUi();
    persistArduinoToStock(false);
}

void Stock::onManualConfirmClicked()
{
    onValidatePoidsClicked();
}

void Stock::parseArduinoLine(const QString &line)
{
    const QString normalizedLine = line.trimmed();

    if (normalizedLine.startsWith("WEIGHT:", Qt::CaseInsensitive)) {
        if (m_freezeValue) {
            return;
        }
        bool ok = false;
        const QString raw = normalizedLine.section(':', 1).trimmed().replace(',', '.');
        const double arduinoValue = raw.toDouble(&ok);
        if (ok) {
            const qint64 nowMs = QDateTime::currentMSecsSinceEpoch();
            const bool recentValidation = (m_lastValidationMs > 0) && ((nowMs - m_lastValidationMs) < 12000);
            const bool suspiciousZero = (arduinoValue <= 0.05) && (m_currentWeightL >= m_minWeightThresholdL);
            const bool intentionalReset = (m_currentStatusRaw == "REMPLISSAGE");
            if (recentValidation && suspiciousZero && !intentionalReset) {
                if (ui->textLogsArduino) {
                    ui->textLogsArduino->append("Info: zero transitoire ignore apres validation");
                }
                return;
            }

            // Requested business mapping: 1 ml sensor unit is treated as 1 L.
            m_currentWeightL = arduinoValue;
            if (m_currentStatusRaw.isEmpty() || m_currentStatusRaw == "STABLE" || m_currentStatusRaw == "FAIBLE") {
                m_currentStatusRaw = (m_currentWeightL < m_minWeightThresholdL) ? "FAIBLE" : "STABLE";
            }
            if (m_spinManualWeight) {
                const QSignalBlocker blocker(m_spinManualWeight);
                m_spinManualWeight->setValue(std::clamp(m_currentWeightL, 0.0, 20.0));
            }
            applyArduinoStateUi();
            persistArduinoToStock(false);
        }
        return;
    }

    if (normalizedLine.startsWith("STATUS:", Qt::CaseInsensitive)) {
        m_currentStatusRaw = normalizedLine.section(':', 1).trimmed().toUpper();
        if (m_currentStatusRaw == "REMPLISSAGE") {
            m_freezeValue = false;
            m_hasStockId = false;
            m_currentStockId = 0;
        }
        if (m_currentStatusRaw == "VALIDATED" || m_currentStatusRaw == "FAIBLE_CONFIRMED") {
            m_freezeValue = true;
            m_lastValidationMs = QDateTime::currentMSecsSinceEpoch();
        }
        applyArduinoStateUi();
        persistArduinoToStock(m_currentStatusRaw == "VALIDATED");
        return;
    }

    if (normalizedLine.startsWith("STOCK_ID:", Qt::CaseInsensitive)) {
        bool ok = false;
        const QString raw = normalizedLine.section(':', 1).trimmed();
        const qlonglong stockId = raw.toLongLong(&ok);
        if (ok) {
            m_currentStockId = stockId;
            m_hasStockId = true;
            applyArduinoStateUi();
        }
        return;
    }

    if (ui->textLogsArduino) {
        ui->textLogsArduino->append("Trame ignoree: " + normalizedLine);
    }
}

void Stock::updateProgressBarColor(const QString &status)
{
    if (!m_progressArduino) {
        return;
    }

    QString chunkColor = "#00C896";
    if (status == "STABLE") {
        chunkColor = "#f59e0b";
    } else if (status == "FAIBLE") {
        chunkColor = "#ef4444";
    } else if (status == "VALIDATED") {
        chunkColor = "#D4AF37";
    }

    m_progressArduino->setStyleSheet(
        QString(
            "QProgressBar { border: 1px solid #334155; border-radius: 10px; text-align: center; "
            "height: 26px; background: #0b1220; color: white; font-weight: 700; }"
            "QProgressBar::chunk { border-radius: 9px; background-color: %1; }"
        ).arg(chunkColor)
    );
}

void Stock::applyArduinoStateUi()
{
    if (m_lblPoidsValeur) {
        m_lblPoidsValeur->setText(QString::number(m_currentWeightL, 'f', 2) + " L");
    }

    if (m_progressArduino) {
        const int barValue = std::clamp(qRound(m_currentWeightL), 0, 20);
        m_progressArduino->setValue(barValue);
    }

    if (m_lblStockIdValeur) {
        QString stockIdText = "STOCK_ID: --";
        QString stockIdStyle =
            "QLabel { color: #93c5fd; font-size: 16px; font-weight: 800; letter-spacing: 1px; "
            "padding: 6px 10px; background-color: rgba(59,130,246,0.15); border: 1px solid rgba(59,130,246,0.4); "
            "border-radius: 8px; }";

        if (m_hasStockId) {
            if (m_currentStockId > 0) {
                stockIdText = QString("STOCK_ID: %1").arg(m_currentStockId, 4, 10, QChar('0'));
                stockIdStyle =
                    "QLabel { color: #d1fae5; font-size: 16px; font-weight: 900; letter-spacing: 1px; "
                    "padding: 6px 10px; background-color: rgba(0,200,150,0.20); border: 1px solid rgba(0,200,150,0.45); "
                    "border-radius: 8px; }";
            } else {
                stockIdText = "STOCK_ID: ECHEC";
                stockIdStyle =
                    "QLabel { color: #fecaca; font-size: 16px; font-weight: 900; letter-spacing: 1px; "
                    "padding: 6px 10px; background-color: rgba(239,68,68,0.22); border: 1px solid rgba(239,68,68,0.45); "
                    "border-radius: 8px; }";
            }
        }

        m_lblStockIdValeur->setText(stockIdText);
        m_lblStockIdValeur->setStyleSheet(stockIdStyle);
    }

    QString statusText = "En attente de donnees...";
    QString statusBadgeStyle = "QLabel { color: #ffffff; font-size: 18px; font-weight: 800; "
                              "padding: 8px 12px; background-color: rgba(255,255,255,0.06); border-radius: 8px; }";
    if (m_currentStatusRaw == "REMPLISSAGE") {
        statusText = "Remplissage en cours";
        statusBadgeStyle = "QLabel { color: #d1fae5; font-size: 18px; font-weight: 800; "
                           "padding: 8px 12px; background-color: rgba(0,200,150,0.20); border-radius: 8px; }";
    } else if (m_currentStatusRaw == "STABLE") {
        statusText = "Stable - en attente validation";
        statusBadgeStyle = "QLabel { color: #fef3c7; font-size: 18px; font-weight: 800; "
                           "padding: 8px 12px; background-color: rgba(245,158,11,0.25); border-radius: 8px; }";
    } else if (m_currentStatusRaw == "FAIBLE") {
        statusText = "Niveau faible";
        statusBadgeStyle = "QLabel { color: #fecaca; font-size: 18px; font-weight: 800; "
                           "padding: 8px 12px; background-color: rgba(239,68,68,0.22); border-radius: 8px; }";
    } else if (m_currentStatusRaw == "FAIBLE_CONFIRMED") {
        statusText = "Echec seuil confirme";
        statusBadgeStyle = "QLabel { color: #fecaca; font-size: 18px; font-weight: 800; "
                           "padding: 8px 12px; background-color: rgba(239,68,68,0.30); border-radius: 8px; }";
    } else if (m_currentStatusRaw == "VALIDATED") {
        statusText = "Valide";
        statusBadgeStyle = "QLabel { color: #fff3c4; font-size: 18px; font-weight: 800; "
                           "padding: 8px 12px; background-color: rgba(212,175,55,0.28); border-radius: 8px; }";
    }

    if (m_lblStatusArduino) {
        m_lblStatusArduino->setText(statusText);
        m_lblStatusArduino->setStyleSheet(statusBadgeStyle);
    }

    updateProgressBarColor(m_currentStatusRaw);

    if (m_lblPoidsValeur) {
        if (m_currentStatusRaw == "VALIDATED") {
            m_lblPoidsValeur->setStyleSheet(
                "QLabel { color: #f5d978; font-size: 64px; font-weight: 900; letter-spacing: 1px; "
                "background-color: #2a2413; border: 1px solid #6f5a1b; border-radius: 10px; padding: 6px 10px; }");
            if (auto *effect = qobject_cast<QGraphicsDropShadowEffect*>(m_lblPoidsValeur->graphicsEffect())) {
                effect->setColor(QColor(212, 175, 55, 170));
            }
        } else {
            m_lblPoidsValeur->setStyleSheet(
                "QLabel { color: #3df5cb; font-size: 64px; font-weight: 900; letter-spacing: 1px; "
                "background-color: #0b1933; border: 1px solid #2f3e56; border-radius: 10px; padding: 6px 10px; }");
            if (auto *effect = qobject_cast<QGraphicsDropShadowEffect*>(m_lblPoidsValeur->graphicsEffect())) {
                effect->setColor(QColor(0, 200, 150, 160));
            }
        }
    }

    if (ui->textLogsArduino) {
        static QString lastLogSignature;
        const QString signature = QString("%1|%2|%3")
                                      .arg(QString::number(m_currentWeightL, 'f', 2))
                                      .arg(m_currentStatusRaw)
                                      .arg(m_hasStockId ? QString::number(m_currentStockId) : QString("--"));
        if (signature == lastLogSignature) {
            return;
        }
        lastLogSignature = signature;

        ui->textLogsArduino->append(
            QString("[%1] WEIGHT=%2L STATUS=%3 STOCK_ID=%4")
                .arg(QDateTime::currentDateTime().toString("HH:mm:ss"))
                .arg(QString::number(m_currentWeightL, 'f', 2))
                .arg(m_currentStatusRaw.isEmpty() ? "N/A" : m_currentStatusRaw)
                .arg(m_hasStockId ? QString::number(m_currentStockId) : QString("--")));
    }
}

void Stock::persistArduinoToStock(bool validated)
{
    if (!connexion || !connexion->isOpen()) {
        return;
    }

    const QString etat = (m_currentStatusRaw == "FAIBLE" || m_currentStatusRaw == "FAIBLE_CONFIRMED")
                             ? "Faible"
                             : ((validated || m_currentStatusRaw == "VALIDATED") ? "Disponible" : "En commande");

    QSqlQuery query(connexion->getDatabase());
    query.prepare(
        "MERGE INTO STOCK s "
        "USING (SELECT 1 AS K FROM DUAL) x "
        "ON (s.TYPE_HUILE = 'MESURE_ARDUINO' AND NVL(s.SOURCE_DONNEE, 'MANUEL') = 'ARDUINO') "
        "WHEN MATCHED THEN UPDATE SET "
        "  s.QUANTITE_ACTUELLE = :qte, "
        "  s.SEUIL_ALERTE = 5, "
        "  s.DATE_DERNIERE_MISE_A_JOUR = SYSDATE, "
        "  s.EMPLACEMENT_STOCKAGE = 'CUVE ARDUINO', "
        "  s.ETAT_STOCK = :etat, "
        "  s.SOURCE_DONNEE = 'ARDUINO', "
        "  s.ARDUINO_STATUS = :astatus, "
        "  s.ARDUINO_STOCK_ID = :asid, "
        "  s.ARDUINO_PORT = :aport, "
        "  s.ARDUINO_DERNIERE_LECTURE = SYSDATE "
        "WHEN NOT MATCHED THEN INSERT "
        "  (ID_STOCK, TYPE_HUILE, QUANTITE_ACTUELLE, SEUIL_ALERTE, DATE_DERNIERE_MISE_A_JOUR, "
        "   EMPLACEMENT_STOCKAGE, ETAT_STOCK, SOURCE_DONNEE, ARDUINO_STATUS, ARDUINO_STOCK_ID, ARDUINO_PORT, ARDUINO_DERNIERE_LECTURE) "
        "VALUES "
        "  (SEQ_STOCK.NEXTVAL, 'MESURE_ARDUINO', :qte, 5, SYSDATE, "
        "   'CUVE ARDUINO', :etat, 'ARDUINO', :astatus, :asid, :aport, SYSDATE)"
    );

    query.bindValue(":qte", m_currentWeightL);
    query.bindValue(":etat", etat);
    query.bindValue(":astatus", m_currentStatusRaw);
    query.bindValue(":asid", m_hasStockId ? QVariant::fromValue(m_currentStockId) : QVariant(QMetaType(QMetaType::LongLong)));
    query.bindValue(":aport", (m_serial && m_serial->isOpen()) ? m_serial->portName() : QString("N/A"));

    if (!query.exec()) {
        // Fallback for older STOCK schema without Arduino-specific columns.
        QSqlQuery qFind(connexion->getDatabase());
        qFind.prepare("SELECT ID_STOCK FROM STOCK WHERE TYPE_HUILE = 'MESURE_ARDUINO' ORDER BY ID_STOCK FETCH FIRST 1 ROWS ONLY");

        if (qFind.exec() && qFind.next()) {
            const int id = qFind.value(0).toInt();
            QSqlQuery qUpd(connexion->getDatabase());
            qUpd.prepare("UPDATE STOCK SET QUANTITE_ACTUELLE = :qte, SEUIL_ALERTE = 5, DATE_DERNIERE_MISE_A_JOUR = SYSDATE, "
                         "EMPLACEMENT_STOCKAGE = 'CUVE ARDUINO', ETAT_STOCK = :etat WHERE ID_STOCK = :id");
            qUpd.bindValue(":qte", m_currentWeightL);
            qUpd.bindValue(":etat", etat);
            qUpd.bindValue(":id", id);
            if (!qUpd.exec() && ui->textLogsArduino) {
                ui->textLogsArduino->append("Erreur DB Arduino (fallback update): " + qUpd.lastError().text());
            }
        } else {
            QSqlQuery qIns(connexion->getDatabase());
            qIns.prepare("INSERT INTO STOCK (TYPE_HUILE, QUANTITE_ACTUELLE, SEUIL_ALERTE, DATE_DERNIERE_MISE_A_JOUR, EMPLACEMENT_STOCKAGE, ETAT_STOCK) "
                         "VALUES ('MESURE_ARDUINO', :qte, 5, SYSDATE, 'CUVE ARDUINO', :etat)");
            qIns.bindValue(":qte", m_currentWeightL);
            qIns.bindValue(":etat", etat);
            if (!qIns.exec() && ui->textLogsArduino) {
                ui->textLogsArduino->append("Erreur DB Arduino (fallback insert): " + qIns.lastError().text());
            }
        }

        if (ui->textLogsArduino) {
            ui->textLogsArduino->append("Info: fallback DB active (schema STOCK ancien). Erreur initiale: " + query.lastError().text());
        }
    }
}

void Stock::onValidatePoidsClicked()
{
    if (m_spinManualWeight && (!m_serial || !m_serial->isOpen())) {
        m_currentWeightL = m_spinManualWeight->value();
    }

    if (m_currentWeightL <= 0.0) {
        afficherMessage("Validation", "Aucune valeur de poids a valider.", true);
        return;
    }

    m_freezeValue = true;
    m_lastValidationMs = QDateTime::currentMSecsSinceEpoch();
    const bool isSuccess = (m_currentWeightL >= m_minWeightThresholdL);
    m_currentStatusRaw = isSuccess ? "VALIDATED" : "FAIBLE";
    applyArduinoStateUi();
    persistArduinoToStock(isSuccess);
    chargerDonneesTable();

    if (isSuccess) {
        afficherMessage("Validation", QString("Poids final valide: %1 L").arg(QString::number(m_currentWeightL, 'f', 2)));
    } else {
        afficherMessage("Validation",
                        QString("Poids enregistre avec echec seuil: %1 L (< %2 L)")
                            .arg(QString::number(m_currentWeightL, 'f', 2))
                            .arg(QString::number(m_minWeightThresholdL, 'f', 0)));
    }
}

void Stock::configurerTableStocks()
{
    ui->tableStocks->setColumnCount(7);
    ui->tableStocks->setHorizontalHeaderLabels({"ID", "Type d'huile", "Quantité (L)", 
                                                  "Seuil (L)", "Dernière MAJ", 
                                                  "Emplacement", "État"});
    
    ui->tableStocks->horizontalHeader()->setStretchLastSection(true);
    ui->tableStocks->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);
    ui->tableStocks->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableStocks->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableStocks->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableStocks->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableStocks->setAlternatingRowColors(true);
    ui->tableStocks->setShowGrid(false);
    ui->tableStocks->verticalHeader()->setVisible(false);
    ui->tableStocks->verticalHeader()->setDefaultSectionSize(34);
    ui->tableStocks->setWordWrap(false);
    ui->tableStocks->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

void Stock::activerScrolling()
{
    // Le wrapping dynamique des onglets en QScrollArea introduisait des artefacts visuels.
    // On conserve la mise en page native définie dans le .ui pour un rendu propre et stable.
    QTabWidget* tabWidget = ui->tabWidgetStock;
    if (!tabWidget) {
        qDebug() << "TabWidget non trouvé";
        return;
    }

    for (int i = 0; i < tabWidget->count(); ++i) {
        QWidget* tab = tabWidget->widget(i);
        if (!tab) {
            continue;
        }

        // Si un QScrollArea existe déjà comme enfant direct d'un onglet, on le rend responsive.
        QScrollArea* existingScrollArea = tab->findChild<QScrollArea*>(QString(), Qt::FindDirectChildrenOnly);
        if (existingScrollArea) {
            existingScrollArea->setWidgetResizable(true);
            existingScrollArea->setFrameShape(QFrame::NoFrame);
        }
    }

    qDebug() << "✓ Layouts natifs conservés (sans wrapping dynamique)";
}

void Stock::onBackClicked()
{
    emit backToMenu();
    this->hide();
}

void Stock::onToggleFullScreen()
{
    if (isFullScreen()) {
        showNormal();
        if (m_btnToggleFullScreen) {
            m_btnToggleFullScreen->setText("Plein ecran");
        }
    } else {
        showFullScreen();
        if (m_btnToggleFullScreen) {
            m_btnToggleFullScreen->setText("Quitter plein ecran");
        }
    }
}

void Stock::chargerDonneesTable()
{
    if (!connexion || !connexion->isOpen()) {
        afficherMessage("Erreur", "Pas de connexion à la base de données", true);
        return;
    }
    
    QSqlQuery query(connexion->getDatabase());
    query.exec("SELECT ID_STOCK, TYPE_HUILE, QUANTITE_ACTUELLE, SEUIL_ALERTE, "
               "DATE_DERNIERE_MISE_A_JOUR, EMPLACEMENT_STOCKAGE, ETAT_STOCK "
               "FROM STOCK ORDER BY ID_STOCK");
    
    ui->tableStocks->setRowCount(0);
    
    int row = 0;
    while (query.next()) {
        ui->tableStocks->insertRow(row);
        
        ui->tableStocks->setItem(row, 0, new QTableWidgetItem(query.value(0).toString()));
        ui->tableStocks->setItem(row, 1, new QTableWidgetItem(query.value(1).toString()));
        ui->tableStocks->setItem(row, 2, new QTableWidgetItem(QString::number(query.value(2).toDouble(), 'f', 2)));
        ui->tableStocks->setItem(row, 3, new QTableWidgetItem(query.value(3).toString()));
        ui->tableStocks->setItem(row, 4, new QTableWidgetItem(query.value(4).toDate().toString("dd/MM/yyyy")));
        ui->tableStocks->setItem(row, 5, new QTableWidgetItem(query.value(5).toString()));
        ui->tableStocks->setItem(row, 6, new QTableWidgetItem(query.value(6).toString()));
        
        row++;
    }
    
    qDebug() << "Données chargées:" << row << "lignes";
    chargerDatesStockCalendrier();
}

void Stock::viderChamps()
{
    ui->lineEditIdStock->clear();
    ui->comboTypeHuile->setCurrentIndex(0);
    ui->spinQuantite->setValue(0);
    ui->spinSeuilAlerte->setValue(0);
    ui->dateEditMaj->setDate(QDate::currentDate());
    ui->lineEditEmplacement->clear();
    ui->comboEtatStock->setCurrentIndex(0);
}

void Stock::remplirChamps(int row)
{
    if (row < 0 || row >= ui->tableStocks->rowCount()) return;
    
    ui->lineEditIdStock->setText(ui->tableStocks->item(row, 0)->text());
    ui->comboTypeHuile->setCurrentText(ui->tableStocks->item(row, 1)->text());
    ui->spinQuantite->setValue(ui->tableStocks->item(row, 2)->text().toDouble());
    ui->spinSeuilAlerte->setValue(ui->tableStocks->item(row, 3)->text().toInt());
    ui->dateEditMaj->setDate(QDate::fromString(ui->tableStocks->item(row, 4)->text(), "dd/MM/yyyy"));
    ui->lineEditEmplacement->setText(ui->tableStocks->item(row, 5)->text());
    ui->comboEtatStock->setCurrentText(ui->tableStocks->item(row, 6)->text());
}

bool Stock::validerChamps()
{
    if (ui->comboTypeHuile->currentText().isEmpty()) {
        afficherMessage("Validation", "Veuillez sélectionner un type d'huile", true);
        return false;
    }
    
    if (ui->lineEditEmplacement->text().trimmed().isEmpty()) {
        afficherMessage("Validation", "Veuillez entrer un emplacement", true);
        return false;
    }
    
    return true;
}

void Stock::afficherMessage(const QString& titre, const QString& message, bool isError)
{
    if (isError) {
        QMessageBox::critical(this, titre, message);
    } else {
        QMessageBox::information(this, titre, message);
    }
}

QString Stock::getEtatStock(double quantite, double seuil)
{
    if (quantite == 0) return "Rupture";
    if (quantite < seuil) return "Faible";
    return "Disponible";
}

// ==================== CRUD OPERATIONS ====================

void Stock::onAjouterClicked()
{
    if (!validerChamps()) return;
    
    if (!connexion || !connexion->isOpen()) {
        afficherMessage("Erreur", "Pas de connexion à la base de données", true);
        return;
    }
    
    QSqlQuery query(connexion->getDatabase());
    query.prepare("INSERT INTO STOCK (TYPE_HUILE, QUANTITE_ACTUELLE, SEUIL_ALERTE, "
                  "DATE_DERNIERE_MISE_A_JOUR, EMPLACEMENT_STOCKAGE, ETAT_STOCK) "
                  "VALUES (:type, :quantite, :seuil, :date, :emplacement, :etat)");
    
    query.bindValue(":type", ui->comboTypeHuile->currentText());
    query.bindValue(":quantite", ui->spinQuantite->value());
    query.bindValue(":seuil", ui->spinSeuilAlerte->value());
    query.bindValue(":date", ui->dateEditMaj->date());
    query.bindValue(":emplacement", ui->lineEditEmplacement->text());
    query.bindValue(":etat", ui->comboEtatStock->currentText());
    
    if (query.exec()) {
        afficherMessage("Succès", "Article ajouté avec succès !");
        chargerDonneesTable();
        viderChamps();
    } else {
        afficherMessage("Erreur", "Erreur lors de l'ajout : " + query.lastError().text(), true);
    }
}

void Stock::onModifierClicked()
{
    if (!validerChamps()) return;
    
    QString id = ui->lineEditIdStock->text();
    if (id.isEmpty()) {
        afficherMessage("Attention", "Veuillez sélectionner un article à modifier", true);
        return;
    }
    
    if (!connexion || !connexion->isOpen()) {
        afficherMessage("Erreur", "Pas de connexion à la base de données", true);
        return;
    }
    
    QSqlQuery query(connexion->getDatabase());
    query.prepare("UPDATE STOCK SET TYPE_HUILE = :type, QUANTITE_ACTUELLE = :quantite, "
                  "SEUIL_ALERTE = :seuil, DATE_DERNIERE_MISE_A_JOUR = :date, "
                  "EMPLACEMENT_STOCKAGE = :emplacement, ETAT_STOCK = :etat "
                  "WHERE ID_STOCK = :id");
    
    query.bindValue(":type", ui->comboTypeHuile->currentText());
    query.bindValue(":quantite", ui->spinQuantite->value());
    query.bindValue(":seuil", ui->spinSeuilAlerte->value());
    query.bindValue(":date", ui->dateEditMaj->date());
    query.bindValue(":emplacement", ui->lineEditEmplacement->text());
    query.bindValue(":etat", ui->comboEtatStock->currentText());
    query.bindValue(":id", id);
    
    if (query.exec()) {
        afficherMessage("Succès", "Article modifié avec succès !");
        chargerDonneesTable();
        viderChamps();
    } else {
        afficherMessage("Erreur", "Erreur lors de la modification : " + query.lastError().text(), true);
    }
}

void Stock::onSupprimerClicked()
{
    QString id = ui->lineEditIdStock->text();
    if (id.isEmpty()) {
        afficherMessage("Attention", "Veuillez sélectionner un article à supprimer", true);
        return;
    }
    
    QMessageBox::StandardButton reply = QMessageBox::question(this, 
        "Confirmation", 
        "Êtes-vous sûr de vouloir supprimer cet article ?",
        QMessageBox::Yes | QMessageBox::No);
    
    if (reply == QMessageBox::No) return;
    
    if (!connexion || !connexion->isOpen()) {
        afficherMessage("Erreur", "Pas de connexion à la base de données", true);
        return;
    }
    
    QSqlQuery query(connexion->getDatabase());
    query.prepare("DELETE FROM STOCK WHERE ID_STOCK = :id");
    query.bindValue(":id", id);
    
    if (query.exec()) {
        afficherMessage("Succès", "Article supprimé avec succès !");
        chargerDonneesTable();
        viderChamps();
    } else {
        afficherMessage("Erreur", "Erreur lors de la suppression : " + query.lastError().text(), true);
    }
}

void Stock::onConsulterClicked()
{
    chargerDonneesTable();
    afficherMessage("Rafraîchissement", "Données actualisées !");
}

// ==================== RECHERCHE ET FILTRES ====================

void Stock::onRechercherClicked()
{
    if (!connexion || !connexion->isOpen()) {
        afficherMessage("Erreur", "Pas de connexion à la base de données", true);
        return;
    }
    
    QString typeRecherche = ui->comboRechercheType->currentText();
    QString etatRecherche = ui->comboRechercheEtat->currentText();
    
    QString sql = "SELECT ID_STOCK, TYPE_HUILE, QUANTITE_ACTUELLE, SEUIL_ALERTE, "
                  "DATE_DERNIERE_MISE_A_JOUR, EMPLACEMENT_STOCKAGE, ETAT_STOCK "
                  "FROM STOCK WHERE 1=1";
    
    if (typeRecherche != "Tous") {
        sql += " AND TYPE_HUILE = :type";
    }
    
    if (etatRecherche != "Tous") {
        sql += " AND ETAT_STOCK = :etat";
    }
    
    sql += " ORDER BY ID_STOCK";
    
    QSqlQuery query(connexion->getDatabase());
    query.prepare(sql);
    
    if (typeRecherche != "Tous") {
        query.bindValue(":type", typeRecherche);
    }
    if (etatRecherche != "Tous") {
        query.bindValue(":etat", etatRecherche);
    }
    
    if (!query.exec()) {
        afficherMessage("Erreur", "Erreur de recherche : " + query.lastError().text(), true);
        return;
    }
    
    // Afficher dans la table des résultats de l'onglet Tri/Recherche
    ui->tableResultatsCombined->clear();
    ui->tableResultatsCombined->setColumnCount(7);
    ui->tableResultatsCombined->setHorizontalHeaderLabels({"ID", "Type d'huile", "Quantité (L)", 
                                                            "Seuil (L)", "Date MAJ", "Emplacement", "État"});
    ui->tableResultatsCombined->setRowCount(0);
    
    int row = 0;
    while (query.next()) {
        ui->tableResultatsCombined->insertRow(row);
        ui->tableResultatsCombined->setItem(row, 0, new QTableWidgetItem(query.value(0).toString()));
        ui->tableResultatsCombined->setItem(row, 1, new QTableWidgetItem(query.value(1).toString()));
        ui->tableResultatsCombined->setItem(row, 2, new QTableWidgetItem(QString::number(query.value(2).toDouble(), 'f', 2)));
        ui->tableResultatsCombined->setItem(row, 3, new QTableWidgetItem(query.value(3).toString()));
        ui->tableResultatsCombined->setItem(row, 4, new QTableWidgetItem(query.value(4).toDate().toString("dd/MM/yyyy")));
        ui->tableResultatsCombined->setItem(row, 5, new QTableWidgetItem(query.value(5).toString()));
        ui->tableResultatsCombined->setItem(row, 6, new QTableWidgetItem(query.value(6).toString()));
        row++;
    }
    
    // Configurer l'apparence de la table
    ui->tableResultatsCombined->horizontalHeader()->setStretchLastSection(true);
    ui->tableResultatsCombined->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableResultatsCombined->setEditTriggers(QAbstractItemView::NoEditTriggers);
    
    afficherMessage("Recherche", QString("✅ %1 résultat(s) trouvé(s) !").arg(row));
}

void Stock::onReinitialiserClicked()
{
    ui->comboRechercheType->setCurrentIndex(0);
    ui->comboRechercheEtat->setCurrentIndex(0);
    ui->tableResultatsCombined->clear();
    ui->tableResultatsCombined->setRowCount(0);
    afficherMessage("Réinitialisation", "Filtres réinitialisés !");
}

// ==================== TRI ====================

void Stock::onAppliquerTriClicked()
{
    if (!connexion || !connexion->isOpen()) {
        afficherMessage("Erreur", "Pas de connexion à la base de données", true);
        return;
    }
    
    QString critere = ui->comboTriCritere->currentText();
    QString colonne = "ID_STOCK";
    
    if (critere == "Type d'huile") colonne = "TYPE_HUILE";
    else if (critere == "Quantité actuelle") colonne = "QUANTITE_ACTUELLE";
    else if (critere == "Seuil d'alerte") colonne = "SEUIL_ALERTE";
    else if (critere == "Date de mise à jour") colonne = "DATE_DERNIERE_MISE_A_JOUR";
    
    QString sql = QString("SELECT ID_STOCK, TYPE_HUILE, QUANTITE_ACTUELLE, SEUIL_ALERTE, "
                         "DATE_DERNIERE_MISE_A_JOUR, EMPLACEMENT_STOCKAGE, ETAT_STOCK "
                         "FROM STOCK ORDER BY %1").arg(colonne);
    
    QSqlQuery query(connexion->getDatabase());
    if (!query.exec(sql)) {
        afficherMessage("Erreur", "Erreur de tri : " + query.lastError().text(), true);
        return;
    }
    
    // Afficher dans la table des résultats
    ui->tableResultatsCombined->clear();
    ui->tableResultatsCombined->setColumnCount(7);
    ui->tableResultatsCombined->setHorizontalHeaderLabels({"ID", "Type d'huile", "Quantité (L)", 
                                                            "Seuil (L)", "Date MAJ", "Emplacement", "État"});
    ui->tableResultatsCombined->setRowCount(0);
    
    int row = 0;
    while (query.next()) {
        ui->tableResultatsCombined->insertRow(row);
        ui->tableResultatsCombined->setItem(row, 0, new QTableWidgetItem(query.value(0).toString()));
        ui->tableResultatsCombined->setItem(row, 1, new QTableWidgetItem(query.value(1).toString()));
        ui->tableResultatsCombined->setItem(row, 2, new QTableWidgetItem(QString::number(query.value(2).toDouble(), 'f', 2)));
        ui->tableResultatsCombined->setItem(row, 3, new QTableWidgetItem(query.value(3).toString()));
        ui->tableResultatsCombined->setItem(row, 4, new QTableWidgetItem(query.value(4).toDate().toString("dd/MM/yyyy")));
        ui->tableResultatsCombined->setItem(row, 5, new QTableWidgetItem(query.value(5).toString()));
        ui->tableResultatsCombined->setItem(row, 6, new QTableWidgetItem(query.value(6).toString()));
        row++;
    }
    
    // Configurer l'apparence de la table
    ui->tableResultatsCombined->horizontalHeader()->setStretchLastSection(true);
    ui->tableResultatsCombined->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableResultatsCombined->setEditTriggers(QAbstractItemView::NoEditTriggers);
    
    afficherMessage("Tri", QString("✅ Données triées par %1 - %2 résultat(s)").arg(critere).arg(row));
}

// ==================== EXPORT ====================

void Stock::onExporterClicked()
{
    if (!ui->tableResultatsCombined || ui->tableResultatsCombined->rowCount() == 0) {
        afficherMessage("Export", "⚠️ Aucune donnée à exporter !\n\nVeuillez d'abord faire une recherche ou un tri.", true);
        return;
    }

    enum class ExportFormat { Pdf, ExcelCsv, WordDoc, Txt };

    ExportFormat format = ExportFormat::Txt;
    QString filtre = "Fichier texte (*.txt)";
    QString extension = "txt";

    if (ui->radioPDF && ui->radioPDF->isChecked()) {
        format = ExportFormat::Pdf;
        filtre = "Fichier PDF (*.pdf)";
        extension = "pdf";
    } else if (ui->radioExcel && ui->radioExcel->isChecked()) {
        format = ExportFormat::ExcelCsv;
        filtre = "Fichier Excel compatible (*.csv)";
        extension = "csv";
    } else if (ui->radioWord && ui->radioWord->isChecked()) {
        format = ExportFormat::WordDoc;
        filtre = "Document Word (*.doc)";
        extension = "doc";
    }

    QString suggested = QString("stock_export_%1.%2")
                            .arg(QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss"))
                            .arg(extension);

    QString fileName = QFileDialog::getSaveFileName(this, "Exporter les résultats", suggested, filtre);
    if (fileName.isEmpty()) {
        return;
    }

    if (!fileName.toLower().endsWith("." + extension)) {
        fileName += "." + extension;
    }

    QStringList headers;
    headers.reserve(ui->tableResultatsCombined->columnCount());
    for (int col = 0; col < ui->tableResultatsCombined->columnCount(); ++col) {
        QTableWidgetItem *headerItem = ui->tableResultatsCombined->horizontalHeaderItem(col);
        headers << (headerItem ? headerItem->text() : QString("Colonne %1").arg(col + 1));
    }

    auto cellText = [this](int row, int col) {
        QTableWidgetItem *item = ui->tableResultatsCombined->item(row, col);
        return item ? item->text() : QString();
    };

    auto csvEscape = [](QString value) {
        if (value.contains('"')) {
            value.replace("\"", "\"\"");
        }
        if (value.contains(';') || value.contains('\n') || value.contains('"')) {
            value = '"' + value + '"';
        }
        return value;
    };

    auto htmlEscape = [](QString value) {
        return value.toHtmlEscaped();
    };

    bool ok = false;
    QString erreur;

    if (format == ExportFormat::ExcelCsv || format == ExportFormat::Txt) {
        QFile file(fileName);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            afficherMessage("Erreur", "Impossible d'ouvrir le fichier pour l'export", true);
            return;
        }

        QTextStream out(&file);
        out << QChar(0xFEFF);

        const QString delimiter = (format == ExportFormat::ExcelCsv) ? ";" : "\t";
        for (int i = 0; i < headers.size(); ++i) {
            out << ((format == ExportFormat::ExcelCsv) ? csvEscape(headers[i]) : headers[i]);
            if (i < headers.size() - 1) {
                out << delimiter;
            }
        }
        out << "\n";

        for (int row = 0; row < ui->tableResultatsCombined->rowCount(); ++row) {
            for (int col = 0; col < ui->tableResultatsCombined->columnCount(); ++col) {
                QString value = cellText(row, col);
                out << ((format == ExportFormat::ExcelCsv) ? csvEscape(value) : value);
                if (col < ui->tableResultatsCombined->columnCount() - 1) {
                    out << delimiter;
                }
            }
            out << "\n";
        }

        file.close();
        ok = true;
    } else {
        QString html;
        html += "<html><head><meta charset='utf-8'>";
        html += "<style>body{font-family:Segoe UI,Arial,sans-serif;}";
        html += "h2{color:#0f5132;}";
        html += "table{border-collapse:collapse;width:100%;}";
        html += "th,td{border:1px solid #cfd8dc;padding:6px;font-size:11pt;}";
        html += "th{background:#14532d;color:white;}</style></head><body>";
        html += QString("<h2>Export Stock - %1</h2>").arg(QDateTime::currentDateTime().toString("dd/MM/yyyy HH:mm"));
        html += "<table><tr>";
        for (const QString &h : headers) {
            html += QString("<th>%1</th>").arg(htmlEscape(h));
        }
        html += "</tr>";

        for (int row = 0; row < ui->tableResultatsCombined->rowCount(); ++row) {
            html += "<tr>";
            for (int col = 0; col < ui->tableResultatsCombined->columnCount(); ++col) {
                html += QString("<td>%1</td>").arg(htmlEscape(cellText(row, col)));
            }
            html += "</tr>";
        }
        html += "</table></body></html>";

        if (format == ExportFormat::WordDoc) {
            QFile file(fileName);
            if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
                afficherMessage("Erreur", "Impossible d'ouvrir le fichier pour l'export", true);
                return;
            }

            QTextStream out(&file);
            out << QChar(0xFEFF) << html;
            file.close();
            ok = true;
        } else if (format == ExportFormat::Pdf) {
            QPdfWriter writer(fileName);
            writer.setPageSize(QPageSize(QPageSize::A4));
            writer.setResolution(300);

            QTextDocument doc;
            doc.setHtml(html);
            doc.print(&writer);
            ok = true;
        }
    }

    if (!ok) {
        afficherMessage("Erreur", erreur.isEmpty() ? "Export échoué." : erreur, true);
        return;
    }

    afficherMessage("Succès", QString("✅ Export réussi !\n\n%1 ligne(s) exportée(s) vers :\n%2")
                                 .arg(ui->tableResultatsCombined->rowCount())
                                 .arg(fileName));
}

// ==================== STATISTIQUES ====================

void Stock::onCalculerClicked()
{
    if (!connexion || !connexion->isOpen()) {
        afficherMessage("Erreur", "Pas de connexion à la base de données", true);
        return;
    }

    QString critere = ui->comboStatsPar->currentText();
    QString colonne = "TYPE_HUILE";

    if (critere == "État du stock" || critere == "État") {
        colonne = "ETAT_STOCK";
    } else if (critere == "Emplacement") {
        colonne = "EMPLACEMENT_STOCKAGE";
    }

    QSqlQuery totalQuery(connexion->getDatabase());
    if (totalQuery.exec("SELECT COUNT(*), NVL(SUM(QUANTITE_ACTUELLE),0) FROM STOCK") && totalQuery.next()) {
        if (ui->labelTotalArticles) {
            ui->labelTotalArticles->setText(QString("Total d'articles : %1").arg(totalQuery.value(0).toInt()));
        }
        if (ui->labelValeurTotale) {
            ui->labelValeurTotale->setText(QString("Volume total : %1 L").arg(QString::number(totalQuery.value(1).toDouble(), 'f', 2)));
        }
    }

    QSqlQuery query(connexion->getDatabase());
    const QString sql = QString(
        "SELECT %1, COUNT(*) as NB, NVL(SUM(QUANTITE_ACTUELLE),0) as TOTAL "
        "FROM STOCK "
        "GROUP BY %1 "
        "ORDER BY NB DESC, TOTAL DESC").arg(colonne);

    if (!query.exec(sql)) {
        afficherMessage("Erreur", "Erreur statistiques : " + query.lastError().text(), true);
        return;
    }

    QStringList categories;
    QList<qreal> counts;
    QList<qreal> totals;

    ui->tableResultatsCombined->clear();
    ui->tableResultatsCombined->setColumnCount(3);
    ui->tableResultatsCombined->setHorizontalHeaderLabels({critere, "Nombre", "Total (L)"});
    ui->tableResultatsCombined->setRowCount(0);

    int row = 0;
    while (query.next()) {
        QString label = query.value(0).toString().trimmed();
        if (label.isEmpty()) {
            label = "Non spécifié";
        }

        const qreal nb = query.value(1).toDouble();
        const qreal totalL = query.value(2).toDouble();

        categories << label;
        counts << nb;
        totals << totalL;

        ui->tableResultatsCombined->insertRow(row);
        ui->tableResultatsCombined->setItem(row, 0, new QTableWidgetItem(label));
        ui->tableResultatsCombined->setItem(row, 1, new QTableWidgetItem(QString::number(nb, 'f', 0)));
        ui->tableResultatsCombined->setItem(row, 2, new QTableWidgetItem(QString::number(totalL, 'f', 2)));
        ++row;
    }

    ui->tableResultatsCombined->horizontalHeader()->setStretchLastSection(true);
    ui->tableResultatsCombined->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableResultatsCombined->setEditTriggers(QAbstractItemView::NoEditTriggers);

    if (row == 0) {
        if (ui->textResumeStats) {
            ui->textResumeStats->setPlainText("Aucune donnée disponible pour les statistiques.");
        }
        afficherMessage("Statistiques", "Aucune donnée à afficher.", true);
        return;
    }

    QString resume;
    resume += QString("Statistiques par %1\n").arg(critere);
    resume += QString("Nombre de catégories: %1\n\n").arg(row);
    for (int i = 0; i < row; ++i) {
        resume += QString("- %1 : %2 article(s), %3 L\n")
                      .arg(categories.at(i))
                      .arg(QString::number(counts.at(i), 'f', 0))
                      .arg(QString::number(totals.at(i), 'f', 2));
    }
    if (ui->textResumeStats) {
        ui->textResumeStats->setPlainText(resume);
    }

    const QString typeGraphique = ui->comboTypeGraphique ? ui->comboTypeGraphique->currentText() : "Barres";

    QChartView *chartView = ui->groupStats->findChild<QChartView*>("statsChartView");
    if (!chartView) {
        chartView = new QChartView(ui->groupStats);
        chartView->setObjectName("statsChartView");
        chartView->setRenderHint(QPainter::Antialiasing, true);
        chartView->setMinimumHeight(260);
        ui->layoutStats->addWidget(chartView);
    }

    auto *chart = new QChart();
    chart->setTitle(QString("Distribution des stocks par %1 (%2)").arg(critere, typeGraphique));
    chart->setAnimationOptions(QChart::SeriesAnimations);
    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);
    chart->setBackgroundBrush(QBrush(QColor("#ffffff")));

    qreal maxY = 0.0;
    for (int i = 0; i < row; ++i) {
        maxY = qMax(maxY, qMax(counts.at(i), totals.at(i)));
    }

    if (typeGraphique == "Camembert") {
        auto *series = new QPieSeries();
        for (int i = 0; i < row; ++i) {
            const qreal value = totals.at(i) > 0.0 ? totals.at(i) : counts.at(i);
            if (value > 0.0) {
                series->append(categories.at(i), value);
            }
        }

        if (series->slices().isEmpty()) {
            for (int i = 0; i < row; ++i) {
                series->append(categories.at(i), counts.at(i));
            }
        }

        chart->addSeries(series);
    } else if (typeGraphique == "Courbe") {
        auto *countSeries = new QLineSeries();
        countSeries->setName("Nombre");
        countSeries->setColor(QColor("#1f6f55"));

        auto *volumeSeries = new QLineSeries();
        volumeSeries->setName("Volume (L)");
        volumeSeries->setColor(QColor("#3b82f6"));

        for (int i = 0; i < row; ++i) {
            countSeries->append(i, counts.at(i));
            volumeSeries->append(i, totals.at(i));
        }

        chart->addSeries(countSeries);
        chart->addSeries(volumeSeries);

        auto *axisX = new QCategoryAxis();
        for (int i = 0; i < row; ++i) {
            axisX->append(categories.at(i), i);
        }
        axisX->setRange(0, qMax(1, row - 1));
        axisX->setLabelsPosition(QCategoryAxis::AxisLabelsPositionOnValue);
        chart->addAxis(axisX, Qt::AlignBottom);
        countSeries->attachAxis(axisX);
        volumeSeries->attachAxis(axisX);

        auto *axisY = new QValueAxis();
        axisY->setLabelFormat("%.0f");
        axisY->setRange(0.0, qMax<qreal>(5.0, maxY * 1.2));
        axisY->setTitleText("Valeur");
        chart->addAxis(axisY, Qt::AlignLeft);
        countSeries->attachAxis(axisY);
        volumeSeries->attachAxis(axisY);
    } else {
        auto *countSet = new QBarSet("Nombre");
        auto *volumeSet = new QBarSet("Volume (L)");
        auto *histSet = new QBarSet("Histogramme (L)");

        for (int i = 0; i < row; ++i) {
            *countSet << counts.at(i);
            *volumeSet << totals.at(i);
            *histSet << totals.at(i);
        }

        countSet->setColor(QColor("#1f6f55"));
        volumeSet->setColor(QColor("#3b82f6"));
        histSet->setColor(QColor("#d97706"));

        auto *series = new QBarSeries();
        if (typeGraphique == "Histogramme") {
            series->append(histSet);
        } else {
            series->append(countSet);
            series->append(volumeSet);
        }

        chart->addSeries(series);

        auto *axisX = new QBarCategoryAxis();
        axisX->append(categories);
        chart->addAxis(axisX, Qt::AlignBottom);
        series->attachAxis(axisX);

        auto *axisY = new QValueAxis();
        axisY->setLabelFormat("%.0f");
        axisY->setRange(0.0, qMax<qreal>(5.0, maxY * 1.2));
        axisY->setTitleText("Valeur");
        chart->addAxis(axisY, Qt::AlignLeft);
        series->attachAxis(axisY);
    }

    chartView->setChart(chart);
    chartView->show();

    afficherMessage("Statistiques", QString("Statistiques calculées et graphique %1 affiché.").arg(typeGraphique.toLower()));
}

// ==================== ALERTES ====================

void Stock::onVerifierAlertesClicked()
{
    if (!connexion || !connexion->isOpen()) {
        afficherMessage("Erreur", "Pas de connexion à la base de données", true);
        return;
    }

    if (ui->listStocksSousSeuil) {
        ui->listStocksSousSeuil->clear();
    }

    QSqlQuery query(connexion->getDatabase());
    query.prepare("SELECT TYPE_HUILE, QUANTITE_ACTUELLE, SEUIL_ALERTE, EMPLACEMENT_STOCKAGE "
                  "FROM STOCK "
                  "WHERE QUANTITE_ACTUELLE <= SEUIL_ALERTE OR QUANTITE_ACTUELLE = 0 "
                  "ORDER BY CASE WHEN QUANTITE_ACTUELLE = 0 THEN 0 ELSE 1 END, QUANTITE_ACTUELLE ASC");

    if (!query.exec()) {
        afficherMessage("Alertes", "Erreur lors de la vérification : " + query.lastError().text(), true);
        return;
    }

    int count = 0;
    int rupture = 0;
    int faible = 0;
    QStringList lignesRapides;

    while (query.next()) {
        ++count;
        const QString type = query.value(0).toString();
        const double quantite = query.value(1).toDouble();
        const double seuil = query.value(2).toDouble();
        const QString emplacement = query.value(3).toString();

        QString line;
        QColor lineColor("#92400e");
        if (quantite <= 0.0) {
            ++rupture;
            line = QString("⛔ %1 | %2 L (seuil %3 L) | %4")
                       .arg(type)
                       .arg(QString::number(quantite, 'f', 2))
                       .arg(QString::number(seuil, 'f', 2))
                       .arg(emplacement);
            lineColor = QColor("#991b1b");
        } else {
            ++faible;
            line = QString("⚠️ %1 | %2 L (seuil %3 L) | %4")
                       .arg(type)
                       .arg(QString::number(quantite, 'f', 2))
                       .arg(QString::number(seuil, 'f', 2))
                       .arg(emplacement);
            lineColor = QColor("#92400e");
        }

        lignesRapides << line;
        if (ui->listStocksSousSeuil) {
            auto *item = new QListWidgetItem(line);
            item->setForeground(QBrush(lineColor));
            ui->listStocksSousSeuil->addItem(item);
        }
    }

    if (ui->listStocksSousSeuil && ui->listStocksSousSeuil->count() == 0) {
        auto *item = new QListWidgetItem("✅ Aucun stock sous le seuil d'alerte.");
        item->setForeground(QBrush(QColor("#166534")));
        ui->listStocksSousSeuil->addItem(item);
    }

    const QString resume = (count == 0)
        ? "Alerte: aucun produit critique actuellement."
        : QString("Alerte: %1 produit(s) critique(s) dont %2 rupture(s) et %3 faible(s).")
              .arg(count)
              .arg(rupture)
              .arg(faible);

    if (ui->textAnalyseTendances && ui->textAnalyseTendances->toPlainText().trimmed().isEmpty()) {
        ui->textAnalyseTendances->setPlainText(resume + "\n\nDétail:\n- " + lignesRapides.join("\n- "));
    }

    afficherMessage("Alertes", resume);
}

void Stock::onAnalyserTendancesClicked()
{
    if (!connexion || !connexion->isOpen()) {
        afficherMessage("Erreur", "Pas de connexion à la base de données", true);
        return;
    }

    int periodDays = 30;
    const QString periode = ui->comboPeriode->currentText();
    if (periode.contains("7")) {
        periodDays = 7;
    } else if (periode.contains("90") || periode.contains("3")) {
        periodDays = 90;
    }

    const QDate startDate = QDate::currentDate().addDays(-periodDays);

    QSqlQuery query(connexion->getDatabase());
    query.prepare(
        "SELECT s.TYPE_HUILE, s.QUANTITE_ACTUELLE, s.SEUIL_ALERTE, "
        "       NVL(SUM(CASE WHEN p.DATEPRODUCTION >= :startDate THEN p.HUILEPRODUITEL ELSE 0 END), 0) AS PROD_PERIODE, "
        "       NVL(COUNT(CASE WHEN p.DATEPRODUCTION >= :startDate THEN 1 END), 0) AS NB_OPERATIONS "
        "FROM STOCK s "
        "LEFT JOIN PRODUCTION p ON p.ID_STOCK = s.ID_STOCK "
        "GROUP BY s.TYPE_HUILE, s.QUANTITE_ACTUELLE, s.SEUIL_ALERTE "
        "ORDER BY PROD_PERIODE DESC, s.TYPE_HUILE");
    query.bindValue(":startDate", startDate);

    if (!query.exec()) {
        afficherMessage("Erreur", "Erreur d'analyse prédictive : " + query.lastError().text(), true);
        return;
    }

    struct OilAnalysis {
        QString type;
        double stockCurrent;
        double threshold;
        double producedPeriod;
        int operations;
        double popularityPct;
        double predicted7;
        double predicted30;
        int riskPct;
        int reorderQty;
        int productionQty;
        QString priority;
    };

    QList<OilAnalysis> analyses;
    double totalProduced = 0.0;

    while (query.next()) {
        OilAnalysis item;
        item.type = query.value(0).toString();
        item.stockCurrent = query.value(1).toDouble();
        item.threshold = query.value(2).toDouble();
        item.producedPeriod = query.value(3).toDouble();
        item.operations = query.value(4).toInt();
        item.popularityPct = 0.0;
        item.predicted7 = 0.0;
        item.predicted30 = 0.0;
        item.riskPct = 0;
        item.reorderQty = 0;
        item.productionQty = 0;
        item.priority = "Basse";

        totalProduced += item.producedPeriod;
        analyses.append(item);
    }

    if (analyses.isEmpty()) {
        afficherMessage("Analyse", "Aucune donnée disponible pour l'analyse.", true);
        return;
    }

    for (OilAnalysis &item : analyses) {
        const double avgDailyFromProd = item.producedPeriod / qMax(1, periodDays);
        const double baselineDemand = qMax(1.0, item.threshold / 30.0);
        const double estimatedDailyDemand = qMax(avgDailyFromProd, baselineDemand);

        item.popularityPct = (totalProduced > 0.0)
            ? (item.producedPeriod / totalProduced) * 100.0
            : (100.0 / analyses.size());

        item.predicted7 = estimatedDailyDemand * 7.0;
        item.predicted30 = estimatedDailyDemand * 30.0;

        double stockCoverageDays = item.stockCurrent / estimatedDailyDemand;
        int risk = 15;
        if (stockCoverageDays <= 3.0) {
            risk = 90;
        } else if (stockCoverageDays <= 7.0) {
            risk = 70;
        } else if (stockCoverageDays <= 14.0) {
            risk = 45;
        }

        if (item.threshold > 0.0 && item.stockCurrent <= (item.threshold * 0.5)) {
            risk = qMax(risk, 95);
        } else if (item.threshold > 0.0 && item.stockCurrent <= item.threshold) {
            risk = qMax(risk, 75);
        }

        item.riskPct = risk;

        const double securityStock = qMax(10.0, item.threshold * 0.6);
        item.reorderQty = qMax(0, static_cast<int>(item.predicted30 + securityStock - item.stockCurrent));
        item.productionQty = qMax(0, static_cast<int>(item.predicted7 + (item.popularityPct > 35.0 ? item.predicted7 * 0.2 : 0.0) - item.stockCurrent * 0.25));

        if (item.riskPct >= 80 || item.popularityPct >= 40.0) {
            item.priority = "Haute";
        } else if (item.riskPct >= 50 || item.popularityPct >= 20.0) {
            item.priority = "Moyenne";
        }
    }

    std::sort(analyses.begin(), analyses.end(), [](const OilAnalysis &a, const OilAnalysis &b) {
        if (a.priority != b.priority) {
            if (a.priority == "Haute") return true;
            if (b.priority == "Haute") return false;
            if (a.priority == "Moyenne") return true;
            if (b.priority == "Moyenne") return false;
        }
        if (a.riskPct != b.riskPct) {
            return a.riskPct > b.riskPct;
        }
        return a.popularityPct > b.popularityPct;
    });

    double totalRisk = 0.0;
    int totalReorder = 0;
    for (const OilAnalysis &item : analyses) {
        totalRisk += item.riskPct;
        totalReorder += item.reorderQty;
    }

    const double avgRisk = analyses.isEmpty() ? 0.0 : totalRisk / analyses.size();
    const QString topStrategic = analyses.isEmpty()
        ? QString("--")
        : QString("%1 (%2%)")
              .arg(analyses.first().type)
              .arg(QString::number(analyses.first().popularityPct, 'f', 1));

    if (ui->labelTopStrategiqueValue) {
        ui->labelTopStrategiqueValue->setText(topStrategic);
    }
    if (ui->labelRisqueMoyenGlobalValue) {
        ui->labelRisqueMoyenGlobalValue->setText(QString("%1 %").arg(QString::number(avgRisk, 'f', 1)));
    }
    if (ui->labelBesoinReapproTotalValue) {
        ui->labelBesoinReapproTotalValue->setText(QString("%1 L").arg(totalReorder));
    }

    ui->tableResultatsCombined->clear();
    ui->tableResultatsCombined->setColumnCount(10);
    ui->tableResultatsCombined->setHorizontalHeaderLabels({
        "Type d'huile", "Stock (L)", "Popularité (%)", "Prévision J+7 (L)",
        "Prévision J+30 (L)", "Risque (%)", "Réappro (L)",
        "Production (L)", "Opérations", "Priorité"
    });
    ui->tableResultatsCombined->setRowCount(0);
    ui->tableResultatsCombined->setWordWrap(false);
    ui->tableResultatsCombined->setAlternatingRowColors(true);
    ui->tableResultatsCombined->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    ui->tableResultatsCombined->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    ui->tableResultatsCombined->verticalHeader()->setDefaultSectionSize(32);

    const QList<int> predictiveColumnWidths = {190, 100, 120, 130, 140, 95, 110, 115, 95, 100};
    for (int col = 0; col < predictiveColumnWidths.size() && col < ui->tableResultatsCombined->columnCount(); ++col) {
        ui->tableResultatsCombined->setColumnWidth(col, predictiveColumnWidths[col]);
    }

    for (int col = 0; col < ui->tableResultatsCombined->columnCount(); ++col) {
        QTableWidgetItem* headerItem = ui->tableResultatsCombined->horizontalHeaderItem(col);
        if (headerItem) {
            headerItem->setTextAlignment(Qt::AlignCenter);
        }
    }

    int row = 0;
    for (const OilAnalysis &item : analyses) {
        ui->tableResultatsCombined->insertRow(row);
        ui->tableResultatsCombined->setItem(row, 0, new QTableWidgetItem(item.type));
        ui->tableResultatsCombined->setItem(row, 1, new QTableWidgetItem(QString::number(item.stockCurrent, 'f', 2)));
        ui->tableResultatsCombined->setItem(row, 2, new QTableWidgetItem(QString::number(item.popularityPct, 'f', 1)));
        ui->tableResultatsCombined->setItem(row, 3, new QTableWidgetItem(QString::number(item.predicted7, 'f', 1)));
        ui->tableResultatsCombined->setItem(row, 4, new QTableWidgetItem(QString::number(item.predicted30, 'f', 1)));
        ui->tableResultatsCombined->setItem(row, 5, new QTableWidgetItem(QString::number(item.riskPct)));
        ui->tableResultatsCombined->setItem(row, 6, new QTableWidgetItem(QString::number(item.reorderQty)));
        ui->tableResultatsCombined->setItem(row, 7, new QTableWidgetItem(QString::number(item.productionQty)));
        ui->tableResultatsCombined->setItem(row, 8, new QTableWidgetItem(QString::number(item.operations)));
        ui->tableResultatsCombined->setItem(row, 9, new QTableWidgetItem(item.priority));

        QColor bgColor("#dcfce7");
        QColor fgColor("#14532d");
        if (item.priority == "Haute") {
            bgColor = QColor("#fee2e2");
            fgColor = QColor("#991b1b");
        } else if (item.priority == "Moyenne") {
            bgColor = QColor("#ffedd5");
            fgColor = QColor("#92400e");
        }

        for (int col = 0; col < ui->tableResultatsCombined->columnCount(); ++col) {
            QTableWidgetItem *cell = ui->tableResultatsCombined->item(row, col);
            if (cell) {
                cell->setBackground(QBrush(bgColor));
                cell->setForeground(QBrush(fgColor));
                if (col == 0) {
                    cell->setTextAlignment(Qt::AlignVCenter | Qt::AlignLeft);
                } else {
                    cell->setTextAlignment(Qt::AlignCenter);
                }
            }
        }

        row++;
    }

    ui->tableResultatsCombined->horizontalHeader()->setStretchLastSection(false);
    ui->tableResultatsCombined->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableResultatsCombined->setEditTriggers(QAbstractItemView::NoEditTriggers);

    QString recommendations;
    recommendations += "ANALYSE PREDICTIVE & PRESCRIPTIVE DES TYPES D'HUILE\n";
    recommendations += "Periode: " + periode + "\n";
    recommendations += "Total production analysee: " + QString::number(totalProduced, 'f', 2) + " L\n\n";
    recommendations += "TOP RECOMMANDATIONS:\n";

    const int topN = qMin(3, analyses.size());
    for (int i = 0; i < topN; ++i) {
        const OilAnalysis &item = analyses[i];
        recommendations += QString("%1) %2\n")
            .arg(i + 1)
            .arg(item.type);
        recommendations += QString("   - Popularite: %1% | Risque rupture: %2%\n")
            .arg(QString::number(item.popularityPct, 'f', 1))
            .arg(item.riskPct);
        recommendations += QString("   - Produire: %1 L | Reappro: %2 L\n")
            .arg(item.productionQty)
            .arg(item.reorderQty);
        recommendations += QString("   - Priorite: %1\n\n")
            .arg(item.priority);
    }

    recommendations += "INTERPRETATION:\n";
    recommendations += "- Priorite Haute: action immediate recommandee.\n";
    recommendations += "- Priorite Moyenne: planifier cette semaine.\n";
    recommendations += "- Priorite Basse: suivi normal.\n";

    if (ui->textAnalyseTendances) {
        ui->textAnalyseTendances->setPlainText(recommendations);
    }

    if (ui->layoutGraphique) {
        if (ui->labelPlaceholderChart) {
            ui->labelPlaceholderChart->hide();
        }

        QChartView *trendChartView = ui->groupGraphique->findChild<QChartView*>("trendChartView");
        if (!trendChartView) {
            trendChartView = new QChartView(ui->groupGraphique);
            trendChartView->setObjectName("trendChartView");
            trendChartView->setRenderHint(QPainter::Antialiasing, true);
            trendChartView->setMinimumHeight(260);
            trendChartView->setStyleSheet(
                "QChartView {"
                "  background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #ffffff, stop:1 #f3faf6);"
                "  border: 1px solid #d7e8db;"
                "  border-radius: 12px;"
                "  padding: 6px;"
                "}");
            ui->layoutGraphique->addWidget(trendChartView);
        }

        auto *pred7 = new QBarSet("Prévision J+7");
        auto *pred30 = new QBarSet("Prévision J+30");
        QStringList categories;
        qreal maxY = 0.0;

        const int maxBars = qMin(8, analyses.size());
        for (int i = 0; i < maxBars; ++i) {
            const OilAnalysis &item = analyses.at(i);
            categories << item.type;
            *pred7 << item.predicted7;
            *pred30 << item.predicted30;
            maxY = qMax(maxY, qMax(item.predicted7, item.predicted30));
        }

        pred7->setBrush(QBrush(QColor("#1f6f55")));
        pred7->setBorderColor(QColor("#14532d"));
        pred30->setBrush(QBrush(QColor("#3b82f6")));
        pred30->setBorderColor(QColor("#1d4ed8"));
        pred7->setLabelColor(QColor("#14532d"));
        pred30->setLabelColor(QColor("#1d4ed8"));

        auto *series = new QBarSeries();
        series->append(pred7);
        series->append(pred30);
        series->setBarWidth(0.72);

        auto *chart = new QChart();
        chart->addSeries(series);
        chart->setTitle(QString("Prévision de demande (%1)").arg(periode));
        chart->setAnimationOptions(QChart::AllAnimations);
        chart->setAnimationDuration(900);
        chart->setTheme(QChart::ChartThemeLight);
        chart->legend()->setVisible(true);
        chart->legend()->setAlignment(Qt::AlignBottom);
        chart->legend()->setLabelColor(QColor("#1e3a32"));
        chart->setBackgroundVisible(false);
        chart->setPlotAreaBackgroundVisible(true);
        chart->setPlotAreaBackgroundBrush(QBrush(QColor("#f8fcfa")));
        chart->setMargins(QMargins(14, 16, 14, 16));

        QFont titleFont("Segoe UI", 11, QFont::Bold);
        chart->setTitleFont(titleFont);
        chart->setTitleBrush(QBrush(QColor("#14532d")));

        auto *axisX = new QBarCategoryAxis();
        axisX->append(categories);
        axisX->setLabelsAngle(-22);
        axisX->setLabelsColor(QColor("#1e3a32"));
        QFont axisXFont("Segoe UI", 9, QFont::DemiBold);
        axisX->setLabelsFont(axisXFont);
        chart->addAxis(axisX, Qt::AlignBottom);
        series->attachAxis(axisX);

        auto *axisY = new QValueAxis();
        axisY->setLabelFormat("%.0f");
        axisY->setRange(0.0, qMax<qreal>(10.0, maxY * 1.2));
        axisY->setTitleText("Litres");
        axisY->setLabelsColor(QColor("#1e3a32"));
        axisY->setTitleBrush(QBrush(QColor("#14532d")));
        axisY->setGridLineColor(QColor("#d6e9dd"));
        axisY->setMinorGridLineColor(QColor("#eaf5ee"));
        QFont axisYFont("Segoe UI", 9);
        axisY->setLabelsFont(axisYFont);
        chart->addAxis(axisY, Qt::AlignLeft);
        series->attachAxis(axisY);

        trendChartView->setChart(chart);
    }

    afficherMessage("Analyse", "Analyse prédictive et recommandations générées avec succès !");
}

void Stock::onAnalyserPredictifHuileClicked()
{
    if (!connexion || !connexion->isOpen()) {
        afficherMessage("Erreur", "Pas de connexion a la base de donnees", true);
        return;
    }

    int periodDays = 30;
    const QString periode = ui->comboPeriodePredictifHuile
                                ? ui->comboPeriodePredictifHuile->currentText()
                                : QString("30 derniers jours");
    if (periode.contains("7")) {
        periodDays = 7;
    } else if (periode.contains("90") || periode.contains("3")) {
        periodDays = 90;
    }

    const QDate startDate = QDate::currentDate().addDays(-periodDays);
    QSqlQuery query(connexion->getDatabase());
    query.prepare(
        "SELECT s.TYPE_HUILE, s.QUANTITE_ACTUELLE, s.SEUIL_ALERTE, "
        "       NVL(SUM(CASE WHEN p.DATEPRODUCTION >= :startDate THEN p.HUILEPRODUITEL ELSE 0 END), 0) AS PROD_PERIODE, "
        "       MAX(s.DATE_DERNIERE_MISE_A_JOUR) AS LAST_UPDATE, "
        "       NVL(s.FOURNISSEUR_NOM, ''), NVL(s.FOURNISSEUR_EMAIL, ''), NVL(s.FOURNISSEUR_TEL, ''), "
        "       MAX(s.DATE_DERNIERE_COMMANDE) AS LAST_ORDER "
        "FROM STOCK s "
        "LEFT JOIN PRODUCTION p ON p.ID_STOCK = s.ID_STOCK "
        "GROUP BY s.TYPE_HUILE, s.QUANTITE_ACTUELLE, s.SEUIL_ALERTE, s.FOURNISSEUR_NOM, s.FOURNISSEUR_EMAIL, s.FOURNISSEUR_TEL "
        "ORDER BY s.TYPE_HUILE");
    query.bindValue(":startDate", startDate);

    if (!query.exec()) {
        afficherMessage("Erreur", "Erreur analyse recommandations : " + query.lastError().text(), true);
        return;
    }

    struct ExpertRecommendation {
        QString type;
        double stock = 0.0;
        double seuil = 0.0;
        double prodPeriode = 0.0;
        QDate lastUpdate;
        QDate lastOrderDate;
        QString fournisseur;
        QString fournisseurEmail;
        QString fournisseurTel;
        double demandJ7 = 0.0;
        double demandJ30 = 0.0;
        double coverageDays = 0.0;
        int risk = 0;
        int optimalQty = 0;
        int idealDelayDays = 0;
        QString confidence;
        QString why;
        QString recommendationLine;
    };

    QList<ExpertRecommendation> rows;

    while (query.next()) {
        ExpertRecommendation item;
        item.type = query.value(0).toString();
        item.stock = query.value(1).toDouble();
        item.seuil = query.value(2).toDouble();
        item.prodPeriode = query.value(3).toDouble();
        item.lastUpdate = query.value(4).toDateTime().date();
        item.fournisseur = query.value(5).toString().trimmed();
        item.fournisseurEmail = query.value(6).toString().trimmed();
        item.fournisseurTel = query.value(7).toString().trimmed();
        item.lastOrderDate = query.value(8).toDateTime().date();

        const double avgDailyFromProd = item.prodPeriode / qMax(1, periodDays);
        const double baselineDaily = (item.seuil > 0.0) ? (item.seuil / 30.0) : 0.5;
        const double estimatedDailyDemand = qMax(0.5, qMax(avgDailyFromProd, baselineDaily));

        item.demandJ7 = estimatedDailyDemand * 7.0;
        item.demandJ30 = estimatedDailyDemand * 30.0;
        item.coverageDays = item.stock / qMax(0.1, estimatedDailyDemand);

        const double securityStock = qMax(10.0, item.seuil * 0.7);
        item.optimalQty = qMax(0, qRound(item.demandJ30 + securityStock - item.stock));

        if (item.coverageDays <= 2.0) {
            item.idealDelayDays = 1;
        } else if (item.coverageDays <= 5.0) {
            item.idealDelayDays = 2;
        } else if (item.coverageDays <= 10.0) {
            item.idealDelayDays = 4;
        } else {
            item.idealDelayDays = 7;
        }

        int freshnessRisk = 25;
        int daysSinceUpdate = 30;
        if (item.lastUpdate.isValid()) {
            daysSinceUpdate = qAbs(item.lastUpdate.daysTo(QDate::currentDate()));
            if (daysSinceUpdate <= 2) {
                freshnessRisk = 5;
            } else if (daysSinceUpdate <= 7) {
                freshnessRisk = 12;
            } else if (daysSinceUpdate <= 14) {
                freshnessRisk = 20;
            } else {
                freshnessRisk = 35;
            }
        }

        int stockRisk = 20;
        if (item.coverageDays <= 2.0) {
            stockRisk = 95;
        } else if (item.coverageDays <= 5.0) {
            stockRisk = 75;
        } else if (item.coverageDays <= 10.0) {
            stockRisk = 55;
        } else {
            stockRisk = 25;
        }

        item.risk = std::clamp(static_cast<int>(0.7 * stockRisk + 0.3 * freshnessRisk), 0, 100);

        const bool hasFullContact = !item.fournisseur.isEmpty() && !item.fournisseurEmail.isEmpty() && !item.fournisseurTel.isEmpty();
        if (!hasFullContact) {
            item.confidence = "Faible";
        } else if (daysSinceUpdate <= 7) {
            item.confidence = "Elevee";
        } else {
            item.confidence = "Moyenne";
        }

        const QString supplier = item.fournisseur.isEmpty() ? "Fournisseur a definir" : item.fournisseur;
        item.recommendationLine = QString("Commander %1 L chez %2 avant %3 jour(s)")
                                      .arg(item.optimalQty)
                                      .arg(supplier)
                                      .arg(item.idealDelayDays);

        item.why = QString("Couverture: %1 j | Demande J+30: %2 L | Risque: %3% | Confiance: %4")
                       .arg(QString::number(item.coverageDays, 'f', 1))
                       .arg(QString::number(item.demandJ30, 'f', 1))
                       .arg(item.risk)
                       .arg(item.confidence);

        rows.append(item);
    }

    if (rows.isEmpty()) {
        afficherMessage("Analyse", "Aucune donnee disponible pour les recommandations.", true);
        return;
    }

    std::sort(rows.begin(), rows.end(), [](const ExpertRecommendation &a, const ExpertRecommendation &b) {
        if (a.risk != b.risk) {
            return a.risk > b.risk;
        }
        return a.optimalQty > b.optimalQty;
    });

    if (ui->tableResultatsPredictifHuile) {
        ui->tableResultatsPredictifHuile->hide();
    }
    if (ui->groupResultatsPredictiveHuile) {
        ui->groupResultatsPredictiveHuile->hide();
    }

    int totalRisk = 0;
    int totalOptimalOrder = 0;
    for (const ExpertRecommendation &item : rows) {
        totalRisk += item.risk;
        totalOptimalOrder += item.optimalQty;
    }

    const int avgRisk = rows.isEmpty() ? 0 : qRound(static_cast<double>(totalRisk) / rows.size());
    const ExpertRecommendation &priorityOil = rows.first();
    const QString topSupplier = priorityOil.fournisseur.isEmpty() ? "Fournisseur a definir" : priorityOil.fournisseur;

    if (ui->labelTopStrategiqueValue2) {
        ui->labelTopStrategiqueValue2->setText(
            QString("Priorite: Commander %1 L de %2").arg(priorityOil.optimalQty).arg(priorityOil.type));
    }
    if (ui->labelRisqueMoyenGlobalValue2) {
        ui->labelRisqueMoyenGlobalValue2->setText(
            QString("Fournisseur recommande: %1 (%2)").arg(topSupplier).arg(priorityOil.confidence));
    }
    if (ui->labelBesoinReapproTotalValue2) {
        ui->labelBesoinReapproTotalValue2->setText(
            QString("Delai ideal: avant %1 jour(s) | Total: %2 L")
                .arg(priorityOil.idealDelayDays)
                .arg(totalOptimalOrder));
    }

    if (ui->textAnalysePredictifHuile) {
        QString report;
        report += "<h3 style='color:#14532d;'>Recommandation optimisee (niveau expert)</h3>";
        report += "<p><b>Periode:</b> " + periode + "<br/>";
        report += "<b>Principe:</b> quantite optimale + fournisseur recommande + delai ideal.</p>";
        report += "<hr/>";

        const int topN = qMin(3, rows.size());
        for (int i = 0; i < topN; ++i) {
            const ExpertRecommendation &item = rows.at(i);
            const QString supplier = item.fournisseur.isEmpty() ? "Fournisseur a definir" : item.fournisseur;
            const QString cardColor = (i == 0) ? "#fee2e2" : ((i == 1) ? "#ffedd5" : "#ecfdf3");
            report += QString(
                "<div style='background:%1;border:1px solid #cbd5e1;border-radius:10px;padding:10px;margin:10px 0;'>"
                "<p style='margin:0 0 8px 0;'><b>%2) %3</b></p>"
                "<p style='margin:0;'><b>Action:</b> Commander %4 L chez %5 avant %6 jour(s)</p>"
                "<p style='margin:4px 0 0 0;'><b>Justification:</b> %7</p>"
                "</div>")
                .arg(cardColor)
                .arg(i + 1)
                .arg(item.type)
                .arg(item.optimalQty)
                .arg(supplier)
                .arg(item.idealDelayDays)
                .arg(item.why);
        }

        report += QString("<p><b>Risque moyen global:</b> %1%% | <b>Volume total suggere:</b> %2 L</p>")
                      .arg(avgRisk)
                      .arg(totalOptimalOrder);

        ui->textAnalysePredictifHuile->setHtml(report);
    }

    afficherMessage("Recommandations", "Recommandations optimisees generees avec succes.");
}

// ==================== RAPPORTS ====================

void Stock::onGenererRapportClicked()
{
    if (!connexion || !connexion->isOpen()) {
        afficherMessage("Erreur", "Pas de connexion à la base de données", true);
        return;
    }

    QSqlQuery query(connexion->getDatabase());
    query.prepare("SELECT ID_STOCK, TYPE_HUILE, QUANTITE_ACTUELLE, SEUIL_ALERTE, DATE_DERNIERE_MISE_A_JOUR, "
                  "EMPLACEMENT_STOCKAGE, ETAT_STOCK "
                  "FROM STOCK ORDER BY DATE_DERNIERE_MISE_A_JOUR DESC, ID_STOCK DESC");

    if (!query.exec()) {
        afficherMessage("Rapport", "Erreur génération rapport : " + query.lastError().text(), true);
        return;
    }

    ui->tableRapportSelection->clear();
    ui->tableRapportSelection->setColumnCount(7);
    ui->tableRapportSelection->setHorizontalHeaderLabels({"ID", "Type", "Quantité", "Seuil", "Date MAJ", "Emplacement", "État"});
    ui->tableRapportSelection->setRowCount(0);
    ui->tableRapportSelection->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableRapportSelection->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);
    ui->tableRapportSelection->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableRapportSelection->setSelectionBehavior(QAbstractItemView::SelectRows);

    int row = 0;
    int rupture = 0;
    int faible = 0;
    int disponible = 0;
    double totalVolume = 0.0;
    while (query.next()) {
        const double qte = query.value(2).toDouble();
        const double seuil = query.value(3).toDouble();
        totalVolume += qte;

        ui->tableRapportSelection->insertRow(row);
        ui->tableRapportSelection->setItem(row, 0, new QTableWidgetItem(query.value(0).toString()));
        ui->tableRapportSelection->setItem(row, 1, new QTableWidgetItem(query.value(1).toString()));
        ui->tableRapportSelection->setItem(row, 2, new QTableWidgetItem(QString::number(qte, 'f', 2)));
        ui->tableRapportSelection->setItem(row, 3, new QTableWidgetItem(QString::number(seuil, 'f', 2)));
        ui->tableRapportSelection->setItem(row, 4, new QTableWidgetItem(query.value(4).toDate().toString("dd/MM/yyyy")));
        ui->tableRapportSelection->setItem(row, 5, new QTableWidgetItem(query.value(5).toString()));
        ui->tableRapportSelection->setItem(row, 6, new QTableWidgetItem(query.value(6).toString()));

        if (qte <= 0.0) {
            ++rupture;
        } else if (seuil > 0.0 && qte <= seuil) {
            ++faible;
        } else {
            ++disponible;
        }

        ++row;
    }

    QString preview;
    preview += "RAPPORT STOCK - ALERTES, TENDANCES ET OPERATIONS\n";
    preview += "Date de génération: " + QDateTime::currentDateTime().toString("dd/MM/yyyy HH:mm:ss") + "\n\n";
    preview += QString("Total produits suivis: %1\n").arg(row);
    preview += QString("Volume total: %1 L\n").arg(QString::number(totalVolume, 'f', 2));
    preview += QString("Disponibles: %1 | Faibles: %2 | Ruptures: %3\n\n")
                   .arg(disponible)
                   .arg(faible)
                   .arg(rupture);

    preview += "ALERTES PRIORITAIRES:\n";
    QSqlQuery qAlert(connexion->getDatabase());
    qAlert.prepare("SELECT TYPE_HUILE, QUANTITE_ACTUELLE, SEUIL_ALERTE, EMPLACEMENT_STOCKAGE "
                   "FROM STOCK "
                   "WHERE QUANTITE_ACTUELLE <= SEUIL_ALERTE OR QUANTITE_ACTUELLE = 0 "
                   "ORDER BY CASE WHEN QUANTITE_ACTUELLE = 0 THEN 0 ELSE 1 END, QUANTITE_ACTUELLE ASC");
    int alertLines = 0;
    if (qAlert.exec()) {
        while (qAlert.next() && alertLines < 5) {
            preview += QString("- %1 | %2 L (seuil %3 L) | %4\n")
                           .arg(qAlert.value(0).toString())
                           .arg(QString::number(qAlert.value(1).toDouble(), 'f', 2))
                           .arg(QString::number(qAlert.value(2).toDouble(), 'f', 2))
                           .arg(qAlert.value(3).toString());
            ++alertLines;
        }
    }
    if (alertLines == 0) {
        preview += "- Aucune alerte active.\n";
    }

    preview += "\nRECOMMANDATIONS:\n";
    preview += "- Traiter en priorité les ruptures.\n";
    preview += "- Planifier le réappro des stocks faibles sur 7 jours.\n";
    preview += "- Recalculer les tendances après chaque mise à jour majeure.\n";

    if (ui->textApercuRapport) {
        ui->textApercuRapport->setPlainText(preview);
    }

    afficherMessage("Rapport", QString("Rapport généré avec succès (%1 ligne(s)).").arg(row));
}

void Stock::onExportRapportPDFClicked()
{
    if (!ui->textApercuRapport || ui->textApercuRapport->toPlainText().trimmed().isEmpty()) {
        afficherMessage("Export PDF", "Générez d'abord un rapport avant l'export.", true);
        return;
    }

    const QString fileName = QFileDialog::getSaveFileName(
        this,
        "Exporter le rapport PDF",
        QString("rapport_stock_%1.pdf").arg(QDate::currentDate().toString("yyyyMMdd")),
        "Fichiers PDF (*.pdf)");

    if (fileName.isEmpty()) {
        return;
    }

    QPdfWriter writer(fileName);
    writer.setPageSize(QPageSize(QPageSize::A4));
    writer.setResolution(96);
    writer.setTitle("Rapport Stock");

    QTextDocument doc;
    doc.setDefaultFont(QFont("Segoe UI", 10));
    doc.setPlainText(ui->textApercuRapport->toPlainText());
    doc.print(&writer);

    afficherMessage("Export PDF", "Rapport exporté avec succès :\n" + fileName);
}

// ==================== SYNCHRONISATION ====================

void Stock::onSynchroniserClicked()
{
    refreshAvailablePorts();
    chargerDonneesTable();
    onCalendarDateSelected(ui->calendarWidgetStock->selectedDate());
    afficherMessage("Synchronisation", "Ports serie et donnees base synchronises.");
}

// ==================== SELECTION TABLE ====================

void Stock::onTableStockSelectionChanged()
{
    QList<QTableWidgetItem*> items = ui->tableStocks->selectedItems();
    if (items.isEmpty()) return;
    
    int row = items.first()->row();
    remplirChamps(row);
}

// ==================== DATE/TIME UPDATE ====================

void Stock::updateDateTime()
{
    QDate currentDate = QDate::currentDate();
    QTime currentTime = QTime::currentTime();
    
    // Format date in French
    QStringList moisFr = {"", "janvier", "février", "mars", "avril", "mai", "juin",
                          "juillet", "août", "septembre", "octobre", "novembre", "décembre"};
    
    QString dateStr = QString("📅 %1 %2 %3")
                          .arg(currentDate.day(), 2, 10, QChar('0'))
                          .arg(moisFr[currentDate.month()])
                          .arg(currentDate.year());
    
    QString timeStr = currentTime.toString("🕐 HH:mm:ss");
    
    if (ui->lblDateTime) {
        ui->lblDateTime->setText(dateStr + " | " + timeStr);
    }
}
