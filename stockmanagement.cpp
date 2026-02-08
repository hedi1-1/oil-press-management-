#include "stockmanagement.h"
#include <QHeaderView>
#include <QScrollArea>
#include <QDateTime>
#include <QFile>
#include <QTextStream>
#include <QFileDialog>

StockManagement::StockManagement(QWidget *parent)
    : QWidget(parent)
{
    // Charger les données de test
    loadSampleData();

    // Créer le layout principal
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    // Créer le QTabWidget principal
    m_tabWidget = new QTabWidget(this);
    m_tabWidget->setStyleSheet(getStyleSheet());

    // Ajouter tous les onglets
    m_tabWidget->addTab(createOnglet1_GestionStock(), "📦 Gestion Stock");
    m_tabWidget->addTab(createOnglet2_TriStock(), "🔃 Tri");
    m_tabWidget->addTab(createOnglet3_RechercheStock(), "🔍 Recherche");
    m_tabWidget->addTab(createOnglet4_ExportStock(), "📤 Export");
    m_tabWidget->addTab(createOnglet5_Statistiques(), "📊 Statistiques");
    m_tabWidget->addTab(createOnglet6_Historique(), "📜 Historique");
    m_tabWidget->addTab(createOnglet7_EtatStock(), "🚦 État Stock");
    m_tabWidget->addTab(createOnglet8_TempsReel(), "⏱️ Temps Réel");
    m_tabWidget->addTab(createOnglet9_AlerteStock(), "⚠️ Alertes");
    m_tabWidget->addTab(createOnglet10_Tendances(), "📈 Tendances");
    m_tabWidget->addTab(createOnglet11_RapportAuto(), "📄 Rapports");
    m_tabWidget->addTab(createOnglet12_Arduino(), "🔌 Arduino");

    mainLayout->addWidget(m_tabWidget);

    // Initialiser le timer pour le temps réel
    m_timerTempsReel = new QTimer(this);
    connect(m_timerTempsReel, &QTimer::timeout, this, &StockManagement::onUpdateTempsReel);
    m_timerTempsReel->start(5000); // Rafraîchissement toutes les 5 secondes

    // Mise à jour initiale de l'état
    updateEtatStock();
}

StockManagement::~StockManagement()
{
}

void StockManagement::loadSampleData()
{
    // Données de test
    m_stocks.append(Stock(1, "Extra vierge", 150.0, 50, QDate::currentDate(), "Entrepôt A", "En stock"));
    m_stocks.append(Stock(2, "Vierge", 30.0, 40, QDate::currentDate().addDays(-5), "Entrepôt B", "Stock faible"));
    m_stocks.append(Stock(3, "Lampante", 200.0, 60, QDate::currentDate().addDays(-2), "Entrepôt A", "En stock"));
    m_stocks.append(Stock(4, "Extra vierge", 10.0, 50, QDate::currentDate().addDays(-10), "Entrepôt C", "Rupture"));
    m_stocks.append(Stock(5, "Vierge", 80.0, 30, QDate::currentDate().addDays(-1), "Entrepôt B", "En stock"));

    // Mouvements de test
    m_mouvements.append(MouvementStock(QDateTime::currentDateTime().addDays(-3), "Entrée", 100.0, "Extra vierge"));
    m_mouvements.append(MouvementStock(QDateTime::currentDateTime().addDays(-2), "Sortie", 50.0, "Vierge"));
    m_mouvements.append(MouvementStock(QDateTime::currentDateTime().addDays(-1), "Entrée", 75.0, "Lampante"));
    m_mouvements.append(MouvementStock(QDateTime::currentDateTime(), "Sortie", 25.0, "Extra vierge"));
}

QString StockManagement::getStyleSheet()
{
    return R"(
        QTabWidget::pane {
            border: 1px solid #1A3C2F;
            border-radius: 5px;
            background: white;
        }
        QTabBar::tab {
            background: #f0f0f0;
            border: 1px solid #ccc;
            padding: 8px 16px;
            margin-right: 2px;
            border-top-left-radius: 4px;
            border-top-right-radius: 4px;
        }
        QTabBar::tab:selected {
            background: #1A3C2F;
            color: white;
        }
        QTabBar::tab:hover:!selected {
            background: #2D5A47;
        }
        QGroupBox {
            font-weight: bold;
            border: 2px solid #1A3C2F;
            border-radius: 8px;
            margin-top: 10px;
            padding-top: 10px;
            background-color: #f0f5f3;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 10px;
            padding: 0 5px;
            color: #1A3C2F;
        }
        QPushButton {
            background-color: #1A3C2F;
            color: white;
            border: none;
            padding: 8px 16px;
            border-radius: 4px;
            font-weight: bold;
        }
        QPushButton:hover {
            background-color: #2D5A47;
        }
        QPushButton:pressed {
            background-color: #0F2A1F;
        }
        QPushButton#btnSupprimer {
            background-color: #c62828;
        }
        QPushButton#btnSupprimer:hover {
            background-color: #d32f2f;
        }
        QLineEdit, QComboBox, QSpinBox, QDoubleSpinBox, QDateEdit {
            border: 1px solid #ccc;
            border-radius: 4px;
            padding: 6px;
            background: white;
        }
        QLineEdit:focus, QComboBox:focus, QSpinBox:focus, QDoubleSpinBox:focus, QDateEdit:focus {
            border: 2px solid #1A3C2F;
        }
        QTableWidget {
            border: 1px solid #ddd;
            border-radius: 4px;
            gridline-color: #e0e0e0;
        }
        QTableWidget::item:selected {
            background-color: #D4E5DC;
            color: black;
        }
        QHeaderView::section {
            background-color: #1A3C2F;
            color: white;
            padding: 8px;
            border: none;
            font-weight: bold;
        }
        QProgressBar {
            border: 1px solid #ccc;
            border-radius: 5px;
            text-align: center;
        }
        QProgressBar::chunk {
            background-color: #1A3C2F;
            border-radius: 4px;
        }
        QLCDNumber {
            border: 2px solid #1A3C2F;
            border-radius: 5px;
            background: #0F2A1F;
            color: #2D5A47;
        }
    )";
}

