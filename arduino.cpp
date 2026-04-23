#include "arduino.h"

#include "connexionmachine.h"
#include "machine.h"
#include "ui_machine.h"

#include <QComboBox>
#include <QDateTime>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLayoutItem>
#include <QPushButton>
#include <QRegularExpression>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QSqlError>
#include <QSqlQuery>
#include <QVBoxLayout>
#include <functional>

namespace {
QString dialogCardStyle(bool darkMode) {
  if (darkMode) {
    return "QGroupBox {"
           " border: 1px solid #2F4256;"
           " border-radius: 10px;"
           " margin-top: 10px;"
           " padding-top: 12px;"
           " background-color: #15202C;"
           " color: #EAF0F8;"
           "}"
           "QGroupBox::title {"
           " subcontrol-origin: margin;"
           " left: 10px;"
           " padding: 0 6px;"
           " color: #A9C4E2;"
           " font-weight: 700;"
           "}";
  }

  return "QGroupBox {"
         " border: 1px solid #C8D7D0;"
         " border-radius: 10px;"
         " margin-top: 10px;"
         " padding-top: 12px;"
         " background-color: #F8FCFA;"
         " color: #17352C;"
         "}"
         "QGroupBox::title {"
         " subcontrol-origin: margin;"
         " left: 10px;"
         " padding: 0 6px;"
         " color: #225143;"
         " font-weight: 700;"
         "}";
}

QString controlButtonStyle(bool darkMode) {
  if (darkMode) {
    return "QPushButton {"
           " background-color: #1F6F5F;"
           " color: #F4FBF9;"
           " border: 1px solid #2B8E79;"
           " border-radius: 8px;"
           " padding: 8px 14px;"
           " font-weight: 700;"
           "}"
           "QPushButton:hover { background-color: #248772; }"
           "QPushButton:pressed { background-color: #1D6D5B; }";
  }

  return "QPushButton {"
         " background-color: #1C6A57;"
         " color: white;"
         " border: 1px solid #2B816A;"
         " border-radius: 8px;"
         " padding: 8px 14px;"
         " font-weight: 700;"
         "}"
         "QPushButton:hover { background-color: #227861; }"
         "QPushButton:pressed { background-color: #1A5F4E; }";
}

QString machineActionButtonStyle(bool darkMode) {
  if (darkMode) {
    return "QPushButton {"
           " text-align: left;"
           " padding: 10px 12px;"
           " border-radius: 10px;"
           " border: 1px solid #2C4155;"
           " background-color: #182533;"
           " color: #EAF0F8;"
           " font-weight: 600;"
           "}"
           "QPushButton:hover {"
           " background-color: #203246;"
           " border: 1px solid #3A5E82;"
           "}"
           "QPushButton:pressed { background-color: #1B2C3D; }";
  }

  return "QPushButton {"
         " text-align: left;"
         " padding: 10px 12px;"
         " border-radius: 10px;"
         " border: 1px solid #C6D8CF;"
         " background-color: #F6FBF8;"
         " color: #173D31;"
         " font-weight: 600;"
         "}"
         "QPushButton:hover {"
         " background-color: #EBF6F1;"
         " border: 1px solid #9ABFAF;"
         "}"
         "QPushButton:pressed { background-color: #E1EFE8; }";
}

struct ThermalThresholdSet {
  double fanOnTemp = 35.0;
  double alertTemp = 50.0;
  double panneTemp = 65.0;
};

struct ThresholdDecision {
  bool emergencyStop = false;
  QString refroidissement;
  QString etatMarche;
  QString etatFonctionnement;
  QString typeAlerte;
  QString criticite;
  double chargeDelta = 0.0;
  double scoreDelta = 0.0;
};

ThresholdDecision decisionForTemperature(double temperature) {
  // Seuil 5: arrêt d'urgence (>= 30): seule la colonne REFROIDISSEMENT passe à off.
  if (temperature >= 30.0) {
    return {true, "off", "", "", "", "", 0.0, 0.0};
  }

  if (temperature >= 29.0) {
    return {false, "on", "ON", "Panne", "Sécurité", "Critique", 1.0, 3.0};
  }

  if (temperature >= 28.0) {
    return {false, "on", "ON", "Alerte", "Maintenance", "Élevé", 0.75, 2.0};
  }

  if (temperature >= 27.0) {
    return {false, "on", "ON", "Alerte", "Température", "Moyen", 0.5, 1.0};
  }

  return {false, "off", "ON", "Normal", "Aucune", "Faible", 0.5, 0.5};
}

enum class RiskColorBand { Green, Orange, Red };

RiskColorBand riskBand(const QString &etatFonctionnement,
                      const QString &criticite,
                      const QString &typeAlerte) {
  const QString etat = etatFonctionnement.trimmed().toLower();
  const QString crit = criticite.trimmed().toLower();
  const QString alerte = typeAlerte.trimmed().toLower();

  if (etat == "panne" || crit == "critique" || alerte.contains("sécur") ||
      alerte.contains("secur")) {
    return RiskColorBand::Red;
  }

  if (etat == "alerte" || crit.contains("moyen") || crit.contains("élev") ||
      crit.contains("eleve") || alerte.contains("temp") ||
      alerte.contains("maintenance")) {
    return RiskColorBand::Orange;
  }

  return RiskColorBand::Green;
}

class ArduinoMachineDialog : public QDialog {
public:
  ArduinoMachineDialog(const QSqlDatabase &db,
                       const QString &tableName,
                       const QString &machineId,
                       const QString &machineName,
                       const QString &machineType,
                       ThermalProfileMode mode,
                       bool isFrench,
                       bool isDarkMode,
                       std::function<void()> onTelemetryPersisted,
                       QWidget *parent = nullptr)
      : QDialog(parent), m_db(db), m_tableName(tableName), m_machineId(machineId),
        m_machineName(machineName), m_machineType(machineType), m_mode(mode),
        m_isFrench(isFrench), m_isDarkMode(isDarkMode),
        m_onTelemetryPersisted(std::move(onTelemetryPersisted)) {
    setWindowTitle(m_isFrench ? "Contrôle Arduino - " + m_machineName
                              : "Arduino control - " + m_machineName);
    setAttribute(Qt::WA_DeleteOnClose);
    setMinimumSize(720, 430);
    buildUi();
    refreshPorts();
  }

