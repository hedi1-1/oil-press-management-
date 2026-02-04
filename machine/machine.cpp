#include "machine.h"
#include "ui_machine.h"

machine::machine(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::machine)
{
    ui->setupUi(this);
    
    // Connect back button
    connect(ui->btnBackToMenu, &QPushButton::clicked, this, &machine::onBackButtonClicked);
    
    // Add toolbar actions
    ui->toolBarMachines->addAction("➕ Ajouter");
    ui->toolBarMachines->addAction("✏️ Modifier");
    ui->toolBarMachines->addAction("🗑️ Supprimer");
    ui->toolBarMachines->addSeparator();
    ui->toolBarMachines->addAction("🔄 Actualiser");
    ui->toolBarMachines->addAction("📤 Exporter");
}

machine::~machine()
{
    delete ui;
}

void machine::onBackButtonClicked()
{
    emit backToMenu();
    this->hide();
}
