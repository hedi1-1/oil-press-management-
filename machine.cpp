#include "machine.h"
#include "connexionmachine.h"
#include "ui_machine.h"
#include <QBrush>
#include <QCheckBox>
#include <QComboBox>
#include <QDateEdit>
#include <QDateTime>
#include <QDebug>
#include <QDesktopServices>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFileDialog>
#include <QFormLayout>
#include <QHeaderView>
#include <QMap>
#include <QMessageBox>
#include <QPainter>
#include <QPrinter>
#include <QRegularExpression>
#include <QSpacerItem>
#include <QSpinBox>
#include <QSet>
#include <QSqlError>
#include <QSqlQuery>
#include <QTextDocumentWriter>
#include <QButtonGroup>
#include <QRadioButton>
#include <QFile>
#include <QTextStream>
#include <QVBoxLayout>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QCategoryAxis>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QPieSeries>
#include <QtCharts/QPieSlice>
#include <QtCharts/QValueAxis>
// ============================================================================
// NavigationBar Implementation
// ============================================================================

NavigationBar::NavigationBar(QWidget *parent)
    : QWidget(parent), layout(nullptr), activeTabIndex(-1) {
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

NavigationBar::~NavigationBar() {}

void NavigationBar::setupLayout() {
  layout = new QHBoxLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(0);
  setLayout(layout);
}

void NavigationBar::addTab(const QString &icon, const QString &text) {
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
  connect(button, &QPushButton::clicked, this,
          &NavigationBar::onTabButtonClicked);

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

void NavigationBar::setActiveTab(int index) {
  if (index < 0 || index >= buttons.size()) {
    return;
  }

  activeTabIndex = index;
  updateStyles();
  emit tabClicked(index);
}

int NavigationBar::getCurrentTab() const { return activeTabIndex; }

void NavigationBar::clear() {
  // Remove all buttons from layout and delete them
  while (!buttons.isEmpty()) {
    QPushButton *button = buttons.takeFirst();
    layout->removeWidget(button);
    delete button;
  }
  activeTabIndex = -1;
}

void NavigationBar::updateStyles() {
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

void NavigationBar::onTabButtonClicked() {
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
    : QMainWindow(parent), ui(new Ui::machine), navigationBar(nullptr),
  employeeTableModel(nullptr), historiqueTableModel(nullptr), m_selectedRow(-1),
      machineCardPollingTimer(nullptr), machineCardDialog(nullptr),
      countsTimer(nullptr), historiqueTimer(nullptr) {
  ui->setupUi(this);

  // Disable toolbar buttons initially
  ui->btnModifierMachine_machine->setEnabled(false);
  ui->btnSupprimerMachine_machine->setEnabled(false);
  ui->btnExporter_machine->setEnabled(false);
  ui->btnToggleOnOff_machine->setEnabled(false);
  ui->btnCarteMachine->setEnabled(false);

  // Connect carte machine button
  connect(ui->btnCarteMachine, &QPushButton::clicked, this,
          &machine::showMachineCard);

  // Fix grid column stretches and spacing to reduce gap between left/right
  // blocks
  QGridLayout *grids[] = {ui->gridCard1Info, ui->gridCard2Info,
                          ui->gridCard3Info};
  for (auto grid : grids) {
    // Labels: no stretch, values: stretch to fill, col 4 (icons): compact at
    // right
    grid->setColumnStretch(0, 0);
    grid->setColumnStretch(1, 1);
    grid->setColumnStretch(2, 0);
    grid->setColumnStretch(3, 1);
    grid->setColumnStretch(4, 0);
    // Set fixed minimum widths to keep blocks compact and close together
    grid->setColumnMinimumWidth(0, 160); // Label gauche
    grid->setColumnMinimumWidth(1, 200); // Valeur gauche
    grid->setColumnMinimumWidth(2, 120); // Label droite
    grid->setColumnMinimumWidth(3, 120); // Valeur droite
    grid->setHorizontalSpacing(15);
  }

  // Setup navigation bar
  setupNavigationBar();

  // Connect back button
  connect(ui->btnBackToMenu_machine, &QPushButton::clicked, this,
          &machine::onBackButtonClicked);

  // Setup date/time timer
  dateTimeTimer = new QTimer(this);
  connect(dateTimeTimer, &QTimer::timeout, this, &machine::updateDateTime);
  dateTimeTimer->start(1000); // Update every second
  updateDateTime();           // Initial update

  onNavigationTabClicked(0);

  // Initialize and load data
  setupMachineTable();
  setupEmployeesTable();
  
  // Initialize date filters to show all machines by default
  ui->date_debut->setDate(QDate(2000, 1, 1));
  ui->date_fin->setDate(QDate::currentDate().addYears(5));
  // Explicit connection to guarantee button functionality
  connect(ui->btnFiltrer, &QPushButton::clicked, this, &machine::appliquerFiltres);

  // Connexion du bouton Actualiser 🔄
  connect(ui->btnActualiserGlobal, &QPushButton::clicked, this, [this]() {
    // Réinitialiser les champs du formulaire de filtrage
    ui->filtre_type_machine->setCurrentIndex(0);
    ui->filtre_etat_marche_machine->setCurrentIndex(0);
    ui->filtre_type_alerte->setCurrentIndex(0);
    ui->filtre_alerte_criticite->setCurrentIndex(0);
    ui->filtre_responsable->setCurrentIndex(0);
    ui->tri_par->setCurrentIndex(0);
    
    // Réinitialiser les dates par défaut
    ui->date_debut->setDate(QDate(2000, 1, 1));
    ui->date_fin->setDate(QDate::currentDate().addYears(5));
    
    // Déclencher le rechargement du tableau complet sans filtres
    appliquerFiltres();
  });

  chargerMachines();
  chargerEmployees();

  // Export PDF button
  connect(ui->btnExporter_machine, &QPushButton::clicked, this,
          [this]() { afficherDialogExport(); });

  // Connect Historique toggle button
  ui->btnHistoriqueToggle_machine->setText(
      QString::fromUtf8("\xF0\x9F\x93\x9C Historique"));
  connect(ui->btnHistoriqueToggle_machine, &QPushButton::toggled, this,
          [this](bool checked) {
            if (checked) {
              chargerHistorique();
              ui->scrollHistoriqueOnOff_machine->setVisible(true);
              ui->tableMachines_machine->setVisible(false);
              ui->scrollListeMachines->setVisible(false);
            } else {
              ui->scrollHistoriqueOnOff_machine->setVisible(false);
              ui->tableMachines_machine->setVisible(true);
              ui->scrollListeMachines->setVisible(false);
            }
          });

  // Connect close button for Historique ON/OFF
  connect(ui->btnFermerHistorique, &QPushButton::clicked, this, [this]() {
    ui->btnHistoriqueToggle_machine->setChecked(false);
    ui->scrollHistoriqueOnOff_machine->setVisible(false);
    ui->scrollListeMachines->setVisible(false);
    ui->tableMachines_machine->setVisible(true);
  });

  // Connect search and filter buttons
  connect(ui->btnRechercher, &QPushButton::clicked, this,
          [this]() { rechercherMachines(); });

  // Enter key on search field = same as clicking search button
  connect(ui->recherche_nom_machine, &QLineEdit::returnPressed, this,
          [this]() { rechercherMachines(); });

  // Install event filter for Escape key on search field and table
  ui->recherche_nom_machine->installEventFilter(this);
  ui->tableMachines_machine->installEventFilter(this);

  connect(ui->btnFiltrer, &QPushButton::clicked, this, [this]() {
    ui->scrollHistoriqueOnOff_machine->setVisible(false);
    ui->scrollListeMachines->setVisible(false);
    ui->tableMachines_machine->setVisible(true);
    ui->btnHistoriqueToggle_machine->setChecked(false);
    // TODO: Implement filter logic
  });

  // Connect toolbar buttons
  connect(ui->btnSupprimerMachine_machine, &QPushButton::clicked, this,
          &machine::on_btnSupprimerMachine_machine_clicked);
  connect(ui->btnModifierMachine_machine, &QPushButton::clicked, this,
          &machine::on_btnModifierMachine_machine_clicked);

  // Connect ON/OFF toggle button
  connect(ui->btnToggleOnOff_machine, &QPushButton::clicked, this, [this]() {
    if (m_selectedMachineId.isEmpty() || m_selectedRow < 0)
      return;

    // Get current state from table
    QString currentState = machineTableModel->item(m_selectedRow, 3)->text();
    QString newState = (currentState == "ON") ? "OFF" : "ON";

    // Update in database
    QSqlQuery query(ConnectionMachine::getInstance().getDatabase());
    query.prepare("UPDATE MACHINE SET ETAT_MARCHE = :etat, DATE_MISE_A_JOUR = "
                  "SYSTIMESTAMP WHERE ID_MACHINE = :id");
    query.bindValue(":etat", newState);
    query.bindValue(":id", m_selectedMachineId);

    if (query.exec()) {
      QMessageBox msgBox(this);
      msgBox.setWindowTitle("Succès");
      msgBox.setText(QString("✅ Machine %1 avec succès !")
                         .arg(newState == "ON" ? "allumée" : "éteinte"));
      msgBox.setIcon(QMessageBox::Information);
      msgBox.setStyleSheet(R"(
        QMessageBox { background-color: #f8f9fa; border: 2px solid #1A3C2F; }
        QLabel { color: #1A3C2F; font-size: 14px; font-weight: bold; padding: 10px; }
        QPushButton { background-color: #1A3C2F; color: white; border: none;
          border-radius: 5px; padding: 8px 20px; font-weight: bold; min-width: 100px; }
        QPushButton:hover { background-color: #0F241E; }
      )");
      msgBox.exec();
      chargerMachines();
    } else {
      QMessageBox::critical(this, "Erreur",
                            "Erreur lors du changement d'état : " +
                                query.lastError().text());
    }
  });

  connect(ui->tableMachines_machine, &QTableView::doubleClicked, this,
          &machine::on_tableMachines_machine_doubleClicked);

  // Setup todo list for maintenance
  setupTodoList();

  // Initialize "Actions" form dates
  ui->dateEdit_derniere_maintenance_machine->setDate(QDate::currentDate());

  // Setup real-time counters polling timer
  countsTimer = new QTimer(this);
  connect(countsTimer, &QTimer::timeout, this, &machine::updateMachineCounts);
  countsTimer->start(10000); // 10 seconds
  updateMachineCounts();     // Initial call

  // Statistics setup
  if (ui->type_statistique_machine->findText("Type machine") == -1) {
    ui->type_statistique_machine->addItem("Type machine");
  }
  connect(ui->btnGenererStats_machine, &QPushButton::clicked, this,
          &machine::on_btnGenererStats_machine_clicked);
  clearStatsChartArea("Sélectionnez un type de statistique et un type de graphique, puis cliquez sur Générer.");
}

machine::~machine() {
  if (dateTimeTimer) {
    dateTimeTimer->stop();
  }
  delete ui;
}

void machine::updateDateTime() {
  QDateTime now = QDateTime::currentDateTime();

  // Noms des mois en français
  QStringList monthsFR = {"janvier",   "février", "mars",     "avril",
                          "mai",       "juin",    "juillet",  "août",
                          "septembre", "octobre", "novembre", "décembre"};

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

  QString dateTimeStr =
      QString("📅 %1 %2 %3 • %4")
          .arg(dateStr, monthStr, QString::number(year), timeStr);

  ui->lblDateTime_machine->setText(dateTimeStr);
}

void machine::onBackButtonClicked() {
  emit backToMenu();
  this->hide();
}

void machine::setupNavigationBar() {
  // Hide the default QTabBar
  ui->tabWidgetMachineManagement->tabBar()->hide();

  // Create the custom navigation bar
  navigationBar = new NavigationBar(this);

  // Add tabs to the navigation bar
  navigationBar->addTab("🏭", "Parc machines");
  navigationBar->addTab("⚙️", "Actions");
  navigationBar->addTab("📊", "Statistiques");
  navigationBar->addTab("🤖", "Décision IA");

  // Insert the navigation bar at the top of the central widget
  QVBoxLayout *mainLayout =
      qobject_cast<QVBoxLayout *>(ui->centralwidget->layout());
  if (mainLayout) {
    // Insert navigation bar after the header (at index 1)
    mainLayout->insertWidget(1, navigationBar);
  }

  // Connect navigation bar signals to tab widget
  connect(navigationBar, QOverload<int>::of(&NavigationBar::tabClicked), this,
          &machine::onNavigationTabClicked);

  // Set the first tab as active
  navigationBar->setActiveTab(0);
}

void machine::setupMachineTable() {
  // Create model with columns matching requested order (17 columns)
  machineTableModel = new QStandardItemModel(0, 17, this);
  machineTableModel->setHorizontalHeaderLabels(
      {"ID", "Nom", "Type", "État marche", "Temp. (°C)", "Charge (%)",
       "Fonctionnement", "Alerte", "Criticité", "Dernière maintenance",
       "Date d'installation", "Score Santé", "Date mise à jour", "Responsable",
       "Refroidissement", "Tag", "Priorité"});

  // Configure UI table view
  ui->tableMachines_machine->setModel(machineTableModel);

  // Style the table to match the interface design
  ui->tableMachines_machine->setStyleSheet(R"(
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
  ui->tableMachines_machine->setAlternatingRowColors(true);
  ui->tableMachines_machine->setSelectionBehavior(
      QAbstractItemView::SelectRows);
  ui->tableMachines_machine->setSelectionMode(
      QAbstractItemView::SingleSelection);
  ui->tableMachines_machine->setSortingEnabled(true);
  ui->tableMachines_machine->verticalHeader()->setVisible(false);
  // Désactive le stretch automatique pour permettre le scroll horizontal
  ui->tableMachines_machine->horizontalHeader()->setStretchLastSection(false);
  ui->tableMachines_machine->horizontalHeader()->setSectionResizeMode(
      QHeaderView::Interactive);
  ui->tableMachines_machine->setHorizontalScrollBarPolicy(
      Qt::ScrollBarAsNeeded);
  ui->tableMachines_machine->setShowGrid(true);

  // Set row height
  ui->tableMachines_machine->verticalHeader()->setDefaultSectionSize(40);
}

void machine::setupEmployeesTable() {
  employeeTableModel = new QStandardItemModel(0, 7, this);
  employeeTableModel->setHorizontalHeaderLabels(
      {"ID", "Nom utilisateur", "Email", "Role", "Etat", "Cree le", "Derniere connexion"});

  ui->tableEmployees_machine->setModel(employeeTableModel);
  ui->tableEmployees_machine->setStyleSheet(R"(
        QTableView {
            border: 2px solid #1A3C2F;
            border-radius: 6px;
            background-color: #FFFFFF;
            alternate-background-color: #F0F7F4;
            gridline-color: #D5E8D4;
            font-size: 11px;
            font-family: 'Segoe UI', Arial, sans-serif;
            selection-background-color: #C8E6C9;
            selection-color: #1A3C2F;
        }
        QTableView::item {
            padding: 6px 10px;
            border-bottom: 1px solid #E8E8E8;
        }
        QHeaderView::section {
            background-color: #1B4D3E;
            color: white;
            padding: 8px;
            border: none;
            border-right: 1px solid #2C5F4F;
            font-weight: bold;
            font-size: 11px;
            font-family: 'Segoe UI', Arial, sans-serif;
        }
        QHeaderView::section:last {
            border-right: none;
        }
    )");

  ui->tableEmployees_machine->setAlternatingRowColors(true);
  ui->tableEmployees_machine->setSelectionBehavior(QAbstractItemView::SelectRows);
  ui->tableEmployees_machine->setSelectionMode(QAbstractItemView::SingleSelection);
  ui->tableEmployees_machine->setEditTriggers(QAbstractItemView::NoEditTriggers);
  ui->tableEmployees_machine->verticalHeader()->setVisible(false);
  ui->tableEmployees_machine->horizontalHeader()->setStretchLastSection(true);
  ui->tableEmployees_machine->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
  ui->tableEmployees_machine->setWordWrap(false);

  connect(ui->tableEmployees_machine, &QTableView::clicked, this,
          [this](const QModelIndex &index) {
            if (!index.isValid()) {
              return;
            }
            onEmployeeRowChosen(index.row());
          });
}

void machine::chargerEmployees() {
  if (!employeeTableModel) {
    return;
  }

  employeeTableModel->setRowCount(0);

  QSqlQuery query(ConnectionMachine::getInstance().getDatabase());
  query.prepare("SELECT USER_ID, USERNAME, EMAIL, ROLE, STATE, CREATED_AT, LAST_LOGIN "
                "FROM EMPLOYEES ORDER BY USER_ID ASC");

  if (!query.exec()) {
    qDebug() << "Erreur lors du chargement des employes :" << query.lastError().text();
    return;
  }

  QFont boldFont;
  boldFont.setBold(true);

  while (query.next()) {
    QList<QStandardItem *> row;

    const QString id = query.value(0).toString();
    const QString username = query.value(1).toString();
    const QString email = query.value(2).toString();
    const QString role = query.value(3).toString();
    const QString state = query.value(4).toString();
    const QDateTime createdAt = query.value(5).toDateTime();
    const QDateTime lastLogin = query.value(6).toDateTime();

    const QString createdAtStr = createdAt.isValid() ? createdAt.toString("dd/MM/yyyy HH:mm") : "-";
    const QString lastLoginStr = lastLogin.isValid() ? lastLogin.toString("dd/MM/yyyy HH:mm") : "Jamais";

    const QStringList values = {id, username, email, role, state, createdAtStr, lastLoginStr};

    for (const QString &value : values) {
      auto *item = new QStandardItem(value);
      item->setEditable(false);
      item->setTextAlignment(Qt::AlignCenter);
      item->setForeground(QColor("#333333"));
      row.append(item);
    }

    row[3]->setFont(boldFont);
    row[4]->setFont(boldFont);

    const QString normalizedState = state.trimmed().toUpper();
    if (normalizedState == "ACTIVE") {
      row[4]->setForeground(QColor("#2E7D32"));
    } else if (normalizedState == "INACTIVE" || normalizedState == "LOCKED" || normalizedState == "BLOCKED") {
      row[4]->setForeground(QColor("#C62828"));
    } else {
      row[4]->setForeground(QColor("#E65100"));
    }

    for (QStandardItem *item : row) {
      item->setData(item->foreground(), Qt::UserRole);
    }

    employeeTableModel->appendRow(row);
  }

  applyEmployeeSelectionLockState();
}

void machine::onEmployeeRowChosen(int row) {
  if (!employeeTableModel || row < 0 || row >= employeeTableModel->rowCount()) {
    return;
  }

  QStandardItem *idItem = employeeTableModel->item(row, 0);
  if (!idItem) {
    return;
  }

  m_selectedEmployeeId = idItem->text().toInt();
  m_selectedEmployeeRow = row;
  m_employeeSelectionLocked = true;

  applyEmployeeSelectionLockState();
  ui->tableEmployees_machine->selectRow(row);
}

void machine::applyEmployeeSelectionLockState() {
  if (!employeeTableModel) {
    return;
  }

  const QColor lockedFg("#A0A0A0");
  const QColor lockedBg("#ECECEC");
  const QColor activeBg("#FFFFFF");

  // Reposition selected row if table has been refreshed.
  if (m_employeeSelectionLocked && m_selectedEmployeeId >= 0) {
    int foundRow = -1;
    for (int r = 0; r < employeeTableModel->rowCount(); ++r) {
      QStandardItem *idItem = employeeTableModel->item(r, 0);
      if (idItem && idItem->text().toInt() == m_selectedEmployeeId) {
        foundRow = r;
        break;
      }
    }

    if (foundRow >= 0) {
      m_selectedEmployeeRow = foundRow;
    } else {
      m_employeeSelectionLocked = false;
      m_selectedEmployeeId = -1;
      m_selectedEmployeeRow = -1;
    }
  }

  for (int r = 0; r < employeeTableModel->rowCount(); ++r) {
    const bool selectedRow = (m_employeeSelectionLocked && r == m_selectedEmployeeRow);

    for (int c = 0; c < employeeTableModel->columnCount(); ++c) {
      QStandardItem *item = employeeTableModel->item(r, c);
      if (!item) {
        continue;
      }

      if (m_employeeSelectionLocked && !selectedRow) {
        item->setFlags(Qt::ItemIsEnabled);
        item->setForeground(lockedFg);
        item->setBackground(lockedBg);
      } else {
        item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        const QVariant savedColor = item->data(Qt::UserRole);
        if (savedColor.isValid()) {
          item->setForeground(savedColor.value<QBrush>());
        }
        item->setBackground(activeBg);
      }
    }
  }
}

void machine::clearEmployeeSelection() {
  m_selectedEmployeeId = -1;
  m_selectedEmployeeRow = -1;
  m_employeeSelectionLocked = false;

  if (ui->tableEmployees_machine) {
    ui->tableEmployees_machine->clearSelection();
  }

  applyEmployeeSelectionLockState();
}

void machine::onNavigationTabClicked(int index) {
  // When changing tabs, hide form and reset view
  if (index == 0) {
    // Parc machines tab: show table by default
    ui->scrollHistoriqueOnOff_machine->setVisible(false);
    ui->scrollListeMachines->setVisible(false);
    ui->tableMachines_machine->setVisible(true);
    ui->btnHistoriqueToggle_machine->setChecked(false);
  } else {
    // Other tabs: hide all parc machines specific views
    ui->scrollHistoriqueOnOff_machine->setVisible(false);
    ui->scrollListeMachines->setVisible(false);
    ui->tableMachines_machine->setVisible(false);

    if (index == 1) {
      // Keep employees list in sync when user opens Actions tab.
      chargerEmployees();
    }
  }

  // Update the tab widget to show the corresponding tab
  ui->tabWidgetMachineManagement->setCurrentIndex(index);
}

// ============================================================================
// Todo List (Machines à réparer) Implementation
// ============================================================================

void machine::setupTodoList() {
  // Connect the "+ Ajouter" button
  connect(ui->btnAjouterTodo_machine, &QPushButton::clicked, this,
          &machine::showAddTodoDialog);

  // Add sample entries for demonstration
  addTodoItem("Presse Hydraulique A3", "Siemens", "Urgent");
  addTodoItem("Convoyeur B12", "Bosch", "Normal");
  addTodoItem("Compresseur C7", "Atlas Copco", "Urgent");
}

void machine::addTodoItem(const QString &machineName, const QString &fabricant,
                          const QString &priority) {
  TodoItem item;
  item.machineName = machineName;
  item.fabricant = fabricant;
  item.priority = priority;
  item.done = false;
  m_todoItems.append(item);
  refreshTodoList();
}

void machine::onTodoCheckToggled(int index, bool checked) {
  if (index >= 0 && index < m_todoItems.size()) {
    m_todoItems[index].done = checked;
    refreshTodoList();
  }
}

QWidget *machine::createTodoItemWidget(int index) {
  const TodoItem &item = m_todoItems[index];

  QWidget *row = new QWidget();
  row->setObjectName("todoRow");

  // Styles based on done state
  if (item.done) {
    row->setStyleSheet("QWidget#todoRow { background-color: #F5F5F5; "
                       "border-radius: 4px; padding: 2px; }");
  } else {
    row->setStyleSheet("QWidget#todoRow { background-color: white; border: 1px "
                       "solid #E8E8E8; border-radius: 4px; padding: 2px; }");
  }

  QHBoxLayout *rowLayout = new QHBoxLayout(row);
  rowLayout->setContentsMargins(10, 6, 10, 6);
  rowLayout->setSpacing(10);

  // Checkbox
  QCheckBox *cb = new QCheckBox();
  cb->setChecked(item.done);
  cb->setStyleSheet("QCheckBox::indicator { width: 18px; height: 18px; }"
                    "QCheckBox::indicator:unchecked { border: 2px solid #999; "
                    "border-radius: 4px; background: white; }"
                    "QCheckBox::indicator:checked { border: 2px solid #2E7D32; "
                    "border-radius: 4px; background: #2E7D32; }");
  // Capture index for lambda
  int idx = index;
  connect(cb, &QCheckBox::toggled, this,
          [this, idx](bool checked) { onTodoCheckToggled(idx, checked); });
  rowLayout->addWidget(cb);

  // Machine name label
  QLabel *lblName = new QLabel(item.machineName);
  if (item.done) {
    lblName->setStyleSheet(
        "font-size: 12px; color: #AAA; text-decoration: line-through; "
        "background: transparent; border: none;");
  } else {
    lblName->setStyleSheet("font-size: 12px; font-weight: bold; color: #333; "
                           "background: transparent; border: none;");
  }
  rowLayout->addWidget(lblName);

  // Fabricant label
  QLabel *lblFab = new QLabel(item.fabricant);
  if (item.done) {
    lblFab->setStyleSheet(
        "font-size: 11px; color: #BBB; text-decoration: line-through; "
        "background: transparent; border: none;");
  } else {
    lblFab->setStyleSheet(
        "font-size: 11px; color: #777; background: transparent; border: none;");
  }
  rowLayout->addWidget(lblFab);

  // Spacer
  rowLayout->addStretch();

  // Priority badge
  QLabel *badge = new QLabel(item.priority);
  if (item.done) {
    badge->setStyleSheet(
        "font-size: 10px; font-weight: bold; color: #CCC; background-color: "
        "#F0F0F0;"
        "border-radius: 4px; padding: 2px 10px; border: none;");
  } else if (item.priority == "Urgent") {
    badge->setStyleSheet(
        "font-size: 10px; font-weight: bold; color: white; background-color: "
        "#D32F2F;"
        "border-radius: 4px; padding: 2px 10px; border: none;");
  } else {
    badge->setStyleSheet(
        "font-size: 10px; font-weight: bold; color: white; background-color: "
        "#F57F17;"
        "border-radius: 4px; padding: 2px 10px; border: none;");
  }
  badge->setAlignment(Qt::AlignCenter);
  rowLayout->addWidget(badge);

  // Delete button
  QPushButton *btnDel = new QPushButton("✕");
  btnDel->setFixedSize(22, 22);
  btnDel->setCursor(Qt::PointingHandCursor);
  btnDel->setStyleSheet("QPushButton { background: transparent; color: #999; "
                        "font-size: 13px; border: none; padding: 0px; }"
                        "QPushButton:hover { color: #D32F2F; }");
  connect(btnDel, &QPushButton::clicked, this, [this, idx]() {
    if (idx >= 0 && idx < m_todoItems.size()) {
      m_todoItems.removeAt(idx);
      refreshTodoList();
    }
  });
  rowLayout->addWidget(btnDel);

  return row;
}

void machine::refreshTodoList() {
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
    if (ia.done != ib.done)
      return !ia.done;
    // Among not-done, Urgent first
    if (!ia.done && !ib.done) {
      if (ia.priority == "Urgent" && ib.priority != "Urgent")
        return true;
      if (ia.priority != "Urgent" && ib.priority == "Urgent")
        return false;
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
      new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding));

  // Update counter in title
  int pending = 0;
  for (const auto &item : m_todoItems) {
    if (!item.done)
      pending++;
  }
  ui->lblTodoTitle->setText(QString("🔧 Machines à réparer (%1)").arg(pending));
}

void machine::showAddTodoDialog() {
  QDialog dialog(this);
  dialog.setWindowTitle("🔧 Ajouter une intervention");
  dialog.setFixedSize(380, 240);
  dialog.setStyleSheet(
      "QDialog { background-color: #1A3C2F; }"
      "QLabel { font-size: 12px; color: #E0E0E0; font-weight: bold; }"
      "QLineEdit { padding: 6px 10px; border: 1px solid #2D5F47; "
      "border-radius: 5px; font-size: 12px; background-color: #F5F5F5; color: "
      "#333; }"
      "QComboBox { padding: 6px 10px; border: 1px solid #2D5F47; "
      "border-radius: 5px; font-size: 12px; background-color: #F5F5F5; color: "
      "#333; }"
      "QComboBox::drop-down { border: none; }"
      "QComboBox QAbstractItemView { background: white; color: #333; "
      "selection-background-color: #2D5F47; selection-color: white; }");

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

  QDialogButtonBox *buttons =
      new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
  buttons->button(QDialogButtonBox::Ok)->setText("Ajouter");
  buttons->button(QDialogButtonBox::Cancel)->setText("Annuler");
  buttons->button(QDialogButtonBox::Ok)
      ->setStyleSheet("QPushButton { background-color: #C9A227; color: "
                      "#1A3C2F; font-weight: bold; font-size: 12px;"
                      "border: none; border-radius: 5px; padding: 8px 20px; }"
                      "QPushButton:hover { background-color: #B8911F; }");
  buttons->button(QDialogButtonBox::Cancel)
      ->setStyleSheet(
          "QPushButton { background-color: rgba(255,255,255,0.15); color: "
          "#CCC; font-size: 12px;"
          "border: 1px solid #4A7A66; border-radius: 5px; padding: 8px 20px; }"
          "QPushButton:hover { background-color: rgba(255,255,255,0.25); "
          "color: white; }");
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

machine::MachineFormData machine::collectMachineFormData() const {
  MachineFormData data;
  data.nom = ui->lineEdit_nom_machine->text().trimmed();
  data.type = ui->comboBox_type_machine->currentText();
  data.etatMarche = ui->comboBox_etat_marche_machine->currentText();
  data.temperature = ui->doubleSpinBox_temperature_machine->value();
  data.charge = ui->doubleSpinBox_niveau_charge_machine->value();
  data.etatFonctionnement =
      ui->comboBox_etat_fonctionnement_machine->currentText();
  data.typeAlerte = ui->comboBox_type_alerte_machine->currentText();
  data.criticite = ui->comboBox_niveau_criticite_machine->currentText();
  data.derniereMaintenance = ui->dateEdit_derniere_maintenance_machine->date();
  data.scoreSante = ui->spinBox_score_sante_machine->value();
  return data;
}

bool machine::machineHasEmployeeColumn() const {
  return !machineEmployeeColumnName().isEmpty();
}

QString machine::machineEmployeeColumnName() const {
  QSqlQuery query(ConnectionMachine::getInstance().getDatabase());
  query.prepare("SELECT COLUMN_NAME "
                "FROM USER_TAB_COLUMNS "
                "WHERE TABLE_NAME = 'MACHINE' "
                "AND COLUMN_NAME IN ('USER_ID', 'ID_USER') "
                "ORDER BY CASE COLUMN_NAME WHEN 'USER_ID' THEN 1 ELSE 2 END");

  if (!query.exec()) {
    return QString();
  }

  if (query.next()) {
    return query.value(0).toString();
  }

  return QString();
}

bool machine::isMachineNameAvailable(const QString &name) const {
  const QString trimmed = name.trimmed();
  if (trimmed.isEmpty()) {
    return false;
  }

  QSqlQuery query(ConnectionMachine::getInstance().getDatabase());
  query.prepare("SELECT COUNT(*) FROM MACHINE "
                "WHERE UPPER(TRIM(NOM_MACHINE)) = UPPER(TRIM(:nom))");
  query.bindValue(":nom", trimmed);

  if (!query.exec()) {
    qDebug() << "Erreur vérification unicité NOM_MACHINE :"
             << query.lastError().text();
    return false;
  }

  if (!query.next()) {
    return false;
  }

  return query.value(0).toInt() == 0;
}

bool machine::insertMachine(const MachineFormData &data, QString *errorMessage) {
  QSqlQuery query(ConnectionMachine::getInstance().getDatabase());
  const bool hasEmployeeSelection = (m_selectedEmployeeId >= 0);
  const QString employeeColumn = machineEmployeeColumnName();
  const bool hasEmployeeColumn = !employeeColumn.isEmpty();

  if (hasEmployeeSelection && !hasEmployeeColumn) {
    if (errorMessage) {
      *errorMessage =
          "La colonne MACHINE.USER_ID (ou ID_USER) est absente. Ajoutez-la pour enregistrer l'employe responsable.";
    }
    return false;
  }

  if (hasEmployeeSelection) {
    query.prepare(
        "INSERT INTO MACHINE (NOM_MACHINE, TYPE_MACHINE, ETAT_MARCHE, "
        "TEMPERATURE_ACTUELLE, NIVEAU_CHARGE, ETAT_FONCTIONNEMENT, TYPE_ALERTE, "
      "NIVEAU_CRITICITE, DATE_DERNIERE_MAINTENANCE, SCORE_SANTE, " + employeeColumn + ") "
        "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");

    query.addBindValue(data.nom);
    query.addBindValue(data.type);
    query.addBindValue(data.etatMarche);
    query.addBindValue(data.temperature);
    query.addBindValue(data.charge);
    query.addBindValue(data.etatFonctionnement);
    query.addBindValue(data.typeAlerte);
    query.addBindValue(data.criticite);
    query.addBindValue(data.derniereMaintenance);
    query.addBindValue(data.scoreSante);
    query.addBindValue(m_selectedEmployeeId);
  } else {
    query.prepare(
        "INSERT INTO MACHINE (NOM_MACHINE, TYPE_MACHINE, ETAT_MARCHE, "
        "TEMPERATURE_ACTUELLE, NIVEAU_CHARGE, ETAT_FONCTIONNEMENT, TYPE_ALERTE, "
        "NIVEAU_CRITICITE, DATE_DERNIERE_MAINTENANCE, SCORE_SANTE) "
        "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");

    query.addBindValue(data.nom);
    query.addBindValue(data.type);
    query.addBindValue(data.etatMarche);
    query.addBindValue(data.temperature);
    query.addBindValue(data.charge);
    query.addBindValue(data.etatFonctionnement);
    query.addBindValue(data.typeAlerte);
    query.addBindValue(data.criticite);
    query.addBindValue(data.derniereMaintenance);
    query.addBindValue(data.scoreSante);
  }

  if (!query.exec()) {
    if (errorMessage) {
      *errorMessage = query.lastError().text();
    }
    return false;
  }

  return true;
}

bool machine::saveMachineWithName(const QString &name,
                                  const MachineFormData &baseData) {
  MachineFormData finalData = baseData;
  finalData.nom = name.trimmed();

  QString errorMessage;
  if (!insertMachine(finalData, &errorMessage)) {
    QMessageBox::critical(this, "Erreur",
                          "Échec de l'ajout dans la base de données : " +
                              errorMessage);
    return false;
  }

  QMessageBox msgBox(this);
  msgBox.setWindowTitle("Succès");
  if (m_selectedEmployeeId >= 0) {
    msgBox.setText(QString("✅ Machine ajoutée avec succès (ID employé responsable: %1) !")
                       .arg(m_selectedEmployeeId));
  } else {
    msgBox.setText("✅ Machine ajoutée avec succès !");
  }
  msgBox.setIcon(QMessageBox::Information);
  msgBox.setStyleSheet(R"(
            QMessageBox {
                background-color: #f8f9fa;
                border: 2px solid #1A3C2F;
            }
            QLabel {
                color: #1A3C2F;
                font-size: 14px;
                font-weight: bold;
                padding: 10px;
            }
            QPushButton {
                background-color: #1A3C2F;
                color: white;
                border: none;
                border-radius: 5px;
                padding: 8px 20px;
                font-weight: bold;
                min-width: 100px;
            }
            QPushButton:hover {
                background-color: #0F241E;
            }
        )");
  msgBox.exec();

  chargerMachines();
  return true;
}

QStringList machine::buildAvailableNameSuggestions(
    const QString &problematicName, int maxSuggestions) const {
  QStringList available;
  QSet<QString> tested;

  auto addCandidateIfAvailable = [&](const QString &candidate) {
    const QString normalized = candidate.trimmed();
    if (normalized.isEmpty()) {
      return;
    }
    if (normalized.compare(problematicName.trimmed(), Qt::CaseInsensitive) ==
        0) {
      return;
    }
    if (tested.contains(normalized.toLower())) {
      return;
    }

    tested.insert(normalized.toLower());
    if (isMachineNameAvailable(normalized)) {
      available.append(normalized);
    }
  };

  const QString base = problematicName.trimmed();
  QRegularExpression trailingNumberRegex("^(.*?)(\\d+)$");
  QRegularExpressionMatch match = trailingNumberRegex.match(base);
  if (match.hasMatch()) {
    const QString prefix = match.captured(1);
    const QString digits = match.captured(2);
    const int number = digits.toInt();
    const int width = digits.length();

    addCandidateIfAvailable(prefix +
                            QString("%1").arg(number + 1, width, 10, QChar('0')));
    addCandidateIfAvailable(prefix +
                            QString("%1").arg(number + 2, width, 10, QChar('0')));
    addCandidateIfAvailable(prefix + QString::number(number + 1));
    addCandidateIfAvailable(prefix + "-" + QString::number(number + 1));
    addCandidateIfAvailable(prefix + "_" + QString::number(number + 1));
    addCandidateIfAvailable(prefix + "-A");
  }

  addCandidateIfAvailable(base + "-001");
  addCandidateIfAvailable(base + "-01");
  addCandidateIfAvailable(base + "-1");
  addCandidateIfAvailable(base + "_01-A");
  addCandidateIfAvailable(base + "_A");
  addCandidateIfAvailable(base + "_NEW");

  for (int i = 1; i <= 200 && available.size() < maxSuggestions; ++i) {
    addCandidateIfAvailable(base + "-" + QString::number(i));
  }

  if (available.size() > maxSuggestions) {
    available = available.mid(0, maxSuggestions);
  }

  return available;
}

void machine::showRequiredNameDialog(const MachineFormData &baseData) {
  QDialog dialog(this);
  dialog.setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
  dialog.setModal(true);
  dialog.setMinimumWidth(560);
  dialog.setStyleSheet(R"(
      QDialog {
          background-color: #f8f9fa;
          border: 2px solid #2d5a1b;
          border-radius: 12px;
      }
      QLabel#titleLabel {
          color: #2d5a1b;
          font-size: 17px;
          font-weight: 700;
      }
      QLabel#messageLabel {
          color: #1f2a1f;
          font-size: 13px;
      }
      QLabel#errorLabel {
          color: #c62828;
          font-size: 12px;
          font-weight: 600;
      }
      QLineEdit {
          border: 1px solid #b9c9bb;
          border-radius: 7px;
          padding: 10px 12px;
          font-size: 13px;
          background: white;
      }
      QLineEdit:focus {
          border: 2px solid #2d5a1b;
      }
      QPushButton#btnValidate {
          background-color: #2d5a1b;
          color: white;
          border: none;
          border-radius: 7px;
          padding: 10px 18px;
          font-weight: 700;
      }
      QPushButton#btnValidate:hover {
          background-color: #214213;
      }
      QPushButton#btnCancel {
          background-color: #f0a500;
          color: #1f1f1f;
          border: none;
          border-radius: 7px;
          padding: 10px 18px;
          font-weight: 700;
      }
      QPushButton#btnCancel:hover {
          background-color: #d89200;
      }
  )");

  QVBoxLayout *layout = new QVBoxLayout(&dialog);
  layout->setContentsMargins(20, 20, 20, 16);
  layout->setSpacing(10);

  QLabel *title = new QLabel("Nom Machine obligatoire", &dialog);
  title->setObjectName("titleLabel");
  layout->addWidget(title);

  QLabel *message = new QLabel(
      "Le champ Nom Machine est obligatoire. Veuillez saisir un nom pour "
      "continuer.",
      &dialog);
  message->setObjectName("messageLabel");
  message->setWordWrap(true);
  layout->addWidget(message);

  QLineEdit *nameInput = new QLineEdit(&dialog);
  nameInput->setPlaceholderText("Ex: Presse-01");
  layout->addWidget(nameInput);

  QLabel *errorLabel = new QLabel(&dialog);
  errorLabel->setObjectName("errorLabel");
  errorLabel->setVisible(false);
  errorLabel->setWordWrap(true);
  layout->addWidget(errorLabel);

  QHBoxLayout *buttonsLayout = new QHBoxLayout();
  buttonsLayout->addStretch();

  QPushButton *btnValidate = new QPushButton("Valider", &dialog);
  btnValidate->setObjectName("btnValidate");
  QPushButton *btnCancel = new QPushButton("Annuler", &dialog);
  btnCancel->setObjectName("btnCancel");

  buttonsLayout->addWidget(btnValidate);
  buttonsLayout->addWidget(btnCancel);
  layout->addLayout(buttonsLayout);

  connect(btnCancel, &QPushButton::clicked, &dialog, [this, &dialog]() {
    on_btnReinitialiser_machine_clicked();
    dialog.reject();
  });

  connect(btnValidate, &QPushButton::clicked, &dialog,
          [this, &dialog, baseData, nameInput, errorLabel]() {
            const QString enteredName = nameInput->text().trimmed();
            if (enteredName.isEmpty()) {
              errorLabel->setText("Veuillez saisir un nom avant de valider.");
              errorLabel->setVisible(true);
              return;
            }

            MachineFormData updatedData = baseData;
            updatedData.nom = enteredName;

            if (isMachineNameAvailable(enteredName)) {
              if (saveMachineWithName(enteredName, updatedData)) {
                dialog.accept();
              }
              return;
            }

            dialog.accept();
            showDuplicateNameDialog(updatedData, enteredName);
          });

  nameInput->setFocus();
  dialog.exec();
}

void machine::showDuplicateNameDialog(const MachineFormData &baseData,
                                      const QString &problematicName) {
  QDialog dialog(this);
  dialog.setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
  dialog.setModal(true);
  dialog.setMinimumWidth(620);
  dialog.setStyleSheet(R"(
      QDialog {
          background-color: #f8f9fa;
          border: 2px solid #2d5a1b;
          border-radius: 12px;
      }
      QLabel#titleLabel {
          color: #2d5a1b;
          font-size: 17px;
          font-weight: 700;
      }
      QLabel#messageLabel {
          color: #1f2a1f;
          font-size: 13px;
      }
      QLabel#errorLabel {
          color: #c62828;
          font-size: 12px;
          font-weight: 600;
      }
      QLineEdit {
          border: 1px solid #b9c9bb;
          border-radius: 7px;
          padding: 10px 12px;
          font-size: 13px;
          background: white;
      }
      QLineEdit:focus {
          border: 2px solid #2d5a1b;
      }
      QPushButton#btnSuggestion {
          background: #e9f2e7;
          color: #2d5a1b;
          border: 1px solid #2d5a1b;
          border-radius: 6px;
          padding: 8px 12px;
          font-weight: 600;
      }
      QPushButton#btnSuggestion:hover {
          background: #d9e9d5;
      }
      QPushButton#btnValidate {
          background-color: #2d5a1b;
          color: white;
          border: none;
          border-radius: 7px;
          padding: 10px 18px;
          font-weight: 700;
      }
      QPushButton#btnValidate:hover {
          background-color: #214213;
      }
      QPushButton#btnCancel {
          background-color: #f0a500;
          color: #1f1f1f;
          border: none;
          border-radius: 7px;
          padding: 10px 18px;
          font-weight: 700;
      }
      QPushButton#btnCancel:hover {
          background-color: #d89200;
      }
  )");

  QVBoxLayout *layout = new QVBoxLayout(&dialog);
  layout->setContentsMargins(20, 20, 20, 16);
  layout->setSpacing(10);

  QLabel *title = new QLabel("Nom Machine déjà utilisé", &dialog);
  title->setObjectName("titleLabel");
  layout->addWidget(title);

  QLabel *message = new QLabel(
      QString("Le nom '%1' existe déjà. Veuillez choisir un autre nom.")
          .arg(problematicName),
      &dialog);
  message->setObjectName("messageLabel");
  message->setWordWrap(true);
  layout->addWidget(message);

  QLabel *suggestionsLabel =
      new QLabel("Suggestions disponibles :", &dialog);
  suggestionsLabel->setObjectName("messageLabel");
  layout->addWidget(suggestionsLabel);

  QWidget *suggestionsWidget = new QWidget(&dialog);
  QGridLayout *suggestionsLayout = new QGridLayout(suggestionsWidget);
  suggestionsLayout->setContentsMargins(0, 0, 0, 0);
  suggestionsLayout->setHorizontalSpacing(8);
  suggestionsLayout->setVerticalSpacing(8);
  layout->addWidget(suggestionsWidget);

  QLineEdit *nameInput = new QLineEdit(&dialog);
  nameInput->setText(problematicName);
  layout->addWidget(nameInput);

  QLabel *errorLabel = new QLabel(&dialog);
  errorLabel->setObjectName("errorLabel");
  errorLabel->setVisible(false);
  errorLabel->setWordWrap(true);
  layout->addWidget(errorLabel);

  QStringList suggestions = buildAvailableNameSuggestions(problematicName, 8);
  if (suggestions.isEmpty()) {
    QLabel *noSuggestion =
        new QLabel("Aucune suggestion libre trouvée automatiquement.", &dialog);
    noSuggestion->setObjectName("messageLabel");
    suggestionsLayout->addWidget(noSuggestion, 0, 0, 1, 2);
  } else {
    for (int i = 0; i < suggestions.size(); ++i) {
      QPushButton *btn = new QPushButton(suggestions.at(i), &dialog);
      btn->setObjectName("btnSuggestion");
      const QString candidate = suggestions.at(i);
      connect(btn, &QPushButton::clicked, &dialog,
              [nameInput, candidate]() {
                nameInput->setText(candidate);
              });
      suggestionsLayout->addWidget(btn, i / 2, i % 2);
    }
  }

  QHBoxLayout *buttonsLayout = new QHBoxLayout();
  buttonsLayout->addStretch();

  QPushButton *btnValidate = new QPushButton("Valider", &dialog);
  btnValidate->setObjectName("btnValidate");
  QPushButton *btnCancel = new QPushButton("Annuler", &dialog);
  btnCancel->setObjectName("btnCancel");

  buttonsLayout->addWidget(btnValidate);
  buttonsLayout->addWidget(btnCancel);
  layout->addLayout(buttonsLayout);

  connect(btnCancel, &QPushButton::clicked, &dialog, &QDialog::reject);

  connect(btnValidate, &QPushButton::clicked, &dialog,
          [this, &dialog, baseData, nameInput, errorLabel]() {
            const QString candidateName = nameInput->text().trimmed();
            if (candidateName.isEmpty()) {
              errorLabel->setText("Veuillez saisir un nom avant de valider.");
              errorLabel->setVisible(true);
              return;
            }

            if (!isMachineNameAvailable(candidateName)) {
              errorLabel->setText(
                  "Ce nom est toujours utilisé. Choisissez une autre "
                  "suggestion ou modifiez le nom.");
              errorLabel->setVisible(true);
              return;
            }

            MachineFormData updatedData = baseData;
            updatedData.nom = candidateName;
            if (saveMachineWithName(candidateName, updatedData)) {
              dialog.accept();
            }
          });

  nameInput->setFocus();
  dialog.exec();
}

