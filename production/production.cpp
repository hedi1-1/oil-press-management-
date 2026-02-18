#include "production.h"
#include "ui_production.h"
#include <QDateTime>

Production::Production(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Production)
    , currentProductionId(0)
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
    
    // Load production history on startup
    loadProductionHistory();
}

Production::~Production()
{
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
    if (currentProductionId == 0) {
        showErrorNotification("Aucune production planifiée!\n\nPlanifiez d'abord une production.");
        return;
    }
    
    currentProduction.setIdProduction(currentProductionId);
    currentProduction.setStatut("En cours");
    currentProduction.setHeureDemarrage(QTime::currentTime());
    currentProduction.setTempsEcoule(0);
    
    // Get current form values
    currentProduction.setQuantiteOlivesKg(ui->spinOlivesKg->value());
    currentProduction.setTypePressage(ui->comboPressType->currentText());
    
    if (currentProduction.updateProduction()) {
        showSuccessNotification("▶ Production démarrée!\n\n"
                               "Heure de démarrage: " + QTime::currentTime().toString("HH:mm:ss") + "\n"
                               "Statut: En cours");
    } else {
        showErrorNotification("Échec du démarrage de la production!");
    }
}

// ============================================================================
// CRUD: UPDATE STATUS - STOP
// ============================================================================

void Production::onStopClicked()
{
    if (currentProductionId == 0) {
        showErrorNotification("Aucune production en cours!");
        return;
    }
    
    currentProduction.setIdProduction(currentProductionId);
    currentProduction.setStatut("Arrêté");
    
    if (currentProduction.updateProduction()) {
        showSuccessNotification("⏹ Production arrêtée!\n\n"
                               "La production a été mise en pause.");
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
    
    // Resize columns to content
    ui->tableProductionHistory->resizeColumnsToContents();
    ui->tableProductionHistory->horizontalHeader()->setStretchLastSection(true);
    
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