  ~ArduinoMachineDialog() override {
    if (m_serial && m_serial->isOpen()) {
      m_serial->close();
    }
  }

private:
  void buildUi() {
    setStyleSheet(m_isDarkMode
                      ? "QDialog { background-color: #101922; } QLabel { color: #D8E4F0; }"
                      : "QDialog { background-color: #F3F8F5; } QLabel { color: #244439; }");

    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(16, 16, 16, 16);
    mainLayout->setSpacing(12);

    m_lblHeader = new QLabel(
        QString("%1 | ID: %2 | %3").arg(m_machineName, m_machineId, m_machineType),
        this);
    m_lblHeader->setStyleSheet(
      m_isDarkMode
        ? "font-size: 15px; font-weight: 800; color: #F2F7FF; letter-spacing: 0.3px;"
        : "font-size: 15px; font-weight: 800; color: #1A3C2F; letter-spacing: 0.3px;");
    mainLayout->addWidget(m_lblHeader);

    auto *connectionBox =
        new QGroupBox(m_isFrench ? "Connexion série" : "Serial connection", this);
    connectionBox->setStyleSheet(dialogCardStyle(m_isDarkMode));
    auto *connectionLayout = new QHBoxLayout(connectionBox);
    connectionLayout->setSpacing(10);
    m_comboPort = new QComboBox(connectionBox);
    m_comboBaudrate = new QComboBox(connectionBox);
    m_comboBaudrate->addItems({"9600", "19200", "38400", "57600", "115200"});
    m_comboBaudrate->setCurrentText("9600");

    auto *btnRefreshPorts =
        new QPushButton(m_isFrench ? "Rafraîchir ports" : "Refresh ports", connectionBox);
    m_btnConnect = new QPushButton(m_isFrench ? "Connecter" : "Connect", connectionBox);
    btnRefreshPorts->setStyleSheet(controlButtonStyle(m_isDarkMode));
    m_btnConnect->setStyleSheet(controlButtonStyle(m_isDarkMode));

    const QString comboStyle = m_isDarkMode
                                   ? "QComboBox { background-color: #0F1720; color: #EAF0F8; border: 1px solid #304559; border-radius: 7px; padding: 6px 10px; }"
                                     "QComboBox::drop-down { border: none; }"
                                   : "QComboBox { background-color: #FFFFFF; color: #1E4236; border: 1px solid #BFD1C8; border-radius: 7px; padding: 6px 10px; }"
                                     "QComboBox::drop-down { border: none; }";
    m_comboPort->setStyleSheet(comboStyle);
    m_comboBaudrate->setStyleSheet(comboStyle);

    connectionLayout->addWidget(new QLabel("Port", connectionBox));
    connectionLayout->addWidget(m_comboPort, 1);
    connectionLayout->addWidget(new QLabel("Baud", connectionBox));
    connectionLayout->addWidget(m_comboBaudrate);
    connectionLayout->addWidget(btnRefreshPorts);
    connectionLayout->addWidget(m_btnConnect);

    mainLayout->addWidget(connectionBox);

    auto *framesLayout = new QHBoxLayout();

    auto *tempBox =
        new QGroupBox(m_isFrench ? "Température en temps réel" : "Real-time temperature", this);
    tempBox->setStyleSheet(dialogCardStyle(m_isDarkMode));
    auto *tempLayout = new QVBoxLayout(tempBox);
    m_lblTemperature = new QLabel("--.- C", tempBox);
    m_lblHumidity = new QLabel("-- %", tempBox);
    m_lblNiveau = new QLabel("Niveau: ----", tempBox);
    m_lblTemperature->setStyleSheet(
      m_isDarkMode
        ? "font-size: 34px; font-weight: 800; color: #7FE2BE;"
        : "font-size: 34px; font-weight: 800; color: #1A6A57;");
    tempLayout->addWidget(m_lblTemperature);
    tempLayout->addWidget(m_lblHumidity);
    tempLayout->addWidget(m_lblNiveau);

    auto *fanBox = new QGroupBox(m_isFrench ? "Etat du ventilateur" : "Fan status", this);
    fanBox->setStyleSheet(dialogCardStyle(m_isDarkMode));
    auto *fanLayout = new QVBoxLayout(fanBox);
    m_lblFanState = new QLabel("OFF", fanBox);
    m_lblFanState->setAlignment(Qt::AlignCenter);
    m_lblFanState->setMinimumHeight(56);
    m_lblFanState->setStyleSheet(
      "font-size: 22px; font-weight: 800; border-radius: 16px; background-color: #7E7E7E; color: white; padding: 6px;");
    m_lblOutcome = new QLabel(m_isFrench ? "État BD: en attente" : "DB status: waiting", fanBox);
    fanLayout->addWidget(m_lblFanState);
    fanLayout->addWidget(m_lblOutcome);

    framesLayout->addWidget(tempBox, 1);
    framesLayout->addWidget(fanBox, 1);
    mainLayout->addLayout(framesLayout);

    m_lblConnectionState = new QLabel(m_isFrench ? "Non connecté" : "Disconnected", this);
    m_lblConnectionState->setStyleSheet(
      m_isDarkMode
        ? "color: #F7CF7C; font-weight: 700; background-color: #2A2B1D; border-radius: 7px; padding: 6px 10px;"
        : "color: #8A5A00; font-weight: 700; background-color: #FFF4D9; border-radius: 7px; padding: 6px 10px;");
    mainLayout->addWidget(m_lblConnectionState);

    m_lblRawFrame = new QLabel(this);
    m_lblRawFrame->setWordWrap(true);
    m_lblRawFrame->setText(m_isFrench ? "Dernière trame: -" : "Last frame: -");
    m_lblRawFrame->setStyleSheet(
      m_isDarkMode
        ? "color: #B9C8D8; background-color: #121F2B; border: 1px solid #2D4257; border-radius: 8px; padding: 8px;"
        : "color: #445A53; background-color: #F7FBF9; border: 1px solid #C7DAD1; border-radius: 8px; padding: 8px;");
    mainLayout->addWidget(m_lblRawFrame);

    auto *liveBox =
        new QGroupBox(m_isFrench ? "État machine en temps réel" : "Real-time machine state", this);
    liveBox->setStyleSheet(dialogCardStyle(m_isDarkMode));
    auto *liveGrid = new QGridLayout(liveBox);
    liveGrid->setContentsMargins(10, 10, 10, 10);
    liveGrid->setHorizontalSpacing(12);
    liveGrid->setVerticalSpacing(6);

    addStatusRow(liveGrid, 0, "TEMPERATURE_ACTUELLE", &m_valTemperatureActuelle);
    addStatusRow(liveGrid, 1, "REFROIDISSEMENT", &m_valRefroidissement);
    addStatusRow(liveGrid, 2, "ETAT_MARCHE", &m_valEtatMarche);
    addStatusRow(liveGrid, 3, "ETAT_FONCTIONNEMENT", &m_valEtatFonctionnement);
    addStatusRow(liveGrid, 4, "TYPE_ALERTE", &m_valTypeAlerte);
    addStatusRow(liveGrid, 5, "NIVEAU_CRITICITE", &m_valCriticite);
    addStatusRow(liveGrid, 6, "NIVEAU_CHARGE", &m_valCharge);
    addStatusRow(liveGrid, 7, "SCORE_SANTE", &m_valScoreSante);
    addStatusRow(liveGrid, 8, "PRIORITE", &m_valPriorite);
    addStatusRow(liveGrid, 9, "TAG", &m_valTag);
    mainLayout->addWidget(liveBox);

    connect(btnRefreshPorts, &QPushButton::clicked, this, [this]() { refreshPorts(); });
    connect(m_btnConnect, &QPushButton::clicked, this, [this]() { toggleConnection(); });

    refreshDbSnapshotAndPanel();
  }

