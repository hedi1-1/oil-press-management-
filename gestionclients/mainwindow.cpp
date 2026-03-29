#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "gestionclientswidget.h"
#include "metierswidget.h"
#include <QDate>
#include <QTime>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setupWidgets();
    
    dateTimeTimer = new QTimer(this);
    connect(dateTimeTimer, &QTimer::timeout, this, &MainWindow::updateDateTime);
    dateTimeTimer->start(1000);
    updateDateTime();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupWidgets()
{
    gestionClientsWidget = new GestionClientsWidget(this);
    metiersWidget = new MetiersWidget(this);
    
    metiersWidget->setClientsData(&gestionClientsWidget->getClients());
    
    ui->tabWidget->addTab(gestionClientsWidget, "Gestion Clients");
    ui->tabWidget->addTab(metiersWidget, "Métiers");
}

void MainWindow::updateDateTime()
{
    QDate currentDate = QDate::currentDate();
    QTime currentTime = QTime::currentTime();
    
    QString dateTimeText = QString("%1 %2 %3  •  %4")
        .arg(currentDate.day(), 2, 10, QChar('0'))
        .arg(currentDate.toString("MMMM"))
        .arg(currentDate.year())
        .arg(currentTime.toString("HH:mm"));
    
    ui->dateTimeLabel->setText(dateTimeText);
    ui->calendarIconLabel->setText(QString::number(currentDate.day()));
}
