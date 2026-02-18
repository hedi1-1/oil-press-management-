#include "machine.h"
#include "ui_machine.h"
#include <QDateTime>
#include <QVBoxLayout>
#include <QDebug>
#include <QCheckBox>
#include <QDialog>
#include <QFormLayout>
#include <QLineEdit>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QSpacerItem>
#include <QStandardItemModel>
#include <QHeaderView>

// ============================================================================
// NavigationBar Implementation
// ============================================================================

NavigationBar::NavigationBar(QWidget *parent)
    : QWidget(parent), layout(nullptr), activeTabIndex(-1)
{
    setupLayout();

    // Set minimum and maximum heights for the navigation bar
    setMinimumHeight(50);
    setMaximumHeight(50);

    // Set stylesheet for the NavigationBar widget itself
    setStyleSheet(R"(
        NavigationBar {
            background-color: #f8f9fa;
            border-bottom: 2px solid #2D5F47;
        }
    )");
}

NavigationBar::~NavigationBar()
{
}

void NavigationBar::setupLayout()
{
    layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    setLayout(layout);
}

void NavigationBar::addTab(const QString &icon, const QString &text)
{
    QPushButton *button = new QPushButton();
    
    // Combine icon and text for the button
    QString buttonText = icon + " " + text;
    button->setText(buttonText);

    // Set fixed height and minimum width
    button->setMinimumHeight(50);
    button->setMinimumWidth(100);

    // Set size policy to expand horizontally
    button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // Connect button click signal
    connect(button, &QPushButton::clicked, this, &NavigationBar::onTabButtonClicked);

    // Add button to layout with equal stretch
    layout->addWidget(button, 1);

    // Add button to the vector
    buttons.append(button);

    // If this is the first tab, make it active
    if (buttons.size() == 1) {
        setActiveTab(0);
    }

    updateStyles();
}

void NavigationBar::setActiveTab(int index)
{
    if (index < 0 || index >= buttons.size()) {
        return;
    }

    activeTabIndex = index;
    updateStyles();
    emit tabClicked(index);
}

int NavigationBar::getCurrentTab() const
{
    return activeTabIndex;
}

void NavigationBar::clear()
{
    // Remove all buttons from layout and delete them
    while (!buttons.isEmpty()) {
        QPushButton *button = buttons.takeFirst();
        layout->removeWidget(button);
        delete button;
    }
    activeTabIndex = -1;
}

void NavigationBar::updateStyles()
{
    for (int i = 0; i < buttons.size(); ++i) {
        QPushButton *button = buttons[i];

        if (i == activeTabIndex) {
            // Active tab styling
            button->setStyleSheet(R"(
                QPushButton {
                    background-color: #1A3C2F;
                    color: white;
                    border: none;
                    border-radius: 0px;
                    padding: 8px 16px;
                    font-size: 12px;
                    font-weight: bold;
                    font-family: 'Segoe UI', Arial, sans-serif;
                    min-height: 50px;
                }
                QPushButton:hover {
                    background-color: #0F241E;
                }
                QPushButton:pressed {
                    background-color: #082118;
                }
            )");
        } else {
            // Inactive tab styling
            button->setStyleSheet(R"(
                QPushButton {
                    background-color: #f8f9fa;
                    color: #333;
                    border: none;
                    border-radius: 0px;
                    padding: 8px 16px;
                    font-size: 12px;
                    font-weight: 600;
                    font-family: 'Segoe UI', Arial, sans-serif;
                    min-height: 50px;
                }
                QPushButton:hover {
                    background-color: #e8e8e8;
                }
                QPushButton:pressed {
                    background-color: #d8d8d8;
                }
            )");
        }
    }
}

void NavigationBar::onTabButtonClicked()
{
    // Find which button was clicked
    QPushButton *clickedButton = qobject_cast<QPushButton *>(sender());
    if (clickedButton) {
        int index = buttons.indexOf(clickedButton);
        if (index >= 0) {
            setActiveTab(index);
        }
    }
}

// ============================================================================
// machine Main Window Implementation
// ============================================================================