  void addStatusRow(QGridLayout *grid, int row, const QString &labelText, QLabel **valuePtr) {
    auto *name = new QLabel(labelText, this);
    name->setStyleSheet(m_isDarkMode ? "font-weight: 700; color: #BFD2E7;" : "font-weight: 700; color: #355247;");
    auto *value = new QLabel("-", this);
    value->setStyleSheet(m_isDarkMode
                             ? "background-color: #182533; border: 1px solid #2F4256; border-radius: 6px; padding: 3px 8px;"
                             : "background-color: #FFFFFF; border: 1px solid #C8D7D0; border-radius: 6px; padding: 3px 8px;");
    grid->addWidget(name, row, 0);
    grid->addWidget(value, row, 1);
    *valuePtr = value;
  }

  void updateStatusPanelColors(const QString &etatFonctionnement,
                               const QString &criticite,
                               const QString &typeAlerte) {
    const RiskColorBand band = riskBand(etatFonctionnement, criticite, typeAlerte);
    m_pulseToggle = !m_pulseToggle;

    QString base;
    QString text;
    if (band == RiskColorBand::Green) {
      base = m_pulseToggle ? "#DDF4E9" : "#CDEFDf";
      text = "#0F5B3E";
    } else if (band == RiskColorBand::Orange) {
      base = m_pulseToggle ? "#FEE7CC" : "#FFDDB4";
      text = "#8C4E06";
    } else {
      base = m_pulseToggle ? "#FFD7D7" : "#FFC8C8";
      text = "#8F1E1E";
    }

    auto apply = [&](QLabel *value) {
      if (!value) {
        return;
      }
      value->setStyleSheet(QString("background-color: %1; color: %2; border: 1px solid transparent; border-radius: 6px; padding: 3px 8px; font-weight: 700;")
                               .arg(base, text));
    };

    apply(m_valTemperatureActuelle);
    apply(m_valRefroidissement);
    apply(m_valEtatMarche);
    apply(m_valEtatFonctionnement);
    apply(m_valTypeAlerte);
    apply(m_valCriticite);
    apply(m_valCharge);
    apply(m_valScoreSante);
    apply(m_valPriorite);
    apply(m_valTag);
  }

