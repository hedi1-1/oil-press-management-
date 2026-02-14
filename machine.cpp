#include "machine.h"
#include "ui_machine.h"
#include <QDateTime>
#include <QVBoxLayout>
#include <QDebug>

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
    
    // Setup navigation bar
    setupNavigationBar();
    
    // Connect back button
    connect(ui->btnBackToMenu, &QPushButton::clicked, this, &machine::onBackButtonClicked);
    
    // Setup date/time timer
    dateTimeTimer = new QTimer(this);
    connect(dateTimeTimer, &QTimer::timeout, this, &machine::updateDateTime);
    dateTimeTimer->start(1000); // Update every second
    updateDateTime(); // Initial update
    
    // Add toolbar actions to the central toolbar
    QAction *actionAjouter = ui->toolBarActions->addAction("➕ Ajouter");
    QAction *actionModifier = ui->toolBarActions->addAction("✏️ Modifier");
    QAction *actionSupprimer = ui->toolBarActions->addAction("🗑️ Supprimer");
    ui->toolBarActions->addSeparator();
    QAction *actionActualiser = ui->toolBarActions->addAction("🔄 Actualiser");
    QAction *actionExporter = ui->toolBarActions->addAction("📤 Exporter");
    
    // Connect toolbar actions
    connect(actionAjouter, &QAction::triggered, this, &machine::onAjouterButtonClicked);
    
    connect(actionModifier, &QAction::triggered, this, [this]() {
        // Hide form and lists, show summary
        ui->widgetFormulaireParcMachines->setVisible(false);
        ui->scrollHistoriqueOnOff->setVisible(false);
        ui->scrollListeMachines->setVisible(true);
        ui->btnHistoriqueToggle->setChecked(false);
        // TODO: Implement modify logic
    });
    
    connect(actionSupprimer, &QAction::triggered, this, [this]() {
        // Hide form and lists, show summary
        ui->widgetFormulaireParcMachines->setVisible(false);
        ui->scrollHistoriqueOnOff->setVisible(false);
        ui->scrollListeMachines->setVisible(true);
        ui->btnHistoriqueToggle->setChecked(false);
        // TODO: Implement delete logic
    });
    
    connect(actionActualiser, &QAction::triggered, this, [this]() {
        // Hide form and lists, show summary
        ui->widgetFormulaireParcMachines->setVisible(false);
        ui->scrollHistoriqueOnOff->setVisible(false);
        ui->scrollListeMachines->setVisible(true);
        ui->btnHistoriqueToggle->setChecked(false);
        // TODO: Implement refresh logic
    });
    
    connect(actionExporter, &QAction::triggered, this, [this]() {
        // Hide form and lists, show summary
        ui->widgetFormulaireParcMachines->setVisible(false);
        ui->scrollHistoriqueOnOff->setVisible(false);
        ui->scrollListeMachines->setVisible(true);
        ui->btnHistoriqueToggle->setChecked(false);
        // TODO: Implement export logic
    });
    
    // Connect Historique ON/OFF toggle button
    connect(ui->btnHistoriqueToggle, &QPushButton::toggled, this, [this](bool checked) {
        // Hide form if visible
        ui->widgetFormulaireParcMachines->setVisible(false);
        
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
    navigationBar->addTab("ℹ️", "Détails machine");
    navigationBar->addTab("📊", "Statistiques");
    navigationBar->addTab("�", "Historique");

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
    
    // Show form
    ui->widgetFormulaireParcMachines->setVisible(true);
    qDebug() << "[DEBUG] Formulaire visible:" << ui->widgetFormulaireParcMachines->isVisible();
}

void machine::masquerFormulaireParc()
{
    qDebug() << "[DEBUG] Masquage du formulaire";
    // Hide form
    ui->widgetFormulaireParcMachines->setVisible(false);
    
    // Show machine list and summary
    ui->scrollListeMachines->setVisible(true);
    ui->scrollHistoriqueOnOff->setVisible(false);
    ui->tableMachines->setVisible(false);
    ui->frameSummary->setVisible(true);
    qDebug() << "[DEBUG] Formulaire visible:" << ui->widgetFormulaireParcMachines->isVisible();
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
