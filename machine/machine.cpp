#include "machine.h"
#include "MachineServer.h"
#include "chatbot.h"
#include "connexionmachine.h"
#include "ui_machine.h"
#include <QBrush>
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
#include <QHostAddress>
#include <QImage>
#include <QMap>
#include <QMessageBox>
#include <QNetworkInterface>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QEventLoop>
#include <QPainter>
#include <QPrinter>
#include <QRegularExpression>
#include <QScrollBar>
#include <QSignalBlocker>
#include <QSpacerItem>
#include <QSpinBox>
#include <QSet>
#include <QSqlError>
#include <QSqlQuery>
#include <QTextDocumentWriter>
#include <QTableWidgetItem>
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

#ifdef MACHINE_USE_QRENCODE
#include <qrencode.h>
#endif

namespace {
struct CardVisualStyle {
  QString borderColor;
  QString backgroundStart;
  QString backgroundEnd;
};

CardVisualStyle cardVisualForState(const QString &fonctionnementRaw) {
  const QString fonctionnement = fonctionnementRaw.trimmed().toLower();
  if (fonctionnement == "normal") {
    return {"#4CAF50", "#174036", "#0F2E27"};
  }
  if (fonctionnement == "alerte") {
    return {"#FF9800", "#5A3A17", "#3F2810"};
  }
  if (fonctionnement == "panne") {
    return {"#F44336", "#5C252B", "#3F171C"};
  }
  return {"#607D8B", "#2E3D45", "#1F2A30"};
}

QString veilleButtonStyle(bool active) {
  if (active) {
    return "QPushButton { min-width: 0px; padding: 4px; font-size: 18px; "
           "background-color: #FF9800; color: white; border-radius: 4px; "
           "border: 2px solid #C96F00; } "
           "QPushButton:disabled { background-color: #e0e0e0; color: #9e9e9e; "
           "border: none; }";
  }

  return "QPushButton { min-width: 0px; padding: 4px; font-size: 18px; "
         "background-color: #607D8B; color: white; border-radius: 4px; border: none; } "
         "QPushButton:disabled { background-color: #e0e0e0; color: #9e9e9e; }";
}

QString onOffButtonStyle(const QString &etatMarcheRaw) {
  const QString etatMarche = etatMarcheRaw.trimmed().toUpper();
  if (etatMarche == "ON") {
    return "QPushButton { min-width: 0px; padding: 4px; font-size: 18px; "
           "background-color: #F44336; color: white; border-radius: 4px; "
           "border: 2px solid #B71C1C; } "
           "QPushButton:disabled { background-color: #e0e0e0; color: #9e9e9e; "
           "border: none; }";
  }
  if (etatMarche == "OFF") {
    return "QPushButton { min-width: 0px; padding: 4px; font-size: 18px; "
           "background-color: #4CAF50; color: white; border-radius: 4px; "
           "border: 2px solid #2E7D32; } "
           "QPushButton:disabled { background-color: #e0e0e0; color: #9e9e9e; "
           "border: none; }";
  }
  if (etatMarche == "VEILLE") {
    return "QPushButton { min-width: 0px; padding: 4px; font-size: 18px; "
           "background-color: #FF9800; color: white; border-radius: 4px; "
           "border: 2px solid #C96F00; } "
           "QPushButton:disabled { background-color: #e0e0e0; color: #9e9e9e; "
           "border: none; }";
  }

  return "QPushButton { min-width: 0px; padding: 4px; font-size: 18px; "
         "background-color: #607D8B; color: white; border-radius: 4px; border: none; } "
         "QPushButton:disabled { background-color: #e0e0e0; color: #9e9e9e; }";
}

int priorityScore(const QString &priorityRaw) {
  const QString p = priorityRaw.trimmed().toLower();
  if (p.contains("urgent") || p.contains("critique")) {
    return 3;
  }
  if (p.contains("haute") || p.contains("élev") || p.contains("eleve")) {
    return 2;
  }
  if (p.contains("moy") || p.contains("normal")) {
    return 1;
  }
  return 0;
}

bool tagImpliesPriority(const QString &tagRaw) {
  const QString t = tagRaw.trimmed().toLower();
  return t.contains("urgent") || t.contains("critique") ||
         t.contains("maintenance") || t.contains("reparer") ||
         t.contains("réparer") || t.contains("priorit") || t.contains("panne");
}

bool isLowPriorityLabel(const QString &priorityRaw) {
  const QString p = priorityRaw.trimmed().toLower();
  return p.contains("basse") || p.contains("faible") || p.contains("low");
}

QString makeDarkStyleFromLight(QString style) {
  const QVector<QPair<QString, QString>> replacements = {
      {"#f5f5f5", "#12161C"},   {"#f8f9fa", "#1B222C"},
      {"#f0f0f0", "#2A333F"},   {"#e8e8e8", "#333F4E"},
      {"#ddd", "#3A4657"},       {"#eee", "#334253"},
      {"#444", "#D5DDEA"},       {"#333", "#E5EAF2"},
      {"background-color: white", "background-color: #1D2632"},
      {"color: white", "color: #F3F6FB"},
      {"#1B4D3E", "#224A42"},   {"#1A3C2F", "#1D4652"},
      {"#0D3B2E", "#173742"},   {"#2D5F47", "#355972"},
      {"#FAFAFA", "#1D2632"},   {"#F5F5F5", "#1A2330"},
      {"#E8F5E9", "#1F3340"},   {"#C8E6C9", "#2D4D56"},
      {"#FFEBEE", "#3C2A31"},   {"#FFCDD2", "#53333C"},
      {"#FFF9C4", "#3C382A"},   {"#FFF59D", "#544D33"},
      {"#E0E0E0", "#394657"},   {"#9e9e9e", "#AEB9C9"}};

  for (const auto &entry : replacements) {
    style.replace(entry.first, entry.second, Qt::CaseInsensitive);
  }
  return style;
}
} // namespace
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
  employeeTableModel(nullptr), historiqueTableModel(nullptr), statsComparisonModel(nullptr), m_selectedRow(-1),
      machineCardPollingTimer(nullptr), machineCardDialog(nullptr),
      countsTimer(nullptr), historiqueTimer(nullptr), m_machineServer(nullptr) {
  ui->setupUi(this);

  m_lightStyleSheet = styleSheet();
  m_darkStyleSheet = makeDarkStyleFromLight(m_lightStyleSheet);

  // Disable toolbar buttons initially
  ui->btnModifierMachine_machine->setEnabled(false);
  ui->btnSupprimerMachine_machine->setEnabled(false);
  ui->btnExporter_machine->setEnabled(false);
  ui->btnToggleOnOff_machine->setEnabled(false);
  ui->btnToggleOnOff_machine->setStyleSheet(onOffButtonStyle(""));
  ui->btnSetVeille_machine->setEnabled(false);
  ui->btnSetVeille_machine->setStyleSheet(veilleButtonStyle(false));
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
    connect(ui->btnThemeToggle_machine, &QPushButton::clicked, this,
      &machine::onThemeToggleClicked);
    connect(ui->btnLanguageToggle_machine, &QPushButton::clicked, this,
      &machine::onLanguageToggleClicked);

  // --- CHATBOT INTEGRATION ---
  m_chatbot = new Chatbot(this);
  m_btnChatbot = new QPushButton("🤖", this);
  m_btnChatbot->setToolTip("Ouvrir le Chatbot Gemini");
  m_btnChatbot->setStyleSheet(
      "QPushButton { "
      "  background-color: rgba(255, 255, 255, 0.12); "
      "  color: #C9A227; "
      "  border: 1px solid rgba(255, 255, 255, 0.25); "
      "  border-radius: 8px; "
      "  font-size: 16px; "
      "} "
      "QPushButton:hover { "
      "  background-color: rgba(255, 255, 255, 0.2); "
      "}"
  );
  m_btnChatbot->setFixedSize(40, 30);
  
  // Insertion claire du bouton Chatbot à droite du bouton Vocal
  // Le layout qui contient le bouton Parler s'appelle subtitleLayout dans le fichier .ui
  if (ui->subtitleLayout) {
      ui->subtitleLayout->addWidget(m_btnChatbot);
  }

  connect(m_btnChatbot, &QPushButton::clicked, m_chatbot, &Chatbot::toggleVisibility);
  m_chatbot->raise();

  // Setup date/time timer
  dateTimeTimer = new QTimer(this);
  connect(dateTimeTimer, &QTimer::timeout, this, &machine::updateDateTime);
  dateTimeTimer->start(1000); // Update every second

  applyTheme();
  applyLanguage();
  updateDateTime(); // Initial update

  onNavigationTabClicked(0);

  // Initialize and load data
  setupMachineTable();
  setupEmployeesTable();
  setupStatsComparisonTable();
  normalizeLowPriorityTags();
  
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
  populateStatsComparisonTable();

    ui->tableCarnetMachine_machine->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableCarnetMachine_machine->verticalHeader()->setVisible(false);
    ui->tableCarnetMachine_machine->horizontalHeader()->setStretchLastSection(true);
    ui->tableCarnetMachine_machine->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    ui->tableCarnetMachine_machine->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    ui->tableCarnetMachine_machine->setWordWrap(true);

    connect(ui->comboDecisionMachine, QOverload<int>::of(&QComboBox::currentIndexChanged),
      this, [this](int) {
        updateAICarnetForSelectedMachine();
        if (ui->comboDecisionMachine->currentData().toString().trimmed().isEmpty()) {
          resetQrPreviewLabel();
        }
      });
    refreshAIMachineSelector();
    connect(ui->btnGenererQrMachine, &QPushButton::clicked,
            this, &machine::on_btnGenererQrMachine_clicked);
    resetQrPreviewLabel();
        ui->lblQrPreviewMachine->setCursor(Qt::PointingHandCursor);
        ui->lblQrPreviewMachine->installEventFilter(this);

    m_serverHostIp = resolveLocalIpv4();
    m_machineServer = new MachineServer(this);
    connect(m_machineServer, &MachineServer::machineRequested,
            this, &machine::onMachineRequestedFromHttp, Qt::UniqueConnection);
    m_machineServer->setHostIp(m_serverHostIp);
    if (!m_machineServer->start(8181)) {
      qDebug() << "[MachineServer] Echec au demarrage du serveur HTTP sur le port 8181.";
    }

  // Export PDF button
  connect(ui->btnExporter_machine, &QPushButton::clicked, this,
          [this]() { afficherDialogExport(); });

  // Connect Historique toggle button
  ui->btnHistoriqueToggle_machine->setText(
      m_isFrench ? QString::fromUtf8("\xF0\x9F\x93\x9C Historique")
                 : QString::fromUtf8("\xF0\x9F\x93\x9C History"));
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
      ui->btnToggleOnOff_machine->setStyleSheet(onOffButtonStyle(newState));
      ui->btnSetVeille_machine->setStyleSheet(veilleButtonStyle(false));
      QMessageBox msgBox(this);
      msgBox.setWindowTitle(m_isFrench ? "Succès" : "Success");
      msgBox.setText(
          m_isFrench
          ? QString("✅ Machine %1 avec succès !")
            .arg(newState == "ON" ? "allumée" : "éteinte")
          : QString("✅ Machine turned %1 successfully!")
            .arg(newState == "ON" ? "ON" : "OFF"));
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
      QMessageBox::critical(this, m_isFrench ? "Erreur" : "Error",
                            (m_isFrench
                                 ? "Erreur lors du changement d'état : "
                                 : "Error while changing state: ") +
                                query.lastError().text());
    }
  });

  connect(ui->btnSetVeille_machine, &QPushButton::clicked, this, [this]() {
    if (m_selectedMachineId.isEmpty() || m_selectedRow < 0)
      return;

    QSqlQuery query(ConnectionMachine::getInstance().getDatabase());
    query.prepare("UPDATE MACHINE SET ETAT_MARCHE = 'VEILLE', DATE_MISE_A_JOUR = "
                  "SYSTIMESTAMP WHERE ID_MACHINE = :id");
    query.bindValue(":id", m_selectedMachineId);

    if (query.exec()) {
      ui->btnToggleOnOff_machine->setStyleSheet(onOffButtonStyle("VEILLE"));
      ui->btnSetVeille_machine->setStyleSheet(veilleButtonStyle(true));
      QMessageBox msgBox(this);
      msgBox.setWindowTitle(m_isFrench ? "Succès" : "Success");
      msgBox.setText(
          m_isFrench
              ? "✅ Machine passée en mode veille avec succès !"
              : "✅ Machine switched to standby successfully!");
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
      QMessageBox::critical(this, m_isFrench ? "Erreur" : "Error",
                            (m_isFrench
                                 ? "Erreur lors du passage en veille : "
                                 : "Error while switching to standby: ") +
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
  connect(ui->btnGenererStats_machine, &QPushButton::clicked, this,
          &machine::on_btnGenererStats_machine_clicked);
  clearStatsChartArea(
      m_isFrench
          ? "Sélectionnez un type de statistique et un type de graphique, puis cliquez sur Générer."
          : "Select a statistics type and a chart type, then click Generate.");
}

machine::~machine() {
  if (dateTimeTimer) {
    dateTimeTimer->stop();
  }
  delete ui;
}

void machine::updateDateTime() {
  QDateTime now = QDateTime::currentDateTime();

  QStringList monthsFR = {"janvier",   "février", "mars",     "avril",
                          "mai",       "juin",    "juillet",  "août",
                          "septembre", "octobre", "novembre", "décembre"};
  QStringList monthsEN = {"January",   "February", "March",    "April",
                          "May",       "June",     "July",     "August",
                          "September", "October",  "November", "December"};

  int day = now.date().day();
  int month = now.date().month();
  int year = now.date().year();
  int hour = now.time().hour();
  int minute = now.time().minute();
  int second = now.time().second();

  QString dateStr = QString::number(day).rightJustified(2, '0');
  QString monthStr = m_isFrench ? monthsFR[month - 1] : monthsEN[month - 1];
  if (m_isFrench && !monthStr.isEmpty()) {
    monthStr[0] = monthStr[0].toUpper();
  }

  QString timeStr = QString::number(hour).rightJustified(2, '0') + ":" +
                    QString::number(minute).rightJustified(2, '0') + ":" +
                    QString::number(second).rightJustified(2, '0');

  QString dateTimeStr;
  if (m_isFrench) {
    dateTimeStr = QString("📅 %1 %2 %3 • %4")
                      .arg(dateStr, monthStr, QString::number(year), timeStr);
  } else {
    dateTimeStr = QString("📅 %1 %2 %3 • %4")
                      .arg(monthStr, dateStr, QString::number(year), timeStr);
  }

  ui->lblDateTime_machine->setText(dateTimeStr);
}

void machine::onThemeToggleClicked() {
  m_isDarkMode = !m_isDarkMode;
  applyTheme();
}

void machine::onLanguageToggleClicked() {
  m_isFrench = !m_isFrench;
  applyLanguage();
  updateDateTime();
}

void machine::applyTheme() {
  setStyleSheet(m_isDarkMode ? m_darkStyleSheet : m_lightStyleSheet);

  const QString headerLight =
    "QWidget#headerWidget { background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #1B4332, stop:0.5 #234E3E, stop:1 #1B4332); border-bottom: 4px solid #C9A227; }";
  const QString headerDark =
    "QWidget#headerWidget { background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #121A24, stop:0.5 #1A2430, stop:1 #121A24); border-bottom: 4px solid #8F7A2F; }";
  ui->headerWidget->setStyleSheet(m_isDarkMode ? headerDark : headerLight);

  const QString iconButtonStyle = m_isDarkMode
    ? "QPushButton { background-color: rgba(30, 39, 51, 0.75); color: #C9A227; border: 1px solid rgba(201, 162, 39, 0.40); border-radius: 8px; font-size: 14px; font-weight: 700; } QPushButton:hover { background-color: rgba(42, 56, 73, 0.90); }"
      : "QPushButton { background-color: rgba(255, 255, 255, 0.12); color: #C9A227; border: 1px solid rgba(255, 255, 255, 0.25); border-radius: 8px; font-size: 14px; font-weight: 700; } QPushButton:hover { background-color: rgba(255, 255, 255, 0.20); }";

  ui->btnThemeToggle_machine->setStyleSheet(iconButtonStyle);
  ui->btnLanguageToggle_machine->setStyleSheet(iconButtonStyle);
  ui->btnThemeToggle_machine->setText(m_isDarkMode ? "☀" : "🌙");

  const QString lightDateStyle =
      "color: rgba(255, 255, 255, 0.9); font-size: 14px; font-weight: 500; font-family: 'Segoe UI', Arial, sans-serif; padding: 8px 16px; background-color: rgba(0, 0, 0, 0.2); border-radius: 20px;";
  const QString darkDateStyle =
      "color: #EAEFF6; font-size: 14px; font-weight: 500; font-family: 'Segoe UI', Arial, sans-serif; padding: 8px 16px; background-color: rgba(10, 13, 18, 0.62); border-radius: 20px; border: 1px solid rgba(255, 255, 255, 0.12);";
  ui->lblDateTime_machine->setStyleSheet(m_isDarkMode ? darkDateStyle
                                                      : lightDateStyle);

    const QString lightOnlineStyle =
      "color: #4ADE80; font-weight: bold; font-size: 14px; padding: 6px 14px; background-color: rgba(74, 222, 128, 0.15); border-radius: 15px; border: 1px solid rgba(74, 222, 128, 0.3);";
    const QString darkOnlineStyle =
      "color: #61E39A; font-weight: bold; font-size: 14px; padding: 6px 14px; background-color: rgba(31, 91, 61, 0.30); border-radius: 15px; border: 1px solid rgba(97, 227, 154, 0.35);";
    ui->lblOnlineStatus_machine->setStyleSheet(m_isDarkMode ? darkOnlineStyle
                                 : lightOnlineStyle);

      const QString lightMachineTableStyle = R"(
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
        QTableView::item { padding: 8px 12px; border-bottom: 1px solid #E8E8E8; }
        QTableView::item:hover { background-color: #E0F2E9; }
        QTableView::item:selected { background-color: #C8E6C9; color: #1A3C2F; }
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
        QHeaderView::section:last { border-right: none; }
      )";

      const QString darkMachineTableStyle = R"(
        QTableView {
          border: 2px solid #2A3F5A;
          border-radius: 6px;
          background-color: #111821;
          alternate-background-color: #182332;
          gridline-color: #30445B;
          font-size: 12px;
          font-family: 'Segoe UI', Arial, sans-serif;
          selection-background-color: #24506A;
          selection-color: #EAF0F8;
        }
        QTableView::item { padding: 8px 12px; border-bottom: 1px solid #27384C; color: #DCE4EF; }
        QTableView::item:hover { background-color: #213447; }
        QTableView::item:selected { background-color: #2A4F66; color: #F2F6FC; }
        QHeaderView::section {
          background-color: #1A2B3F;
          color: #F2F6FC;
          padding: 10px 8px;
          border: none;
          border-right: 1px solid #2D425A;
          font-weight: bold;
          font-size: 12px;
          font-family: 'Segoe UI', Arial, sans-serif;
        }
        QHeaderView::section:last { border-right: none; }
      )";

      const QString lightEmployeesTableStyle = R"(
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
        QTableView::item { padding: 6px 10px; border-bottom: 1px solid #E8E8E8; }
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
        QHeaderView::section:last { border-right: none; }
      )";

      const QString darkEmployeesTableStyle = R"(
        QTableView {
          border: 2px solid #2A3F5A;
          border-radius: 6px;
          background-color: #111821;
          alternate-background-color: #182332;
          gridline-color: #30445B;
          font-size: 11px;
          font-family: 'Segoe UI', Arial, sans-serif;
          selection-background-color: #24506A;
          selection-color: #EAF0F8;
        }
        QTableView::item { padding: 6px 10px; border-bottom: 1px solid #27384C; color: #DCE4EF; }
        QHeaderView::section {
          background-color: #1A2B3F;
          color: #F2F6FC;
          padding: 8px;
          border: none;
          border-right: 1px solid #2D425A;
          font-weight: bold;
          font-size: 11px;
          font-family: 'Segoe UI', Arial, sans-serif;
        }
        QHeaderView::section:last { border-right: none; }
      )";

      ui->tableMachines_machine->setStyleSheet(
        m_isDarkMode ? darkMachineTableStyle : lightMachineTableStyle);
      ui->tableEmployees_machine->setStyleSheet(
        m_isDarkMode ? darkEmployeesTableStyle : lightEmployeesTableStyle);
      ui->tableCarnetMachine_machine->setStyleSheet(
        m_isDarkMode ? darkEmployeesTableStyle : lightEmployeesTableStyle);
      ui->tableComparaisonStats_machine->setStyleSheet(
        m_isDarkMode ? darkEmployeesTableStyle : lightEmployeesTableStyle);
      ui->tableHistorique_machine->setStyleSheet(
        m_isDarkMode
          ? "QTableView { border: 1px solid #2E435A; background-color: #111821; color: #EAF0F8; gridline-color: #30445B; } QTableView::item { background-color: transparent; color: #EAF0F8; padding: 6px; } QHeaderView::section { background-color: #1A2B3F; color: #F2F6FC; font-weight: 600; }"
          : "QTableView { border: 1px solid #C9D4D0; background-color: #FFFFFF; color: #1F2933; gridline-color: #D8E0DC; } QTableView::item { background-color: transparent; color: #1F2933; padding: 6px; } QHeaderView::section { background-color: #F3F6F4; color: #0F1720; font-weight: 600; }");

      ui->scrollHistoriqueOnOff_machine->setStyleSheet("");

      ui->zoneGraphique_machine->setStyleSheet(
        m_isDarkMode
          ? "background-color: #101722; border: 1px solid #2E435A; border-radius: 4px;"
          : "background-color: white; border: 1px solid #ddd; border-radius: 4px;");

      ui->tabWidgetMachineManagement->setStyleSheet(
        m_isDarkMode
          ? "QTabWidget::pane { border: 1px solid #2E435A; background-color: #101722; }"
          : "");

      ui->tabWidgetAIDetails->setStyleSheet(
        m_isDarkMode
          ? "QTabWidget::pane { border: 1px solid #2E435A; background-color: #101722; }"
          : "");

      // Premium dark status cards: keep same structure, only harmonize colors.
      if (m_isDarkMode) {
        ui->lblMachinesNormales_machine->setStyleSheet(
            "background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #162633, stop:1 #101B24);"
            "color: #75E3A6; border-left: 5px solid #2BC47A; border-radius: 10px;");
        ui->lblMachinesAlerte_machine->setStyleSheet(
            "background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #2A2418, stop:1 #1B1812);"
            "color: #FFB35C; border-left: 5px solid #F58A2C; border-radius: 10px;");
        ui->lblMachinesPanne_machine->setStyleSheet(
            "background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #2B171D, stop:1 #1D1116);"
            "color: #FF7A86; border-left: 5px solid #E34C5D; border-radius: 10px;");
      } else {
        ui->lblMachinesNormales_machine->setStyleSheet("");
        ui->lblMachinesAlerte_machine->setStyleSheet("");
        ui->lblMachinesPanne_machine->setStyleSheet("");
      }

      refreshTodoList();
}

void machine::applyLanguage() {
  setWindowTitle(m_isFrench ? "Gestion des Machines & Alertes"
                            : "Machine Management & Alerts");
  ui->btnBackToMenu_machine->setText(m_isFrench ? "← Retour" : "← Back");
  ui->lblAppTitle_machine->setText(m_isFrench ? "GESTION DES MACHINES"
                                              : "MACHINE MANAGEMENT");
  ui->lblAppSubtitle->setText(m_isFrench ? "SURVEILLANCE & ALERTES"
                                         : "MONITORING & ALERTS");
  ui->lblOnlineStatus_machine->setText(m_isFrench ? "● EN LIGNE" : "● ONLINE");

  ui->btnThemeToggle_machine->setToolTip(
      m_isFrench ? "Basculer thème sombre / clair"
                 : "Toggle dark / light mode");
  ui->btnLanguageToggle_machine->setToolTip(
      m_isFrench ? "Changer la langue (FR / EN)"
                 : "Change language (FR / EN)");

    ui->groupRecherche_machine->setTitle(m_isFrench ? "Filtrage & Tri"
                            : "Filters & Sorting");
    ui->lblFiltresAvances->setText(m_isFrench ? "⚙ Filtres avancés:"
                        : "⚙ Advanced filters:");
    ui->lblPeriodeDu->setText(m_isFrench ? "Période Du:" : "Period From:");
    ui->lblPeriodeAu->setText(m_isFrench ? "Au:" : "To:");
    ui->btnFiltrer->setText(m_isFrench ? "⚙ Filtrer" : "⚙ Filter");
    ui->btnActualiserGlobal->setToolTip(
      m_isFrench ? "Actualiser et réinitialiser les filtres"
           : "Refresh and reset filters");

    ui->lblRechercheRapide->setText(m_isFrench ? "Recherche:" : "Search:");
    ui->recherche_nom_machine->setPlaceholderText(
      m_isFrench ? "Rechercher par nom..." : "Search by name...");
    ui->btnRechercher->setToolTip(m_isFrench ? "Rechercher" : "Search");
    ui->btnModifierMachine_machine->setToolTip(
      m_isFrench ? "Modifier la machine sélectionnée"
           : "Edit selected machine");
    ui->btnSupprimerMachine_machine->setToolTip(
      m_isFrench ? "Supprimer la machine sélectionnée"
           : "Delete selected machine");
    ui->btnToggleOnOff_machine->setToolTip(
      m_isFrench ? "Allumer / Éteindre la machine"
           : "Power ON / OFF machine");
    ui->btnSetVeille_machine->setToolTip(
      m_isFrench ? "Mettre la machine en veille"
           : "Set machine to standby");
    ui->btnCarteMachine->setToolTip(
      m_isFrench ? "Fiche machine" : "Machine card");
    ui->btnHistoriqueToggle_machine->setText(
      m_isFrench ? "📜 Historique" : "📜 History");
    ui->btnExporter_machine->setText(m_isFrench ? "📤 Exporter" : "📤 Export");
    ui->lblHistoriqueTitle->setText(m_isFrench ? "Historique ON / OFF des machines"
                         : "Machines ON / OFF history");

    ui->pushButton_enregistrer_machine->setText(
      m_isFrench ? "💾 Enregistrer" : "💾 Save");
    ui->btnReinitialiser_machine->setText(
      m_isFrench ? "🔄 Réinitialiser" : "🔄 Reset");
    ui->groupEmployeesActions->setTitle(
      m_isFrench ? "Employés enregistrés" : "Registered employees");
    ui->labelEmployeesHint->setText(
      m_isFrench ? "Liste synchronisée depuis la table EMPLOYEES"
           : "List synchronized from EMPLOYEES table");

    ui->groupOptionsStats->setTitle(
      m_isFrench ? "Options de visualisation" : "Visualization options");
    ui->lblTypeStats->setText(
      m_isFrench ? "Type de statistique:" : "Statistics type:");
    ui->lblTypeGraphique->setText(
      m_isFrench ? "Type de graphique:" : "Chart type:");
    ui->btnGenererStats_machine->setText(
      m_isFrench ? "📊 Générer" : "📊 Generate");
    ui->groupComparaisonStats->setTitle(
      m_isFrench ? "Comparaison des machines" : "Machine comparison");
    ui->groupSelectionAIMachine->setTitle(
      m_isFrench ? "Sélection de la machine" : "Machine selection");
    ui->lblSelectionMachineIA->setText(
      m_isFrench ? "Machine:" : "Machine:");
    ui->btnGenererQrMachine->setText(
      m_isFrench ? "▦ Générer QR" : "▦ Generate QR");
    ui->btnGenererQrMachine->setToolTip(
      m_isFrench ? "Générer le QR code de la machine sélectionnée"
                 : "Generate the selected machine QR code");
    if (ui->lblQrPreviewMachine->pixmap(Qt::ReturnByValue).isNull()) {
      resetQrPreviewLabel();
    }
    ui->groupAIDetails->setTitle(
      m_isFrench ? "Carnet / Arduino" : "Journal / Arduino");
    ui->tabWidgetAIDetails->setTabText(
      0, m_isFrench ? "Carnet / Journal" : "Journal");
    ui->tabWidgetAIDetails->setTabText(
      1, "Arduino");

    if (statsComparisonModel) {
    statsComparisonModel->setHorizontalHeaderLabels(
      m_isFrench
        ? QStringList{"Segment", "Rang urgence", "Machine", "Type", "Dernière maintenance", "Tag", "Priorité", "Analyse"}
        : QStringList{"Segment", "Urgency rank", "Machine", "Type", "Last maintenance", "Tag", "Priority", "Analysis"});
    }

    const auto replaceComboItems = [](QComboBox *combo,
                    const QStringList &items) {
    if (!combo || combo->count() != items.size()) {
      return;
    }
    for (int i = 0; i < items.size(); ++i) {
      combo->setItemText(i, items.at(i));
    }
    };

    replaceComboItems(
      ui->filtre_type_machine,
      m_isFrench ? QStringList{"-- Type machine --", "Presse", "Broyeur",
                   "Malaxeur", "Chauffeur"}
           : QStringList{"-- Machine type --", "Press", "Crusher",
                   "Mixer", "Heater"});
    ui->filtre_type_machine->setItemData(1, "Presse");
    ui->filtre_type_machine->setItemData(2, "Broyeur");
    ui->filtre_type_machine->setItemData(3, "Malaxeur");
    ui->filtre_type_machine->setItemData(4, "Chauffeur");

    replaceComboItems(ui->filtre_etat_marche_machine,
            m_isFrench
              ? QStringList{"-- État marche --", "ON", "OFF", "VEILLE"}
              : QStringList{"-- Running state --", "ON", "OFF", "STANDBY"});
    ui->filtre_etat_marche_machine->setItemData(1, "ON");
    ui->filtre_etat_marche_machine->setItemData(2, "OFF");
    ui->filtre_etat_marche_machine->setItemData(3, "VEILLE");

    replaceComboItems(ui->filtre_type_alerte,
            m_isFrench ? QStringList{"-- Type alerte --", "Aucune",
                         "Température", "Surcharge", "Panne"}
                   : QStringList{"-- Alert type --", "None",
                         "Temperature", "Overload", "Failure"});
    ui->filtre_type_alerte->setItemData(1, "Aucune");
    ui->filtre_type_alerte->setItemData(2, "Température");
    ui->filtre_type_alerte->setItemData(3, "Surcharge");
    ui->filtre_type_alerte->setItemData(4, "Panne");

    replaceComboItems(ui->filtre_alerte_criticite,
            m_isFrench ? QStringList{"-- Criticité --", "Faible",
                         "Moyen", "Élevé", "Critique"}
                   : QStringList{"-- Severity --", "Low", "Medium",
                         "High", "Critical"});
    ui->filtre_alerte_criticite->setItemData(1, "Faible");
    ui->filtre_alerte_criticite->setItemData(2, "Moyen");
    ui->filtre_alerte_criticite->setItemData(3, "Élevé");
    ui->filtre_alerte_criticite->setItemData(4, "Critique");
    replaceComboItems(ui->tri_par,
            m_isFrench
              ? QStringList{"-- Trier par --", "nom_machine", "type_machine",
                      "etat_fonctionnement", "temperature_actuelle",
                      "date_derniere_maintenance", "niveau_criticite"}
              : QStringList{"-- Sort by --", "nom_machine", "type_machine",
                      "etat_fonctionnement", "temperature_actuelle",
                      "date_derniere_maintenance", "niveau_criticite"});

    const int statsTypeIndex = ui->type_statistique_machine->currentIndex();
    ui->type_statistique_machine->clear();
    if (m_isFrench) {
    ui->type_statistique_machine->addItems(
      {"État fonctionnement", "Types alertes", "Température moyenne", "Type machine"});
    } else {
    ui->type_statistique_machine->addItems(
      {"Operating state", "Alert types", "Average temperature", "Machine type"});
    }
    ui->type_statistique_machine->setCurrentIndex(
      qBound(0, statsTypeIndex, ui->type_statistique_machine->count() - 1));

    const int chartTypeIndex = ui->type_graphique_machine->currentIndex();
    ui->type_graphique_machine->clear();
    if (m_isFrench) {
    ui->type_graphique_machine->addItems({"Histogramme (barres)", "Camembert", "Courbe"});
    } else {
    ui->type_graphique_machine->addItems({"Bar chart", "Pie chart", "Line chart"});
    }
    ui->type_graphique_machine->setCurrentIndex(
      qBound(0, chartTypeIndex, ui->type_graphique_machine->count() - 1));

    // Clarify chart choices to avoid ambiguity.
    ui->type_graphique_machine->setItemData(
      0,
      m_isFrench ? "Colonnes verticales pour comparer les valeurs"
                 : "Vertical columns to compare values",
      Qt::ToolTipRole);
    ui->type_graphique_machine->setItemData(
      1,
      m_isFrench ? "Parts d'un total en pourcentage"
                 : "Parts of a whole in percentage",
      Qt::ToolTipRole);
    ui->type_graphique_machine->setItemData(
      2,
      m_isFrench ? "Évolution ou tendance entre catégories"
                 : "Evolution or trend across categories",
      Qt::ToolTipRole);

    refreshResponsableFilterOptions();

    clearStatsChartArea(
      m_isFrench
        ? "Sélectionnez un type de statistique et un type de graphique, puis cliquez sur Générer."
        : "Select a statistics type and a chart type, then click Generate.");

    refreshTodoList();

  refreshNavigationTabs();

  ui->tabWidgetMachineManagement->setTabText(
      0, m_isFrench ? "🏭 Parc machines" : "🏭 Machine fleet");
  ui->tabWidgetMachineManagement->setTabText(
      1, m_isFrench ? "⚙️ Actions" : "⚙️ Actions");
  ui->tabWidgetMachineManagement->setTabText(
      2, m_isFrench ? "📊 Statistiques" : "📊 Statistics");
  ui->tabWidgetMachineManagement->setTabText(
      3, m_isFrench ? "📘 Carnet IA" : "📘 AI Logbook");

  updateAICarnetForSelectedMachine();
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
  refreshNavigationTabs();

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

void machine::refreshNavigationTabs() {
  if (!navigationBar) {
    return;
  }

  const int activeIndex = ui->tabWidgetMachineManagement->currentIndex();
  QSignalBlocker blocker(navigationBar);
  navigationBar->clear();

  if (m_isFrench) {
    navigationBar->addTab("🏭", "Parc machines");
    navigationBar->addTab("⚙️", "Actions");
    navigationBar->addTab("📊", "Statistiques");
    navigationBar->addTab("📘", "Carnet IA");
  } else {
    navigationBar->addTab("🏭", "Machine fleet");
    navigationBar->addTab("⚙️", "Actions");
    navigationBar->addTab("📊", "Statistics");
    navigationBar->addTab("📘", "AI Logbook");
  }

  navigationBar->setActiveTab(activeIndex < 0 ? 0 : activeIndex);
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

void machine::setupStatsComparisonTable() {
  statsComparisonModel = new QStandardItemModel(0, 8, this);
  statsComparisonModel->setHorizontalHeaderLabels(
      {"Segment", "Rang urgence", "Machine", "Type", "Dernière maintenance", "Tag", "Priorité", "Analyse"});

  ui->tableComparaisonStats_machine->setModel(statsComparisonModel);
  ui->tableComparaisonStats_machine->setAlternatingRowColors(true);
  ui->tableComparaisonStats_machine->setSelectionBehavior(QAbstractItemView::SelectRows);
  ui->tableComparaisonStats_machine->setSelectionMode(QAbstractItemView::SingleSelection);
  ui->tableComparaisonStats_machine->setEditTriggers(QAbstractItemView::NoEditTriggers);
  ui->tableComparaisonStats_machine->verticalHeader()->setVisible(false);
  ui->tableComparaisonStats_machine->horizontalHeader()->setStretchLastSection(true);
  ui->tableComparaisonStats_machine->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
}

void machine::normalizeLowPriorityTags() {
  QSqlQuery query(ConnectionMachine::getInstance().getDatabase());
  query.prepare(
      "UPDATE MACHINE "
      "SET TAG = '-' "
      "WHERE (TAG IS NULL OR TRIM(TAG) = '') "
      "AND ("
      "LOWER(TRIM(NVL(PRIORITE, ''))) LIKE '%basse%' "
      "OR LOWER(TRIM(NVL(PRIORITE, ''))) LIKE '%faible%' "
      "OR LOWER(TRIM(NVL(PRIORITE, ''))) LIKE '%low%'"
      ")");

  if (!query.exec()) {
    qDebug() << "Erreur normalisation TAG/priorité :" << query.lastError().text();
  }
}

void machine::populateStatsComparisonTable() {
  if (!statsComparisonModel) {
    return;
  }

  statsComparisonModel->setRowCount(0);

  struct EvalRow {
    QString machine;
    QString type;
    QString maintenance;
    QString tag;
    QString priorite;
    QString segment;
    QString urgence;
    QString analyse;
    int score = 0;
    int ageDays = 0;
  };

  if (m_allMachines.isEmpty()) {
    return;
  }

  const QDate today = QDate::currentDate();
  QMap<QString, int> maxAgeByType;

  auto normalized = [](const QString &v) -> QString {
    return v.trimmed().toLower();
  };

  auto maintenanceAgeDays = [&](const QDate &d) -> int {
    if (!d.isValid()) {
      return 9999;
    }
    return qMax(0, d.daysTo(today));
  };

  auto isTagCritical = [&](const QString &tag) -> bool {
    const QString t = normalized(tag);
    return t.contains("anomal") || t.contains("crit") || t.contains("panne") ||
           t.contains("urgent") || t.contains("recurrent") || t.contains("récurrent") ||
           t.contains("surcharge") || t.contains("defaut") || t.contains("défaut");
  };

  auto isTagNeutral = [&](const QString &tag) -> bool {
    const QString t = normalized(tag);
    return t.isEmpty() || t == "-" || t == "ok" || t == "normal" ||
           t == "aucune" || t == "none" || t == "neutre";
  };

  auto isPriorityHigh = [&](const QString &priorite) -> bool {
    const QString p = normalized(priorite);
    return p.contains("haute") || p.contains("élev") || p.contains("eleve") ||
           p.contains("crit") || p.contains("urgent") || p.contains("high");
  };

  auto isPriorityLow = [&](const QString &priorite) -> bool {
    const QString p = normalized(priorite);
    return p.isEmpty() || p == "-" || p.contains("basse") || p.contains("faible") ||
           p.contains("low") || p.contains("normale") || p.contains("normal");
  };

  for (const MachineData &m : m_allMachines) {
    const QString type = m.type.trimmed().isEmpty() ? QString("Non défini") : m.type.trimmed();
    const int age = maintenanceAgeDays(m.maintenance);
    maxAgeByType[type] = qMax(maxAgeByType.value(type, 0), age);
  }

  QVector<EvalRow> urgenceRows;
  QVector<EvalRow> performerRows;

  for (const MachineData &m : m_allMachines) {
    EvalRow row;
    row.machine = QString("%1 (%2)").arg(m.nom.trimmed().isEmpty() ? QString("Machine") : m.nom.trimmed(), m.id);
    row.type = m.type.trimmed().isEmpty() ? QString("Non défini") : m.type.trimmed();
    row.maintenance = m.maintenance.isValid() ? m.maintenance.toString("yyyy-MM-dd") : "-";
    row.tag = m.tag.trimmed().isEmpty() ? "-" : m.tag.trimmed();
    row.priorite = m.priorite.trimmed().isEmpty() ? "-" : m.priorite.trimmed();
    row.ageDays = maintenanceAgeDays(m.maintenance);

    const bool oldestInType = (row.ageDays >= maxAgeByType.value(row.type));
    const bool tagCritical = isTagCritical(row.tag);
    const bool highPriority = isPriorityHigh(row.priorite);

    row.score = 0;
    QStringList reasons;

    if (oldestInType) {
      row.score += 3;
      reasons << (m_isFrench ? "Plus ancienne maintenance du type"
                             : "Oldest maintenance in this type");
    }
    if (row.ageDays > 180) {
      row.score += 2;
      reasons << (m_isFrench ? "Maintenance très ancienne (>180j)"
                             : "Very old maintenance (>180d)");
    } else if (row.ageDays > 90) {
      row.score += 1;
      reasons << (m_isFrench ? "Maintenance ancienne (>90j)"
                             : "Old maintenance (>90d)");
    }
    if (tagCritical) {
      row.score += 3;
      reasons << (m_isFrench ? "TAG signale anomalie/criticité"
                             : "TAG indicates anomaly/critical issue");
    }
    if (highPriority) {
      row.score += 4;
      reasons << (m_isFrench ? "Priorité élevée/critique"
                             : "High/critical priority");
    }

    if (row.score >= 8) {
      row.urgence = m_isFrench ? "Critique" : "Critical";
    } else if (row.score >= 5) {
      row.urgence = m_isFrench ? "Élevée" : "High";
    } else if (row.score >= 3) {
      row.urgence = m_isFrench ? "Moyenne" : "Medium";
    } else {
      row.urgence = m_isFrench ? "Faible" : "Low";
    }

    row.segment = m_isFrench ? "Urgence" : "Urgency";
    row.analyse = reasons.isEmpty()
                      ? (m_isFrench ? "RAS" : "No major risk")
                      : reasons.join(" • ");
    urgenceRows.append(row);

    const bool recentMaintenance = row.ageDays <= 30;
    const bool neutralTag = isTagNeutral(row.tag);
    const bool lowPriority = isPriorityLow(row.priorite);
    if (recentMaintenance && neutralTag && lowPriority) {
      EvalRow best = row;
      best.segment = m_isFrench ? "Best performer" : "Best performer";
      best.urgence = "-";
      best.analyse = m_isFrench
                         ? "Maintenance récente + TAG neutre + priorité basse"
                         : "Recent maintenance + neutral TAG + low priority";
      performerRows.append(best);
    }
  }

  std::sort(urgenceRows.begin(), urgenceRows.end(), [](const EvalRow &a, const EvalRow &b) {
    if (a.score != b.score) {
      return a.score > b.score;
    }
    return a.ageDays > b.ageDays;
  });

  std::sort(performerRows.begin(), performerRows.end(), [](const EvalRow &a, const EvalRow &b) {
    return a.ageDays < b.ageDays;
  });

  auto appendRow = [&](const EvalRow &r, int rank) {
    QList<QStandardItem *> items;
    items << new QStandardItem(r.segment)
          << new QStandardItem(r.segment == (m_isFrench ? "Urgence" : "Urgency") ? QString::number(rank) + " - " + r.urgence : "-")
          << new QStandardItem(r.machine)
          << new QStandardItem(r.type)
          << new QStandardItem(r.maintenance)
          << new QStandardItem(r.tag)
          << new QStandardItem(r.priorite)
          << new QStandardItem(r.analyse);

    QColor fg = m_isDarkMode ? QColor("#EAF0F8") : QColor("#2B2B2B");
    QColor bg = Qt::transparent;
    if (r.segment == (m_isFrench ? "Best performer" : "Best performer")) {
      bg = m_isDarkMode ? QColor("#163126") : QColor("#E8F5E9");
      fg = m_isDarkMode ? QColor("#8BE2B4") : QColor("#1B5E20");
    } else if (r.urgence == (m_isFrench ? "Critique" : "Critical")) {
      bg = m_isDarkMode ? QColor("#3B1F25") : QColor("#FFEBEE");
      fg = m_isDarkMode ? QColor("#FF9EA8") : QColor("#B71C1C");
    } else if (r.urgence == (m_isFrench ? "Élevée" : "High")) {
      bg = m_isDarkMode ? QColor("#3A2A1A") : QColor("#FFF3E0");
      fg = m_isDarkMode ? QColor("#FFC98A") : QColor("#E65100");
    }

    for (int i = 0; i < items.size(); ++i) {
      items[i]->setEditable(false);
      items[i]->setTextAlignment(i >= 2 ? Qt::AlignLeft | Qt::AlignVCenter : Qt::AlignCenter);
      items[i]->setForeground(fg);
      if (bg != Qt::transparent) {
        items[i]->setBackground(bg);
      }
    }
    statsComparisonModel->appendRow(items);
  };

  int rank = 1;
  for (const EvalRow &r : urgenceRows) {
    appendRow(r, rank++);
  }
  for (const EvalRow &r : performerRows) {
    appendRow(r, -1);
  }
}

void machine::refreshAIMachineSelector() {
  if (!ui->comboDecisionMachine) {
    return;
  }

  const QString previousId = ui->comboDecisionMachine->currentData().toString();
  QSignalBlocker blocker(ui->comboDecisionMachine);
  ui->comboDecisionMachine->clear();

  ui->comboDecisionMachine->addItem(m_isFrench ? "Aucune" : "None", QString());

  for (const MachineData &m : m_allMachines) {
    const QString machineName = m.nom.trimmed().isEmpty() ? QString("Machine") : m.nom.trimmed();
    const QString machineType = m.type.trimmed().isEmpty() ? QString("Non défini") : m.type.trimmed();
    ui->comboDecisionMachine->addItem(
        QString("%1 (%2)").arg(machineName, machineType), m.id);
  }

  const int previousIndex = ui->comboDecisionMachine->findData(previousId);
  if (previousIndex >= 0) {
    ui->comboDecisionMachine->setCurrentIndex(previousIndex);
  } else if (ui->comboDecisionMachine->count() > 0) {
    ui->comboDecisionMachine->setCurrentIndex(0);
  }

  updateAICarnetForSelectedMachine();
}

QString machine::resolveLocalIpv4() const {
  const QList<QHostAddress> addresses = QNetworkInterface::allAddresses();
  for (const QHostAddress &address : addresses) {
    if (address.protocol() != QAbstractSocket::IPv4Protocol) {
      continue;
    }
    if (address.isLoopback()) {
      continue;
    }
    const QString ip = address.toString();
    if (!ip.startsWith("169.254.")) {
      return ip;
    }
  }
  return "127.0.0.1";
}

QPixmap machine::generateMachineQrPixmap(const QString &payload,
                                         int moduleSize,
                                         int margin) const {
  if (payload.trimmed().isEmpty()) {
    return QPixmap();
  }

#ifdef MACHINE_USE_QRENCODE
  QRcode *qr = QRcode_encodeString(payload.toUtf8().constData(), 0,
                                   QR_ECLEVEL_M, QR_MODE_8, 1);
  if (!qr || qr->width <= 0 || !qr->data) {
    if (qr) {
      QRcode_free(qr);
    }
    return QPixmap();
  }

  const int qrWidth = qr->width;
  const int pixelSize = (qrWidth + margin * 2) * moduleSize;

  QImage image(pixelSize, pixelSize, QImage::Format_RGB32);
  image.fill(Qt::white);

  QPainter painter(&image);
  painter.setPen(Qt::NoPen);
  painter.setBrush(Qt::black);

  for (int y = 0; y < qrWidth; ++y) {
    for (int x = 0; x < qrWidth; ++x) {
      const unsigned char value = qr->data[y * qrWidth + x];
      if (value & 0x1) {
        const int px = (x + margin) * moduleSize;
        const int py = (y + margin) * moduleSize;
        painter.drawRect(px, py, moduleSize, moduleSize);
      }
    }
  }

  QRcode_free(qr);
  return QPixmap::fromImage(image);
#else
  Q_UNUSED(moduleSize);

  const int side = 200;
  const QString encodedData = QString::fromUtf8(QUrl::toPercentEncoding(payload));
  const QUrl qrServiceUrl(QString("https://api.qrserver.com/v1/create-qr-code/?size=%1x%1&margin=%2&data=%3")
                              .arg(side)
                              .arg(qMax(0, margin))
                              .arg(encodedData));

  QNetworkAccessManager manager;
  QNetworkRequest request(qrServiceUrl);
  QNetworkReply *reply = manager.get(request);

  QEventLoop loop;
  QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
  loop.exec();

  QPixmap pixmap;
  if (reply->error() == QNetworkReply::NoError) {
    pixmap.loadFromData(reply->readAll());
  }
  reply->deleteLater();

  return pixmap;
#endif
}

void machine::resetQrPreviewLabel() {
  if (!ui || !ui->lblQrPreviewMachine) {
    return;
  }

  ui->lblQrPreviewMachine->setPixmap(QPixmap());
  ui->lblQrPreviewMachine->setText(m_isFrench ? "QR\nen attente" : "QR\nwaiting");
  ui->lblQrPreviewMachine->setProperty("qrUrl", QString());
  ui->lblQrPreviewMachine->setProperty("machineId", QString());
  ui->lblQrPreviewMachine->setToolTip(
      m_isFrench ? "Générez un QR puis cliquez pour ouvrir la fiche"
                 : "Generate a QR then click to open machine sheet");
  ui->lblQrPreviewMachine->setStyleSheet(
      "QLabel { background-color: #FFFFFF; border: 1px solid #C5D3CC; border-radius: 6px; color: #2F5247; font-weight: 600; }");
}

void machine::on_btnGenererQrMachine_clicked() {
  const QString machineId = ui->comboDecisionMachine->currentData().toString();
  if (machineId.isEmpty()) {
    QMessageBox::warning(this,
                         m_isFrench ? "Machine requise" : "Machine required",
                         m_isFrench
                             ? "Veuillez choisir une machine avant de generer le QR code."
                             : "Please choose a machine before generating the QR code.");
    return;
  }

  if (!m_machineServer) {
    m_machineServer = new MachineServer(this);
    connect(m_machineServer, &MachineServer::machineRequested,
            this, &machine::onMachineRequestedFromHttp, Qt::UniqueConnection);
  }
  if (m_serverHostIp.isEmpty()) {
    m_serverHostIp = resolveLocalIpv4();
  }
  m_machineServer->setHostIp(m_serverHostIp);
  if (!m_machineServer->start(8181)) {
    QMessageBox::warning(this,
                         m_isFrench ? "Serveur HTTP" : "HTTP server",
                         m_isFrench
                             ? "Le serveur HTTP n'a pas pu demarrer sur le port 8181."
                             : "HTTP server could not start on port 8181.");
    return;
  }

  const QString targetUrl = m_machineServer->machineUrlForId(machineId);
  const QPixmap qrPixmap = generateMachineQrPixmap(targetUrl, 4, 2);
  if (qrPixmap.isNull()) {
    QMessageBox::critical(this,
                          m_isFrench ? "Erreur QR" : "QR error",
                          m_isFrench
                              ? "Impossible de generer le QR code."
                              : "Unable to generate the QR code.");
    return;
  }

  const QSize targetSize(140, 140);
  ui->lblQrPreviewMachine->setPixmap(
      qrPixmap.scaled(targetSize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
  ui->lblQrPreviewMachine->setText(QString());
  ui->lblQrPreviewMachine->setProperty("qrUrl", targetUrl);
  ui->lblQrPreviewMachine->setProperty("machineId", machineId);
  ui->lblQrPreviewMachine->setToolTip(
      m_isFrench
          ? QString("%1\n(Cliquez pour ouvrir la fiche complète)").arg(targetUrl)
          : QString("%1\n(Click to open full machine sheet)").arg(targetUrl));
}

void machine::onMachineRequestedFromHttp(const QString &machineId) {
  if (machineId.trimmed().isEmpty()) {
    return;
  }

  m_selectedMachineId = machineId.trimmed();

  if (!selectMachineInTableById(machineId)) {
    chargerMachines();
    if (!selectMachineInTableById(machineId)) {
      m_selectedRow = -1;
    }
  }

  raise();
  activateWindow();

  if (navigationBar) {
    navigationBar->setActiveTab(0);
  }
}

bool machine::selectMachineInTableById(const QString &machineId) {
  if (!machineTableModel || !ui->tableMachines_machine) {
    return false;
  }

  const QString wanted = machineId.trimmed();
  if (wanted.isEmpty()) {
    return false;
  }

  for (int row = 0; row < machineTableModel->rowCount(); ++row) {
    QStandardItem *idItem = machineTableModel->item(row, 0);
    if (!idItem || idItem->text().trimmed() != wanted) {
      continue;
    }

    m_selectedMachineId = wanted;
    m_selectedRow = row;

    ui->tableMachines_machine->selectRow(row);
    ui->tableMachines_machine->scrollTo(machineTableModel->index(row, 0));

    ui->btnModifierMachine_machine->setEnabled(true);
    ui->btnSupprimerMachine_machine->setEnabled(true);
    ui->btnExporter_machine->setEnabled(true);
    ui->btnToggleOnOff_machine->setEnabled(true);
    ui->btnSetVeille_machine->setEnabled(true);
    ui->btnCarteMachine->setEnabled(true);

    const QString etatMarche = machineTableModel->item(row, 3)
                                   ? machineTableModel->item(row, 3)->text()
                                   : QString();
    ui->btnToggleOnOff_machine->setStyleSheet(onOffButtonStyle(etatMarche));
    ui->btnSetVeille_machine->setStyleSheet(
        veilleButtonStyle(etatMarche.trimmed().toUpper() == "VEILLE"));
    return true;
  }

  return false;
}

void machine::resizeEvent(QResizeEvent *event) {
  QMainWindow::resizeEvent(event);
  if (m_chatbot) {
    int w = m_chatbot->width();
    int h = height();
    if (m_chatbot->isVisible()) {
      m_chatbot->setGeometry(width() - w, 0, w, h);
    } else {
      m_chatbot->setGeometry(width(), 0, w, h);
    }
  }
}

void machine::updateAICarnetForSelectedMachine() {
  if (!ui->tableCarnetMachine_machine) {
    return;
  }

  ui->tableCarnetMachine_machine->clearContents();
  ui->tableCarnetMachine_machine->setColumnCount(2);
  ui->tableCarnetMachine_machine->setHorizontalHeaderLabels(
      m_isFrench ? QStringList{"Date & heure", "Carnet de vie"}
                 : QStringList{"Date & time", "Life journal"});
  ui->tableCarnetMachine_machine->setRowCount(0);

  const QString selectedId = ui->comboDecisionMachine->currentData().toString();
  if (selectedId.isEmpty()) {
    ui->tableCarnetMachine_machine->setRowCount(1);
    ui->tableCarnetMachine_machine->setItem(0, 0, new QTableWidgetItem("-"));
    ui->tableCarnetMachine_machine->setItem(
        0, 1,
        new QTableWidgetItem(m_isFrench
                                 ? "Sélectionnez une machine pour afficher son carnet de vie."
                                 : "Select a machine to display its life journal."));
    return;
  }

  MachineData selectedMachine;
  bool found = false;
  for (const MachineData &m : m_allMachines) {
    if (m.id == selectedId) {
      selectedMachine = m;
      found = true;
      break;
    }
  }

  if (!found) {
    return;
  }

  QDateTime baseTime = QDateTime::currentDateTime();
  QSqlQuery query(ConnectionMachine::getInstance().getDatabase());
  query.prepare("SELECT DATE_MISE_A_JOUR FROM MACHINE WHERE ID_MACHINE = :id");
  query.bindValue(":id", selectedId);
  if (query.exec() && query.next()) {
    const QDateTime dbDateTime = query.value(0).toDateTime();
    if (dbDateTime.isValid()) {
      baseTime = dbDateTime;
    }
  }

  QList<QPair<QDateTime, QString>> events;
  const QString machineName = selectedMachine.nom.trimmed().isEmpty() ? QString("Machine") : selectedMachine.nom.trimmed();
  const QString machineType = selectedMachine.type.trimmed().isEmpty() ? QString("Non défini") : selectedMachine.type.trimmed();

  events.append(qMakePair(
      baseTime.addSecs(-900),
      m_isFrench
          ? QString("Machine %1 de type %2 prise en suivi dans le carnet de vie.")
                .arg(machineName, machineType)
          : QString("Machine %1 (%2) added to life journal monitoring.")
                .arg(machineName, machineType)));

  const QString etatMarche = selectedMachine.etatMarche.trimmed().toUpper();
  if (etatMarche == "ON") {
    events.append(qMakePair(baseTime.addSecs(-780),
                            m_isFrench ? "Passage en marche: machine allumée."
                                       : "Switched to ON state."));
  } else if (etatMarche == "OFF") {
    events.append(qMakePair(baseTime.addSecs(-780),
                            m_isFrench ? "Arrêt de la machine confirmé."
                                       : "Machine was stopped."));
  } else if (etatMarche == "VEILLE") {
    events.append(qMakePair(baseTime.addSecs(-780),
                            m_isFrench ? "Machine passée en mode veille."
                                       : "Machine switched to standby."));
  }

  const QString alerte = selectedMachine.alerte.trimmed();
  if (!alerte.isEmpty() && alerte != "Aucune" && alerte != "None" && alerte != "-") {
    events.append(qMakePair(
        baseTime.addSecs(-540),
        m_isFrench ? QString("Alerte détectée: %1.").arg(alerte)
                   : QString("Alert detected: %1.").arg(alerte)));
  }

  const QString etatFonct = selectedMachine.fonctionnement.trimmed();
  if (etatFonct == "Panne") {
    events.append(qMakePair(baseTime.addSecs(-420),
                            m_isFrench ? "Machine passée en panne, intervention recommandée."
                                       : "Machine entered failure state, intervention recommended."));
  } else if (etatFonct == "Alerte") {
    events.append(qMakePair(baseTime.addSecs(-420),
                            m_isFrench ? "Comportement instable observé, surveillance renforcée."
                                       : "Unstable behavior observed, reinforced monitoring."));
  } else {
    events.append(qMakePair(baseTime.addSecs(-420),
                            m_isFrench ? "Retour progressif à un fonctionnement normal."
                                       : "Progressive return to normal operation."));
  }

  if (selectedMachine.temperature >= 80.0) {
    events.append(qMakePair(
        baseTime.addSecs(-240),
        m_isFrench
            ? QString("Machine surchauffée puis refroidie en 4 min (%.1f °C max).")
                  .arg(selectedMachine.temperature)
            : QString("Overheat detected then cooled down in 4 min (%.1f °C peak).")
                  .arg(selectedMachine.temperature)));
  } else if (selectedMachine.temperature >= 65.0) {
    events.append(qMakePair(
        baseTime.addSecs(-240),
        m_isFrench
            ? QString("Hausse thermique surveillée: %.1f °C.").arg(selectedMachine.temperature)
            : QString("Thermal increase under monitoring: %.1f °C.").arg(selectedMachine.temperature)));
  } else {
    events.append(qMakePair(
        baseTime.addSecs(-240),
        m_isFrench
            ? QString("Température stable autour de %.1f °C.").arg(selectedMachine.temperature)
            : QString("Temperature remained stable around %.1f °C.").arg(selectedMachine.temperature)));
  }

  const QString maintText = selectedMachine.maintenance.isValid()
                                ? selectedMachine.maintenance.toString("dd/MM/yyyy")
                                : (m_isFrench ? "date non définie" : "undefined date");
  events.append(qMakePair(
      baseTime.addSecs(-120),
      m_isFrench
          ? QString("Dernière maintenance enregistrée le %1.").arg(maintText)
          : QString("Last maintenance recorded on %1.").arg(maintText)));

  events.append(qMakePair(
      baseTime,
      m_isFrench ? "Mise à jour des indicateurs machine et clôture du cycle courant."
                 : "Machine indicators updated and current cycle closed."));

  std::sort(events.begin(), events.end(),
            [](const QPair<QDateTime, QString> &a, const QPair<QDateTime, QString> &b) {
              return a.first < b.first;
            });

  ui->tableCarnetMachine_machine->setRowCount(events.size());
  const QColor carnetTextColor = m_isDarkMode ? QColor("#EAF0F8") : QColor("#1B2F2A");
  for (int i = 0; i < events.size(); ++i) {
    auto *timeItem = new QTableWidgetItem(events[i].first.toString("dd/MM/yyyy HH:mm"));
    auto *eventItem = new QTableWidgetItem(events[i].second);
    timeItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    eventItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    timeItem->setForeground(QBrush(carnetTextColor));
    eventItem->setForeground(QBrush(carnetTextColor));
    timeItem->setTextAlignment(Qt::AlignCenter);
    eventItem->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    ui->tableCarnetMachine_machine->setItem(i, 0, timeItem);
    ui->tableCarnetMachine_machine->setItem(i, 1, eventItem);
    ui->tableCarnetMachine_machine->setRowHeight(i, 38);
  }
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
  connect(ui->btnSummaryActualiser_machine, &QPushButton::clicked, this,
          [this]() { chargerMachines(); });

  // Liste alimentee automatiquement depuis la base de donnees (TAG/PRIORITE)
  ui->btnAjouterTodo_machine->setVisible(false);

  reloadTodoFromMachineData();
}

void machine::reloadTodoFromMachineData() {
  m_todoItems.clear();

  for (const MachineData &m : m_allMachines) {
    const QString rawPriority = m.priorite.trimmed();
    const QString rawTag = m.tag.trimmed();

    const bool hasPriority = (priorityScore(rawPriority) >= 2);
    const bool hasPriorityTag = tagImpliesPriority(rawTag);
    if (!hasPriority && !hasPriorityTag) {
      continue;
    }

    TodoItem item;
    item.machineId = m.id;
    item.machineName = m.nom;
    item.machineType = m.type;
    if (isLowPriorityLabel(rawPriority)) {
      item.tag = "-";
    } else {
      item.tag = (rawTag.isEmpty() || rawTag == "-") ? "-" : rawTag;
    }
    item.priority = (rawPriority.isEmpty() || rawPriority == "-")
                        ? "A definir"
                        : rawPriority;
    m_todoItems.append(item);
  }

  std::sort(m_todoItems.begin(), m_todoItems.end(),
            [](const TodoItem &a, const TodoItem &b) {
              const int pa = priorityScore(a.priority);
              const int pb = priorityScore(b.priority);
              if (pa != pb) {
                return pa > pb;
              }
              return a.machineName.toLower() < b.machineName.toLower();
            });

  refreshTodoList();
}

QWidget *machine::createTodoItemWidget(int index) {
  const TodoItem &item = m_todoItems[index];

  QWidget *row = new QWidget();
  row->setObjectName("todoRow");

  const int score = priorityScore(item.priority);
  const QString leftAccent =
      (score >= 3) ? "#B42318" : ((score >= 2) ? "#EA580C" : "#1B4D3E");
  const QString rowBg = m_isDarkMode ? "#121C28" : "#FFFFFF";
  const QString rowBorder = m_isDarkMode ? "#2B3E54" : "#DDE5DF";
  row->setStyleSheet(QString("QWidget#todoRow { background-color: %1; "
                             "border: 1px solid %2; border-left: 5px solid %3; "
                             "border-radius: 9px; }")
                        .arg(rowBg, rowBorder, leftAccent));

  QHBoxLayout *rowLayout = new QHBoxLayout(row);
  rowLayout->setContentsMargins(8, 5, 8, 5);
  rowLayout->setSpacing(8);

  QVBoxLayout *infoLayout = new QVBoxLayout();
  infoLayout->setSpacing(1);
  infoLayout->setContentsMargins(0, 0, 0, 0);

  QLabel *lblName = new QLabel(item.machineName);
  lblName->setStyleSheet(QString("font-size: 12px; font-weight: 700; color: %1; "
                                 "background: transparent; border: none;")
                             .arg(m_isDarkMode ? "#E6EDF8" : "#143A2A"));

  QLabel *lblType = new QLabel(
      QString("%1: %2")
          .arg(m_isFrench ? "Type" : "Type")
          .arg(item.machineType));
    lblType->setStyleSheet(QString("font-size: 11px; color: %1; "
                   "background: transparent; border: none;")
                 .arg(m_isDarkMode ? "#9EB0C4" : "#5A6B62"));

  infoLayout->addWidget(lblName);
  infoLayout->addWidget(lblType);
  rowLayout->addLayout(infoLayout, 1);

  QLabel *badge = new QLabel(item.priority);
  if (score >= 3) {
    badge->setStyleSheet(
        "font-size: 10px; font-weight: 700; color: #FFFFFF; "
        "background-color: #B42318; border-radius: 10px; "
        "padding: 3px 10px; border: none;");
  } else if (score >= 2) {
    badge->setStyleSheet(
        "font-size: 10px; font-weight: 700; color: #FFFFFF; "
        "background-color: #EA580C; border-radius: 10px; "
        "padding: 3px 10px; border: none;");
  } else {
    badge->setStyleSheet(
        "font-size: 10px; font-weight: 700; color: #FFFFFF; "
        "background-color: #1B4D3E; border-radius: 10px; "
        "padding: 3px 10px; border: none;");
  }
  badge->setAlignment(Qt::AlignCenter);
  rowLayout->addWidget(badge);

  QLabel *tagBadge = new QLabel(item.tag);
  tagBadge->setAlignment(Qt::AlignCenter);
  tagBadge->setStyleSheet(QString(
      "font-size: 10px; font-weight: 600; color: %1; "
      "background-color: %2; border: 1px solid %3; "
      "border-radius: 10px; padding: 3px 10px;")
                              .arg(m_isDarkMode ? "#9EC5E3" : "#1B4D3E",
                                   m_isDarkMode ? "#1A2A3A" : "#EAF4EF",
                                   m_isDarkMode ? "#35506C" : "#C7DACF"));
  rowLayout->addWidget(tagBadge);

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

  // Create widgets
  for (int i = 0; i < m_todoItems.size(); ++i) {
    QWidget *w = createTodoItemWidget(i);
    ui->layoutTodoContent->addWidget(w);
  }

  if (m_todoItems.isEmpty()) {
    QLabel *emptyLabel = new QLabel(
        m_isFrench ? "Aucune machine prioritaire pour le moment."
                   : "No priority machine at the moment.");
    emptyLabel->setAlignment(Qt::AlignCenter);
    emptyLabel->setMinimumHeight(56);
    emptyLabel->setStyleSheet(QString(
      "font-size: 12px; color: %1; background-color: %2; "
      "border: 1px dashed %3; border-radius: 8px; padding: 10px;")
                    .arg(m_isDarkMode ? "#B8C7D8" : "#6F7D75",
                       m_isDarkMode ? "#111A25" : "#F7FAF8",
                       m_isDarkMode ? "#3A4F67" : "#C7D6CD"));
    ui->layoutTodoContent->addWidget(emptyLabel);
  }

  // Add spacer at bottom to push items up
  ui->layoutTodoContent->addItem(
      new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding));

  // Update counter in title
  int pending = m_todoItems.size();
  ui->lblTodoTitle->setText(
      m_isFrench ? QString("🔧 Machines à réparer (%1)").arg(pending)
                 : QString("🔧 Machines to repair (%1)").arg(pending));

  if (ui->scrollTodoList_machine && ui->scrollTodoList_machine->verticalScrollBar()) {
    ui->scrollTodoList_machine->verticalScrollBar()->setValue(0);
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
    QMessageBox::critical(this, m_isFrench ? "Erreur" : "Error",
                          (m_isFrench
                               ? "Échec de l'ajout dans la base de données : "
                               : "Failed to insert into database: ") +
                              errorMessage);
    return false;
  }

  QMessageBox msgBox(this);
  msgBox.setWindowTitle(m_isFrench ? "Succès" : "Success");
  if (m_selectedEmployeeId >= 0) {
    msgBox.setText(
        m_isFrench
            ? QString("✅ Machine ajoutée avec succès (ID employé responsable: %1) !")
                  .arg(m_selectedEmployeeId)
            : QString("✅ Machine added successfully (responsible employee ID: %1)!")
                  .arg(m_selectedEmployeeId));
  } else {
    msgBox.setText(m_isFrench ? "✅ Machine ajoutée avec succès !"
                              : "✅ Machine added successfully!");
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
    m.tag = query.value(15).toString().trimmed();
    m.priorite = query.value(16).toString().trimmed();
    if (isLowPriorityLabel(m.priorite)) {
      m.tag = "-";
    } else if (m.tag.isEmpty()) {
      m.tag = "-";
    }

    m_allMachines.append(m);
  }

  reloadTodoFromMachineData();

  refreshResponsableFilterOptions();

  // 3. Appliquer le filtrage initial avec les valeurs par défaut (tout afficher)
  appliquerFiltres();
  populateStatsComparisonTable();
  refreshAIMachineSelector();
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
  ui->filtre_responsable->addItem(
      m_isFrench ? "-- Responsable --" : "-- Responsible --");
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
  QString fType = ui->filtre_type_machine->currentData().toString();
  if (fType.isEmpty()) fType = ui->filtre_type_machine->currentText();
  QString fEtat = ui->filtre_etat_marche_machine->currentData().toString();
  if (fEtat.isEmpty()) fEtat = ui->filtre_etat_marche_machine->currentText();
  QString fAlerte = ui->filtre_type_alerte->currentData().toString();
  if (fAlerte.isEmpty()) fAlerte = ui->filtre_type_alerte->currentText();
  QString fCriticite = ui->filtre_alerte_criticite->currentData().toString();
  if (fCriticite.isEmpty()) fCriticite = ui->filtre_alerte_criticite->currentText();
  QString fResponsable = ui->filtre_responsable->currentText();
  
  QDate dateDu = ui->date_debut->date();
  QDate dateAu = ui->date_fin->date();
  QString triPar = ui->tri_par->currentText();

  const QColor premiumGreen = m_isDarkMode ? QColor("#6ED6A2") : QColor("#2E7D32");
  const QColor premiumOrange = m_isDarkMode ? QColor("#F3A35A") : QColor("#E65100");
  const QColor premiumRed = m_isDarkMode ? QColor("#EF6B77") : QColor("#C62828");
  const QColor baseText = m_isDarkMode ? QColor("#DCE4EF") : QColor("#333333");
  const QColor disabledBg = m_isDarkMode ? QColor("#1A2533") : QColor("#E8E8E8");
  const QColor disabledFg = m_isDarkMode ? QColor("#6F8197") : QColor("#999999");

  // 3. Filtrer la liste des machines avec une logique ET (AND)
  QList<MachineData> filteredMachines;
  for (const MachineData &m : m_allMachines) {
      if (ui->filtre_type_machine->currentIndex() != 0 && m.type != fType) continue;
      if (ui->filtre_etat_marche_machine->currentIndex() != 0 && m.etatMarche != fEtat) continue;
      if (ui->filtre_type_alerte->currentIndex() != 0 && m.alerte != fAlerte) continue;
      if (ui->filtre_alerte_criticite->currentIndex() != 0 && m.criticite != fCriticite) continue;
      if (ui->filtre_responsable->currentIndex() != 0 && m.responsable != fResponsable) continue;
      
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
      item->setForeground(baseText);
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
      const QColor bgGrise = disabledBg;
      const QColor fgBoldGrise = disabledFg;

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
      row[3]->setForeground(premiumGreen);
    else if (m.etatMarche == "VEILLE")
      row[3]->setForeground(premiumOrange);
    else if (m.etatMarche == "OFF")
      row[3]->setForeground(premiumRed);

    if (!machineInactive) {
      // -- Etat Fonctionnement (Colonne 6)
      row[6]->setFont(boldFont);
      if (m.fonctionnement == "Normal")
        row[6]->setForeground(premiumGreen);
      else if (m.fonctionnement == "Alerte")
        row[6]->setForeground(premiumOrange);
      else if (m.fonctionnement == "Panne")
        row[6]->setForeground(premiumRed);

      // -- Type Alerte (Colonne 7)
      row[7]->setFont(boldFont);
      if (m.alerte == "Aucune")
        row[7]->setForeground(premiumGreen);
      else if (m.alerte == "Panne")
        row[7]->setForeground(premiumOrange);
      else if (m.alerte == "Surcharge")
        row[7]->setForeground(premiumRed);

      // -- Niveau Criticité (Colonne 8)
      row[8]->setFont(boldFont);
      if (m.criticite == "Faible")
        row[8]->setForeground(premiumGreen);
      else if (m.criticite == QString::fromUtf8("Élevé"))
        row[8]->setForeground(premiumOrange);
      else if (m.criticite == "Critique")
        row[8]->setForeground(premiumRed);
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
      m_isDarkMode
        ? "QLabel { color: #C7D4E4; font-size: 13px; font-weight: 600; background-color: #0F1722; border: 1px dashed #3A4F67; border-radius: 10px; padding: 22px; }"
        : "QLabel { color: #4A4A4A; font-size: 13px; font-weight: 600; background-color: #F8FBFA; border: 1px dashed #9BB8AA; border-radius: 10px; padding: 22px; }");
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
    clearStatsChartArea(m_isFrench ? "Aucune donnée disponible pour cette statistique."
                                   : "No data available for this statistic.");
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
  chart->setBackgroundBrush(QColor(m_isDarkMode ? "#0F1722" : "#FFFFFF"));

  const QString canonStatsType =
      (statsType == "Average temperature") ? "Température moyenne"
      : (statsType == "Operating state") ? "État fonctionnement"
      : (statsType == "Alert types") ? "Types alertes"
      : (statsType == "Machine type") ? "Type machine"
      : statsType;

  const QString canonChartType =
      (chartType == "Barres" || chartType == "Histogramme (barres)" || chartType == "Bar chart") ? "BarChart"
      : (chartType == "Secteur" || chartType == "Camembert" || chartType == "Pie chart") ? "PieChart"
      : (chartType == "Ligne" || chartType == "Courbe" || chartType == "Line chart") ? "LineChart"
      : chartType;

  const bool isTemperatureStats = (canonStatsType == "Température moyenne");
  const bool showPercentages = !isTemperatureStats;

  double totalValue = 0.0;
  for (const auto &entry : data) {
    totalValue += entry.second;
  }

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
  if (canonStatsType == "État fonctionnement") {
    themeColor = QColor("#2E7D32");
  } else if (canonStatsType == "Types alertes") {
    themeColor = QColor("#E65100");
  } else if (canonStatsType == "Température moyenne") {
    themeColor = QColor("#0277BD");
  } else if (canonStatsType == "Type machine") {
    themeColor = QColor("#1565C0");
  }

  chart->setTitle(
      QString(m_isFrench ? "Statistique: %1" : "Statistic: %1").arg(statsType));
  chart->setTitleBrush(QBrush(themeColor));
  chart->legend()->setVisible(true);
  chart->legend()->setAlignment(Qt::AlignBottom);
  chart->legend()->setLabelColor(QColor(m_isDarkMode ? "#DCE4EF" : "#2B2B2B"));

  const QList<QColor> fallbackPalette = {
      QColor("#2E7D32"), QColor("#1565C0"), QColor("#6A1B9A"), QColor("#EF6C00"),
      QColor("#00897B"), QColor("#C62828"), QColor("#5D4037"), QColor("#455A64")};

  auto colorForLabel = [&](const QString &label, int index) -> QColor {
    const QString key = label.trimmed();
    if (canonStatsType == "État fonctionnement" && etatPalette.contains(key)) {
      return etatPalette.value(key);
    }
    if (canonStatsType == "Types alertes" && alertePalette.contains(key)) {
      return alertePalette.value(key);
    }
    if ((canonStatsType == "Type machine" || canonStatsType == "Température moyenne") &&
        typeMachinePalette.contains(key)) {
      return typeMachinePalette.value(key);
    }
    return fallbackPalette[index % fallbackPalette.size()];
  };

  auto percentageForValue = [&](double value) -> double {
    if (totalValue <= 0.0) {
      return 0.0;
    }
    return (value * 100.0) / totalValue;
  };

  auto valueText = [&](double value) -> QString {
    return isTemperatureStats
               ? QString::number(value, 'f', 2) + " °C"
               : QString::number(value, 'f', 0);
  };

  auto valueWithPercentageText = [&](double value) -> QString {
    if (!showPercentages) {
      return valueText(value);
    }
    return QString("%1 (%2%)")
        .arg(valueText(value))
        .arg(QString::number(percentageForValue(value), 'f', 1));
  };

  double maxValue = 0.0;
  for (const auto &entry : data) {
    if (entry.second > maxValue) {
      maxValue = entry.second;
    }
  }

  QString summaryHtml = QString("<div style='font-weight:700; margin-bottom:4px;'>%1</div>")
                            .arg(m_isFrench ? "Résumé" : "Summary");
  summaryHtml += QString("<div style='margin-bottom:6px;'>%1: <b>%2</b> • %3: <b>%4</b></div>")
                     .arg(m_isFrench ? "Catégories" : "Categories")
                     .arg(data.size())
                     .arg(m_isFrench ? "Total" : "Total")
                     .arg(valueText(totalValue));

  summaryHtml += "<table cellpadding='3' cellspacing='0' style='width:100%;'>";
  for (int i = 0; i < data.size(); ++i) {
    const auto &entry = data[i];
    const QColor itemColor = colorForLabel(entry.first, i);
    summaryHtml += QString("<tr><td><span style='color:%1; font-size:14px;'>●</span> %2</td><td align='right'><b>%3</b></td></tr>")
                       .arg(itemColor.name(), entry.first, valueWithPercentageText(entry.second));
  }
  summaryHtml += "</table>";

  QLabel *summaryLabel = new QLabel(summaryHtml, ui->zoneGraphique_machine);
  summaryLabel->setWordWrap(true);
  summaryLabel->setTextFormat(Qt::RichText);
  summaryLabel->setStyleSheet(
      m_isDarkMode
          ? "QLabel { color: #DCE4EF; background-color: #111B29; border: 1px solid #2E435A; border-radius: 8px; padding: 10px; font-size: 12px; }"
          : "QLabel { color: #2D3A37; background-color: #F6FBF9; border: 1px solid #CFE2D9; border-radius: 8px; padding: 10px; font-size: 12px; }");

  if (canonChartType == "PieChart") {
    auto *series = new QPieSeries();
    series->setPieSize(0.72);
    for (int i = 0; i < data.size(); ++i) {
      const auto &entry = data[i];
      QPieSlice *slice = series->append(entry.first, entry.second);
      slice->setLabel(QString("%1: %2")
                          .arg(entry.first)
                          .arg(valueWithPercentageText(entry.second)));
      slice->setLabelVisible(true);
      slice->setLabelPosition(QPieSlice::LabelOutside);
      slice->setLabelColor(QColor(m_isDarkMode ? "#EAF0F8" : "#1F1F1F"));
      slice->setBrush(colorForLabel(entry.first, i));
      slice->setPen(QPen(QColor("#FFFFFF"), 1));
    }
    chart->addSeries(series);
  } else if (canonChartType == "LineChart") {
    auto *series = new QLineSeries();
    series->setName(statsType);
    series->setColor(themeColor);
    series->setPointsVisible(true);
    series->setPointLabelsVisible(true);
    series->setPointLabelsFormat(isTemperatureStats ? "@yPoint °C" : "@yPoint");

    auto *axisX = new QCategoryAxis();
    axisX->setLabelsPosition(QCategoryAxis::AxisLabelsPositionOnValue);
    axisX->setLabelsAngle(-35);
    axisX->setLabelsColor(QColor(m_isDarkMode ? "#DCE4EF" : "#2B2B2B"));

    for (int i = 0; i < data.size(); ++i) {
      series->append(i, data[i].second);
      axisX->append(data[i].first, i);
    }

    chart->addSeries(series);
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);
    axisX->setRange(0, data.size() > 1 ? data.size() - 1 : 1);

    auto *axisY = new QValueAxis();
    axisY->setTitleText(
      isTemperatureStats ? (m_isFrench ? "Température moyenne (°C)"
                       : "Average temperature (°C)")
                 : (m_isFrench ? "Nombre de machines"
                       : "Number of machines"));
    axisY->setRange(0, maxValue > 0 ? maxValue * 1.2 : 10);
    axisY->setLabelFormat(isTemperatureStats ? "%.2f" : "%.0f");
    axisY->setLabelsColor(QColor(m_isDarkMode ? "#DCE4EF" : "#2B2B2B"));
    axisY->setGridLineColor(QColor(m_isDarkMode ? "#2B3E55" : "#DCE7E2"));
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);
  } else {
    auto *series = new QBarSeries();
    QStringList categories;

    for (int i = 0; i < data.size(); ++i) {
      categories << data[i].first;
    }

    for (int i = 0; i < data.size(); ++i) {
      auto *set = new QBarSet(data[i].first);
      for (int j = 0; j < data.size(); ++j) {
        *set << (i == j ? data[i].second : 0.0);
      }
      set->setColor(colorForLabel(data[i].first, i));
      set->setLabelColor(QColor(m_isDarkMode ? "#EAF0F8" : "#1F1F1F"));
      series->append(set);
    }

    series->setBarWidth(0.75);
    series->setLabelsVisible(false);
    chart->addSeries(series);

    auto *axisX = new QBarCategoryAxis();
    axisX->append(categories);
    axisX->setLabelsAngle(-30);
    axisX->setLabelsColor(QColor(m_isDarkMode ? "#DCE4EF" : "#2B2B2B"));
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    auto *axisY = new QValueAxis();
    axisY->setTitleText(
      isTemperatureStats ? (m_isFrench ? "Température moyenne (°C)"
                       : "Average temperature (°C)")
                 : (m_isFrench ? "Nombre de machines"
                       : "Number of machines"));
    axisY->setRange(0, maxValue > 0 ? maxValue * 1.2 : 10);
    axisY->setLabelFormat(isTemperatureStats ? "%.2f" : "%.0f");
    axisY->setLabelsColor(QColor(m_isDarkMode ? "#DCE4EF" : "#2B2B2B"));
    axisY->setGridLineColor(QColor(m_isDarkMode ? "#2B3E55" : "#DCE7E2"));
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);
  }

  auto *chartView = new QChartView(chart, ui->zoneGraphique_machine);
  chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setStyleSheet(
      m_isDarkMode
        ? "QChartView { background-color: #111B29; border: 1px solid #2E435A; border-radius: 8px; padding: 6px; }"
        : "QChartView { background-color: #FFFFFF; border: 1px solid #D7E5DF; border-radius: 8px; padding: 6px; }");

  QLabel *legendHelp = new QLabel(
      QString(m_isFrench ? "Affichage %1 • %2" : "Display %1 • %2")
        .arg(chartType, statsType),
      ui->zoneGraphique_machine);
  legendHelp->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
  legendHelp->setStyleSheet(
      m_isDarkMode
          ? "QLabel { color: #9FB2C6; font-size: 11px; font-weight: 600; }"
          : "QLabel { color: #47695D; font-size: 11px; font-weight: 600; }");

  auto *splitWidget = new QWidget(ui->zoneGraphique_machine);
  auto *splitLayout = new QHBoxLayout(splitWidget);
  splitLayout->setContentsMargins(0, 0, 0, 0);
  splitLayout->setSpacing(10);
  summaryLabel->setMinimumWidth(320);
  summaryLabel->setMaximumWidth(380);
  splitLayout->addWidget(summaryLabel);
  splitLayout->addWidget(chartView, 1);

  layout->addWidget(splitWidget);
  layout->addWidget(legendHelp);
}

void machine::on_btnGenererStats_machine_clicked() {
  QString statsType = ui->type_statistique_machine->currentText();
  QString chartType = ui->type_graphique_machine->currentText();

  if (statsType == "Operating state") {
    statsType = "État fonctionnement";
  } else if (statsType == "Alert types") {
    statsType = "Types alertes";
  } else if (statsType == "Average temperature") {
    statsType = "Température moyenne";
  } else if (statsType == "Machine type") {
    statsType = "Type machine";
  }

  if (chartType == "Barres" || chartType == "Histogramme (barres)" || chartType == "Bar chart") {
    chartType = "BarChart";
  } else if (chartType == "Secteur" || chartType == "Camembert" || chartType == "Pie chart") {
    chartType = "PieChart";
  } else if (chartType == "Ligne" || chartType == "Courbe" || chartType == "Line chart") {
    chartType = "LineChart";
  }

  const QList<QPair<QString, double>> data = loadStatisticsData(statsType);
  if (data.isEmpty()) {
    clearStatsChartArea(
        m_isFrench ? "Aucune donnée trouvée pour ce type de statistique."
                   : "No data found for this statistics type.");
    populateStatsComparisonTable();
    return;
  }

  renderStatisticsChart(data, statsType, chartType);
  populateStatsComparisonTable();
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
  ui->btnSetVeille_machine->setEnabled(true);
  const QString etatMarche = machineTableModel->item(m_selectedRow, 3)->text().trimmed().toUpper();
  ui->btnToggleOnOff_machine->setStyleSheet(onOffButtonStyle(etatMarche));
  ui->btnSetVeille_machine->setStyleSheet(veilleButtonStyle(etatMarche == "VEILLE"));
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
      this, m_isFrench ? "Confirmation de suppression" : "Delete confirmation",
      (m_isFrench
         ? QString("Êtes-vous sûr de vouloir supprimer la machine %1 ?")
         : QString("Are you sure you want to delete machine %1?"))
          .arg(m_selectedMachineId),
      QMessageBox::Yes | QMessageBox::No);

  if (reply == QMessageBox::Yes) {
    QSqlQuery query(ConnectionMachine::getInstance().getDatabase());
    query.prepare("DELETE FROM MACHINE WHERE ID_MACHINE = :id");
    query.bindValue(":id", m_selectedMachineId);

    if (query.exec()) {
      QMessageBox::information(this, m_isFrench ? "Succès" : "Success",
                               m_isFrench ? "Machine supprimée avec succès."
                                          : "Machine deleted successfully.");
      // Rafraîchir le tableau
      chargerMachines();
      // Désactiver les boutons
      ui->btnModifierMachine_machine->setEnabled(false);
      ui->btnSupprimerMachine_machine->setEnabled(false);
      ui->btnExporter_machine->setEnabled(false);
      ui->btnToggleOnOff_machine->setEnabled(false);
      ui->btnToggleOnOff_machine->setStyleSheet(onOffButtonStyle(""));
      ui->btnSetVeille_machine->setEnabled(false);
      ui->btnSetVeille_machine->setStyleSheet(veilleButtonStyle(false));
      ui->btnCarteMachine->setEnabled(false);
      m_selectedMachineId = "";
      m_selectedRow = -1;
    } else {
      QMessageBox::critical(this, m_isFrench ? "Erreur" : "Error",
                            (m_isFrench
                                 ? "Erreur lors de la suppression de la machine : "
                                 : "Error while deleting machine: ") +
                                query.lastError().text());
    }
  }
}

void machine::on_btnModifierMachine_machine_clicked() {
  if (m_selectedMachineId.isEmpty() || m_selectedRow < 0)
    return;

  // 1. Créer la boîte de dialogue modale
  QDialog dialog(this);
  dialog.setWindowTitle((m_isFrench ? "Modifier la machine — "
                                     : "Edit machine - ") +
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
      QMessageBox::information(
          this, m_isFrench ? "Succès" : "Success",
          m_isFrench ? "La machine a été modifiée avec succès."
                     : "Machine updated successfully.");
      chargerMachines(); // Rafraîchir le tableau
      dialog.accept();
    } else {
      QMessageBox::critical(this, m_isFrench ? "Erreur" : "Error",
                            (m_isFrench ? "Erreur lors de la modification : "
                                        : "Error while updating machine: ") +
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

  const QColor premiumGreen = m_isDarkMode ? QColor("#6ED6A2") : QColor("#2E7D32");
  const QColor premiumOrange = m_isDarkMode ? QColor("#F3A35A") : QColor("#E65100");
  const QColor premiumRed = m_isDarkMode ? QColor("#EF6B77") : QColor("#C62828");
  const QColor baseText = m_isDarkMode ? QColor("#DCE4EF") : QColor("#333333");

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
      item->setForeground(baseText);
      row.append(item);
    }

    // Coloration conditionnelle
    row[3]->setFont(boldFont);
    if (etatMarche == "ON")
      row[3]->setForeground(premiumGreen);
    else if (etatMarche == "VEILLE")
      row[3]->setForeground(premiumOrange);
    else if (etatMarche == "OFF")
      row[3]->setForeground(premiumRed);

    row[6]->setFont(boldFont);
    if (fonction == "Normal")
      row[6]->setForeground(premiumGreen);
    else if (fonction == "Alerte")
      row[6]->setForeground(premiumOrange);
    else if (fonction == "Panne")
      row[6]->setForeground(premiumRed);

    row[7]->setFont(boldFont);
    if (alerte == "Aucune")
      row[7]->setForeground(premiumGreen);
    else if (alerte == "Panne")
      row[7]->setForeground(premiumOrange);
    else if (alerte == "Surcharge")
      row[7]->setForeground(premiumRed);

    row[8]->setFont(boldFont);
    if (criticite == "Faible")
      row[8]->setForeground(premiumGreen);
    else if (criticite == "Élevé")
      row[8]->setForeground(premiumOrange);
    else if (criticite == "Critique")
      row[8]->setForeground(premiumRed);

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
  if (obj == ui->lblQrPreviewMachine &&
      event->type() == QEvent::MouseButtonRelease) {
    const QString qrUrl =
        ui->lblQrPreviewMachine->property("qrUrl").toString().trimmed();

    if (qrUrl.isEmpty()) {
      QMessageBox::information(
          this,
          m_isFrench ? "QR indisponible" : "QR unavailable",
          m_isFrench ? "Générez d'abord un QR code pour une machine."
                     : "Generate a QR code for a machine first.");
      return true;
    }

    QDesktopServices::openUrl(QUrl(qrUrl));

    return true;
  }

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
  const auto tr = [this](const QString &fr, const QString &en) {
    return m_isFrench ? fr : en;
  };

  if (m_selectedRow < 0) {
    QMessageBox::warning(this, m_isFrench ? "Aucune machine sélectionnée"
                                          : "No machine selected",
                         m_isFrench
                             ? "Veuillez double-cliquer sur une machine dans le tableau avant d'exporter."
                             : "Please double-click a machine in the table before exporting.");
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
    QMessageBox::critical(this, m_isFrench ? "Erreur" : "Error",
                          m_isFrench
                              ? "Impossible de récupérer les données de la machine."
                              : "Unable to load machine data.");
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
    QString genDate = now.toString("dd/MM/yyyy") +
            (m_isFrench ? QString::fromUtf8(" à ") : QString(" at ")) +
            now.toString("HH:mm:ss");

  QString defaultName, filter, ext;
  if (format == "Word") {
      defaultName = QString("%1_%2_%3.odt")
              .arg(m_isFrench ? "Fiche_Machine" : "Machine_Sheet", id, nom);
      filter = tr("Document Word/ODF (*.odt)", "Word/ODF Document (*.odt)");
      ext = "odt";
  } else if (format == "Excel") {
      defaultName = QString("%1_%2_%3.csv")
              .arg(m_isFrench ? "Fiche_Machine" : "Machine_Sheet", id, nom);
      filter = "Excel CSV (*.csv)";
      ext = "csv";
  } else {
      defaultName = QString("%1_%2_%3.txt")
              .arg(m_isFrench ? "Fiche_Machine" : "Machine_Sheet", id, nom);
      filter = tr("Texte brut (*.txt)", "Plain text (*.txt)");
      ext = "txt";
  }

  QString filePath = QFileDialog::getSaveFileName(
      this, m_isFrench ? "Enregistrer la fiche machine" : "Save machine sheet",
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

      if (!m_isFrench) {
        html.replace("Fiche Machine", "Machine Sheet");
        html.replace("Informations générales", "General Information");
        html.replace("Nom</span>", "Name</span>");
        html.replace("État de marche", "Running state");
        html.replace("Paramètres techniques", "Technical Parameters");
        html.replace("Température (°C)", "Temperature (°C)");
        html.replace("Charge (%)", "Load (%)");
        html.replace("Fonctionnement", "Operation");
        html.replace("Alerte", "Alert");
        html.replace("Criticité", "Severity");
        html.replace("Score Santé", "Health Score");
        html.replace("Dernière maintenance", "Last maintenance");
        html.replace("Date d'installation", "Installation date");
        html.replace("Date mise à jour", "Update date");
        html.replace("Suivi et métadonnées", "Tracking and metadata");
        html.replace("Responsable", "Responsible");
        html.replace("Refroidissement", "Cooling");
        html.replace("Priorité", "Priority");
        html.replace("Document généré le", "Document generated on");
        html.replace("Système de Gestion des Machines — Zitouna",
                     "Machine Management System - Zitouna");
      }

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
          
          out << tr("Attribut", "Attribute") << ";" << tr("Valeur", "Value") << "\n";
          out << tr("ID Machine", "Machine ID") << ";" << id << "\n";
          out << tr("Nom", "Name") << ";" << nom << "\n";
          out << tr("Type", "Type") << ";" << type << "\n";
          out << tr(QString::fromUtf8("État de marche"), "Running state") << ";" << etatMarche << "\n";
          out << tr(QString::fromUtf8("Température (°C)"), "Temperature (°C)") << ";" << temperature << "\n";
          out << tr("Charge (%)", "Load (%)") << ";" << charge << "\n";
          out << tr("Fonctionnement", "Operation") << ";" << fonctionnement << "\n";
          out << tr("Alerte", "Alert") << ";" << alerte << "\n";
          out << tr(QString::fromUtf8("Criticité"), "Severity") << ";" << criticite << "\n";
          out << tr(QString::fromUtf8("Score Santé"), "Health score") << ";" << scoreSante << "\n";
          out << tr(QString::fromUtf8("Dernière maintenance"), "Last maintenance") << ";" << maintenance << "\n";
          out << tr("Date d'installation", "Installation date") << ";" << installation << "\n";
          out << tr(QString::fromUtf8("Date mise à jour"), "Update date") << ";" << miseAJour << "\n";
          out << tr("Responsable", "Responsible") << ";" << responsable << "\n";
          out << tr("Refroidissement", "Cooling") << ";" << refroidissement << "\n";
          out << tr("Tag", "Tag") << ";" << tag << "\n";
          out << tr(QString::fromUtf8("Priorité"), "Priority") << ";" << priorite << "\n";
          out << tr(QString::fromUtf8("Document généré le"), "Document generated on") << ";" << genDate << "\n";
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
          out << tr(QString("FICHE MACHINE: %1").arg(nom),
                    QString("MACHINE SHEET: %1").arg(nom)).rightJustified(40, ' ') << "\n";
          out << "============================================================\n\n";

          out << tr("--- INFORMATIONS GENERALES ---", "--- GENERAL INFORMATION ---") << "\n";
          out << tr("ID Machine:", "Machine ID:").leftJustified(w, ' ') << id << "\n";
          out << tr("Nom:", "Name:").leftJustified(w, ' ') << nom << "\n";
          out << tr("Type:", "Type:").leftJustified(w, ' ') << type << "\n";
          out << tr(QString::fromUtf8("État de marche:"), "Running state:").leftJustified(w, ' ') << etatMarche << "\n\n";

          out << tr("--- PARAMETRES TECHNIQUES ---", "--- TECHNICAL PARAMETERS ---") << "\n";
          out << tr(QString::fromUtf8("Température (°C):"), "Temperature (°C):").leftJustified(w, ' ') << temperature << "\n";
          out << tr("Charge (%):", "Load (%):").leftJustified(w, ' ') << charge << "\n";
          out << tr("Fonctionnement:", "Operation:").leftJustified(w, ' ') << fonctionnement << "\n";
          out << tr("Alerte:", "Alert:").leftJustified(w, ' ') << alerte << "\n";
          out << tr(QString::fromUtf8("Criticité:"), "Severity:").leftJustified(w, ' ') << criticite << "\n";
          out << tr(QString::fromUtf8("Score Santé:"), "Health score:").leftJustified(w, ' ') << scoreSante << "\n\n";

          out << tr("--- DATES ---", "--- DATES ---") << "\n";
          out << tr(QString::fromUtf8("Dernière maintenance:"), "Last maintenance:").leftJustified(w, ' ') << maintenance << "\n";
          out << tr("Date d'installation:", "Installation date:").leftJustified(w, ' ') << installation << "\n";
          out << tr(QString::fromUtf8("Date mise à jour:"), "Update date:").leftJustified(w, ' ') << miseAJour << "\n\n";

          out << tr("--- SUIVI ET META ---", "--- TRACKING & META ---") << "\n";
          out << tr("Responsable:", "Responsible:").leftJustified(w, ' ') << responsable << "\n";
          out << tr("Refroidissement:", "Cooling:").leftJustified(w, ' ') << refroidissement << "\n";
          out << tr("Tag:", "Tag:").leftJustified(w, ' ') << tag << "\n";
          out << tr(QString::fromUtf8("Priorité:"), "Priority:").leftJustified(w, ' ') << priorite << "\n\n";

          out << "============================================================\n";
          out << tr(QString::fromUtf8("Document généré le "), "Document generated on ") << genDate << "\n";
          out << tr(QString::fromUtf8("Système de Gestion des Machines — Zitouna\n"),
                    "Machine Management System - Zitouna\n");
          
          txtFile.close();
          success = true;
      }
  }

  if (success) {
      QMessageBox msgBox(this);
      msgBox.setWindowTitle(m_isFrench ? "Succès" : "Success");
      msgBox.setText((m_isFrench
                ? QString::fromUtf8("✅ Fiche machine exportée avec succès en ")
                : QString::fromUtf8("✅ Machine sheet exported successfully as ")) +
             format + " !");
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
      QMessageBox::critical(this, m_isFrench ? "Erreur" : "Error",
                m_isFrench
                  ? "L'export a échoué. Vérifiez vos permissions de fichiers."
                  : "Export failed. Check your file permissions.");
  }
}

void machine::exporterPDF() {
  // 1. Check that a machine is selected
  if (m_selectedRow < 0) {
    QMessageBox::warning(this, m_isFrench ? "Aucune machine sélectionnée"
                                          : "No machine selected",
                         m_isFrench
                             ? "Veuillez double-cliquer sur une machine dans le tableau avant d'exporter."
                             : "Please double-click a machine in the table before exporting.");
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
    QMessageBox::critical(this, m_isFrench ? "Erreur" : "Error",
                          m_isFrench
                              ? "Impossible de récupérer les données de la machine."
                              : "Unable to load machine data.");
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
  QString defaultName = QString("%1_%2_%3.pdf")
                            .arg(m_isFrench ? "Fiche_Machine" : "Machine_Sheet", id, nom);
  QString filePath = QFileDialog::getSaveFileName(
      this, m_isFrench ? "Enregistrer la fiche machine" : "Save machine sheet",
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
                    (m_isFrench ? QString::fromUtf8(" à ") : QString(" at ")) +
                    now.toString("HH:mm:ss");

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

  if (!m_isFrench) {
    html.replace("Fiche Machine", "Machine Sheet");
    html.replace("Informations générales", "General Information");
    html.replace("Nom</span>", "Name</span>");
    html.replace("État de marche", "Running state");
    html.replace("Paramètres techniques", "Technical Parameters");
    html.replace("Température (°C)", "Temperature (°C)");
    html.replace("Charge (%)", "Load (%)");
    html.replace("Fonctionnement", "Operation");
    html.replace("Alerte", "Alert");
    html.replace("Criticité", "Severity");
    html.replace("Score Santé", "Health Score");
    html.replace("Dernière maintenance", "Last maintenance");
    html.replace("Date d'installation", "Installation date");
    html.replace("Date mise à jour", "Update date");
    html.replace("Suivi et métadonnées", "Tracking and metadata");
    html.replace("Responsable", "Responsible");
    html.replace("Refroidissement", "Cooling");
    html.replace("Priorité", "Priority");
    html.replace("Document généré le", "Document generated on");
    html.replace("Système de Gestion des Machines — Zitouna",
                 "Machine Management System - Zitouna");
  }

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
  msgBox.setWindowTitle(m_isFrench ? "Succès" : "Success");
  msgBox.setText(m_isFrench
                     ? QString::fromUtf8("✅ Fiche machine exportée avec succès !")
                     : QString::fromUtf8("✅ Machine sheet exported successfully!"));
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
      m_isFrench ? "Historique des machines" : "Machines history");
  ui->btnFermerHistorique->setText(QString::fromUtf8("\xE2\x9C\x95"));

  // Create or reset model
  if (!historiqueTableModel) {
    historiqueTableModel = new QStandardItemModel(0, 4, this);
  }
  historiqueTableModel->setRowCount(0);
  historiqueTableModel->setHorizontalHeaderLabels(
      {"Nom Machine", "Temps ON", "Temps OFF", "Temps Veille"});

  // Configure table view
  ui->tableHistorique_machine->setModel(historiqueTableModel);
  ui->tableHistorique_machine->verticalHeader()->setVisible(false);
  ui->tableHistorique_machine->horizontalHeader()->setStretchLastSection(true);
  ui->tableHistorique_machine->horizontalHeader()->setSectionResizeMode(
      QHeaderView::Stretch);
  ui->tableHistorique_machine->setEditTriggers(
      QAbstractItemView::NoEditTriggers);
  ui->tableHistorique_machine->setAlternatingRowColors(false);
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

    auto *itemNom = new QStandardItem(entry.machineName);
    itemNom->setEditable(false);
    itemNom->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    row.append(itemNom);

    auto *itemOn = new QStandardItem(formatDuration(entry.secondsOn));
    itemOn->setEditable(false);
    itemOn->setTextAlignment(Qt::AlignCenter);
    row.append(itemOn);

    auto *itemOff = new QStandardItem(formatDuration(entry.secondsOff));
    itemOff->setEditable(false);
    itemOff->setTextAlignment(Qt::AlignCenter);
    row.append(itemOff);

    auto *itemVeille = new QStandardItem(formatDuration(entry.secondsVeille));
    itemVeille->setEditable(false);
    itemVeille->setTextAlignment(Qt::AlignCenter);
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
  if (m_selectedMachineId.isEmpty())
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
      QMessageBox::critical(this, m_isFrench ? "Erreur" : "Error",
                            m_isFrench
                                ? QString::fromUtf8("Impossible de récupérer les données de la machine.")
                                : QString::fromUtf8("Unable to load machine data."));
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

  const CardVisualStyle initialVisual = cardVisualForState(fonctionnement);

  // Create modal dialog
  machineCardDialog = new QDialog(this);
  machineCardDialog->setWindowTitle(
      QString::fromUtf8("\xF0\x9F\x97\xBA\xEF\xB8\x8F Fiche Machine — ") + nom);
  machineCardDialog->setMinimumWidth(500);
  machineCardDialog->setMinimumHeight(620);
  machineCardDialog->setModal(false); // Non-modal to allow background interaction
  machineCardDialog->setObjectName("machineCardDialog");

  // Build dialog style based on app theme while keeping state accent color.
  auto buildDialogStyle = [this](const CardVisualStyle &visual) -> QString {
    const QString bgStart = m_isDarkMode ? "#1A2230" : "#F7FAF8";
    const QString bgEnd = m_isDarkMode ? "#111826" : "#EEF4F1";
    const QString mainText = m_isDarkMode ? "#EAF0F8" : "#19372B";
    const QString subText = m_isDarkMode ? "#9FB2C6" : "#3B6B55";
    const QString buttonBg = m_isDarkMode ? "#243447" : "#DCE8E1";
    const QString buttonText = m_isDarkMode ? "#EAF0F8" : "#204336";

    return QString(
        "QDialog#machineCardDialog { "
        "  background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 %1, stop:1 %2); "
        "  border: 4px solid %3; "
        "  border-radius: 12px; "
        "} "
        "QLabel { color: %4; font-family: 'Segoe UI', Arial, sans-serif; border: none; } "
        "QLabel#cardTitle { font-size: 18px; font-weight: bold; color: %4; } "
        "QLabel#cardFieldLabel { font-size: 12px; color: %5; font-weight: 600; } "
        "QLabel#cardFieldValue { font-size: 13px; color: %4; font-weight: bold; } "
        "QLabel#cardStatusBadge { "
        "  font-size: 13px; font-weight: bold; color: #FFFFFF; "
        "  background-color: %3; border-radius: 8px; padding: 4px 14px; "
        "} "
        "QPushButton#btnCloseCard { "
        "  background-color: %6; color: %7; "
        "  font-size: 12px; font-weight: bold; border: 1px solid rgba(255,255,255,0.3); "
        "  border-radius: 6px; padding: 8px 20px; min-width: 100px; "
        "} "
        "QPushButton#btnCloseCard:hover { background-color: %6; border: 1px solid %3; } "
          )
          .arg(bgStart, bgEnd, visual.borderColor, mainText, subText, buttonBg,
               buttonText);
  };

        machineCardDialog->setStyleSheet(buildDialogStyle(initialVisual));

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
  machineCardPollingTimer->setInterval(1000);

  QString machineId = m_selectedMachineId;
  QDialog *dlg = machineCardDialog;

  connect(machineCardPollingTimer, &QTimer::timeout, this,
          [this, dlg, machineId, buildDialogStyle, lblStatus]() {
            if (!dlg || !dlg->isVisible()) {
              if (machineCardPollingTimer)
                machineCardPollingTimer->stop();
              return;
            }
            // Re-query ETAT_FONCTIONNEMENT and refresh card colors in real time.
            QSqlQuery pollQuery(ConnectionMachine::getInstance().getDatabase());
            pollQuery.prepare("SELECT ETAT_FONCTIONNEMENT FROM MACHINE WHERE ID_MACHINE = :id");
            pollQuery.bindValue(":id", machineId);
            if (pollQuery.exec() && pollQuery.next()) {
              QString newFonct = pollQuery.value(0).toString();
              const CardVisualStyle visual = cardVisualForState(newFonct);
              dlg->setStyleSheet(buildDialogStyle(visual));
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
