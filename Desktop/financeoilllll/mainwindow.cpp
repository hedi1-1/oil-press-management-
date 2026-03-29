#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "transactiontab.h"
#include "searchtab.h"
#include "statstab.h"
#include "journaltab.h"
#include "budgettab.h"
#include "advancedtab.h"
#include "exporttab.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QLabel>
#include <QPushButton>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    initializeUI();
    applyStyles();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initializeUI()
{
    // Créer le widget central
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *centralLayout = new QVBoxLayout(centralWidget);
    centralLayout->setContentsMargins(0, 0, 0, 0);
    centralLayout->setSpacing(0);

    // ===== HEADER =====
    QWidget *headerWidget = new QWidget();
    headerWidget->setMinimumHeight(100);
    QHBoxLayout *headerLayout = new QHBoxLayout(headerWidget);
    headerLayout->setContentsMargins(20, 15, 20, 15);
    headerLayout->setSpacing(20);

    // Bouton Retour
    QPushButton *btnReturn = new QPushButton("← Retour");
    btnReturn->setMaximumWidth(120);
    btnReturn->setMinimumHeight(40);
    btnReturn->setStyleSheet(
        "QPushButton { "
        "background-color: #d4a852; "
        "color: white; "
        "border: none; "
        "border-radius: 4px; "
        "font-weight: bold; "
        "font-size: 11pt; "
        "} "
        "QPushButton:hover { "
        "background-color: #c49743; "
        "}"
    );

    // Titre principal
    QVBoxLayout *titleLayout = new QVBoxLayout();
    QLabel *lblMainTitle = new QLabel("GESTION FINANCIÈRE");
    lblMainTitle->setStyleSheet("font-size: 24pt; font-weight: bold; color: #ffffff;");
    QLabel *lblSubtitle = new QLabel("TABLEAU DE BORD FINANCIER");
    lblSubtitle->setStyleSheet("font-size: 11pt; color: #d4a852; letter-spacing: 2px;");
    titleLayout->addWidget(lblMainTitle);
    titleLayout->addWidget(lblSubtitle);

    // Logo ou icône
    QLabel *lblIcon = new QLabel();
    QPixmap logoPixmap;
    
    // Essayer différents chemins possibles
    if (!logoPixmap.load(":/images/resources/pressiq_logo.png")) {
        if (!logoPixmap.load("resources/pressiq_logo.png")) {
            logoPixmap.load("./resources/pressiq_logo.png");
        }
    }
    
    if (!logoPixmap.isNull()) {
        logoPixmap = logoPixmap.scaledToWidth(120, Qt::SmoothTransformation);
        lblIcon->setPixmap(logoPixmap);
    } else {
        // Fallback : afficher un texte simple
        lblIcon->setText("PressIQ");
        lblIcon->setStyleSheet("font-size: 12pt; font-weight: bold; color: #27ae60;");
    }
    lblIcon->setAlignment(Qt::AlignCenter);
    lblIcon->setMaximumWidth(150);

    // Status online
    QVBoxLayout *statusLayout = new QVBoxLayout();
    statusLayout->addStretch();
    QLabel *lblStatus = new QLabel("🟢 EN LIGNE");
    lblStatus->setStyleSheet("color: #27ae60; font-weight: bold; font-size: 9pt;");
    lblStatus->setAlignment(Qt::AlignRight);
    statusLayout->addWidget(lblStatus);

    // Assembler le header
    headerLayout->addWidget(btnReturn);
    headerLayout->addWidget(lblIcon);
    headerLayout->addLayout(titleLayout);
    headerLayout->addStretch();
    headerLayout->addLayout(statusLayout);

    // Appliquer le style du header
    // Appliquer le style du header
    headerWidget->setStyleSheet("background-color: #1A3C2F;"); // border-bottom supprimé

    centralLayout->addWidget(headerWidget);

    // Créer le QTabWidget
    mainTabWidget = new QTabWidget(this);

    // Créer les tabs
    transactionTab = new TransactionTab();
    searchTab = new SearchTab();
    statsTab = new StatsTab();
    journalTab = new JournalTab();
    budgetTab = new BudgetTab();
    advancedTab = new AdvancedTab();
    exportTab = new ExportTab();

    // Ajouter les tabs
    mainTabWidget->addTab(transactionTab, "Transactions");
    mainTabWidget->addTab(searchTab, "Recherche & Tri");
    mainTabWidget->addTab(statsTab, "Statistiques");
    mainTabWidget->addTab(journalTab, "Journal");
    mainTabWidget->addTab(budgetTab, "Budget");
    mainTabWidget->addTab(advancedTab, "Analyse");
    mainTabWidget->addTab(exportTab, "Export");

    centralLayout->addWidget(mainTabWidget);
    setCentralWidget(centralWidget);

    // Connect transaction changes to stats update
    connect(transactionTab, &TransactionTab::transactionsChanged, 
            statsTab, &StatsTab::updateStats);
    
    // Connect transaction changes to journal update
    connect(transactionTab, &TransactionTab::transactionsChanged,
            journalTab, &JournalTab::chargerToutesTransactions);

    // Configuration de la fenêtre
    setWindowTitle("💰 Gestion Financière");
    resize(1400, 950);
}

