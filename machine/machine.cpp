#include "machine.h"
#include "connexionmachine.h"
#include "ui_machine.h"
#include <QCoreApplication>
#include <QCheckBox>
#include <QComboBox>
#include <QDateEdit>
#include <QDateTime>
#include <QDebug>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QPixmap>
#include <QSpacerItem>
#include <QSpinBox>
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
    : QMainWindow(parent), ui(new Ui::machine), navigationBar(nullptr),
      m_selectedRow(-1) {
  ui->setupUi(this);
  {
      QPixmap logo;
      for (const QString &p : QStringList{":/logo.png", ":/assets/logo.png", "logo.png",
           "../production/logo.png", "../machine/assets/logo.png",
           QCoreApplication::applicationDirPath() + "/../../logo.png",
           QCoreApplication::applicationDirPath() + "/../../../production/logo.png"}) {
          logo = QPixmap(p);
          if (!logo.isNull()) break;
      }
      if (!logo.isNull())
          ui->lblLogo_machine->setPixmap(logo.scaledToHeight(50, Qt::SmoothTransformation));
  }

  // Disable toolbar buttons initially
  ui->btnModifierMachine_machine->setEnabled(false);
  ui->btnSupprimerMachine_machine->setEnabled(false);

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
  chargerMachines();

  // Style the whole window
  connect(ui->btnExporter_machine, &QPushButton::clicked, this, [this]() {
    // Hide form and lists, show summary
    ui->scrollHistoriqueOnOff_machine->setVisible(false);
    ui->scrollListeMachines->setVisible(true);
    ui->btnHistoriqueToggle_machine->setChecked(false);
    // TODO: Implement export logic
  });

  // Connect Afficher Tableau toggle button
  connect(ui->btnAfficherTableau_machine, &QPushButton::toggled, this,
          [this](bool checked) {
            ui->scrollHistoriqueOnOff_machine->setVisible(false);
            ui->btnHistoriqueToggle_machine->blockSignals(true);
            ui->btnHistoriqueToggle_machine->setChecked(false);
            ui->btnHistoriqueToggle_machine->blockSignals(false);

            if (checked) {
              ui->tableMachines_machine->setVisible(true);
              ui->scrollListeMachines->setVisible(false);
            } else {
              ui->tableMachines_machine->setVisible(false);
              ui->scrollListeMachines->setVisible(true);
            }
          });

  // Connect Historique ON/OFF toggle button
  connect(ui->btnHistoriqueToggle_machine, &QPushButton::toggled, this,
          [this](bool checked) {
            ui->btnAfficherTableau_machine->blockSignals(true);
            ui->btnAfficherTableau_machine->setChecked(false);
            ui->btnAfficherTableau_machine->blockSignals(false);

            if (checked) {
              ui->scrollHistoriqueOnOff_machine->setVisible(true);
              ui->scrollListeMachines->setVisible(false);
            } else {
              ui->scrollHistoriqueOnOff_machine->setVisible(false);
              ui->scrollListeMachines->setVisible(true);
            }
            ui->tableMachines_machine->setVisible(false);
          });

  // Connect close button for Historique ON/OFF
  connect(ui->btnFermerHistorique, &QPushButton::clicked, this, [this]() {
    ui->btnHistoriqueToggle_machine->setChecked(false);
    ui->scrollHistoriqueOnOff_machine->setVisible(false);
    ui->scrollListeMachines->setVisible(true);
    ui->tableMachines_machine->setVisible(false);
  });

  // Connect search and filter buttons
  connect(ui->btnRechercher, &QPushButton::clicked, this, [this]() {
    ui->scrollHistoriqueOnOff_machine->setVisible(false);
    ui->scrollListeMachines->setVisible(true);
    ui->btnHistoriqueToggle_machine->setChecked(false);
    // TODO: Implement search logic
  });

  connect(ui->btnFiltrer, &QPushButton::clicked, this, [this]() {
    ui->scrollHistoriqueOnOff_machine->setVisible(false);
    ui->scrollListeMachines->setVisible(true);
    ui->btnHistoriqueToggle_machine->setChecked(false);
    // TODO: Implement filter logic
  });

  // Connect toolbar buttons
  connect(ui->btnSupprimerMachine_machine, &QPushButton::clicked, this,
          &machine::on_btnSupprimerMachine_machine_clicked);
  connect(ui->btnModifierMachine_machine, &QPushButton::clicked, this,
          &machine::on_btnModifierMachine_machine_clicked);

  connect(ui->tableMachines_machine, &QTableView::doubleClicked, this,
          &machine::on_tableMachines_machine_doubleClicked);

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
  // Create model with columns matching requested order (13 columns)
  machineTableModel = new QStandardItemModel(0, 13, this);
  machineTableModel->setHorizontalHeaderLabels(
      {"ID", "Nom", "Type", "État marche", "Temp. (°C)", "Charge (%)",
       "Fonctionnement", "Alerte", "Criticité", "Dernière maintenance",
       "Date d'installation", "Score Santé", "Date mise à jour"});

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

void machine::onNavigationTabClicked(int index) {
  // When changing tabs, hide form and reset view
  if (index == 0) {
    // Parc machines tab: show list by default
    ui->scrollHistoriqueOnOff_machine->setVisible(false);
    ui->scrollListeMachines->setVisible(true);
    ui->btnHistoriqueToggle_machine->setChecked(false);
  } else {
    // Other tabs: hide all parc machines specific views
    ui->scrollHistoriqueOnOff_machine->setVisible(false);
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
  QSqlQuery query(ConnectionMachine::getInstance().getDatabase());
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

    // Refresh table to show the new machine
    chargerMachines();
  } else {
    QMessageBox::critical(this, "Erreur",
                          "Échec de l'ajout dans la base de données : " +
                              query.lastError().text());
  }
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
}

void machine::chargerMachines() {
  // 1. Vider le tableau (remettre le nombre de lignes à 0)
  machineTableModel->setRowCount(0);

  // 2. Préparer et exécuter la requête SQL pour récupérer les données dans
  // l'ordre exact de la base de données
  QSqlQuery query(ConnectionMachine::getInstance().getDatabase());
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

void machine::on_tableMachines_machine_doubleClicked(const QModelIndex &index) {
  if (!index.isValid())
    return;

  m_selectedRow = index.row();
  m_selectedMachineId = machineTableModel->item(m_selectedRow, 0)->text();

  // Highlight the row and enable buttons
  ui->tableMachines_machine->selectRow(m_selectedRow);
  ui->btnModifierMachine_machine->setEnabled(true);
  ui->btnSupprimerMachine_machine->setEnabled(true);

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
  cmbEtat->addItems({"ON", "OFF"});
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
  cmbFonct->addItems({"Normal", "Dégradé", "Arrêt"});
  cmbFonct->setCurrentText(machineTableModel->item(m_selectedRow, 6)->text());

  QComboBox *cmbAlerte = new QComboBox();
  cmbAlerte->addItems({"Aucune", "Faible", "Moyenne", "Critique"});
  cmbAlerte->setCurrentText(machineTableModel->item(m_selectedRow, 7)->text());

  QComboBox *cmbCrit = new QComboBox();
  cmbCrit->addItems({"Faible", "Moyenne", "Élevé", "Critique"});
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
  formLayout->addRow("Mode Fonctionnement:", cmbFonct);
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
