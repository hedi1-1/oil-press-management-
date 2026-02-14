#include "login.h"
#include "ui_login.h"

Login::Login(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Login)
{
    ui->setupUi(this);
    setWindowTitle("PressIQ - Connexion");
    
    // Connect suivant button to navigation
    connect(ui->btnSuivant, &QPushButton::clicked, this, &Login::onSuivantClicked);
}

Login::~Login()
{
    delete ui;
}

void Login::onSuivantClicked()
{
    emit goToMenu();
    this->hide();
}
