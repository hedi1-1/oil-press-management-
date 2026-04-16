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
#include <QFileDialog>
#include <QAbstractSpinBox>
#include <QHeaderView>
#include <QScrollArea>
#include <QScrollBar>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTabWidget>
#include <QFrame>
#include <QTextCharFormat>
#include <QBrush>
#include <QGraphicsOpacityEffect>
#include <QGraphicsDropShadowEffect>
#include <QPropertyAnimation>
#include <QEasingCurve>
#include <QDateTime>
#include <QSignalBlocker>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <algorithm>

Stock::Stock(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Stock)
    , dateTimeTimer(nullptr)
    , calendarRefreshTimer(nullptr)
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

    initialiserCalendrier();
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
                                       "Huile de colza", "Huile de sésame"});
    
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
    query.bindValue(":asid", m_hasStockId ? QVariant::fromValue(m_currentStockId) : QVariant(QVariant::LongLong));
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
    ui->tableStocks->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableStocks->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableStocks->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void Stock::activerScrolling()
{
    // Obtenir le QTabWidget principal
    QTabWidget* tabWidget = ui->tabWidgetStock;
    if (!tabWidget) {
        qDebug() << "TabWidget non trouvé";
        return;
    }
    
    // Parcourir tous les onglets
    for (int i = 0; i < tabWidget->count(); ++i) {
        QWidget* tab = tabWidget->widget(i);
        
        // Vérifier si l'onglet n'a pas déjà de QScrollArea
        QScrollArea* existingScrollArea = tab->findChild<QScrollArea*>();
        if (existingScrollArea) {
            // Configurer la scrollarea existante
            existingScrollArea->setWidgetResizable(true);
            existingScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
            existingScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
            continue;
        }
        
        // Pour les onglets sans scrollarea, en ajouter une
        QLayout* oldLayout = tab->layout();
        if (!oldLayout) continue;
        
        // Créer une nouvelle scrollarea
        QScrollArea* scrollArea = new QScrollArea(tab);
        scrollArea->setWidgetResizable(true);
        scrollArea->setFrameShape(QFrame::NoFrame);
        scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        
        // Créer un widget conteneur
        QWidget* scrollContent = new QWidget();
        
        // Obtenir tous les widgets enfants
        QList<QWidget*> children;
        for (QObject* child : tab->children()) {
            if (QWidget* widget = qobject_cast<QWidget*>(child)) {
                if (widget != scrollArea && widget->parent() == tab) {
                    children.append(widget);
                }
            }
        }
        
        // Créer une nouvelle layout pour le contenu
        QVBoxLayout* contentLayout = new QVBoxLayout(scrollContent);
        contentLayout->setContentsMargins(10, 10, 10, 10);
        
        // Déplacer les widgets dans le nouveau conteneur
        for (QWidget* child : children) {
            child->setParent(scrollContent);
            contentLayout->addWidget(child);
        }
        
        scrollArea->setWidget(scrollContent);
        
        // Supprimer l'ancienne layout et en créer une nouvelle
        delete oldLayout;
        QVBoxLayout* newLayout = new QVBoxLayout(tab);
        newLayout->setContentsMargins(0, 0, 0, 0);
        newLayout->addWidget(scrollArea);
        
        qDebug() << "Scrolling activé pour l'onglet:" << tabWidget->tabText(i);
    }
    
    qDebug() << "✓ Scrolling activé sur tous les onglets";
}