  void refreshDbSnapshotAndPanel() {
    QSqlQuery readQuery(m_db);
    readQuery.prepare(QString(
            "SELECT NVL(TEMPERATURE_ACTUELLE, 0), NVL(REFROIDISSEMENT, '-'), "
            "ETAT_MARCHE, ETAT_FONCTIONNEMENT, TYPE_ALERTE, "
            "NIVEAU_CRITICITE, NVL(NIVEAU_CHARGE, 0), NVL(SCORE_SANTE, 0), "
            "NVL(PRIORITE, '-'), NVL(TAG, '-'), DATE_DERNIERE_MAINTENANCE, "
            "CASE WHEN TEMPERATURE_ACTUELLE > 80 "
            "  OR DATE_DERNIERE_MAINTENANCE <= SYSDATE + 30 "
            "  THEN 'À vérifier' ELSE NULL END AS TAG_CALC, "
            "CASE WHEN NIVEAU_CRITICITE = 'Critique' "
            "  OR ETAT_FONCTIONNEMENT = 'Panne' THEN 'Haute' "
            "  WHEN NIVEAU_CRITICITE = 'Élevé' "
            "  OR ETAT_FONCTIONNEMENT = 'Alerte' THEN 'Moyenne' "
            "  ELSE 'Basse' END AS PRIORITE_CALC "
            "FROM %1 WHERE ID_MACHINE = :id")
                .arg(m_tableName));
    readQuery.bindValue(":id", m_machineId);
    if (!readQuery.exec() || !readQuery.next()) {
      return;
    }

    const QString temperatureActuelle = QString::number(readQuery.value(0).toDouble(), 'f', 2);
    const QString refroidissement = readQuery.value(1).toString();
    const QString etatMarche = readQuery.value(2).toString();
    const QString etatFonctionnement = readQuery.value(3).toString();
    const QString typeAlerte = readQuery.value(4).toString();
    const QString criticite = readQuery.value(5).toString();
    const QString charge = QString::number(readQuery.value(6).toDouble(), 'f', 2);
    const QString score = QString::number(readQuery.value(7).toDouble(), 'f', 2);
    const QString prioriteDb = readQuery.value(8).toString();
    const QString tagDb = readQuery.value(9).toString();
    const QString tagCalc = readQuery.value(11).toString();
    const QString prioriteCalc = readQuery.value(12).toString();

    const QString tag = tagDb.trimmed().isEmpty() || tagDb.trimmed() == "-"
                ? (tagCalc.trimmed().isEmpty() ? "-" : tagCalc)
                : tagDb;
    const QString priorite = prioriteDb.trimmed().isEmpty() || prioriteDb.trimmed() == "-"
                   ? (prioriteCalc.trimmed().isEmpty() ? "-" : prioriteCalc)
                   : prioriteDb;

    if (m_valTemperatureActuelle) m_valTemperatureActuelle->setText(temperatureActuelle + " C");
    if (m_valRefroidissement) m_valRefroidissement->setText(refroidissement);
    if (m_valEtatMarche) m_valEtatMarche->setText(etatMarche);
    if (m_valEtatFonctionnement) m_valEtatFonctionnement->setText(etatFonctionnement);
    if (m_valTypeAlerte) m_valTypeAlerte->setText(typeAlerte);
    if (m_valCriticite) m_valCriticite->setText(criticite);
    if (m_valCharge) m_valCharge->setText(charge);
    if (m_valScoreSante) m_valScoreSante->setText(score);
    if (m_valPriorite) m_valPriorite->setText(priorite);
    if (m_valTag) m_valTag->setText(tag);

    updateStatusPanelColors(etatFonctionnement, criticite, typeAlerte);
  }

