#include "gestionclients.h"
#include "ui_gestionclients.h"
#include <QCoreApplication>
#include <QDateTime>
#include <QPixmap>

GestionClients::GestionClients(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::GestionClients)
{
    ui->setupUi(this);
    {
        QPixmap logo;
        for (const QString &p : QStringList{":/logo.png", "logo.png", "../production/logo.png",
             QCoreApplication::applicationDirPath() + "/../../logo.png",
             QCoreApplication::applicationDirPath() + "/../../../production/logo.png"}) {
            logo = QPixmap(p);
            if (!logo.isNull()) break;
        }
        if (!logo.isNull())
            ui->lblLogo->setPixmap(logo.scaledToHeight(50, Qt::SmoothTransformation));
    }

    // ── Create child widgets and add as tabs ──
    gestionClientsWidget = new GestionClientsWidget(this);
    metiersWidget = new MetiersWidget(this);

    // Share client data between widgets
    metiersWidget->setClientsData(&gestionClientsWidget->getClients());

    // Add tabs with icons
    ui->tabWidget->addTab(gestionClientsWidget, QString::fromUtf8("\xF0\x9F\x91\xA5 Gestion Clients"));
    ui->tabWidget->addTab(metiersWidget, QString::fromUtf8("\xF0\x9F\x94\x8D M\xC3\xA9tiers"));

    // ── Back button → hide + emit backToMenu ──
    connect(ui->btnBackToMenu, &QPushButton::clicked, this, [this]() {
        this->hide();
        emit backToMenu();
    });

    // ── Date/Time timer ──
    dateTimeTimer = new QTimer(this);
    connect(dateTimeTimer, &QTimer::timeout, this, &GestionClients::updateDateTime);
    dateTimeTimer->start(1000);
    updateDateTime();

    // ── Update stat cards when clients change ──
    connect(gestionClientsWidget, &GestionClientsWidget::clientsUpdated,
            this, &GestionClients::updateStatCards);

    // Initial stats update
    updateStatCards();
}

GestionClients::~GestionClients()
{
    delete ui;
}

void GestionClients::updateDateTime()
{
    QDateTime now = QDateTime::currentDateTime();
    // Format: 📅 18 Février 2026 | 🕐 14:30
    QString dateStr = now.toString("dd MMMM yyyy");
    QString timeStr = now.toString("HH:mm");
    ui->lblDateTime->setText(QString::fromUtf8("\xF0\x9F\x93\x85 %1 | \xF0\x9F\x95\x90 %2")
                             .arg(dateStr, timeStr));
}

void GestionClients::updateStatCards()
{
    const QList<Client> &clients = gestionClientsWidget->getClients();

    int total = clients.size();
    int importants = 0;
    int standards = 0;
    double totalOlives = 0;

    for (const Client &c : clients) {
        if (c.statut == "Important")
            importants++;
        else
            standards++;
        totalOlives += c.total_olives_livrees;
    }

    ui->lblValueTotal->setText(QString::number(total));
    ui->lblValueImportant->setText(QString::number(importants));
    ui->lblValueStandard->setText(QString::number(standards));
    ui->lblValueOlives->setText(QString::number(totalOlives, 'f', 0) + " kg");
}

