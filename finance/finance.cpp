#include "finance.h"
#include "ui_finance.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <Qt>
#include <QGroupBox>
#include <QLabel>
#include <QGridLayout>
#include <QFrame>
#include <QAbstractItemView>
#include <QPixmap>
#include <QCloseEvent>

// ΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉ
// IMPL├ëMENTATION: TransactionTab
// ΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉ
TransactionTab::TransactionTab(QWidget *parent)
    : QWidget(parent)
{
    initializeUI();
    setupConnections();
}

void TransactionTab::initializeUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // ===== Zone Formulaire =====
    QGroupBox *formGroup = new QGroupBox("Transaction Info", this);
    QVBoxLayout *formLayout = new QVBoxLayout(formGroup);

    // Row 1
    QHBoxLayout *row1 = new QHBoxLayout();
    row1->addWidget(new QLabel("ID:"));
    txtId = new QLineEdit();
    txtId->setReadOnly(true);
    row1->addWidget(txtId);
    row1->addWidget(new QLabel("Type:"));
    cbType = new QComboBox();
    cbType->addItems({"Revenu", "D├⌐pense"});
    row1->addWidget(cbType);
    formLayout->addLayout(row1);

    // Row 2
    QHBoxLayout *row2 = new QHBoxLayout();
    row2->addWidget(new QLabel("Cat├⌐gorie:"));
    cbCategorie = new QComboBox();
    cbCategorie->addItems({"Achat", "Vente", "Autre"});
    row2->addWidget(cbCategorie);
    row2->addWidget(new QLabel("Montant:"));
    spinMontant = new QDoubleSpinBox();
    spinMontant->setMaximum(999999.99);
    row2->addWidget(spinMontant);
    formLayout->addLayout(row2);

    // Row 3
    QHBoxLayout *row3 = new QHBoxLayout();
    row3->addWidget(new QLabel("Date:"));
    dateEdit = new QDateEdit();
    dateEdit->setDate(QDate::currentDate());
    row3->addWidget(dateEdit);
    row3->addWidget(new QLabel("Client:"));
    cbClient = new QComboBox();
    cbClient->addItems({"Client 1", "Client 2", "Client 3"});
    row3->addWidget(cbClient);
    formLayout->addLayout(row3);

    // Row 4
    formLayout->addWidget(new QLabel("Description:"));
    txtDescription = new QTextEdit();
    txtDescription->setMaximumHeight(80);
    formLayout->addWidget(txtDescription);

    // Row 5 - Buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    btnAdd = new QPushButton("Ajouter");
    btnUpdate = new QPushButton("Modifier");
    btnClear = new QPushButton("R├⌐initialiser");
    buttonLayout->addWidget(btnAdd);
    buttonLayout->addWidget(btnUpdate);
    buttonLayout->addWidget(btnClear);
    buttonLayout->addStretch();
    formLayout->addLayout(buttonLayout);

    mainLayout->addWidget(formGroup);

    // ===== Zone Tableau =====
    QGroupBox *tableGroup = new QGroupBox("Liste des Transactions", this);
    QVBoxLayout *tableLayout = new QVBoxLayout(tableGroup);

    tableTransaction = new QTableWidget();
    tableTransaction->setColumnCount(8);
    tableTransaction->setHorizontalHeaderLabels({"ID", "Type", "Cat├⌐gorie", "Montant", "Date", "Client", "Description", "Actions"});
    
    // Ajuster automatiquement la largeur des colonnes
    tableTransaction->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    
    tableLayout->addWidget(tableTransaction);

    QHBoxLayout *tableButtonLayout = new QHBoxLayout();
    btnDelete = new QPushButton("Supprimer");
    btnRefresh = new QPushButton("Rafra├«chir");
    tableButtonLayout->addWidget(btnDelete);
    tableButtonLayout->addWidget(btnRefresh);
    tableButtonLayout->addStretch();
    tableLayout->addLayout(tableButtonLayout);

    mainLayout->addWidget(tableGroup);
    setLayout(mainLayout);
}

void TransactionTab::setupConnections()
{
    connect(btnAdd, &QPushButton::clicked, this, &TransactionTab::ajouterTransaction);
    connect(btnUpdate, &QPushButton::clicked, this, &TransactionTab::modifierTransaction);
    connect(btnClear, &QPushButton::clicked, this, &TransactionTab::effacerFormulaire);
    connect(btnDelete, &QPushButton::clicked, this, &TransactionTab::supprimerTransaction);
    connect(btnRefresh, &QPushButton::clicked, this, &TransactionTab::chargerTransactions);
}

void TransactionTab::ajouterTransaction()
{
    // ├Ç impl├⌐menter
}

void TransactionTab::modifierTransaction()
{
    // ├Ç impl├⌐menter
}

void TransactionTab::supprimerTransaction()
{
    // ├Ç impl├⌐menter
}

void TransactionTab::chargerTransactions()
{
    // ├Ç impl├⌐menter
}

void TransactionTab::effacerFormulaire()
{
    txtId->clear();
    cbType->setCurrentIndex(0);
    cbCategorie->setCurrentIndex(0);
    spinMontant->setValue(0);
    dateEdit->setDate(QDate::currentDate());
    txtDescription->clear();
    cbClient->setCurrentIndex(0);
}

// ΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉ
// IMPL├ëMENTATION: SearchTab
// ΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉ
SearchTab::SearchTab(QWidget *parent)
    : QWidget(parent)
{
    initializeUI();
    setupConnections();
}