  void refreshPorts() {
    const QString previous = m_comboPort->currentText();
    m_comboPort->clear();
    const auto ports = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &port : ports) {
      const QString label = port.description().trimmed().isEmpty()
                                ? port.portName()
                                : QString("%1 - %2").arg(port.portName(), port.description().trimmed());
      m_comboPort->addItem(label, port.portName());
    }
    int idx = m_comboPort->findText(previous);
    if (idx < 0) {
      idx = m_comboPort->findData(previous);
    }
    if (idx >= 0) {
      m_comboPort->setCurrentIndex(idx);
    } else if (m_comboPort->count() > 0) {
      m_comboPort->setCurrentIndex(0);
    }
  }

  ThermalThresholdSet thresholdsForCurrentMachine() const {
    if (m_mode == ThermalProfileMode::StandardA) {
      return {35.0, 50.0, 65.0};
    }
    if (m_mode == ThermalProfileMode::HotEnvironmentB) {
      return {45.0, 70.0, 85.0};
    }

    ThermalThresholdSet fallback{35.0, 50.0, 65.0};
    const QString typeLower = m_machineType.trimmed().toLower();
    if (typeLower.contains("broyeur")) {
      fallback = {30.0, 40.0, 55.0};
    } else if (typeLower.contains("malaxeur")) {
      fallback = {45.0, 60.0, 75.0};
    } else if (typeLower.contains("presse")) {
      fallback = {45.0, 70.0, 85.0};
    } else if (typeLower.contains("chauffeur")) {
      fallback = {50.0, 80.0, 95.0};
    }

    QSqlQuery query(m_db);
    query.prepare("SELECT FAN_ON_TEMP, ALERT_TEMP, PANNE_TEMP "
                  "FROM MACHINE_THRESHOLDS "
                  "WHERE UPPER(TRIM(TYPE_MACHINE)) = UPPER(TRIM(:type))");
    query.bindValue(":type", m_machineType);
    if (query.exec() && query.next()) {
      bool okFan = false;
      bool okAlert = false;
      bool okPanne = false;
      const double fan = query.value(0).toDouble(&okFan);
      const double alert = query.value(1).toDouble(&okAlert);
      const double panne = query.value(2).toDouble(&okPanne);
      if (okFan && okAlert && okPanne && fan < alert && alert < panne) {
        return {fan, alert, panne};
      }
    }

    return fallback;
  }

  void toggleConnection() {
    if (m_serial && m_serial->isOpen()) {
      m_serial->close();
      m_btnConnect->setText(m_isFrench ? "Connecter" : "Connect");
      m_lblConnectionState->setText(m_isFrench ? "Connexion fermée" : "Connection closed");
      return;
    }

    refreshPorts();

    QString portName = m_comboPort->currentData().toString().trimmed();
    if (portName.isEmpty()) {
      portName = m_comboPort->currentText().trimmed();
      const int splitIndex = portName.indexOf(" - ");
      if (splitIndex > 0) {
        portName = portName.left(splitIndex).trimmed();
      }
    }

    if (portName.isEmpty()) {
      m_lblConnectionState->setText(m_isFrench ? "Aucun port série disponible"
                                               : "No serial port available");
      return;
    }

    if (!m_serial) {
      m_serial = new QSerialPort(this);
      connect(m_serial, &QSerialPort::readyRead, this, [this]() { handleReadyRead(); });
    } else {
      m_serial->close();
      m_serial->clearError();
    }

    m_serial->setPortName(portName);
    m_serial->setBaudRate(m_comboBaudrate->currentText().toInt());
    m_serial->setDataBits(QSerialPort::Data8);
    m_serial->setParity(QSerialPort::NoParity);
    m_serial->setStopBits(QSerialPort::OneStop);
    m_serial->setFlowControl(QSerialPort::NoFlowControl);

        // Certains drivers USB serie refusent ReadOnly: tentative d'abord en ReadWrite.
        if (!m_serial->open(QIODevice::ReadWrite) && !m_serial->open(QIODevice::ReadOnly)) {
          const QString errText = m_serial->errorString();
          const bool permissionDenied =
              (m_serial->error() == QSerialPort::PermissionError) ||
              errText.contains("Accès refusé", Qt::CaseInsensitive) ||
              errText.contains("Access denied", Qt::CaseInsensitive);
      m_lblConnectionState->setText(
              permissionDenied
              ? (m_isFrench
                 ? QString("Échec de connexion: %1. Port occupé. Ferme Arduino IDE/Moniteur série puis réessaie.")
                   .arg(errText)
                 : QString("Connection failed: %1. Port busy. Close Arduino IDE/Serial Monitor then retry.")
                   .arg(errText))
              : (m_isFrench
                 ? QString("Échec de connexion: %1").arg(errText)
                 : QString("Connection failed: %1").arg(errText)));
      return;
    }

        m_serial->setDataTerminalReady(true);
        m_serial->setReadBufferSize(4096);

    m_btnConnect->setText(m_isFrench ? "Déconnecter" : "Disconnect");
    m_lblConnectionState->setText((m_isFrench ? "Connecté sur " : "Connected on ") + portName +
                                  QString(" @ %1").arg(m_comboBaudrate->currentText()));
  }

