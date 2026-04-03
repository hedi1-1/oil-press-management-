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
  QString machineName;
  QString fabricant;
  QString priority; // "Urgent" or "Normal"
  bool done;
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
  void onNavigationTabClicked(int index);
  void on_pushButton_enregistrer_machine_clicked();
  void on_btnReinitialiser_machine_clicked();
  void on_tableMachines_machine_doubleClicked(const QModelIndex &index);
  void on_btnSupprimerMachine_machine_clicked();
  void on_btnModifierMachine_machine_clicked();
  void on_btnFiltrer_clicked();

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
  void setupTodoList();
  void refreshTodoList();
  void addTodoItem(const QString &machineName, const QString &fabricant,
                   const QString &priority);
  void onTodoCheckToggled(int index, bool checked);
  void showAddTodoDialog();
  QWidget *createTodoItemWidget(int index);

  Ui::machine *ui;
  QTimer *dateTimeTimer;
  NavigationBar *navigationBar;
  QVector<TodoItem> m_todoItems;
  QStandardItemModel *machineTableModel;
  QStandardItemModel *employeeTableModel;
  QStandardItemModel *historiqueTableModel;
  QList<MachineData> m_allMachines; // Stockage global pour le filtrage
  QString m_selectedMachineId;
  int m_selectedRow;
  QTimer *machineCardPollingTimer;
  QDialog *machineCardDialog;
  QTimer *countsTimer;
  QTimer *historiqueTimer;
  QVector<HistoriqueEntry> m_historiqueEntries;
  void setupMachineTable();
  void setupEmployeesTable();
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
  void showMachineCard();
  void updateMachineCounts();
  void updateHistoriqueDisplay();
};
#endif // MACHINE_H