void SearchTab::initializeUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // ===== Groupe Recherche =====
    QGroupBox *searchGroup = new QGroupBox("Recherche", this);
    QVBoxLayout *searchLayout = new QVBoxLayout(searchGroup);

    QHBoxLayout *row1 = new QHBoxLayout();
    row1->addWidget(new QLabel("Type de recherche:"));
    cbSearchType = new QComboBox();
    cbSearchType->addItems({"Client", "Cat├⌐gorie", "Montant", "Date"});
    row1->addWidget(cbSearchType);
    row1->addStretch();
    searchLayout->addLayout(row1);

    QHBoxLayout *row2 = new QHBoxLayout();
    row2->addWidget(new QLabel("Rechercher:"));
    txtSearch = new QLineEdit();
    row2->addWidget(txtSearch);
    searchLayout->addLayout(row2);

    QHBoxLayout *row3 = new QHBoxLayout();
    row3->addWidget(new QLabel("De:"));
    dateFrom = new QDateEdit();
    dateFrom->setDate(QDate::currentDate());
    row3->addWidget(dateFrom);
    row3->addWidget(new QLabel("├Ç:"));
    dateTo = new QDateEdit();
    dateTo->setDate(QDate::currentDate());
    row3->addWidget(dateTo);
    searchLayout->addLayout(row3);

    btnSearch = new QPushButton("Rechercher");
    searchLayout->addWidget(btnSearch);

    mainLayout->addWidget(searchGroup);

    // ===== R├⌐sultats =====
    QGroupBox *resultGroup = new QGroupBox("R├⌐sultats", this);
    QVBoxLayout *resultLayout = new QVBoxLayout(resultGroup);
    tableResult = new QTableWidget();
    tableResult->setColumnCount(7);
    tableResult->setHorizontalHeaderLabels({"ID", "Type", "Cat├⌐gorie", "Montant", "Date", "Client", "Description"});
    
    // Ajuster automatiquement la largeur des colonnes
    tableResult->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    
    resultLayout->addWidget(tableResult);
    mainLayout->addWidget(resultGroup);

    // ===== Groupe Tri =====
    QGroupBox *sortGroup = new QGroupBox("Tri", this);
    QHBoxLayout *sortLayout = new QHBoxLayout(sortGroup);

    sortLayout->addWidget(new QLabel("Trier par:"));
    cbSortBy = new QComboBox();
    cbSortBy->addItems({"Date", "Montant", "Client", "Cat├⌐gorie"});
    sortLayout->addWidget(cbSortBy);

    sortLayout->addWidget(new QLabel("Ordre:"));
    cbOrder = new QComboBox();
    cbOrder->addItems({"Croissant", "D├⌐croissant"});
    sortLayout->addWidget(cbOrder);

    btnSort = new QPushButton("Trier");
    sortLayout->addWidget(btnSort);
    sortLayout->addStretch();

    mainLayout->addWidget(sortGroup);

    setLayout(mainLayout);
}

void SearchTab::setupConnections()
{
    connect(btnSearch, &QPushButton::clicked, this, &SearchTab::rechercher);
    connect(btnSort, &QPushButton::clicked, this, &SearchTab::trier);
}

void SearchTab::rechercher()
{
    // ├Ç impl├⌐menter
}

void SearchTab::trier()
{
    // ├Ç impl├⌐menter
}

// ΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉ
// IMPL├ëMENTATION: StatsTab
// ΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉ
StatsTab::StatsTab(QWidget *parent)
    : QWidget(parent)
{
    initializeUI();
    setupConnections();
}

void StatsTab::initializeUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(15);
    mainLayout->setContentsMargins(10, 10, 10, 10);

    // ===== R├⌐sum├⌐ Financier =====
    mainLayout->addWidget(createSummarySection());

    // ===== S├⌐lection Type Graphique =====
    mainLayout->addWidget(createChartControlSection());

    // ===== Zone Graphique =====
    mainLayout->addWidget(createChartSection());

    mainLayout->addStretch();
    setLayout(mainLayout);
}

QGroupBox* StatsTab::createSummarySection()
{
    QGroupBox *summaryGroup = new QGroupBox("≡ƒôè R├⌐sum├⌐ Financier", this);
    summaryGroup->setStyleSheet("font-size: 11pt;");

    QGridLayout *gridLayout = new QGridLayout(summaryGroup);
    gridLayout->setSpacing(20);
    gridLayout->setContentsMargins(15, 15, 15, 15);

    // Card 1: Revenus
    QVBoxLayout *revLayout = new QVBoxLayout();
    QLabel *lblRevenuTitle = new QLabel("≡ƒÆ░ Total Revenus");
    lblRevenuTitle->setStyleSheet("font-weight: bold; font-size: 11pt; color: #1B4332;");
    lblTotalRevenus = new QLabel("0.00 DT");
    lblTotalRevenus->setStyleSheet("font-weight: bold; font-size: 16pt; color: #27ae60;");
    lblTotalRevenus->setAlignment(Qt::AlignCenter);
    revLayout->addWidget(lblRevenuTitle);
    revLayout->addWidget(lblTotalRevenus);

    QWidget *revCard = new QWidget();
    revCard->setLayout(revLayout);
    revCard->setStyleSheet("background-color: #f0f8f4; border: 2px solid #27ae60; border-radius: 8px; padding: 15px;");

    // Card 2: D├⌐penses
    QVBoxLayout *depLayout = new QVBoxLayout();
    QLabel *lblDepenseTitle = new QLabel("≡ƒÆ╕ Total D├⌐penses");
    lblDepenseTitle->setStyleSheet("font-weight: bold; font-size: 11pt; color: #1B4332;");
    lblTotalDepenses = new QLabel("0.00 DT");
    lblTotalDepenses->setStyleSheet("font-weight: bold; font-size: 16pt; color: #e74c3c;");
    lblTotalDepenses->setAlignment(Qt::AlignCenter);
    depLayout->addWidget(lblDepenseTitle);
    depLayout->addWidget(lblTotalDepenses);

    QWidget *depCard = new QWidget();
    depCard->setLayout(depLayout);
    depCard->setStyleSheet("background-color: #fef5f5; border: 2px solid #e74c3c; border-radius: 8px; padding: 15px;");

    // Card 3: B├⌐n├⌐fice
    QVBoxLayout *benLayout = new QVBoxLayout();
    QLabel *lblBeneficeTitle = new QLabel("≡ƒÄ» B├⌐n├⌐fice Net");
    lblBeneficeTitle->setStyleSheet("font-weight: bold; font-size: 11pt; color: #1B4332;");
    lblBenefice = new QLabel("0.00 DT");
    lblBenefice->setStyleSheet("font-weight: bold; font-size: 16pt; color: #1B4332;");
    lblBenefice->setAlignment(Qt::AlignCenter);
    benLayout->addWidget(lblBeneficeTitle);
    benLayout->addWidget(lblBenefice);

    QWidget *benCard = new QWidget();
    benCard->setLayout(benLayout);
    benCard->setStyleSheet("background-color: #f5f9f8; border: 2px solid #1B4332; border-radius: 8px; padding: 15px;");

    // Ajouter les cartes au grid
    gridLayout->addWidget(revCard, 0, 0);
    gridLayout->addWidget(depCard, 0, 1);
    gridLayout->addWidget(benCard, 0, 2);

    return summaryGroup;
}

