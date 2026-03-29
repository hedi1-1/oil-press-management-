/**
 * @file mainwindow.cpp
 * @brief Implémentation de la fenêtre principale
 */

#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QVBoxLayout>
#include <QMessageBox>
#include <QTableWidgetItem>
#include <QDate>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QChart>
#include <QChartView>
#include <QBarSet>
#include <QBarSeries>
#include <QBarCategoryAxis>
#include <QValueAxis>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_selectedTransactionId(-1)
{
    ui->setupUi(this);
    
    // Créer les objets métier
    m_database = new Database();
    m_logic = new BusinessLogic(m_database);
    
    // Initialiser l'application
    initializeApplication();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete m_logic;
    delete m_database;
}

/**
 * ===== INITIALISATION =====
 */

void MainWindow::initializeApplication()
{
    // Initialiser la base de données
    if (!m_database->initializeDatabase())
    {
        showError("Erreur: Impossible d'initialiser la base de données");
        return;
    }
    
    // Configurer les connexions de signaux/slots
    setupConnections();
    
    // Charger les catégories
    loadCategories();
    
    // Définir la date d'aujourd'hui
    setTodayDate();
    
    // Charger les transactions
    loadTransactions();
    
    // Mettre à jour les statistiques
    updateStatistics();
    
    // Afficher le rapport initial
    ui->textEdit_report->setPlainText(m_logic->generateReport());
    
    showSuccess("Application initialisée avec succès");
}

void MainWindow::setupConnections()
{
    // ===== Transactions =====
    connect(ui->btn_add, &QPushButton::clicked, this, &MainWindow::onAddTransaction);
    connect(ui->btn_modify, &QPushButton::clicked, this, &MainWindow::onModifyTransaction);
    connect(ui->btn_delete, &QPushButton::clicked, this, &MainWindow::onDeleteTransaction);
    connect(ui->btn_clear, &QPushButton::clicked, this, &MainWindow::onClearForm);
    
    connect(ui->tableWidget_transactions, &QTableWidget::cellClicked, 
            this, &MainWindow::onTransactionSelected);
    
    connect(ui->lineEdit_search, &QLineEdit::textChanged, 
            this, &MainWindow::onSearchTransactions);
    
    // ===== Onglet Tab Change pour mise à jour =====
    connect(ui->tabWidget, QOverload<int>::of(&QTabWidget::currentChanged),
            [this](int index) {
        if (index == 1) // Onglet Statistiques
            updateStatistics();
        else if (index == 2) // Onglet Rapports
            ui->textEdit_report->setPlainText(m_logic->generateReport());
    });
    
    // ===== Rapports =====
    connect(ui->btn_exportPDF, &QPushButton::clicked, this, &MainWindow::onExportPDF);
    connect(ui->btn_exportExcel, &QPushButton::clicked, this, &MainWindow::onExportExcel);
    connect(ui->btn_exportCSV, &QPushButton::clicked, this, &MainWindow::onExportCSV);
    connect(ui->btn_exportTXT, &QPushButton::clicked, this, &MainWindow::onExportTXT);
    
    // ===== Prévisions =====
    connect(ui->btn_calculateForecast, &QPushButton::clicked, 
            this, &MainWindow::onCalculateForecast);
}

void MainWindow::loadCategories()
{
    // Catégories prédéfinies
    QStringList defaultCategories = {
        "Production",
        "Vente",
        "Maintenance",
        "Électricité",
        "Carburant",
        "Salaires",
        "Transport",
        "Emballage",
        "Divers"
    };
    
    // Récupérer les catégories existantes
    QStringList categories = m_logic->getCategories();
    
    for (const auto &cat : defaultCategories)
    {
        if (!categories.contains(cat))
            categories.append(cat);
    }
    
    categories.sort();
    
    // Charger dans le ComboBox
    ui->comboBox_categorie->clear();
    ui->comboBox_categorie->addItem("Sélectionner...");
    for (const auto &cat : categories)
    {
        ui->comboBox_categorie->addItem(cat);
    }
}

