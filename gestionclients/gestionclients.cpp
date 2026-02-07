#include "gestionclients.h"
#include "ui_gestionclients.h"
#include <QDate>
#include <QTime>

GestionClients::GestionClients(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::GestionClients)
{
    ui->setupUi(this);
    setWindowTitle("Gestion des Clients - PRESSIQ");
    
    // Connecter le bouton retour
    connect(ui->pushButton_retour, &QPushButton::clicked, this, &GestionClients::onBackClicked);
    
    // Mise à jour de la date/heure
    updateDateTime();
}

GestionClients::~GestionClients()
{
    delete ui;
}

void GestionClients::onBackClicked()
{
    emit backToMenu();
    this->hide();
}

void GestionClients::updateDateTime()
{
    QDate currentDate = QDate::currentDate();
    QTime currentTime = QTime::currentTime();
    
    QStringList monthNames = {"Janvier", "Février", "Mars", "Avril", "Mai", "Juin",
                              "Juillet", "Août", "Septembre", "Octobre", "Novembre", "Décembre"};
    
    ui->calendarIconLabel->setText(QString("%1").arg(currentDate.day(), 2, 10, QChar('0')));
    
    QString formattedDateTime = QString("%1 %2 %3  •  %4")
                                    .arg(currentDate.day(), 2, 10, QChar('0'))
                                    .arg(monthNames[currentDate.month() - 1])
                                    .arg(currentDate.year())
                                    .arg(currentTime.toString("HH:mm"));
    
    ui->dateTimeLabel->setText(formattedDateTime);
}