QGroupBox* StatsTab::createChartControlSection()
{
    QGroupBox *controlGroup = new QGroupBox("ΓÜÖ∩╕Å Configuration Graphique", this);
    QHBoxLayout *controlLayout = new QHBoxLayout(controlGroup);
    controlLayout->setSpacing(10);

    QLabel *lblType = new QLabel("Type de graphique:");
    lblType->setStyleSheet("font-weight: bold;");
    cbChartType = new QComboBox();
    cbChartType->addItems({"Camembert (Revenus/D├⌐penses)", "Histogramme Mensuel", "Courbe Tendance", "Comparatif Cat├⌐gories"});
    cbChartType->setMinimumWidth(250);

    btnGenerate = new QPushButton("≡ƒöä G├⌐n├⌐rer Graphique");
    btnGenerate->setMinimumWidth(150);
    btnGenerate->setMinimumHeight(35);

    controlLayout->addWidget(lblType);
    controlLayout->addWidget(cbChartType);
    controlLayout->addWidget(btnGenerate);
    controlLayout->addStretch();

    return controlGroup;
}

QGroupBox* StatsTab::createChartSection()
{
    QGroupBox *chartGroup = new QGroupBox("≡ƒôê Visualisation", this);
    QVBoxLayout *chartLayout = new QVBoxLayout(chartGroup);
    chartLayout->setContentsMargins(10, 10, 10, 10);

    chartView = new QWidget();
    chartView->setMinimumHeight(400);
    chartView->setStyleSheet(
        "background-color: #ffffff; "
        "border: 2px solid #ddd; "
        "border-radius: 8px; "
        "background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "stop:0 #fafafa, stop:1 #f5f5f5);"
    );

    // Placeholder texte
    QVBoxLayout *placeholderLayout = new QVBoxLayout(chartView);
    QLabel *placeholderLabel = new QLabel("S├⌐lectionnez un type et cliquez sur 'G├⌐n├⌐rer Graphique'");
    placeholderLabel->setAlignment(Qt::AlignCenter);
    placeholderLabel->setStyleSheet("color: #999; font-size: 12pt;");
    placeholderLayout->addStretch();
    placeholderLayout->addWidget(placeholderLabel);
    placeholderLayout->addStretch();

    chartLayout->addWidget(chartView);

    return chartGroup;
}

void StatsTab::setupConnections()
{
    connect(btnGenerate, &QPushButton::clicked, this, &StatsTab::afficherGraphique);
}

void StatsTab::calculerStats()
{
    // ├Ç impl├⌐menter
}

void StatsTab::afficherGraphique()
{
    // ├Ç impl├⌐menter
}

// ΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉ
// IMPL├ëMENTATION: JournalTab
// ΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉ
JournalTab::JournalTab(QWidget *parent)
    : QWidget(parent)
{
    initializeUI();
    setupConnections();
}

void JournalTab::initializeUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // ===== Filtres =====
    QGroupBox *filterGroup = new QGroupBox("Filtres", this);
    QVBoxLayout *filterLayout = new QVBoxLayout(filterGroup);

    QHBoxLayout *filterRow1 = new QHBoxLayout();
    filterRow1->addWidget(new QLabel("Date:"));
    dateFilter = new QDateEdit();
    dateFilter->setDate(QDate::currentDate());
    filterRow1->addWidget(dateFilter);
    filterLayout->addLayout(filterRow1);

    QHBoxLayout *filterRow2 = new QHBoxLayout();
    filterRow2->addWidget(new QLabel("Type:"));
    cbTypeFilter = new QComboBox();
    cbTypeFilter->addItems({"Tous", "Revenu", "D├⌐pense"});
    filterRow2->addWidget(cbTypeFilter);
    filterRow2->addWidget(new QLabel("Cat├⌐gorie:"));
    cbCatFilter = new QComboBox();
    cbCatFilter->addItems({"Tous", "Achat", "Vente", "Autre"});
    filterRow2->addWidget(cbCatFilter);
    filterLayout->addLayout(filterRow2);

    btnFilter = new QPushButton("Appliquer Filtres");
    filterLayout->addWidget(btnFilter);

    mainLayout->addWidget(filterGroup);

    // ===== Tableau Journal =====
    QGroupBox *journalGroup = new QGroupBox("Journal des Transactions", this);
    QVBoxLayout *journalLayout = new QVBoxLayout(journalGroup);

    tableJournal = new QTableWidget();
    tableJournal->setColumnCount(7);
    tableJournal->setHorizontalHeaderLabels({"Date", "Type", "Cat├⌐gorie", "Montant", "Client", "Description", "Statut"});
    
    // Ajuster automatiquement la largeur des colonnes
    tableJournal->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    
    journalLayout->addWidget(tableJournal);

    mainLayout->addWidget(journalGroup);

    setLayout(mainLayout);
}

void JournalTab::setupConnections()
{
    connect(btnFilter, &QPushButton::clicked, this, &JournalTab::filtrerJournal);
}

void JournalTab::filtrerJournal()
{
    // ├Ç impl├⌐menter
}

void JournalTab::afficherHistorique()
{
    // ├Ç impl├⌐menter
}

// ΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉ
// IMPL├ëMENTATION: BudgetTab
// ΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉ
BudgetTab::BudgetTab(QWidget *parent)
    : QWidget(parent)
{
    initializeUI();
    setupConnections();
}