// =====================================================
// ONGLET 1 : GESTION DU STOCK (CRUD)
// =====================================================
QWidget* StockManagement::createOnglet1_GestionStock()
{
    QWidget *page = new QWidget();
    QVBoxLayout *mainLayout = new QVBoxLayout(page);
    mainLayout->setSpacing(15);
    mainLayout->setContentsMargins(20, 20, 20, 20);

    // GroupBox - Informations du stock
    QGroupBox *groupInfo = new QGroupBox("📋 Informations du stock");
    QGridLayout *gridLayout = new QGridLayout(groupInfo);
    gridLayout->setSpacing(10);

    int row = 0;

    // ID Stock
    gridLayout->addWidget(new QLabel("ID Stock :"), row, 0);
    m_lineIdStock = new QLineEdit();
    m_lineIdStock->setPlaceholderText("Saisir l'ID du stock");
    gridLayout->addWidget(m_lineIdStock, row++, 1);

    // Type d'huile
    gridLayout->addWidget(new QLabel("Type d'huile :"), row, 0);
    m_comboTypeHuile = new QComboBox();
    m_comboTypeHuile->addItems({"Extra vierge", "Vierge", "Lampante"});
    gridLayout->addWidget(m_comboTypeHuile, row++, 1);

    // Quantité actuelle
    gridLayout->addWidget(new QLabel("Quantité actuelle (L) :"), row, 0);
    m_spinQuantite = new QDoubleSpinBox();
    m_spinQuantite->setRange(0, 100000);
    m_spinQuantite->setDecimals(2);
    m_spinQuantite->setSuffix(" L");
    gridLayout->addWidget(m_spinQuantite, row++, 1);

    // Seuil d'alerte
    gridLayout->addWidget(new QLabel("Seuil d'alerte :"), row, 0);
    m_spinSeuilAlerte = new QSpinBox();
    m_spinSeuilAlerte->setRange(0, 10000);
    m_spinSeuilAlerte->setSuffix(" L");
    gridLayout->addWidget(m_spinSeuilAlerte, row++, 1);

    // Date dernière mise à jour
    gridLayout->addWidget(new QLabel("Date dernière mise à jour :"), row, 0);
    m_dateEditMaj = new QDateEdit(QDate::currentDate());
    m_dateEditMaj->setCalendarPopup(true);
    gridLayout->addWidget(m_dateEditMaj, row++, 1);

    // Emplacement
    gridLayout->addWidget(new QLabel("Emplacement de stockage :"), row, 0);
    m_lineEmplacement = new QLineEdit();
    m_lineEmplacement->setPlaceholderText("Ex: Entrepôt A, Zone 3");
    gridLayout->addWidget(m_lineEmplacement, row++, 1);

    // État du stock
    gridLayout->addWidget(new QLabel("État du stock :"), row, 0);
    m_comboEtatStock = new QComboBox();
    m_comboEtatStock->addItems({"En stock", "Stock faible", "Rupture"});
    gridLayout->addWidget(m_comboEtatStock, row++, 1);

    mainLayout->addWidget(groupInfo);

    // Boutons CRUD
    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->setSpacing(10);

    m_btnAjouter = new QPushButton("➕ Ajouter");
    m_btnModifier = new QPushButton("✏️ Modifier");
    m_btnSupprimer = new QPushButton("🗑️ Supprimer");
    m_btnSupprimer->setObjectName("btnSupprimer");
    m_btnConsulter = new QPushButton("👁️ Consulter");

    btnLayout->addWidget(m_btnAjouter);
    btnLayout->addWidget(m_btnModifier);
    btnLayout->addWidget(m_btnSupprimer);
    btnLayout->addWidget(m_btnConsulter);
    btnLayout->addStretch();

    mainLayout->addLayout(btnLayout);

    // Table des stocks
    QGroupBox *groupTable = new QGroupBox("📦 Liste des stocks");
    QVBoxLayout *tableLayout = new QVBoxLayout(groupTable);

    m_tableStocks = new QTableWidget();
    setupTable(m_tableStocks);
    populateTableWithStocks(m_tableStocks);
    tableLayout->addWidget(m_tableStocks);

    mainLayout->addWidget(groupTable, 1);

    // Connexions
    connect(m_btnAjouter, &QPushButton::clicked, this, &StockManagement::onAjouterStock);
    connect(m_btnModifier, &QPushButton::clicked, this, &StockManagement::onModifierStock);
    connect(m_btnSupprimer, &QPushButton::clicked, this, &StockManagement::onSupprimerStock);
    connect(m_btnConsulter, &QPushButton::clicked, this, &StockManagement::onConsulterStock);
    connect(m_tableStocks, &QTableWidget::itemSelectionChanged, this, &StockManagement::onTableSelectionChanged);

    return page;
}

void StockManagement::setupTable(QTableWidget *table)
{
    table->setColumnCount(7);
    table->setHorizontalHeaderLabels({"ID", "Type d'huile", "Quantité (L)", "Seuil", "Date MAJ", "Emplacement", "État"});
    table->horizontalHeader()->setStretchLastSection(true);
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setSelectionMode(QAbstractItemView::SingleSelection);
    table->setAlternatingRowColors(true);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void StockManagement::populateTableWithStocks(QTableWidget *table)
{
    table->setRowCount(m_stocks.size());
    for (int i = 0; i < m_stocks.size(); ++i) {
        const Stock &s = m_stocks[i];
        table->setItem(i, 0, new QTableWidgetItem(QString::number(s.getId())));
        table->setItem(i, 1, new QTableWidgetItem(s.getTypeHuile()));
        table->setItem(i, 2, new QTableWidgetItem(QString::number(s.getQuantite(), 'f', 2)));
        table->setItem(i, 3, new QTableWidgetItem(QString::number(s.getSeuilAlerte())));
        table->setItem(i, 4, new QTableWidgetItem(s.getDateMaj().toString("dd/MM/yyyy")));
        table->setItem(i, 5, new QTableWidgetItem(s.getEmplacement()));
        table->setItem(i, 6, new QTableWidgetItem(s.getEtat()));

        // Colorer selon l'état
        QColor bgColor;
        if (s.getEtat() == "Rupture") bgColor = QColor("#ffcdd2");
        else if (s.getEtat() == "Stock faible") bgColor = QColor("#fff9c4");
        else bgColor = QColor("#c8e6c9");

        for (int j = 0; j < 7; ++j) {
            table->item(i, j)->setBackground(bgColor);
        }
    }
}

void StockManagement::onAjouterStock()
{
    int newId = m_stocks.isEmpty() ? 1 : m_stocks.last().getId() + 1;
    Stock newStock(
        newId,
        m_comboTypeHuile->currentText(),
        m_spinQuantite->value(),
        m_spinSeuilAlerte->value(),
        m_dateEditMaj->date(),
        m_lineEmplacement->text(),
        m_comboEtatStock->currentText()
    );
    m_stocks.append(newStock);
    populateTableWithStocks(m_tableStocks);
    updateEtatStock();
    verifierAlertes();
    QMessageBox::information(this, "Succès", "Stock ajouté avec succès !");
}

void StockManagement::onModifierStock()
{
    int row = m_tableStocks->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Attention", "Veuillez sélectionner un stock à modifier.");
        return;
    }

    m_stocks[row].setTypeHuile(m_comboTypeHuile->currentText());
    m_stocks[row].setQuantite(m_spinQuantite->value());
    m_stocks[row].setSeuilAlerte(m_spinSeuilAlerte->value());
    m_stocks[row].setDateMaj(m_dateEditMaj->date());
    m_stocks[row].setEmplacement(m_lineEmplacement->text());
    m_stocks[row].setEtat(m_comboEtatStock->currentText());

    populateTableWithStocks(m_tableStocks);
    updateEtatStock();
    verifierAlertes();
    QMessageBox::information(this, "Succès", "Stock modifié avec succès !");
}

void StockManagement::onSupprimerStock()
{
    int row = m_tableStocks->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Attention", "Veuillez sélectionner un stock à supprimer.");
        return;
    }

    QMessageBox::StandardButton reply = QMessageBox::question(this, "Confirmation",
        "Êtes-vous sûr de vouloir supprimer ce stock ?",
        QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        m_stocks.removeAt(row);
        populateTableWithStocks(m_tableStocks);
        updateEtatStock();
        QMessageBox::information(this, "Succès", "Stock supprimé avec succès !");
    }
}

void StockManagement::onConsulterStock()
{
    int row = m_tableStocks->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Attention", "Veuillez sélectionner un stock à consulter.");
        return;
    }

    const Stock &s = m_stocks[row];
    QString details = QString(
        "📦 DÉTAILS DU STOCK\n\n"
        "ID : %1\n"
        "Type d'huile : %2\n"
        "Quantité : %3 L\n"
        "Seuil d'alerte : %4 L\n"
        "Date MAJ : %5\n"
        "Emplacement : %6\n"
        "État : %7"
    ).arg(s.getId())
     .arg(s.getTypeHuile())
     .arg(s.getQuantite(), 0, 'f', 2)
     .arg(s.getSeuilAlerte())
     .arg(s.getDateMaj().toString("dd/MM/yyyy"))
     .arg(s.getEmplacement())
     .arg(s.getEtat());

    QMessageBox::information(this, "Consultation Stock", details);
}

