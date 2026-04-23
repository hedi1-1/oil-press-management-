#ifndef MACHINE_H
#define MACHINE_H

#include <QButtonGroup>
#include <QCheckBox>
#include <QComboBox>
#include <QDate>
#include <QRadioButton>
#include <QProcess>
#include <QDialog>
#include <QEvent>
#include <QGridLayout>
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

class Chatbot;

// Machine Main Window Class
class machine : public QMainWindow {
  Q_OBJECT

public:
  machine(QWidget *parent = nullptr);
  ~machine();

protected:
  bool eventFilter(QObject *obj, QEvent *event) override;
  void resizeEvent(QResizeEvent *event) override;

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
  void refreshEligibleArduinoMachines();

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
  void setupArduinoTab();
  QString machineTableName() const;
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
  Chatbot *m_chatbot;
  QPushButton *m_btnChatbot;
  QString m_serverHostIp;
  bool m_isDarkMode = false;
  bool m_isFrench = true;
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
  QComboBox *m_comboArduinoMode = nullptr;
  QLabel *m_lblArduinoEligibleStatus = nullptr;
  QPushButton *m_btnArduinoRefresh = nullptr;
  QWidget *m_arduinoButtonsHost = nullptr;
  QGridLayout *m_arduinoButtonsLayout = nullptr;
  QTimer *m_arduinoEligibleRefreshTimer = nullptr;
  mutable QString m_machineTableCache;
};
#endif // MACHINE_H