void BudgetTab::initializeUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // ===== Budget =====
    QGroupBox *budgetGroup = new QGroupBox("D├⌐finir le Budget", this);
    QVBoxLayout *budgetLayout = new QVBoxLayout(budgetGroup);

    QHBoxLayout *budgetRow = new QHBoxLayout();
    budgetRow->addWidget(new QLabel("Mois:"));
    cbMonth = new QComboBox();
    cbMonth->addItems({"Janvier", "F├⌐vrier", "Mars", "Avril", "Mai", "Juin",
                       "Juillet", "Ao├╗t", "Septembre", "Octobre", "Novembre", "D├⌐cembre"});
    budgetRow->addWidget(cbMonth);

    budgetRow->addWidget(new QLabel("Budget (DT):"));
    spinBudget = new QDoubleSpinBox();
    spinBudget->setMaximum(9999999.99);
    budgetRow->addWidget(spinBudget);

    btnSaveBudget = new QPushButton("Enregistrer Budget");
    budgetRow->addWidget(btnSaveBudget);
    budgetRow->addStretch();

    budgetLayout->addLayout(budgetRow);
    mainLayout->addWidget(budgetGroup);

    // ===== R├⌐sultat Comparatif =====
    QGroupBox *resultGroup = new QGroupBox("Comparaison avec D├⌐penses R├⌐elles", this);
    QVBoxLayout *resultLayout = new QVBoxLayout(resultGroup);

    QHBoxLayout *resultRow = new QHBoxLayout();

    // D├⌐pense R├⌐elle
    QVBoxLayout *depLayout = new QVBoxLayout();
    depLayout->addWidget(new QLabel("D├⌐pense R├⌐elle (DT):"));
    lblDepenseReelle = new QLabel("0.00");
    lblDepenseReelle->setStyleSheet("font-weight: bold; font-size: 14px;");
    depLayout->addWidget(lblDepenseReelle);
    resultRow->addLayout(depLayout);

    // ├ëcart
    QVBoxLayout *ecartLayout = new QVBoxLayout();
    ecartLayout->addWidget(new QLabel("├ëcart (DT):"));
    lblEcart = new QLabel("0.00");
    lblEcart->setStyleSheet("font-weight: bold; font-size: 14px;");
    ecartLayout->addWidget(lblEcart);
    resultRow->addLayout(ecartLayout);

    resultLayout->addLayout(resultRow);

    // Progress Bar
    resultLayout->addWidget(new QLabel("Utilisation du Budget:"));
    progressBudget = new QProgressBar();
    progressBudget->setValue(0);
    resultLayout->addWidget(progressBudget);

    mainLayout->addWidget(resultGroup);

    mainLayout->addStretch();
    setLayout(mainLayout);
}

void BudgetTab::setupConnections()
{
    connect(btnSaveBudget, &QPushButton::clicked, this, &BudgetTab::enregistrerBudget);
}

void BudgetTab::enregistrerBudget()
{
    // ├Ç impl├⌐menter
}

void BudgetTab::comparerBudget()
{
    // ├Ç impl├⌐menter
}

// ΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉ
// IMPL├ëMENTATION: AdvancedTab
// ΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉ
AdvancedTab::AdvancedTab(QWidget *parent)
    : QWidget(parent)
{
    initializeUI();
    setupConnections();
}

