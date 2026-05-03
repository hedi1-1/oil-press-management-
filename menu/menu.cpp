#include "menu.h"
#include "ui_menu.h"
#include <QCoreApplication>
#include "../production/production.h"
#include "../machine/machine.h"
#include "../userstaff/userstaff.h"
#include "../gestionclients/gestionclients.h"
#include "../stock/stock.h"
#include "../finance/finance/finance.h"
#include <QMessageBox>
#include <QDateTime>
#include <QTimer>
#include <QLocale>
#include <QPixmap>
#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QProcess>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QKeyEvent>

menu::menu(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::menu)
    , productionWindow(nullptr)
    , machineWindow(nullptr)
    , userstaffWindow(nullptr)
    , gestionClientsWindow(nullptr)
    , stockWindow(nullptr)
    , financeWindow(nullptr)
    , m_userStaffGateBusy(false)
{
    ui->setupUi(this);
    {
        QPixmap logo;
        for (const QString &p : QStringList{":/logo.png", "logo.png", "../production/logo.png",
             QCoreApplication::applicationDirPath() + "/../../logo.png",
             QCoreApplication::applicationDirPath() + "/../../../logo.png",
             QCoreApplication::applicationDirPath() + "/../../../production/logo.png"}) {
            logo = QPixmap(p);
            if (!logo.isNull()) { qDebug() << "[menu] Logo loaded from:" << p; break; }
        }
        if (!logo.isNull())
            ui->lblLogo->setPixmap(logo.scaledToHeight(70, Qt::SmoothTransformation));
        else
            qDebug() << "[menu] WARNING: Logo not found anywhere!";
    }

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
    if (m_userStaffGateBusy) return;

    m_userStaffGateBusy = true;
    const bool granted = showUserStaffAccessGate();
    m_userStaffGateBusy = false;
    if (!granted) {
        return;
    }

    // Create and show the user & staff management window
    if (!userstaffWindow) {
        userstaffWindow = new userstaff();
        // Connect the back signal to show this menu again
        connect(userstaffWindow, &userstaff::backToMenu, this, &menu::show);
    }
    
    userstaffWindow->show();
    userstaffWindow->raise();
    userstaffWindow->activateWindow();
    this->hide(); // Hide the menu window
}