void machine::on_pushButton_enregistrer_machine_clicked() {
  MachineFormData formData = collectMachineFormData();

  if (formData.nom.isEmpty()) {
    showRequiredNameDialog(formData);
    return;
  }

  if (!isMachineNameAvailable(formData.nom)) {
    showDuplicateNameDialog(formData, formData.nom);
    return;
  }

  saveMachineWithName(formData.nom, formData);
}

void machine::on_btnReinitialiser_machine_clicked() {
  // 1. Réinitialiser le nom de la machine (QLineEdit)
  ui->lineEdit_nom_machine->clear();

  // 2. Remettre le type de machine au premier index "Presse" (QComboBox)
  ui->comboBox_type_machine->setCurrentIndex(0);

  // 3. Définir l'état de marche sur "OFF" (QComboBox)
  ui->comboBox_etat_marche_machine->setCurrentText("OFF");

  // 4. Réinitialiser la température à 0.00 (QDoubleSpinBox)
  ui->doubleSpinBox_temperature_machine->setValue(0.00);

  // 5. Réinitialiser le niveau de charge à 0.00 (QDoubleSpinBox)
  ui->doubleSpinBox_niveau_charge_machine->setValue(0.00);

  // 6. Remettre l'état de fonctionnement sur "Normal" (QComboBox)
  ui->comboBox_etat_fonctionnement_machine->setCurrentText("Normal");

  // 7. Définir le type d'alerte sur "Aucune" (QComboBox)
  ui->comboBox_type_alerte_machine->setCurrentText("Aucune");

  // 8. Remettre le niveau de criticité sur "Faible" (QComboBox)
  ui->comboBox_niveau_criticite_machine->setCurrentText("Faible");

  // 9. Mettre la date de dernière maintenance à la date du jour (QDateEdit)
  ui->dateEdit_derniere_maintenance_machine->setDate(QDate::currentDate());

  // 10. Réinitialiser le score de santé à 100 (QSpinBox)
  ui->spinBox_score_sante_machine->setValue(100);

  // 11. Déverrouiller la sélection employé (si un responsable avait été choisi)
  clearEmployeeSelection();
}