machine::machine(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::machine)
    , navigationBar(nullptr)
{
    ui->setupUi(this);
    
    // Fix grid column stretches and spacing to reduce gap between left/right blocks
    QGridLayout* grids[] = { ui->gridCard1Info, ui->gridCard2Info, ui->gridCard3Info };
    for (auto grid : grids) {
        // Labels: no stretch, values: stretch to fill, col 4 (icons): compact at right
        grid->setColumnStretch(0, 0);
        grid->setColumnStretch(1, 1);
        grid->setColumnStretch(2, 0);
        grid->setColumnStretch(3, 1);
        grid->setColumnStretch(4, 0);
        // Set fixed minimum widths to keep blocks compact and close together
        grid->setColumnMinimumWidth(0, 160);  // Label gauche
        grid->setColumnMinimumWidth(1, 200);  // Valeur gauche
        grid->setColumnMinimumWidth(2, 120);  // Label droite
        grid->setColumnMinimumWidth(3, 120);  // Valeur droite
        grid->setHorizontalSpacing(15);
    }
    
    // Setup navigation bar
    setupNavigationBar();
    
    // Connect back button
    connect(ui->btnBackToMenu, &QPushButton::clicked, this, &machine::onBackButtonClicked);
    
    // Setup date/time timer
    dateTimeTimer = new QTimer(this);
    connect(dateTimeTimer, &QTimer::timeout, this, &machine::updateDateTime);
    dateTimeTimer->start(1000); // Update every second
    updateDateTime(); // Initial update
    
    // Setup machine table
    setupMachineTable();
    
    // Connect Exporter button
    connect(ui->btnExporter, &QPushButton::clicked, this, [this]() {
        // Hide form and lists, show summary
        ui->widgetFormulaireParcMachines->setVisible(false);
        ui->scrollHistoriqueOnOff->setVisible(false);
        ui->scrollListeMachines->setVisible(true);
        ui->btnHistoriqueToggle->setChecked(false);
        // TODO: Implement export logic
    });
    
    // Connect Afficher Tableau toggle button
    connect(ui->btnAfficherTableau, &QPushButton::toggled, this, [this](bool checked) {
        ui->widgetFormulaireParcMachines->setVisible(false);
        ui->scrollHistoriqueOnOff->setVisible(false);
        ui->btnHistoriqueToggle->blockSignals(true);
        ui->btnHistoriqueToggle->setChecked(false);
        ui->btnHistoriqueToggle->blockSignals(false);
        
        if (checked) {
            ui->tableMachines->setVisible(true);
            ui->scrollListeMachines->setVisible(false);
        } else {
            ui->tableMachines->setVisible(false);
            ui->scrollListeMachines->setVisible(true);
        }
    });
    
    // Connect Historique ON/OFF toggle button
    connect(ui->btnHistoriqueToggle, &QPushButton::toggled, this, [this](bool checked) {
        // Hide form if visible
        ui->widgetFormulaireParcMachines->setVisible(false);
        ui->btnAfficherTableau->blockSignals(true);
        ui->btnAfficherTableau->setChecked(false);
        ui->btnAfficherTableau->blockSignals(false);
        
        if (checked) {
            ui->scrollHistoriqueOnOff->setVisible(true);
            ui->scrollListeMachines->setVisible(false);
        } else {
            ui->scrollHistoriqueOnOff->setVisible(false);
            ui->scrollListeMachines->setVisible(true);
        }
        ui->tableMachines->setVisible(false);
    });
    
    // Connect close button for Historique ON/OFF
    connect(ui->btnFermerHistorique, &QPushButton::clicked, this, [this]() {
        ui->btnHistoriqueToggle->setChecked(false);
        ui->scrollHistoriqueOnOff->setVisible(false);
        ui->scrollListeMachines->setVisible(true);
        ui->tableMachines->setVisible(false);
    });
    
    // Connect search and filter buttons
    connect(ui->btnRechercher, &QPushButton::clicked, this, [this]() {
        // Hide form if visible and show list
        ui->widgetFormulaireParcMachines->setVisible(false);
        ui->scrollHistoriqueOnOff->setVisible(false);
        ui->scrollListeMachines->setVisible(true);
        ui->btnHistoriqueToggle->setChecked(false);
        // TODO: Implement search logic
    });
    
    connect(ui->btnFiltrer, &QPushButton::clicked, this, [this]() {
        // Hide form if visible and show list
        ui->widgetFormulaireParcMachines->setVisible(false);
        ui->scrollHistoriqueOnOff->setVisible(false);
        ui->scrollListeMachines->setVisible(true);
        ui->btnHistoriqueToggle->setChecked(false);
        // TODO: Implement filter logic
    });
    
    // Form buttons are auto-connected via Qt's naming convention:
    // on_btnEnregistrerParc_clicked() and on_btnAnnulerParc_clicked()
    
    // Create back button for formulaire view
    btnRetourFormulaire = new QPushButton("←", ui->groupRecherche);
    btnRetourFormulaire->setFixedSize(36, 36);
    btnRetourFormulaire->setCursor(Qt::PointingHandCursor);
    btnRetourFormulaire->setStyleSheet(R"(
        QPushButton {
            background: transparent;
            color: #1B4D3E;
            font-size: 22px;
            font-weight: bold;
            border: none;
            border-radius: 8px;
            padding: 0px;
        }
        QPushButton:hover {
            background-color: rgba(27, 77, 62, 60);
        }
    )");
    btnRetourFormulaire->setVisible(false);
    connect(btnRetourFormulaire, &QPushButton::clicked, this, &machine::masquerFormulaireParc);

    // Create "Ajout d'une machine" title label inside groupRecherche
    lblTitreAjout = new QLabel("➕ Ajout d'une machine", ui->groupRecherche);
    lblTitreAjout->setStyleSheet(R"(
        QLabel {
            color: #1B4D3E;
            font-size: 15px;
            font-weight: bold;
            font-family: 'Segoe UI', Arial, sans-serif;
            padding: 10px 30px;
            background-color: transparent;
            border: none;
            letter-spacing: 1.5px;
        }
    )");
    lblTitreAjout->setAlignment(Qt::AlignCenter);
    lblTitreAjout->setFixedHeight(42);
    
    // Add back button and title to layout
    QHBoxLayout *layoutFormHeader = new QHBoxLayout();
    layoutFormHeader->setContentsMargins(0, 0, 0, 0);
    layoutFormHeader->setSpacing(4);
    layoutFormHeader->addWidget(btnRetourFormulaire);
    layoutFormHeader->addWidget(lblTitreAjout, 1);
    ui->layoutRechercheGlobal->addLayout(layoutFormHeader);
    lblTitreAjout->setVisible(false);
    
    // Save original groupbox stylesheet
    groupRechercheOriginalStyle = ui->groupRecherche->styleSheet();

    // Setup todo list for maintenance
    setupTodoList();

    // Connect actionneurs button to show formulaire
    connect(ui->btnSummaryActionneurs, &QPushButton::clicked, this, &machine::afficherFormulaireParc);

    // Initialize form as hidden
    ui->widgetFormulaireParcMachines->setVisible(false);
    qDebug() << "[DEBUG] Initialisation: formulaire masque";
}