bool menu::showUserStaffAccessGate()
{
    QDialog dlg(this);
    dlg.setModal(true);
    dlg.setWindowTitle("Secure Access Gateway");
    dlg.setMinimumSize(620, 400);
    dlg.setStyleSheet(
        "QDialog {"
        "  background: qlineargradient(x1:0,y1:0,x2:1,y2:1,"
        "      stop:0 #040c17, stop:0.55 #082035, stop:1 #0f2f20);"
        "}"
        "QFrame#rfidCard {"
        "  background: rgba(4, 18, 28, 224);"
        "  border: 1px solid #2d7197;"
        "  border-radius: 14px;"
        "}"
        "QLabel#gateTitle {"
        "  color: #8de0ff;"
        "  font-family: Consolas, 'Courier New', monospace;"
        "  font-size: 24px;"
        "  font-weight: 900;"
        "  letter-spacing: 1px;"
        "}"
        "QLabel#gateSubtitle {"
        "  color: #d7f9ff;"
        "  font-size: 12px;"
        "}"
        "QLineEdit#scanInput {"
        "  background: #03131f;"
        "  color: #d9fbff;"
        "  border: 1px solid #2498c8;"
        "  border-radius: 8px;"
        "  padding: 10px 12px;"
        "  font-size: 14px;"
        "  font-family: Consolas, 'Courier New', monospace;"
        "}"
        "QPushButton#scanBtn {"
        "  background: #0c7fb3;"
        "  color: white;"
        "  border: 1px solid #55bde8;"
        "  border-radius: 8px;"
        "  padding: 10px 14px;"
        "  font-weight: 700;"
        "}"
        "QPushButton#scanBtn:hover { background: #0a709e; }"
        "QPushButton#abortBtn {"
        "  background: #2a3944;"
        "  color: #f2f7fa;"
        "  border: 1px solid #617a8d;"
        "  border-radius: 8px;"
        "  padding: 10px 14px;"
        "  font-weight: 700;"
        "}"
        "QPushButton#abortBtn:hover { background: #22313c; }"
        "QLabel#statusLabel {"
        "  border-radius: 8px;"
        "  padding: 8px 10px;"
        "  font-family: Consolas, 'Courier New', monospace;"
        "  font-size: 13px;"
        "  font-weight: 800;"
        "}"
        "QLabel#hintLabel {"
        "  color: #a9dbeb;"
        "  font-size: 11px;"
        "}"
    );

    auto *root = new QVBoxLayout(&dlg);
    root->setContentsMargins(20, 20, 20, 20);
    root->setSpacing(12);

    auto *card = new QFrame(&dlg);
    card->setObjectName("rfidCard");
    auto *cardLayout = new QVBoxLayout(card);
    cardLayout->setContentsMargins(18, 16, 18, 16);
    cardLayout->setSpacing(12);

    auto *title = new QLabel("ADMIN PRIVILEGES REQUIRED", card);
    title->setObjectName("gateTitle");
    auto *subtitle = new QLabel(
        "USE ADMIN KEYPASS (Arduino D2 button)",
        card);
    subtitle->setObjectName("gateSubtitle");
    subtitle->setWordWrap(true);

    auto *statusLabel = new QLabel("PENDING: ADMIN CHECK (5s)", card);
    statusLabel->setObjectName("statusLabel");
    statusLabel->setStyleSheet("QLabel#statusLabel { background: #3f2b14; color: #ffde8a; border: 1px solid #a46d1f; }");

    auto *hint = new QLabel(
        "Auto-grant enabled for demo: access will be granted in 5 seconds.",
        card);
    hint->setObjectName("hintLabel");
    hint->setWordWrap(true);

    auto *buttonsRow = new QHBoxLayout();
    buttonsRow->setSpacing(10);
    auto *bypassBtn = new QPushButton("Bypass", card);
    bypassBtn->setObjectName("scanBtn");
    buttonsRow->addWidget(bypassBtn, 1);
    auto *abortBtn = new QPushButton("Abort", card);
    abortBtn->setObjectName("abortBtn");
    buttonsRow->addWidget(abortBtn, 1);

    cardLayout->addWidget(title);
    cardLayout->addWidget(subtitle);
    cardLayout->addWidget(statusLabel);
    cardLayout->addWidget(hint);
    cardLayout->addLayout(buttonsRow);
    root->addWidget(card);

    bool approved = false;
    QProcess serialWatcher;
    const QString preferredPort = QString::fromLocal8Bit(qgetenv("PRESSIQ_USERSTAFF_ARDUINO_PORT")).trimmed().isEmpty()
                                      ? QStringLiteral("COM5")
                                      : QString::fromLocal8Bit(qgetenv("PRESSIQ_USERSTAFF_ARDUINO_PORT")).trimmed();
    const QString powershellExe = QStringLiteral("C:/Windows/System32/WindowsPowerShell/v1.0/powershell.exe");

    auto triggerServoTurn = [&]() {
        if (serialWatcher.state() != QProcess::NotRunning) {
            serialWatcher.kill();
            serialWatcher.waitForFinished(600);
        }

        QSerialPort cmdPort;
        cmdPort.setPortName(preferredPort);
        cmdPort.setBaudRate(QSerialPort::Baud9600);
        cmdPort.setDataBits(QSerialPort::Data8);
        cmdPort.setParity(QSerialPort::NoParity);
        cmdPort.setStopBits(QSerialPort::OneStop);
        cmdPort.setFlowControl(QSerialPort::NoFlowControl);
        if (cmdPort.open(QIODevice::ReadWrite)) {
            cmdPort.write("SERVO_TURN\n");
            cmdPort.waitForBytesWritten(500);
            cmdPort.close();
        }
    };

    auto markApproved = [&approved, statusLabel, hint, &dlg, &triggerServoTurn](const QString &detail) {
        if (approved) return;
        approved = true;
        triggerServoTurn();
        statusLabel->setText(detail);
        statusLabel->setStyleSheet("QLabel#statusLabel { background: #0f3e24; color: #9bffcc; border: 1px solid #1de27d; }");
        hint->setText("Access granted. Opening UserStaff page...");
        QTimer::singleShot(250, &dlg, [&dlg]() { dlg.done(QDialog::Accepted); });
    };

    auto activateBypassLikeEnter = [&]() {
        if (!bypassBtn->isEnabled()) {
            return;
        }
        bypassBtn->setFocus();
        QKeyEvent pressEnter(QEvent::KeyPress, Qt::Key_Return, Qt::NoModifier);
        QKeyEvent releaseEnter(QEvent::KeyRelease, Qt::Key_Return, Qt::NoModifier);
        QCoreApplication::sendEvent(bypassBtn, &pressEnter);
        QCoreApplication::sendEvent(bypassBtn, &releaseEnter);
        // Ensure activation even if style/platform ignores key event synthesis.
        bypassBtn->click();
    };

    auto startSerialWatcher = [&]() {
        if (approved || serialWatcher.state() != QProcess::NotRunning) {
            return;
        }

        const QString psScript =
            "$ErrorActionPreference='Stop';"
            "$port='" + preferredPort + "';"
            "try {"
            "  $sp = New-Object System.IO.Ports.SerialPort $port,9600,None,8,one;"
            "  $sp.ReadTimeout = 200;"
            "  $sp.NewLine = \"`n\";"
            "  $sp.DtrEnable = $false;"
            "  $sp.RtsEnable = $false;"
            "  $sp.Open();"
            "  Write-Output ('WATCHER_STARTED:' + $port);"
            "  Write-Output ('PORT_OPEN:' + $port);"
            "  $buffer = '';"
            "  $granted = $false;"
            "  while ($true) {"
            "    try {"
            "      if ($sp.BytesToRead -gt 0) {"
            "        $buffer += $sp.ReadExisting();"
            "        $buffer = $buffer -replace \"`r\",'';"
            "        while ($buffer.Contains(\"`n\")) {"
            "          $idx = $buffer.IndexOf(\"`n\");"
            "          $line = $buffer.Substring(0,$idx).Trim();"
            "          $buffer = $buffer.Substring($idx + 1);"
            "          if ($line.Length -gt 0) {"
            "            Write-Output ('LINE:' + $line);"
            "            if ($line -match 'ENTRY_GRANTED:BUTTON|STATUS: BUTTON_PRESSED|SIGNAL: BUTTON_OK|STATUS: BUTTON_EDGE=') {"
            "              Write-Output 'ACCESS_GRANTED';"
            "              $granted = $true;"
            "              break;"
            "            }"
            "          }"
            "        }"
            "      } else {"
            "        [System.Threading.Thread]::Sleep(40);"
            "      }"
            "      if ($granted) { break; }"
            "    } catch [System.TimeoutException] {"
            "      [System.Threading.Thread]::Sleep(40);"
            "    } catch [System.IO.IOException] {"
            "      [System.Threading.Thread]::Sleep(80);"
            "    } catch {"
            "      [System.Threading.Thread]::Sleep(80);"
            "    }"
            "  }"
            "  $sp.Close();"
            "} catch {"
            "  Write-Output ('PORT_ERROR:' + $_.Exception.Message);"
            "}";

        statusLabel->setText("PENDING: USE ADMIN KEYPASS");
        statusLabel->setStyleSheet("QLabel#statusLabel { background: #3f2b14; color: #ffde8a; border: 1px solid #a46d1f; }");
        hint->setText("Starting serial watcher on " + preferredPort + "...");

        serialWatcher.setProgram(powershellExe);
        serialWatcher.setArguments(QStringList() << "-NoProfile" << "-NonInteractive" << "-ExecutionPolicy" << "Bypass" << "-Command" << psScript);
        serialWatcher.setProcessChannelMode(QProcess::MergedChannels);
        serialWatcher.start();

        if (!serialWatcher.waitForStarted(2500)) {
            statusLabel->setText("PENDING: USE ADMIN KEYPASS (watcher start failed)");
            statusLabel->setStyleSheet("QLabel#statusLabel { background: #3f2b14; color: #ffde8a; border: 1px solid #a46d1f; }");
            hint->setText("Watcher start error on " + preferredPort + ": " + serialWatcher.errorString());
        }
    };

    auto handleWatcherOutput = [&]() {
        const QString out = QString::fromLocal8Bit(serialWatcher.readAll());
        const QStringList lines = out.split('\n', Qt::SkipEmptyParts);
        for (const QString &raw : lines) {
            const QString line = raw.trimmed();
            if (line.isEmpty()) {
                continue;
            }
            if (line.startsWith("WATCHER_STARTED:", Qt::CaseInsensitive)) {
                statusLabel->setText("PENDING: USE ADMIN KEYPASS");
                statusLabel->setStyleSheet("QLabel#statusLabel { background: #3f2b14; color: #ffde8a; border: 1px solid #a46d1f; }");
                hint->setText("Watcher started on " + line.mid(QString("WATCHER_STARTED:").size()) + ". Waiting for Arduino signal...");
            } else if (line.startsWith("PORT_OPEN:", Qt::CaseInsensitive)) {
                statusLabel->setText("PENDING: USE ADMIN KEYPASS on " + line.mid(QString("PORT_OPEN:").size()));
                statusLabel->setStyleSheet("QLabel#statusLabel { background: #3f2b14; color: #ffde8a; border: 1px solid #a46d1f; }");
                hint->setText("Press Arduino D2 button now.");
            } else if (line.startsWith("LINE:", Qt::CaseInsensitive)) {
                hint->setText("Signal: " + line.mid(QString("LINE:").size()));
            } else if (line.startsWith("ACCESS_GRANTED", Qt::CaseInsensitive)) {
                activateBypassLikeEnter();
                break;
            } else if (line.startsWith("PORT_ERROR:", Qt::CaseInsensitive)) {
                statusLabel->setText("PENDING: USE ADMIN KEYPASS (open failed)");
                statusLabel->setStyleSheet("QLabel#statusLabel { background: #3f2b14; color: #ffde8a; border: 1px solid #a46d1f; }");
                hint->setText("Open error on " + preferredPort + ": " + line.mid(QString("PORT_ERROR:").size()));
            }
        }
    };

    QObject::connect(&serialWatcher, &QProcess::readyRead, &dlg, handleWatcherOutput);
    QObject::connect(&serialWatcher, &QProcess::errorOccurred, &dlg, [&](QProcess::ProcessError) {
        statusLabel->setText("PENDING: USE ADMIN KEYPASS (watcher error)");
        statusLabel->setStyleSheet("QLabel#statusLabel { background: #3f2b14; color: #ffde8a; border: 1px solid #a46d1f; }");
        hint->setText("Watcher process error: " + serialWatcher.errorString());
    });
    QObject::connect(&serialWatcher, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), &dlg,
                     [&](int code, QProcess::ExitStatus) {
        if (!approved) {
            statusLabel->setText("PENDING: USE ADMIN KEYPASS (watcher exited)");
            statusLabel->setStyleSheet("QLabel#statusLabel { background: #3f2b14; color: #ffde8a; border: 1px solid #a46d1f; }");
            hint->setText("Watcher exited (code=" + QString::number(code) + "). Retrying...");
        }
    });

    QObject::connect(bypassBtn, &QPushButton::clicked, &dlg, [&]() {
        markApproved("ACCESS GRANTED (BYPASS)");
    });
    QObject::connect(abortBtn, &QPushButton::clicked, &dlg, [&dlg]() { dlg.reject(); });

    QTimer retryTimer(&dlg);
    retryTimer.setInterval(1200);
    QObject::connect(&retryTimer, &QTimer::timeout, &dlg, [&]() {
        if (!approved && serialWatcher.state() == QProcess::NotRunning) {
            startSerialWatcher();
        }
    });
    retryTimer.start();

    startSerialWatcher();

    // Deadline demo mode: auto-approve after 5 seconds.
    QTimer::singleShot(5000, &dlg, [&]() {
        if (!approved) {
            markApproved("ACCESS GRANTED");
        }
    });

    QObject::connect(&dlg, &QDialog::finished, &dlg, [&]() {
        if (serialWatcher.state() != QProcess::NotRunning) {
            serialWatcher.kill();
            serialWatcher.waitForFinished(500);
        }
    });

    abortBtn->setAutoDefault(false);
    abortBtn->setDefault(false);
    bypassBtn->setAutoDefault(true);
    bypassBtn->setDefault(true);
    bypassBtn->setFocus();
    const int result = dlg.exec();

    return result == QDialog::Accepted && approved;
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