  void handleReadyRead() {
    if (!m_serial) {
      return;
    }

    m_pendingBuffer += QString::fromUtf8(m_serial->readAll());
    int newlinePos = m_pendingBuffer.indexOf('\n');
    while (newlinePos >= 0) {
      const QString line = m_pendingBuffer.left(newlinePos).trimmed();
      m_pendingBuffer.remove(0, newlinePos + 1);
      processFrameLine(line);
      newlinePos = m_pendingBuffer.indexOf('\n');
    }
  }

  void processFrameLine(const QString &line) {
    if (line.isEmpty()) {
      return;
    }

    static const QRegularExpression frameRegex(
      R"(Temp:\s*([-+]?\d+(?:\.\d+)?)\s*C\s*\|\s*Humidite:\s*([-+]?\d+(?:\.\d+)?)\s*%\s*\|\s*Niveau:\s*([^|]+?)\s*\|\s*Ventilateur:\s*(ON|OFF))",
      QRegularExpression::CaseInsensitiveOption);

    if (line.contains("Erreur capteur", Qt::CaseInsensitive)) {
      m_lblConnectionState->setText(
        m_isFrench ? "Trame reçue: erreur capteur Arduino"
             : "Frame received: Arduino sensor error");
      m_lblConnectionState->setStyleSheet(
        m_isDarkMode
          ? "color: #FFD08A; font-weight: 700; background-color: #2F2416; border-radius: 7px; padding: 6px 10px;"
          : "color: #8A5A00; font-weight: 700; background-color: #FFF4D9; border-radius: 7px; padding: 6px 10px;");
      m_lblRawFrame->setText((m_isFrench ? "Dernière trame: " : "Last frame: ") + line);
      return;
    }

    const QRegularExpressionMatch match = frameRegex.match(line);
    if (!match.hasMatch()) {
      return;
    }

    const double temperature = match.captured(1).toDouble();
    const double humidity = match.captured(2).toDouble();
    const QString levelText = match.captured(3).trimmed();
    const QString fanState = match.captured(4).toUpper();

    m_lblTemperature->setText(QString::number(temperature, 'f', 1) + " C");
    m_lblHumidity->setText(QString("Humidité: %1 %").arg(QString::number(humidity, 'f', 0)));
    m_lblNiveau->setText(QString("Niveau: %1").arg(levelText));
    m_lblFanState->setText(fanState);

    if (fanState == "ON") {
      m_lblFanState->setStyleSheet(
          "font-size: 22px; font-weight: 700; border-radius: 8px; background-color: #2E7D32; color: white;");
    } else {
      m_lblFanState->setStyleSheet(
          "font-size: 22px; font-weight: 700; border-radius: 8px; background-color: #7E7E7E; color: white;");
    }

    const ThresholdDecision decision = decisionForTemperature(temperature);

    m_lblOutcome->setText(m_isFrench
                              ? QString("Mode métier: %1")
                                    .arg(decision.emergencyStop ? "Arrêt d'urgence" : "Mise à jour active")
                              : QString("Business mode: %1")
                                    .arg(decision.emergencyStop ? "Emergency stop" : "Active update"));

    m_lblRawFrame->setText((m_isFrench ? "Dernière trame: " : "Last frame: ") + line);

    QSqlQuery query(m_db);
    if (decision.emergencyStop) {
      query.prepare(QString("UPDATE %1 SET "
                            "REFROIDISSEMENT = 'off', "
                            "DATE_MISE_A_JOUR = SYSTIMESTAMP "
                            "WHERE ID_MACHINE = :id")
                        .arg(m_tableName));
      query.bindValue(":id", m_machineId);
    } else {
      query.prepare(QString("UPDATE %1 SET "
                            "TEMPERATURE_ACTUELLE = :temperature, "
                            "REFROIDISSEMENT = :cooling, "
                            "ETAT_MARCHE = :etat_marche, "
                            "ETAT_FONCTIONNEMENT = :state, "
                            "TYPE_ALERTE = :alert, "
                            "NIVEAU_CRITICITE = :criticality, "
                            "NIVEAU_CHARGE = NVL(NIVEAU_CHARGE, 0) + :charge_delta, "
                            "SCORE_SANTE = GREATEST(0, NVL(SCORE_SANTE, 100) - :score_delta), "
                            "DATE_MISE_A_JOUR = SYSTIMESTAMP "
                            "WHERE ID_MACHINE = :id")
                        .arg(m_tableName));

      query.bindValue(":temperature", temperature);
      query.bindValue(":cooling", decision.refroidissement);
      query.bindValue(":etat_marche", decision.etatMarche);
      query.bindValue(":state", decision.etatFonctionnement);
      query.bindValue(":alert", decision.typeAlerte);
      query.bindValue(":criticality", decision.criticite);
      query.bindValue(":charge_delta", decision.chargeDelta);
      query.bindValue(":score_delta", decision.scoreDelta);
      query.bindValue(":id", m_machineId);
    }

    if (!query.exec()) {
      m_lblConnectionState->setText(
          (m_isFrench ? "Erreur SQL: " : "SQL error: ") + query.lastError().text());
      return;
    }

    refreshDbSnapshotAndPanel();

    if (m_onTelemetryPersisted) {
      m_onTelemetryPersisted();
    }
  }