void MainWindow::loadTransactions()
{
    ui->tableWidget_transactions->setRowCount(0);
    
    auto transactions = m_database->getAllTransactions();
    
    for (int i = 0; i < transactions.size(); ++i)
    {
        const auto &trans = transactions[i];
        ui->tableWidget_transactions->insertRow(i);
        
        ui->tableWidget_transactions->setItem(i, 0, 
            new QTableWidgetItem(trans["id_transaction"].toString()));
        ui->tableWidget_transactions->setItem(i, 1, 
            new QTableWidgetItem(trans["type"].toString()));
        ui->tableWidget_transactions->setItem(i, 2, 
            new QTableWidgetItem(trans["categorie"].toString()));
        ui->tableWidget_transactions->setItem(i, 3, 
            new QTableWidgetItem(QString::number(trans["montant"].toDouble(), 'f', 2)));
        ui->tableWidget_transactions->setItem(i, 4, 
            new QTableWidgetItem(trans["date_trans"].toString()));
        ui->tableWidget_transactions->setItem(i, 5, 
            new QTableWidgetItem(trans["description"].toString()));
    }
}

void MainWindow::loadFilteredTransactions(const QString &filter)
{
    ui->tableWidget_transactions->setRowCount(0);
    
    auto transactions = m_database->getAllTransactions();
    int row = 0;
    
    for (const auto &trans : transactions)
    {
        QString categorie = trans["categorie"].toString();
        QString description = trans["description"].toString();
        
        if (categorie.contains(filter, Qt::CaseInsensitive) ||
            description.contains(filter, Qt::CaseInsensitive))
        {
            ui->tableWidget_transactions->insertRow(row);
            
            ui->tableWidget_transactions->setItem(row, 0, 
                new QTableWidgetItem(trans["id_transaction"].toString()));
            ui->tableWidget_transactions->setItem(row, 1, 
                new QTableWidgetItem(trans["type"].toString()));
            ui->tableWidget_transactions->setItem(row, 2, 
                new QTableWidgetItem(trans["categorie"].toString()));
            ui->tableWidget_transactions->setItem(row, 3, 
                new QTableWidgetItem(QString::number(trans["montant"].toDouble(), 'f', 2)));
            ui->tableWidget_transactions->setItem(row, 4, 
                new QTableWidgetItem(trans["date_trans"].toString()));
            ui->tableWidget_transactions->setItem(row, 5, 
                new QTableWidgetItem(trans["description"].toString()));
            
            row++;
        }
    }
}

void MainWindow::setTodayDate()
{
    ui->dateEdit->setDate(QDate::currentDate());
}

bool MainWindow::validateForm()
{
    if (ui->comboBox_categorie->currentIndex() == 0)
    {
        showError("Veuillez sélectionner une catégorie");
        return false;
    }
    
    if (ui->spinBox_montant->value() <= 0)
    {
        showError("Le montant doit être positif");
        return false;
    }
    
    if (ui->textEdit_description->toPlainText().isEmpty())
    {
        showError("Veuillez entrer une description");
        return false;
    }
    
    return true;
}

/**
 * ===== TRANSACTIONS SLOTS =====
 */

void MainWindow::onAddTransaction()
{
    if (!validateForm())
        return;
    
    QString type = ui->comboBox_type->currentText();
    QString categorie = ui->comboBox_categorie->currentText();
    double montant = ui->spinBox_montant->value();
    QString date = ui->dateEdit->date().toString("yyyy-MM-dd");
    QString description = ui->textEdit_description->toPlainText();
    
    if (m_database->addTransaction(type, categorie, montant, date, description))
    {
        showSuccess("Transaction ajoutée avec succès");
        onClearForm();
        loadTransactions();
        updateStatistics();
    }
    else
    {
        showError("Erreur lors de l'ajout: " + m_database->getLastError());
    }
}

void MainWindow::onModifyTransaction()
{
    if (m_selectedTransactionId == -1)
    {
        showError("Veuillez sélectionner une transaction à modifier");
        return;
    }
    
    if (!validateForm())
        return;
    
    QString type = ui->comboBox_type->currentText();
    QString categorie = ui->comboBox_categorie->currentText();
    double montant = ui->spinBox_montant->value();
    QString date = ui->dateEdit->date().toString("yyyy-MM-dd");
    QString description = ui->textEdit_description->toPlainText();
    
    if (m_database->updateTransaction(m_selectedTransactionId, type, categorie, 
                                      montant, date, description))
    {
        showSuccess("Transaction modifiée avec succès");
        onClearForm();
        loadTransactions();
        updateStatistics();
    }
    else
    {
        showError("Erreur lors de la modification");
    }
}

