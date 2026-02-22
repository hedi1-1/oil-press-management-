#include "production.h"
#include "ui_production.h"
#include <QDateTime>
#include <QScrollBar>
#include <QComboBox>

Production::Production(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Production)
    , currentProductionId(0)
    , m_productionService(new ProductionService(this))
    , m_smartAssistant(new SmartProductionAssistant(m_productionService, this))
    , m_productionBot(new ProductionBot(m_productionService, m_smartAssistant, this))
    , m_voiceAssistant(new VoiceAssistant(m_productionService, m_smartAssistant, this))
    , m_aiChatDisplay(nullptr)
    , m_aiCommandInput(nullptr)
    , m_btnSendAi(nullptr)
    , m_btnVoice(nullptr)
    , m_btnBotToggle(nullptr)
    , m_btnAiAnalyze(nullptr)
    , m_btnAiOptimize(nullptr)
    , m_lblBotStatus(nullptr)
    , m_lblVoiceStatus(nullptr)
    , m_botDecisionList(nullptr)
    , m_comboSelectProd(nullptr)
    , m_lblProdDate(nullptr)
    , m_lblProdQty(nullptr)
    , m_lblProdType(nullptr)
    , m_lblProdDuree(nullptr)
{
    ui->setupUi(this);
    
    // Establish database connection
    if (!conn.createconnect()) {
        showErrorNotification("Impossible de se connecter à la base de données Oracle");
    }
    
    // Connect back button
    connect(ui->btnBackToMenu, &QPushButton::clicked, this, &Production::onBackButtonClicked);
    
    // Connect CRUD buttons
    connect(ui->btnPlanifier, &QPushButton::clicked, this, &Production::onPlanifierClicked);
    connect(ui->btnStart, &QPushButton::clicked, this, &Production::onStartClicked);
    connect(ui->btnStop, &QPushButton::clicked, this, &Production::onStopClicked);
    connect(ui->btnCalculateYield, &QPushButton::clicked, this, &Production::onCalculateYieldClicked);
    connect(ui->btnValidateQuality, &QPushButton::clicked, this, &Production::onValidateQualityClicked);
    connect(ui->btnGenerateReport, &QPushButton::clicked, this, &Production::onGenerateReportClicked);
    
    // Connect history buttons
    connect(ui->btnDeleteProduction, &QPushButton::clicked, this, &Production::onDeleteClicked);
    connect(ui->btnModifyProduction, &QPushButton::clicked, this, &Production::onModifyClicked);
    connect(ui->btnUpdateMonitoring, &QPushButton::clicked, this, &Production::onRefreshHistoryClicked);
    
    // TABLE ROW SELECTION — sets currentProductionId from selected row
    connect(ui->tableProductionHistory, &QTableWidget::currentCellChanged,
            this, [this](int row, int /*col*/, int /*prevRow*/, int /*prevCol*/) {
                onTableRowSelected(row);
            });
    
    // Setup AI Assistant tab
    setupAiTab();
    
    // Setup Lancement tab — production selector combo
    setupLancementSelector();
    
    // Connect voice assistant signals
    connect(m_voiceAssistant, &VoiceAssistant::responseReady,
            this, &Production::onVoiceResponse);
    
    // Connect bot signals
    connect(m_productionBot, &ProductionBot::newDecision,
            this, &Production::onBotDecision);
    connect(m_productionBot, &ProductionBot::alerteAnomalie, this,
            [this](int id, const QString &msg) {
                appendAiChat("🚨 ALERTE",
                             QString("Production #%1 — %2").arg(id).arg(msg), "#dc2626");
            });
    // When bot modifies the DB, refresh the table automatically
    connect(m_productionBot, &ProductionBot::productionTableChanged,
            this, &Production::loadProductionHistory);
    
    // Load production history on startup
    loadProductionHistory();
}

Production::~Production()
{
    m_productionBot->stop();
    conn.closeconnect();
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
        
        // Refresh history table
        loadProductionHistory();
    } else {
        showErrorNotification("Échec de la planification!\n\nConsultez la console Application Output pour les détails.");
    }
}

// ============================================================================
// CRUD: UPDATE STATUS - START
// ============================================================================

void Production::onStartClicked()
{
    // If combo has a selection, use that production ID
    if (m_comboSelectProd && m_comboSelectProd->currentData().isValid()) {
        currentProductionId = m_comboSelectProd->currentData().toInt();
    }
    
    if (currentProductionId == 0) {
        showErrorNotification("Aucune production sélectionnée!\n\nSélectionnez une production planifiée dans la liste.");
        return;
    }
    
    if (m_productionService->demarrerProduction(currentProductionId)) {
        showSuccessNotification("▶ Production #" + QString::number(currentProductionId) + " démarrée!\n\n"
                               "Heure de démarrage: " + QTime::currentTime().toString("HH:mm:ss") + "\n"
                               "Statut: En cours");
        
        // Update Lancement tab display
        ui->lblCurrentStatus->setText("🔵  En cours");
        ui->lblCurrentStatus->setStyleSheet(
            "background-color: #DBEAFE; color: #1E40AF; border: 2px solid #60A5FA; "
            "font-size: 18px; font-weight: 700; padding: 15px 25px; border-radius: 10px;");
        ui->lblStartTime->setText("⏰  " + QTime::currentTime().toString("HH:mm:ss"));
        ui->lblPhase->setText("Phase : Production en cours");
        ui->progressProduction->setValue(50);
        
        loadProductionHistory();
    } else {
        showErrorNotification("Échec du démarrage!\n\nVérifiez que la production est au statut 'Planifié'.");
    }
}

