#include "login.h"
#include "ui_login.h"

#include <QMessageBox>

Login::Login(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Login)
{
    ui->setupUi(this);

    setFixedSize(380, 420);

    // Title styling
    ui->titleLabel->setStyleSheet(
        "font-size: 26px;"
        "font-weight: bold;"
        "color: white;"
    );

    ui->subtitleLabel->setStyleSheet(
        "font-size: 14px;"
        "color: #aaaaaa;"
    );

    // Inputs styling
    ui->usernameEdit->setFixedHeight(40);
    ui->passwordEdit->setFixedHeight(40);

    ui->usernameEdit->setStyleSheet(
        "background:#1e1e1e;"
        "border:1px solid #333;"
        "border-radius:8px;"
        "padding:10px;"
        "color:white;"
    );

    ui->passwordEdit->setStyleSheet(ui->usernameEdit->styleSheet());

    // Button styling
    ui->loginButton->setFixedHeight(45);
    ui->loginButton->setStyleSheet(
        "QPushButton {"
        " background:#4a90e2;"
        " border:none;"
        " border-radius:10px;"
        " color:white;"
        " font-size:15px;"
        " font-weight:bold;"
        "}"
        "QPushButton:hover { background:#357abd; }"
        "QPushButton:pressed { background:#2c5f99; }"
    );
}

Login::~Login()
{
    delete ui;
}

void Login::on_loginButton_clicked()
{
    QString username = ui->usernameEdit->text();
    QString password = ui->passwordEdit->text();

    if (username.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please fill all fields.");
        return;
    }

    // 🔐 Backend logic later
    QMessageBox::information(this, "Login", "Login successful!");
}