void Stock::onBackClicked()
{
    emit backToMenu();
    this->hide();
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

void Stock::initialiserCalendrier()
{
    if (!ui->calendarWidgetStock || !ui->tableCalendarDetails) {
        return;
    }

    ui->calendarWidgetStock->setGridVisible(true);
    ui->calendarWidgetStock->setVerticalHeaderFormat(QCalendarWidget::NoVerticalHeader);

    ui->tableCalendarDetails->setColumnCount(6);
    ui->tableCalendarDetails->setHorizontalHeaderLabels({"Type d'huile", "Quantite (L)", "Seuil (L)", "Emplacement", "Etat", "Date MAJ"});
    ui->tableCalendarDetails->horizontalHeader()->setStretchLastSection(true);
    ui->tableCalendarDetails->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableCalendarDetails->setEditTriggers(QAbstractItemView::NoEditTriggers);

    chargerDatesStockCalendrier();
    onCalendarDateSelected(ui->calendarWidgetStock->selectedDate());
}

void Stock::chargerDatesStockCalendrier()
{
    if (!connexion || !connexion->isOpen() || !ui->calendarWidgetStock) {
        return;
    }

    dateStatutMap.clear();

    QSqlQuery query(connexion->getDatabase());
    query.prepare("SELECT TRUNC(DATE_DERNIERE_MISE_A_JOUR), QUANTITE_ACTUELLE, SEUIL_ALERTE "
                  "FROM STOCK WHERE DATE_DERNIERE_MISE_A_JOUR IS NOT NULL");

    if (!query.exec()) {
        qDebug() << "Erreur chargement dates calendrier:" << query.lastError().text();
        return;
    }

    while (query.next()) {
        QDate date = query.value(0).toDate();
        double quantite = query.value(1).toDouble();
        double seuil = query.value(2).toDouble();

        QString statut = "disponible";
        if (seuil > 0 && quantite <= (seuil * 0.5)) {
            statut = "critique";
        } else if (seuil > 0 && quantite <= seuil) {
            statut = "alerte";
        }

        const QString current = dateStatutMap.value(date);
        if (current == "critique") {
            continue;
        }
        if (current == "alerte" && statut == "disponible") {
            continue;
        }

        dateStatutMap.insert(date, statut);
    }

    mettreEnCouleurCalendrier();
    mettreAJourKpiCalendrier();
}

void Stock::mettreAJourKpiCalendrier()
{
    int alertDates = 0;
    int criticalDates = 0;

    for (auto it = dateStatutMap.constBegin(); it != dateStatutMap.constEnd(); ++it) {
        if (it.value() == "critique") {
            criticalDates++;
        } else if (it.value() == "alerte") {
            alertDates++;
        }
    }

    if (ui->labelKpiDatesActivesValue) {
        ui->labelKpiDatesActivesValue->setText(QString::number(dateStatutMap.size()));
    }
    if (ui->labelKpiAlertesValue) {
        ui->labelKpiAlertesValue->setText(QString::number(alertDates));
    }
    if (ui->labelKpiCritiquesValue) {
        ui->labelKpiCritiquesValue->setText(QString::number(criticalDates));
    }
}

void Stock::mettreEnCouleurCalendrier()
{
    if (!ui->calendarWidgetStock) {
        return;
    }

    ui->calendarWidgetStock->setDateTextFormat(QDate(), QTextCharFormat());

    for (auto it = dateStatutMap.constBegin(); it != dateStatutMap.constEnd(); ++it) {
        QTextCharFormat fmt;

        if (it.value() == "critique") {
            fmt.setBackground(QBrush(QColor("#ef4444")));
            fmt.setForeground(QBrush(Qt::white));
        } else if (it.value() == "alerte") {
            fmt.setBackground(QBrush(QColor("#f59e0b")));
            fmt.setForeground(QBrush(Qt::black));
        } else {
            fmt.setBackground(QBrush(QColor("#22c55e")));
            fmt.setForeground(QBrush(Qt::white));
        }

        fmt.setFontWeight(QFont::DemiBold);
        ui->calendarWidgetStock->setDateTextFormat(it.key(), fmt);
    }
}

void Stock::onCalendarDateSelected(const QDate &date)
{
    afficherStocksParDate(date);
}

void Stock::onCalendarShowMonth(int year, int month)
{
    Q_UNUSED(year);
    Q_UNUSED(month);
    mettreEnCouleurCalendrier();
}

void Stock::afficherStocksParDate(const QDate &date)
{
    if (!connexion || !connexion->isOpen() || !ui->tableCalendarDetails) {
        return;
    }

    QSqlQuery query(connexion->getDatabase());
    query.prepare("SELECT TYPE_HUILE, QUANTITE_ACTUELLE, SEUIL_ALERTE, EMPLACEMENT_STOCKAGE, ETAT_STOCK, DATE_DERNIERE_MISE_A_JOUR "
                  "FROM STOCK "
                  "WHERE TRUNC(DATE_DERNIERE_MISE_A_JOUR) = TO_DATE(:dateValue, 'YYYY-MM-DD') "
                  "ORDER BY TYPE_HUILE");
    query.bindValue(":dateValue", date.toString("yyyy-MM-dd"));

    if (!query.exec()) {
        qDebug() << "Erreur afficherStocksParDate:" << query.lastError().text();
        return;
    }

    ui->tableCalendarDetails->setRowCount(0);

    int row = 0;
    int countDisponible = 0;
    int countAlerte = 0;
    int countCritique = 0;
    while (query.next()) {
        const double quantite = query.value(1).toDouble();
        const double seuil = query.value(2).toDouble();

        ui->tableCalendarDetails->insertRow(row);
        ui->tableCalendarDetails->setItem(row, 0, new QTableWidgetItem(query.value(0).toString()));
        ui->tableCalendarDetails->setItem(row, 1, new QTableWidgetItem(QString::number(quantite, 'f', 2)));
        ui->tableCalendarDetails->setItem(row, 2, new QTableWidgetItem(QString::number(seuil, 'f', 2)));
        ui->tableCalendarDetails->setItem(row, 3, new QTableWidgetItem(query.value(3).toString()));
        ui->tableCalendarDetails->setItem(row, 4, new QTableWidgetItem(query.value(4).toString()));
        ui->tableCalendarDetails->setItem(row, 5, new QTableWidgetItem(query.value(5).toDate().toString("dd/MM/yyyy")));

        QColor rowColor("#dcfce7");
        QColor textColor("#14532d");
        if (seuil > 0 && quantite <= (seuil * 0.5)) {
            rowColor = QColor("#fee2e2");
            textColor = QColor("#991b1b");
            countCritique++;
        } else if (seuil > 0 && quantite <= seuil) {
            rowColor = QColor("#ffedd5");
            textColor = QColor("#92400e");
            countAlerte++;
        } else {
            countDisponible++;
        }

        for (int col = 0; col < ui->tableCalendarDetails->columnCount(); ++col) {
            QTableWidgetItem *item = ui->tableCalendarDetails->item(row, col);
            if (item) {
                item->setBackground(QBrush(rowColor));
                item->setForeground(QBrush(textColor));
            }
        }

        row++;
    }

    if (ui->labelCalendarSummary) {
        ui->labelCalendarSummary->setText(
            QString("📌 Date selectionnee : %1 | %2 enregistrement(s) | "
                    "<span style='color:#14532d;'>🟢 %3</span> "
                    "<span style='color:#92400e;'>🟡 %4</span> "
                    "<span style='color:#991b1b;'>🔴 %5</span>")
                .arg(date.toString("dd/MM/yyyy"))
                .arg(row)
                .arg(countDisponible)
                .arg(countAlerte)
                .arg(countCritique));
        animerResumeCalendrier();
    }
}

void Stock::animerResumeCalendrier()
{
    if (!ui->labelCalendarSummary) {
        return;
    }

    QGraphicsOpacityEffect *effect = qobject_cast<QGraphicsOpacityEffect*>(ui->labelCalendarSummary->graphicsEffect());
    if (!effect) {
        effect = new QGraphicsOpacityEffect(ui->labelCalendarSummary);
        ui->labelCalendarSummary->setGraphicsEffect(effect);
    }

    QPropertyAnimation *animation = new QPropertyAnimation(effect, "opacity", ui->labelCalendarSummary);
    animation->setDuration(280);
    animation->setStartValue(0.35);
    animation->setEndValue(1.0);
    animation->setEasingCurve(QEasingCurve::OutCubic);
    animation->start(QAbstractAnimation::DeleteWhenStopped);
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
    // Vérifier qu'il y a des données à exporter
    if (ui->tableResultatsCombined->rowCount() == 0) {
        afficherMessage("Export", "⚠️  Aucune donnée à exporter !\n\n"
                                   "Veuillez d'abord faire une recherche ou un tri.", true);
        return;
    }
    
    QString fileName = QFileDialog::getSaveFileName(this, "Exporter les résultats", 
                                                     "", "Fichiers CSV (*.csv)");
    
    if (fileName.isEmpty()) return;
    
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        afficherMessage("Erreur", "Impossible d'ouvrir le fichier pour l'export", true);
        return;
    }
    
    QTextStream out(&file);
    // UTF-8 par défaut dans Qt moderne
    
    // En-têtes
    out << "ID,Type d'huile,Quantité (L),Seuil (L),Date MAJ,Emplacement,État\n";
    
    // Données
    for (int i = 0; i < ui->tableResultatsCombined->rowCount(); ++i) {
        for (int j = 0; j < ui->tableResultatsCombined->columnCount(); ++j) {
            QTableWidgetItem* item = ui->tableResultatsCombined->item(i, j);
            if (item) {
                QString text = item->text();
                // Gérer les virgules dans le texte
                if (text.contains(",") || text.contains("\"")) {
                    text = "\"" + text.replace("\"", "\"\"") + "\"";
                }
                out << text;
            }
            if (j < ui->tableResultatsCombined->columnCount() - 1) out << ",";
        }
        out << "\n";
    }
    
    file.close();
    afficherMessage("Succès", QString("✅ Export réussi !\n\n"
                                      "%1 ligne(s) exportée(s) vers :\n%2")
                                      .arg(ui->tableResultatsCombined->rowCount())
                                      .arg(fileName));
}