void StockManagement::onTableSelectionChanged()
{
    int row = m_tableStocks->currentRow();
    if (row >= 0 && row < m_stocks.size()) {
        const Stock &s = m_stocks[row];
        m_lineIdStock->setText(QString::number(s.getId()));
        m_comboTypeHuile->setCurrentText(s.getTypeHuile());
        m_spinQuantite->setValue(s.getQuantite());
        m_spinSeuilAlerte->setValue(s.getSeuilAlerte());
        m_dateEditMaj->setDate(s.getDateMaj());
        m_lineEmplacement->setText(s.getEmplacement());
        m_comboEtatStock->setCurrentText(s.getEtat());
    }
}

// =====================================================
// ONGLET 2 : TRI DU STOCK
// =====================================================
QWidget* StockManagement::createOnglet2_TriStock()
{
    QWidget *page = new QWidget();
    QVBoxLayout *mainLayout = new QVBoxLayout(page);
    mainLayout->setSpacing(15);
    mainLayout->setContentsMargins(20, 20, 20, 20);

    // GroupBox - Options de tri
    QGroupBox *groupTri = new QGroupBox("🔃 Options de tri");
    QHBoxLayout *triLayout = new QHBoxLayout(groupTri);

    triLayout->addWidget(new QLabel("Trier selon :"));
    m_comboTriCritere = new QComboBox();
    m_comboTriCritere->addItems({"Type d'huile", "Quantité disponible", "État du stock", "Date dernière mise à jour"});
    triLayout->addWidget(m_comboTriCritere);

    QPushButton *btnTri = new QPushButton("✅ Appliquer le tri");
    connect(btnTri, &QPushButton::clicked, this, &StockManagement::onAppliquerTri);
    triLayout->addWidget(btnTri);
    triLayout->addStretch();

    mainLayout->addWidget(groupTri);

    // Table triée
    QGroupBox *groupTable = new QGroupBox("📋 Stock trié");
    QVBoxLayout *tableLayout = new QVBoxLayout(groupTable);
    m_tableTriee = new QTableWidget();
    setupTable(m_tableTriee);
    populateTableWithStocks(m_tableTriee);
    tableLayout->addWidget(m_tableTriee);
    mainLayout->addWidget(groupTable, 1);

    return page;
}

void StockManagement::onAppliquerTri()
{
    QList<Stock> sortedStocks = m_stocks;
    QString critere = m_comboTriCritere->currentText();

    if (critere == "Type d'huile") {
        std::sort(sortedStocks.begin(), sortedStocks.end(), [](const Stock &a, const Stock &b) {
            return a.getTypeHuile() < b.getTypeHuile();
        });
    } else if (critere == "Quantité disponible") {
        std::sort(sortedStocks.begin(), sortedStocks.end(), [](const Stock &a, const Stock &b) {
            return a.getQuantite() > b.getQuantite();
        });
    } else if (critere == "État du stock") {
        std::sort(sortedStocks.begin(), sortedStocks.end(), [](const Stock &a, const Stock &b) {
            return a.getEtat() < b.getEtat();
        });
    } else if (critere == "Date dernière mise à jour") {
        std::sort(sortedStocks.begin(), sortedStocks.end(), [](const Stock &a, const Stock &b) {
            return a.getDateMaj() > b.getDateMaj();
        });
    }

    // Afficher les résultats triés
    m_tableTriee->setRowCount(sortedStocks.size());
    for (int i = 0; i < sortedStocks.size(); ++i) {
        const Stock &s = sortedStocks[i];
        m_tableTriee->setItem(i, 0, new QTableWidgetItem(QString::number(s.getId())));
        m_tableTriee->setItem(i, 1, new QTableWidgetItem(s.getTypeHuile()));
        m_tableTriee->setItem(i, 2, new QTableWidgetItem(QString::number(s.getQuantite(), 'f', 2)));
        m_tableTriee->setItem(i, 3, new QTableWidgetItem(QString::number(s.getSeuilAlerte())));
        m_tableTriee->setItem(i, 4, new QTableWidgetItem(s.getDateMaj().toString("dd/MM/yyyy")));
        m_tableTriee->setItem(i, 5, new QTableWidgetItem(s.getEmplacement()));
        m_tableTriee->setItem(i, 6, new QTableWidgetItem(s.getEtat()));
    }
}

// =====================================================
// ONGLET 3 : RECHERCHE DANS LE STOCK
// =====================================================
QWidget* StockManagement::createOnglet3_RechercheStock()
{
    QWidget *page = new QWidget();
    QVBoxLayout *mainLayout = new QVBoxLayout(page);
    mainLayout->setSpacing(15);
    mainLayout->setContentsMargins(20, 20, 20, 20);

    // GroupBox - Critères de recherche
    QGroupBox *groupRecherche = new QGroupBox("🔍 Critères de recherche");
    QGridLayout *gridLayout = new QGridLayout(groupRecherche);

    gridLayout->addWidget(new QLabel("Type d'huile :"), 0, 0);
    m_comboRechercheType = new QComboBox();
    m_comboRechercheType->addItems({"Tous", "Extra vierge", "Vierge", "Lampante"});
    gridLayout->addWidget(m_comboRechercheType, 0, 1);

    gridLayout->addWidget(new QLabel("État du stock :"), 1, 0);
    m_comboRechercheEtat = new QComboBox();
    m_comboRechercheEtat->addItems({"Tous", "En stock", "Stock faible", "Rupture"});
    gridLayout->addWidget(m_comboRechercheEtat, 1, 1);

    gridLayout->addWidget(new QLabel("Emplacement :"), 2, 0);
    m_lineRechercheEmplacement = new QLineEdit();
    m_lineRechercheEmplacement->setPlaceholderText("Laisser vide pour tous");
    gridLayout->addWidget(m_lineRechercheEmplacement, 2, 1);

    QHBoxLayout *btnLayout = new QHBoxLayout();
    QPushButton *btnRechercher = new QPushButton("🔍 Rechercher");
    QPushButton *btnReset = new QPushButton("🔄 Réinitialiser");
    btnLayout->addWidget(btnRechercher);
    btnLayout->addWidget(btnReset);
    btnLayout->addStretch();
    gridLayout->addLayout(btnLayout, 3, 0, 1, 2);

    connect(btnRechercher, &QPushButton::clicked, this, &StockManagement::onRechercher);
    connect(btnReset, &QPushButton::clicked, this, &StockManagement::onReinitialiserRecherche);

    mainLayout->addWidget(groupRecherche);

    // Table des résultats
    QGroupBox *groupResultats = new QGroupBox("📋 Résultats de recherche");
    QVBoxLayout *tableLayout = new QVBoxLayout(groupResultats);
    m_tableResultats = new QTableWidget();
    setupTable(m_tableResultats);
    tableLayout->addWidget(m_tableResultats);
    mainLayout->addWidget(groupResultats, 1);

    return page;
}