void MainWindow::onDeleteTransaction()
{
    if (m_selectedTransactionId == -1)
    {
        showError("Veuillez sélectionner une transaction à supprimer");
        return;
    }
    
    QMessageBox::StandardButton reply = QMessageBox::question(this,
        "Confirmation",
        "Êtes-vous sûr de vouloir supprimer cette transaction?",
        QMessageBox::Yes | QMessageBox::No);
    
    if (reply == QMessageBox::Yes)
    {
        if (m_database->deleteTransaction(m_selectedTransactionId))
        {
            showSuccess("Transaction supprimée avec succès");
            onClearForm();
            loadTransactions();
            updateStatistics();
        }
        else
        {
            showError("Erreur lors de la suppression");
        }
    }
}

void MainWindow::onClearForm()
{
    ui->comboBox_type->setCurrentIndex(0);
    ui->comboBox_categorie->setCurrentIndex(0);
    ui->spinBox_montant->setValue(0.0);
    ui->textEdit_description->clear();
    setTodayDate();
    
    m_selectedTransactionId = -1;
    ui->tableWidget_transactions->clearSelection();
    
    // Désactiver les boutons Modifier et Supprimer
    ui->btn_modify->setEnabled(false);
    ui->btn_delete->setEnabled(false);
}

void MainWindow::onTransactionSelected(int row, int column)
{
    Q_UNUSED(column);
    
    int id = ui->tableWidget_transactions->item(row, 0)->text().toInt();
    m_selectedTransactionId = id;
    
    // Activer les boutons Modifier et Supprimer
    ui->btn_modify->setEnabled(true);
    ui->btn_delete->setEnabled(true);
    
    // Récupérer les données de la transaction
    auto transactions = m_database->getAllTransactions();
    
    for (const auto &trans : transactions)
    {
        if (trans["id_transaction"].toInt() == id)
        {
            ui->comboBox_type->setCurrentText(trans["type"].toString());
            ui->comboBox_categorie->setCurrentText(trans["categorie"].toString());
            ui->spinBox_montant->setValue(trans["montant"].toDouble());
            ui->dateEdit->setDate(QDate::fromString(trans["date_trans"].toString(), "yyyy-MM-dd"));
            ui->textEdit_description->setPlainText(trans["description"].toString());
            
            break;
        }
    }
}

void MainWindow::onSearchTransactions()
{
    QString filter = ui->lineEdit_search->text();
    
    if (filter.isEmpty())
    {
        loadTransactions();
    }
    else
    {
        loadFilteredTransactions(filter);
    }
}

/**
 * ===== STATISTIQUES SLOTS =====
 */

void MainWindow::updateStatistics()
{
    double totalRevenu = m_logic->getTotalRevenu();
    double totalDepense = m_logic->getTotalDepense();
    double profit = m_logic->getProfit();
    
    ui->label_totalRevenue->setText(QString::number(totalRevenu, 'f', 2) + " L");
    ui->label_totalExpense->setText(QString::number(totalDepense, 'f', 2) + " L");
    ui->label_profit->setText(QString::number(profit, 'f', 2) + " L");
    
    // Mettre à couleur du profit
    if (profit >= 0)
        ui->label_profit->setStyleSheet("QLabel { font-size: 24px; font-weight: bold; color: #4CAF50; }");
    else
        ui->label_profit->setStyleSheet("QLabel { font-size: 24px; font-weight: bold; color: #f44336; }");
    
    createChart();
}

void MainWindow::createChart()
{
    // Obtenir les données
    double totalRevenue = m_logic->getTotalRevenu();
    double totalExpense = m_logic->getTotalDepense();
    
    // Créer les barres
    QBarSet *revenueSet = new QBarSet("Revenus");
    revenueSet->append(totalRevenue);
    revenueSet->setColor(QColor(76, 175, 80)); // Vert
    
    QBarSet *expenseSet = new QBarSet("Dépenses");
    expenseSet->append(totalExpense);
    expenseSet->setColor(QColor(244, 67, 54)); // Rouge
    
    // Créer la série
    QBarSeries *series = new QBarSeries();
    series->append(revenueSet);
    series->append(expenseSet);
    
    // Créer le graphique
    QtCharts::QChart *chart = new QtCharts::QChart();
    chart->addSeries(series);
    chart->setTitle("Revenus vs Dépenses");
    chart->setAnimationOptions(QtCharts::QChart::SeriesAnimations);
    
    // Configurer les axes
    QStringList categories;
    categories << "Montants (L)";
    
    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    axisX->append(categories);
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);
    
    QValueAxis *axisY = new QValueAxis();
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);
    
    // Afficher dans le widget
    QtCharts::QChartView *chartView = new QtCharts::QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    
    // Nettoyer l'ancien graphique
    QLayout *layout = ui->widget_chart->layout();
    if (layout == nullptr)
    {
        layout = new QVBoxLayout(ui->widget_chart);
        ui->widget_chart->setLayout(layout);
    }
    else
    {
        QLayoutItem *item;
        while ((item = layout->takeAt(0)) != nullptr)
        {
            delete item->widget();
            delete item;
        }
    }
    
    layout->addWidget(chartView);
}

