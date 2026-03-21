#ifndef MACHINE_H
#define MACHINE_H

#include <QCheckBox>
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

private:
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
  QStandardItemModel *historiqueTableModel;
  QString m_selectedMachineId;
  int m_selectedRow;
  QTimer *machineCardPollingTimer;
  QDialog *machineCardDialog;
  QTimer *countsTimer;
  QTimer *historiqueTimer;
  QVector<HistoriqueEntry> m_historiqueEntries;
  void setupMachineTable();
  void chargerMachines();
  void chargerHistorique();
  void rechercherMachines();
  void exporterPDF();
  void showMachineCard();
  void updateMachineCounts();
  void updateHistoriqueDisplay();
};
#endif // MACHINE_H