// ==================== STATISTIQUES ====================

void Stock::onCalculerClicked()
{
    if (!connexion || !connexion->isOpen()) return;
    
    QString critere = ui->comboStatsPar->currentText();
    QString colonne = "TYPE_HUILE";
    
    if (critere == "État du stock") colonne = "ETAT_STOCK";
    else if (critere == "Emplacement") colonne = "EMPLACEMENT_STOCKAGE";
    
    QSqlQuery query(connexion->getDatabase());
    QString sql = QString("SELECT %1, COUNT(*) as NB, SUM(QUANTITE_ACTUELLE) as TOTAL "
                         "FROM STOCK GROUP BY %1 ORDER BY NB DESC").arg(colonne);
    
    query.exec(sql);
    
    ui->tableResultatsCombined->clear();
    ui->tableResultatsCombined->setColumnCount(3);
    ui->tableResultatsCombined->setHorizontalHeaderLabels({critere, "Nombre", "Total (L)"});
    ui->tableResultatsCombined->setRowCount(0);
    
    int row = 0;
    while (query.next()) {
        ui->tableResultatsCombined->insertRow(row);
        ui->tableResultatsCombined->setItem(row, 0, new QTableWidgetItem(query.value(0).toString()));
        ui->tableResultatsCombined->setItem(row, 1, new QTableWidgetItem(query.value(1).toString()));
        ui->tableResultatsCombined->setItem(row, 2, new QTableWidgetItem(QString::number(query.value(2).toDouble(), 'f', 2)));
        row++;
    }
    
    afficherMessage("Statistiques", "Statistiques calculées avec succès !");
}

