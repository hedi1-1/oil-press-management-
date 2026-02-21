#ifndef MACHINE_H
#define MACHINE_H

#include <QCheckBox>
#include <QHBoxLayout>
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

// Machine Main Window Class
class machine : public QMainWindow {
  Q_OBJECT

public:
  machine(QWidget *parent = nullptr);
  ~machine();

signals:
  void backToMenu();

private slots:
  void onBackButtonClicked();
  void updateDateTime();
  void onNavigationTabClicked(int index);
  void on_pushButton_enregistrer_machine_clicked();
  void on_btnReinitialiser_clicked();

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
  void setupMachineTable();
  void chargerMachines();
};
#endif // MACHINE_H