void machine::chargerMachines() {
  m_allMachines.clear();

  // 1. Préparer et exécuter la requête SQL pour récupérer les données dans
  // l'ordre exact de la base de données
  const QString employeeColumn = machineEmployeeColumnName();
  const bool hasEmployeeColumn = !employeeColumn.isEmpty();
  QSqlQuery query(ConnectionMachine::getInstance().getDatabase());
  if (hasEmployeeColumn) {
    query.prepare(
        "SELECT M.ID_MACHINE, M.NOM_MACHINE, M.TYPE_MACHINE, M.ETAT_MARCHE, "
        "M.TEMPERATURE_ACTUELLE, M.NIVEAU_CHARGE, M.ETAT_FONCTIONNEMENT, "
        "M.TYPE_ALERTE, M.NIVEAU_CRITICITE, M.DATE_DERNIERE_MAINTENANCE, "
        "M.DATE_INSTALLATION, M.SCORE_SANTE, M.DATE_MISE_A_JOUR, "
        "NVL(E.USERNAME, '-') AS RESPONSABLE, "
        "NVL(M.REFROIDISSEMENT, '-') AS REFROIDISSEMENT, "
        "NVL(M.TAG, '-') AS TAG, "
        "NVL(M.PRIORITE, '-') AS PRIORITE "
        "FROM MACHINE M LEFT JOIN EMPLOYEES E ON M." + employeeColumn + " = E.USER_ID "
        "ORDER BY M.ID_MACHINE ASC");
  } else {
    query.prepare("SELECT ID_MACHINE, NOM_MACHINE, TYPE_MACHINE, ETAT_MARCHE, "
                  "TEMPERATURE_ACTUELLE, NIVEAU_CHARGE, ETAT_FONCTIONNEMENT, "
                  "TYPE_ALERTE, NIVEAU_CRITICITE, DATE_DERNIERE_MAINTENANCE, "
                  "DATE_INSTALLATION, SCORE_SANTE, DATE_MISE_A_JOUR, '-' AS RESPONSABLE, "
                  "NVL(REFROIDISSEMENT, '-') AS REFROIDISSEMENT, "
                  "NVL(TAG, '-') AS TAG, "
                  "NVL(PRIORITE, '-') AS PRIORITE "
                  "FROM MACHINE ORDER BY ID_MACHINE ASC");
  }

  if (!query.exec()) {
    qDebug() << "Erreur lors du chargement des machines :"
             << query.lastError().text();
    return;
  }

  // 2. Parcourir les résultats et remplir notre QList en mémoire
  while (query.next()) {
    MachineData m;
    m.id = query.value(0).toString();
    m.nom = query.value(1).toString();
    m.type = query.value(2).toString();
    m.etatMarche = query.value(3).toString();
    m.temperature = query.value(4).toDouble();
    m.charge = query.value(5).toDouble();
    m.fonctionnement = query.value(6).toString();
    m.alerte = query.value(7).toString();
    m.criticite = query.value(8).toString();
    m.maintenance = query.value(9).toDate();
    m.installation = query.value(10).toDate();
    m.scoreSante = query.value(11).toInt();
    m.miseAJour = query.value(12).toDate();
    m.responsable = query.value(13).toString();
    m.refroidissement = query.value(14).toString();
    m.tag = query.value(15).toString();
    m.priorite = query.value(16).toString();

    m_allMachines.append(m);
  }

  refreshResponsableFilterOptions();

  // 3. Appliquer le filtrage initial avec les valeurs par défaut (tout afficher)
  appliquerFiltres();
}