  QSqlDatabase m_db;
  QString m_tableName;
  QString m_machineId;
  QString m_machineName;
  QString m_machineType;
  ThermalProfileMode m_mode = ThermalProfileMode::StandardA;
  bool m_isFrench = true;
  bool m_isDarkMode = false;
  std::function<void()> m_onTelemetryPersisted;

  QComboBox *m_comboPort = nullptr;
  QComboBox *m_comboBaudrate = nullptr;
  QPushButton *m_btnConnect = nullptr;
  QLabel *m_lblHeader = nullptr;
  QLabel *m_lblTemperature = nullptr;
  QLabel *m_lblHumidity = nullptr;
  QLabel *m_lblNiveau = nullptr;
  QLabel *m_lblFanState = nullptr;
  QLabel *m_lblOutcome = nullptr;
  QLabel *m_lblConnectionState = nullptr;
  QLabel *m_lblRawFrame = nullptr;
  QLabel *m_valTemperatureActuelle = nullptr;
  QLabel *m_valRefroidissement = nullptr;
  QLabel *m_valEtatMarche = nullptr;
  QLabel *m_valEtatFonctionnement = nullptr;
  QLabel *m_valTypeAlerte = nullptr;
  QLabel *m_valCriticite = nullptr;
  QLabel *m_valCharge = nullptr;
  QLabel *m_valScoreSante = nullptr;
  QLabel *m_valPriorite = nullptr;
  QLabel *m_valTag = nullptr;
  QSerialPort *m_serial = nullptr;
  QString m_pendingBuffer;
  bool m_pulseToggle = false;
};
} // namespace

void machine::setupArduinoTab() {
  if (!ui->layoutArduinoPlaceholder) {
    return;
  }

  QLayoutItem *child = nullptr;
  while ((child = ui->layoutArduinoPlaceholder->takeAt(0)) != nullptr) {
    if (child->widget()) {
      delete child->widget();
    }
    delete child;
  }

    auto *topRow = new QHBoxLayout();
    auto *title =
      new QLabel(m_isFrench ? "Centre Arduino — Machines éligibles" : "Arduino center — Eligible machines", this);
    title->setStyleSheet(m_isDarkMode
                 ? "font-weight: 800; font-size: 14px; color: #EAF0F8;"
                 : "font-weight: 800; font-size: 14px; color: #1A3C2F;");
  topRow->addWidget(title);
  topRow->addStretch(1);

  m_comboArduinoMode = new QComboBox(this);
  m_comboArduinoMode->addItem(m_isFrench ? "Hypothèse A (standard)" : "Hypothesis A (standard)",
                              static_cast<int>(ThermalProfileMode::StandardA));
  m_comboArduinoMode->addItem(
      m_isFrench ? "Hypothèse B (environnement chaud)" : "Hypothesis B (hot environment)",
      static_cast<int>(ThermalProfileMode::HotEnvironmentB));
  m_comboArduinoMode->addItem(m_isFrench ? "Hypothèse C (adaptatif)" : "Hypothesis C (adaptive)",
                              static_cast<int>(ThermalProfileMode::AdaptiveC));
  m_comboArduinoMode->setStyleSheet(m_isDarkMode
                                        ? "QComboBox { background-color: #0F1720; color: #EAF0F8; border: 1px solid #304559; border-radius: 7px; padding: 6px 10px; } QComboBox::drop-down { border: none; }"
                                        : "QComboBox { background-color: #FFFFFF; color: #1E4236; border: 1px solid #BFD1C8; border-radius: 7px; padding: 6px 10px; } QComboBox::drop-down { border: none; }");
  topRow->addWidget(m_comboArduinoMode);

  m_btnArduinoRefresh = new QPushButton(m_isFrench ? "Actualiser" : "Refresh", this);
    m_btnArduinoRefresh->setStyleSheet(controlButtonStyle(m_isDarkMode));
  topRow->addWidget(m_btnArduinoRefresh);

  auto *topRowHost = new QWidget(this);
  topRowHost->setLayout(topRow);
  ui->layoutArduinoPlaceholder->addWidget(topRowHost);

  m_lblArduinoEligibleStatus = new QLabel(this);
  m_lblArduinoEligibleStatus->setStyleSheet(
      m_isDarkMode
          ? "color: #C5D6E8; background-color: #14202C; border: 1px solid #2F4256; border-radius: 8px; padding: 6px 10px;"
          : "color: #355247; background-color: #F6FBF8; border: 1px solid #C8D7D0; border-radius: 8px; padding: 6px 10px;");
  ui->layoutArduinoPlaceholder->addWidget(m_lblArduinoEligibleStatus);

  m_arduinoButtonsHost = new QWidget(this);
  m_arduinoButtonsLayout = new QGridLayout(m_arduinoButtonsHost);
  m_arduinoButtonsLayout->setContentsMargins(0, 0, 0, 0);
  m_arduinoButtonsLayout->setHorizontalSpacing(8);
  m_arduinoButtonsLayout->setVerticalSpacing(8);
  m_arduinoButtonsHost->setStyleSheet(m_isDarkMode
                                          ? "background-color: #101922; border: 1px solid #2F4256; border-radius: 10px;"
                                          : "background-color: #F8FCFA; border: 1px solid #C8D7D0; border-radius: 10px;");
  ui->layoutArduinoPlaceholder->addWidget(m_arduinoButtonsHost, 1);

  connect(m_btnArduinoRefresh, &QPushButton::clicked, this,
          &machine::refreshEligibleArduinoMachines);

  m_arduinoEligibleRefreshTimer = new QTimer(this);
  connect(m_arduinoEligibleRefreshTimer, &QTimer::timeout, this,
          &machine::refreshEligibleArduinoMachines);
  m_arduinoEligibleRefreshTimer->start(10000);

  refreshEligibleArduinoMachines();
}

