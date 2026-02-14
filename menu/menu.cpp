#include "menu.h"
#include "ui_menu.h"
#include "../production/production.h"
#include "../machine/machine.h"
#include "../userstaff/userstaff.h"
#include "../gestionclients/gestionclients.h"
#include "../stock/stock.h"
#include "../finance/finance.h"
#include <QMessageBox>
#include <QDateTime>
#include <QTimer>
#include <QLocale>

menu::menu(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::menu)
    , productionWindow(nullptr)
    , machineWindow(nullptr)
    , userstaffWindow(nullptr)
    , gestionClientsWindow(nullptr)
    , stockWindow(nullptr)
    , financeWindow(nullptr)
{
    ui->setupUi(this);
    
    // Connect button signals to slots
    connect(ui->btnClientManagement, &QPushButton::clicked, this, &menu::onClientManagementClicked);
    connect(ui->btnUserStaffManagement, &QPushButton::clicked, this, &menu::onUserStaffManagementClicked);
    connect(ui->btnStockManagement, &QPushButton::clicked, this, &menu::onStockManagementClicked);
    connect(ui->btnProductionManagement, &QPushButton::clicked, this, &menu::onProductionManagementClicked);
    connect(ui->btnMachineAlertManagement, &QPushButton::clicked, this, &menu::onMachineAlertManagementClicked);
    connect(ui->btnFinanceManagement, &QPushButton::clicked, this, &menu::onFinanceManagementClicked);
    
    // Setup timer for automatic date/time update
    dateTimeTimer = new QTimer(this);
    connect(dateTimeTimer, &QTimer::timeout, this, &menu::updateDateTime);
    updateDateTime(); // Initial update
    dateTimeTimer->start(1000); // Update every second
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
    if (userstaffWindow) {
        delete userstaffWindow;
    }
    if (gestionClientsWindow) {
        delete gestionClientsWindow;
    }
    if (stockWindow) {
        delete stockWindow;
    }
    if (financeWindow) {
        delete financeWindow;
    }
}

void menu::onClientManagementClicked()
{
    // Create and show the client management window
    if (!gestionClientsWindow) {
        gestionClientsWindow = new GestionClients();
        // Connect the back signal to show this menu again
        connect(gestionClientsWindow, &GestionClients::backToMenu, this, &menu::show);
    }
    
    gestionClientsWindow->show();
    this->hide(); // Hide the menu window
}

void menu::onUserStaffManagementClicked()
{
    // Create and show the user & staff management window
    if (!userstaffWindow) {
        userstaffWindow = new userstaff();
        // Connect the back signal to show this menu again
        connect(userstaffWindow, &userstaff::backToMenu, this, &menu::show);
    }
    
    userstaffWindow->show();
    this->hide(); // Hide the menu window
}

void menu::onStockManagementClicked()
{
    // Create and show the stock management window
    if (!stockWindow) {
        stockWindow = new Stock();
        // Connect the back signal to show this menu again
        connect(stockWindow, &Stock::backToMenu, this, &menu::show);
    }
    
    stockWindow->show();
    this->hide(); // Hide the menu window
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
    // Create and show the machine & alerts management window
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
    // Create and show the finance management window
    if (!financeWindow) {
        financeWindow = new Finance();
        // Connect the back signal to show this menu again
        connect(financeWindow, &Finance::backToMenu, this, &menu::show);
    }
    
    financeWindow->show();
    this->hide(); // Hide the menu window
}

void menu::updateDateTime()
{
    QDateTime currentDateTime = QDateTime::currentDateTime();
    QLocale frenchLocale(QLocale::French);
    
    QString dateText = frenchLocale.toString(currentDateTime.date(), "dd MMMM yyyy");
    QString timeText = currentDateTime.toString("HH:mm:ss");
    
    QString dateTimeText = QString("📅 %1 | 🕐 %2")
                          .arg(dateText)
                          .arg(timeText);
    ui->lblDateTime->setText(dateTimeText);
}