machine::~machine()
{
    if (dateTimeTimer) {
        dateTimeTimer->stop();
    }
    delete ui;
}

void machine::updateDateTime()
{
    QDateTime now = QDateTime::currentDateTime();
    
    // Noms des mois en français
    QStringList monthsFR = {"janvier", "février", "mars", "avril", "mai", "juin",
                            "juillet", "août", "septembre", "octobre", "novembre", "décembre"};
    
    int day = now.date().day();
    int month = now.date().month();
    int year = now.date().year();
    int hour = now.time().hour();
    int minute = now.time().minute();
    int second = now.time().second();
    
    // Format: "📅 04 Février 2026 • 14:30:45"
    QString dateStr = QString::number(day).rightJustified(2, '0');
    QString monthStr = monthsFR[month - 1];
    monthStr[0] = monthStr[0].toUpper();
    
    QString timeStr = QString::number(hour).rightJustified(2, '0') + ":" + 
                      QString::number(minute).rightJustified(2, '0') + ":" +
                      QString::number(second).rightJustified(2, '0');
    
    QString dateTimeStr = QString("📅 %1 %2 %3 • %4")
        .arg(dateStr, monthStr, QString::number(year), timeStr);
    
    ui->lblDateTime->setText(dateTimeStr);
}

void machine::onBackButtonClicked()
{
    emit backToMenu();
    this->hide();
}

void machine::setupNavigationBar()
{
    // Hide the default QTabBar
    ui->tabWidgetMachineManagement->tabBar()->hide();

    // Create the custom navigation bar
    navigationBar = new NavigationBar(this);
    
    // Add tabs to the navigation bar
    navigationBar->addTab("🏭", "Parc machines");
    navigationBar->addTab("📊", "Statistiques");
    navigationBar->addTab("🤖", "Décision IA");

    // Insert the navigation bar at the top of the central widget
    QVBoxLayout *mainLayout = qobject_cast<QVBoxLayout *>(ui->centralwidget->layout());
    if (mainLayout) {
        // Insert navigation bar after the header (at index 1)
        mainLayout->insertWidget(1, navigationBar);
    }

    // Connect navigation bar signals to tab widget
    connect(navigationBar, QOverload<int>::of(&NavigationBar::tabClicked),
            this, &machine::onNavigationTabClicked);

    // Set the first tab as active
    navigationBar->setActiveTab(0);
}