void AdvancedTab::initializeUI()
{
    // 1. Setup Scroll Area to handle responsiveness without crushing
    QVBoxLayout *outerLayout = new QVBoxLayout(this);
    outerLayout->setContentsMargins(0, 0, 0, 0);
    outerLayout->setSpacing(0);

    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setStyleSheet("QScrollArea { background-color: transparent; }");

    QWidget *contentWidget = new QWidget();
    contentWidget->setObjectName("contentWidget");
    
    // Main Layout inside the Scroll Area
    QVBoxLayout *mainLayout = new QVBoxLayout(contentWidget);
    mainLayout->setSpacing(25); // Generic spacing between groups
    mainLayout->setContentsMargins(30, 30, 30, 30);

    // =========================================================================
    // 1. RENTABILIT├ë SECTION
    // =========================================================================
    QGroupBox *rentGroup = new QGroupBox("Rentabilit├⌐", contentWidget);
    rentGroup->setObjectName("rentGroup");
    QHBoxLayout *rentLayout = new QHBoxLayout(rentGroup);
    rentLayout->setSpacing(20);
    rentLayout->setContentsMargins(20, 25, 20, 20);

    rentLayout->addWidget(new QLabel("Mois:"));
    cbMonthRent = new QComboBox();
    cbMonthRent->setObjectName("cbMonthRent");
    cbMonthRent->addItems({"Janvier", "F├⌐vrier", "Mars", "Avril", "Mai", "Juin",
                           "Juillet", "Ao├╗t", "Septembre", "Octobre", "Novembre", "D├⌐cembre"});
    cbMonthRent->setMinimumWidth(150);
    cbMonthRent->setMaximumWidth(250);
    rentLayout->addWidget(cbMonthRent);

    btnCalcRent = new QPushButton("Calculer Rentabilit├⌐");
    btnCalcRent->setObjectName("btnCalcRent");
    btnCalcRent->setCursor(Qt::PointingHandCursor);
    btnCalcRent->setMinimumWidth(150);
    rentLayout->addWidget(btnCalcRent);

    rentLayout->addStretch(); 

    rentLayout->addWidget(new QLabel("R├⌐sultat:"));
    lblRent = new QLabel("N/A");
    lblRent->setObjectName("lblRent");
    lblRent->setStyleSheet("font-weight: bold; color: #1B4332; font-size: 14pt; margin-left: 10px;");
    rentLayout->addWidget(lblRent);

    mainLayout->addWidget(rentGroup);

    // =========================================================================
    // 2. D├ëTECTION D├ëPENSES ANORMALES SECTION
    // =========================================================================
    QGroupBox *anomalyGroup = new QGroupBox("D├⌐tection D├⌐penses Anormales", contentWidget);
    anomalyGroup->setObjectName("anomalyGroup");
    QVBoxLayout *anomalyLayout = new QVBoxLayout(anomalyGroup);
    anomalyLayout->setSpacing(15);
    anomalyLayout->setContentsMargins(20, 25, 20, 20);

    QHBoxLayout *anomalyRow = new QHBoxLayout();
    anomalyRow->addWidget(new QLabel("Seuil d'Alerte (DT):"));
    spinSeuil = new QDoubleSpinBox();
    spinSeuil->setObjectName("spinSeuil");
    spinSeuil->setMaximum(9999999.99);
    spinSeuil->setMinimumWidth(120);
    spinSeuil->setMaximumWidth(200);
    anomalyRow->addWidget(spinSeuil);

    btnDetect = new QPushButton("D├⌐tecter");
    btnDetect->setObjectName("btnDetect");
    btnDetect->setCursor(Qt::PointingHandCursor);
    btnDetect->setMinimumWidth(120);
    anomalyRow->addWidget(btnDetect);
    anomalyRow->addStretch();
    anomalyLayout->addLayout(anomalyRow);

    tableAnomaly = new QTableWidget();
    tableAnomaly->setObjectName("tableAnomaly");
    tableAnomaly->setColumnCount(5);
    tableAnomaly->setHorizontalHeaderLabels({"Date", "Cat├⌐gorie", "Montant", "├ëcart %", "Alerte"});
    tableAnomaly->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableAnomaly->setAlternatingRowColors(true);
    tableAnomaly->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableAnomaly->setMinimumHeight(180);
    anomalyLayout->addWidget(tableAnomaly);

    mainLayout->addWidget(anomalyGroup);

    // =========================================================================
    // 3. PR├ëVISION SECTION
    // =========================================================================
    QGroupBox *predictGroup = new QGroupBox("Pr├⌐vision", contentWidget);
    predictGroup->setObjectName("predictGroup");
    QVBoxLayout *predictMainLayout = new QVBoxLayout(predictGroup);
    predictMainLayout->setSpacing(15);
    predictMainLayout->setContentsMargins(20, 25, 20, 20);

    QHBoxLayout *predictControls = new QHBoxLayout();
    predictControls->addWidget(new QLabel("P├⌐riode:"));
    cbPeriod = new QComboBox();
    cbPeriod->setObjectName("cbPeriod");
    cbPeriod->addItems({"1 Mois", "3 Mois", "6 Mois", "1 An"});
    cbPeriod->setMinimumWidth(150);
    cbPeriod->setMaximumWidth(250);
    predictControls->addWidget(cbPeriod);

    btnPredict = new QPushButton("Pr├⌐dire");
    btnPredict->setObjectName("btnPredict");
    btnPredict->setCursor(Qt::PointingHandCursor);
    btnPredict->setMinimumWidth(120);
    predictControls->addWidget(btnPredict);
    predictControls->addStretch();
    predictMainLayout->addLayout(predictControls);

    chartPredict = new QWidget();
    chartPredict->setObjectName("chartPredict");
    chartPredict->setMinimumHeight(250);
    chartPredict->setStyleSheet("background-color: #f8f9fa; border: 2px dashed #cfd8dc; border-radius: 8px;");
    
    QVBoxLayout *chartPlaceholderLayout = new QVBoxLayout(chartPredict);
    QLabel *lblChartPlaceholder = new QLabel("Zone Graphique Pr├⌐visionnelle", chartPredict);
    lblChartPlaceholder->setAlignment(Qt::AlignCenter);
    lblChartPlaceholder->setStyleSheet("color: #78909c; font-weight: bold; font-size: 14px;");
    chartPlaceholderLayout->addWidget(lblChartPlaceholder);

    predictMainLayout->addWidget(chartPredict);

    mainLayout->addWidget(predictGroup);

    // =========================================================================
    // 4. CLASSEMENT CLIENTS SECTION
    // =========================================================================
    QGroupBox *clientGroup = new QGroupBox("Classement Clients", contentWidget);
    clientGroup->setObjectName("clientGroup");
    QVBoxLayout *clientLayout = new QVBoxLayout(clientGroup);
    clientLayout->setSpacing(15);
    clientLayout->setContentsMargins(20, 25, 20, 20);

    QHBoxLayout *clientRow = new QHBoxLayout();
    clientRow->addWidget(new QLabel("Classer par:"));
    cbRank = new QComboBox();
    cbRank->setObjectName("cbRank");
    cbRank->addItems({"Chiffre d'Affaires", "Fr├⌐quence", "R├⌐cemment Actif"});
    cbRank->setMinimumWidth(200);
    cbRank->setMaximumWidth(350);
    clientRow->addWidget(cbRank);
    clientRow->addStretch();
    clientLayout->addLayout(clientRow);

    tableClients = new QTableWidget();
    tableClients->setObjectName("tableClients");
    tableClients->setColumnCount(4);
    tableClients->setHorizontalHeaderLabels({"Rang", "Client", "Montant Total", "Transactions"});
    tableClients->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableClients->setAlternatingRowColors(true);
    tableClients->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableClients->setMinimumHeight(180);
    clientLayout->addWidget(tableClients);

    mainLayout->addWidget(clientGroup);

    // =========================================================================
    // 5. SIMULATION FINANCI├êRE SECTION
    // =========================================================================
    QGroupBox *simGroup = new QGroupBox("Simulation Financi├¿re", contentWidget);
    simGroup->setObjectName("simGroup");
    
    QGridLayout *simGrid = new QGridLayout(simGroup);
    simGrid->setHorizontalSpacing(30);
    simGrid->setVerticalSpacing(20);
    simGrid->setContentsMargins(20, 25, 20, 20);

    // Row 0
    simGrid->addWidget(new QLabel("Prix Unitaire (DT):"), 0, 0);
    spinPrice = new QDoubleSpinBox();
    spinPrice->setObjectName("spinPrice");
    spinPrice->setMaximum(999999.99);
    spinPrice->setMinimumWidth(120);
    spinPrice->setMaximumWidth(200);
    simGrid->addWidget(spinPrice, 0, 1);
    
    simGrid->addWidget(new QLabel("Frais (DT):"), 0, 2);
    spinCharge = new QDoubleSpinBox();
    spinCharge->setObjectName("spinCharge");
    spinCharge->setMaximum(999999.99);
    spinCharge->setMinimumWidth(120);
    spinCharge->setMaximumWidth(200);
    simGrid->addWidget(spinCharge, 0, 3);

    // Row 1
    simGrid->addWidget(new QLabel("Ventes (Qt├⌐s):"), 1, 0);
    spinSales = new QDoubleSpinBox();
    spinSales->setObjectName("spinSales");
    spinSales->setMaximum(999999.99);
    spinSales->setMinimumWidth(120);
    spinSales->setMaximumWidth(200);
    simGrid->addWidget(spinSales, 1, 1);

    btnSimulate = new QPushButton("Simuler");
    btnSimulate->setObjectName("btnSimulate");
    btnSimulate->setCursor(Qt::PointingHandCursor);
    btnSimulate->setMinimumWidth(120);
    btnSimulate->setMaximumWidth(200);
    simGrid->addWidget(btnSimulate, 1, 3);

    // Row 2 - Result
    QHBoxLayout *simResultLayout = new QHBoxLayout();
    simResultLayout->addStretch();
    simResultLayout->addWidget(new QLabel("R├⌐sultat de simulation :"));
    lblResult = new QLabel("N/A");
    lblResult->setObjectName("lblResult");
    lblResult->setStyleSheet("font-weight: bold; font-size: 16px; color: #1B4332; padding: 6px 15px; border: 2px solid #1B4332; border-radius: 6px; background-color: #e6fffa;");
    simResultLayout->addWidget(lblResult);
    simResultLayout->addStretch();
    
    simGrid->addLayout(simResultLayout, 2, 0, 1, 4);

    mainLayout->addWidget(simGroup);
    
    // Add finishing touch - bottom spacer
    mainLayout->addStretch();
    
    // Finalize Scroll Area
    scrollArea->setWidget(contentWidget);
    outerLayout->addWidget(scrollArea);
    
    setLayout(outerLayout);
}

