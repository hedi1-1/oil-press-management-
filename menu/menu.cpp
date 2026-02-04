#include "menu.h"
#include "ui_menu.h"
#include "../production/production.h"
#include "../machine/machine.h"
#include <QMessageBox>

menu::menu(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::menu)
    , productionWindow(nullptr)
    , machineWindow(nullptr)
{
    ui->setupUi(this);
    
    // Connect button signals to slots
    connect(ui->btnClientManagement, &QPushButton::clicked, this, &menu::onClientManagementClicked);
    connect(ui->btnUserStaffManagement, &QPushButton::clicked, this, &menu::onUserStaffManagementClicked);
    connect(ui->btnStockManagement, &QPushButton::clicked, this, &menu::onStockManagementClicked);
    connect(ui->btnProductionManagement, &QPushButton::clicked, this, &menu::onProductionManagementClicked);
    connect(ui->btnMachineAlertManagement, &QPushButton::clicked, this, &menu::onMachineAlertManagementClicked);
    connect(ui->btnFinanceManagement, &QPushButton::clicked, this, &menu::onFinanceManagementClicked);
}

menu::~menu()
{
    delete ui;
    if (productionWindow) {
        delete productionWindow;
    }
    if (machineWindow) {
        delete machineWindow;
    }
}

void menu::onClientManagementClicked()
{
    QMessageBox::information(this, "Module non disponible", 
                            "Le module 'Gestion des clients' sera disponible prochainement.");
}

void menu::onUserStaffManagementClicked()
{
    QMessageBox::information(this, "Module non disponible", 
                            "Le module 'Gestion des utilisateurs et du personnel' sera disponible prochainement.");
}

void menu::onStockManagementClicked()
{
    QMessageBox::information(this, "Module non disponible", 
                            "Le module 'Gestion du stock' sera disponible prochainement.");
}

void menu::onProductionManagementClicked()
{
    // Create and show the production management window
    if (!productionWindow) {
        productionWindow = new Production();
        // Connect the back signal to show this menu again
        connect(productionWindow, &Production::backToMenu, this, &menu::show);
    }
    
    productionWindow->show();
    this->hide(); // Hide the menu window
}

void menu::onMachineAlertManagementClicked()
{
    // Create and show the machine management window
    if (!machineWindow) {
        machineWindow = new machine();
        // Connect the back signal to show this menu again
        connect(machineWindow, &machine::backToMenu, this, &menu::show);
    }
    
    machineWindow->show();
    this->hide(); // Hide the menu window
}

void menu::onFinanceManagementClicked()
{
    QMessageBox::information(this, "Module non disponible", 
                            "Le module 'Gestion financière' sera disponible prochainement.");
}