void machine::setupMachineTable()
{
    // Create model with columns matching the machine form fields
    machineTableModel = new QStandardItemModel(0, 13, this);
    machineTableModel->setHorizontalHeaderLabels({
        "ID", "Nom", "Type", "État marche", "Temp. (°C)",
        "Charge (%)", "Fonctionnement", "Alerte", "Criticité", "Dernière maintenance",
        "Date d'installation", "Fabricant", "Date mise à jour"
    });

    // Add sample data rows
    struct MachineData {
        QString nom, type, etatMarche, temp, charge, fonctionnement, alerte, criticite, maintenance, dateInstallation, fabricant, dateMaj;
    };

    QVector<MachineData> sampleData = {
        {"Presse-HX200",   "Presse",    "ON",    "72.5",  "85",  "Normal",  "Aucune",       "Faible",   "2026-01-15", "2022-03-10", "Jean Dupont",      "2026-02-18"},
        {"Broyeur-M400",   "Broyeur",   "ON",    "88.0",  "92",  "Alerte",  "Température",  "Élevé",    "2025-12-20", "2021-07-22", "Marie Martin",     "2026-02-17"},
        {"Malaxeur-V100",  "Malaxeur",  "OFF",   "25.0",  "0",   "Normal",  "Aucune",       "Faible",   "2026-02-01", "2023-01-05", "Pierre Bernard",   "2026-02-15"},
        {"Chauffeur-T300", "Chauffeur", "ON",    "145.0", "78",  "Normal",  "Aucune",       "Moyen",    "2026-01-28", "2020-11-18", "Sophie Durand",    "2026-02-18"},
        {"Presse-HX350",   "Presse",    "VEILLE","45.0",  "30",  "Normal",  "Aucune",       "Faible",   "2026-02-10", "2024-06-30", "Jean Dupont",      "2026-02-16"},
        {"Broyeur-M600",   "Broyeur",   "ON",    "95.0",  "98",  "Panne",   "Surcharge",    "Critique", "2025-11-05", "2019-09-12", "Luc Moreau",       "2026-02-14"},
        {"Malaxeur-V250",  "Malaxeur",  "ON",    "60.0",  "65",  "Alerte",  "Panne",        "Élevé",    "2026-01-03", "2022-08-25", "Claire Lefebvre",  "2026-02-18"},
        {"Chauffeur-T500", "Chauffeur", "ON",    "120.0", "55",  "Normal",  "Aucune",       "Faible",   "2026-02-14", "2023-04-14", "Sophie Durand",    "2026-02-17"},
    };

    int idCounter = 1;
    for (const auto &m : sampleData) {
        QList<QStandardItem*> row;

        // ID column (auto-increment)
        auto *idItem = new QStandardItem(QString::number(idCounter++));
        idItem->setEditable(false);
        idItem->setTextAlignment(Qt::AlignCenter);
        idItem->setForeground(QColor("#666666"));
        row.append(idItem);

        QStringList vals = {m.nom, m.type, m.etatMarche, m.temp, m.charge, m.fonctionnement, m.alerte, m.criticite, m.maintenance, m.dateInstallation, m.fabricant, m.dateMaj};
        for (const auto &v : vals) {
            auto *item = new QStandardItem(v);
            item->setEditable(false);
            item->setTextAlignment(Qt::AlignCenter);
            item->setForeground(QColor("#333333"));
            row.append(item);
        }

        // Bold font for colored columns
        QFont boldFont;
        boldFont.setBold(true);

        // Color-code the "Fonctionnement" column (index 6) — text color only
        row[6]->setFont(boldFont);
        if (m.fonctionnement == "Normal") {
            row[6]->setForeground(QColor("#2E7D32"));
        } else if (m.fonctionnement == "Alerte") {
            row[6]->setForeground(QColor("#E65100"));
        } else if (m.fonctionnement == "Panne") {
            row[6]->setForeground(QColor("#C62828"));
        }

        // Color-code the "État marche" column (index 3) — text color only
        row[3]->setFont(boldFont);
        if (m.etatMarche == "ON") {
            row[3]->setForeground(QColor("#2E7D32"));
        } else if (m.etatMarche == "OFF") {
            row[3]->setForeground(QColor("#C62828"));
        } else if (m.etatMarche == "VEILLE") {
            row[3]->setForeground(QColor("#E65100"));
        }

        // Color-code the "Criticité" column (index 8) — text color only
        row[8]->setFont(boldFont);
        if (m.criticite == "Critique") {
            row[8]->setForeground(QColor("#C62828"));
        } else if (m.criticite == "Élevé") {
            row[8]->setForeground(QColor("#E65100"));
        } else if (m.criticite == "Moyen") {
            row[8]->setForeground(QColor("#F57F17"));
        } else {
            row[8]->setForeground(QColor("#2E7D32"));
        }

        // Color-code the "Alerte" column (index 7) — text color only
        row[7]->setFont(boldFont);
        if (m.alerte == "Aucune") {
            row[7]->setForeground(QColor("#2E7D32"));
        } else if (m.alerte == "Température") {
            row[7]->setForeground(QColor("#E65100"));
        } else if (m.alerte == "Surcharge") {
            row[7]->setForeground(QColor("#C62828"));
        } else if (m.alerte == "Panne") {
            row[7]->setForeground(QColor("#C62828"));
        }

        machineTableModel->appendRow(row);
    }

    // Assign model to the table view
    ui->tableMachines->setModel(machineTableModel);

    // Style the table to match the interface design
    ui->tableMachines->setStyleSheet(R"(
        QTableView {
            border: 2px solid #1A3C2F;
            border-radius: 6px;
            background-color: #FFFFFF;
            alternate-background-color: #F0F7F4;
            gridline-color: #D5E8D4;
            font-size: 12px;
            font-family: 'Segoe UI', Arial, sans-serif;
            selection-background-color: #C8E6C9;
            selection-color: #1A3C2F;
        }
        QTableView::item {
            padding: 8px 12px;
            border-bottom: 1px solid #E8E8E8;
        }
        QTableView::item:hover {
            background-color: #E0F2E9;
        }
        QTableView::item:selected {
            background-color: #C8E6C9;
            color: #1A3C2F;
        }
        QHeaderView::section {
            background-color: #1B4D3E;
            color: white;
            padding: 10px 8px;
            border: none;
            border-right: 1px solid #2C5F4F;
            font-weight: bold;
            font-size: 12px;
            font-family: 'Segoe UI', Arial, sans-serif;
        }
        QHeaderView::section:last {
            border-right: none;
        }
    )");

    // Configure table view behavior
    ui->tableMachines->setAlternatingRowColors(true);
    ui->tableMachines->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableMachines->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableMachines->setSortingEnabled(true);
    ui->tableMachines->verticalHeader()->setVisible(false);
    ui->tableMachines->horizontalHeader()->setStretchLastSection(true);
    ui->tableMachines->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableMachines->setShowGrid(true);

    // Set row height
    ui->tableMachines->verticalHeader()->setDefaultSectionSize(40);
}

