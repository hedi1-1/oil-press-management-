#include "production.h"
#include "ui_production.h"

Production::Production(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Production)
{
    ui->setupUi(this);
    
    // Connect back button
    connect(ui->btnBackToMenu, &QPushButton::clicked, this, &Production::onBackButtonClicked);
}

Production::~Production()
{
    delete ui;
}

void Production::onBackButtonClicked()
{
    emit backToMenu();
    this->hide();
}