// ============================================================================
// CRUD: UPDATE STATUS - STOP
// ============================================================================

void Production::onStopClicked()
{
    if (m_comboSelectProd && m_comboSelectProd->currentData().isValid()) {
        currentProductionId = m_comboSelectProd->currentData().toInt();
    }
    if (currentProductionId == 0) {
        showErrorNotification("Aucune production en cours!");
        return;
    }
    
    if (m_productionService->arreterProduction(currentProductionId)) {
        showSuccessNotification("⏹ Production #" + QString::number(currentProductionId) + " arrêtée!");
        
        ui->lblCurrentStatus->setText("🟠  Arrêté");
        ui->lblCurrentStatus->setStyleSheet(
            "background-color: #FEF3C7; color: #92400E; border: 2px solid #FCD34D; "
            "font-size: 18px; font-weight: 700; padding: 15px 25px; border-radius: 10px;");
        ui->lblPhase->setText("Phase : Arrêté");
        
        loadProductionHistory();
    } else {
        showErrorNotification("Échec de l'arrêt de la production!");
    }
}

// ============================================================================
// CRUD: CALCULATE YIELD
// ============================================================================

void Production::onCalculateYieldClicked()
{
    int olivesKg = ui->spinOlivesKg->value();
    double oilLiters = ui->spinOilProduced->value();
    
    if (olivesKg <= 0) {
        showErrorNotification("Quantité d'olives invalide!\n\nEntrez une quantité supérieure à 0.");
        return;
    }
    
    double rendement = (oilLiters / static_cast<double>(olivesKg)) * 100.0;
    
    // Update UI
    ui->lblYield->setText(QString::number(rendement, 'f', 2) + " %");
    ui->progressYield->setValue(static_cast<int>(rendement));
    
    // Update production in database
    if (currentProductionId > 0) {
        currentProduction.setIdProduction(currentProductionId);
        currentProduction.setHuileProduiteL(oilLiters);
        currentProduction.setRendement(rendement);
        currentProduction.setQuantiteOlivesKg(olivesKg);
        
        if (currentProduction.updateProduction()) {
            QString evaluation;
            if (rendement >= 18) {
                evaluation = "🌟 Excellent rendement!";
            } else if (rendement >= 15) {
                evaluation = "✓ Bon rendement";
            } else if (rendement >= 10) {
                evaluation = "⚠ Rendement moyen";
            } else {
                evaluation = "⚠ Rendement faible";
            }
            
            showSuccessNotification("📊 Rendement calculé et enregistré!\n\n"
                                   "Olives: " + QString::number(olivesKg) + " kg\n"
                                   "Huile: " + QString::number(oilLiters, 'f', 2) + " L\n"
                                   "Rendement: " + QString::number(rendement, 'f', 2) + " %\n\n"
                                   + evaluation);
        }
    } else {
        showSuccessNotification("📊 Rendement calculé!\n\n"
                               "Rendement: " + QString::number(rendement, 'f', 2) + " %\n\n"
                               "Note: Planifiez une production pour enregistrer.");
    }
}

// ============================================================================
// CRUD: VALIDATE QUALITY
// ============================================================================

void Production::onValidateQualityClicked()
{
    if (currentProductionId == 0) {
        showErrorNotification("Aucune production à valider!\n\nPlanifiez d'abord une production.");
        return;
    }
    
    currentProduction.setIdProduction(currentProductionId);
    currentProduction.setQualite("Extra Vierge");
    currentProduction.setConformeNormes(true);
    currentProduction.setStatut("Qualité validée");
    
    if (currentProduction.updateProduction()) {
        showSuccessNotification("✓ Qualité validée avec succès!\n\n"
                               "Classification: Extra Vierge\n"
                               "Conforme aux normes: Oui\n"
                               "Statut: Validé");
    } else {
        showErrorNotification("Échec de la validation qualité!");
    }
}

// ============================================================================
// CRUD: GENERATE REPORT
// ============================================================================