void AdvancedTab::setupConnections()
{
    connect(btnCalcRent, &QPushButton::clicked, this, &AdvancedTab::calculerRentabilite);
    connect(btnDetect, &QPushButton::clicked, this, &AdvancedTab::detecterAnomalies);
    connect(btnPredict, &QPushButton::clicked, this, &AdvancedTab::predire);
    connect(btnSimulate, &QPushButton::clicked, this, &AdvancedTab::simuler);
}

void AdvancedTab::calculerRentabilite()
{
    // ├Ç impl├⌐menter
}

void AdvancedTab::detecterAnomalies()
{
    // ├Ç impl├⌐menter
}

void AdvancedTab::predire()
{
    // ├Ç impl├⌐menter
}

void AdvancedTab::classerClients()
{
    // ├Ç impl├⌐menter
}

void AdvancedTab::simuler()
{
    // ├Ç impl├⌐menter
}

// ΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉ
// IMPL├ëMENTATION: ExportTab
// ΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉ
ExportTab::ExportTab(QWidget *parent)
    : QWidget(parent)
{
    initializeUI();
    setupConnections();
}

void ExportTab::initializeUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // ===== Export =====
    QGroupBox *exportGroup = new QGroupBox("Param├¿tres d'Export", this);
    QVBoxLayout *exportLayout = new QVBoxLayout(exportGroup);

    QHBoxLayout *row1 = new QHBoxLayout();
    row1->addWidget(new QLabel("Format d'Export:"));
    cbFormat = new QComboBox();
    cbFormat->addItems({"PDF", "Excel (.xlsx)", "Word (.docx)", "Texte (.txt)", "CSV"});
    row1->addWidget(cbFormat);
    row1->addStretch();
    exportLayout->addLayout(row1);

    QHBoxLayout *row2 = new QHBoxLayout();
    row2->addWidget(new QLabel("P├⌐riode - De:"));
    dateExportFrom = new QDateEdit();
    dateExportFrom->setDate(QDate::currentDate().addMonths(-1));
    row2->addWidget(dateExportFrom);
    row2->addWidget(new QLabel("├Ç:"));
    dateExportTo = new QDateEdit();
    dateExportTo->setDate(QDate::currentDate());
    row2->addWidget(dateExportTo);
    exportLayout->addLayout(row2);

    btnExport = new QPushButton("Exporter");
    btnExport->setMinimumHeight(40);
    btnExport->setStyleSheet("font-weight: bold;");
    exportLayout->addWidget(btnExport);

    mainLayout->addWidget(exportGroup);

    // ===== Status =====
    QGroupBox *statusGroup = new QGroupBox("Statut d'Export", this);
    QVBoxLayout *statusLayout = new QVBoxLayout(statusGroup);

    lblStatus = new QLabel("En attente d'export...");
    lblStatus->setStyleSheet("color: #666; font-size: 12px;");
    statusLayout->addWidget(lblStatus);

    mainLayout->addWidget(statusGroup);

    mainLayout->addStretch();
    setLayout(mainLayout);
}

void ExportTab::setupConnections()
{
    connect(btnExport, &QPushButton::clicked, this, [this]() {
        QString format = cbFormat->currentText();
        if (format.contains("PDF")) {
            exportPDF();
        } else if (format.contains("Excel")) {
            exportExcel();
        } else if (format.contains("Word")) {
            exportWord();
        } else if (format.contains("Texte") || format.contains("CSV")) {
            exportTxt();
        }
    });
}

void ExportTab::exportPDF()
{
    // ├Ç impl├⌐menter
    lblStatus->setText("Export PDF en cours...");
}

void ExportTab::exportExcel()
{
    // ├Ç impl├⌐menter
    lblStatus->setText("Export Excel en cours...");
}

void ExportTab::exportWord()
{
    // ├Ç impl├⌐menter
    lblStatus->setText("Export Word en cours...");
}

void ExportTab::exportTxt()
{
    // ├Ç impl├⌐menter
    lblStatus->setText("Export Texte/CSV en cours...");
}

// ΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉ
// IMPL├ëMENTATION: Finance
// ΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉ
Finance::Finance(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Finance)
{
    ui->setupUi(this);
    initializeUI();
    applyStyles();
}

Finance::~Finance()
{
    delete ui;
}