// ==================== ALERTES ====================

void Stock::onVerifierAlertesClicked()
{
    if (!connexion || !connexion->isOpen()) return;
    
    QSqlQuery query(connexion->getDatabase());
    query.exec("SELECT TYPE_HUILE, QUANTITE_ACTUELLE, SEUIL_ALERTE, EMPLACEMENT_STOCKAGE "
               "FROM STOCK WHERE QUANTITE_ACTUELLE < SEUIL_ALERTE OR QUANTITE_ACTUELLE = 0 "
               "ORDER BY QUANTITE_ACTUELLE");
    
    QString message = "=== ALERTES STOCK ===\n\n";
    int count = 0;
    
    while (query.next()) {
        count++;
        QString type = query.value(0).toString();
        double quantite = query.value(1).toDouble();
        double seuil = query.value(2).toDouble();
        QString emplacement = query.value(3).toString();
        
        if (quantite == 0) {
            message += QString("⛔ RUPTURE : %1 (%2)\n").arg(type).arg(emplacement);
        } else {
            message += QString("⚠️  FAIBLE : %1 - %2 L (seuil: %3 L) - %4\n")
                          .arg(type).arg(quantite).arg(seuil).arg(emplacement);
        }
    }
    
    if (count == 0) {
        message = "✅ Aucune alerte ! Tous les stocks sont au-dessus des seuils.";
    } else {
        message += QString("\n📊 Total : %1 alerte(s)").arg(count);
    }
    
    QMessageBox::warning(this, "Vérification des alertes", message);
}

// ==================== HISTORIQUE ====================

void Stock::onAfficherHistoriqueClicked()
{
    chargerDonneesTable();
    
    ui->tableHistorique->clear();
    ui->tableHistorique->setColumnCount(7);
    ui->tableHistorique->setHorizontalHeaderLabels({"ID", "Type", "Quantité", "Seuil", 
                                                     "Date MAJ", "Emplacement", "État"});
    ui->tableHistorique->setRowCount(0);
    
    // Copier les données de tableStocks vers tableHistorique
    for (int i = 0; i < ui->tableStocks->rowCount(); ++i) {
        ui->tableHistorique->insertRow(i);
        for (int j = 0; j < 7; ++j) {
            ui->tableHistorique->setItem(i, j, new QTableWidgetItem(ui->tableStocks->item(i, j)->text()));
        }
    }
    
    afficherMessage("Historique", "Historique chargé avec succès !");
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

    afficherMessage("Analyse", "Analyse prédictive et recommandations générées avec succès !");
}

