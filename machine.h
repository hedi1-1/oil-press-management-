#ifndef MACHINE_H
#define MACHINE_H

#include <QButtonGroup>
#include <QCheckBox>
#include <QDate>
#include <QRadioButton>
#include <QProcess>
#include <QDialog>
#include <QEvent>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QMainWindow>
#include <QPair>
#include <QPixmap>
#include <QPushButton>
#include <QStandardItemModel>
#include <QTimer>
#include <QVector>
#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui {
class machine;
};
QT_END_NAMESPACE

class MachineServer;
class Assistant;
class QComboBox;
class QFrame;
class QGridLayout;
class QLabel;
class QScrollArea;
class QSerialPort;
class QTimer;

struct ArduinoThresholdProfile {
  double fanOn = 35.0;
  double alert = 50.0;
  double panne = 65.0;
};

enum class ArduinoThresholdMode {
  HypothesisA = 0,
  HypothesisB = 1,
  HypothesisC = 2
};

class ArduinoMachineDialog : public QDialog {
  Q_OBJECT

public:
  ArduinoMachineDialog(const QString &machineId,
                       const QString &machineName,
                       const QString &machineType,
                       bool isFrench,
                       bool isDarkMode,
                       QWidget *parent = nullptr);
  ~ArduinoMachineDialog() override;

signals:
  void telemetryApplied(const QString &machineId);

private slots:
  void toggleConnection();
  void refreshPortList();
  void readSerialData();
  void onThresholdModeChanged(int index);

private:
  void buildUi();
  void applyTheme();
  void setConnectionState(bool connected);
  void handleSerialLine(const QString &line);
  void updateTelemetry(double temperature,
                       double humidity,
                       const QString &level,
                       const QString &fanState);
  ArduinoThresholdProfile resolveThresholdProfile() const;
  ArduinoThresholdProfile loadProfileFromDatabase() const;
  QString formatHeader() const;
  int computeHealthScore(double temperature,
                         const ArduinoThresholdProfile &profile,
                         const QString &etatFonctionnement) const;

  QString m_machineId;
  QString m_machineName;
  QString m_machineType;
  bool m_isFrench = true;
  bool m_isDarkMode = false;
  ArduinoThresholdMode m_thresholdMode = ArduinoThresholdMode::HypothesisB;

  QSerialPort *m_serial = nullptr;
  QByteArray m_serialBuffer;

  QLabel *m_headerLabel = nullptr;
  QLabel *m_statusLabel = nullptr;
  QLabel *m_tempValueLabel = nullptr;
  QLabel *m_fanValueLabel = nullptr;
  QLabel *m_levelValueLabel = nullptr;
  QLabel *m_humidityValueLabel = nullptr;
  QComboBox *m_portCombo = nullptr;
  QComboBox *m_baudCombo = nullptr;
  QComboBox *m_thresholdCombo = nullptr;
  QPushButton *m_connectButton = nullptr;
  QPushButton *m_refreshPortsButton = nullptr;
};

// Navigation Bar Class Declaration
class NavigationBar : public QWidget {
  Q_OBJECT

public:
  explicit NavigationBar(QWidget *parent = nullptr);
  ~NavigationBar();

  void addTab(const QString &icon, const QString &text);
  void setActiveTab(int index);
  int getCurrentTab() const;
  void clear();

signals:
  void tabClicked(int index);

private slots:
  void onTabButtonClicked();

private:
  void updateStyles();
  void setupLayout();

  QHBoxLayout *layout;
  QVector<QPushButton *> buttons;
  int activeTabIndex;
  const QString ACTIVE_COLOR = "#1A3C2F";
};

// Export Dialog Class Declaration
class ExportDialog : public QDialog {
    Q_OBJECT
public:
    explicit ExportDialog(QWidget *parent = nullptr);
    QString getSelectedFormat() const;

private:
    QString selectedFormat;
    QButtonGroup *buttonGroup;
    void setupUi();
};

// Todo item for maintenance list
struct TodoItem {
  QString machineId;
  QString machineName;
  QString machineType;
  QString tag;
  QString priority;
};

// Historique entry for live timer
struct HistoriqueEntry {
  QString machineName;
  QString currentState; // "ON", "OFF", "VEILLE"
  int secondsOn;
  int secondsOff;
  int secondsVeille;
};