void Finance::initializeUI()
{
    // Cr├⌐er le widget central
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *centralLayout = new QVBoxLayout(centralWidget);
    centralLayout->setContentsMargins(0, 0, 0, 0);
    centralLayout->setSpacing(0);

    // ===== HEADER =====
    QWidget *headerWidget = new QWidget();
    headerWidget->setMinimumHeight(100);
    QHBoxLayout *headerLayout = new QHBoxLayout(headerWidget);
    headerLayout->setContentsMargins(20, 15, 20, 15);
    headerLayout->setSpacing(20);

    // Bouton Retour
    QPushButton *btnReturn = new QPushButton("ΓåÉ Retour");
    btnReturn->setMaximumWidth(120);
    btnReturn->setMinimumHeight(40);
    btnReturn->setStyleSheet(
        "QPushButton { "
        "background-color: #d4a852; "
        "color: white; "
        "border: none; "
        "border-radius: 4px; "
        "font-weight: bold; "
        "font-size: 11pt; "
        "} "
        "QPushButton:hover { "
        "background-color: #c49743; "
        "}"
    );
    // Connect return button to close/back signal
    connect(btnReturn, &QPushButton::clicked, this, &Finance::close);

    // Titre principal
    QVBoxLayout *titleLayout = new QVBoxLayout();
    QLabel *lblMainTitle = new QLabel("GESTION FINANCI├êRE");
    lblMainTitle->setStyleSheet("font-size: 24pt; font-weight: bold; color: #ffffff;");
    QLabel *lblSubtitle = new QLabel("TABLEAU DE BORD FINANCIER");
    lblSubtitle->setStyleSheet("font-size: 11pt; color: #d4a852; letter-spacing: 2px;");
    titleLayout->addWidget(lblMainTitle);
    titleLayout->addWidget(lblSubtitle);

    // Logo ou ic├┤ne
    QLabel *lblIcon = new QLabel();
    QPixmap logoPixmap;
    
    // Essayer diff├⌐rents chemins possibles
    if (!logoPixmap.load(":/images/resources/pressiq_logo.png")) {
        if (!logoPixmap.load("resources/pressiq_logo.png")) {
            logoPixmap.load("./resources/pressiq_logo.png");
        }
    }
    
    if (!logoPixmap.isNull()) {
        logoPixmap = logoPixmap.scaledToWidth(120, Qt::SmoothTransformation);
        lblIcon->setPixmap(logoPixmap);
    } else {
        // Fallback : afficher un texte simple
        lblIcon->setText("PressIQ");
        lblIcon->setStyleSheet("font-size: 12pt; font-weight: bold; color: #27ae60;");
    }
    lblIcon->setAlignment(Qt::AlignCenter);
    lblIcon->setMaximumWidth(150);

    // Status online
    QVBoxLayout *statusLayout = new QVBoxLayout();
    statusLayout->addStretch();
    QLabel *lblStatus = new QLabel("≡ƒƒó EN LIGNE");
    lblStatus->setStyleSheet("color: #27ae60; font-weight: bold; font-size: 9pt;");
    lblStatus->setAlignment(Qt::AlignRight);
    statusLayout->addWidget(lblStatus);

    // Assembler le header
    headerLayout->addWidget(btnReturn);
    headerLayout->addWidget(lblIcon);
    headerLayout->addLayout(titleLayout);
    headerLayout->addStretch();
    headerLayout->addLayout(statusLayout);

    // Appliquer le style du header avec gradient moderne
    headerWidget->setStyleSheet(
        "QWidget#headerWidget { "
        "background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
        "stop:0 #1B4332, stop:0.5 #234E3E, stop:1 #1B4332); "
        "border-bottom: 4px solid #C9A227; "
        "}"
    );
    headerWidget->setObjectName("headerWidget");

    centralLayout->addWidget(headerWidget);

    // Cr├⌐er le QTabWidget
    mainTabWidget = new QTabWidget(this);

    // Cr├⌐er les tabs
    transactionTab = new TransactionTab();
    searchTab = new SearchTab();
    statsTab = new StatsTab();
    journalTab = new JournalTab();
    budgetTab = new BudgetTab();
    advancedTab = new AdvancedTab();
    exportTab = new ExportTab();

    // Ajouter les tabs
    mainTabWidget->addTab(transactionTab, "Transactions");
    mainTabWidget->addTab(searchTab, "Recherche & Tri");
    mainTabWidget->addTab(statsTab, "Statistiques");
    mainTabWidget->addTab(journalTab, "Journal");
    mainTabWidget->addTab(budgetTab, "Budget");
    mainTabWidget->addTab(advancedTab, "Analyse");
    mainTabWidget->addTab(exportTab, "Export");

    centralLayout->addWidget(mainTabWidget);
    setCentralWidget(centralWidget);

    // Configuration de la fen├¬tre
    setWindowTitle("≡ƒÆ░ Gestion Financi├¿re");
    resize(1400, 950);
}