void Stock::onAnalyserPredictifHuileClicked()
{
    if (ui->comboPeriodePredictifHuile && ui->comboPeriode) {
        ui->comboPeriode->setCurrentText(ui->comboPeriodePredictifHuile->currentText());
    }

    onAnalyserTendancesClicked();

    if (ui->tableResultatsPredictifHuile && ui->tableResultatsCombined) {
        ui->tableResultatsPredictifHuile->clear();
        ui->tableResultatsPredictifHuile->setColumnCount(ui->tableResultatsCombined->columnCount());

        QStringList headers;
        for (int col = 0; col < ui->tableResultatsCombined->columnCount(); ++col) {
            QTableWidgetItem* headerItem = ui->tableResultatsCombined->horizontalHeaderItem(col);
            headers << (headerItem ? headerItem->text() : QString("Colonne %1").arg(col + 1));
        }
        ui->tableResultatsPredictifHuile->setHorizontalHeaderLabels(headers);
        ui->tableResultatsPredictifHuile->setRowCount(0);
        ui->tableResultatsPredictifHuile->setWordWrap(false);
        ui->tableResultatsPredictifHuile->setAlternatingRowColors(true);
        ui->tableResultatsPredictifHuile->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
        ui->tableResultatsPredictifHuile->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
        ui->tableResultatsPredictifHuile->verticalHeader()->setDefaultSectionSize(32);

        for (int row = 0; row < ui->tableResultatsCombined->rowCount(); ++row) {
            ui->tableResultatsPredictifHuile->insertRow(row);
            for (int col = 0; col < ui->tableResultatsCombined->columnCount(); ++col) {
                QTableWidgetItem* source = ui->tableResultatsCombined->item(row, col);
                if (!source) {
                    continue;
                }
                QTableWidgetItem* cloned = source->clone();
                ui->tableResultatsPredictifHuile->setItem(row, col, cloned);
            }
        }

        for (int col = 0; col < ui->tableResultatsPredictifHuile->columnCount(); ++col) {
            ui->tableResultatsPredictifHuile->setColumnWidth(col, ui->tableResultatsCombined->columnWidth(col));
            QTableWidgetItem* headerItem = ui->tableResultatsPredictifHuile->horizontalHeaderItem(col);
            if (headerItem) {
                headerItem->setTextAlignment(Qt::AlignCenter);
            }
        }

        ui->tableResultatsPredictifHuile->horizontalHeader()->setStretchLastSection(false);
        ui->tableResultatsPredictifHuile->setSelectionBehavior(QAbstractItemView::SelectRows);
        ui->tableResultatsPredictifHuile->setEditTriggers(QAbstractItemView::NoEditTriggers);
    }

    if (ui->textAnalysePredictifHuile && ui->textAnalyseTendances) {
        ui->textAnalysePredictifHuile->setPlainText(ui->textAnalyseTendances->toPlainText());
    }

    if (ui->labelTopStrategiqueValue2 && ui->labelTopStrategiqueValue) {
        ui->labelTopStrategiqueValue2->setText(ui->labelTopStrategiqueValue->text());
    }
    if (ui->labelRisqueMoyenGlobalValue2 && ui->labelRisqueMoyenGlobalValue) {
        ui->labelRisqueMoyenGlobalValue2->setText(ui->labelRisqueMoyenGlobalValue->text());
    }
    if (ui->labelBesoinReapproTotalValue2 && ui->labelBesoinReapproTotalValue) {
        ui->labelBesoinReapproTotalValue2->setText(ui->labelBesoinReapproTotalValue->text());
    }
}

// ==================== RAPPORTS ====================

void Stock::onGenererRapportClicked()
{
    chargerDonneesTable();
    
    ui->tableRapportSelection->clear();
    ui->tableRapportSelection->setColumnCount(7);
    ui->tableRapportSelection->setHorizontalHeaderLabels({"ID", "Type", "Quantité", "Seuil", 
                                                           "Date MAJ", "Emplacement", "État"});
    ui->tableRapportSelection->setRowCount(0);
    
    // Copier toutes les données vers le rapport
    for (int i = 0; i < ui->tableStocks->rowCount(); ++i) {
        ui->tableRapportSelection->insertRow(i);
        for (int j = 0; j < 7; ++j) {
            ui->tableRapportSelection->setItem(i, j, new QTableWidgetItem(ui->tableStocks->item(i, j)->text()));
        }
    }
    
    afficherMessage("Rapport", "Rapport généré avec succès !");
}

void Stock::onExportRapportPDFClicked()
{
    afficherMessage("Export PDF", "Fonctionnalité d'export PDF en cours de développement.\n"
                                   "Utilisez l'export CSV pour le moment.");
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