void machine::refreshResponsableFilterOptions() {
  if (!ui->filtre_responsable) {
    return;
  }

  const QString previous = ui->filtre_responsable->currentText();

  QSet<QString> responsablesSet;
  for (const MachineData &m : m_allMachines) {
    const QString responsable = m.responsable.trimmed();
    if (!responsable.isEmpty() && responsable != "-") {
      responsablesSet.insert(responsable);
    }
  }

  QStringList responsables = responsablesSet.values();
  responsables.sort(Qt::CaseInsensitive);

  ui->filtre_responsable->blockSignals(true);
  ui->filtre_responsable->clear();
  ui->filtre_responsable->addItem("-- Responsable --");
  ui->filtre_responsable->addItems(responsables);

  const int previousIndex = ui->filtre_responsable->findText(previous);
  if (previousIndex >= 0) {
    ui->filtre_responsable->setCurrentIndex(previousIndex);
  } else {
    ui->filtre_responsable->setCurrentIndex(0);
  }
  ui->filtre_responsable->blockSignals(false);
}

void machine::appliquerFiltres() {
  // 1. Vider le tableau
  machineTableModel->setRowCount(0);

  // 2. Lire les valeurs des filtres de l'interface
  QString fType = ui->filtre_type_machine->currentText();
  QString fEtat = ui->filtre_etat_marche_machine->currentText();
  QString fAlerte = ui->filtre_type_alerte->currentText();
  QString fCriticite = ui->filtre_alerte_criticite->currentText();
  QString fResponsable = ui->filtre_responsable->currentText();
  
  QDate dateDu = ui->date_debut->date();
  QDate dateAu = ui->date_fin->date();
  QString triPar = ui->tri_par->currentText();

  // 3. Filtrer la liste des machines avec une logique ET (AND)
  QList<MachineData> filteredMachines;
  for (const MachineData &m : m_allMachines) {
      if (fType != "-- Type machine --" && m.type != fType) continue;
      if (fEtat != "-- État marche --" && m.etatMarche != fEtat) continue;
      if (fAlerte != "-- Type alerte --" && m.alerte != fAlerte) continue;
      if (fCriticite != "-- Criticité --" && m.criticite != fCriticite) continue;
      if (fResponsable != "-- Responsable --" && m.responsable != fResponsable) continue;
      
      // Filtre sur la date de mise à jour (inclusivement entre dateDu et dateAu)
      // Ne filtrer que si la date est valide
      if (m.miseAJour.isValid() && (m.miseAJour < dateDu || m.miseAJour > dateAu)) continue;

      filteredMachines.append(m);
  }

  // 4. Trier la liste filtrée
  if (triPar == "nom_machine") {
      std::sort(filteredMachines.begin(), filteredMachines.end(), [](const MachineData &a, const MachineData &b){ return a.nom < b.nom; });
  } else if (triPar == "type_machine") {
      std::sort(filteredMachines.begin(), filteredMachines.end(), [](const MachineData &a, const MachineData &b){ return a.type < b.type; });
  } else if (triPar == "etat_fonctionnement") {
      std::sort(filteredMachines.begin(), filteredMachines.end(), [](const MachineData &a, const MachineData &b){ return a.fonctionnement < b.fonctionnement; });
  } else if (triPar == "temperature_actuelle") {
      std::sort(filteredMachines.begin(), filteredMachines.end(), [](const MachineData &a, const MachineData &b){ return a.temperature < b.temperature; });
  } else if (triPar == "date_derniere_maintenance") {
      std::sort(filteredMachines.begin(), filteredMachines.end(), [](const MachineData &a, const MachineData &b){ return a.maintenance < b.maintenance; });
  } else if (triPar == "niveau_criticite") {
      std::sort(filteredMachines.begin(), filteredMachines.end(), [](const MachineData &a, const MachineData &b){ return a.criticite < b.criticite; });
  }

  // Polices communes
  QFont boldFont;
  boldFont.setBold(true);

  // 5. Parcourir et afficher les machines filtrées et triées
  for (const MachineData &m : filteredMachines) {
    QList<QStandardItem *> row;

    QString tempStr = QString::number(m.temperature, 'f', 2);
    QString chargeStr = QString::number(m.charge, 'f', 2);
    QString scoreStr = QString::number(m.scoreSante);

    QStringList values = {
        m.id, m.nom, m.type, m.etatMarche, 
        tempStr, chargeStr, m.fonctionnement, 
        m.alerte, m.criticite, 
        m.maintenance.toString("yyyy-MM-dd"),
        m.installation.toString("yyyy-MM-dd"), 
        scoreStr, 
      m.miseAJour.toString("yyyy-MM-dd"),
      m.responsable,
      m.refroidissement,
      m.tag,
      m.priorite
    };

    // Créer les items pour chaque cellule de la ligne
    for (int i = 0; i < values.size(); ++i) {
      auto *item = new QStandardItem(values[i]);
      item->setEditable(false);
      item->setTextAlignment(Qt::AlignCenter);
      item->setForeground(QColor("#333333")); // Force sombre
      row.append(item);
    }

    // -------------------------------------------------------------------------
    // Désactivation conditionnelle : OFF ou VEILLE → griser les colonnes
    // métier (temp, charge, fonctionnement, alerte, criticité, score santé)
    // Colonnes concernées : 4, 5, 6, 7, 8, 11
    // -------------------------------------------------------------------------
    const bool machineInactive = (m.etatMarche == "OFF" || m.etatMarche == "VEILLE");
    const QList<int> colsADesactiver = {4, 5, 6, 7, 8, 11};

    if (machineInactive) {
      const QColor bgGrise("#E8E8E8");     // fond gris clair
      const QColor fgGrise("#AAAAAA");     // texte gris
      const QColor fgBoldGrise("#999999"); // texte gras grisé

      for (int col : colsADesactiver) {
        // Retirer Qt::ItemIsEnabled pour le grisage natif Qt
        row[col]->setFlags(row[col]->flags() & ~Qt::ItemIsEnabled);
        row[col]->setBackground(bgGrise);
        row[col]->setForeground(fgBoldGrise);
      }
    }

    // Coloration conditionnelle (seulement si la machine est ON)

    // -- État Marche (Colonne 3) — toujours coloré pour indiquer l'état
    row[3]->setFont(boldFont);
    if (m.etatMarche == "ON")
      row[3]->setForeground(QColor("#2E7D32")); // Vert
    else if (m.etatMarche == "VEILLE")
      row[3]->setForeground(QColor("#E65100")); // Orange
    else if (m.etatMarche == "OFF")
      row[3]->setForeground(QColor("#C62828")); // Rouge

    if (!machineInactive) {
      // -- Etat Fonctionnement (Colonne 6)
      row[6]->setFont(boldFont);
      if (m.fonctionnement == "Normal")
        row[6]->setForeground(QColor("#2E7D32")); // Vert
      else if (m.fonctionnement == "Alerte")
        row[6]->setForeground(QColor("#E65100")); // Orange
      else if (m.fonctionnement == "Panne")
        row[6]->setForeground(QColor("#C62828")); // Rouge

      // -- Type Alerte (Colonne 7)
      row[7]->setFont(boldFont);
      if (m.alerte == "Aucune")
        row[7]->setForeground(QColor("#2E7D32")); // Vert
      else if (m.alerte == "Panne")
        row[7]->setForeground(QColor("#E65100")); // Orange
      else if (m.alerte == "Surcharge")
        row[7]->setForeground(QColor("#C62828")); // Rouge

      // -- Niveau Criticité (Colonne 8)
      row[8]->setFont(boldFont);
      if (m.criticite == "Faible")
        row[8]->setForeground(QColor("#2E7D32")); // Vert
      else if (m.criticite == QString::fromUtf8("Élevé"))
        row[8]->setForeground(QColor("#E65100")); // Orange
      else if (m.criticite == "Critique")
        row[8]->setForeground(QColor("#C62828")); // Rouge
    }

    machineTableModel->appendRow(row);
  }
}

void machine::on_btnFiltrer_clicked() {
    appliquerFiltres();
}

void machine::clearStatsChartArea(const QString &message) {
  QLayout *layout = ui->zoneGraphique_machine->layout();
  if (!layout) {
    auto *newLayout = new QVBoxLayout(ui->zoneGraphique_machine);
    newLayout->setContentsMargins(16, 16, 16, 16);
    newLayout->setSpacing(10);
    layout = newLayout;
  }

  while (QLayoutItem *item = layout->takeAt(0)) {
    if (item->widget()) {
      item->widget()->deleteLater();
    }
    delete item;
  }

  QLabel *placeholder = new QLabel(message, ui->zoneGraphique_machine);
  placeholder->setAlignment(Qt::AlignCenter);
  placeholder->setWordWrap(true);
  placeholder->setStyleSheet(
      "QLabel { color: #4A4A4A; font-size: 13px; font-weight: 600; "
      "background-color: #F8FBFA; border: 1px dashed #9BB8AA; border-radius: 10px; "
      "padding: 22px; }");
  layout->addWidget(placeholder);
}

QList<QPair<QString, double>>
machine::loadStatisticsData(const QString &statsType) const {
  QList<QPair<QString, double>> data;
  QSqlQuery query(ConnectionMachine::getInstance().getDatabase());

  if (statsType == "État fonctionnement") {
    query.prepare("SELECT NVL(TRIM(ETAT_FONCTIONNEMENT), 'Non défini') AS LIBELLE, "
                  "COUNT(*) AS VALEUR "
                  "FROM MACHINE "
                  "GROUP BY NVL(TRIM(ETAT_FONCTIONNEMENT), 'Non défini') "
                  "ORDER BY VALEUR DESC");
  } else if (statsType == "Types alertes") {
    query.prepare("SELECT NVL(TRIM(TYPE_ALERTE), 'Aucune') AS LIBELLE, "
                  "COUNT(*) AS VALEUR "
                  "FROM MACHINE "
                  "GROUP BY NVL(TRIM(TYPE_ALERTE), 'Aucune') "
                  "ORDER BY VALEUR DESC");
  } else if (statsType == "Température moyenne") {
    query.prepare("SELECT NVL(TRIM(TYPE_MACHINE), 'Non défini') AS LIBELLE, "
                  "ROUND(AVG(NVL(TEMPERATURE_ACTUELLE, 0)), 2) AS VALEUR "
                  "FROM MACHINE "
                  "GROUP BY NVL(TRIM(TYPE_MACHINE), 'Non défini') "
                  "ORDER BY VALEUR DESC");
  } else if (statsType == "Type machine") {
    query.prepare("SELECT NVL(TRIM(TYPE_MACHINE), 'Non défini') AS LIBELLE, "
                  "COUNT(*) AS VALEUR "
                  "FROM MACHINE "
                  "GROUP BY NVL(TRIM(TYPE_MACHINE), 'Non défini') "
                  "ORDER BY VALEUR DESC");
  } else {
    return data;
  }

  if (!query.exec()) {
    qDebug() << "Erreur statistiques :" << query.lastError().text();
    return data;
  }

  while (query.next()) {
    const QString label = query.value(0).toString().trimmed();
    const double value = query.value(1).toDouble();
    data.append(qMakePair(label.isEmpty() ? QString("Non défini") : label, value));
  }

  return data;
}

void machine::renderStatisticsChart(const QList<QPair<QString, double>> &data,
                                    const QString &statsType,
                                    const QString &chartType) {
  if (data.isEmpty()) {
    clearStatsChartArea("Aucune donnée disponible pour cette statistique.");
    return;
  }

  QLayout *layout = ui->zoneGraphique_machine->layout();
  if (!layout) {
    auto *newLayout = new QVBoxLayout(ui->zoneGraphique_machine);
    newLayout->setContentsMargins(12, 12, 12, 12);
    newLayout->setSpacing(8);
    layout = newLayout;
  }

  while (QLayoutItem *item = layout->takeAt(0)) {
    if (item->widget()) {
      item->widget()->deleteLater();
    }
    delete item;
  }

  auto *chart = new QChart();
  chart->setAnimationOptions(QChart::SeriesAnimations);
  chart->setBackgroundBrush(QColor("#FFFFFF"));

  const bool isTemperatureStats = (statsType == "Température moyenne");

  const QMap<QString, QColor> etatPalette = {
      {"Normal", QColor("#2E7D32")},
      {"Alerte", QColor("#E65100")},
      {"Panne", QColor("#C62828")},
      {"Non défini", QColor("#607D8B")},
  };
  const QMap<QString, QColor> alertePalette = {
      {"Aucune", QColor("#2E7D32")},
      {"Température", QColor("#F57C00")},
      {"Maintenance", QColor("#FB8C00")},
      {"Sécurité", QColor("#C62828")},
      {"Surcharge", QColor("#D84315")},
      {"Panne", QColor("#B71C1C")},
      {"Non défini", QColor("#607D8B")},
  };
  const QMap<QString, QColor> typeMachinePalette = {
      {"Presse", QColor("#1565C0")},
      {"Malaxeur", QColor("#00897B")},
      {"Broyeur", QColor("#6A1B9A")},
      {"Chauffeur", QColor("#EF6C00")},
      {"Autre", QColor("#546E7A")},
      {"Non défini", QColor("#607D8B")},
  };

  QColor themeColor("#1A3C2F");
  if (statsType == "État fonctionnement") {
    themeColor = QColor("#2E7D32");
  } else if (statsType == "Types alertes") {
    themeColor = QColor("#E65100");
  } else if (statsType == "Température moyenne") {
    themeColor = QColor("#0277BD");
  } else if (statsType == "Type machine") {
    themeColor = QColor("#1565C0");
  }

  chart->setTitle(QString("Statistique: %1").arg(statsType));
  chart->setTitleBrush(QBrush(themeColor));
  chart->legend()->setVisible(true);
  chart->legend()->setAlignment(Qt::AlignBottom);
  chart->legend()->setLabelColor(QColor("#2B2B2B"));

  const QList<QColor> fallbackPalette = {
      QColor("#2E7D32"), QColor("#1565C0"), QColor("#6A1B9A"), QColor("#EF6C00"),
      QColor("#00897B"), QColor("#C62828"), QColor("#5D4037"), QColor("#455A64")};

  auto colorForLabel = [&](const QString &label, int index) -> QColor {
    const QString key = label.trimmed();
    if (statsType == "État fonctionnement" && etatPalette.contains(key)) {
      return etatPalette.value(key);
    }
    if (statsType == "Types alertes" && alertePalette.contains(key)) {
      return alertePalette.value(key);
    }
    if ((statsType == "Type machine" || statsType == "Température moyenne") &&
        typeMachinePalette.contains(key)) {
      return typeMachinePalette.value(key);
    }
    return fallbackPalette[index % fallbackPalette.size()];
  };

  double maxValue = 0.0;
  for (const auto &entry : data) {
    if (entry.second > maxValue) {
      maxValue = entry.second;
    }
  }

  if (chartType == "PieChart") {
    auto *series = new QPieSeries();
    for (int i = 0; i < data.size(); ++i) {
      const auto &entry = data[i];
      QPieSlice *slice = series->append(entry.first, entry.second);
      const QString suffix = isTemperatureStats ? " °C" : "";
      slice->setLabel(QString("%1: %2%3").arg(entry.first).arg(entry.second, 0, 'f', isTemperatureStats ? 2 : 0).arg(suffix));
      slice->setLabelVisible(true);
      slice->setBrush(colorForLabel(entry.first, i));
      slice->setPen(QPen(QColor("#FFFFFF"), 1));
    }
    chart->addSeries(series);
  } else if (chartType == "LineChart") {
    auto *series = new QLineSeries();
    series->setName(statsType);
    series->setColor(themeColor);
    series->setPointsVisible(true);
    series->setPointLabelsVisible(true);
    series->setPointLabelsFormat(isTemperatureStats ? "@yPoint °C" : "@yPoint");

    auto *axisX = new QCategoryAxis();
    axisX->setLabelsPosition(QCategoryAxis::AxisLabelsPositionOnValue);
    axisX->setLabelsAngle(-35);
    axisX->setLabelsColor(QColor("#2B2B2B"));

    for (int i = 0; i < data.size(); ++i) {
      series->append(i, data[i].second);
      axisX->append(data[i].first, i);
    }

    chart->addSeries(series);
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);
    axisX->setRange(0, data.size() > 1 ? data.size() - 1 : 1);

    auto *axisY = new QValueAxis();
    axisY->setTitleText(isTemperatureStats ? "Température moyenne (°C)"
                                           : "Nombre de machines");
    axisY->setRange(0, maxValue > 0 ? maxValue * 1.2 : 10);
    axisY->setLabelFormat(isTemperatureStats ? "%.2f" : "%.0f");
    axisY->setLabelsColor(QColor("#2B2B2B"));
    axisY->setGridLineColor(QColor("#DCE7E2"));
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);
  } else {
    auto *series = new QBarSeries();
    QStringList categories;

    for (int i = 0; i < data.size(); ++i) {
      const auto &entry = data[i];
      auto *set = new QBarSet(entry.first);
      *set << entry.second;
      set->setColor(colorForLabel(entry.first, i));
      set->setLabelColor(QColor("#1F1F1F"));
      series->append(set);
      categories << entry.first;
    }

    series->setLabelsVisible(true);
    series->setLabelsPosition(QAbstractBarSeries::LabelsOutsideEnd);
    series->setLabelsFormat(isTemperatureStats ? "@value °C" : "@value");
    chart->addSeries(series);

    auto *axisX = new QBarCategoryAxis();
    axisX->append(categories);
    axisX->setLabelsAngle(-30);
    axisX->setLabelsColor(QColor("#2B2B2B"));
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    auto *axisY = new QValueAxis();
    axisY->setTitleText(isTemperatureStats ? "Température moyenne (°C)"
                                           : "Nombre de machines");
    axisY->setRange(0, maxValue > 0 ? maxValue * 1.2 : 10);
    axisY->setLabelFormat(isTemperatureStats ? "%.2f" : "%.0f");
    axisY->setLabelsColor(QColor("#2B2B2B"));
    axisY->setGridLineColor(QColor("#DCE7E2"));
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);
  }

  auto *chartView = new QChartView(chart, ui->zoneGraphique_machine);
  chartView->setRenderHint(QPainter::Antialiasing);
  chartView->setStyleSheet(
      "QChartView { background-color: #FFFFFF; border: 1px solid #D7E5DF; "
      "border-radius: 8px; padding: 6px; }");

  QLabel *legendHelp = new QLabel(
      QString("Affichage %1 • %2").arg(chartType, statsType),
      ui->zoneGraphique_machine);
  legendHelp->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
  legendHelp->setStyleSheet("QLabel { color: #47695D; font-size: 11px; font-weight: 600; }");

  layout->addWidget(chartView);
  layout->addWidget(legendHelp);
}