void StockManagement::onRechercher()
{
    QString typeFilter = m_comboRechercheType->currentText();
    QString etatFilter = m_comboRechercheEtat->currentText();
    QString emplacementFilter = m_lineRechercheEmplacement->text().toLower();

    QList<Stock> results;
    for (const Stock &s : m_stocks) {
        bool match = true;

        if (typeFilter != "Tous" && s.getTypeHuile() != typeFilter)
            match = false;
        if (etatFilter != "Tous" && s.getEtat() != etatFilter)
            match = false;
        if (!emplacementFilter.isEmpty() && !s.getEmplacement().toLower().contains(emplacementFilter))
            match = false;

        if (match) results.append(s);
    }

    m_tableResultats->setRowCount(results.size());
    for (int i = 0; i < results.size(); ++i) {
        const Stock &s = results[i];
        m_tableResultats->setItem(i, 0, new QTableWidgetItem(QString::number(s.getId())));
        m_tableResultats->setItem(i, 1, new QTableWidgetItem(s.getTypeHuile()));
        m_tableResultats->setItem(i, 2, new QTableWidgetItem(QString::number(s.getQuantite(), 'f', 2)));
        m_tableResultats->setItem(i, 3, new QTableWidgetItem(QString::number(s.getSeuilAlerte())));
        m_tableResultats->setItem(i, 4, new QTableWidgetItem(s.getDateMaj().toString("dd/MM/yyyy")));
        m_tableResultats->setItem(i, 5, new QTableWidgetItem(s.getEmplacement()));
        m_tableResultats->setItem(i, 6, new QTableWidgetItem(s.getEtat()));
    }

    QMessageBox::information(this, "Recherche", QString("%1 résultat(s) trouvé(s).").arg(results.size()));
}

void StockManagement::onReinitialiserRecherche()
{
    m_comboRechercheType->setCurrentIndex(0);
    m_comboRechercheEtat->setCurrentIndex(0);
    m_lineRechercheEmplacement->clear();
    m_tableResultats->setRowCount(0);
}

// =====================================================
// ONGLET 4 : EXPORT DU STOCK
// =====================================================
QWidget* StockManagement::createOnglet4_ExportStock()
{
    QWidget *page = new QWidget();
    QVBoxLayout *mainLayout = new QVBoxLayout(page);
    mainLayout->setSpacing(15);
    mainLayout->setContentsMargins(20, 20, 20, 20);

    // GroupBox 1 - Format
    QGroupBox *groupFormat = new QGroupBox("📁 Choisir le format");
    QHBoxLayout *formatLayout = new QHBoxLayout(groupFormat);

    m_radioPDF = new QRadioButton("📄 PDF");
    m_radioPDF->setChecked(true);
    m_radioExcel = new QRadioButton("📊 Excel");

    formatLayout->addWidget(m_radioPDF);
    formatLayout->addWidget(m_radioExcel);
    formatLayout->addStretch();

    mainLayout->addWidget(groupFormat);

    // GroupBox 2 - Options
    QGroupBox *groupOptions = new QGroupBox("⚙️ Options d'export");
    QVBoxLayout *optLayout = new QVBoxLayout(groupOptions);

    m_checkStats = new QCheckBox("📊 Inclure statistiques");
    m_checkDateTime = new QCheckBox("📅 Inclure date & heure");
    m_checkDateTime->setChecked(true);

    QHBoxLayout *nomLayout = new QHBoxLayout();
    nomLayout->addWidget(new QLabel("Nom du fichier :"));
    m_lineNomFichier = new QLineEdit("export_stock");
    nomLayout->addWidget(m_lineNomFichier);

    optLayout->addWidget(m_checkStats);
    optLayout->addWidget(m_checkDateTime);
    optLayout->addLayout(nomLayout);

    mainLayout->addWidget(groupOptions);

    // Bouton export
    QPushButton *btnExport = new QPushButton("💾 Exporter le stock");
    btnExport->setMinimumHeight(50);
    btnExport->setStyleSheet("font-size: 16px;");
    connect(btnExport, &QPushButton::clicked, this, &StockManagement::onExporterStock);
    mainLayout->addWidget(btnExport);

    mainLayout->addStretch();

    return page;
}

void StockManagement::onExporterStock()
{
    QString format;
    QString extension;
    QString filter;

    if (m_radioPDF->isChecked()) {
        format = "PDF";
        extension = ".pdf";
        filter = "PDF Files (*.pdf)";
    } else {
        format = "Excel";
        extension = ".csv";
        filter = "CSV Files (*.csv)";
    }

    QString fileName = m_lineNomFichier->text() + extension;
    QString filePath = QFileDialog::getSaveFileName(this, "Exporter en " + format, fileName, filter);

    if (filePath.isEmpty()) return;

    if (format == "Excel") {
        // Export Excel (format CSV compatible Excel)
        QFile file(filePath);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            
            if (m_checkDateTime->isChecked()) {
                out << "Export du stock;" << QDateTime::currentDateTime().toString("dd/MM/yyyy HH:mm:ss") << "\n\n";
            }

            // En-têtes
            out << "ID;Type d'huile;Quantité (L);Seuil;Date MAJ;Emplacement;État\n";

            // Données
            for (const Stock &s : m_stocks) {
                out << s.getId() << ";"
                    << s.getTypeHuile() << ";"
                    << QString::number(s.getQuantite(), 'f', 2) << ";"
                    << s.getSeuilAlerte() << ";"
                    << s.getDateMaj().toString("dd/MM/yyyy") << ";"
                    << s.getEmplacement() << ";"
                    << s.getEtat() << "\n";
            }

            if (m_checkStats->isChecked()) {
                double total = 0;
                for (const Stock &s : m_stocks) total += s.getQuantite();
                out << "\n";
                out << "STATISTIQUES\n";
                out << "Total articles;" << m_stocks.size() << "\n";
                out << "Volume total (L);" << QString::number(total, 'f', 2) << "\n";
            }

            file.close();
            QMessageBox::information(this, "Succès", "Export Excel réussi !\n\nFichier : " + filePath);
        } else {
            QMessageBox::critical(this, "Erreur", "Impossible de créer le fichier.");
        }
    } else {
        // Export PDF
        QFile file(filePath);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            
            out << "=================================================\n";
            out << "           RAPPORT DE STOCK - PRESSIQ\n";
            out << "=================================================\n\n";
            
            if (m_checkDateTime->isChecked()) {
                out << "Date d'export : " << QDateTime::currentDateTime().toString("dd/MM/yyyy HH:mm:ss") << "\n\n";
            }

            out << "-------------------------------------------------\n";
            out << QString("%1 | %2 | %3 | %4 | %5\n")
                   .arg("ID", -5)
                   .arg("Type", -15)
                   .arg("Quantité", -12)
                   .arg("Emplacement", -15)
                   .arg("État", -12);
            out << "-------------------------------------------------\n";

            for (const Stock &s : m_stocks) {
                out << QString("%1 | %2 | %3 L | %4 | %5\n")
                       .arg(s.getId(), -5)
                       .arg(s.getTypeHuile(), -15)
                       .arg(s.getQuantite(), 8, 'f', 2)
                       .arg(s.getEmplacement(), -15)
                       .arg(s.getEtat(), -12);
            }

            out << "-------------------------------------------------\n";

            if (m_checkStats->isChecked()) {
                double total = 0;
                int enStock = 0, stockFaible = 0, rupture = 0;
                for (const Stock &s : m_stocks) {
                    total += s.getQuantite();
                    if (s.getEtat() == "En stock") enStock++;
                    else if (s.getEtat() == "Stock faible") stockFaible++;
                    else rupture++;
                }
                out << "\n=== STATISTIQUES ===\n";
                out << "Total articles : " << m_stocks.size() << "\n";
                out << "Volume total : " << QString::number(total, 'f', 2) << " L\n";
                out << "En stock : " << enStock << " | Stock faible : " << stockFaible << " | Rupture : " << rupture << "\n";
            }

            out << "\n=================================================\n";
            out << "           Généré par PRESSIQ v1.0\n";
            out << "=================================================\n";

            file.close();
            QMessageBox::information(this, "Succès", "Export PDF réussi !\n\nFichier : " + filePath);
        } else {
            QMessageBox::critical(this, "Erreur", "Impossible de créer le fichier.");
        }
    }
}