void MainWindow::applyStyles()
{
    // Couleur vert professionnel #1A3C2F
    QString stylesheet = R"(
        QMainWindow {
            background-color: #ffffff;
        }

        QTabWidget::pane {
            border: 1px solid #ddd;
        }

        QTabBar::tab {
            background-color: #e8e8e8;
            color: #000;
            padding: 8px 20px;
            border: 1px solid #ccc;
            border-bottom: none;
            font-weight: bold;
        }

        QTabBar::tab:selected {
            background-color: #1A3C2F;
            color: #fff;
            border-bottom: 2px solid #1A3C2F;
        }

        QTabBar::tab:hover:!selected {
            background-color: #d0d0d0;
        }

        QGroupBox {
            color: #000;
            border: 2px solid #1A3C2F;
            border-radius: 5px;
            margin-top: 8px;
            padding-top: 8px;
            font-weight: bold;
        }

        QGroupBox::title {
            subcontrol-origin: margin;
            left: 10px;
            padding: 0 3px 0 3px;
            color: #1A3C2F;
        }

        QPushButton {
            background-color: #1A3C2F;
            color: white;
            border: none;
            border-radius: 4px;
            padding: 8px 16px;
            font-weight: bold;
            transition: background-color 0.3s;
        }

        QPushButton:hover {
            background-color: #0f2620;
        }

        QPushButton:pressed {
            background-color: #0a1812;
        }

        QLineEdit, QTextEdit {
            border: 1px solid #bbb;
            border-radius: 3px;
            padding: 5px;
            background-color: #fafafa;
            color: #000;
            font-weight: bold;
        }

        QLineEdit:focus, QTextEdit:focus {
            border: 2px solid #1A3C2F;
            background-color: #fff;
            color: #000;
        }

        QComboBox {
            border: 1px solid #bbb;
            border-radius: 3px;
            padding: 5px;
            background-color: #fafafa;
            color: #000;
            font-weight: bold;
        }

        QComboBox:focus {
            border: 2px solid #1A3C2F;
            color: #000;
        }

        QSpinBox, QDoubleSpinBox, QDateEdit {
            border: 1px solid #bbb;
            border-radius: 3px;
            padding: 5px;
            background-color: #fafafa;
            color: #000;
            font-weight: bold;
        }

        QSpinBox:focus, QDoubleSpinBox:focus, QDateEdit:focus {
            border: 2px solid #1A3C2F;
            color: #000;
        }

        QTableWidget, QTableView {
            border: 1px solid #ddd;
            gridline-color: #eee;
            background-color: #fff;
        }

        QTableWidget::item:selected {
            background-color: #1A3C2F;
            color: white;
        }

        QProgressBar {
            border: 2px solid #ddd;
            border-radius: 4px;
            background-color: #f0f0f0;
            text-align: center;
        }

        QProgressBar::chunk {
            background-color: #1A3C2F;
            border-radius: 2px;
        }

        QLabel {
            color: #000;
            font-weight: bold;
        }
    )";

    qApp->setStyle("Fusion");
    qApp->setStyleSheet(stylesheet);
}