void machine::onNavigationTabClicked(int index)
{
    // When changing tabs, hide form and reset view
    if (index == 0) {
        // Parc machines tab: show list by default
        ui->widgetFormulaireParcMachines->setVisible(false);
        ui->scrollHistoriqueOnOff->setVisible(false);
        ui->scrollListeMachines->setVisible(true);
        ui->btnHistoriqueToggle->setChecked(false);
    } else {
        // Other tabs: hide all parc machines specific views
        ui->widgetFormulaireParcMachines->setVisible(false);
        ui->scrollHistoriqueOnOff->setVisible(false);
        ui->scrollListeMachines->setVisible(false);
    }
    
    // Update the tab widget to show the corresponding tab
    ui->tabWidgetMachineManagement->setCurrentIndex(index);
}

void machine::onAjouterButtonClicked()
{
    qDebug() << "[DEBUG] Bouton Ajouter cliqué";
    afficherFormulaireParc();
}

// Auto-connected slot for Annuler button
void machine::on_btnAnnulerParc_clicked()
{
    qDebug() << "[DEBUG] *** BOUTON ANNULER CLIQUE ***";
    masquerFormulaireParc();
    reinitialiserFormulaireParc();
}

// Auto-connected slot for Enregistrer button
void machine::on_btnEnregistrerParc_clicked()
{
    qDebug() << "[DEBUG] *** BOUTON ENREGISTRER CLIQUE ***";
    // TODO: Implement save logic here
    // Example: save data to database
    
    // Hide the form and reset fields
    masquerFormulaireParc();
    reinitialiserFormulaireParc();
}