// =====================================================
// ONGLET 5 : STATISTIQUES DU STOCK
// =====================================================
QWidget* StockManagement::createOnglet5_Statistiques()
{
    QWidget *page = new QWidget();
    QVBoxLayout *mainLayout = new QVBoxLayout(page);
    mainLayout->setSpacing(15);
    mainLayout->setContentsMargins(20, 20, 20, 20);

    // Résumé rapide
    QGroupBox *groupResume = new QGroupBox("📊 Résumé rapide");
    QHBoxLayout *resumeLayout = new QHBoxLayout(groupResume);

    m_labelTotalArticles = new QLabel("Total d'articles : 0");
    m_labelTotalArticles->setStyleSheet("font-size: 14px; font-weight: bold;");
    m_labelValeurTotale = new QLabel("Volume total : 0 L");
    m_labelValeurTotale->setStyleSheet("font-size: 14px; font-weight: bold;");

    resumeLayout->addWidget(m_labelTotalArticles);
    resumeLayout->addWidget(m_labelValeurTotale);
    resumeLayout->addStretch();

    mainLayout->addWidget(groupResume);

    // Options statistiques
    QGroupBox *groupOptions = new QGroupBox("⚙️ Options de calcul");
    QHBoxLayout *optLayout = new QHBoxLayout(groupOptions);

    optLayout->addWidget(new QLabel("Statistiques par :"));
    m_comboStatsPar = new QComboBox();
    m_comboStatsPar->addItems({"Type d'huile", "Quantité", "État", "Jour", "Mois"});
    optLayout->addWidget(m_comboStatsPar);

    QPushButton *btnCalculer = new QPushButton("📈 Calculer");
    connect(btnCalculer, &QPushButton::clicked, this, &StockManagement::onCalculerStatistiques);
    optLayout->addWidget(btnCalculer);
    optLayout->addStretch();

    mainLayout->addWidget(groupOptions);

    // Résumé statistique
    QGroupBox *groupStats = new QGroupBox("📝 Résumé statistique");
    QVBoxLayout *statsLayout = new QVBoxLayout(groupStats);
    m_textResumeStats = new QTextEdit();
    m_textResumeStats->setReadOnly(true);
    m_textResumeStats->setMinimumHeight(200);
    statsLayout->addWidget(m_textResumeStats);
    mainLayout->addWidget(groupStats, 1);

    return page;
}

void StockManagement::onCalculerStatistiques()
{
    // Calculs de base
    int total = m_stocks.size();
    double volumeTotal = 0;
    int enStock = 0, stockFaible = 0, rupture = 0;
    QMap<QString, double> parType;

    for (const Stock &s : m_stocks) {
        volumeTotal += s.getQuantite();

        if (s.getEtat() == "En stock") enStock++;
        else if (s.getEtat() == "Stock faible") stockFaible++;
        else rupture++;

        parType[s.getTypeHuile()] += s.getQuantite();
    }

    m_labelTotalArticles->setText(QString("Total d'articles : %1").arg(total));
    m_labelValeurTotale->setText(QString("Volume total : %1 L").arg(volumeTotal, 0, 'f', 2));

    // Générer le rapport
    QString rapport;
    rapport += "═══════════════════════════════════════\n";
    rapport += "        📊 RAPPORT STATISTIQUE\n";
    rapport += "═══════════════════════════════════════\n\n";

    rapport += QString("📦 Total d'articles : %1\n").arg(total);
    rapport += QString("💧 Volume total : %1 L\n\n").arg(volumeTotal, 0, 'f', 2);

    rapport += "📈 Répartition par état :\n";
    rapport += QString("   ✅ En stock : %1 (%2%)\n").arg(enStock).arg(total > 0 ? enStock * 100 / total : 0);
    rapport += QString("   ⚠️ Stock faible : %1 (%2%)\n").arg(stockFaible).arg(total > 0 ? stockFaible * 100 / total : 0);
    rapport += QString("   ❌ Rupture : %1 (%2%)\n\n").arg(rupture).arg(total > 0 ? rupture * 100 / total : 0);

    rapport += "🛢️ Volume par type d'huile :\n";
    for (auto it = parType.begin(); it != parType.end(); ++it) {
        rapport += QString("   • %1 : %2 L\n").arg(it.key()).arg(it.value(), 0, 'f', 2);
    }

    rapport += "\n═══════════════════════════════════════\n";
    rapport += QString("📅 Généré le : %1").arg(QDateTime::currentDateTime().toString("dd/MM/yyyy HH:mm"));

    m_textResumeStats->setText(rapport);
}

// =====================================================
// ONGLET 6 : HISTORIQUE DES MOUVEMENTS
// =====================================================
QWidget* StockManagement::createOnglet6_Historique()
{
    QWidget *page = new QWidget();
    QVBoxLayout *mainLayout = new QVBoxLayout(page);
    mainLayout->setSpacing(15);
    mainLayout->setContentsMargins(20, 20, 20, 20);

    // GroupBox - Filtre par période
    QGroupBox *groupFiltre = new QGroupBox("📅 Filtre par période");
    QHBoxLayout *filtreLayout = new QHBoxLayout(groupFiltre);

    filtreLayout->addWidget(new QLabel("Date début :"));
    m_dateDebut = new QDateEdit(QDate::currentDate().addMonths(-1));
    m_dateDebut->setCalendarPopup(true);
    filtreLayout->addWidget(m_dateDebut);

    filtreLayout->addWidget(new QLabel("Date fin :"));
    m_dateFin = new QDateEdit(QDate::currentDate());
    m_dateFin->setCalendarPopup(true);
    filtreLayout->addWidget(m_dateFin);

    QPushButton *btnAfficher = new QPushButton("📋 Afficher");
    connect(btnAfficher, &QPushButton::clicked, this, &StockManagement::onAfficherHistorique);
    filtreLayout->addWidget(btnAfficher);
    filtreLayout->addStretch();

    mainLayout->addWidget(groupFiltre);

    // Table historique
    QGroupBox *groupTable = new QGroupBox("📜 Historique des mouvements");
    QVBoxLayout *tableLayout = new QVBoxLayout(groupTable);

    m_tableHistorique = new QTableWidget();
    m_tableHistorique->setColumnCount(4);
    m_tableHistorique->setHorizontalHeaderLabels({"Date & Heure", "Type mouvement", "Quantité (L)", "Produit"});
    m_tableHistorique->horizontalHeader()->setStretchLastSection(true);
    m_tableHistorique->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_tableHistorique->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tableHistorique->setAlternatingRowColors(true);
    m_tableHistorique->setEditTriggers(QAbstractItemView::NoEditTriggers);

    tableLayout->addWidget(m_tableHistorique);
    mainLayout->addWidget(groupTable, 1);

    // Afficher les données initiales
    onAfficherHistorique();

    return page;
}

void StockManagement::onAfficherHistorique()
{
    QDate debut = m_dateDebut->date();
    QDate fin = m_dateFin->date();

    QList<MouvementStock> filtered;
    for (const MouvementStock &m : m_mouvements) {
        QDate dateMouv = m.getDateHeure().date();
        if (dateMouv >= debut && dateMouv <= fin) {
            filtered.append(m);
        }
    }

    m_tableHistorique->setRowCount(filtered.size());
    for (int i = 0; i < filtered.size(); ++i) {
        const MouvementStock &m = filtered[i];
        m_tableHistorique->setItem(i, 0, new QTableWidgetItem(m.getDateHeure().toString("dd/MM/yyyy HH:mm")));
        m_tableHistorique->setItem(i, 1, new QTableWidgetItem(m.getTypeMouvement()));
        m_tableHistorique->setItem(i, 2, new QTableWidgetItem(QString::number(m.getQuantite(), 'f', 2)));
        m_tableHistorique->setItem(i, 3, new QTableWidgetItem(m.getProduit()));

        // Colorer selon le type
        QColor bgColor = (m.getTypeMouvement() == "Entrée") ? QColor("#c8e6c9") : QColor("#ffcdd2");
        for (int j = 0; j < 4; ++j) {
            m_tableHistorique->item(i, j)->setBackground(bgColor);
        }
    }
}