void machine::refreshEligibleArduinoMachines() {
  if (!m_arduinoButtonsLayout) {
    return;
  }

  QLayoutItem *child = nullptr;
  while ((child = m_arduinoButtonsLayout->takeAt(0)) != nullptr) {
    if (child->widget()) {
      delete child->widget();
    }
    delete child;
  }

  QSqlQuery query(ConnectionMachine::getInstance().getDatabase());
  query.prepare(QString("SELECT ID_MACHINE, NOM_MACHINE, TYPE_MACHINE "
                        "FROM %1 "
                        "WHERE ETAT_MARCHE = 'ON' "
                        "AND ETAT_FONCTIONNEMENT = 'Normal' "
                        "AND TYPE_ALERTE = 'Aucune' "
                        "AND NIVEAU_CRITICITE = 'Faible' "
                        "ORDER BY NOM_MACHINE ASC")
                    .arg(machineTableName()));

  if (!query.exec()) {
    if (m_lblArduinoEligibleStatus) {
      m_lblArduinoEligibleStatus->setText(
          (m_isFrench ? "Erreur SQL: " : "SQL error: ") + query.lastError().text());
    }
    return;
  }

  int count = 0;
  int row = 0;
  int col = 0;
  while (query.next()) {
    const QString machineId = query.value(0).toString().trimmed();
    const QString machineName = query.value(1).toString().trimmed();
    const QString machineType = query.value(2).toString().trimmed();

    auto *button = new QPushButton(
        machineName.isEmpty() ? QString("Machine %1").arg(machineId) : machineName,
        m_arduinoButtonsHost);
    button->setToolTip(QString("ID %1 | %2").arg(machineId, machineType));
    button->setMinimumHeight(44);
    button->setStyleSheet(machineActionButtonStyle(m_isDarkMode));

    connect(button, &QPushButton::clicked, this, [this, machineId, machineName, machineType]() {
      const int modeIndex = m_comboArduinoMode
                ? m_comboArduinoMode->currentData().toInt()
                : static_cast<int>(ThermalProfileMode::StandardA);
      const ThermalProfileMode mode = static_cast<ThermalProfileMode>(modeIndex);

      auto *dialog = new ArduinoMachineDialog(
          ConnectionMachine::getInstance().getDatabase(), machineTableName(), machineId,
          machineName, machineType, mode, m_isFrench, m_isDarkMode,
          [this]() {
            chargerMachines();
            refreshEligibleArduinoMachines();
            updateAICarnetForSelectedMachine();
          },
          this);
      dialog->show();
    });

    m_arduinoButtonsLayout->addWidget(button, row, col);
    ++count;
    ++col;
    if (col >= 3) {
      col = 0;
      ++row;
    }
  }

  if (count == 0) {
    auto *empty = new QLabel(
        m_isFrench ? "Aucune machine éligible (ON/Normal/Aucune/Faible)."
                   : "No eligible machine (ON/Normal/None/Low).",
        m_arduinoButtonsHost);
    empty->setAlignment(Qt::AlignCenter);
    empty->setMinimumHeight(56);
    m_arduinoButtonsLayout->addWidget(empty, 0, 0, 1, 3);
  }

  if (m_lblArduinoEligibleStatus) {
    m_lblArduinoEligibleStatus->setText(
        m_isFrench ? QString("%1 machine(s) éligible(s) - MAJ %2")
                         .arg(count)
                         .arg(QDateTime::currentDateTime().toString("dd/MM/yyyy HH:mm:ss"))
                   : QString("%1 eligible machine(s) - Updated %2")
                         .arg(count)
                         .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss")));
  }
}
