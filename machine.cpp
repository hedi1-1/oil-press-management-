#include "machine.h"
#include "ui_machine.h"
#include <QCheckBox>
#include <QComboBox>
#include <QDateTime>
#include <QDebug>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QHeaderView>
#include <QLineEdit>
#include <QMessageBox>
#include <QSpacerItem>
#include <QSqlError>
#include <QSqlQuery>
#include <QStandardItemModel>
#include <QVBoxLayout>

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
    : QMainWindow(parent), ui(new Ui::machine), navigationBar(nullptr) {
  ui->setupUi(this);

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
  connect(ui->btnBackToMenu, &QPushButton::clicked, this,
          &machine::onBackButtonClicked);

  // Setup date/time timer
  dateTimeTimer = new QTimer(this);
  connect(dateTimeTimer, &QTimer::timeout, this, &machine::updateDateTime);
  dateTimeTimer->start(1000); // Update every second
  updateDateTime();           // Initial update

  onNavigationTabClicked(0);

  // Initialize and load data
  setupMachineTable();
  chargerMachines();

  // Style the whole window
  connect(ui->btnExporter, &QPushButton::clicked, this, [this]() {
    // Hide form and lists, show summary
    ui->scrollHistoriqueOnOff->setVisible(false);
    ui->scrollListeMachines->setVisible(true);
    ui->btnHistoriqueToggle->setChecked(false);
    // TODO: Implement export logic
  });

  // Connect Afficher Tableau toggle button
  connect(ui->btnAfficherTableau, &QPushButton::toggled, this,
          [this](bool checked) {
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
  connect(ui->btnHistoriqueToggle, &QPushButton::toggled, this,
          [this](bool checked) {
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
    ui->scrollHistoriqueOnOff->setVisible(false);
    ui->scrollListeMachines->setVisible(true);
    ui->btnHistoriqueToggle->setChecked(false);
    // TODO: Implement search logic
  });

  connect(ui->btnFiltrer, &QPushButton::clicked, this, [this]() {
    ui->scrollHistoriqueOnOff->setVisible(false);
    ui->scrollListeMachines->setVisible(true);
    ui->btnHistoriqueToggle->setChecked(false);
    // TODO: Implement filter logic
  });

  // Setup todo list for maintenance
  setupTodoList();

  // Initialize "Actions" form dates
  ui->dateEdit_derniere_maintenance_machine->setDate(QDate::currentDate());
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

  ui->lblDateTime->setText(dateTimeStr);
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
  // Create model with columns matching requested order (13 columns)
  machineTableModel = new QStandardItemModel(0, 13, this);
  machineTableModel->setHorizontalHeaderLabels(
      {"ID", "Nom", "Type", "État marche", "Temp. (°C)", "Charge (%)",
       "Fonctionnement", "Alerte", "Criticité", "Dernière maintenance",
       "Date d'installation", "Score Santé", "Date mise à jour"});

  // Configure UI table view
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
  // Désactive le stretch automatique pour permettre le scroll horizontal
  ui->tableMachines->horizontalHeader()->setStretchLastSection(false);
  ui->tableMachines->horizontalHeader()->setSectionResizeMode(
      QHeaderView::Interactive);
  ui->tableMachines->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  ui->tableMachines->setShowGrid(true);

  // Set row height
  ui->tableMachines->verticalHeader()->setDefaultSectionSize(40);
}

void machine::onNavigationTabClicked(int index) {
  // When changing tabs, hide form and reset view
  if (index == 0) {
    // Parc machines tab: show list by default
    ui->scrollHistoriqueOnOff->setVisible(false);
    ui->scrollListeMachines->setVisible(true);
    ui->btnHistoriqueToggle->setChecked(false);
  } else {
    // Other tabs: hide all parc machines specific views
    ui->scrollHistoriqueOnOff->setVisible(false);
    ui->scrollListeMachines->setVisible(false);
  }

  // Update the tab widget to show the corresponding tab
  ui->tabWidgetMachineManagement->setCurrentIndex(index);
}

// ============================================================================
// Todo List (Machines à réparer) Implementation
// ============================================================================

void machine::setupTodoList() {
  // Connect the "+ Ajouter" button
  connect(ui->btnAjouterTodo, &QPushButton::clicked, this,
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

void machine::on_pushButton_enregistrer_machine_clicked() {
  // 1. Retrieve information from the form
  QString nom = ui->lineEdit_nom_machine->text().trimmed();
  QString type = ui->comboBox_type_machine->currentText();
  QString etatMarche = ui->comboBox_etat_marche_machine->currentText();
  double temp = ui->doubleSpinBox_temperature_machine->value();
  double charge = ui->doubleSpinBox_niveau_charge_machine->value();
  QString etatFonct = ui->comboBox_etat_fonctionnement_machine->currentText();
  QString typeAlerte = ui->comboBox_type_alerte_machine->currentText();
  QString criticite = ui->comboBox_niveau_criticite_machine->currentText();
  QDate maintenance = ui->dateEdit_derniere_maintenance_machine->date();
  int scoreSante = ui->spinBox_score_sante_machine->value();

  // 2. Simple validation
  if (nom.isEmpty()) {
    QMessageBox::warning(this, "Champs vides",
                         "Veuillez remplir le nom de la machine.");
    return;
  }

  // 3. Database Insertion
  QSqlQuery query;
  query.prepare(
      "INSERT INTO MACHINE (NOM_MACHINE, TYPE_MACHINE, ETAT_MARCHE, "
      "TEMPERATURE_ACTUELLE, NIVEAU_CHARGE, ETAT_FONCTIONNEMENT, TYPE_ALERTE, "
      "NIVEAU_CRITICITE, DATE_DERNIERE_MAINTENANCE, SCORE_SANTE) "
      "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");

  query.addBindValue(nom);
  query.addBindValue(type);
  query.addBindValue(etatMarche);
  query.addBindValue(temp);
  query.addBindValue(charge);
  query.addBindValue(etatFonct);
  query.addBindValue(typeAlerte);
  query.addBindValue(criticite);
  query.addBindValue(maintenance); // SQL handle QDate automatically
  query.addBindValue(scoreSante);

  if (query.exec()) {
    // 4. Success Popup with Custom Green Theme
    QMessageBox msgBox(this);
    msgBox.setWindowTitle("Succès");
    msgBox.setText("✅ Machine ajoutée avec succès !");
    msgBox.setIcon(QMessageBox::Information);

    // Apply custom styling to match the theme (Green #1A3C2F)
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

    // Optionally reset the form or refresh the table
    ui->lineEdit_nom_machine->clear();
    // Refresh table if needed (e.g., call setupMachineTable() or a refresh
    // method)
  } else {
    QMessageBox::critical(this, "Erreur",
                          "Échec de l'ajout dans la base de données : " +
                              query.lastError().text());
  }
}

void machine::on_btnReinitialiser_clicked() {
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
}

void machine::chargerMachines() {
  // 1. Vider le tableau (remettre le nombre de lignes à 0)
  machineTableModel->setRowCount(0);

  // 2. Préparer et exécuter la requête SQL pour récupérer les données dans
  // l'ordre exact de la base de données
  QSqlQuery query;
  // Correction : DATE_MAJ -> DATE_MISE_A_JOUR
  // Note : FABRICANT n'existe pas dans la BD image, on utilise "" ou
  // SCORE_SANTE? On va utiliser SCORE_SANTE pour la colonne 12 comme dans
  // l'image BD
  query.prepare("SELECT ID_MACHINE, NOM_MACHINE, TYPE_MACHINE, ETAT_MARCHE, "
                "TEMPERATURE_ACTUELLE, NIVEAU_CHARGE, ETAT_FONCTIONNEMENT, "
                "TYPE_ALERTE, NIVEAU_CRITICITE, DATE_DERNIERE_MAINTENANCE, "
                "DATE_INSTALLATION, SCORE_SANTE, DATE_MISE_A_JOUR "
                "FROM MACHINE ORDER BY ID_MACHINE ASC");

  if (!query.exec()) {
    qDebug() << "Erreur lors du chargement des machines :"
             << query.lastError().text();
    return;
  }

  // Polices et couleurs communes
  QFont boldFont;
  boldFont.setBold(true);

  // 3. Parcourir les résultats de la requête ligne par ligne
  while (query.next()) {
    QList<QStandardItem *> row;

    // Récupérer les valeurs colonnes par colonnes
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
    QString scoreSante =
        query.value(11)
            .toString(); // Utilisation de SCORE_SANTE à la place de FABRICANT
    QString maj = query.value(12).toDate().toString("yyyy-MM-dd");

    QStringList values = {id,      nom,        type,   etatMarche, temp,
                          charge,  fonction,   alerte, criticite,  maintenance,
                          install, scoreSante, maj};

    // Créer les items pour chaque cellule de la ligne
    for (int i = 0; i < values.size(); ++i) {
      auto *item = new QStandardItem(values[i]);
      item->setEditable(false);
      item->setTextAlignment(Qt::AlignCenter);
      item->setForeground(
          QColor("#333333")); // Force la couleur sombre pour la visibilité
      row.append(item);
    }

    // 4. Coloration conditionnelle selon les règles métier

    // -- État Marche (Colonne 3)
    row[3]->setFont(boldFont);
    if (etatMarche == "ON")
      row[3]->setForeground(QColor("#2E7D32")); // Vert
    else if (etatMarche == "VEILLE")
      row[3]->setForeground(QColor("#E65100")); // Orange
    else if (etatMarche == "OFF")
      row[3]->setForeground(QColor("#C62828")); // Rouge

    // -- Etat Fonctionnement (Colonne 6)
    row[6]->setFont(boldFont);
    if (fonction == "Normal")
      row[6]->setForeground(QColor("#2E7D32")); // Vert
    else if (fonction == "Alerte")
      row[6]->setForeground(QColor("#E65100")); // Orange
    else if (fonction == "Panne")
      row[6]->setForeground(QColor("#C62828")); // Rouge

    // -- Type Alerte (Colonne 7)
    row[7]->setFont(boldFont);
    if (alerte == "Aucune")
      row[7]->setForeground(QColor("#2E7D32")); // Vert
    else if (alerte == "Panne")
      row[7]->setForeground(QColor("#E65100")); // Orange
    else if (alerte == "Surcharge")
      row[7]->setForeground(QColor("#C62828")); // Rouge

    // -- Niveau Criticité (Colonne 8)
    row[8]->setFont(boldFont);
    if (criticite == "Faible")
      row[8]->setForeground(QColor("#2E7D32")); // Vert
    else if (criticite == "Élevé")
      row[8]->setForeground(QColor("#E65100")); // Orange
    else if (criticite == "Critique")
      row[8]->setForeground(QColor("#C62828")); // Rouge

    // Ajouter la ligne complète au modèle
    machineTableModel->appendRow(row);
  }
}