// =====================================================
// ONGLET 7 : ÉTAT DU STOCK
// =====================================================
QWidget* StockManagement::createOnglet7_EtatStock()
{
    QWidget *page = new QWidget();
    QVBoxLayout *mainLayout = new QVBoxLayout(page);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(20, 20, 20, 20);

    // État global
    QGroupBox *groupEtat = new QGroupBox("🚦 État global du stock");
    QVBoxLayout *etatLayout = new QVBoxLayout(groupEtat);

    m_labelEtatGlobal = new QLabel("État : Normal");
    m_labelEtatGlobal->setStyleSheet("font-size: 24px; font-weight: bold; color: #2e7d32;");
    m_labelEtatGlobal->setAlignment(Qt::AlignCenter);
    etatLayout->addWidget(m_labelEtatGlobal);

    m_progressStock = new QProgressBar();
    m_progressStock->setMinimum(0);
    m_progressStock->setMaximum(100);
    m_progressStock->setValue(75);
    m_progressStock->setMinimumHeight(30);
    m_progressStock->setFormat("Niveau global : %p%");
    etatLayout->addWidget(m_progressStock);

    mainLayout->addWidget(groupEtat);

    // Produits critiques
    QGroupBox *groupCritiques = new QGroupBox("⚠️ Produits critiques");
    QVBoxLayout *critLayout = new QVBoxLayout(groupCritiques);

    m_listProduitsCritiques = new QListWidget();
    m_listProduitsCritiques->setStyleSheet("QListWidget::item { padding: 10px; }");
    critLayout->addWidget(m_listProduitsCritiques);

    mainLayout->addWidget(groupCritiques, 1);

    return page;
}

void StockManagement::updateEtatStock()
{
    if (!m_progressStock || !m_labelEtatGlobal || !m_listProduitsCritiques) return;

    double totalCapacite = 0;
    double totalQuantite = 0;
    int critiques = 0;

    m_listProduitsCritiques->clear();

    for (const Stock &s : m_stocks) {
        totalCapacite += s.getSeuilAlerte() * 3; // Estimation capacité max
        totalQuantite += s.getQuantite();

        if (s.estCritique() || s.getEtat() == "Rupture") {
            critiques++;
            QListWidgetItem *item = new QListWidgetItem(
                QString("⚠️ %1 - %2 (Quantité: %3 L / Seuil: %4 L)")
                .arg(s.getTypeHuile())
                .arg(s.getEmplacement())
                .arg(s.getQuantite(), 0, 'f', 2)
                .arg(s.getSeuilAlerte())
            );
            item->setBackground(QColor("#ffcdd2"));
            m_listProduitsCritiques->addItem(item);
        } else if (s.estSousSeuil() || s.getEtat() == "Stock faible") {
            QListWidgetItem *item = new QListWidgetItem(
                QString("⚡ %1 - %2 (Quantité: %3 L / Seuil: %4 L)")
                .arg(s.getTypeHuile())
                .arg(s.getEmplacement())
                .arg(s.getQuantite(), 0, 'f', 2)
                .arg(s.getSeuilAlerte())
            );
            item->setBackground(QColor("#fff9c4"));
            m_listProduitsCritiques->addItem(item);
        }
    }

    int niveau = (totalCapacite > 0) ? (totalQuantite / totalCapacite * 100) : 0;
    m_progressStock->setValue(qMin(niveau, 100));

    // Couleur selon niveau
    QString couleur;
    QString etat;
    if (niveau >= 60) {
        couleur = "#4caf50"; etat = "✅ Normal";
        m_labelEtatGlobal->setStyleSheet("font-size: 24px; font-weight: bold; color: #2e7d32;");
    } else if (niveau >= 30) {
        couleur = "#ff9800"; etat = "⚠️ Stock bas";
        m_labelEtatGlobal->setStyleSheet("font-size: 24px; font-weight: bold; color: #ff9800;");
    } else {
        couleur = "#f44336"; etat = "🔴 Critique";
        m_labelEtatGlobal->setStyleSheet("font-size: 24px; font-weight: bold; color: #f44336;");
    }

    m_labelEtatGlobal->setText(QString("État : %1").arg(etat));
    m_progressStock->setStyleSheet(QString("QProgressBar::chunk { background-color: %1; }").arg(couleur));
}

// =====================================================
// ONGLET 8 : SUIVI EN TEMPS RÉEL
// =====================================================
QWidget* StockManagement::createOnglet8_TempsReel()
{
    QWidget *page = new QWidget();
    QVBoxLayout *mainLayout = new QVBoxLayout(page);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(20, 20, 20, 20);

    // Affichage principal
    QGroupBox *groupDisplay = new QGroupBox("⏱️ Suivi en temps réel");
    QVBoxLayout *displayLayout = new QVBoxLayout(groupDisplay);

    m_labelQuantiteActuelle = new QLabel("Quantité totale actuelle :");
    m_labelQuantiteActuelle->setStyleSheet("font-size: 18px;");
    displayLayout->addWidget(m_labelQuantiteActuelle);

    m_lcdVolume = new QLCDNumber();
    m_lcdVolume->setDigitCount(10);
    m_lcdVolume->setSegmentStyle(QLCDNumber::Flat);
    m_lcdVolume->setMinimumHeight(100);
    m_lcdVolume->display(0);
    displayLayout->addWidget(m_lcdVolume);

    QLabel *labelUnit = new QLabel("Litres");
    labelUnit->setAlignment(Qt::AlignCenter);
    labelUnit->setStyleSheet("font-size: 16px; color: #666;");
    displayLayout->addWidget(labelUnit);

    m_labelDerniereMaj = new QLabel("Dernière mise à jour : --");
    m_labelDerniereMaj->setAlignment(Qt::AlignCenter);
    m_labelDerniereMaj->setStyleSheet("font-size: 12px; color: #888;");
    displayLayout->addWidget(m_labelDerniereMaj);

    mainLayout->addWidget(groupDisplay);

    // Info
    QLabel *labelInfo = new QLabel("🔄 Mise à jour automatique toutes les 5 secondes");
    labelInfo->setAlignment(Qt::AlignCenter);
    labelInfo->setStyleSheet("color: #666; font-style: italic;");
    mainLayout->addWidget(labelInfo);

    mainLayout->addStretch();

    // Première mise à jour
    onUpdateTempsReel();

    return page;
}

void StockManagement::onUpdateTempsReel()
{
    if (!m_lcdVolume || !m_labelDerniereMaj) return;

    double total = 0;
    for (const Stock &s : m_stocks) {
        total += s.getQuantite();
    }

    m_lcdVolume->display(total);
    m_labelDerniereMaj->setText(QString("Dernière mise à jour : %1")
        .arg(QDateTime::currentDateTime().toString("HH:mm:ss")));
}