void machine::afficherFormulaireParc()
{
    qDebug() << "[DEBUG] Affichage du formulaire";
    // Reset form fields
    reinitialiserFormulaireParc();
    
    // Hide lists and summary
    ui->scrollListeMachines->setVisible(false);
    ui->scrollHistoriqueOnOff->setVisible(false);
    ui->tableMachines->setVisible(false);
    ui->frameSummary->setVisible(false);
    
    // Uncheck historique toggle button if checked
    ui->btnHistoriqueToggle->setChecked(false);
    
    // Replace search content with "Ajout d'une machine" title
    masquerContenuRecherche();
    
    // Show form
    ui->widgetFormulaireParcMachines->setVisible(true);
    qDebug() << "[DEBUG] Formulaire visible:" << ui->widgetFormulaireParcMachines->isVisible();
}

void machine::masquerFormulaireParc()
{
    qDebug() << "[DEBUG] Masquage du formulaire";
    // Hide form
    ui->widgetFormulaireParcMachines->setVisible(false);
    
    // Restore search content
    restaurerContenuRecherche();
    
    // Show machine list and summary
    ui->scrollListeMachines->setVisible(true);
    ui->scrollHistoriqueOnOff->setVisible(false);
    ui->tableMachines->setVisible(false);
    ui->frameSummary->setVisible(true);
    qDebug() << "[DEBUG] Formulaire visible:" << ui->widgetFormulaireParcMachines->isVisible();
}

void machine::masquerContenuRecherche()
{
    // Hide all child widgets of groupRecherche except lblTitreAjout and btnRetourFormulaire
    const QList<QWidget*> children = ui->groupRecherche->findChildren<QWidget*>();
    for (QWidget *child : children) {
        if (child != lblTitreAjout && child != btnRetourFormulaire) {
            child->setVisible(false);
        }
    }
    // Remove title and apply compact style to groupbox
    ui->groupRecherche->setTitle("");
    ui->groupRecherche->setStyleSheet(R"(
        QGroupBox {
            border: 2px solid #1A3C2F;
            border-radius: 8px;
            margin-top: 0px;
            padding: 0px;
            background-color: white;
        }
        QGroupBox::title {
            padding: 0px;
            margin: 0px;
            subcontrol-position: top left;
        }
    )");
    ui->layoutRechercheGlobal->setContentsMargins(8, 8, 8, 8);
    ui->layoutRechercheGlobal->setSpacing(0);
    btnRetourFormulaire->setVisible(true);
    lblTitreAjout->setVisible(true);
}

void machine::restaurerContenuRecherche()
{
    // Hide the "Ajout" label and back button
    lblTitreAjout->setVisible(false);
    btnRetourFormulaire->setVisible(false);
    // Restore groupbox original style, title and sizing
    ui->groupRecherche->setStyleSheet(groupRechercheOriginalStyle);
    ui->groupRecherche->setTitle("Recherche et tri");
    ui->layoutRechercheGlobal->setContentsMargins(15, 18, 15, 15);
    ui->layoutRechercheGlobal->setSpacing(12);
    // Show all child widgets of groupRecherche except lblTitreAjout and btnRetourFormulaire
    const QList<QWidget*> children = ui->groupRecherche->findChildren<QWidget*>();
    for (QWidget *child : children) {
        if (child != lblTitreAjout && child != btnRetourFormulaire) {
            child->setVisible(true);
        }
    }
}

void machine::reinitialiserFormulaireParc()
{
    // Reset all form fields to default values
    ui->nom_machine_parc->clear();
    ui->type_machine_parc->setCurrentIndex(0); // "Presse"
    ui->etat_marche_parc->setCurrentIndex(0); // "ON"
    ui->temperature_actuelle_parc->setValue(0.00);
    ui->niveau_charge_parc->setValue(0);
    ui->etat_fonctionnement_parc->setCurrentIndex(0); // "Normal"
    ui->type_alerte_parc->setCurrentIndex(0); // "Aucune"
    ui->niveau_criticite_parc->setCurrentIndex(0); // "Faible"
    ui->date_derniere_maintenance_parc->setDate(QDate(2000, 1, 1));
}

// ============================================================================
// Todo List (Machines à réparer) Implementation
// ============================================================================

void machine::setupTodoList()
{
    // Connect the "+ Ajouter" button
    connect(ui->btnAjouterTodo, &QPushButton::clicked, this, &machine::showAddTodoDialog);

    // Add sample entries for demonstration
    addTodoItem("Presse Hydraulique A3", "Siemens", "Urgent");
    addTodoItem("Convoyeur B12", "Bosch", "Normal");
    addTodoItem("Compresseur C7", "Atlas Copco", "Urgent");
}