void Finance::applyStyles()
{
    // PressIQ Professional Theme - Enhanced Modern Design
    QString stylesheet = R"(
        /* ΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉ
           PRESSIQ MODERN PROFESSIONAL THEME
           Based on PressIQ Production Module v2.0
        ΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉΓòÉ */

        QMainWindow {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:1, 
                        stop:0 #f0f2f5, stop:1 #e4e7eb);
        }

        /* ΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇ
           TAB WIDGET - MODERN DESIGN
        ΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇ */
        QTabWidget::pane {
            border: none;
            background-color: transparent;
            border-radius: 12px;
            margin-top: 5px;
        }

        QTabBar {
            background: transparent;
        }

        QTabBar::tab {
            background-color: white;
            color: #666;
            padding: 12px 24px;
            margin-right: 4px;
            border: none;
            border-radius: 8px 8px 0 0;
            font-size: 13px;
            font-weight: 600;
            font-family: 'Segoe UI', Arial, sans-serif;
            min-width: 120px;
        }

        QTabBar::tab:selected {
            background-color: #1B4332;
            color: white;
            font-weight: 700;
        }

        QTabBar::tab:hover:!selected {
            background-color: #f0f0f0;
            color: #1B4332;
        }

        /* ΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇ
           GROUP BOX - CARD STYLE
        ΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇ */
        QGroupBox {
            font-size: 14px;
            font-weight: 700;
            font-family: 'Segoe UI', Arial, sans-serif;
            color: #1B4332;
            background-color: white;
            border: none;
            border-radius: 12px;
            margin-top: 20px;
            padding: 20px 15px 15px 15px;
        }

        QGroupBox::title {
            subcontrol-origin: margin;
            subcontrol-position: top left;
            left: 20px;
            top: 0px;
            padding: 8px 16px;
            background-color: #1B4332;
            color: white;
            border-radius: 8px;
            font-size: 13px;
        }

        /* ΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇ
           INPUT FIELDS - MODERN STYLE
        ΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇ */
        QLineEdit, QSpinBox, QDoubleSpinBox, QDateEdit, QComboBox {
            padding: 10px 14px;
            border: 2px solid #e0e0e0;
            border-radius: 8px;
            background-color: #fafafa;
            color: #333;
            font-size: 14px;
            font-family: 'Segoe UI', Arial, sans-serif;
            min-height: 20px;
            selection-background-color: #1B4332;
        }

        QLineEdit:focus, QSpinBox:focus, QDoubleSpinBox:focus, QDateEdit:focus, QComboBox:focus {
            border: 2px solid #1B4332;
            background-color: white;
        }

        QLineEdit:hover, QSpinBox:hover, QDoubleSpinBox:hover, QDateEdit:hover, QComboBox:hover {
            border: 2px solid #4ADE80;
        }

        QComboBox::drop-down {
            border: none;
            padding-right: 15px;
            width: 30px;
        }

        QComboBox QAbstractItemView {
            border: 2px solid #1B4332;
            border-radius: 8px;
            background-color: white;
            selection-background-color: #1B4332;
            selection-color: white;
            padding: 5px;
        }

        /* ΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇ
           TEXT EDIT - MODERN STYLE
        ΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇ */
        QTextEdit, QTextBrowser {
            border: 2px solid #e0e0e0;
            border-radius: 8px;
            background-color: #fafafa;
            color: #333;
            font-size: 14px;
            font-family: 'Segoe UI', Arial, sans-serif;
            padding: 12px;
        }

        QTextEdit:focus, QTextBrowser:focus {
            border: 2px solid #1B4332;
            background-color: white;
        }

        /* ΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇ
           BUTTONS - PROFESSIONAL STYLE
        ΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇ */
        QPushButton {
            padding: 12px 24px;
            border-radius: 8px;
            font-size: 14px;
            font-weight: 600;
            font-family: 'Segoe UI', Arial, sans-serif;
            min-width: 120px;
            border: none;
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1, 
                        stop:0 #234E3E, stop:1 #1B4332);
            color: white;
        }

        QPushButton:hover {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1, 
                        stop:0 #2D5A45, stop:1 #234E3E);
        }

        QPushButton:pressed {
            background: #1B4332;
        }

        /* ΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇ
           PROGRESS BAR - MODERN ANIMATED STYLE
        ΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇ */
        QProgressBar {
            border: none;
            border-radius: 10px;
            text-align: center;
            font-weight: 700;
            font-size: 13px;
            background-color: #e5e7eb;
            min-height: 24px;
            color: white;
        }

        QProgressBar::chunk {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0, 
                        stop:0 #1B4332, stop:0.5 #234E3E, stop:1 #1B4332);
            border-radius: 10px;
        }

        /* ΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇ
           TABLE WIDGET - PROFESSIONAL STYLE
        ΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇ */
        QTableWidget {
            border: none;
            border-radius: 8px;
            background-color: white;
            gridline-color: #f0f0f0;
            font-size: 13px;
            font-family: 'Segoe UI', Arial, sans-serif;
        }

        QTableWidget::item {
            padding: 12px;
            border-bottom: 1px solid #f0f0f0;
        }

        QTableWidget::item:selected {
            background-color: rgba(27, 67, 50, 0.1);
            color: #1B4332;
        }

        QTableWidget::item:hover {
            background-color: rgba(27, 67, 50, 0.05);
        }

        QHeaderView {
            background: #1B4332;
            background-color: #1B4332;
            border: none;
            border-radius: 8px;
        }

        QTableCornerButton::section {
            background-color: #1B4332;
            border: none;
        }

        QHeaderView::section {
            background-color: #1B4332;
            color: white;
            padding: 25px 15px;
            border: none;
            font-weight: 700;
            font-size: 13px;
            text-transform: uppercase;
            letter-spacing: 3px;
        }

        QHeaderView::section:first {
            border-top-left-radius: 8px;
        }

        QHeaderView::section:last {
            border-top-right-radius: 8px;
        }

        /* ΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇ
           LABELS - TYPOGRAPHY
        ΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇ */
        QLabel {
            color: #374151;
            font-size: 14px;
            font-family: 'Segoe UI', Arial, sans-serif;
        }

        /* ΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇ
           CHECKBOX - MODERN STYLE
        ΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇ */
        QCheckBox {
            font-size: 14px;
            font-family: 'Segoe UI', Arial, sans-serif;
            color: #374151;
            spacing: 12px;
            padding: 8px;
        }

        QCheckBox::indicator {
            width: 22px;
            height: 22px;
            border-radius: 6px;
        }

        QCheckBox::indicator:unchecked {
            background-color: #fafafa;
            border: 2px solid #d1d5db;
        }

        QCheckBox::indicator:unchecked:hover {
            border: 2px solid #1B4332;
        }

        QCheckBox::indicator:checked {
            background-color: #1B4332;
            border: 2px solid #1B4332;
        }

        /* ΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇ
           SCROLLBAR - MINIMAL STYLE
        ΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇΓöÇ */
        QScrollBar:vertical {
            border: none;
            background: #f0f0f0;
            width: 10px;
            border-radius: 5px;
            margin: 0;
        }

        QScrollBar::handle:vertical {
            background: #c0c0c0;
            border-radius: 5px;
            min-height: 30px;
        }

        QScrollBar::handle:vertical:hover {
            background: #1B4332;
        }

        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
            height: 0;
        }

        QScrollBar::horizontal {
            border: none;
            background: #f0f0f0;
            height: 10px;
            border-radius: 5px;
        }

        QScrollBar::handle:horizontal {
            background: #c0c0c0;
            border-radius: 5px;
            min-width: 30px;
        }

        QScrollBar::handle:horizontal:hover {
            background: #1B4332;
        }
    )";

    qApp->setStyle("Fusion");
    qApp->setStyleSheet(stylesheet);
}