void Production::onGenerateReportClicked()
{
    if (currentProductionId == 0) {
        showErrorNotification("Aucune production pour le rapport!\n\nPlanifiez d'abord une production.");
        return;
    }
    
    currentProduction.setIdProduction(currentProductionId);
    currentProduction.setDateGenerationRapport(QDate::currentDate());
    currentProduction.setStatut("Terminé");
    
    if (currentProduction.updateProduction()) {
        showSuccessNotification("📄 Rapport généré avec succès!\n\n"
                               "Date: " + QDate::currentDate().toString("dd/MM/yyyy") + "\n"
                               "Production ID: " + QString::number(currentProductionId) + "\n"
                               "Statut: Terminé");
        
        // Reset for new production
        currentProductionId = 0;
        loadProductionHistory(); // Refresh the table
    } else {
        showErrorNotification("Échec de la génération du rapport!");
    }
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
        if (statut.contains("Qualite valid") || statut == "Qualite validee") {
            statutItem->setBackground(QColor("#D1FAE5"));
            statutItem->setForeground(QColor("#065F46"));
        } else if (statut == "Non conforme") {
            statutItem->setBackground(QColor("#FEE2E2"));
            statutItem->setForeground(QColor("#991B1B"));
        } else if (statut == "Termine" || statut.contains("Termin")) {
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
    
    // Resize columns to content
    ui->tableProductionHistory->resizeColumnsToContents();
    ui->tableProductionHistory->horizontalHeader()->setStretchLastSection(true);
    
    qDebug() << "Loaded" << row << "productions into history table";
    
    // Also refresh the launch combo
    populateLaunchCombo();
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
// CRUD: MODIFY SELECTED PRODUCTION
// ============================================================================

void Production::onModifyClicked()
{
    // Get selected row
    int selectedRow = ui->tableProductionHistory->currentRow();
    if (selectedRow < 0) {
        showErrorNotification("Sélectionnez une production dans le tableau!");
        return;
    }
    
    // Get data from selected row
    int productionId = ui->tableProductionHistory->item(selectedRow, 0)->text().toInt();
    QString olivesText = ui->tableProductionHistory->item(selectedRow, 2)->text();
    olivesText.remove(" kg");
    int olivesKg = olivesText.toInt();
    
    QString huileText = ui->tableProductionHistory->item(selectedRow, 3)->text();
    huileText.remove(" L");
    double huileL = huileText.toDouble();
    
    // Set current production ID for updates
    currentProductionId = productionId;
    
    // Fill form with selected data
    ui->spinOlivesKg->setValue(olivesKg);
    ui->spinOilProduced->setValue(huileL);
    
    // Switch to Planification tab
    ui->tabWidgetProduction->setCurrentIndex(0);
    
    showSuccessNotification("Production chargée pour modification!\n\n"
                           "ID: " + QString::number(productionId) + "\n"
                           "Modifiez les valeurs puis cliquez sur 'Planifier'");
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

// ============================================================================
// TABLE ROW SELECTION — User selects a production from the table
// ============================================================================

void Production::onTableRowSelected(int row)
{
    if (row < 0) {
        currentProductionId = 0;
        return;
    }
    QTableWidgetItem *idItem = ui->tableProductionHistory->item(row, 0);
    if (!idItem) return;

    currentProductionId = idItem->text().toInt();

    // Read status from the table
    QTableWidgetItem *statutItem = ui->tableProductionHistory->item(row, 5);
    QString statut = statutItem ? statutItem->text() : "";

    // Fill form with selected data
    QTableWidgetItem *olivesItem = ui->tableProductionHistory->item(row, 2);
    if (olivesItem) {
        QString txt = olivesItem->text();
        txt.remove(" kg");
        ui->spinOlivesKg->setValue(txt.toInt());
    }
    QTableWidgetItem *huileItem = ui->tableProductionHistory->item(row, 3);
    if (huileItem) {
        QString txt = huileItem->text();
        txt.remove(" L");
        ui->spinOilProduced->setValue(txt.toDouble());
    }

    updateButtonStates(statut);

    qDebug() << "Selected production ID:" << currentProductionId << "Statut:" << statut;
}

void Production::updateButtonStates(const QString &statut)
{
    // Enable/disable buttons based on selected production's status
    bool isPlanifie  = (statut == "Planifie" || statut == "Planifiee");
    bool isEnCours   = (statut == "En cours");
    bool isTermine   = (statut == "Termine");
    bool isArrete    = (statut == "Arrete" || statut.contains("Arr"));
    bool hasSelection = (currentProductionId > 0);

    ui->btnStart->setEnabled(hasSelection && (isPlanifie || isArrete));
    ui->btnStop->setEnabled(hasSelection && isEnCours);
    ui->btnCalculateYield->setEnabled(hasSelection);
    ui->btnValidateQuality->setEnabled(hasSelection && isTermine);
    ui->btnGenerateReport->setEnabled(hasSelection);
    ui->btnDeleteProduction->setEnabled(hasSelection);
    ui->btnModifyProduction->setEnabled(hasSelection);
}

// ============================================================================
// LANCEMENT TAB — Production Selector Setup
// ============================================================================

void Production::setupLancementSelector()
{
    // Insert a "Sélection de production" group at the TOP of the Lancement tab layout
    QGroupBox *selectorBox = new QGroupBox("  Sélectionner une production à lancer");
    selectorBox->setStyleSheet(
        "QGroupBox { font-weight: 700; color: #1B4332; font-size: 14px; "
        "border: 2px solid #d1d5db; border-radius: 12px; padding-top: 20px; margin-top: 6px; "
        "background-color: white; }"
        "QGroupBox::title { subcontrol-origin: margin; left: 14px; padding: 0 8px; }");

    QVBoxLayout *selectorLayout = new QVBoxLayout(selectorBox);
    selectorLayout->setSpacing(10);
    selectorLayout->setContentsMargins(16, 16, 16, 12);

    // ── Combo box ──
    m_comboSelectProd = new QComboBox();
    m_comboSelectProd->setMinimumHeight(40);
    m_comboSelectProd->setStyleSheet(
        "QComboBox { background-color: #f9fafb; color: #1f2937; border: 2px solid #d1d5db; "
        "border-radius: 8px; padding: 8px 14px; font-size: 13px; font-weight: 600; }"
        "QComboBox:focus { border-color: #1B4332; }"
        "QComboBox::drop-down { border: none; width: 36px; }"
        "QComboBox::down-arrow { image: none; border-left: 5px solid transparent; "
        "border-right: 5px solid transparent; border-top: 7px solid #1B4332; margin-right: 10px; }"
        "QComboBox QAbstractItemView { background-color: white; color: #1f2937; "
        "border: 2px solid #d1d5db; border-radius: 6px; padding: 4px; "
        "selection-background-color: #D1FAE5; selection-color: #065F46; font-size: 13px; }");
    m_comboSelectProd->addItem("-- Selectionnez une production --", 0);
    selectorLayout->addWidget(m_comboSelectProd);

    // ── Info cards row ──
    QHBoxLayout *infoRow = new QHBoxLayout();
    infoRow->setSpacing(10);

    auto makeInfoCard = [](const QString &title, const QString &defaultValue, 
                           const QString &borderColor) -> QLabel* {
        QLabel *lbl = new QLabel();
        lbl->setTextFormat(Qt::PlainText);
        lbl->setText(title + "\n" + defaultValue);
        lbl->setStyleSheet(
            QString("background-color: #f9fafb; border: 2px solid %1; border-radius: 8px; "
                    "padding: 8px 14px; min-width: 120px; font-size: 12px; color: #1f2937; font-weight: 600;")
            .arg(borderColor));
        lbl->setAlignment(Qt::AlignCenter);
        return lbl;
    };

    m_lblProdDate  = makeInfoCard("DATE", "--/--/----", "#d1d5db");
    m_lblProdQty   = makeInfoCard("OLIVES", "-- kg", "#d1d5db");
    m_lblProdType  = makeInfoCard("TYPE PRESSAGE", "--", "#d1d5db");
    m_lblProdDuree = makeInfoCard("DUREE ESTIMEE", "-- min", "#d1d5db");

    infoRow->addWidget(m_lblProdDate);
    infoRow->addWidget(m_lblProdQty);
    infoRow->addWidget(m_lblProdType);
    infoRow->addWidget(m_lblProdDuree);
    infoRow->addStretch();
    selectorLayout->addLayout(infoRow);

    // Insert at position 0 (top) of the Lancement tab's layout
    ui->layoutLancement->insertWidget(0, selectorBox);

    // Reduce spacing on the original .ui groups to reclaim vertical space
    ui->groupLancementStatus->setMaximumHeight(110);
    ui->groupProgress->setMaximumHeight(100);
    ui->groupControls->setMaximumHeight(100);

    // ── Connect combo selection ──
    connect(m_comboSelectProd, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, [this](int /*index*/) {
        int id = m_comboSelectProd->currentData().toInt();
        if (id > 0) {
            currentProductionId = id;
            updateLancementDisplay(id);
        } else {
            // Reset display
            m_lblProdDate->setText("DATE\n--/--/----");
            m_lblProdQty->setText("OLIVES\n-- kg");
            m_lblProdType->setText("TYPE PRESSAGE\n--");
            m_lblProdDuree->setText("DUREE ESTIMEE\n-- min");
            m_lblProdDate->setStyleSheet("background-color: #f9fafb; border: 2px solid #d1d5db; border-radius: 8px; padding: 8px 14px; min-width: 120px; font-size: 12px; color: #1f2937; font-weight: 600;");
            m_lblProdQty->setStyleSheet(m_lblProdDate->styleSheet());
            m_lblProdType->setStyleSheet(m_lblProdDate->styleSheet());
            m_lblProdDuree->setStyleSheet(m_lblProdDate->styleSheet());
            ui->lblCurrentStatus->setText("🟢  Pret a demarrer");
            ui->lblCurrentStatus->setStyleSheet(
                "background-color: #D1FAE5; color: #065F46; border: 2px solid #34D399; "
                "font-size: 16px; font-weight: 700; padding: 12px 20px; border-radius: 10px;");
            ui->progressProduction->setValue(0);
            ui->lblPhase->setText("Phase : En attente");
            ui->lblProgressEnd->setText("0 / 500 kg traites");
            ui->lblStartTime->setText("--:--:--");
            ui->lblElapsedTime->setText("00:00:00");
        }
    });

    // Populate on startup
    populateLaunchCombo();
}

// ============================================================================
// POPULATE LAUNCH COMBO — Load planned/en-cours productions from DB
// ============================================================================

void Production::populateLaunchCombo()
{
    if (!m_comboSelectProd) return;

    int previousId = m_comboSelectProd->currentData().toInt();
    m_comboSelectProd->blockSignals(true);
    m_comboSelectProd->clear();
    m_comboSelectProd->addItem("-- Sélectionnez une production --", 0);

    QSqlQuery query(QSqlDatabase::database("production_conn"));
    query.exec("SELECT IDPRODUCTION, DATEPRODUCTION, QUANTITEOLIVESKG, TYPEPRESSAGE, STATUT, DUREEESTIMEE "
               "FROM PRODUCTION WHERE STATUT IN ('Planifie','Planifiee','En cours') "
               "ORDER BY IDPRODUCTION DESC");

    int restoreIndex = 0;
    int idx = 1;
    while (query.next()) {
        int id       = query.value(0).toInt();
        QString date = query.value(1).toDate().toString("dd/MM/yyyy");
        int qty      = query.value(2).toInt();
        QString type = query.value(3).toString();
        QString statut = query.value(4).toString();

        QString icon = (statut == "En cours") ? "🔵" : "📋";
        QString label = QString("%1  #%2 — %3 — %4 kg — %5")
                            .arg(icon).arg(id).arg(date).arg(qty).arg(type);
        m_comboSelectProd->addItem(label, id);

        if (id == previousId) restoreIndex = idx;
        idx++;
    }

    m_comboSelectProd->blockSignals(false);

    // Restore previous selection
    if (restoreIndex > 0 && restoreIndex < m_comboSelectProd->count()) {
        m_comboSelectProd->setCurrentIndex(restoreIndex);
    }
}

// ============================================================================
// UPDATE LANCEMENT DISPLAY — Fill info cards + progress from selected prod
// ============================================================================

void Production::updateLancementDisplay(int idProduction)
{
    ProductionModel prod = m_productionService->getProductionById(idProduction);
    if (prod.getIdProduction() <= 0) return;

    QString date = prod.getDateProduction().toString("dd/MM/yyyy");
    int qty = prod.getQuantiteOlivesKg();
    QString type = prod.getTypePressage();
    int duree = prod.getDureeEstimee();
    QString statut = prod.getStatut();
    double huile = prod.getHuileProduiteL();
    double rendement = prod.getRendement();

    QString activeStyle = "background-color: #f0fdf4; border: 2px solid #34D399; border-radius: 8px; "
                          "padding: 8px 14px; min-width: 120px; font-size: 12px; color: #1B4332; font-weight: 700;";

    // Update info cards (plain text)
    m_lblProdDate->setText("DATE\n" + date);
    m_lblProdDate->setStyleSheet(activeStyle);

    m_lblProdQty->setText("OLIVES\n" + QString::number(qty) + " kg");
    m_lblProdQty->setStyleSheet(activeStyle);

    m_lblProdType->setText("TYPE PRESSAGE\n" + type);
    m_lblProdType->setStyleSheet(activeStyle);

    m_lblProdDuree->setText("DUREE ESTIMEE\n" + QString::number(duree) + " min");
    m_lblProdDuree->setStyleSheet(activeStyle);

    // Update status section
    if (statut == "En cours") {
        ui->lblCurrentStatus->setText("En cours");
        ui->lblCurrentStatus->setStyleSheet(
            "background-color: #DBEAFE; color: #1E40AF; border: 2px solid #60A5FA; "
            "font-size: 16px; font-weight: 700; padding: 12px 20px; border-radius: 10px;");
        ui->lblPhase->setText("Phase : Production en cours");
        ui->progressProduction->setValue(50);
        ui->lblProgressEnd->setText(QString("%1 / %2 kg traites").arg(qty / 2).arg(qty));
        ui->lblStartTime->setText(prod.getHeureDemarrage().toString("HH:mm:ss"));

    } else if (statut == "Termine") {
        ui->lblCurrentStatus->setText("Termine");
        ui->lblCurrentStatus->setStyleSheet(
            "background-color: #DCFCE7; color: #166534; border: 2px solid #4ADE80; "
            "font-size: 16px; font-weight: 700; padding: 12px 20px; border-radius: 10px;");
        ui->lblPhase->setText(QString("Termine - %1 L, rendement %2%")
                              .arg(huile, 0, 'f', 1).arg(rendement, 0, 'f', 1));
        ui->progressProduction->setValue(100);
        ui->lblProgressEnd->setText(QString("%1 / %1 kg traites").arg(qty));

    } else if (statut.contains("Qualite")) {
        ui->lblCurrentStatus->setText("Qualite validee");
        ui->lblCurrentStatus->setStyleSheet(
            "background-color: #D1FAE5; color: #065F46; border: 2px solid #34D399; "
            "font-size: 16px; font-weight: 700; padding: 12px 20px; border-radius: 10px;");
        ui->lblPhase->setText(QString("Qualite validee - %1 L, rendement %2%")
                              .arg(huile, 0, 'f', 1).arg(rendement, 0, 'f', 1));
        ui->progressProduction->setValue(100);
        ui->lblProgressEnd->setText(QString("%1 / %1 kg traites").arg(qty));

    } else {
        // Planifié
        ui->lblCurrentStatus->setText("Planifie");
        ui->lblCurrentStatus->setStyleSheet(
            "background-color: #FEF3C7; color: #92400E; border: 2px solid #FCD34D; "
            "font-size: 16px; font-weight: 700; padding: 12px 20px; border-radius: 10px;");
        ui->lblPhase->setText("Phase : En attente de lancement");
        ui->progressProduction->setValue(0);
        ui->lblProgressEnd->setText(QString("0 / %1 kg traites").arg(qty));
        ui->lblStartTime->setText("--:--:--");
        ui->lblElapsedTime->setText("00:00:00");
    }

    // Also fill spin boxes on Planification tab
    ui->spinOlivesKg->setValue(qty);
}

// ============================================================================
// AI TAB SETUP
// ============================================================================

void Production::setupAiTab()
{
    // Create the AI Assistant tab and add it to the existing tab widget
    QWidget *aiTab = new QWidget();
    aiTab->setStyleSheet("background-color: #0d1117; color: #e6edf3;");

    QVBoxLayout *mainLayout = new QVBoxLayout(aiTab);
    mainLayout->setSpacing(12);
    mainLayout->setContentsMargins(16, 16, 16, 16);

    // ── Header ──────────────────────────────────────────────────────────
    QHBoxLayout *headerLayout = new QHBoxLayout();
    QLabel *headerLabel = new QLabel("🤖 PRESS IQ — Assistant Intelligent de Production");
    headerLabel->setStyleSheet(
        "font-size: 18px; font-weight: bold; color: #58a6ff; padding: 8px 0;");
    headerLayout->addWidget(headerLabel);
    headerLayout->addStretch();

    m_btnBotToggle = new QPushButton("🤖 Démarrer Bot Employé");
    m_btnBotToggle->setCursor(Qt::PointingHandCursor);
    m_btnBotToggle->setStyleSheet(
        "QPushButton { background-color: #238636; color: white; border: none; "
        "border-radius: 8px; padding: 10px 22px; font-weight: bold; font-size: 13px; }"
        "QPushButton:hover { background-color: #2ea043; }");
    headerLayout->addWidget(m_btnBotToggle);

    mainLayout->addLayout(headerLayout);

    // ── Status bar ──────────────────────────────────────────────────────
    QHBoxLayout *statusLayout = new QHBoxLayout();
    m_lblBotStatus = new QLabel("🔴 Bot : Inactif");
    m_lblBotStatus->setStyleSheet("font-size: 12px; color: #8b949e; padding: 4px 8px; "
                                  "background: #161b22; border-radius: 4px;");
    m_lblVoiceStatus = new QLabel("🎤 Voix : Désactivée");
    m_lblVoiceStatus->setStyleSheet("font-size: 12px; color: #8b949e; padding: 4px 8px; "
                                    "background: #161b22; border-radius: 4px;");
    statusLayout->addWidget(m_lblBotStatus);
    statusLayout->addWidget(m_lblVoiceStatus);
    statusLayout->addStretch();
    mainLayout->addLayout(statusLayout);

    // ── Chat display ────────────────────────────────────────────────────
    m_aiChatDisplay = new QTextEdit();
    m_aiChatDisplay->setReadOnly(true);
    m_aiChatDisplay->setMinimumHeight(200);
    m_aiChatDisplay->setStyleSheet(
        "QTextEdit {"
        "  background-color: #161b22; color: #e6edf3; border: 1px solid #30363d;"
        "  border-radius: 8px; font-size: 13px; padding: 10px;"
        "  font-family: 'Segoe UI', Consolas, monospace;"
        "}");
    m_aiChatDisplay->setHtml(
        "<p style='color:#58a6ff;'>🤖 <b>PRESS IQ</b> : Bienvenue ! Je suis votre assistant "
        "intelligent de production. Tapez une commande ou dites 'aide' pour voir les options.</p>");
    mainLayout->addWidget(m_aiChatDisplay);

    // ── Quick action buttons ────────────────────────────────────────────
    QGroupBox *actionsBox = new QGroupBox("⚡ Actions Rapides");
    actionsBox->setStyleSheet(
        "QGroupBox { font-weight: bold; color: #58a6ff; border: 1px solid #30363d; "
        "border-radius: 8px; padding-top: 16px; margin-top: 8px; }"
        "QGroupBox::title { subcontrol-origin: margin; left: 12px; padding: 0 6px; }");
    QHBoxLayout *actionsLayout = new QHBoxLayout(actionsBox);

    auto makeBtn = [](const QString &text, const QString &bg) {
        QPushButton *btn = new QPushButton(text);
        btn->setStyleSheet(
            QString("QPushButton { background-color: %1; color: white; border: none; "
                    "border-radius: 6px; padding: 8px 16px; font-weight: 600; font-size: 12px; }"
                    "QPushButton:hover { opacity: 0.9; }").arg(bg));
        btn->setCursor(Qt::PointingHandCursor);
        return btn;
    };

    m_btnAiAnalyze  = makeBtn("🔍 Analyser", "#238636");
    m_btnAiOptimize = makeBtn("⚙️ Optimiser", "#1f6feb");
    m_btnVoice      = makeBtn("🎤 Voix", "#bf8700");

    actionsLayout->addWidget(m_btnAiAnalyze);
    actionsLayout->addWidget(m_btnAiOptimize);
    actionsLayout->addWidget(m_btnVoice);
    mainLayout->addWidget(actionsBox);

    // ── Bot decisions list ──────────────────────────────────────────────
    QGroupBox *decisionsBox = new QGroupBox("📝 Décisions du Bot");
    decisionsBox->setStyleSheet(
        "QGroupBox { font-weight: bold; color: #58a6ff; border: 1px solid #30363d; "
        "border-radius: 8px; padding-top: 16px; margin-top: 8px; }"
        "QGroupBox::title { subcontrol-origin: margin; left: 12px; padding: 0 6px; }");
    QVBoxLayout *decisionsLayout = new QVBoxLayout(decisionsBox);
    m_botDecisionList = new QListWidget();
    m_botDecisionList->setMaximumHeight(120);
    m_botDecisionList->setStyleSheet(
        "QListWidget { background-color: #161b22; color: #e6edf3; border: 1px solid #30363d; "
        "border-radius: 6px; font-size: 12px; }"
        "QListWidget::item { padding: 4px 8px; border-bottom: 1px solid #21262d; }");
    decisionsLayout->addWidget(m_botDecisionList);
    mainLayout->addWidget(decisionsBox);

    // ── Input bar ───────────────────────────────────────────────────────
    QHBoxLayout *inputLayout = new QHBoxLayout();
    m_aiCommandInput = new QLineEdit();
    m_aiCommandInput->setPlaceholderText("Tapez une commande... (ex: 'analyser production 1', 'aide')");
    m_aiCommandInput->setStyleSheet(
        "QLineEdit { background-color: #161b22; color: #e6edf3; border: 1px solid #30363d; "
        "border-radius: 8px; padding: 10px 14px; font-size: 13px; }"
        "QLineEdit:focus { border-color: #58a6ff; }");

    m_btnSendAi = new QPushButton("Envoyer");
    m_btnSendAi->setStyleSheet(
        "QPushButton { background-color: #238636; color: white; border: none; "
        "border-radius: 8px; padding: 10px 20px; font-weight: bold; font-size: 13px; }"
        "QPushButton:hover { background-color: #2ea043; }");
    m_btnSendAi->setCursor(Qt::PointingHandCursor);

    inputLayout->addWidget(m_aiCommandInput);
    inputLayout->addWidget(m_btnSendAi);
    mainLayout->addLayout(inputLayout);

    // Add the tab to the existing tab widget
    ui->tabWidgetProduction->addTab(aiTab, "🤖 Assistant IA");

    // ── Connections ─────────────────────────────────────────────────────
    connect(m_btnSendAi, &QPushButton::clicked, this, &Production::onSendAiCommand);
    connect(m_aiCommandInput, &QLineEdit::returnPressed, this, &Production::onSendAiCommand);
    connect(m_btnVoice, &QPushButton::clicked, this, &Production::onVoiceToggle);
    connect(m_btnBotToggle, &QPushButton::clicked, this, &Production::onBotToggle);
    connect(m_btnAiAnalyze, &QPushButton::clicked, this, &Production::onAiAnalyzeClicked);
    connect(m_btnAiOptimize, &QPushButton::clicked, this, &Production::onAiOptimizeClicked);
}

// ============================================================================
// AI CHAT HELPER
// ============================================================================

void Production::appendAiChat(const QString &sender, const QString &message, const QString &color)
{
    QString html = QString("<p style='margin:4px 0;'>"
                           "<span style='color:%1; font-weight:bold;'>%2</span> : "
                           "<span style='color:#e6edf3;'>%3</span></p>")
                       .arg(color, sender, message.toHtmlEscaped().replace("\n", "<br>"));
    m_aiChatDisplay->append(html);

    // Auto-scroll to bottom
    QScrollBar *sb = m_aiChatDisplay->verticalScrollBar();
    sb->setValue(sb->maximum());
}

// ============================================================================
// AI SLOTS
// ============================================================================

void Production::onSendAiCommand()
{
    QString text = m_aiCommandInput->text().trimmed();
    if (text.isEmpty()) return;

    appendAiChat("👤 Vous", text, "#7ee787");
    m_aiCommandInput->clear();

    // Process through voice assistant (which handles intent analysis)
    m_voiceAssistant->setVoiceEnabled(false);  // Don't speak on text input
    m_voiceAssistant->processTextCommand(text);
    m_voiceAssistant->setVoiceEnabled(true);
}

void Production::onVoiceToggle()
{
    if (m_voiceAssistant->isListening()) {
        m_voiceAssistant->stopListening();
        m_lblVoiceStatus->setText("🎤 Voix : Désactivée");
        m_lblVoiceStatus->setStyleSheet("font-size: 12px; color: #8b949e; padding: 4px 8px; "
                                        "background: #161b22; border-radius: 4px;");
        m_btnVoice->setText("🎤 Voix");
        appendAiChat("🎤 SYSTÈME", "Écoute vocale désactivée.", "#bf8700");
    } else {
        m_voiceAssistant->startListening();
        m_lblVoiceStatus->setText("🎤 Voix : Active — Parlez...");
        m_lblVoiceStatus->setStyleSheet("font-size: 12px; color: #7ee787; padding: 4px 8px; "
                                        "background: #161b22; border-radius: 4px;");
        m_btnVoice->setText("🔴 Arrêter Voix");
        appendAiChat("🎤 SYSTÈME", "Écoute vocale activée. Parlez maintenant...", "#7ee787");
    }
}

void Production::onBotToggle()
{
    if (m_productionBot->isRunning()) {
        m_productionBot->stop();
        m_lblBotStatus->setText("🔴 Bot Employé : Inactif");
        m_lblBotStatus->setStyleSheet("font-size: 12px; color: #f85149; padding: 4px 8px; "
                                      "background: #161b22; border-radius: 4px;");
        m_btnBotToggle->setText("🤖 Démarrer Bot Employé");
        m_btnBotToggle->setStyleSheet(
            "QPushButton { background-color: #238636; color: white; border: none; "
            "border-radius: 8px; padding: 10px 22px; font-weight: bold; font-size: 13px; }"
            "QPushButton:hover { background-color: #2ea043; }");
        appendAiChat("🤖 BOT", "Bot Employé arrêté.", "#f85149");
    } else {
        m_productionBot->start(10);  // Cycle every 10 seconds
        m_lblBotStatus->setText("🟢 Bot Employé : Actif (10s)");
        m_lblBotStatus->setStyleSheet("font-size: 12px; color: #7ee787; padding: 4px 8px; "
                                      "background: #161b22; border-radius: 4px;");
        m_btnBotToggle->setText("⏹ Arrêter Bot Employé");
        m_btnBotToggle->setStyleSheet(
            "QPushButton { background-color: #da3633; color: white; border: none; "
            "border-radius: 8px; padding: 10px 22px; font-weight: bold; font-size: 13px; }"
            "QPushButton:hover { background-color: #b91c1c; }");
        appendAiChat("🤖 BOT", "Bot Employé activé ! Il va planifier, lancer, produire et contrôler la qualité automatiquement toutes les 10 secondes.", "#7ee787");
    }
}

void Production::onAiAnalyzeClicked()
{
    if (currentProductionId <= 0) {
        appendAiChat("🤖 PRESS IQ",
                     "Aucune production sélectionnée. Sélectionnez une production ou spécifiez un ID.",
                     "#f0883e");
        return;
    }

    ProductionModel prod = m_productionService->getProductionById(currentProductionId);
    if (prod.getIdProduction() <= 0) {
        appendAiChat("🤖 PRESS IQ",
                     QString("Production #%1 introuvable.").arg(currentProductionId), "#f85149");
        return;
    }

    SmartProductionAssistant::AnalysisResult analysis = m_smartAssistant->analyserProduction(prod);

    QString msg = QString("📊 Analyse de la production #%1\n\n"
                          "Score : %2/100 — Niveau : %3\n"
                          "Résumé : %4\n")
                      .arg(currentProductionId)
                      .arg(analysis.score)
                      .arg(analysis.niveau)
                      .arg(analysis.summary);

    if (!analysis.suggestions.isEmpty()) {
        msg += "\n💡 Suggestions :\n";
        for (const auto &s : analysis.suggestions)
            msg += "  • " + s + "\n";
    }
    if (!analysis.warnings.isEmpty()) {
        msg += "\n⚠️ Alertes :\n";
        for (const auto &w : analysis.warnings)
            msg += "  • " + w + "\n";
    }

    appendAiChat("🤖 PRESS IQ", msg, "#58a6ff");
}

void Production::onAiOptimizeClicked()
{
    if (currentProductionId <= 0) {
        appendAiChat("🤖 PRESS IQ",
                     "Sélectionnez une production pour obtenir un plan d'optimisation.",
                     "#f0883e");
        return;
    }

    ProductionModel prod = m_productionService->getProductionById(currentProductionId);
    if (prod.getIdProduction() <= 0) {
        appendAiChat("🤖 PRESS IQ",
                     QString("Production #%1 introuvable.").arg(currentProductionId), "#f85149");
        return;
    }

    SmartProductionAssistant::OptimisationPlan plan = m_smartAssistant->proposerOptimisation(prod);

    QString msg = QString("⚙️ Plan d'Optimisation — Production #%1\n\n"
                          "Type recommandé : %2\n"
                          "Durée optimale  : %3 min\n"
                          "Rendement cible : %.1f %%\n")
                      .arg(currentProductionId)
                      .arg(plan.typePressageRecommande)
                      .arg(plan.dureeOptimale)
                      .arg(plan.rendementCible);

    if (!plan.actions.isEmpty()) {
        msg += "\n📋 Actions recommandées :\n";
        for (int i = 0; i < plan.actions.size(); ++i)
            msg += QString("  %1. %2\n").arg(i + 1).arg(plan.actions[i]);
    }

    appendAiChat("🤖 PRESS IQ", msg, "#8957e5");
}

void Production::onVoiceResponse(const QString &response)
{
    appendAiChat("🤖 PRESS IQ", response, "#58a6ff");
}

void Production::onBotDecision(const BotDecision &decision)
{
    QString icon;
    if (decision.type == "planification") icon = "📋";
    else if (decision.type == "lancement") icon = "▶️";
    else if (decision.type == "production") icon = "🏭";
    else if (decision.type == "qualite")   icon = "✅";
    else if (decision.type == "anomalie")  icon = "🚨";
    else                                   icon = "⚙️";

    QString item = QString("%1 [%2] #%3 — %4")
                       .arg(icon)
                       .arg(decision.dateDecision.toString("HH:mm:ss"))
                       .arg(decision.idProduction)
                       .arg(decision.description.left(100));

    m_botDecisionList->insertItem(0, item);

    // Color the item based on impact
    QListWidgetItem *listItem = m_botDecisionList->item(0);
    if (decision.impact == "positif") {
        listItem->setForeground(QColor("#7ee787"));
    } else if (decision.impact == "negatif") {
        listItem->setForeground(QColor("#f85149"));
    } else {
        listItem->setForeground(QColor("#e6edf3"));
    }

    // Keep only last 100 items
    while (m_botDecisionList->count() > 100)
        delete m_botDecisionList->takeItem(m_botDecisionList->count() - 1);

    // Also show in chat
    appendAiChat(icon + " BOT", decision.description, 
                 decision.impact == "negatif" ? "#f85149" : "#58a6ff");
}