void machine::addTodoItem(const QString &machineName, const QString &fabricant, const QString &priority)
{
    TodoItem item;
    item.machineName = machineName;
    item.fabricant = fabricant;
    item.priority = priority;
    item.done = false;
    m_todoItems.append(item);
    refreshTodoList();
}

void machine::onTodoCheckToggled(int index, bool checked)
{
    if (index >= 0 && index < m_todoItems.size()) {
        m_todoItems[index].done = checked;
        refreshTodoList();
    }
}

QWidget* machine::createTodoItemWidget(int index)
{
    const TodoItem &item = m_todoItems[index];

    QWidget *row = new QWidget();
    row->setObjectName("todoRow");

    // Styles based on done state
    if (item.done) {
        row->setStyleSheet(
            "QWidget#todoRow { background-color: #F5F5F5; border-radius: 4px; padding: 2px; }"
        );
    } else {
        row->setStyleSheet(
            "QWidget#todoRow { background-color: white; border: 1px solid #E8E8E8; border-radius: 4px; padding: 2px; }"
        );
    }

    QHBoxLayout *rowLayout = new QHBoxLayout(row);
    rowLayout->setContentsMargins(10, 6, 10, 6);
    rowLayout->setSpacing(10);

    // Checkbox
    QCheckBox *cb = new QCheckBox();
    cb->setChecked(item.done);
    cb->setStyleSheet(
        "QCheckBox::indicator { width: 18px; height: 18px; }"
        "QCheckBox::indicator:unchecked { border: 2px solid #999; border-radius: 4px; background: white; }"
        "QCheckBox::indicator:checked { border: 2px solid #2E7D32; border-radius: 4px; background: #2E7D32; }"
    );
    // Capture index for lambda
    int idx = index;
    connect(cb, &QCheckBox::toggled, this, [this, idx](bool checked) {
        onTodoCheckToggled(idx, checked);
    });
    rowLayout->addWidget(cb);

    // Machine name label
    QLabel *lblName = new QLabel(item.machineName);
    if (item.done) {
        lblName->setStyleSheet("font-size: 12px; color: #AAA; text-decoration: line-through; background: transparent; border: none;");
    } else {
        lblName->setStyleSheet("font-size: 12px; font-weight: bold; color: #333; background: transparent; border: none;");
    }
    rowLayout->addWidget(lblName);

    // Fabricant label
    QLabel *lblFab = new QLabel(item.fabricant);
    if (item.done) {
        lblFab->setStyleSheet("font-size: 11px; color: #BBB; text-decoration: line-through; background: transparent; border: none;");
    } else {
        lblFab->setStyleSheet("font-size: 11px; color: #777; background: transparent; border: none;");
    }
    rowLayout->addWidget(lblFab);

    // Spacer
    rowLayout->addStretch();

    // Priority badge
    QLabel *badge = new QLabel(item.priority);
    if (item.done) {
        badge->setStyleSheet(
            "font-size: 10px; font-weight: bold; color: #CCC; background-color: #F0F0F0;"
            "border-radius: 4px; padding: 2px 10px; border: none;"
        );
    } else if (item.priority == "Urgent") {
        badge->setStyleSheet(
            "font-size: 10px; font-weight: bold; color: white; background-color: #D32F2F;"
            "border-radius: 4px; padding: 2px 10px; border: none;"
        );
    } else {
        badge->setStyleSheet(
            "font-size: 10px; font-weight: bold; color: white; background-color: #F57F17;"
            "border-radius: 4px; padding: 2px 10px; border: none;"
        );
    }
    badge->setAlignment(Qt::AlignCenter);
    rowLayout->addWidget(badge);

    // Delete button
    QPushButton *btnDel = new QPushButton("✕");
    btnDel->setFixedSize(22, 22);
    btnDel->setCursor(Qt::PointingHandCursor);
    btnDel->setStyleSheet(
        "QPushButton { background: transparent; color: #999; font-size: 13px; border: none; padding: 0px; }"
        "QPushButton:hover { color: #D32F2F; }"
    );
    connect(btnDel, &QPushButton::clicked, this, [this, idx]() {
        if (idx >= 0 && idx < m_todoItems.size()) {
            m_todoItems.removeAt(idx);
            refreshTodoList();
        }
    });
    rowLayout->addWidget(btnDel);

    return row;
}