/**
 * ===== RAPPORTS SLOTS =====
 */

void MainWindow::onExportPDF()
{
    QString filePath = QFileDialog::getSaveFileName(this,
        "Exporter en PDF", "",
        "PDF Files (*.pdf);;All Files (*)");
    
    if (filePath.isEmpty())
        return;
    
    // Pour PDF, on génère d'abord en HTML puis on exporte
    showSuccess("Fonction PDF en développement. Utilisez plutôt l'export TXT.");
}

void MainWindow::onExportExcel()
{
    QString filePath = QFileDialog::getSaveFileName(this,
        "Exporter en Excel", "",
        "Excel Files (*.xlsx);;All Files (*)");
    
    if (filePath.isEmpty())
        return;
    
    // Pour Excel, utiliser le CSV et le convertir
    showSuccess("Fonction Excel en développement. Utilisez plutôt l'export CSV.");
}

void MainWindow::onExportCSV()
{
    QString filePath = QFileDialog::getSaveFileName(this,
        "Exporter en CSV", "",
        "CSV Files (*.csv);;All Files (*)");
    
    if (filePath.isEmpty())
        return;
    
    if (m_logic->exportToCSV(filePath))
    {
        showSuccess("Export CSV réussi: " + filePath);
    }
    else
    {
        showError("Erreur lors de l'export CSV");
    }
}

void MainWindow::onExportTXT()
{
    QString filePath = QFileDialog::getSaveFileName(this,
        "Exporter en TXT", "",
        "Text Files (*.txt);;All Files (*)");
    
    if (filePath.isEmpty())
        return;
    
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        showError("Impossible d'ouvrir le fichier");
        return;
    }
    
    QTextStream stream(&file);
    stream.setCodec("UTF-8");
    stream << m_logic->generateReport();
    
    file.close();
    showSuccess("Export TXT réussi: " + filePath);
}

/**
 * ===== PRÉVISIONS SLOTS =====
 */

void MainWindow::onCalculateForecast()
{
    double revenueIncrease = ui->spinBox_revenueIncrease->value();
    double expenseIncrease = ui->spinBox_expenseIncrease->value();
    
    // Prévisions simples
    double forecastRevenue = m_logic->forecastNextMonthRevenue();
    double forecastExpense = m_logic->forecastNextMonthExpense();
    double forecastCashFlow = m_logic->forecastCashFlow(1);
    
    // Simulation budgétaire
    double simulatedProfit = m_logic->simulateBudget(revenueIncrease, expenseIncrease);
    
    // Afficher les résultats
    ui->label_forecastRevenue->setText(QString::number(forecastRevenue, 'f', 2) + " L");
    ui->label_forecastExpense->setText(QString::number(forecastExpense, 'f', 2) + " L");
    ui->label_forecastCashFlow->setText(QString::number(forecastCashFlow, 'f', 2) + " L");
    
    ui->label_simulationResult->setText(
        QString("Profit Simulé : %1 L").arg(simulatedProfit, 0, 'f', 2)
    );
    
    // Mettre à couleur appropriée
    if (simulatedProfit >= 0)
        ui->label_simulationResult->setStyleSheet(
            "QLabel { font-size: 18px; font-weight: bold; color: #4CAF50; "
            "padding: 20px; background-color: #f0f0f0; border-radius: 4px; }"
        );
    else
        ui->label_simulationResult->setStyleSheet(
            "QLabel { font-size: 18px; font-weight: bold; color: #f44336; "
            "padding: 20px; background-color: #f0f0f0; border-radius: 4px; }"
        );
}

/**
 * ===== MESSAGES UTILISATEUR =====
 */

void MainWindow::showError(const QString &message)
{
    QMessageBox::warning(this, "Erreur", message);
    ui->statusbar->showMessage("Erreur: " + message, 5000);
}

void MainWindow::showSuccess(const QString &message)
{
    ui->statusbar->showMessage(message, 3000);
}