void machine::on_btnGenererStats_machine_clicked() {
  const QString statsType = ui->type_statistique_machine->currentText();
  const QString chartType = ui->type_graphique_machine->currentText();

  const QList<QPair<QString, double>> data = loadStatisticsData(statsType);
  if (data.isEmpty()) {
    clearStatsChartArea("Aucune donnée trouvée pour ce type de statistique.");
    return;
  }

  renderStatisticsChart(data, statsType, chartType);
}

void machine::on_tableMachines_machine_doubleClicked(const QModelIndex &index) {
  if (!index.isValid())
    return;

  m_selectedRow = index.row();
  m_selectedMachineId = machineTableModel->item(m_selectedRow, 0)->text();

  // Highlight the row and enable buttons
  ui->tableMachines_machine->selectRow(m_selectedRow);
  ui->btnModifierMachine_machine->setEnabled(true);
  ui->btnSupprimerMachine_machine->setEnabled(true);
  ui->btnExporter_machine->setEnabled(true);
  ui->btnToggleOnOff_machine->setEnabled(true);
  ui->btnCarteMachine->setEnabled(true);

  qDebug() << "Machine sélectionnée:" << m_selectedMachineId << "à la ligne"
           << m_selectedRow;
}

void machine::on_btnSupprimerMachine_machine_clicked() {
  if (m_selectedMachineId.isEmpty())
    return;

  // Demander confirmation
  QMessageBox::StandardButton reply;
  reply = QMessageBox::question(
      this, "Confirmation de suppression",
      QString("Êtes-vous sûr de vouloir supprimer la machine %1 ?")
          .arg(m_selectedMachineId),
      QMessageBox::Yes | QMessageBox::No);

  if (reply == QMessageBox::Yes) {
    QSqlQuery query(ConnectionMachine::getInstance().getDatabase());
    query.prepare("DELETE FROM MACHINE WHERE ID_MACHINE = :id");
    query.bindValue(":id", m_selectedMachineId);

    if (query.exec()) {
      QMessageBox::information(this, "Succès",
                               "Machine supprimée avec succès.");
      // Rafraîchir le tableau
      chargerMachines();
      // Désactiver les boutons
      ui->btnModifierMachine_machine->setEnabled(false);
      ui->btnSupprimerMachine_machine->setEnabled(false);
      ui->btnExporter_machine->setEnabled(false);
      ui->btnToggleOnOff_machine->setEnabled(false);
      ui->btnCarteMachine->setEnabled(false);
      m_selectedMachineId = "";
      m_selectedRow = -1;
    } else {
      QMessageBox::critical(this, "Erreur",
                            "Erreur lors de la suppression de la machine : " +
                                query.lastError().text());
    }
  }
}