// Machine Main Window Class
class machine : public QMainWindow {
  Q_OBJECT

public:
  machine(QWidget *parent = nullptr);
  ~machine();

protected:
  bool eventFilter(QObject *obj, QEvent *event) override;

signals:
  void backToMenu();

private slots:
  void onBackButtonClicked();
  void updateDateTime();
  void onThemeToggleClicked();
  void onLanguageToggleClicked();
  void onNavigationTabClicked(int index);
  void on_pushButton_enregistrer_machine_clicked();
  void on_btnReinitialiser_machine_clicked();
  void on_tableMachines_machine_doubleClicked(const QModelIndex &index);
  void on_btnSupprimerMachine_machine_clicked();
  void on_btnModifierMachine_machine_clicked();
  void on_btnFiltrer_clicked();
  void on_btnGenererStats_machine_clicked();
  void on_btnGenererQrMachine_clicked();
  void onMachineRequestedFromHttp(const QString &machineId);

private:
  struct MachineData {
    QString id;
    QString nom;
    QString type;
    QString etatMarche;
    double temperature;
    double charge;
    QString fonctionnement;
    QString alerte;
    QString criticite;
    QDate maintenance;
    QDate installation;
    int scoreSante;
    QDate miseAJour;
    QString responsable;
    QString refroidissement;
    QString tag;
    QString priorite;
  };
  struct MachineFormData {
    QString nom;
    QString type;
    QString etatMarche;
    double temperature = 0.0;
    double charge = 0.0;
    QString etatFonctionnement;
    QString typeAlerte;
    QString criticite;
    QDate derniereMaintenance;
    int scoreSante = 100;
  };

  MachineFormData collectMachineFormData() const;
  bool isMachineNameAvailable(const QString &name) const;
  bool insertMachine(const MachineFormData &data, QString *errorMessage = nullptr);
  bool saveMachineWithName(const QString &name, const MachineFormData &baseData);
  QStringList buildAvailableNameSuggestions(const QString &problematicName,
                                            int maxSuggestions = 6) const;
  void showRequiredNameDialog(const MachineFormData &baseData);
  void showDuplicateNameDialog(const MachineFormData &baseData,
                               const QString &problematicName);

  void setupNavigationBar();
  QString resolveLocalIpv4() const;
  QPixmap generateMachineQrPixmap(const QString &payload,
                                  int moduleSize = 4,
                                  int margin = 2) const;
  void resetQrPreviewLabel();
  void refreshNavigationTabs();
  void applyTheme();
  void applyLanguage();
  void setupTodoList();
  void reloadTodoFromMachineData();
  void refreshTodoList();
  QWidget *createTodoItemWidget(int index);

  Ui::machine *ui;
  QTimer *dateTimeTimer;
  NavigationBar *navigationBar;
  QVector<TodoItem> m_todoItems;
  QStandardItemModel *machineTableModel;
  QStandardItemModel *employeeTableModel;
  QStandardItemModel *historiqueTableModel;
  QStandardItemModel *statsComparisonModel;
  QList<MachineData> m_allMachines; // Stockage global pour le filtrage
  QString m_selectedMachineId;
  int m_selectedRow;
  QTimer *machineCardPollingTimer;
  QDialog *machineCardDialog;
  QTimer *countsTimer;
  QTimer *historiqueTimer;
  QVector<HistoriqueEntry> m_historiqueEntries;
  MachineServer *m_machineServer;
  Assistant *m_assistant;
  QString m_serverHostIp;
  bool m_isDarkMode = false;
  bool m_isFrench = true;
  QPushButton *m_btnVoiceAssistant = nullptr;
  QString m_lightStyleSheet;
  QString m_darkStyleSheet;
  void setupMachineTable();
  void setupEmployeesTable();
  void setupStatsComparisonTable();
  void populateStatsComparisonTable();
  void normalizeLowPriorityTags();
  void refreshAIMachineSelector();
  void updateAICarnetForSelectedMachine();
  void chargerMachines();
  void chargerEmployees();
  void refreshResponsableFilterOptions();
  void onEmployeeRowChosen(int row);
  void applyEmployeeSelectionLockState();
  void clearEmployeeSelection();
  bool machineHasEmployeeColumn() const;
  QString machineEmployeeColumnName() const;
  void appliquerFiltres();
    int m_selectedEmployeeId = -1;
    int m_selectedEmployeeRow = -1;
    bool m_employeeSelectionLocked = false;
  void chargerHistorique();
  void rechercherMachines();
  void exporterPDF();
  void afficherDialogExport();
  QList<QPair<QString, double>> loadStatisticsData(const QString &statsType) const;
  void renderStatisticsChart(const QList<QPair<QString, double>> &data,
                             const QString &statsType,
                             const QString &chartType);
  void clearStatsChartArea(const QString &message);
  void showMachineCard();
  void updateMachineCounts();
  void updateHistoriqueDisplay();
  bool selectMachineInTableById(const QString &machineId);
  void setupArduinoModule();
  void refreshArduinoEligibleMachines();
  void clearArduinoMachineButtons();
  void openArduinoMachineDialog(const QString &machineId);

private slots:
  void onArduinoTelemetryApplied(const QString &machineId);

private:
  QWidget *m_arduinoPanel = nullptr;
  QScrollArea *m_arduinoScroll = nullptr;
  QGridLayout *m_arduinoGrid = nullptr;
  QLabel *m_arduinoTitleLabel = nullptr;
  QLabel *m_arduinoEmptyLabel = nullptr;
  QPushButton *m_btnArduinoRefresh = nullptr;
  QTimer *m_arduinoRefreshTimer = nullptr;
  QVector<QPushButton *> m_arduinoButtons;
};
#endif // MACHINE_H
