#ifndef MENU_H
#define MENU_H

#include <QMainWindow>
#include <QTimer>

// Forward declarations
class Production;
class machine;
class userstaff;
class GestionClients;
class Stock;
class Finance;

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
    void updateDateTime();

private:
    Ui::menu *ui;
    Production *productionWindow;
    machine *machineWindow;
    userstaff *userstaffWindow;
    GestionClients *gestionClientsWindow;
    Stock *stockWindow;
    Finance *financeWindow;
    QTimer *dateTimeTimer;
};
#endif // MENU_H