// =====================================================
// ONGLET 9 : ALERTE STOCK FAIBLE
// =====================================================
QWidget* StockManagement::createOnglet9_AlerteStock()
{
    QWidget *page = new QWidget();
    QVBoxLayout *mainLayout = new QVBoxLayout(page);
    mainLayout->setSpacing(15);
    mainLayout->setContentsMargins(20, 20, 20, 20);

    // Seuil
    QGroupBox *groupSeuil = new QGroupBox("⚙️ Configuration des alertes");
    QHBoxLayout *seuilLayout = new QHBoxLayout(groupSeuil);

    m_labelSeuilDefini = new QLabel("Les alertes sont déclenchées automatiquement quand la quantité ≤ seuil d'alerte");
    m_labelSeuilDefini->setWordWrap(true);
    seuilLayout->addWidget(m_labelSeuilDefini);

    QPushButton *btnVerifier = new QPushButton("🔍 Vérifier maintenant");
    connect(btnVerifier, &QPushButton::clicked, this, &StockManagement::verifierAlertes);
    seuilLayout->addWidget(btnVerifier);

    mainLayout->addWidget(groupSeuil);

    // Liste des stocks sous seuil
    QGroupBox *groupAlerts = new QGroupBox("⚠️ Stocks sous le seuil d'alerte");
    QVBoxLayout *alertLayout = new QVBoxLayout(groupAlerts);

    m_listStocksSousSeuil = new QListWidget();
    m_listStocksSousSeuil->setStyleSheet("QListWidget::item { padding: 10px; font-size: 14px; }");
    alertLayout->addWidget(m_listStocksSousSeuil);

    mainLayout->addWidget(groupAlerts, 1);

    // Vérification initiale
    verifierAlertes();

    return page;
}

void StockManagement::verifierAlertes()
{
    if (!m_listStocksSousSeuil) return;

    m_listStocksSousSeuil->clear();
    int alertCount = 0;

    for (const Stock &s : m_stocks) {
        if (s.estSousSeuil()) {
            alertCount++;
            QString icon = s.estCritique() ? "🔴" : "🟡";
            QString status = s.estCritique() ? "CRITIQUE" : "FAIBLE";

            QListWidgetItem *item = new QListWidgetItem(
                QString("%1 [%2] %3 - %4\n   Quantité: %5 L | Seuil: %6 L | Manque: %7 L")
                .arg(icon)
                .arg(status)
                .arg(s.getTypeHuile())
                .arg(s.getEmplacement())
                .arg(s.getQuantite(), 0, 'f', 2)
                .arg(s.getSeuilAlerte())
                .arg(s.getSeuilAlerte() - s.getQuantite(), 0, 'f', 2)
            );

            if (s.estCritique()) {
                item->setBackground(QColor("#ffcdd2"));
            } else {
                item->setBackground(QColor("#fff9c4"));
            }

            m_listStocksSousSeuil->addItem(item);
        }
    }

    if (alertCount == 0) {
        QListWidgetItem *item = new QListWidgetItem("✅ Aucun stock sous le seuil d'alerte");
        item->setBackground(QColor("#c8e6c9"));
        m_listStocksSousSeuil->addItem(item);
    } else {
        // Afficher une alerte
        QMessageBox::warning(this, "⚠️ Alerte Stock",
            QString("%1 stock(s) sous le seuil d'alerte !\n\nVeuillez réapprovisionner rapidement.")
            .arg(alertCount));
    }
}

// =====================================================
// ONGLET 10 : ANALYSE DES TENDANCES
// =====================================================
QWidget* StockManagement::createOnglet10_Tendances()
{
    QWidget *page = new QWidget();
    QVBoxLayout *mainLayout = new QVBoxLayout(page);
    mainLayout->setSpacing(15);
    mainLayout->setContentsMargins(20, 20, 20, 20);

    // Options
    QGroupBox *groupOptions = new QGroupBox("📈 Analyse des tendances");
    QHBoxLayout *optLayout = new QHBoxLayout(groupOptions);

    optLayout->addWidget(new QLabel("Période :"));
    m_comboPeriode = new QComboBox();
    m_comboPeriode->addItems({"7 derniers jours", "30 derniers jours", "3 derniers mois", "Cette année"});
    optLayout->addWidget(m_comboPeriode);

    QPushButton *btnAnalyser = new QPushButton("📊 Analyser");
    connect(btnAnalyser, &QPushButton::clicked, this, &StockManagement::onAnalyserTendances);
    optLayout->addWidget(btnAnalyser);
    optLayout->addStretch();

    mainLayout->addWidget(groupOptions);

    // Zone graphique (placeholder)
    QGroupBox *groupGraph = new QGroupBox("📉 Graphique de tendances");
    QVBoxLayout *graphLayout = new QVBoxLayout(groupGraph);

    QLabel *labelPlaceholder = new QLabel("📊 Zone réservée pour QChartView\n(Nécessite Qt Charts)");
    labelPlaceholder->setAlignment(Qt::AlignCenter);
    labelPlaceholder->setStyleSheet("background: #f0f0f0; padding: 50px; border-radius: 10px; color: #666;");
    labelPlaceholder->setMinimumHeight(200);
    graphLayout->addWidget(labelPlaceholder);

    mainLayout->addWidget(groupGraph);

    // Analyse textuelle
    QGroupBox *groupAnalyse = new QGroupBox("📝 Analyse automatique");
    QVBoxLayout *analyseLayout = new QVBoxLayout(groupAnalyse);

    m_textAnalyseTendances = new QTextEdit();
    m_textAnalyseTendances->setReadOnly(true);
    analyseLayout->addWidget(m_textAnalyseTendances);

    mainLayout->addWidget(groupAnalyse, 1);

    return page;
}

void StockManagement::onAnalyserTendances()
{
    QString periode = m_comboPeriode->currentText();

    // Calculs simples
    QMap<QString, double> parType;
    for (const Stock &s : m_stocks) {
        parType[s.getTypeHuile()] += s.getQuantite();
    }

    QString analyse;
    analyse += "═══════════════════════════════════════\n";
    analyse += "        📈 ANALYSE DES TENDANCES\n";
    analyse += "═══════════════════════════════════════\n\n";
    analyse += QString("📅 Période analysée : %1\n\n").arg(periode);

    analyse += "🛢️ Répartition actuelle par type :\n";
    double maxVol = 0;
    QString maxType;
    for (auto it = parType.begin(); it != parType.end(); ++it) {
        analyse += QString("   • %1 : %2 L\n").arg(it.key()).arg(it.value(), 0, 'f', 2);
        if (it.value() > maxVol) {
            maxVol = it.value();
            maxType = it.key();
        }
    }

    analyse += QString("\n📊 Type dominant : %1 (%2 L)\n").arg(maxType).arg(maxVol, 0, 'f', 2);

    analyse += "\n💡 Recommandations :\n";
    analyse += "   • Surveiller les niveaux de stock faible\n";
    analyse += "   • Planifier le réapprovisionnement\n";
    analyse += "   • Optimiser les emplacements de stockage\n";

    analyse += "\n═══════════════════════════════════════\n";
    analyse += QString("📅 Analysé le : %1").arg(QDateTime::currentDateTime().toString("dd/MM/yyyy HH:mm"));

    m_textAnalyseTendances->setText(analyse);
}