void machine::on_btnModifierMachine_machine_clicked() {
  if (m_selectedMachineId.isEmpty() || m_selectedRow < 0)
    return;

  // 1. Créer la boîte de dialogue modale
  QDialog dialog(this);
  dialog.setWindowTitle("Modifier la machine — " +
                        machineTableModel->item(m_selectedRow, 1)->text());
  dialog.setMinimumWidth(450);
  dialog.setModal(true);

  // Appliquer un style premium
  dialog.setStyleSheet(R"(
        QDialog { background-color: #f8f9fa; border: 1px solid #1A3C2F; border-radius: 8px; }
        QLabel { font-weight: bold; color: #1B4D3E; font-size: 13px; }
        QLineEdit, QComboBox, QSpinBox, QDateEdit { 
            padding: 8px; border: 1px solid #ced4da; border-radius: 4px; background: white; 
        }
        QLineEdit:focus, QComboBox:focus, QSpinBox:focus, QDateEdit:focus { 
            border-color: #2D5F47; 
        }
        QPushButton { 
            padding: 10px 20px; font-weight: bold; border-radius: 6px; font-size: 13px; 
        }
    )");

  QFormLayout *formLayout = new QFormLayout(&dialog);
  formLayout->setSpacing(15);
  formLayout->setContentsMargins(20, 20, 20, 20);

  // 2. Créer et pré-remplir les champs
  // Index des colonnes : 0:ID, 1:Nom, 2:Type, 3:Etat, 4:Temp, 5:Charge,
  // 6:Fonct, 7:Alerte, 8:Crit, 9:Maint, 10:Inst, 11:Score

  QLineEdit *txtNom =
      new QLineEdit(machineTableModel->item(m_selectedRow, 1)->text());

  QComboBox *cmbType = new QComboBox();
  cmbType->addItems({"Presse", "Malaxeur", "Broyeur", "Chauffeur", "Autre"});
  cmbType->setCurrentText(machineTableModel->item(m_selectedRow, 2)->text());

  QComboBox *cmbEtat = new QComboBox();
  cmbEtat->addItems({"ON", "OFF", "VEILLE"});
  cmbEtat->setCurrentText(machineTableModel->item(m_selectedRow, 3)->text());

  QSpinBox *spinTemp = new QSpinBox();
  spinTemp->setRange(-50, 500);
  spinTemp->setSuffix(" °C");
  spinTemp->setValue(machineTableModel->item(m_selectedRow, 4)
                         ->text()
                         .replace(" °C", "")
                         .toInt());

  QSpinBox *spinCharge = new QSpinBox();
  spinCharge->setRange(0, 100);
  spinCharge->setSuffix(" %");
  spinCharge->setValue(machineTableModel->item(m_selectedRow, 5)
                           ->text()
                           .replace(" %", "")
                           .toInt());

  QComboBox *cmbFonct = new QComboBox();
  cmbFonct->addItems({"Normal", "Alerte", "Panne"});
  cmbFonct->setCurrentText(machineTableModel->item(m_selectedRow, 6)->text());

  QComboBox *cmbAlerte = new QComboBox();
  cmbAlerte->addItems({"Aucune", "Température", "Maintenance", "Sécurité"});
  cmbAlerte->setCurrentText(machineTableModel->item(m_selectedRow, 7)->text());

  QComboBox *cmbCrit = new QComboBox();
  cmbCrit->addItems({"Faible", "Moyen", QString::fromUtf8("\xC3\x89lev\xC3\xa9"), "Critique"});
  cmbCrit->setCurrentText(machineTableModel->item(m_selectedRow, 8)->text());

  QDateEdit *dateMaint = new QDateEdit(QDate::fromString(
      machineTableModel->item(m_selectedRow, 9)->text(), "yyyy-MM-dd"));
  dateMaint->setCalendarPopup(true);

  QDateEdit *dateInst = new QDateEdit(QDate::fromString(
      machineTableModel->item(m_selectedRow, 10)->text(), "yyyy-MM-dd"));
  dateInst->setCalendarPopup(true);

  QSpinBox *spinScore = new QSpinBox();
  spinScore->setRange(0, 100);
  spinScore->setValue(
      machineTableModel->item(m_selectedRow, 11)->text().toInt());

  // Ajouter au formulaire
  formLayout->addRow("Nom de la machine:", txtNom);
  formLayout->addRow("Type:", cmbType);
  formLayout->addRow("État de marche:", cmbEtat);
  formLayout->addRow("Température:", spinTemp);
  formLayout->addRow("Charge:", spinCharge);
  formLayout->addRow(QString::fromUtf8("\xC3\x89tat de fonctionnement:"), cmbFonct);
  formLayout->addRow("Alerte:", cmbAlerte);
  formLayout->addRow("Criticité:", cmbCrit);
  formLayout->addRow("Dernière Maintenance:", dateMaint);
  formLayout->addRow("Date Installation:", dateInst);
  formLayout->addRow("Score Santé:", spinScore);

  // 3. Boutons d'action
  QHBoxLayout *btnLayout = new QHBoxLayout();
  QPushButton *btnSave = new QPushButton("Enregistrer");
  btnSave->setStyleSheet("background-color: #2D5F47; color: white;");
  QPushButton *btnCancel = new QPushButton("Annuler");
  btnCancel->setStyleSheet("background-color: #6c757d; color: white;");

  btnLayout->addWidget(btnSave);
  btnLayout->addWidget(btnCancel);
  formLayout->addRow(btnLayout);

  connect(btnCancel, &QPushButton::clicked, &dialog, &QDialog::reject);
  connect(btnSave, &QPushButton::clicked, [&]() {
    // Exécuter l'UPDATE dans la base de données
    QSqlQuery query(ConnectionMachine::getInstance().getDatabase());
    query.prepare("UPDATE MACHINE SET "
                  "NOM_MACHINE = :nom, "
                  "TYPE_MACHINE = :type, "
                  "ETAT_MARCHE = :etat, "
                  "TEMPERATURE_ACTUELLE = :temp, "
                  "NIVEAU_CHARGE = :charge, "
                  "ETAT_FONCTIONNEMENT = :fonct, "
                  "TYPE_ALERTE = :alerte, "
                  "NIVEAU_CRITICITE = :crit, "
                  "DATE_DERNIERE_MAINTENANCE = TO_DATE(:maint, 'YYYY-MM-DD'), "
                  "DATE_INSTALLATION = TO_DATE(:inst, 'YYYY-MM-DD'), "
                  "SCORE_SANTE = :score, "
                  "DATE_MISE_A_JOUR = SYSTIMESTAMP "
                  "WHERE ID_MACHINE = :id");

    query.bindValue(":nom", txtNom->text());
    query.bindValue(":type", cmbType->currentText());
    query.bindValue(":etat", cmbEtat->currentText());
    query.bindValue(":temp", spinTemp->value());
    query.bindValue(":charge", spinCharge->value());
    query.bindValue(":fonct", cmbFonct->currentText());
    query.bindValue(":alerte", cmbAlerte->currentText());
    query.bindValue(":crit", cmbCrit->currentText());
    query.bindValue(":maint", dateMaint->date().toString("yyyy-MM-dd"));
    query.bindValue(":inst", dateInst->date().toString("yyyy-MM-dd"));
    query.bindValue(":score", spinScore->value());
    query.bindValue(":id", m_selectedMachineId);

    if (query.exec()) {
      QMessageBox::information(this, "Succès",
                               "La machine a été modifiée avec succès.");
      chargerMachines(); // Rafraîchir le tableau
      dialog.accept();
    } else {
      QMessageBox::critical(this, "Erreur",
                            "Erreur lors de la modification : " +
                                query.lastError().text());
    }
  });

  dialog.exec();
}

// ============================================================================
// Search Implementation
// ============================================================================

void machine::rechercherMachines() {
  // Ensure table view is visible
  ui->scrollHistoriqueOnOff_machine->setVisible(false);
  ui->scrollListeMachines->setVisible(false);
  ui->tableMachines_machine->setVisible(true);
  ui->btnHistoriqueToggle_machine->setChecked(false);

  QString searchText = ui->recherche_nom_machine->text().trimmed();

  // If search is empty, reload all machines
  if (searchText.isEmpty()) {
    chargerMachines();
    return;
  }

  // Clear table
  machineTableModel->setRowCount(0);

  // Query with case-insensitive LIKE on NOM_MACHINE
  const QString employeeColumn = machineEmployeeColumnName();
  const bool hasEmployeeColumn = !employeeColumn.isEmpty();
  QSqlQuery query(ConnectionMachine::getInstance().getDatabase());
  if (hasEmployeeColumn) {
    query.prepare(
        "SELECT M.ID_MACHINE, M.NOM_MACHINE, M.TYPE_MACHINE, M.ETAT_MARCHE, "
        "M.TEMPERATURE_ACTUELLE, M.NIVEAU_CHARGE, M.ETAT_FONCTIONNEMENT, "
        "M.TYPE_ALERTE, M.NIVEAU_CRITICITE, M.DATE_DERNIERE_MAINTENANCE, "
        "M.DATE_INSTALLATION, M.SCORE_SANTE, M.DATE_MISE_A_JOUR, "
        "NVL(E.USERNAME, '-') AS RESPONSABLE, "
        "NVL(M.REFROIDISSEMENT, '-') AS REFROIDISSEMENT, "
        "NVL(M.TAG, '-') AS TAG, "
        "NVL(M.PRIORITE, '-') AS PRIORITE "
        "FROM MACHINE M LEFT JOIN EMPLOYEES E ON M." + employeeColumn + " = E.USER_ID "
        "WHERE UPPER(M.NOM_MACHINE) LIKE UPPER(:search) "
        "ORDER BY M.ID_MACHINE ASC");
  } else {
    query.prepare("SELECT ID_MACHINE, NOM_MACHINE, TYPE_MACHINE, ETAT_MARCHE, "
                  "TEMPERATURE_ACTUELLE, NIVEAU_CHARGE, ETAT_FONCTIONNEMENT, "
                  "TYPE_ALERTE, NIVEAU_CRITICITE, DATE_DERNIERE_MAINTENANCE, "
                  "DATE_INSTALLATION, SCORE_SANTE, DATE_MISE_A_JOUR, '-' AS RESPONSABLE, "
                  "NVL(REFROIDISSEMENT, '-') AS REFROIDISSEMENT, "
                  "NVL(TAG, '-') AS TAG, "
                  "NVL(PRIORITE, '-') AS PRIORITE "
                  "FROM MACHINE WHERE UPPER(NOM_MACHINE) LIKE UPPER(:search) "
                  "ORDER BY ID_MACHINE ASC");
  }
  query.bindValue(":search", "%" + searchText + "%");

  if (!query.exec()) {
    qDebug() << "Erreur recherche machines :" << query.lastError().text();
    return;
  }

  QFont boldFont;
  boldFont.setBold(true);

  while (query.next()) {
    QList<QStandardItem *> row;

    QString id = query.value(0).toString();
    QString nom = query.value(1).toString();
    QString type = query.value(2).toString();
    QString etatMarche = query.value(3).toString();
    QString temp = query.value(4).toString();
    QString charge = query.value(5).toString();
    QString fonction = query.value(6).toString();
    QString alerte = query.value(7).toString();
    QString criticite = query.value(8).toString();
    QString maintenance = query.value(9).toDate().toString("yyyy-MM-dd");
    QString install = query.value(10).toDate().toString("yyyy-MM-dd");
    QString scoreSante = query.value(11).toString();
    QString maj = query.value(12).toDate().toString("yyyy-MM-dd");
    QString responsable = query.value(13).toString();
    QString refroidissement = query.value(14).toString();
    QString tag = query.value(15).toString();
    QString priorite = query.value(16).toString();

    QStringList values = {id,      nom,        type,   etatMarche, temp,
                          charge,  fonction,   alerte, criticite,  maintenance,
          install, scoreSante, maj, responsable, refroidissement, tag, priorite};

    for (int i = 0; i < values.size(); ++i) {
      auto *item = new QStandardItem(values[i]);
      item->setEditable(false);
      item->setTextAlignment(Qt::AlignCenter);
      item->setForeground(QColor("#333333"));
      row.append(item);
    }

    // Coloration conditionnelle
    row[3]->setFont(boldFont);
    if (etatMarche == "ON")
      row[3]->setForeground(QColor("#2E7D32"));
    else if (etatMarche == "VEILLE")
      row[3]->setForeground(QColor("#E65100"));
    else if (etatMarche == "OFF")
      row[3]->setForeground(QColor("#C62828"));

    row[6]->setFont(boldFont);
    if (fonction == "Normal")
      row[6]->setForeground(QColor("#2E7D32"));
    else if (fonction == "Alerte")
      row[6]->setForeground(QColor("#E65100"));
    else if (fonction == "Panne")
      row[6]->setForeground(QColor("#C62828"));

    row[7]->setFont(boldFont);
    if (alerte == "Aucune")
      row[7]->setForeground(QColor("#2E7D32"));
    else if (alerte == "Panne")
      row[7]->setForeground(QColor("#E65100"));
    else if (alerte == "Surcharge")
      row[7]->setForeground(QColor("#C62828"));

    row[8]->setFont(boldFont);
    if (criticite == "Faible")
      row[8]->setForeground(QColor("#2E7D32"));
    else if (criticite == "Élevé")
      row[8]->setForeground(QColor("#E65100"));
    else if (criticite == "Critique")
      row[8]->setForeground(QColor("#C62828"));

    machineTableModel->appendRow(row);
  }

  // Reset selection state
  m_selectedMachineId = "";
  m_selectedRow = -1;
  ui->btnModifierMachine_machine->setEnabled(false);
  ui->btnSupprimerMachine_machine->setEnabled(false);
  ui->btnExporter_machine->setEnabled(false);
  ui->btnToggleOnOff_machine->setEnabled(false);
  ui->btnCarteMachine->setEnabled(false);
}

bool machine::eventFilter(QObject *obj, QEvent *event) {
  if (event->type() == QEvent::KeyPress) {
    QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
    if (keyEvent->key() == Qt::Key_Escape) {
      // Escape from search field: clear search and reload
      if (obj == ui->recherche_nom_machine) {
        ui->recherche_nom_machine->clear();
        chargerMachines();
      }

      // Always: deselect machine and disable buttons
      ui->tableMachines_machine->clearSelection();
      m_selectedMachineId = "";
      m_selectedRow = -1;
      ui->btnModifierMachine_machine->setEnabled(false);
      ui->btnSupprimerMachine_machine->setEnabled(false);
      ui->btnExporter_machine->setEnabled(false);
      ui->btnToggleOnOff_machine->setEnabled(false);
      ui->btnCarteMachine->setEnabled(false);
      return true;
    }
  }
  return QMainWindow::eventFilter(obj, event);
}

// ============================================================================
// ExportDialog Implementation
// ============================================================================

ExportDialog::ExportDialog(QWidget *parent)
    : QDialog(parent), selectedFormat("PDF") {
    setupUi();
}

void ExportDialog::setupUi() {
    setWindowTitle("Exporter la fiche machine");
    setFixedSize(350, 250);
    setStyleSheet("QDialog { background-color: #f8f9fa; }"
                  "QLabel { font-size: 14px; font-weight: bold; color: #1B4D3E; }"
                  "QRadioButton { font-size: 13px; color: #333; padding: 5px; }"
                  "QRadioButton::indicator { width: 16px; height: 16px; }"
                  "QRadioButton::indicator::unchecked { border: 2px solid #ccc; border-radius: 9px; }"
                  "QRadioButton::indicator::checked { border: 2px solid #1B4D3E; border-radius: 9px; background-color: #1B4D3E; }");

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);

    QLabel *lblTitle = new QLabel("Choisissez le format d'export :");
    mainLayout->addWidget(lblTitle);

    buttonGroup = new QButtonGroup(this);

    QStringList formats = {"PDF", "Word (.odt)", "Excel (.csv)", "Texte brut (.txt)"};
    QStringList values = {"PDF", "Word", "Excel", "TXT"};

    for (int i = 0; i < formats.size(); ++i) {
        QRadioButton *rb = new QRadioButton(formats[i]);
        if (i == 0) rb->setChecked(true); // default option
        buttonGroup->addButton(rb, i);
        mainLayout->addWidget(rb);
    }

    QHBoxLayout *btnLayout = new QHBoxLayout();
    QPushButton *btnCancel = new QPushButton("Annuler");
    QPushButton *btnExport = new QPushButton("Exporter");

    btnCancel->setStyleSheet("QPushButton { background-color: #ddd; color: #333; border: none; padding: 8px 15px; border-radius: 4px; font-weight: bold; }"
                             "QPushButton:hover { background-color: #ccc; }");
    btnExport->setStyleSheet("QPushButton { background-color: #1A3C2F; color: white; border: none; padding: 8px 15px; border-radius: 4px; font-weight: bold; }"
                             "QPushButton:hover { background-color: #0F241E; }");

    btnLayout->addStretch();
    btnLayout->addWidget(btnCancel);
    btnLayout->addWidget(btnExport);

    mainLayout->addSpacing(10);
    mainLayout->addLayout(btnLayout);

    connect(btnCancel, &QPushButton::clicked, this, &QDialog::reject);
    connect(btnExport, &QPushButton::clicked, this, &QDialog::accept);
}

QString ExportDialog::getSelectedFormat() const {
    int id = buttonGroup->checkedId();
    if (id == 0) return "PDF";
    if (id == 1) return "Word";
    if (id == 2) return "Excel";
    if (id == 3) return "TXT";
    return "PDF";
}

// ============================================================================
// Multi-Format Export Methods
// ============================================================================

void machine::afficherDialogExport() {
  if (m_selectedRow < 0) {
    QMessageBox::warning(this, "Aucune machine sélectionnée",
                         "Veuillez double-cliquer sur une machine dans le "
                         "tableau avant d'exporter.");
    return;
  }

  ExportDialog dialog(this);
  if (dialog.exec() != QDialog::Accepted) {
      return;
  }

  QString format = dialog.getSelectedFormat();

  if (format == "PDF") {
      exporterPDF();
      return;
  }

  QString machineId = machineTableModel->item(m_selectedRow, 0)->text();

  const QString employeeColumn = machineEmployeeColumnName();
  const bool hasEmployeeColumn = !employeeColumn.isEmpty();
  QSqlQuery query(ConnectionMachine::getInstance().getDatabase());
  if (hasEmployeeColumn) {
    query.prepare(
        "SELECT M.ID_MACHINE, M.NOM_MACHINE, M.TYPE_MACHINE, M.ETAT_MARCHE, "
        "M.TEMPERATURE_ACTUELLE, M.NIVEAU_CHARGE, M.ETAT_FONCTIONNEMENT, "
        "M.TYPE_ALERTE, M.NIVEAU_CRITICITE, M.DATE_DERNIERE_MAINTENANCE, "
        "M.DATE_INSTALLATION, M.SCORE_SANTE, M.DATE_MISE_A_JOUR, "
        "NVL(E.USERNAME, '-') AS RESPONSABLE, "
        "NVL(M.REFROIDISSEMENT, '-') AS REFROIDISSEMENT, "
        "NVL(M.TAG, '-') AS TAG, "
        "NVL(M.PRIORITE, '-') AS PRIORITE "
        "FROM MACHINE M LEFT JOIN EMPLOYEES E ON M." + employeeColumn + " = E.USER_ID "
        "WHERE M.ID_MACHINE = :id");
  } else {
    query.prepare(
        "SELECT ID_MACHINE, NOM_MACHINE, TYPE_MACHINE, ETAT_MARCHE, "
        "TEMPERATURE_ACTUELLE, NIVEAU_CHARGE, ETAT_FONCTIONNEMENT, "
        "TYPE_ALERTE, NIVEAU_CRITICITE, DATE_DERNIERE_MAINTENANCE, "
        "DATE_INSTALLATION, SCORE_SANTE, DATE_MISE_A_JOUR, "
        "'-' AS RESPONSABLE, "
        "NVL(REFROIDISSEMENT, '-') AS REFROIDISSEMENT, "
        "NVL(TAG, '-') AS TAG, "
        "NVL(PRIORITE, '-') AS PRIORITE "
        "FROM MACHINE WHERE ID_MACHINE = :id");
  }
  query.bindValue(":id", machineId);

  if (!query.exec() || !query.next()) {
    QMessageBox::critical(this, "Erreur",
                          "Impossible de récupérer les données de la machine.");
    return;
  }

  QString id = query.value(0).toString();
  QString nom = query.value(1).toString();
  QString type = query.value(2).toString();
  QString etatMarche = query.value(3).toString();
  QString temperature = query.value(4).toString();
  QString charge = query.value(5).toString();
  QString fonctionnement = query.value(6).toString();
  QString alerte = query.value(7).toString();
  QString criticite = query.value(8).toString();
  QString maintenance = query.value(9).toDate().toString("yyyy-MM-dd");
  QString installation = query.value(10).toDate().toString("yyyy-MM-dd");
  QString scoreSante = query.value(11).toString();
  QString miseAJour = query.value(12).toDate().toString("yyyy-MM-dd");
  QString responsable = query.value(13).toString();
  QString refroidissement = query.value(14).toString();
  QString tag = query.value(15).toString();
  QString priorite = query.value(16).toString();

  QDateTime now = QDateTime::currentDateTime();
  QString genDate = now.toString("dd/MM/yyyy") + QString::fromUtf8(" à ") + now.toString("HH:mm:ss");

  QString defaultName, filter, ext;
  if (format == "Word") {
      defaultName = QString("Fiche_Machine_%1_%2.odt").arg(id, nom);
      filter = "Word/ODF Document (*.odt)";
      ext = "odt";
  } else if (format == "Excel") {
      defaultName = QString("Fiche_Machine_%1_%2.csv").arg(id, nom);
      filter = "Excel CSV (*.csv)";
      ext = "csv";
  } else {
      defaultName = QString("Fiche_Machine_%1_%2.txt").arg(id, nom);
      filter = "Texte brut (*.txt)";
      ext = "txt";
  }

  QString filePath = QFileDialog::getSaveFileName(
      this, "Enregistrer la fiche machine",
      QDir::homePath() + "/Desktop/" + defaultName, filter);

  if (filePath.isEmpty()) return;

  bool success = false;

  if (format == "Word") {
      // Create HTML exactly like PDF to retain styles
      QString etatColor = (etatMarche == "ON") ? "#2E7D32" : "#C62828";
      QString criticiteColor;
      if (criticite == "Faible") criticiteColor = "#2E7D32";
      else if (criticite == QString::fromUtf8("Élevé")) criticiteColor = "#E65100";
      else criticiteColor = "#C62828";

      QString html =
          QString::fromUtf8(
              "<html>"
              "<head>"
              "<meta charset='UTF-8'>"
              "<style>"
              "  body { font-family: 'Segoe UI', Arial, sans-serif; color: #333; margin: 0; padding: 0; }"
              "  .header { background-color: #1B4D3E; border-left: 8px solid #C9A227; "
              "            padding: 18px 30px; border-radius: 6px; margin-bottom: 30px; }"
              "  .header h1 { color: white; font-size: 26px; font-weight: bold; margin: 0; text-align: center; }"
              "  .section { margin-bottom: 22px; margin-left: 10px; }"
              "  .section-title { color: #1B4D3E; font-size: 18px; font-weight: bold; "
              "                    margin-bottom: 8px; border-bottom: 2px solid #D5E8D4; padding-bottom: 6px; }"
              "  .field { font-size: 13px; line-height: 2.0; margin-left: 5px; }"
              "  .field-label { color: #555; }"
              "  .field-value { color: #222; font-weight: bold; }"
              "  .footer { margin-top: 30px; font-size: 11px; color: #777; margin-left: 10px; }"
              "  .footer-brand { color: #1B4D3E; font-size: 11px; margin-top: 4px; }"
              "</style>"
              "</head>"
              "<body>"
              "<div class='header'><h1>🏭  Fiche Machine</h1></div>"
              "<div class='section'>"
              "  <div class='section-title'>📋  Informations générales</div>"
              "  <div class='field'>🔢 <span class='field-label'>ID Machine</span>&nbsp;&nbsp;&nbsp;"
              "    <span class='field-value'>%1</span></div>"
              "  <div class='field'>📝 <span class='field-label'>Nom</span>&nbsp;&nbsp;&nbsp;"
              "    <span class='field-value'>%2</span></div>"
              "  <div class='field'>⚙️ <span class='field-label'>Type</span>&nbsp;&nbsp;&nbsp;"
              "    <span class='field-value'>%3</span></div>"
              "  <div class='field'>🔌 <span class='field-label'>État de marche</span>&nbsp;"
              "    <span style='color:%4; font-weight:bold;'>%5</span></div>"
              "</div>"
              "<div class='section'>"
              "  <div class='section-title'>📊  Paramètres techniques</div>"
              "  <div class='field'>🌡 <span class='field-label'>Température (°C)</span>&nbsp;"
              "    <span class='field-value'>%6</span></div>"
              "  <div class='field'>⚡ <span class='field-label'>Charge (%)</span>&nbsp;&nbsp;&nbsp;"
              "    <span class='field-value'>%7</span></div>"
              "  <div class='field'>🔧 <span class='field-label'>Fonctionnement</span>&nbsp;"
              "    <span class='field-value'>%8</span></div>"
              "  <div class='field'>⚠️ <span class='field-label'>Alerte</span>&nbsp;&nbsp;&nbsp;"
              "    <span class='field-value'>%9</span></div>"
              "  <div class='field'>🚨 <span class='field-label'>Criticité</span>&nbsp;&nbsp;&nbsp;"
              "    <span style='color:%10; font-weight:bold;'>%11</span></div>"
              "  <div class='field'>💯 <span class='field-label'>Score Santé</span>&nbsp;&nbsp;&nbsp;"
              "    <span class='field-value'>%12</span></div>"
              "</div>"
              "<div class='section'>"
              "  <div class='section-title'>📅  Dates</div>"
              "  <div class='field'>🔧 <span class='field-label'>Dernière maintenance</span>&nbsp;"
              "    <span class='field-value'>%13</span></div>"
              "  <div class='field'>📋 <span class='field-label'>Date d'installation</span>&nbsp;&nbsp;&nbsp;"
              "    <span class='field-value'>%14</span></div>"
              "  <div class='field'>📅 <span class='field-label'>Date mise à jour</span>&nbsp;&nbsp;&nbsp;"
              "    <span class='field-value'>%15</span></div>"
              "</div>"
                  "<div class='section'>"
                  "  <div class='section-title'>🧩  Suivi et métadonnées</div>"
                  "  <div class='field'>👤 <span class='field-label'>Responsable</span>&nbsp;"
                  "    <span class='field-value'>%16</span></div>"
                  "  <div class='field'>❄️ <span class='field-label'>Refroidissement</span>&nbsp;"
                  "    <span class='field-value'>%17</span></div>"
                  "  <div class='field'>🏷️ <span class='field-label'>Tag</span>&nbsp;"
                  "    <span class='field-value'>%18</span></div>"
                  "  <div class='field'>📌 <span class='field-label'>Priorité</span>&nbsp;"
                  "    <span class='field-value'>%19</span></div>"
                  "</div>"
              "<div class='footer'>"
                  "  <div>Document généré le %20</div>"
              "  <div class='footer-brand'>🏭  Système de Gestion des Machines — Zitouna</div>"
              "</div>"
              "</body></html>")
              .arg(id, nom, type, etatColor, etatMarche, temperature, charge, fonctionnement, alerte)
                  .arg(criticiteColor, criticite, scoreSante, maintenance, installation, miseAJour,
                    responsable, refroidissement, tag, priorite, genDate);

      QTextDocument doc;
      doc.setHtml(html);
      QTextDocumentWriter writer(filePath);
      writer.setFormat("odf");
      success = writer.write(&doc);

  } else if (format == "Excel") {
      QFile csvFile(filePath);
      if (csvFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
          QTextStream out(&csvFile);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
          out.setEncoding(QStringConverter::Utf8);
#else
          out.setCodec("UTF-8");
#endif
          out.setGenerateByteOrderMark(true); // Excel needs UTF-8 BOM to read accents
          
          out << "Attribut;Valeur\n";
          out << "ID Machine;" << id << "\n";
          out << "Nom;" << nom << "\n";
          out << "Type;" << type << "\n";
          out << QString::fromUtf8("État de marche;") << etatMarche << "\n";
          out << QString::fromUtf8("Température (°C);") << temperature << "\n";
          out << "Charge (%);" << charge << "\n";
          out << "Fonctionnement;" << fonctionnement << "\n";
          out << "Alerte;" << alerte << "\n";
          out << QString::fromUtf8("Criticité;") << criticite << "\n";
          out << QString::fromUtf8("Score Santé;") << scoreSante << "\n";
          out << QString::fromUtf8("Dernière maintenance;") << maintenance << "\n";
          out << "Date d'installation;" << installation << "\n";
          out << QString::fromUtf8("Date mise à jour;") << miseAJour << "\n";
          out << "Responsable;" << responsable << "\n";
          out << "Refroidissement;" << refroidissement << "\n";
          out << "Tag;" << tag << "\n";
          out << QString::fromUtf8("Priorité;") << priorite << "\n";
          out << QString::fromUtf8("Document généré le;") << genDate << "\n";
          csvFile.close();
          success = true;
      }
  } else if (format == "TXT") {
      QFile txtFile(filePath);
      if (txtFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
          QTextStream out(&txtFile);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
          out.setEncoding(QStringConverter::Utf8);
#else
          out.setCodec("UTF-8");
#endif
          int w = 30;
          out << "============================================================\n";
          out << QString("FICHE MACHINE: %1").arg(nom).rightJustified(40, ' ') << "\n";
          out << "============================================================\n\n";

          out << "--- INFORMATIONS GENERALES ---\n";
          out << QString("ID Machine:").leftJustified(w, ' ') << id << "\n";
          out << QString("Nom:").leftJustified(w, ' ') << nom << "\n";
          out << QString("Type:").leftJustified(w, ' ') << type << "\n";
          out << QString::fromUtf8("État de marche:").leftJustified(w, ' ') << etatMarche << "\n\n";

          out << "--- PARAMETRES TECHNIQUES ---\n";
          out << QString::fromUtf8("Température (°C):").leftJustified(w, ' ') << temperature << "\n";
          out << QString("Charge (%):").leftJustified(w, ' ') << charge << "\n";
          out << QString("Fonctionnement:").leftJustified(w, ' ') << fonctionnement << "\n";
          out << QString("Alerte:").leftJustified(w, ' ') << alerte << "\n";
          out << QString::fromUtf8("Criticité:").leftJustified(w, ' ') << criticite << "\n";
          out << QString::fromUtf8("Score Santé:").leftJustified(w, ' ') << scoreSante << "\n\n";

          out << "--- DATES ---\n";
          out << QString::fromUtf8("Dernière maintenance:").leftJustified(w, ' ') << maintenance << "\n";
          out << QString("Date d'installation:").leftJustified(w, ' ') << installation << "\n";
          out << QString::fromUtf8("Date mise à jour:").leftJustified(w, ' ') << miseAJour << "\n\n";

          out << "--- SUIVI ET META ---\n";
          out << QString("Responsable:").leftJustified(w, ' ') << responsable << "\n";
          out << QString("Refroidissement:").leftJustified(w, ' ') << refroidissement << "\n";
          out << QString("Tag:").leftJustified(w, ' ') << tag << "\n";
          out << QString::fromUtf8("Priorité:").leftJustified(w, ' ') << priorite << "\n\n";

          out << "============================================================\n";
          out << QString::fromUtf8("Document généré le ") << genDate << "\n";
          out << QString::fromUtf8("Système de Gestion des Machines — Zitouna\n");
          
          txtFile.close();
          success = true;
      }
  }

  if (success) {
      QMessageBox msgBox(this);
      msgBox.setWindowTitle(QString::fromUtf8("Succès"));
      msgBox.setText(QString::fromUtf8("✅ Fiche machine exportée avec succès en ") + format + " !");
      msgBox.setIcon(QMessageBox::Information);
      msgBox.setStyleSheet(R"(
        QMessageBox { background-color: #f8f9fa; border: 2px solid #1A3C2F; }
        QLabel { color: #1A3C2F; font-size: 14px; font-weight: bold; padding: 10px; }
        QPushButton { background-color: #1A3C2F; color: white; border: none;
          border-radius: 5px; padding: 8px 20px; font-weight: bold; min-width: 100px; }
        QPushButton:hover { background-color: #0F241E; }
      )");
      msgBox.exec();

      QDesktopServices::openUrl(QUrl::fromLocalFile(filePath));
  } else {
      QMessageBox::critical(this, "Erreur", "L'export a échoué. Vérifiez vos permissions de fichiers.");
  }
}

void machine::exporterPDF() {
  // 1. Check that a machine is selected
  if (m_selectedRow < 0) {
    QMessageBox::warning(this, "Aucune machine s\xC3\xa9lectionn\xC3\xa9e",
                         "Veuillez double-cliquer sur une machine dans le "
                         "tableau avant d'exporter.");
    return;
  }

  // 2. Get machine ID from the table model
  QString machineId = machineTableModel->item(m_selectedRow, 0)->text();

  // 3. Fetch all machine data from database
  const QString employeeColumn = machineEmployeeColumnName();
  const bool hasEmployeeColumn = !employeeColumn.isEmpty();
  QSqlQuery query(ConnectionMachine::getInstance().getDatabase());
  if (hasEmployeeColumn) {
    query.prepare(
        "SELECT M.ID_MACHINE, M.NOM_MACHINE, M.TYPE_MACHINE, M.ETAT_MARCHE, "
        "M.TEMPERATURE_ACTUELLE, M.NIVEAU_CHARGE, M.ETAT_FONCTIONNEMENT, "
        "M.TYPE_ALERTE, M.NIVEAU_CRITICITE, M.DATE_DERNIERE_MAINTENANCE, "
        "M.DATE_INSTALLATION, M.SCORE_SANTE, M.DATE_MISE_A_JOUR, "
        "NVL(E.USERNAME, '-') AS RESPONSABLE, "
        "NVL(M.REFROIDISSEMENT, '-') AS REFROIDISSEMENT, "
        "NVL(M.TAG, '-') AS TAG, "
        "NVL(M.PRIORITE, '-') AS PRIORITE "
        "FROM MACHINE M LEFT JOIN EMPLOYEES E ON M." + employeeColumn + " = E.USER_ID "
        "WHERE M.ID_MACHINE = :id");
  } else {
    query.prepare(
        "SELECT ID_MACHINE, NOM_MACHINE, TYPE_MACHINE, ETAT_MARCHE, "
        "TEMPERATURE_ACTUELLE, NIVEAU_CHARGE, ETAT_FONCTIONNEMENT, "
        "TYPE_ALERTE, NIVEAU_CRITICITE, DATE_DERNIERE_MAINTENANCE, "
        "DATE_INSTALLATION, SCORE_SANTE, DATE_MISE_A_JOUR, "
        "'-' AS RESPONSABLE, "
        "NVL(REFROIDISSEMENT, '-') AS REFROIDISSEMENT, "
        "NVL(TAG, '-') AS TAG, "
        "NVL(PRIORITE, '-') AS PRIORITE "
        "FROM MACHINE WHERE ID_MACHINE = :id");
  }
  query.bindValue(":id", machineId);

  if (!query.exec() || !query.next()) {
    QMessageBox::critical(this, "Erreur",
                          "Impossible de r\xC3\xa9cup\xC3\xa9rer les "
                          "donn\xC3\xa9es de la machine.");
    return;
  }

  // Extract all values
  QString id = query.value(0).toString();
  QString nom = query.value(1).toString();
  QString type = query.value(2).toString();
  QString etatMarche = query.value(3).toString();
  QString temperature = query.value(4).toString();
  QString charge = query.value(5).toString();
  QString fonctionnement = query.value(6).toString();
  QString alerte = query.value(7).toString();
  QString criticite = query.value(8).toString();
  QString maintenance = query.value(9).toDate().toString("yyyy-MM-dd");
  QString installation = query.value(10).toDate().toString("yyyy-MM-dd");
  QString scoreSante = query.value(11).toString();
  QString miseAJour = query.value(12).toDate().toString("yyyy-MM-dd");
  QString responsable = query.value(13).toString();
  QString refroidissement = query.value(14).toString();
  QString tag = query.value(15).toString();
  QString priorite = query.value(16).toString();

  // 4. Ask user where to save
  QString defaultName = QString("Fiche_Machine_%1_%2.pdf").arg(id, nom);
  QString filePath = QFileDialog::getSaveFileName(
      this, "Enregistrer la fiche machine",
      QDir::homePath() + "/Desktop/" + defaultName, "PDF (*.pdf)");

  if (filePath.isEmpty())
    return;

  // 5. Determine colors for dynamic values
  QString etatColor = (etatMarche == "ON") ? "#2E7D32" : "#C62828";
  QString criticiteColor;
  if (criticite == "Faible")
    criticiteColor = "#2E7D32";
  else if (criticite == QString::fromUtf8("\xC3\x89lev\xC3\xa9"))
    criticiteColor = "#E65100";
  else
    criticiteColor = "#C62828";

  // 6. Generate timestamp
  QDateTime now = QDateTime::currentDateTime();
  QString genDate = now.toString("dd/MM/yyyy") +
                    QString::fromUtf8(" \xC3\xA0 ") + now.toString("HH:mm:ss");

  // 7. Build HTML content matching the reference image exactly
  QString html =
      QString::fromUtf8(
          "<html>"
          "<head>"
          "<meta charset='UTF-8'>"
          "<style>"
          "  body { font-family: 'Segoe UI', Arial, sans-serif; color: #333; "
          "margin: 0; padding: 0; }"
          "  .header { background-color: #1B4D3E; border-left: 8px solid "
          "#C9A227; "
          "            padding: 18px 30px; border-radius: 6px; margin-bottom: "
          "30px; }"
          "  .header h1 { color: white; font-size: 26px; font-weight: bold; "
          "margin: 0; text-align: center; }"
          "  .section { margin-bottom: 22px; margin-left: 10px; }"
          "  .section-title { color: #1B4D3E; font-size: 18px; font-weight: "
          "bold; "
          "                    margin-bottom: 8px; border-bottom: 2px solid "
          "#D5E8D4; padding-bottom: 6px; }"
          "  .field { font-size: 13px; line-height: 2.0; margin-left: 5px; }"
          "  .field-label { color: #555; }"
          "  .field-value { color: #222; font-weight: bold; }"
          "  .footer { margin-top: 30px; font-size: 11px; color: #777; "
          "margin-left: 10px; }"
          "  .footer-brand { color: #1B4D3E; font-size: 11px; margin-top: 4px; "
          "}"
          "</style>"
          "</head>"
          "<body>"

          // ===== HEADER =====
          "<div class='header'>"
          "  <h1>\xF0\x9F\x8F\xAD  Fiche Machine</h1>"
          "</div>"

          // ===== INFORMATIONS GENERALES =====
          "<div class='section'>"
          "  <div class='section-title'>\xF0\x9F\x93\x8B  Informations "
          "g\xC3\xa9n\xC3\xa9rales</div>"
          "  <div class='field'>\xF0\x9F\x94\xA2 <span class='field-label'>ID "
          "Machine</span>&nbsp;&nbsp;&nbsp;"
          "    <span class='field-value'>%1</span></div>"
          "  <div class='field'>\xF0\x9F\x93\x9D <span "
          "class='field-label'>Nom</span>&nbsp;&nbsp;&nbsp;"
          "    <span class='field-value'>%2</span></div>"
          "  <div class='field'>\xE2\x9A\x99\xEF\xB8\x8F <span "
          "class='field-label'>Type</span>&nbsp;&nbsp;&nbsp;"
          "    <span class='field-value'>%3</span></div>"
          "  <div class='field'>\xF0\x9F\x94\x8C <span "
          "class='field-label'>\xC3\x89tat de marche</span>&nbsp;"
          "    <span style='color:%4; font-weight:bold;'>%5</span></div>"
          "</div>"

          // ===== PARAMETRES TECHNIQUES =====
          "<div class='section'>"
          "  <div class='section-title'>\xF0\x9F\x93\x8A  Param\xC3\xA8tres "
          "techniques</div>"
          "  <div class='field'>\xF0\x9F\x8C\xA1 <span "
          "class='field-label'>Temp\xC3\xa9rature (\xC2\xB0C)</span>&nbsp;"
          "    <span class='field-value'>%6</span></div>"
          "  <div class='field'>\xE2\x9A\xA1 <span class='field-label'>Charge "
          "(%%)</span>&nbsp;&nbsp;&nbsp;"
          "    <span class='field-value'>%7</span></div>"
          "  <div class='field'>\xF0\x9F\x94\xA7 <span "
          "class='field-label'>Fonctionnement</span>&nbsp;"
          "    <span class='field-value'>%8</span></div>"
          "  <div class='field'>\xE2\x9A\xA0\xEF\xB8\x8F <span "
          "class='field-label'>Alerte</span>&nbsp;&nbsp;&nbsp;"
          "    <span class='field-value'>%9</span></div>"
          "  <div class='field'>\xF0\x9F\x94\xB0 <span "
          "class='field-label'>Criticit\xC3\xa9</span>&nbsp;&nbsp;&nbsp;"
          "    <span style='color:%10; font-weight:bold;'>%11</span></div>"
          "  <div class='field'>\xF0\x9F\x92\xAF <span "
          "class='field-label'>Score Sant\xC3\xa9</span>&nbsp;&nbsp;&nbsp;"
          "    <span class='field-value'>%12</span></div>"
          "</div>"

          // ===== DATES =====
          "<div class='section'>"
          "  <div class='section-title'>\xF0\x9F\x93\x85  Dates</div>"
          "  <div class='field'>\xF0\x9F\x94\xA7 <span "
          "class='field-label'>Derni\xC3\xA8re maintenance</span>&nbsp;"
          "    <span class='field-value'>%13</span></div>"
          "  <div class='field'>\xF0\x9F\x93\x8B <span "
          "class='field-label'>Date d'installation</span>&nbsp;&nbsp;&nbsp;"
          "    <span class='field-value'>%14</span></div>"
          "  <div class='field'>\xF0\x9F\x93\x85 <span "
          "class='field-label'>Date mise \xC3\xA0 jour</span>&nbsp;&nbsp;&nbsp;"
          "    <span class='field-value'>%15</span></div>"
          "</div>"

          // ===== SUIVI ET METADONNEES =====
          "<div class='section'>"
          "  <div class='section-title'>\xF0\x9F\xA7\xA9  Suivi et "
          "m\xC3\xA9tadonn\xC3\xA9es</div>"
          "  <div class='field'>\xF0\x9F\x91\xA4 <span "
          "class='field-label'>Responsable</span>&nbsp;"
          "    <span class='field-value'>%16</span></div>"
          "  <div class='field'>\xE2\x9D\x84\xEF\xB8\x8F <span "
          "class='field-label'>Refroidissement</span>&nbsp;"
          "    <span class='field-value'>%17</span></div>"
          "  <div class='field'>\xF0\x9F\x8F\xB7\xEF\xB8\x8F <span "
          "class='field-label'>Tag</span>&nbsp;"
          "    <span class='field-value'>%18</span></div>"
          "  <div class='field'>\xF0\x9F\x93\x8C <span "
          "class='field-label'>Priorit\xC3\xA9</span>&nbsp;"
          "    <span class='field-value'>%19</span></div>"
          "</div>"

          // ===== FOOTER =====
          "<div class='footer'>"
          "  <div>Document g\xC3\xA9n\xC3\xA9r\xC3\xA9 le %20</div>"
          "  <div class='footer-brand'>\xF0\x9F\x8F\xAD  Syst\xC3\xA8me de "
          "Gestion des Machines \xe2\x80\x94 Zitouna</div>"
          "</div>"

          "</body></html>")
          .arg(id, nom, type, etatColor, etatMarche, temperature, charge,
               fonctionnement, alerte)
          .arg(criticiteColor, criticite, scoreSante, maintenance, installation, miseAJour,
            responsable, refroidissement, tag, priorite, genDate);

  // 8. Setup printer and render HTML to PDF
  QPrinter printer(QPrinter::HighResolution);
  printer.setOutputFormat(QPrinter::PdfFormat);
  printer.setOutputFileName(filePath);
  printer.setPageSize(QPageSize(QPageSize::A4));
  printer.setPageMargins(QMarginsF(15, 15, 15, 15), QPageLayout::Millimeter);

  QTextDocument doc;
  doc.setHtml(html);
  doc.setPageSize(QSizeF(printer.pageRect(QPrinter::Point).size()));
  doc.print(&printer);

  // 9. Open the generated PDF
  QDesktopServices::openUrl(QUrl::fromLocalFile(filePath));

  QMessageBox msgBox(this);
  msgBox.setWindowTitle(QString::fromUtf8("Succ\xC3\xA8s"));
  msgBox.setText(QString::fromUtf8(
      "\xE2\x9C\x85 Fiche machine export\xC3\xa9e avec succ\xC3\xA8s !"));
  msgBox.setIcon(QMessageBox::Information);
  msgBox.setStyleSheet(R"(
    QMessageBox { background-color: #f8f9fa; border: 2px solid #1A3C2F; }
    QLabel { color: #1A3C2F; font-size: 14px; font-weight: bold; padding: 10px; }
    QPushButton { background-color: #1A3C2F; color: white; border: none;
      border-radius: 5px; padding: 8px 20px; font-weight: bold; min-width: 100px; }
    QPushButton:hover { background-color: #0F241E; }
  )");
  msgBox.exec();
}

// ============================================================================
// Historique Implementation (Live Timer)
// ============================================================================

void machine::chargerHistorique() {
  // Title
  ui->lblHistoriqueTitle->setText(
      QString::fromUtf8("\xF0\x9F\x93\x9C Historique des machines"));
  ui->btnFermerHistorique->setText(QString::fromUtf8("\xE2\x9C\x95"));

  // Create or reset model
  if (!historiqueTableModel) {
    historiqueTableModel = new QStandardItemModel(0, 4, this);
  }
  historiqueTableModel->setRowCount(0);
  historiqueTableModel->setHorizontalHeaderLabels(
      {QString::fromUtf8("\xF0\x9F\x8F\xAD Nom Machine"),
       QString::fromUtf8("\xE2\x8F\xB1 Temps ON"),
       QString::fromUtf8("\xE2\x8F\xB1 Temps OFF"),
       QString::fromUtf8("\xE2\x8F\xB1 Temps Veille")});

  // Configure table view
  ui->tableHistorique_machine->setModel(historiqueTableModel);
  ui->tableHistorique_machine->verticalHeader()->setVisible(false);
  ui->tableHistorique_machine->horizontalHeader()->setStretchLastSection(true);
  ui->tableHistorique_machine->horizontalHeader()->setSectionResizeMode(
      QHeaderView::Stretch);
  ui->tableHistorique_machine->setEditTriggers(
      QAbstractItemView::NoEditTriggers);
  ui->tableHistorique_machine->verticalHeader()->setDefaultSectionSize(45);
  ui->tableHistorique_machine->setShowGrid(true);

  // Query: get seconds in current state for each machine
  QSqlQuery query(ConnectionMachine::getInstance().getDatabase());
  query.prepare(
      "SELECT NOM_MACHINE, ETAT_MARCHE, "
      "EXTRACT(DAY FROM (SYSTIMESTAMP - DATE_MISE_A_JOUR)) * 86400 + "
      "EXTRACT(HOUR FROM (SYSTIMESTAMP - DATE_MISE_A_JOUR)) * 3600 + "
      "EXTRACT(MINUTE FROM (SYSTIMESTAMP - DATE_MISE_A_JOUR)) * 60 + "
      "EXTRACT(SECOND FROM (SYSTIMESTAMP - DATE_MISE_A_JOUR)) AS SECONDS_ETAT "
      "FROM MACHINE "
      "ORDER BY NOM_MACHINE");

  if (!query.exec()) {
    qDebug() << "Erreur chargement historique :" << query.lastError().text();
    return;
  }

  // Store entries in memory for live update
  m_historiqueEntries.clear();

  QFont boldFont;
  boldFont.setBold(true);
  boldFont.setFamily("Segoe UI");

  // Helper to format seconds as "Xh Ym Zs"
  auto formatDuration = [](int totalSeconds) -> QString {
    if (totalSeconds <= 0)
      return "0h 00m 00s";
    int hours = totalSeconds / 3600;
    int minutes = (totalSeconds % 3600) / 60;
    int seconds = totalSeconds % 60;
    return QString("%1h %2m %3s")
        .arg(hours)
        .arg(minutes, 2, 10, QChar('0'))
        .arg(seconds, 2, 10, QChar('0'));
  };

  while (query.next()) {
    HistoriqueEntry entry;
    entry.machineName = query.value(0).toString();
    entry.currentState = query.value(1).toString().toUpper();
    int secondsEtat = query.value(2).toInt();

    entry.secondsOn = 0;
    entry.secondsOff = 0;
    entry.secondsVeille = 0;

    if (entry.currentState == "ON")
      entry.secondsOn = secondsEtat;
    else if (entry.currentState == "OFF")
      entry.secondsOff = secondsEtat;
    else
      entry.secondsVeille = secondsEtat;

    m_historiqueEntries.append(entry);

    // Build row
    QList<QStandardItem *> row;

    auto *itemNom =
        new QStandardItem(QString::fromUtf8("\xF0\x9F\x8F\xAD ") + entry.machineName);
    itemNom->setEditable(false);
    itemNom->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    itemNom->setFont(boldFont);
    itemNom->setForeground(QColor("#1B4D3E"));
    row.append(itemNom);

    auto *itemOn = new QStandardItem(formatDuration(entry.secondsOn));
    itemOn->setEditable(false);
    itemOn->setTextAlignment(Qt::AlignCenter);
    itemOn->setFont(boldFont);
    itemOn->setForeground(QColor("#155724"));
    itemOn->setBackground(QColor("#d4edda"));
    row.append(itemOn);

    auto *itemOff = new QStandardItem(formatDuration(entry.secondsOff));
    itemOff->setEditable(false);
    itemOff->setTextAlignment(Qt::AlignCenter);
    itemOff->setFont(boldFont);
    itemOff->setForeground(QColor("#721c24"));
    itemOff->setBackground(QColor("#f8d7da"));
    row.append(itemOff);

    auto *itemVeille = new QStandardItem(formatDuration(entry.secondsVeille));
    itemVeille->setEditable(false);
    itemVeille->setTextAlignment(Qt::AlignCenter);
    itemVeille->setFont(boldFont);
    itemVeille->setForeground(QColor("#856404"));
    itemVeille->setBackground(QColor("#fff3cd"));
    row.append(itemVeille);

    historiqueTableModel->appendRow(row);
  }

  // Start 1-second live timer
  if (!historiqueTimer) {
    historiqueTimer = new QTimer(this);
    connect(historiqueTimer, &QTimer::timeout, this,
            &machine::updateHistoriqueDisplay);
  }
  historiqueTimer->start(1000);
}

void machine::updateHistoriqueDisplay() {
  // If historique view is not visible, stop timer
  if (!ui->scrollHistoriqueOnOff_machine->isVisible()) {
    if (historiqueTimer)
      historiqueTimer->stop();
    return;
  }

  auto formatDuration = [](int totalSeconds) -> QString {
    if (totalSeconds <= 0)
      return "0h 00m 00s";
    int hours = totalSeconds / 3600;
    int minutes = (totalSeconds % 3600) / 60;
    int seconds = totalSeconds % 60;
    return QString("%1h %2m %3s")
        .arg(hours)
        .arg(minutes, 2, 10, QChar('0'))
        .arg(seconds, 2, 10, QChar('0'));
  };

  for (int i = 0; i < m_historiqueEntries.size(); ++i) {
    HistoriqueEntry &e = m_historiqueEntries[i];

    // Increment the active state's counter
    if (e.currentState == "ON")
      e.secondsOn++;
    else if (e.currentState == "OFF")
      e.secondsOff++;
    else
      e.secondsVeille++;

    // Update table cells (columns 1, 2, 3)
    if (i < historiqueTableModel->rowCount()) {
      historiqueTableModel->item(i, 1)->setText(formatDuration(e.secondsOn));
      historiqueTableModel->item(i, 2)->setText(formatDuration(e.secondsOff));
      historiqueTableModel->item(i, 3)->setText(formatDuration(e.secondsVeille));
    }
  }
}

// ============================================================================
// Machine Card (Fiche Machine) Implementation
// ============================================================================

void machine::showMachineCard() {
  if (m_selectedMachineId.isEmpty() || m_selectedRow < 0)
    return;

  const QString employeeColumn = machineEmployeeColumnName();
  const bool hasEmployeeColumn = !employeeColumn.isEmpty();
  const QString fromClause = hasEmployeeColumn
                                 ? ("FROM MACHINE M LEFT JOIN EMPLOYEES E ON M." +
                                    employeeColumn +
                                    " = E.USER_ID WHERE M.ID_MACHINE = :id")
                                 : "FROM MACHINE M WHERE M.ID_MACHINE = :id";
    const QString responsableExpr =
      hasEmployeeColumn ? "NVL(E.USERNAME, '-')" : "'-'";

  // Query all machine info from Oracle
  QSqlQuery query(ConnectionMachine::getInstance().getDatabase());
    const QString queryWithGarantie =
      "SELECT M.NOM_MACHINE, M.TYPE_MACHINE, M.ETAT_MARCHE, M.TEMPERATURE_ACTUELLE, "
      "M.NIVEAU_CHARGE, M.ETAT_FONCTIONNEMENT, M.TYPE_ALERTE, M.NIVEAU_CRITICITE, "
      "M.DATE_DERNIERE_MAINTENANCE, M.DATE_INSTALLATION, M.SCORE_SANTE, "
      "NVL(M.REFROIDISSEMENT, '-') AS REFROIDISSEMENT, "
      "NVL(M.TAG, '-') AS TAG, "
      "NVL(M.PRIORITE, '-') AS PRIORITE, " +
      responsableExpr + " AS RESPONSABLE, "
      "M.GARANTIE_EXPIRATION " + fromClause;
    query.prepare(queryWithGarantie);
  query.bindValue(":id", m_selectedMachineId);

  // Try with GARANTIE_EXPIRATION, if it fails try without
  bool hasGarantie = true;
  if (!query.exec() || !query.next()) {
    // Retry without GARANTIE_EXPIRATION column
    QSqlQuery query2(ConnectionMachine::getInstance().getDatabase());
    const QString queryWithoutGarantie =
      "SELECT M.NOM_MACHINE, M.TYPE_MACHINE, M.ETAT_MARCHE, M.TEMPERATURE_ACTUELLE, "
      "M.NIVEAU_CHARGE, M.ETAT_FONCTIONNEMENT, M.TYPE_ALERTE, M.NIVEAU_CRITICITE, "
      "M.DATE_DERNIERE_MAINTENANCE, M.DATE_INSTALLATION, M.SCORE_SANTE, "
      "NVL(M.REFROIDISSEMENT, '-') AS REFROIDISSEMENT, "
      "NVL(M.TAG, '-') AS TAG, "
      "NVL(M.PRIORITE, '-') AS PRIORITE, " +
      responsableExpr + " AS RESPONSABLE " + fromClause;
    query2.prepare(queryWithoutGarantie);
    query2.bindValue(":id", m_selectedMachineId);
    if (!query2.exec() || !query2.next()) {
      QMessageBox::critical(this, "Erreur",
                            QString::fromUtf8("Impossible de r\xC3\xa9cup\xC3\xa9rer "
                                              "les donn\xC3\xa9es de la machine."));
      return;
    }
    hasGarantie = false;
    query = query2;
  }

  // Extract values
  QString nom = query.value(0).toString();
  QString type = query.value(1).toString();
  QString etatMarche = query.value(2).toString();
  QString temperature = query.value(3).toString();
  QString charge = query.value(4).toString();
  QString fonctionnement = query.value(5).toString();
  QString alerte = query.value(6).toString();
  QString criticite = query.value(7).toString();
  QString maintenance = query.value(8).toDate().toString("yyyy-MM-dd");
  QString installation = query.value(9).toDate().toString("yyyy-MM-dd");
  QString scoreSante = query.value(10).toString();
  QString refroidissement = query.value(11).toString();
  QString tag = query.value(12).toString();
  QString priorite = query.value(13).toString();
  QString responsable = query.value(14).toString();
  QString garantie = hasGarantie ? query.value(15).toDate().toString("yyyy-MM-dd") : "N/A";

  // Determine border color based on ETAT_FONCTIONNEMENT
  QString borderColor;
  if (fonctionnement == "Normal")
    borderColor = "#4CAF50";
  else if (fonctionnement == "Alerte")
    borderColor = "#FF9800";
  else if (fonctionnement == "Panne")
    borderColor = "#F44336";
  else
    borderColor = "#4CAF50";

  // Create modal dialog
  machineCardDialog = new QDialog(this);
  machineCardDialog->setWindowTitle(
      QString::fromUtf8("\xF0\x9F\x97\xBA\xEF\xB8\x8F Fiche Machine — ") + nom);
  machineCardDialog->setMinimumWidth(500);
  machineCardDialog->setMinimumHeight(620);
  machineCardDialog->setModal(false); // Non-modal to allow background interaction
  machineCardDialog->setObjectName("machineCardDialog");

  // Lambda to build stylesheet with dynamic border color
  auto buildDialogStyle = [](const QString &bColor) -> QString {
    return QString(
        "QDialog#machineCardDialog { "
        "  background-color: #1A3C2F; "
        "  border: 4px solid %1; "
        "  border-radius: 12px; "
        "} "
        "QLabel { color: white; font-family: 'Segoe UI', Arial, sans-serif; border: none; } "
        "QLabel#cardTitle { font-size: 18px; font-weight: bold; color: white; } "
        "QLabel#cardFieldLabel { font-size: 12px; color: #A0C4B0; font-weight: 600; } "
        "QLabel#cardFieldValue { font-size: 13px; color: white; font-weight: bold; } "
        "QLabel#cardStatusBadge { "
        "  font-size: 13px; font-weight: bold; color: white; "
        "  background-color: %1; border-radius: 8px; padding: 4px 14px; "
        "} "
        "QPushButton#btnCloseCard { "
        "  background-color: rgba(255,255,255,0.15); color: white; "
        "  font-size: 12px; font-weight: bold; border: 1px solid rgba(255,255,255,0.3); "
        "  border-radius: 6px; padding: 8px 20px; min-width: 100px; "
        "} "
        "QPushButton#btnCloseCard:hover { background-color: rgba(255,255,255,0.25); } "
        ).arg(bColor);
  };

  machineCardDialog->setStyleSheet(buildDialogStyle(borderColor));

  QVBoxLayout *mainLay = new QVBoxLayout(machineCardDialog);
  mainLay->setSpacing(12);
  mainLay->setContentsMargins(24, 20, 24, 20);

  // Title
  QLabel *lblTitle = new QLabel(
      QString::fromUtf8("\xF0\x9F\x97\xBA\xEF\xB8\x8F Fiche Machine"));
  lblTitle->setObjectName("cardTitle");
  lblTitle->setAlignment(Qt::AlignCenter);
  mainLay->addWidget(lblTitle);

  // Status badge
  QLabel *lblStatus = new QLabel(fonctionnement);
  lblStatus->setObjectName("cardStatusBadge");
  lblStatus->setAlignment(Qt::AlignCenter);
  lblStatus->setMaximumWidth(160);
  QHBoxLayout *statusRow = new QHBoxLayout();
  statusRow->addStretch();
  statusRow->addWidget(lblStatus);
  statusRow->addStretch();
  mainLay->addLayout(statusRow);

  // Separator
  QFrame *sep = new QFrame();
  sep->setFrameShape(QFrame::HLine);
  sep->setStyleSheet("background-color: rgba(255,255,255,0.2); max-height: 1px;");
  mainLay->addWidget(sep);

  // Grid of fields
  QGridLayout *grid = new QGridLayout();
  grid->setHorizontalSpacing(20);
  grid->setVerticalSpacing(8);

  auto addField = [&](int row, int col, const QString &label, const QString &value) {
    QLabel *lbl = new QLabel(label);
    lbl->setObjectName("cardFieldLabel");
    QLabel *val = new QLabel(value);
    val->setObjectName("cardFieldValue");
    grid->addWidget(lbl, row, col * 2);
    grid->addWidget(val, row, col * 2 + 1);
  };

  addField(0, 0, "Nom:", nom);
  addField(0, 1, "Type:", type);
  addField(1, 0, QString::fromUtf8("\xC3\x89tat marche:"), etatMarche);
  addField(1, 1, QString::fromUtf8("Temp\xC3\xa9rature:"), temperature + QString::fromUtf8(" \xC2\xB0C"));
  addField(2, 0, "Charge:", charge + " %");
  addField(2, 1, "Fonctionnement:", fonctionnement);
  addField(3, 0, "Alerte:", alerte);
  addField(3, 1, QString::fromUtf8("Criticit\xC3\xa9:"), criticite);
  addField(4, 0, "Score Santé:", scoreSante);
  addField(4, 1, "Maintenance:", maintenance);
  addField(5, 0, "Installation:", installation);
  addField(5, 1, "Garantie:", garantie);
  addField(6, 0, "Responsable:", responsable);
  addField(6, 1, "Refroidissement:", refroidissement);
  addField(7, 0, "Tag:", tag);
  addField(7, 1, QString::fromUtf8("Priorité:"), priorite);

  mainLay->addLayout(grid);

  // Spacer
  mainLay->addStretch();

  // Close button
  QPushButton *btnClose = new QPushButton("Fermer");
  btnClose->setObjectName("btnCloseCard");
  btnClose->setCursor(Qt::PointingHandCursor);
  QHBoxLayout *closeRow = new QHBoxLayout();
  closeRow->addStretch();
  closeRow->addWidget(btnClose);
  closeRow->addStretch();
  mainLay->addLayout(closeRow);

  connect(btnClose, &QPushButton::clicked, machineCardDialog, &QDialog::close);

  // === Polling timer for real-time color update ===
  if (machineCardPollingTimer) {
    machineCardPollingTimer->stop();
    delete machineCardPollingTimer;
  }
  machineCardPollingTimer = new QTimer(this);
  machineCardPollingTimer->setInterval(5000); // 5 seconds

  QString machineId = m_selectedMachineId;
  QDialog *dlg = machineCardDialog;

  connect(machineCardPollingTimer, &QTimer::timeout, this,
          [this, dlg, machineId, buildDialogStyle, lblStatus]() {
            if (!dlg || !dlg->isVisible()) {
              if (machineCardPollingTimer)
                machineCardPollingTimer->stop();
              return;
            }
            // Re-query ETAT_FONCTIONNEMENT
            QSqlQuery pollQuery(ConnectionMachine::getInstance().getDatabase());
            pollQuery.prepare("SELECT ETAT_FONCTIONNEMENT FROM MACHINE WHERE ID_MACHINE = :id");
            pollQuery.bindValue(":id", machineId);
            if (pollQuery.exec() && pollQuery.next()) {
              QString newFonct = pollQuery.value(0).toString();
              QString newColor;
              if (newFonct == "Normal")
                newColor = "#4CAF50";
              else if (newFonct == "Alerte")
                newColor = "#FF9800";
              else if (newFonct == "Panne")
                newColor = "#F44336";
              else
                newColor = "#4CAF50";
              dlg->setStyleSheet(buildDialogStyle(newColor));
              lblStatus->setText(newFonct);
            }
          });

  machineCardPollingTimer->start();

  // Stop polling when dialog closes
  connect(dlg, &QDialog::finished, this, [this]() {
    if (machineCardPollingTimer) {
      machineCardPollingTimer->stop();
    }
    machineCardDialog = nullptr;
  });

  machineCardDialog->show();
}

// ============================================================================
// Real-time Machine Counts Implementation
// ============================================================================

void machine::updateMachineCounts() {
  int countNormal = 0, countAlerte = 0, countPanne = 0;

  QSqlQuery query(ConnectionMachine::getInstance().getDatabase());

  // Count Normal
  query.prepare("SELECT COUNT(*) FROM MACHINE WHERE ETAT_FONCTIONNEMENT = 'Normal'");
  if (query.exec() && query.next())
    countNormal = query.value(0).toInt();

  // Count Alerte
  query.prepare("SELECT COUNT(*) FROM MACHINE WHERE ETAT_FONCTIONNEMENT = 'Alerte'");
  if (query.exec() && query.next())
    countAlerte = query.value(0).toInt();

  // Count Panne
  query.prepare("SELECT COUNT(*) FROM MACHINE WHERE ETAT_FONCTIONNEMENT = 'Panne'");
  if (query.exec() && query.next())
    countPanne = query.value(0).toInt();

  // Update labels with HTML format matching the .ui design
  ui->lblMachinesNormales_machine->setText(
      QString("<html><head/><body>"
              "<p style=\"margin:0; padding:0;\">"
              "<span style=\" font-size:14pt; font-weight:700; color:#2E7D32;\">\xe2\x9c\x93</span>"
              "<span style=\" font-size:11pt;\"> Machines normales</span></p>"
              "<p style=\"margin:2px 0 0 0; padding:0;\">"
              "<span style=\" font-size:20pt; font-weight:700; color:#1B5E20;\">%1</span></p>"
              "</body></html>")
          .arg(countNormal));

  ui->lblMachinesAlerte_machine->setText(
      QString("<html><head/><body>"
              "<p style=\"margin:0; padding:0;\">"
              "<span style=\" font-size:14pt; font-weight:700; color:#F57F17;\">!</span>"
              "<span style=\" font-size:11pt;\"> Machines en alerte</span></p>"
              "<p style=\"margin:2px 0 0 0; padding:0;\">"
              "<span style=\" font-size:20pt; font-weight:700; color:#FFC107;\">%1</span></p>"
              "</body></html>")
          .arg(countAlerte));

  ui->lblMachinesPanne_machine->setText(
      QString("<html><head/><body>"
              "<p style=\"margin:0; padding:0;\">"
              "<span style=\" font-size:16pt; font-weight:700; color:#D32F2F;\">\xe2\x9c\x97</span>"
              "<span style=\" font-size:12pt;\"> Machines en panne</span></p>"
              "<p style=\"margin:2px 0 0 0; padding:0;\">"
              "<span style=\" font-size:24pt; font-weight:700; color:#B71C1C;\">%1</span></p>"
              "</body></html>")
          .arg(countPanne));
}
