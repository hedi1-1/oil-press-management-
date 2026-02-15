#include "stock.h"
#include "ui_stock.h"
#include <QDate>
#include <QTime>

Stock::Stock(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Stock)
    , dateTimeTimer(nullptr)
{
    ui->setupUi(this);
    
    // Connect back button
    connect(ui->btnBackToMenu, &QPushButton::clicked, this, &Stock::onBackClicked);
    
    // Setup date/time timer
    dateTimeTimer = new QTimer(this);
    connect(dateTimeTimer, &QTimer::timeout, this, &Stock::updateDateTime);
    dateTimeTimer->start(1000);
    updateDateTime();
}

Stock::~Stock()
{
    if (dateTimeTimer) {
        dateTimeTimer->stop();
        delete dateTimeTimer;
    }
    delete ui;
}

void Stock::onBackClicked()
{
    emit backToMenu();
    this->hide();
}

void Stock::updateDateTime()
{
    QDate currentDate = QDate::currentDate();
    QTime currentTime = QTime::currentTime();
    
    // Format date in French
    QStringList moisFr = {"", "janvier", "février", "mars", "avril", "mai", "juin",
                          "juillet", "août", "septembre", "octobre", "novembre", "décembre"};
    
    QString dateStr = QString("%1 %2 %3")
                          .arg(currentDate.day())
                          .arg(moisFr[currentDate.month()])
                          .arg(currentDate.year());
    
    QString timeStr = currentTime.toString("HH:mm:ss");
    
    if (ui->lblDateTime) {
        ui->lblDateTime->setText(dateStr + " | " + timeStr);
    }
}