void machine::refreshTodoList()
{
    // Clear existing widgets from layoutTodoContent
    QLayout *layout = ui->layoutTodoContent;
    QLayoutItem *child;
    while ((child = layout->takeAt(0)) != nullptr) {
        if (child->widget()) {
            delete child->widget();
        }
        if (child->spacerItem()) {
            delete child;
        } else {
            delete child;
        }
    }

    // Sort: Urgent+not done first, Normal+not done next, done items last
    QVector<int> indices;
    for (int i = 0; i < m_todoItems.size(); ++i) {
        indices.append(i);
    }
    std::sort(indices.begin(), indices.end(), [this](int a, int b) {
        const TodoItem &ia = m_todoItems[a];
        const TodoItem &ib = m_todoItems[b];
        // Done items go last
        if (ia.done != ib.done) return !ia.done;
        // Among not-done, Urgent first
        if (!ia.done && !ib.done) {
            if (ia.priority == "Urgent" && ib.priority != "Urgent") return true;
            if (ia.priority != "Urgent" && ib.priority == "Urgent") return false;
        }
        return false;
    });

    // Reorder m_todoItems according to sorted indices
    QVector<TodoItem> sorted;
    for (int idx : indices) {
        sorted.append(m_todoItems[idx]);
    }
    m_todoItems = sorted;

    // Create widgets in sorted order
    for (int i = 0; i < m_todoItems.size(); ++i) {
        QWidget *w = createTodoItemWidget(i);
        ui->layoutTodoContent->addWidget(w);
    }

    // Add spacer at bottom to push items up
    ui->layoutTodoContent->addItem(
        new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding)
    );

    // Update counter in title
    int pending = 0;
    for (const auto &item : m_todoItems) {
        if (!item.done) pending++;
    }
    ui->lblTodoTitle->setText(QString("🔧 Machines à réparer (%1)").arg(pending));
}

void machine::showAddTodoDialog()
{
    QDialog dialog(this);
    dialog.setWindowTitle("🔧 Ajouter une intervention");
    dialog.setFixedSize(380, 240);
    dialog.setStyleSheet(
        "QDialog { background-color: #1A3C2F; }"
        "QLabel { font-size: 12px; color: #E0E0E0; font-weight: bold; }"
        "QLineEdit { padding: 6px 10px; border: 1px solid #2D5F47; border-radius: 5px; font-size: 12px; background-color: #F5F5F5; color: #333; }"
        "QComboBox { padding: 6px 10px; border: 1px solid #2D5F47; border-radius: 5px; font-size: 12px; background-color: #F5F5F5; color: #333; }"
        "QComboBox::drop-down { border: none; }"
        "QComboBox QAbstractItemView { background: white; color: #333; selection-background-color: #2D5F47; selection-color: white; }"
    );

    QFormLayout *form = new QFormLayout(&dialog);
    form->setSpacing(14);
    form->setContentsMargins(20, 20, 20, 16);

    QLineEdit *editMachine = new QLineEdit();
    editMachine->setPlaceholderText("Ex: Presse Hydraulique A3");
    form->addRow("Machine :", editMachine);

    QLineEdit *editFabricant = new QLineEdit();
    editFabricant->setPlaceholderText("Ex: Siemens");
    form->addRow("Fabricant :", editFabricant);

    QComboBox *comboPriority = new QComboBox();
    comboPriority->addItems({"Urgent", "Normal"});
    form->addRow("Priorité :", comboPriority);

    QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    buttons->button(QDialogButtonBox::Ok)->setText("Ajouter");
    buttons->button(QDialogButtonBox::Cancel)->setText("Annuler");
    buttons->button(QDialogButtonBox::Ok)->setStyleSheet(
        "QPushButton { background-color: #C9A227; color: #1A3C2F; font-weight: bold; font-size: 12px;"
        "border: none; border-radius: 5px; padding: 8px 20px; }"
        "QPushButton:hover { background-color: #B8911F; }"
    );
    buttons->button(QDialogButtonBox::Cancel)->setStyleSheet(
        "QPushButton { background-color: rgba(255,255,255,0.15); color: #CCC; font-size: 12px;"
        "border: 1px solid #4A7A66; border-radius: 5px; padding: 8px 20px; }"
        "QPushButton:hover { background-color: rgba(255,255,255,0.25); color: white; }"
    );
    form->addRow(buttons);

    connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() == QDialog::Accepted) {
        QString name = editMachine->text().trimmed();
        QString fab = editFabricant->text().trimmed();
        QString prio = comboPriority->currentText();
        if (!name.isEmpty() && !fab.isEmpty()) {
            addTodoItem(name, fab, prio);
        }
    }
}
