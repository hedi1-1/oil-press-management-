#ifndef MENU_H
#define MENU_H

#include <QMainWindow>

// Forward declarations
class Production;
class machine;
class userstaff;

QT_BEGIN_NAMESPACE
namespace Ui {
class menu;
}
QT_END_NAMESPACE

class menu : public QMainWindow
{
    Q_OBJECT

public:
    menu(QWidget *parent = nullptr);
    ~menu();

private slots:
    void onClientManagementClicked();
    void onUserStaffManagementClicked();
    void onStockManagementClicked();
    void onProductionManagementClicked();
    void onMachineAlertManagementClicked();
    void onFinanceManagementClicked();

private:
    Ui::menu *ui;
    Production *productionWindow;
    machine *machineWindow;
    userstaff *userstaffWindow;
};
#endif // MENU_H
