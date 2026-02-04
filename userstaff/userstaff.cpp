#include "userstaff.h"
#include "ui_userstaff.h"

userstaff::userstaff(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::userstaff)
{
    ui->setupUi(this);
    setWindowTitle("PressIQ - User & Staff Management");
    connect(ui->btnBack, &QPushButton::clicked, this, &userstaff::onBackClicked);
}

userstaff::~userstaff()
{
    delete ui;
}

void userstaff::onBackClicked()
{
    emit backToMenu();
    this->hide();
}