// =====================================================
// ONGLET 11 : RAPPORT AUTOMATIQUE
// =====================================================
QWidget* StockManagement::createOnglet11_RapportAuto()
{
    QWidget *page = new QWidget();
    QVBoxLayout *mainLayout = new QVBoxLayout(page);
    mainLayout->setSpacing(15);
    mainLayout->setContentsMargins(20, 20, 20, 20);

    // Sélection du stock
    QGroupBox *groupSelect = new QGroupBox("📦 Sélectionner un stock");
    QVBoxLayout *selectLayout = new QVBoxLayout(groupSelect);

    m_tableRapportSelection = new QTableWidget();
    setupTable(m_tableRapportSelection);
    populateTableWithStocks(m_tableRapportSelection);
    m_tableRapportSelection->setMaximumHeight(200);
    selectLayout->addWidget(m_tableRapportSelection);

    mainLayout->addWidget(groupSelect);

    // Boutons
    QHBoxLayout *btnLayout = new QHBoxLayout();
    QPushButton *btnGenerer = new QPushButton("📄 Générer rapport");
    QPushButton *btnExportPDF = new QPushButton("📥 Export PDF");

    connect(btnGenerer, &QPushButton::clicked, this, &StockManagement::onGenererRapport);
    connect(btnExportPDF, &QPushButton::clicked, this, &StockManagement::onExportRapportPDF);

    btnLayout->addWidget(btnGenerer);
    btnLayout->addWidget(btnExportPDF);
    btnLayout->addStretch();
    mainLayout->addLayout(btnLayout);

    // Aperçu du rapport
    QGroupBox *groupApercu = new QGroupBox("📝 Aperçu du rapport");
    QVBoxLayout *apercuLayout = new QVBoxLayout(groupApercu);

    m_textApercuRapport = new QTextEdit();
    m_textApercuRapport->setReadOnly(true);
    apercuLayout->addWidget(m_textApercuRapport);

    mainLayout->addWidget(groupApercu, 1);

    return page;
}

void StockManagement::onGenererRapport()
{
    int row = m_tableRapportSelection->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Attention", "Veuillez sélectionner un stock.");
        return;
    }

    const Stock &s = m_stocks[row];

    QString rapport;
    rapport += "╔══════════════════════════════════════════╗\n";
    rapport += "║       📄 RAPPORT DE STOCK DÉTAILLÉ       ║\n";
    rapport += "╚══════════════════════════════════════════╝\n\n";

    rapport += QString("📅 Date du rapport : %1\n\n").arg(QDateTime::currentDateTime().toString("dd/MM/yyyy HH:mm:ss"));

    rapport += "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
    rapport += "              INFORMATIONS GÉNÉRALES\n";
    rapport += "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n\n";

    rapport += QString("🆔 ID Stock       : %1\n").arg(s.getId());
    rapport += QString("🛢️ Type d'huile   : %1\n").arg(s.getTypeHuile());
    rapport += QString("💧 Quantité       : %1 L\n").arg(s.getQuantite(), 0, 'f', 2);
    rapport += QString("⚠️ Seuil alerte   : %1 L\n").arg(s.getSeuilAlerte());
    rapport += QString("📅 Dernière MAJ   : %1\n").arg(s.getDateMaj().toString("dd/MM/yyyy"));
    rapport += QString("📍 Emplacement    : %1\n").arg(s.getEmplacement());
    rapport += QString("📊 État           : %1\n\n").arg(s.getEtat());

    rapport += "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
    rapport += "                    ANALYSE\n";
    rapport += "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n\n";

    double ratio = (s.getSeuilAlerte() > 0) ? (s.getQuantite() / s.getSeuilAlerte() * 100) : 0;
    rapport += QString("📈 Niveau vs seuil : %1%\n").arg(ratio, 0, 'f', 1);

    if (s.estCritique()) {
        rapport += "🔴 ATTENTION : Stock critique !\n";
        rapport += "   → Réapprovisionnement urgent nécessaire\n";
    } else if (s.estSousSeuil()) {
        rapport += "🟡 AVERTISSEMENT : Stock faible\n";
        rapport += "   → Prévoir réapprovisionnement\n";
    } else {
        rapport += "🟢 Stock dans les normes\n";
    }

    rapport += "\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
    rapport += "Rapport généré automatiquement par PRESSIQ v1.0\n";

    m_textApercuRapport->setText(rapport);
}

void StockManagement::onExportRapportPDF()
{
    if (m_textApercuRapport->toPlainText().isEmpty()) {
        QMessageBox::warning(this, "Attention", "Veuillez d'abord générer un rapport.");
        return;
    }

    QString filePath = QFileDialog::getSaveFileName(this, "Exporter en PDF", "rapport_stock.pdf", "PDF Files (*.pdf)");
    if (!filePath.isEmpty()) {
        // Note: L'export PDF nécessite Qt PrintSupport
        QMessageBox::information(this, "Export PDF",
            "Export PDF prévu.\n\n(Nécessite Qt PrintSupport pour une implémentation complète)");
    }
}

// =====================================================
// ONGLET 12 : SYNCHRONISATION ARDUINO
// =====================================================
QWidget* StockManagement::createOnglet12_Arduino()
{
    QWidget *page = new QWidget();
    QVBoxLayout *mainLayout = new QVBoxLayout(page);
    mainLayout->setSpacing(15);
    mainLayout->setContentsMargins(20, 20, 20, 20);

    // État connexion
    QGroupBox *groupConnexion = new QGroupBox("🔌 Connexion Arduino");
    QHBoxLayout *connLayout = new QHBoxLayout(groupConnexion);

    m_labelEtatConnexion = new QLabel("🔴 Non connecté");
    m_labelEtatConnexion->setStyleSheet("font-size: 16px; font-weight: bold; color: #f44336;");
    connLayout->addWidget(m_labelEtatConnexion);

    QPushButton *btnSync = new QPushButton("🔄 Synchroniser");
    connect(btnSync, &QPushButton::clicked, this, &StockManagement::onSynchroniserArduino);
    connLayout->addWidget(btnSync);
    connLayout->addStretch();

    mainLayout->addWidget(groupConnexion);

    // Valeur capteur
    QGroupBox *groupCapteur = new QGroupBox("📡 Données du capteur");
    QVBoxLayout *capteurLayout = new QVBoxLayout(groupCapteur);

    m_labelValeurCapteur = new QLabel("Valeur capteur : -- L");
    m_labelValeurCapteur->setStyleSheet("font-size: 24px; font-weight: bold;");
    m_labelValeurCapteur->setAlignment(Qt::AlignCenter);
    capteurLayout->addWidget(m_labelValeurCapteur);

    mainLayout->addWidget(groupCapteur);

    // Logs
    QGroupBox *groupLogs = new QGroupBox("📋 Logs & Anomalies");
    QVBoxLayout *logsLayout = new QVBoxLayout(groupLogs);

    m_textLogsArduino = new QTextEdit();
    m_textLogsArduino->setReadOnly(true);
    m_textLogsArduino->setStyleSheet("font-family: Consolas, monospace;");
    m_textLogsArduino->append("[INFO] En attente de connexion Arduino...");
    logsLayout->addWidget(m_textLogsArduino);

    mainLayout->addWidget(groupLogs, 1);

    return page;
}

void StockManagement::onSynchroniserArduino()
{
    // Simulation de connexion Arduino
    m_textLogsArduino->append(QString("[%1] Tentative de connexion...").arg(QDateTime::currentDateTime().toString("HH:mm:ss")));

    // Simuler un délai avec QTimer
    QTimer::singleShot(1000, this, [this]() {
        // Simuler une valeur
        double valeurSimulee = 150.0 + (rand() % 100);

        m_labelEtatConnexion->setText("🟢 Connecté (COM3)");
        m_labelEtatConnexion->setStyleSheet("font-size: 16px; font-weight: bold; color: #4caf50;");

        m_labelValeurCapteur->setText(QString("Valeur capteur : %1 L").arg(valeurSimulee, 0, 'f', 2));

        m_textLogsArduino->append(QString("[%1] Connexion établie sur COM3").arg(QDateTime::currentDateTime().toString("HH:mm:ss")));
        m_textLogsArduino->append(QString("[%1] Lecture capteur : %2 L").arg(QDateTime::currentDateTime().toString("HH:mm:ss")).arg(valeurSimulee, 0, 'f', 2));
        m_textLogsArduino->append(QString("[%1] Synchronisation réussie ✓").arg(QDateTime::currentDateTime().toString("HH:mm:ss")));
    });
}
