/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.7.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDateEdit>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLCDNumber>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QVBoxLayout *mainVerticalLayout;
    QWidget *headerWidget;
    QHBoxLayout *headerLayout;
    QLabel *labelLogoImage;
    QLabel *labelLogo;
    QSpacerItem *headerSpacer;
    QLabel *labelStatus;
    QTabWidget *tabWidgetStock;
    QWidget *tabGestionStock;
    QVBoxLayout *verticalLayout_1;
    QGroupBox *groupInfoStock;
    QGridLayout *gridLayoutInfo;
    QLabel *labelIdStock;
    QLineEdit *lineEditIdStock;
    QLabel *labelTypeHuile;
    QComboBox *comboTypeHuile;
    QLabel *labelQuantite;
    QDoubleSpinBox *spinQuantite;
    QLabel *labelSeuilAlerte;
    QSpinBox *spinSeuilAlerte;
    QLabel *labelDateMaj;
    QDateEdit *dateEditMaj;
    QLabel *labelEmplacement;
    QLineEdit *lineEditEmplacement;
    QLabel *labelEtatStock;
    QComboBox *comboEtatStock;
    QHBoxLayout *layoutBtnCRUD;
    QPushButton *btnAjouter;
    QPushButton *btnModifier;
    QPushButton *btnSupprimer;
    QPushButton *btnConsulter;
    QSpacerItem *horizontalSpacer_1;
    QGroupBox *groupTableStocks;
    QVBoxLayout *verticalLayout_table;
    QTableWidget *tableStocks;
    QWidget *tabTri;
    QVBoxLayout *verticalLayout_2;
    QGroupBox *groupOptionsTri;
    QHBoxLayout *layoutTri;
    QLabel *labelTrierSelon;
    QComboBox *comboTriCritere;
    QPushButton *btnAppliquerTri;
    QSpacerItem *horizontalSpacer_2;
    QGroupBox *groupTableTriee;
    QVBoxLayout *layoutTableTriee;
    QTableWidget *tableTriee;
    QWidget *tabRecherche;
    QVBoxLayout *verticalLayout_3;
    QGroupBox *groupCriteres;
    QGridLayout *gridLayoutRecherche;
    QLabel *labelRechType;
    QComboBox *comboRechercheType;
    QLabel *labelRechEtat;
    QComboBox *comboRechercheEtat;
    QLabel *labelRechEmpl;
    QLineEdit *lineRechercheEmplacement;
    QHBoxLayout *layoutBtnRecherche;
    QPushButton *btnRechercher;
    QPushButton *btnReinitialiser;
    QSpacerItem *horizontalSpacer_3;
    QGroupBox *groupResultats;
    QVBoxLayout *layoutResultats;
    QTableWidget *tableResultats;
    QWidget *tabExport;
    QVBoxLayout *verticalLayout_4;
    QGroupBox *groupFormat;
    QHBoxLayout *layoutFormat;
    QRadioButton *radioPDF;
    QRadioButton *radioExcel;
    QSpacerItem *horizontalSpacer_4;
    QGroupBox *groupOptions;
    QVBoxLayout *layoutOptions;
    QCheckBox *checkStats;
    QCheckBox *checkDateTime;
    QHBoxLayout *layoutNomFichier;
    QLabel *labelNomFichier;
    QLineEdit *lineNomFichier;
    QPushButton *btnExporter;
    QSpacerItem *verticalSpacer_1;
    QWidget *tabStatistiques;
    QVBoxLayout *verticalLayout_5;
    QGroupBox *groupResume;
    QHBoxLayout *layoutResume;
    QLabel *labelTotalArticles;
    QLabel *labelValeurTotale;
    QSpacerItem *horizontalSpacer_5;
    QGroupBox *groupOptionsStats;
    QHBoxLayout *layoutOptionsStats;
    QLabel *labelStatsPar;
    QComboBox *comboStatsPar;
    QPushButton *btnCalculer;
    QSpacerItem *horizontalSpacer_6;
    QGroupBox *groupStats;
    QVBoxLayout *layoutStats;
    QTextEdit *textResumeStats;
    QWidget *tabHistorique;
    QVBoxLayout *verticalLayout_6;
    QGroupBox *groupFiltrePeriode;
    QHBoxLayout *layoutFiltrePeriode;
    QLabel *labelDateDebut;
    QDateEdit *dateDebut;
    QLabel *labelDateFin;
    QDateEdit *dateFin;
    QPushButton *btnAfficherHistorique;
    QSpacerItem *horizontalSpacer_7;
    QGroupBox *groupHistorique;
    QVBoxLayout *layoutHistorique;
    QTableWidget *tableHistorique;
    QWidget *tabEtatStock;
    QVBoxLayout *verticalLayout_7;
    QGroupBox *groupEtatGlobal;
    QVBoxLayout *layoutEtatGlobal;
    QLabel *labelEtatGlobal;
    QProgressBar *progressStock;
    QGroupBox *groupProduitsCritiques;
    QVBoxLayout *layoutProduitsCritiques;
    QListWidget *listProduitsCritiques;
    QWidget *tabTempsReel;
    QVBoxLayout *verticalLayout_8;
    QGroupBox *groupTempsReel;
    QVBoxLayout *layoutTempsReel;
    QLabel *labelQuantiteActuelle;
    QLCDNumber *lcdVolume;
    QLabel *labelUnite;
    QLabel *labelDerniereMaj;
    QLabel *labelInfoRefresh;
    QSpacerItem *verticalSpacer_2;
    QWidget *tabAlertes;
    QVBoxLayout *verticalLayout_9;
    QGroupBox *groupConfigAlertes;
    QHBoxLayout *layoutConfigAlertes;
    QLabel *labelSeuilDefini;
    QPushButton *btnVerifierAlertes;
    QGroupBox *groupStocksSousSeuil;
    QVBoxLayout *layoutStocksSousSeuil;
    QListWidget *listStocksSousSeuil;
    QWidget *tabTendances;
    QVBoxLayout *verticalLayout_10;
    QGroupBox *groupOptionsTendances;
    QHBoxLayout *layoutOptionsTendances;
    QLabel *labelPeriode;
    QComboBox *comboPeriode;
    QPushButton *btnAnalyserTendances;
    QSpacerItem *horizontalSpacer_8;
    QGroupBox *groupGraphique;
    QVBoxLayout *layoutGraphique;
    QLabel *labelPlaceholderChart;
    QGroupBox *groupAnalyseAuto;
    QVBoxLayout *layoutAnalyseAuto;
    QTextEdit *textAnalyseTendances;
    QWidget *tabRapports;
    QVBoxLayout *verticalLayout_11;
    QGroupBox *groupSelectionRapport;
    QVBoxLayout *layoutSelectionRapport;
    QTableWidget *tableRapportSelection;
    QHBoxLayout *layoutBtnRapport;
    QPushButton *btnGenererRapport;
    QPushButton *btnExportRapportPDF;
    QSpacerItem *horizontalSpacer_9;
    QGroupBox *groupApercuRapport;
    QVBoxLayout *layoutApercuRapport;
    QTextEdit *textApercuRapport;
    QWidget *tabArduino;
    QVBoxLayout *verticalLayout_12;
    QGroupBox *groupConnexion;
    QHBoxLayout *layoutConnexion;
    QLabel *labelEtatConnexion;
    QPushButton *btnSynchroniser;
    QSpacerItem *horizontalSpacer_10;
    QGroupBox *groupCapteur;
    QVBoxLayout *layoutCapteur;
    QLabel *labelValeurCapteur;
    QGroupBox *groupLogs;
    QVBoxLayout *layoutLogs;
    QTextEdit *textLogsArduino;
    QWidget *footerWidget;
    QHBoxLayout *footerLayout;
    QLabel *labelStatut;
    QSpacerItem *footerSpacer;
    QLabel *labelCopyright;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(1441, 616);
        MainWindow->setMinimumSize(QSize(1441, 616));
        MainWindow->setStyleSheet(QString::fromUtf8("QMainWindow {\n"
"    background-color: #f5f5f5;\n"
"}"));
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        mainVerticalLayout = new QVBoxLayout(centralwidget);
        mainVerticalLayout->setSpacing(0);
        mainVerticalLayout->setObjectName("mainVerticalLayout");
        mainVerticalLayout->setContentsMargins(0, 0, 0, 0);
        headerWidget = new QWidget(centralwidget);
        headerWidget->setObjectName("headerWidget");
        headerWidget->setMinimumSize(QSize(0, 80));
        headerWidget->setMaximumSize(QSize(16777215, 80));
        headerWidget->setStyleSheet(QString::fromUtf8("QWidget#headerWidget {\n"
"    background: qlineargradient(x1:0, y1:0, x2:1, y2:0,\n"
"        stop:0 #0F2A1F, stop:1 #1A3C2F);\n"
"}"));
        headerLayout = new QHBoxLayout(headerWidget);
        headerLayout->setObjectName("headerLayout");
        headerLayout->setContentsMargins(20, 10, 20, 10);
        labelLogoImage = new QLabel(headerWidget);
        labelLogoImage->setObjectName("labelLogoImage");
        labelLogoImage->setMinimumSize(QSize(60, 60));
        labelLogoImage->setMaximumSize(QSize(60, 60));
        labelLogoImage->setPixmap(QPixmap(QString::fromUtf8(":/images/logo.png")));
        labelLogoImage->setScaledContents(true);

        headerLayout->addWidget(labelLogoImage);

        labelLogo = new QLabel(headerWidget);
        labelLogo->setObjectName("labelLogo");
        labelLogo->setStyleSheet(QString::fromUtf8("color: white;"));

        headerLayout->addWidget(labelLogo);

        headerSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        headerLayout->addItem(headerSpacer);

        labelStatus = new QLabel(headerWidget);
        labelStatus->setObjectName("labelStatus");
        labelStatus->setStyleSheet(QString::fromUtf8("color: #2D5A4A; font-weight: bold; font-size: 14px;"));

        headerLayout->addWidget(labelStatus);


        mainVerticalLayout->addWidget(headerWidget);

        tabWidgetStock = new QTabWidget(centralwidget);
        tabWidgetStock->setObjectName("tabWidgetStock");
        QFont font;
        font.setStrikeOut(false);
        tabWidgetStock->setFont(font);
        tabWidgetStock->setStyleSheet(QString::fromUtf8("QTabWidget::pane {\n"
"    border: 1px solid #1A3C2F;\n"
"    border-radius: 5px;\n"
"    background: white;\n"
"}\n"
"QTabBar::tab {\n"
"    background: #f0f0f0;\n"
"    border: 1px solid #ccc;\n"
"    padding: 8px 16px;\n"
"    margin-right: 2px;\n"
"    border-top-left-radius: 4px;\n"
"    border-top-right-radius: 4px;\n"
"}\n"
"QTabBar::tab:selected {\n"
"    background: #1A3C2F;\n"
"    color: white;\n"
"}\n"
"QTabBar::tab:hover:!selected {\n"
"    background: #3D7A6A;\n"
"}"));
        tabGestionStock = new QWidget();
        tabGestionStock->setObjectName("tabGestionStock");
        verticalLayout_1 = new QVBoxLayout(tabGestionStock);
        verticalLayout_1->setObjectName("verticalLayout_1");
        verticalLayout_1->setContentsMargins(20, 20, 20, 20);
        groupInfoStock = new QGroupBox(tabGestionStock);
        groupInfoStock->setObjectName("groupInfoStock");
        groupInfoStock->setStyleSheet(QString::fromUtf8("QGroupBox {\n"
"    font-weight: bold;\n"
"    border: 2px solid #1A3C2F;\n"
"    border-radius: 8px;\n"
"    margin-top: 10px;\n"
"    padding-top: 10px;\n"
"    background-color: #F5FAF7;\n"
"}\n"
"QGroupBox::title {\n"
"    subcontrol-origin: margin;\n"
"    left: 10px;\n"
"    padding: 0 5px;\n"
"    color: #1A3C2F;\n"
"}"));
        gridLayoutInfo = new QGridLayout(groupInfoStock);
        gridLayoutInfo->setObjectName("gridLayoutInfo");
        labelIdStock = new QLabel(groupInfoStock);
        labelIdStock->setObjectName("labelIdStock");

        gridLayoutInfo->addWidget(labelIdStock, 0, 0, 1, 1);

        lineEditIdStock = new QLineEdit(groupInfoStock);
        lineEditIdStock->setObjectName("lineEditIdStock");
        lineEditIdStock->setStyleSheet(QString::fromUtf8("QLineEdit {\n"
"    border: 1px solid #ccc;\n"
"    border-radius: 4px;\n"
"    padding: 6px;\n"
"    background: white;\n"
"}\n"
"QLineEdit:focus {\n"
"    border: 2px solid #1A3C2F;\n"
"}"));

        gridLayoutInfo->addWidget(lineEditIdStock, 0, 1, 1, 1);

        labelTypeHuile = new QLabel(groupInfoStock);
        labelTypeHuile->setObjectName("labelTypeHuile");

        gridLayoutInfo->addWidget(labelTypeHuile, 1, 0, 1, 1);

        comboTypeHuile = new QComboBox(groupInfoStock);
        comboTypeHuile->addItem(QString());
        comboTypeHuile->addItem(QString());
        comboTypeHuile->addItem(QString());
        comboTypeHuile->setObjectName("comboTypeHuile");
        comboTypeHuile->setStyleSheet(QString::fromUtf8("QComboBox {\n"
"    border: 1px solid #ccc;\n"
"    border-radius: 4px;\n"
"    padding: 6px;\n"
"    background: white;\n"
"}\n"
"QComboBox:focus {\n"
"    border: 2px solid #1A3C2F;\n"
"}"));

        gridLayoutInfo->addWidget(comboTypeHuile, 1, 1, 1, 1);

        labelQuantite = new QLabel(groupInfoStock);
        labelQuantite->setObjectName("labelQuantite");

        gridLayoutInfo->addWidget(labelQuantite, 2, 0, 1, 1);

        spinQuantite = new QDoubleSpinBox(groupInfoStock);
        spinQuantite->setObjectName("spinQuantite");
        spinQuantite->setStyleSheet(QString::fromUtf8("QDoubleSpinBox {\n"
"    border: 1px solid #ccc;\n"
"    border-radius: 4px;\n"
"    padding: 6px;\n"
"    background: white;\n"
"}\n"
"QDoubleSpinBox:focus {\n"
"    border: 2px solid #1A3C2F;\n"
"}"));
        spinQuantite->setDecimals(2);
        spinQuantite->setMaximum(100000.000000000000000);

        gridLayoutInfo->addWidget(spinQuantite, 2, 1, 1, 1);

        labelSeuilAlerte = new QLabel(groupInfoStock);
        labelSeuilAlerte->setObjectName("labelSeuilAlerte");

        gridLayoutInfo->addWidget(labelSeuilAlerte, 3, 0, 1, 1);

        spinSeuilAlerte = new QSpinBox(groupInfoStock);
        spinSeuilAlerte->setObjectName("spinSeuilAlerte");
        spinSeuilAlerte->setStyleSheet(QString::fromUtf8("QSpinBox {\n"
"    border: 1px solid #ccc;\n"
"    border-radius: 4px;\n"
"    padding: 6px;\n"
"    background: white;\n"
"}\n"
"QSpinBox:focus {\n"
"    border: 2px solid #1A3C2F;\n"
"}"));
        spinSeuilAlerte->setMaximum(10000);

        gridLayoutInfo->addWidget(spinSeuilAlerte, 3, 1, 1, 1);

        labelDateMaj = new QLabel(groupInfoStock);
        labelDateMaj->setObjectName("labelDateMaj");

        gridLayoutInfo->addWidget(labelDateMaj, 4, 0, 1, 1);

        dateEditMaj = new QDateEdit(groupInfoStock);
        dateEditMaj->setObjectName("dateEditMaj");
        dateEditMaj->setStyleSheet(QString::fromUtf8("QDateEdit {\n"
"    border: 1px solid #ccc;\n"
"    border-radius: 4px;\n"
"    padding: 6px;\n"
"    background: white;\n"
"}\n"
"QDateEdit:focus {\n"
"    border: 2px solid #1A3C2F;\n"
"}"));
        dateEditMaj->setCalendarPopup(true);

        gridLayoutInfo->addWidget(dateEditMaj, 4, 1, 1, 1);

        labelEmplacement = new QLabel(groupInfoStock);
        labelEmplacement->setObjectName("labelEmplacement");

        gridLayoutInfo->addWidget(labelEmplacement, 5, 0, 1, 1);

        lineEditEmplacement = new QLineEdit(groupInfoStock);
        lineEditEmplacement->setObjectName("lineEditEmplacement");
        lineEditEmplacement->setStyleSheet(QString::fromUtf8("QLineEdit {\n"
"    border: 1px solid #ccc;\n"
"    border-radius: 4px;\n"
"    padding: 6px;\n"
"    background: white;\n"
"}\n"
"QLineEdit:focus {\n"
"    border: 2px solid #1A3C2F;\n"
"}"));

        gridLayoutInfo->addWidget(lineEditEmplacement, 5, 1, 1, 1);

        labelEtatStock = new QLabel(groupInfoStock);
        labelEtatStock->setObjectName("labelEtatStock");

        gridLayoutInfo->addWidget(labelEtatStock, 6, 0, 1, 1);

        comboEtatStock = new QComboBox(groupInfoStock);
        comboEtatStock->addItem(QString());
        comboEtatStock->addItem(QString());
        comboEtatStock->addItem(QString());
        comboEtatStock->setObjectName("comboEtatStock");
        comboEtatStock->setStyleSheet(QString::fromUtf8("QComboBox {\n"
"    border: 1px solid #ccc;\n"
"    border-radius: 4px;\n"
"    padding: 6px;\n"
"    background: white;\n"
"}\n"
"QComboBox:focus {\n"
"    border: 2px solid #1A3C2F;\n"
"}"));

        gridLayoutInfo->addWidget(comboEtatStock, 6, 1, 1, 1);


        verticalLayout_1->addWidget(groupInfoStock);

        layoutBtnCRUD = new QHBoxLayout();
        layoutBtnCRUD->setSpacing(10);
        layoutBtnCRUD->setObjectName("layoutBtnCRUD");
        btnAjouter = new QPushButton(tabGestionStock);
        btnAjouter->setObjectName("btnAjouter");
        btnAjouter->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"    background-color: #1A3C2F;\n"
"    color: white;\n"
"    border: none;\n"
"    padding: 8px 16px;\n"
"    border-radius: 4px;\n"
"    font-weight: bold;\n"
"}\n"
"QPushButton:hover {\n"
"    background-color: #234D3D;\n"
"}\n"
"QPushButton:pressed {\n"
"    background-color: #0F2A1F;\n"
"}"));

        layoutBtnCRUD->addWidget(btnAjouter);

        btnModifier = new QPushButton(tabGestionStock);
        btnModifier->setObjectName("btnModifier");
        btnModifier->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"    background-color: #1A3C2F;\n"
"    color: white;\n"
"    border: none;\n"
"    padding: 8px 16px;\n"
"    border-radius: 4px;\n"
"    font-weight: bold;\n"
"}\n"
"QPushButton:hover {\n"
"    background-color: #234D3D;\n"
"}\n"
"QPushButton:pressed {\n"
"    background-color: #0F2A1F;\n"
"}"));

        layoutBtnCRUD->addWidget(btnModifier);

        btnSupprimer = new QPushButton(tabGestionStock);
        btnSupprimer->setObjectName("btnSupprimer");
        btnSupprimer->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"    background-color: #c62828;\n"
"    color: white;\n"
"    border: none;\n"
"    padding: 8px 16px;\n"
"    border-radius: 4px;\n"
"    font-weight: bold;\n"
"}\n"
"QPushButton:hover {\n"
"    background-color: #d32f2f;\n"
"}\n"
"QPushButton:pressed {\n"
"    background-color: #b71c1c;\n"
"}"));

        layoutBtnCRUD->addWidget(btnSupprimer);

        btnConsulter = new QPushButton(tabGestionStock);
        btnConsulter->setObjectName("btnConsulter");
        btnConsulter->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"    background-color: #1A3C2F;\n"
"    color: white;\n"
"    border: none;\n"
"    padding: 8px 16px;\n"
"    border-radius: 4px;\n"
"    font-weight: bold;\n"
"}\n"
"QPushButton:hover {\n"
"    background-color: #234D3D;\n"
"}\n"
"QPushButton:pressed {\n"
"    background-color: #0F2A1F;\n"
"}"));

        layoutBtnCRUD->addWidget(btnConsulter);

        horizontalSpacer_1 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        layoutBtnCRUD->addItem(horizontalSpacer_1);


        verticalLayout_1->addLayout(layoutBtnCRUD);

        groupTableStocks = new QGroupBox(tabGestionStock);
        groupTableStocks->setObjectName("groupTableStocks");
        groupTableStocks->setStyleSheet(QString::fromUtf8("QGroupBox {\n"
"    font-weight: bold;\n"
"    border: 2px solid #1A3C2F;\n"
"    border-radius: 8px;\n"
"    margin-top: 10px;\n"
"    padding-top: 10px;\n"
"    background-color: #F5FAF7;\n"
"}\n"
"QGroupBox::title {\n"
"    subcontrol-origin: margin;\n"
"    left: 10px;\n"
"    padding: 0 5px;\n"
"    color: #1A3C2F;\n"
"}"));
        verticalLayout_table = new QVBoxLayout(groupTableStocks);
        verticalLayout_table->setObjectName("verticalLayout_table");
        tableStocks = new QTableWidget(groupTableStocks);
        if (tableStocks->columnCount() < 7)
            tableStocks->setColumnCount(7);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        tableStocks->setHorizontalHeaderItem(0, __qtablewidgetitem);
        QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
        tableStocks->setHorizontalHeaderItem(1, __qtablewidgetitem1);
        QTableWidgetItem *__qtablewidgetitem2 = new QTableWidgetItem();
        tableStocks->setHorizontalHeaderItem(2, __qtablewidgetitem2);
        QTableWidgetItem *__qtablewidgetitem3 = new QTableWidgetItem();
        tableStocks->setHorizontalHeaderItem(3, __qtablewidgetitem3);
        QTableWidgetItem *__qtablewidgetitem4 = new QTableWidgetItem();
        tableStocks->setHorizontalHeaderItem(4, __qtablewidgetitem4);
        QTableWidgetItem *__qtablewidgetitem5 = new QTableWidgetItem();
        tableStocks->setHorizontalHeaderItem(5, __qtablewidgetitem5);
        QTableWidgetItem *__qtablewidgetitem6 = new QTableWidgetItem();
        tableStocks->setHorizontalHeaderItem(6, __qtablewidgetitem6);
        tableStocks->setObjectName("tableStocks");
        tableStocks->setStyleSheet(QString::fromUtf8("QTableWidget {\n"
"    border: 1px solid #ddd;\n"
"    border-radius: 4px;\n"
"    gridline-color: #e0e0e0;\n"
"}\n"
"QTableWidget::item:selected {\n"
"    background-color: #D0E5DC;\n"
"    color: black;\n"
"}\n"
"QHeaderView::section {\n"
"    background-color: #1A3C2F;\n"
"    color: white;\n"
"    padding: 8px;\n"
"    border: none;\n"
"    font-weight: bold;\n"
"}"));
        tableStocks->setAlternatingRowColors(true);
        tableStocks->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);

        verticalLayout_table->addWidget(tableStocks);


        verticalLayout_1->addWidget(groupTableStocks);

        tabWidgetStock->addTab(tabGestionStock, QString());
        tabTri = new QWidget();
        tabTri->setObjectName("tabTri");
        verticalLayout_2 = new QVBoxLayout(tabTri);
        verticalLayout_2->setObjectName("verticalLayout_2");
        verticalLayout_2->setContentsMargins(20, 20, 20, 20);
        groupOptionsTri = new QGroupBox(tabTri);
        groupOptionsTri->setObjectName("groupOptionsTri");
        groupOptionsTri->setStyleSheet(QString::fromUtf8("QGroupBox {\n"
"    font-weight: bold;\n"
"    border: 2px solid #1A3C2F;\n"
"    border-radius: 8px;\n"
"    margin-top: 10px;\n"
"    padding-top: 10px;\n"
"    background-color: #F5FAF7;\n"
"}\n"
"QGroupBox::title {\n"
"    subcontrol-origin: margin;\n"
"    left: 10px;\n"
"    padding: 0 5px;\n"
"    color: #1A3C2F;\n"
"}"));
        layoutTri = new QHBoxLayout(groupOptionsTri);
        layoutTri->setObjectName("layoutTri");
        labelTrierSelon = new QLabel(groupOptionsTri);
        labelTrierSelon->setObjectName("labelTrierSelon");

        layoutTri->addWidget(labelTrierSelon);

        comboTriCritere = new QComboBox(groupOptionsTri);
        comboTriCritere->addItem(QString());
        comboTriCritere->addItem(QString());
        comboTriCritere->addItem(QString());
        comboTriCritere->addItem(QString());
        comboTriCritere->setObjectName("comboTriCritere");
        comboTriCritere->setStyleSheet(QString::fromUtf8("QComboBox {\n"
"    border: 1px solid #ccc;\n"
"    border-radius: 4px;\n"
"    padding: 6px;\n"
"    background: white;\n"
"    min-width: 200px;\n"
"}"));

        layoutTri->addWidget(comboTriCritere);

        btnAppliquerTri = new QPushButton(groupOptionsTri);
        btnAppliquerTri->setObjectName("btnAppliquerTri");
        btnAppliquerTri->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"    background-color: #1A3C2F;\n"
"    color: white;\n"
"    border: none;\n"
"    padding: 8px 16px;\n"
"    border-radius: 4px;\n"
"    font-weight: bold;\n"
"}\n"
"QPushButton:hover {\n"
"    background-color: #234D3D;\n"
"}"));

        layoutTri->addWidget(btnAppliquerTri);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        layoutTri->addItem(horizontalSpacer_2);


        verticalLayout_2->addWidget(groupOptionsTri);

        groupTableTriee = new QGroupBox(tabTri);
        groupTableTriee->setObjectName("groupTableTriee");
        groupTableTriee->setStyleSheet(QString::fromUtf8("QGroupBox {\n"
"    font-weight: bold;\n"
"    border: 2px solid #1A3C2F;\n"
"    border-radius: 8px;\n"
"    margin-top: 10px;\n"
"    padding-top: 10px;\n"
"    background-color: #F5FAF7;\n"
"}\n"
"QGroupBox::title {\n"
"    subcontrol-origin: margin;\n"
"    left: 10px;\n"
"    padding: 0 5px;\n"
"    color: #1A3C2F;\n"
"}"));
        layoutTableTriee = new QVBoxLayout(groupTableTriee);
        layoutTableTriee->setObjectName("layoutTableTriee");
        tableTriee = new QTableWidget(groupTableTriee);
        if (tableTriee->columnCount() < 7)
            tableTriee->setColumnCount(7);
        QTableWidgetItem *__qtablewidgetitem7 = new QTableWidgetItem();
        tableTriee->setHorizontalHeaderItem(0, __qtablewidgetitem7);
        QTableWidgetItem *__qtablewidgetitem8 = new QTableWidgetItem();
        tableTriee->setHorizontalHeaderItem(1, __qtablewidgetitem8);
        QTableWidgetItem *__qtablewidgetitem9 = new QTableWidgetItem();
        tableTriee->setHorizontalHeaderItem(2, __qtablewidgetitem9);
        QTableWidgetItem *__qtablewidgetitem10 = new QTableWidgetItem();
        tableTriee->setHorizontalHeaderItem(3, __qtablewidgetitem10);
        QTableWidgetItem *__qtablewidgetitem11 = new QTableWidgetItem();
        tableTriee->setHorizontalHeaderItem(4, __qtablewidgetitem11);
        QTableWidgetItem *__qtablewidgetitem12 = new QTableWidgetItem();
        tableTriee->setHorizontalHeaderItem(5, __qtablewidgetitem12);
        QTableWidgetItem *__qtablewidgetitem13 = new QTableWidgetItem();
        tableTriee->setHorizontalHeaderItem(6, __qtablewidgetitem13);
        tableTriee->setObjectName("tableTriee");
        tableTriee->setStyleSheet(QString::fromUtf8("QTableWidget {\n"
"    border: 1px solid #ddd;\n"
"    border-radius: 4px;\n"
"    gridline-color: #e0e0e0;\n"
"}\n"
"QTableWidget::item:selected {\n"
"    background-color: #D0E5DC;\n"
"    color: black;\n"
"}\n"
"QHeaderView::section {\n"
"    background-color: #1A3C2F;\n"
"    color: white;\n"
"    padding: 8px;\n"
"    border: none;\n"
"    font-weight: bold;\n"
"}"));
        tableTriee->setAlternatingRowColors(true);
        tableTriee->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);

        layoutTableTriee->addWidget(tableTriee);


        verticalLayout_2->addWidget(groupTableTriee);

        tabWidgetStock->addTab(tabTri, QString());
        tabRecherche = new QWidget();
        tabRecherche->setObjectName("tabRecherche");
        verticalLayout_3 = new QVBoxLayout(tabRecherche);
        verticalLayout_3->setObjectName("verticalLayout_3");
        verticalLayout_3->setContentsMargins(20, 20, 20, 20);
        groupCriteres = new QGroupBox(tabRecherche);
        groupCriteres->setObjectName("groupCriteres");
        groupCriteres->setStyleSheet(QString::fromUtf8("QGroupBox {\n"
"    font-weight: bold;\n"
"    border: 2px solid #1A3C2F;\n"
"    border-radius: 8px;\n"
"    margin-top: 10px;\n"
"    padding-top: 10px;\n"
"    background-color: #F5FAF7;\n"
"}\n"
"QGroupBox::title {\n"
"    subcontrol-origin: margin;\n"
"    left: 10px;\n"
"    padding: 0 5px;\n"
"    color: #1A3C2F;\n"
"}"));
        gridLayoutRecherche = new QGridLayout(groupCriteres);
        gridLayoutRecherche->setObjectName("gridLayoutRecherche");
        labelRechType = new QLabel(groupCriteres);
        labelRechType->setObjectName("labelRechType");

        gridLayoutRecherche->addWidget(labelRechType, 0, 0, 1, 1);

        comboRechercheType = new QComboBox(groupCriteres);
        comboRechercheType->addItem(QString());
        comboRechercheType->addItem(QString());
        comboRechercheType->addItem(QString());
        comboRechercheType->addItem(QString());
        comboRechercheType->setObjectName("comboRechercheType");
        comboRechercheType->setStyleSheet(QString::fromUtf8("QComboBox {\n"
"    border: 1px solid #ccc;\n"
"    border-radius: 4px;\n"
"    padding: 6px;\n"
"    background: white;\n"
"}"));

        gridLayoutRecherche->addWidget(comboRechercheType, 0, 1, 1, 1);

        labelRechEtat = new QLabel(groupCriteres);
        labelRechEtat->setObjectName("labelRechEtat");

        gridLayoutRecherche->addWidget(labelRechEtat, 1, 0, 1, 1);

        comboRechercheEtat = new QComboBox(groupCriteres);
        comboRechercheEtat->addItem(QString());
        comboRechercheEtat->addItem(QString());
        comboRechercheEtat->addItem(QString());
        comboRechercheEtat->addItem(QString());
        comboRechercheEtat->setObjectName("comboRechercheEtat");
        comboRechercheEtat->setStyleSheet(QString::fromUtf8("QComboBox {\n"
"    border: 1px solid #ccc;\n"
"    border-radius: 4px;\n"
"    padding: 6px;\n"
"    background: white;\n"
"}"));

        gridLayoutRecherche->addWidget(comboRechercheEtat, 1, 1, 1, 1);

        labelRechEmpl = new QLabel(groupCriteres);
        labelRechEmpl->setObjectName("labelRechEmpl");

        gridLayoutRecherche->addWidget(labelRechEmpl, 2, 0, 1, 1);

        lineRechercheEmplacement = new QLineEdit(groupCriteres);
        lineRechercheEmplacement->setObjectName("lineRechercheEmplacement");
        lineRechercheEmplacement->setStyleSheet(QString::fromUtf8("QLineEdit {\n"
"    border: 1px solid #ccc;\n"
"    border-radius: 4px;\n"
"    padding: 6px;\n"
"    background: white;\n"
"}"));

        gridLayoutRecherche->addWidget(lineRechercheEmplacement, 2, 1, 1, 1);

        layoutBtnRecherche = new QHBoxLayout();
        layoutBtnRecherche->setObjectName("layoutBtnRecherche");
        btnRechercher = new QPushButton(groupCriteres);
        btnRechercher->setObjectName("btnRechercher");
        btnRechercher->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"    background-color: #1A3C2F;\n"
"    color: white;\n"
"    border: none;\n"
"    padding: 8px 16px;\n"
"    border-radius: 4px;\n"
"    font-weight: bold;\n"
"}\n"
"QPushButton:hover {\n"
"    background-color: #234D3D;\n"
"}"));

        layoutBtnRecherche->addWidget(btnRechercher);

        btnReinitialiser = new QPushButton(groupCriteres);
        btnReinitialiser->setObjectName("btnReinitialiser");
        btnReinitialiser->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"    background-color: #757575;\n"
"    color: white;\n"
"    border: none;\n"
"    padding: 8px 16px;\n"
"    border-radius: 4px;\n"
"    font-weight: bold;\n"
"}\n"
"QPushButton:hover {\n"
"    background-color: #9e9e9e;\n"
"}"));

        layoutBtnRecherche->addWidget(btnReinitialiser);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        layoutBtnRecherche->addItem(horizontalSpacer_3);


        gridLayoutRecherche->addLayout(layoutBtnRecherche, 3, 0, 1, 2);


        verticalLayout_3->addWidget(groupCriteres);

        groupResultats = new QGroupBox(tabRecherche);
        groupResultats->setObjectName("groupResultats");
        groupResultats->setStyleSheet(QString::fromUtf8("QGroupBox {\n"
"    font-weight: bold;\n"
"    border: 2px solid #1A3C2F;\n"
"    border-radius: 8px;\n"
"    margin-top: 10px;\n"
"    padding-top: 10px;\n"
"    background-color: #F5FAF7;\n"
"}\n"
"QGroupBox::title {\n"
"    subcontrol-origin: margin;\n"
"    left: 10px;\n"
"    padding: 0 5px;\n"
"    color: #1A3C2F;\n"
"}"));
        layoutResultats = new QVBoxLayout(groupResultats);
        layoutResultats->setObjectName("layoutResultats");
        tableResultats = new QTableWidget(groupResultats);
        if (tableResultats->columnCount() < 7)
            tableResultats->setColumnCount(7);
        QTableWidgetItem *__qtablewidgetitem14 = new QTableWidgetItem();
        tableResultats->setHorizontalHeaderItem(0, __qtablewidgetitem14);
        QTableWidgetItem *__qtablewidgetitem15 = new QTableWidgetItem();
        tableResultats->setHorizontalHeaderItem(1, __qtablewidgetitem15);
        QTableWidgetItem *__qtablewidgetitem16 = new QTableWidgetItem();
        tableResultats->setHorizontalHeaderItem(2, __qtablewidgetitem16);
        QTableWidgetItem *__qtablewidgetitem17 = new QTableWidgetItem();
        tableResultats->setHorizontalHeaderItem(3, __qtablewidgetitem17);
        QTableWidgetItem *__qtablewidgetitem18 = new QTableWidgetItem();
        tableResultats->setHorizontalHeaderItem(4, __qtablewidgetitem18);
        QTableWidgetItem *__qtablewidgetitem19 = new QTableWidgetItem();
        tableResultats->setHorizontalHeaderItem(5, __qtablewidgetitem19);
        QTableWidgetItem *__qtablewidgetitem20 = new QTableWidgetItem();
        tableResultats->setHorizontalHeaderItem(6, __qtablewidgetitem20);
        tableResultats->setObjectName("tableResultats");
        tableResultats->setStyleSheet(QString::fromUtf8("QTableWidget {\n"
"    border: 1px solid #ddd;\n"
"    border-radius: 4px;\n"
"    gridline-color: #e0e0e0;\n"
"}\n"
"QHeaderView::section {\n"
"    background-color: #1A3C2F;\n"
"    color: white;\n"
"    padding: 8px;\n"
"    border: none;\n"
"    font-weight: bold;\n"
"}"));
        tableResultats->setAlternatingRowColors(true);
        tableResultats->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);

        layoutResultats->addWidget(tableResultats);


        verticalLayout_3->addWidget(groupResultats);

        tabWidgetStock->addTab(tabRecherche, QString());
        tabExport = new QWidget();
        tabExport->setObjectName("tabExport");
        verticalLayout_4 = new QVBoxLayout(tabExport);
        verticalLayout_4->setObjectName("verticalLayout_4");
        verticalLayout_4->setContentsMargins(20, 20, 20, 20);
        groupFormat = new QGroupBox(tabExport);
        groupFormat->setObjectName("groupFormat");
        groupFormat->setStyleSheet(QString::fromUtf8("QGroupBox {\n"
"    font-weight: bold;\n"
"    border: 2px solid #1A3C2F;\n"
"    border-radius: 8px;\n"
"    margin-top: 10px;\n"
"    padding-top: 10px;\n"
"    background-color: #F5FAF7;\n"
"}\n"
"QGroupBox::title {\n"
"    subcontrol-origin: margin;\n"
"    left: 10px;\n"
"    padding: 0 5px;\n"
"    color: #1A3C2F;\n"
"}"));
        layoutFormat = new QHBoxLayout(groupFormat);
        layoutFormat->setObjectName("layoutFormat");
        radioPDF = new QRadioButton(groupFormat);
        radioPDF->setObjectName("radioPDF");
        radioPDF->setChecked(true);

        layoutFormat->addWidget(radioPDF);

        radioExcel = new QRadioButton(groupFormat);
        radioExcel->setObjectName("radioExcel");

        layoutFormat->addWidget(radioExcel);

        horizontalSpacer_4 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        layoutFormat->addItem(horizontalSpacer_4);


        verticalLayout_4->addWidget(groupFormat);

        groupOptions = new QGroupBox(tabExport);
        groupOptions->setObjectName("groupOptions");
        groupOptions->setStyleSheet(QString::fromUtf8("QGroupBox {\n"
"    font-weight: bold;\n"
"    border: 2px solid #1A3C2F;\n"
"    border-radius: 8px;\n"
"    margin-top: 10px;\n"
"    padding-top: 10px;\n"
"    background-color: #F5FAF7;\n"
"}\n"
"QGroupBox::title {\n"
"    subcontrol-origin: margin;\n"
"    left: 10px;\n"
"    padding: 0 5px;\n"
"    color: #1A3C2F;\n"
"}"));
        layoutOptions = new QVBoxLayout(groupOptions);
        layoutOptions->setObjectName("layoutOptions");
        checkStats = new QCheckBox(groupOptions);
        checkStats->setObjectName("checkStats");

        layoutOptions->addWidget(checkStats);

        checkDateTime = new QCheckBox(groupOptions);
        checkDateTime->setObjectName("checkDateTime");
        checkDateTime->setChecked(true);

        layoutOptions->addWidget(checkDateTime);

        layoutNomFichier = new QHBoxLayout();
        layoutNomFichier->setObjectName("layoutNomFichier");
        labelNomFichier = new QLabel(groupOptions);
        labelNomFichier->setObjectName("labelNomFichier");

        layoutNomFichier->addWidget(labelNomFichier);

        lineNomFichier = new QLineEdit(groupOptions);
        lineNomFichier->setObjectName("lineNomFichier");
        lineNomFichier->setStyleSheet(QString::fromUtf8("QLineEdit {\n"
"    border: 1px solid #ccc;\n"
"    border-radius: 4px;\n"
"    padding: 6px;\n"
"    background: white;\n"
"}"));

        layoutNomFichier->addWidget(lineNomFichier);


        layoutOptions->addLayout(layoutNomFichier);


        verticalLayout_4->addWidget(groupOptions);

        btnExporter = new QPushButton(tabExport);
        btnExporter->setObjectName("btnExporter");
        btnExporter->setMinimumSize(QSize(0, 50));
        btnExporter->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"    background-color: #1A3C2F;\n"
"    color: white;\n"
"    border: none;\n"
"    padding: 8px 16px;\n"
"    border-radius: 4px;\n"
"    font-weight: bold;\n"
"    font-size: 16px;\n"
"}\n"
"QPushButton:hover {\n"
"    background-color: #234D3D;\n"
"}"));

        verticalLayout_4->addWidget(btnExporter);

        verticalSpacer_1 = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout_4->addItem(verticalSpacer_1);

        tabWidgetStock->addTab(tabExport, QString());
        tabStatistiques = new QWidget();
        tabStatistiques->setObjectName("tabStatistiques");
        verticalLayout_5 = new QVBoxLayout(tabStatistiques);
        verticalLayout_5->setObjectName("verticalLayout_5");
        verticalLayout_5->setContentsMargins(20, 20, 20, 20);
        groupResume = new QGroupBox(tabStatistiques);
        groupResume->setObjectName("groupResume");
        groupResume->setStyleSheet(QString::fromUtf8("QGroupBox {\n"
"    font-weight: bold;\n"
"    border: 2px solid #1A3C2F;\n"
"    border-radius: 8px;\n"
"    margin-top: 10px;\n"
"    padding-top: 10px;\n"
"    background-color: #F5FAF7;\n"
"}\n"
"QGroupBox::title {\n"
"    subcontrol-origin: margin;\n"
"    left: 10px;\n"
"    padding: 0 5px;\n"
"    color: #1A3C2F;\n"
"}"));
        layoutResume = new QHBoxLayout(groupResume);
        layoutResume->setObjectName("layoutResume");
        labelTotalArticles = new QLabel(groupResume);
        labelTotalArticles->setObjectName("labelTotalArticles");
        labelTotalArticles->setStyleSheet(QString::fromUtf8("font-size: 14px; font-weight: bold;"));

        layoutResume->addWidget(labelTotalArticles);

        labelValeurTotale = new QLabel(groupResume);
        labelValeurTotale->setObjectName("labelValeurTotale");
        labelValeurTotale->setStyleSheet(QString::fromUtf8("font-size: 14px; font-weight: bold;"));

        layoutResume->addWidget(labelValeurTotale);

        horizontalSpacer_5 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        layoutResume->addItem(horizontalSpacer_5);


        verticalLayout_5->addWidget(groupResume);

        groupOptionsStats = new QGroupBox(tabStatistiques);
        groupOptionsStats->setObjectName("groupOptionsStats");
        groupOptionsStats->setStyleSheet(QString::fromUtf8("QGroupBox {\n"
"    font-weight: bold;\n"
"    border: 2px solid #1A3C2F;\n"
"    border-radius: 8px;\n"
"    margin-top: 10px;\n"
"    padding-top: 10px;\n"
"    background-color: #F5FAF7;\n"
"}\n"
"QGroupBox::title {\n"
"    subcontrol-origin: margin;\n"
"    left: 10px;\n"
"    padding: 0 5px;\n"
"    color: #1A3C2F;\n"
"}"));
        layoutOptionsStats = new QHBoxLayout(groupOptionsStats);
        layoutOptionsStats->setObjectName("layoutOptionsStats");
        labelStatsPar = new QLabel(groupOptionsStats);
        labelStatsPar->setObjectName("labelStatsPar");

        layoutOptionsStats->addWidget(labelStatsPar);

        comboStatsPar = new QComboBox(groupOptionsStats);
        comboStatsPar->addItem(QString());
        comboStatsPar->addItem(QString());
        comboStatsPar->addItem(QString());
        comboStatsPar->addItem(QString());
        comboStatsPar->addItem(QString());
        comboStatsPar->setObjectName("comboStatsPar");
        comboStatsPar->setStyleSheet(QString::fromUtf8("QComboBox {\n"
"    border: 1px solid #ccc;\n"
"    border-radius: 4px;\n"
"    padding: 6px;\n"
"    background: white;\n"
"    min-width: 150px;\n"
"}"));

        layoutOptionsStats->addWidget(comboStatsPar);

        btnCalculer = new QPushButton(groupOptionsStats);
        btnCalculer->setObjectName("btnCalculer");
        btnCalculer->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"    background-color: #1A3C2F;\n"
"    color: white;\n"
"    border: none;\n"
"    padding: 8px 16px;\n"
"    border-radius: 4px;\n"
"    font-weight: bold;\n"
"}\n"
"QPushButton:hover {\n"
"    background-color: #234D3D;\n"
"}"));

        layoutOptionsStats->addWidget(btnCalculer);

        horizontalSpacer_6 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        layoutOptionsStats->addItem(horizontalSpacer_6);


        verticalLayout_5->addWidget(groupOptionsStats);

        groupStats = new QGroupBox(tabStatistiques);
        groupStats->setObjectName("groupStats");
        groupStats->setStyleSheet(QString::fromUtf8("QGroupBox {\n"
"    font-weight: bold;\n"
"    border: 2px solid #1A3C2F;\n"
"    border-radius: 8px;\n"
"    margin-top: 10px;\n"
"    padding-top: 10px;\n"
"    background-color: #F5FAF7;\n"
"}\n"
"QGroupBox::title {\n"
"    subcontrol-origin: margin;\n"
"    left: 10px;\n"
"    padding: 0 5px;\n"
"    color: #1A3C2F;\n"
"}"));
        layoutStats = new QVBoxLayout(groupStats);
        layoutStats->setObjectName("layoutStats");
        textResumeStats = new QTextEdit(groupStats);
        textResumeStats->setObjectName("textResumeStats");
        textResumeStats->setMinimumSize(QSize(0, 200));
        textResumeStats->setReadOnly(true);

        layoutStats->addWidget(textResumeStats);


        verticalLayout_5->addWidget(groupStats);

        tabWidgetStock->addTab(tabStatistiques, QString());
        tabHistorique = new QWidget();
        tabHistorique->setObjectName("tabHistorique");
        verticalLayout_6 = new QVBoxLayout(tabHistorique);
        verticalLayout_6->setObjectName("verticalLayout_6");
        verticalLayout_6->setContentsMargins(20, 20, 20, 20);
        groupFiltrePeriode = new QGroupBox(tabHistorique);
        groupFiltrePeriode->setObjectName("groupFiltrePeriode");
        groupFiltrePeriode->setStyleSheet(QString::fromUtf8("QGroupBox {\n"
"    font-weight: bold;\n"
"    border: 2px solid #1A3C2F;\n"
"    border-radius: 8px;\n"
"    margin-top: 10px;\n"
"    padding-top: 10px;\n"
"    background-color: #F5FAF7;\n"
"}\n"
"QGroupBox::title {\n"
"    subcontrol-origin: margin;\n"
"    left: 10px;\n"
"    padding: 0 5px;\n"
"    color: #1A3C2F;\n"
"}"));
        layoutFiltrePeriode = new QHBoxLayout(groupFiltrePeriode);
        layoutFiltrePeriode->setObjectName("layoutFiltrePeriode");
        labelDateDebut = new QLabel(groupFiltrePeriode);
        labelDateDebut->setObjectName("labelDateDebut");

        layoutFiltrePeriode->addWidget(labelDateDebut);

        dateDebut = new QDateEdit(groupFiltrePeriode);
        dateDebut->setObjectName("dateDebut");
        dateDebut->setStyleSheet(QString::fromUtf8("QDateEdit {\n"
"    border: 1px solid #ccc;\n"
"    border-radius: 4px;\n"
"    padding: 6px;\n"
"    background: white;\n"
"}"));
        dateDebut->setCalendarPopup(true);

        layoutFiltrePeriode->addWidget(dateDebut);

        labelDateFin = new QLabel(groupFiltrePeriode);
        labelDateFin->setObjectName("labelDateFin");

        layoutFiltrePeriode->addWidget(labelDateFin);

        dateFin = new QDateEdit(groupFiltrePeriode);
        dateFin->setObjectName("dateFin");
        dateFin->setStyleSheet(QString::fromUtf8("QDateEdit {\n"
"    border: 1px solid #ccc;\n"
"    border-radius: 4px;\n"
"    padding: 6px;\n"
"    background: white;\n"
"}"));
        dateFin->setCalendarPopup(true);

        layoutFiltrePeriode->addWidget(dateFin);

        btnAfficherHistorique = new QPushButton(groupFiltrePeriode);
        btnAfficherHistorique->setObjectName("btnAfficherHistorique");
        btnAfficherHistorique->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"    background-color: #1A3C2F;\n"
"    color: white;\n"
"    border: none;\n"
"    padding: 8px 16px;\n"
"    border-radius: 4px;\n"
"    font-weight: bold;\n"
"}\n"
"QPushButton:hover {\n"
"    background-color: #234D3D;\n"
"}"));

        layoutFiltrePeriode->addWidget(btnAfficherHistorique);

        horizontalSpacer_7 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        layoutFiltrePeriode->addItem(horizontalSpacer_7);


        verticalLayout_6->addWidget(groupFiltrePeriode);

        groupHistorique = new QGroupBox(tabHistorique);
        groupHistorique->setObjectName("groupHistorique");
        groupHistorique->setStyleSheet(QString::fromUtf8("QGroupBox {\n"
"    font-weight: bold;\n"
"    border: 2px solid #1A3C2F;\n"
"    border-radius: 8px;\n"
"    margin-top: 10px;\n"
"    padding-top: 10px;\n"
"    background-color: #F5FAF7;\n"
"}\n"
"QGroupBox::title {\n"
"    subcontrol-origin: margin;\n"
"    left: 10px;\n"
"    padding: 0 5px;\n"
"    color: #1A3C2F;\n"
"}"));
        layoutHistorique = new QVBoxLayout(groupHistorique);
        layoutHistorique->setObjectName("layoutHistorique");
        tableHistorique = new QTableWidget(groupHistorique);
        if (tableHistorique->columnCount() < 4)
            tableHistorique->setColumnCount(4);
        QTableWidgetItem *__qtablewidgetitem21 = new QTableWidgetItem();
        tableHistorique->setHorizontalHeaderItem(0, __qtablewidgetitem21);
        QTableWidgetItem *__qtablewidgetitem22 = new QTableWidgetItem();
        tableHistorique->setHorizontalHeaderItem(1, __qtablewidgetitem22);
        QTableWidgetItem *__qtablewidgetitem23 = new QTableWidgetItem();
        tableHistorique->setHorizontalHeaderItem(2, __qtablewidgetitem23);
        QTableWidgetItem *__qtablewidgetitem24 = new QTableWidgetItem();
        tableHistorique->setHorizontalHeaderItem(3, __qtablewidgetitem24);
        tableHistorique->setObjectName("tableHistorique");
        tableHistorique->setStyleSheet(QString::fromUtf8("QTableWidget {\n"
"    border: 1px solid #ddd;\n"
"    border-radius: 4px;\n"
"    gridline-color: #e0e0e0;\n"
"}\n"
"QHeaderView::section {\n"
"    background-color: #1A3C2F;\n"
"    color: white;\n"
"    padding: 8px;\n"
"    border: none;\n"
"    font-weight: bold;\n"
"}"));
        tableHistorique->setAlternatingRowColors(true);
        tableHistorique->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);

        layoutHistorique->addWidget(tableHistorique);


        verticalLayout_6->addWidget(groupHistorique);

        tabWidgetStock->addTab(tabHistorique, QString());
        tabEtatStock = new QWidget();
        tabEtatStock->setObjectName("tabEtatStock");
        verticalLayout_7 = new QVBoxLayout(tabEtatStock);
        verticalLayout_7->setObjectName("verticalLayout_7");
        verticalLayout_7->setContentsMargins(20, 20, 20, 20);
        groupEtatGlobal = new QGroupBox(tabEtatStock);
        groupEtatGlobal->setObjectName("groupEtatGlobal");
        groupEtatGlobal->setStyleSheet(QString::fromUtf8("QGroupBox {\n"
"    font-weight: bold;\n"
"    border: 2px solid #1A3C2F;\n"
"    border-radius: 8px;\n"
"    margin-top: 10px;\n"
"    padding-top: 10px;\n"
"    background-color: #F5FAF7;\n"
"}\n"
"QGroupBox::title {\n"
"    subcontrol-origin: margin;\n"
"    left: 10px;\n"
"    padding: 0 5px;\n"
"    color: #1A3C2F;\n"
"}"));
        layoutEtatGlobal = new QVBoxLayout(groupEtatGlobal);
        layoutEtatGlobal->setObjectName("layoutEtatGlobal");
        labelEtatGlobal = new QLabel(groupEtatGlobal);
        labelEtatGlobal->setObjectName("labelEtatGlobal");
        labelEtatGlobal->setStyleSheet(QString::fromUtf8("font-size: 24px; font-weight: bold; color: #1A3C2F;"));
        labelEtatGlobal->setAlignment(Qt::AlignmentFlag::AlignCenter);

        layoutEtatGlobal->addWidget(labelEtatGlobal);

        progressStock = new QProgressBar(groupEtatGlobal);
        progressStock->setObjectName("progressStock");
        progressStock->setMinimumSize(QSize(0, 30));
        progressStock->setStyleSheet(QString::fromUtf8("QProgressBar {\n"
"    border: 1px solid #ccc;\n"
"    border-radius: 5px;\n"
"    text-align: center;\n"
"}\n"
"QProgressBar::chunk {\n"
"    background-color: #2D5A4A;\n"
"    border-radius: 4px;\n"
"}"));
        progressStock->setValue(75);

        layoutEtatGlobal->addWidget(progressStock);


        verticalLayout_7->addWidget(groupEtatGlobal);

        groupProduitsCritiques = new QGroupBox(tabEtatStock);
        groupProduitsCritiques->setObjectName("groupProduitsCritiques");
        groupProduitsCritiques->setStyleSheet(QString::fromUtf8("QGroupBox {\n"
"    font-weight: bold;\n"
"    border: 2px solid #1A3C2F;\n"
"    border-radius: 8px;\n"
"    margin-top: 10px;\n"
"    padding-top: 10px;\n"
"    background-color: #F5FAF7;\n"
"}\n"
"QGroupBox::title {\n"
"    subcontrol-origin: margin;\n"
"    left: 10px;\n"
"    padding: 0 5px;\n"
"    color: #1A3C2F;\n"
"}"));
        layoutProduitsCritiques = new QVBoxLayout(groupProduitsCritiques);
        layoutProduitsCritiques->setObjectName("layoutProduitsCritiques");
        listProduitsCritiques = new QListWidget(groupProduitsCritiques);
        listProduitsCritiques->setObjectName("listProduitsCritiques");
        listProduitsCritiques->setStyleSheet(QString::fromUtf8("QListWidget::item { padding: 10px; }"));

        layoutProduitsCritiques->addWidget(listProduitsCritiques);


        verticalLayout_7->addWidget(groupProduitsCritiques);

        tabWidgetStock->addTab(tabEtatStock, QString());
        tabTempsReel = new QWidget();
        tabTempsReel->setObjectName("tabTempsReel");
        verticalLayout_8 = new QVBoxLayout(tabTempsReel);
        verticalLayout_8->setObjectName("verticalLayout_8");
        verticalLayout_8->setContentsMargins(20, 20, 20, 20);
        groupTempsReel = new QGroupBox(tabTempsReel);
        groupTempsReel->setObjectName("groupTempsReel");
        groupTempsReel->setStyleSheet(QString::fromUtf8("QGroupBox {\n"
"    font-weight: bold;\n"
"    border: 2px solid #1A3C2F;\n"
"    border-radius: 8px;\n"
"    margin-top: 10px;\n"
"    padding-top: 10px;\n"
"    background-color: #F5FAF7;\n"
"}\n"
"QGroupBox::title {\n"
"    subcontrol-origin: margin;\n"
"    left: 10px;\n"
"    padding: 0 5px;\n"
"    color: #1A3C2F;\n"
"}"));
        layoutTempsReel = new QVBoxLayout(groupTempsReel);
        layoutTempsReel->setObjectName("layoutTempsReel");
        labelQuantiteActuelle = new QLabel(groupTempsReel);
        labelQuantiteActuelle->setObjectName("labelQuantiteActuelle");
        labelQuantiteActuelle->setStyleSheet(QString::fromUtf8("font-size: 18px;"));
        labelQuantiteActuelle->setAlignment(Qt::AlignmentFlag::AlignCenter);

        layoutTempsReel->addWidget(labelQuantiteActuelle);

        lcdVolume = new QLCDNumber(groupTempsReel);
        lcdVolume->setObjectName("lcdVolume");
        lcdVolume->setMinimumSize(QSize(0, 100));
        lcdVolume->setStyleSheet(QString::fromUtf8("QLCDNumber {\n"
"    border: 2px solid #1A3C2F;\n"
"    border-radius: 5px;\n"
"    background: #0F2A1F;\n"
"    color: #2D5A4A;\n"
"}"));
        lcdVolume->setDigitCount(10);
        lcdVolume->setSegmentStyle(QLCDNumber::SegmentStyle::Flat);

        layoutTempsReel->addWidget(lcdVolume);

        labelUnite = new QLabel(groupTempsReel);
        labelUnite->setObjectName("labelUnite");
        labelUnite->setStyleSheet(QString::fromUtf8("font-size: 16px; color: #666;"));
        labelUnite->setAlignment(Qt::AlignmentFlag::AlignCenter);

        layoutTempsReel->addWidget(labelUnite);

        labelDerniereMaj = new QLabel(groupTempsReel);
        labelDerniereMaj->setObjectName("labelDerniereMaj");
        labelDerniereMaj->setStyleSheet(QString::fromUtf8("font-size: 12px; color: #888;"));
        labelDerniereMaj->setAlignment(Qt::AlignmentFlag::AlignCenter);

        layoutTempsReel->addWidget(labelDerniereMaj);


        verticalLayout_8->addWidget(groupTempsReel);

        labelInfoRefresh = new QLabel(tabTempsReel);
        labelInfoRefresh->setObjectName("labelInfoRefresh");
        labelInfoRefresh->setStyleSheet(QString::fromUtf8("color: #666; font-style: italic;"));
        labelInfoRefresh->setAlignment(Qt::AlignmentFlag::AlignCenter);

        verticalLayout_8->addWidget(labelInfoRefresh);

        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout_8->addItem(verticalSpacer_2);

        tabWidgetStock->addTab(tabTempsReel, QString());
        tabAlertes = new QWidget();
        tabAlertes->setObjectName("tabAlertes");
        verticalLayout_9 = new QVBoxLayout(tabAlertes);
        verticalLayout_9->setObjectName("verticalLayout_9");
        verticalLayout_9->setContentsMargins(20, 20, 20, 20);
        groupConfigAlertes = new QGroupBox(tabAlertes);
        groupConfigAlertes->setObjectName("groupConfigAlertes");
        groupConfigAlertes->setStyleSheet(QString::fromUtf8("QGroupBox {\n"
"    font-weight: bold;\n"
"    border: 2px solid #1A3C2F;\n"
"    border-radius: 8px;\n"
"    margin-top: 10px;\n"
"    padding-top: 10px;\n"
"    background-color: #F5FAF7;\n"
"}\n"
"QGroupBox::title {\n"
"    subcontrol-origin: margin;\n"
"    left: 10px;\n"
"    padding: 0 5px;\n"
"    color: #1A3C2F;\n"
"}"));
        layoutConfigAlertes = new QHBoxLayout(groupConfigAlertes);
        layoutConfigAlertes->setObjectName("layoutConfigAlertes");
        labelSeuilDefini = new QLabel(groupConfigAlertes);
        labelSeuilDefini->setObjectName("labelSeuilDefini");
        labelSeuilDefini->setWordWrap(true);

        layoutConfigAlertes->addWidget(labelSeuilDefini);

        btnVerifierAlertes = new QPushButton(groupConfigAlertes);
        btnVerifierAlertes->setObjectName("btnVerifierAlertes");
        btnVerifierAlertes->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"    background-color: #ff9800;\n"
"    color: white;\n"
"    border: none;\n"
"    padding: 8px 16px;\n"
"    border-radius: 4px;\n"
"    font-weight: bold;\n"
"}\n"
"QPushButton:hover {\n"
"    background-color: #ffa726;\n"
"}"));

        layoutConfigAlertes->addWidget(btnVerifierAlertes);


        verticalLayout_9->addWidget(groupConfigAlertes);

        groupStocksSousSeuil = new QGroupBox(tabAlertes);
        groupStocksSousSeuil->setObjectName("groupStocksSousSeuil");
        groupStocksSousSeuil->setStyleSheet(QString::fromUtf8("QGroupBox {\n"
"    font-weight: bold;\n"
"    border: 2px solid #1A3C2F;\n"
"    border-radius: 8px;\n"
"    margin-top: 10px;\n"
"    padding-top: 10px;\n"
"    background-color: #F5FAF7;\n"
"}\n"
"QGroupBox::title {\n"
"    subcontrol-origin: margin;\n"
"    left: 10px;\n"
"    padding: 0 5px;\n"
"    color: #1A3C2F;\n"
"}"));
        layoutStocksSousSeuil = new QVBoxLayout(groupStocksSousSeuil);
        layoutStocksSousSeuil->setObjectName("layoutStocksSousSeuil");
        listStocksSousSeuil = new QListWidget(groupStocksSousSeuil);
        listStocksSousSeuil->setObjectName("listStocksSousSeuil");
        listStocksSousSeuil->setStyleSheet(QString::fromUtf8("QListWidget::item { padding: 10px; font-size: 14px; }"));

        layoutStocksSousSeuil->addWidget(listStocksSousSeuil);


        verticalLayout_9->addWidget(groupStocksSousSeuil);

        tabWidgetStock->addTab(tabAlertes, QString());
        tabTendances = new QWidget();
        tabTendances->setObjectName("tabTendances");
        verticalLayout_10 = new QVBoxLayout(tabTendances);
        verticalLayout_10->setObjectName("verticalLayout_10");
        verticalLayout_10->setContentsMargins(20, 20, 20, 20);
        groupOptionsTendances = new QGroupBox(tabTendances);
        groupOptionsTendances->setObjectName("groupOptionsTendances");
        groupOptionsTendances->setStyleSheet(QString::fromUtf8("QGroupBox {\n"
"    font-weight: bold;\n"
"    border: 2px solid #1A3C2F;\n"
"    border-radius: 8px;\n"
"    margin-top: 10px;\n"
"    padding-top: 10px;\n"
"    background-color: #F5FAF7;\n"
"}\n"
"QGroupBox::title {\n"
"    subcontrol-origin: margin;\n"
"    left: 10px;\n"
"    padding: 0 5px;\n"
"    color: #1A3C2F;\n"
"}"));
        layoutOptionsTendances = new QHBoxLayout(groupOptionsTendances);
        layoutOptionsTendances->setObjectName("layoutOptionsTendances");
        labelPeriode = new QLabel(groupOptionsTendances);
        labelPeriode->setObjectName("labelPeriode");

        layoutOptionsTendances->addWidget(labelPeriode);

        comboPeriode = new QComboBox(groupOptionsTendances);
        comboPeriode->addItem(QString());
        comboPeriode->addItem(QString());
        comboPeriode->addItem(QString());
        comboPeriode->addItem(QString());
        comboPeriode->setObjectName("comboPeriode");
        comboPeriode->setStyleSheet(QString::fromUtf8("QComboBox {\n"
"    border: 1px solid #ccc;\n"
"    border-radius: 4px;\n"
"    padding: 6px;\n"
"    background: white;\n"
"    min-width: 180px;\n"
"}"));

        layoutOptionsTendances->addWidget(comboPeriode);

        btnAnalyserTendances = new QPushButton(groupOptionsTendances);
        btnAnalyserTendances->setObjectName("btnAnalyserTendances");
        btnAnalyserTendances->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"    background-color: #1A3C2F;\n"
"    color: white;\n"
"    border: none;\n"
"    padding: 8px 16px;\n"
"    border-radius: 4px;\n"
"    font-weight: bold;\n"
"}\n"
"QPushButton:hover {\n"
"    background-color: #234D3D;\n"
"}"));

        layoutOptionsTendances->addWidget(btnAnalyserTendances);

        horizontalSpacer_8 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        layoutOptionsTendances->addItem(horizontalSpacer_8);


        verticalLayout_10->addWidget(groupOptionsTendances);

        groupGraphique = new QGroupBox(tabTendances);
        groupGraphique->setObjectName("groupGraphique");
        groupGraphique->setStyleSheet(QString::fromUtf8("QGroupBox {\n"
"    font-weight: bold;\n"
"    border: 2px solid #1A3C2F;\n"
"    border-radius: 8px;\n"
"    margin-top: 10px;\n"
"    padding-top: 10px;\n"
"    background-color: #F5FAF7;\n"
"}\n"
"QGroupBox::title {\n"
"    subcontrol-origin: margin;\n"
"    left: 10px;\n"
"    padding: 0 5px;\n"
"    color: #1A3C2F;\n"
"}"));
        layoutGraphique = new QVBoxLayout(groupGraphique);
        layoutGraphique->setObjectName("layoutGraphique");
        labelPlaceholderChart = new QLabel(groupGraphique);
        labelPlaceholderChart->setObjectName("labelPlaceholderChart");
        labelPlaceholderChart->setMinimumSize(QSize(0, 200));
        labelPlaceholderChart->setStyleSheet(QString::fromUtf8("background: #f0f0f0; padding: 50px; border-radius: 10px; color: #666;"));
        labelPlaceholderChart->setAlignment(Qt::AlignmentFlag::AlignCenter);

        layoutGraphique->addWidget(labelPlaceholderChart);


        verticalLayout_10->addWidget(groupGraphique);

        groupAnalyseAuto = new QGroupBox(tabTendances);
        groupAnalyseAuto->setObjectName("groupAnalyseAuto");
        groupAnalyseAuto->setStyleSheet(QString::fromUtf8("QGroupBox {\n"
"    font-weight: bold;\n"
"    border: 2px solid #1A3C2F;\n"
"    border-radius: 8px;\n"
"    margin-top: 10px;\n"
"    padding-top: 10px;\n"
"    background-color: #F5FAF7;\n"
"}\n"
"QGroupBox::title {\n"
"    subcontrol-origin: margin;\n"
"    left: 10px;\n"
"    padding: 0 5px;\n"
"    color: #1A3C2F;\n"
"}"));
        layoutAnalyseAuto = new QVBoxLayout(groupAnalyseAuto);
        layoutAnalyseAuto->setObjectName("layoutAnalyseAuto");
        textAnalyseTendances = new QTextEdit(groupAnalyseAuto);
        textAnalyseTendances->setObjectName("textAnalyseTendances");
        textAnalyseTendances->setReadOnly(true);

        layoutAnalyseAuto->addWidget(textAnalyseTendances);


        verticalLayout_10->addWidget(groupAnalyseAuto);

        tabWidgetStock->addTab(tabTendances, QString());
        tabRapports = new QWidget();
        tabRapports->setObjectName("tabRapports");
        verticalLayout_11 = new QVBoxLayout(tabRapports);
        verticalLayout_11->setObjectName("verticalLayout_11");
        verticalLayout_11->setContentsMargins(20, 20, 20, 20);
        groupSelectionRapport = new QGroupBox(tabRapports);
        groupSelectionRapport->setObjectName("groupSelectionRapport");
        groupSelectionRapport->setStyleSheet(QString::fromUtf8("QGroupBox {\n"
"    font-weight: bold;\n"
"    border: 2px solid #1A3C2F;\n"
"    border-radius: 8px;\n"
"    margin-top: 10px;\n"
"    padding-top: 10px;\n"
"    background-color: #F5FAF7;\n"
"}\n"
"QGroupBox::title {\n"
"    subcontrol-origin: margin;\n"
"    left: 10px;\n"
"    padding: 0 5px;\n"
"    color: #1A3C2F;\n"
"}"));
        layoutSelectionRapport = new QVBoxLayout(groupSelectionRapport);
        layoutSelectionRapport->setObjectName("layoutSelectionRapport");
        tableRapportSelection = new QTableWidget(groupSelectionRapport);
        if (tableRapportSelection->columnCount() < 7)
            tableRapportSelection->setColumnCount(7);
        QTableWidgetItem *__qtablewidgetitem25 = new QTableWidgetItem();
        tableRapportSelection->setHorizontalHeaderItem(0, __qtablewidgetitem25);
        QTableWidgetItem *__qtablewidgetitem26 = new QTableWidgetItem();
        tableRapportSelection->setHorizontalHeaderItem(1, __qtablewidgetitem26);
        QTableWidgetItem *__qtablewidgetitem27 = new QTableWidgetItem();
        tableRapportSelection->setHorizontalHeaderItem(2, __qtablewidgetitem27);
        QTableWidgetItem *__qtablewidgetitem28 = new QTableWidgetItem();
        tableRapportSelection->setHorizontalHeaderItem(3, __qtablewidgetitem28);
        QTableWidgetItem *__qtablewidgetitem29 = new QTableWidgetItem();
        tableRapportSelection->setHorizontalHeaderItem(4, __qtablewidgetitem29);
        QTableWidgetItem *__qtablewidgetitem30 = new QTableWidgetItem();
        tableRapportSelection->setHorizontalHeaderItem(5, __qtablewidgetitem30);
        QTableWidgetItem *__qtablewidgetitem31 = new QTableWidgetItem();
        tableRapportSelection->setHorizontalHeaderItem(6, __qtablewidgetitem31);
        tableRapportSelection->setObjectName("tableRapportSelection");
        tableRapportSelection->setMaximumSize(QSize(16777215, 200));
        tableRapportSelection->setStyleSheet(QString::fromUtf8("QTableWidget {\n"
"    border: 1px solid #ddd;\n"
"    border-radius: 4px;\n"
"    gridline-color: #e0e0e0;\n"
"}\n"
"QHeaderView::section {\n"
"    background-color: #1A3C2F;\n"
"    color: white;\n"
"    padding: 8px;\n"
"    border: none;\n"
"    font-weight: bold;\n"
"}"));
        tableRapportSelection->setAlternatingRowColors(true);
        tableRapportSelection->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);

        layoutSelectionRapport->addWidget(tableRapportSelection);


        verticalLayout_11->addWidget(groupSelectionRapport);

        layoutBtnRapport = new QHBoxLayout();
        layoutBtnRapport->setObjectName("layoutBtnRapport");
        btnGenererRapport = new QPushButton(tabRapports);
        btnGenererRapport->setObjectName("btnGenererRapport");
        btnGenererRapport->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"    background-color: #1A3C2F;\n"
"    color: white;\n"
"    border: none;\n"
"    padding: 8px 16px;\n"
"    border-radius: 4px;\n"
"    font-weight: bold;\n"
"}\n"
"QPushButton:hover {\n"
"    background-color: #234D3D;\n"
"}"));

        layoutBtnRapport->addWidget(btnGenererRapport);

        btnExportRapportPDF = new QPushButton(tabRapports);
        btnExportRapportPDF->setObjectName("btnExportRapportPDF");
        btnExportRapportPDF->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"    background-color: #1565c0;\n"
"    color: white;\n"
"    border: none;\n"
"    padding: 8px 16px;\n"
"    border-radius: 4px;\n"
"    font-weight: bold;\n"
"}\n"
"QPushButton:hover {\n"
"    background-color: #1976d2;\n"
"}"));

        layoutBtnRapport->addWidget(btnExportRapportPDF);

        horizontalSpacer_9 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        layoutBtnRapport->addItem(horizontalSpacer_9);


        verticalLayout_11->addLayout(layoutBtnRapport);

        groupApercuRapport = new QGroupBox(tabRapports);
        groupApercuRapport->setObjectName("groupApercuRapport");
        groupApercuRapport->setStyleSheet(QString::fromUtf8("QGroupBox {\n"
"    font-weight: bold;\n"
"    border: 2px solid #1A3C2F;\n"
"    border-radius: 8px;\n"
"    margin-top: 10px;\n"
"    padding-top: 10px;\n"
"    background-color: #F5FAF7;\n"
"}\n"
"QGroupBox::title {\n"
"    subcontrol-origin: margin;\n"
"    left: 10px;\n"
"    padding: 0 5px;\n"
"    color: #1A3C2F;\n"
"}"));
        layoutApercuRapport = new QVBoxLayout(groupApercuRapport);
        layoutApercuRapport->setObjectName("layoutApercuRapport");
        textApercuRapport = new QTextEdit(groupApercuRapport);
        textApercuRapport->setObjectName("textApercuRapport");
        textApercuRapport->setReadOnly(true);

        layoutApercuRapport->addWidget(textApercuRapport);


        verticalLayout_11->addWidget(groupApercuRapport);

        tabWidgetStock->addTab(tabRapports, QString());
        tabArduino = new QWidget();
        tabArduino->setObjectName("tabArduino");
        verticalLayout_12 = new QVBoxLayout(tabArduino);
        verticalLayout_12->setObjectName("verticalLayout_12");
        verticalLayout_12->setContentsMargins(20, 20, 20, 20);
        groupConnexion = new QGroupBox(tabArduino);
        groupConnexion->setObjectName("groupConnexion");
        groupConnexion->setStyleSheet(QString::fromUtf8("QGroupBox {\n"
"    font-weight: bold;\n"
"    border: 2px solid #1A3C2F;\n"
"    border-radius: 8px;\n"
"    margin-top: 10px;\n"
"    padding-top: 10px;\n"
"    background-color: #F5FAF7;\n"
"}\n"
"QGroupBox::title {\n"
"    subcontrol-origin: margin;\n"
"    left: 10px;\n"
"    padding: 0 5px;\n"
"    color: #1A3C2F;\n"
"}"));
        layoutConnexion = new QHBoxLayout(groupConnexion);
        layoutConnexion->setObjectName("layoutConnexion");
        labelEtatConnexion = new QLabel(groupConnexion);
        labelEtatConnexion->setObjectName("labelEtatConnexion");
        labelEtatConnexion->setStyleSheet(QString::fromUtf8("font-size: 16px; font-weight: bold; color: #f44336;"));

        layoutConnexion->addWidget(labelEtatConnexion);

        btnSynchroniser = new QPushButton(groupConnexion);
        btnSynchroniser->setObjectName("btnSynchroniser");
        btnSynchroniser->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"    background-color: #2196f3;\n"
"    color: white;\n"
"    border: none;\n"
"    padding: 8px 16px;\n"
"    border-radius: 4px;\n"
"    font-weight: bold;\n"
"}\n"
"QPushButton:hover {\n"
"    background-color: #42a5f5;\n"
"}"));

        layoutConnexion->addWidget(btnSynchroniser);

        horizontalSpacer_10 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        layoutConnexion->addItem(horizontalSpacer_10);


        verticalLayout_12->addWidget(groupConnexion);

        groupCapteur = new QGroupBox(tabArduino);
        groupCapteur->setObjectName("groupCapteur");
        groupCapteur->setStyleSheet(QString::fromUtf8("QGroupBox {\n"
"    font-weight: bold;\n"
"    border: 2px solid #1A3C2F;\n"
"    border-radius: 8px;\n"
"    margin-top: 10px;\n"
"    padding-top: 10px;\n"
"    background-color: #F5FAF7;\n"
"}\n"
"QGroupBox::title {\n"
"    subcontrol-origin: margin;\n"
"    left: 10px;\n"
"    padding: 0 5px;\n"
"    color: #1A3C2F;\n"
"}"));
        layoutCapteur = new QVBoxLayout(groupCapteur);
        layoutCapteur->setObjectName("layoutCapteur");
        labelValeurCapteur = new QLabel(groupCapteur);
        labelValeurCapteur->setObjectName("labelValeurCapteur");
        labelValeurCapteur->setStyleSheet(QString::fromUtf8("font-size: 24px; font-weight: bold;"));
        labelValeurCapteur->setAlignment(Qt::AlignmentFlag::AlignCenter);

        layoutCapteur->addWidget(labelValeurCapteur);


        verticalLayout_12->addWidget(groupCapteur);

        groupLogs = new QGroupBox(tabArduino);
        groupLogs->setObjectName("groupLogs");
        groupLogs->setStyleSheet(QString::fromUtf8("QGroupBox {\n"
"    font-weight: bold;\n"
"    border: 2px solid #1A3C2F;\n"
"    border-radius: 8px;\n"
"    margin-top: 10px;\n"
"    padding-top: 10px;\n"
"    background-color: #F5FAF7;\n"
"}\n"
"QGroupBox::title {\n"
"    subcontrol-origin: margin;\n"
"    left: 10px;\n"
"    padding: 0 5px;\n"
"    color: #1A3C2F;\n"
"}"));
        layoutLogs = new QVBoxLayout(groupLogs);
        layoutLogs->setObjectName("layoutLogs");
        textLogsArduino = new QTextEdit(groupLogs);
        textLogsArduino->setObjectName("textLogsArduino");
        textLogsArduino->setStyleSheet(QString::fromUtf8("font-family: Consolas, monospace;"));
        textLogsArduino->setReadOnly(true);

        layoutLogs->addWidget(textLogsArduino);


        verticalLayout_12->addWidget(groupLogs);

        tabWidgetStock->addTab(tabArduino, QString());

        mainVerticalLayout->addWidget(tabWidgetStock);

        footerWidget = new QWidget(centralwidget);
        footerWidget->setObjectName("footerWidget");
        footerWidget->setMinimumSize(QSize(0, 50));
        footerWidget->setMaximumSize(QSize(16777215, 50));
        footerWidget->setStyleSheet(QString::fromUtf8("QWidget#footerWidget {\n"
"    background-color: #ffffff;\n"
"    border-top: 1px solid #e0e0e0;\n"
"}"));
        footerLayout = new QHBoxLayout(footerWidget);
        footerLayout->setObjectName("footerLayout");
        footerLayout->setContentsMargins(20, 10, 20, 10);
        labelStatut = new QLabel(footerWidget);
        labelStatut->setObjectName("labelStatut");
        labelStatut->setStyleSheet(QString::fromUtf8("color: #666;"));

        footerLayout->addWidget(labelStatut);

        footerSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        footerLayout->addItem(footerSpacer);

        labelCopyright = new QLabel(footerWidget);
        labelCopyright->setObjectName("labelCopyright");
        labelCopyright->setStyleSheet(QString::fromUtf8("color: #999; font-size: 11px;"));

        footerLayout->addWidget(labelCopyright);


        mainVerticalLayout->addWidget(footerWidget);

        MainWindow->setCentralWidget(centralwidget);
        QWidget::setTabOrder(lineEditIdStock, btnExporter);
        QWidget::setTabOrder(btnExporter, comboTypeHuile);
        QWidget::setTabOrder(comboTypeHuile, spinQuantite);
        QWidget::setTabOrder(spinQuantite, spinSeuilAlerte);
        QWidget::setTabOrder(spinSeuilAlerte, dateEditMaj);
        QWidget::setTabOrder(dateEditMaj, lineEditEmplacement);
        QWidget::setTabOrder(lineEditEmplacement, comboEtatStock);
        QWidget::setTabOrder(comboEtatStock, btnAjouter);
        QWidget::setTabOrder(btnAjouter, btnModifier);
        QWidget::setTabOrder(btnModifier, btnSupprimer);
        QWidget::setTabOrder(btnSupprimer, btnConsulter);
        QWidget::setTabOrder(btnConsulter, tableStocks);
        QWidget::setTabOrder(tableStocks, comboTriCritere);
        QWidget::setTabOrder(comboTriCritere, btnAppliquerTri);
        QWidget::setTabOrder(btnAppliquerTri, tableTriee);
        QWidget::setTabOrder(tableTriee, comboRechercheType);
        QWidget::setTabOrder(comboRechercheType, comboRechercheEtat);
        QWidget::setTabOrder(comboRechercheEtat, lineRechercheEmplacement);
        QWidget::setTabOrder(lineRechercheEmplacement, btnRechercher);
        QWidget::setTabOrder(btnRechercher, btnReinitialiser);
        QWidget::setTabOrder(btnReinitialiser, tableResultats);
        QWidget::setTabOrder(tableResultats, radioPDF);
        QWidget::setTabOrder(radioPDF, radioExcel);
        QWidget::setTabOrder(radioExcel, checkStats);
        QWidget::setTabOrder(checkStats, checkDateTime);
        QWidget::setTabOrder(checkDateTime, lineNomFichier);
        QWidget::setTabOrder(lineNomFichier, comboStatsPar);
        QWidget::setTabOrder(comboStatsPar, btnCalculer);
        QWidget::setTabOrder(btnCalculer, textResumeStats);
        QWidget::setTabOrder(textResumeStats, dateDebut);
        QWidget::setTabOrder(dateDebut, dateFin);
        QWidget::setTabOrder(dateFin, btnAfficherHistorique);
        QWidget::setTabOrder(btnAfficherHistorique, tableHistorique);
        QWidget::setTabOrder(tableHistorique, listProduitsCritiques);
        QWidget::setTabOrder(listProduitsCritiques, btnVerifierAlertes);
        QWidget::setTabOrder(btnVerifierAlertes, listStocksSousSeuil);
        QWidget::setTabOrder(listStocksSousSeuil, comboPeriode);
        QWidget::setTabOrder(comboPeriode, btnAnalyserTendances);
        QWidget::setTabOrder(btnAnalyserTendances, textAnalyseTendances);
        QWidget::setTabOrder(textAnalyseTendances, tableRapportSelection);
        QWidget::setTabOrder(tableRapportSelection, btnGenererRapport);
        QWidget::setTabOrder(btnGenererRapport, btnExportRapportPDF);
        QWidget::setTabOrder(btnExportRapportPDF, textApercuRapport);
        QWidget::setTabOrder(textApercuRapport, btnSynchroniser);
        QWidget::setTabOrder(btnSynchroniser, textLogsArduino);

        retranslateUi(MainWindow);

        tabWidgetStock->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "PRESSIQ - Stock Management", nullptr));
        labelLogoImage->setText(QString());
        labelLogo->setText(QCoreApplication::translate("MainWindow", "<h1 style='margin: 0;'>PressIQ</h1>\n"
"<p style='color: #D0E5DC; margin: 0; font-size: 12px;'>STOCK MANAGEMENT</p>", nullptr));
        labelStatus->setText(QCoreApplication::translate("MainWindow", "EN LIGNE", nullptr));
        groupInfoStock->setTitle(QCoreApplication::translate("MainWindow", "Informations du stock", nullptr));
        labelIdStock->setText(QCoreApplication::translate("MainWindow", "ID Stock :", nullptr));
        lineEditIdStock->setPlaceholderText(QCoreApplication::translate("MainWindow", "Saisir l'ID du stock", nullptr));
        labelTypeHuile->setText(QCoreApplication::translate("MainWindow", "Type d'huile :", nullptr));
        comboTypeHuile->setItemText(0, QCoreApplication::translate("MainWindow", "Extra vierge", nullptr));
        comboTypeHuile->setItemText(1, QCoreApplication::translate("MainWindow", "Vierge", nullptr));
        comboTypeHuile->setItemText(2, QCoreApplication::translate("MainWindow", "Lampante", nullptr));

        labelQuantite->setText(QCoreApplication::translate("MainWindow", "Quantite actuelle (L) :", nullptr));
        spinQuantite->setSuffix(QCoreApplication::translate("MainWindow", " L", nullptr));
        labelSeuilAlerte->setText(QCoreApplication::translate("MainWindow", "Seuil d'alerte :", nullptr));
        spinSeuilAlerte->setSuffix(QCoreApplication::translate("MainWindow", " L", nullptr));
        labelDateMaj->setText(QCoreApplication::translate("MainWindow", "Date derniere mise a jour :", nullptr));
        labelEmplacement->setText(QCoreApplication::translate("MainWindow", "Emplacement de stockage :", nullptr));
        lineEditEmplacement->setPlaceholderText(QCoreApplication::translate("MainWindow", "Ex: Entrepot A, Zone 3", nullptr));
        labelEtatStock->setText(QCoreApplication::translate("MainWindow", "Etat du stock :", nullptr));
        comboEtatStock->setItemText(0, QCoreApplication::translate("MainWindow", "En stock", nullptr));
        comboEtatStock->setItemText(1, QCoreApplication::translate("MainWindow", "Stock faible", nullptr));
        comboEtatStock->setItemText(2, QCoreApplication::translate("MainWindow", "Rupture", nullptr));

        btnAjouter->setText(QCoreApplication::translate("MainWindow", "Ajouter", nullptr));
        btnModifier->setText(QCoreApplication::translate("MainWindow", "Modifier", nullptr));
        btnSupprimer->setText(QCoreApplication::translate("MainWindow", "Supprimer", nullptr));
        btnConsulter->setText(QCoreApplication::translate("MainWindow", "Consulter", nullptr));
        groupTableStocks->setTitle(QCoreApplication::translate("MainWindow", "Liste des stocks", nullptr));
        QTableWidgetItem *___qtablewidgetitem = tableStocks->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QCoreApplication::translate("MainWindow", "ID", nullptr));
        QTableWidgetItem *___qtablewidgetitem1 = tableStocks->horizontalHeaderItem(1);
        ___qtablewidgetitem1->setText(QCoreApplication::translate("MainWindow", "Type d'huile", nullptr));
        QTableWidgetItem *___qtablewidgetitem2 = tableStocks->horizontalHeaderItem(2);
        ___qtablewidgetitem2->setText(QCoreApplication::translate("MainWindow", "Quantite (L)", nullptr));
        QTableWidgetItem *___qtablewidgetitem3 = tableStocks->horizontalHeaderItem(3);
        ___qtablewidgetitem3->setText(QCoreApplication::translate("MainWindow", "Seuil", nullptr));
        QTableWidgetItem *___qtablewidgetitem4 = tableStocks->horizontalHeaderItem(4);
        ___qtablewidgetitem4->setText(QCoreApplication::translate("MainWindow", "Date MAJ", nullptr));
        QTableWidgetItem *___qtablewidgetitem5 = tableStocks->horizontalHeaderItem(5);
        ___qtablewidgetitem5->setText(QCoreApplication::translate("MainWindow", "Emplacement", nullptr));
        QTableWidgetItem *___qtablewidgetitem6 = tableStocks->horizontalHeaderItem(6);
        ___qtablewidgetitem6->setText(QCoreApplication::translate("MainWindow", "Etat", nullptr));
        tabWidgetStock->setTabText(tabWidgetStock->indexOf(tabGestionStock), QCoreApplication::translate("MainWindow", "Gestion Stock", nullptr));
        groupOptionsTri->setTitle(QCoreApplication::translate("MainWindow", "Options de tri", nullptr));
        labelTrierSelon->setText(QCoreApplication::translate("MainWindow", "Trier selon :", nullptr));
        comboTriCritere->setItemText(0, QCoreApplication::translate("MainWindow", "Type d'huile", nullptr));
        comboTriCritere->setItemText(1, QCoreApplication::translate("MainWindow", "Quantite disponible", nullptr));
        comboTriCritere->setItemText(2, QCoreApplication::translate("MainWindow", "Etat du stock", nullptr));
        comboTriCritere->setItemText(3, QCoreApplication::translate("MainWindow", "Date derniere mise a jour", nullptr));

        btnAppliquerTri->setText(QCoreApplication::translate("MainWindow", "Appliquer le tri", nullptr));
        groupTableTriee->setTitle(QCoreApplication::translate("MainWindow", "Stock trie", nullptr));
        QTableWidgetItem *___qtablewidgetitem7 = tableTriee->horizontalHeaderItem(0);
        ___qtablewidgetitem7->setText(QCoreApplication::translate("MainWindow", "ID", nullptr));
        QTableWidgetItem *___qtablewidgetitem8 = tableTriee->horizontalHeaderItem(1);
        ___qtablewidgetitem8->setText(QCoreApplication::translate("MainWindow", "Type d'huile", nullptr));
        QTableWidgetItem *___qtablewidgetitem9 = tableTriee->horizontalHeaderItem(2);
        ___qtablewidgetitem9->setText(QCoreApplication::translate("MainWindow", "Quantite (L)", nullptr));
        QTableWidgetItem *___qtablewidgetitem10 = tableTriee->horizontalHeaderItem(3);
        ___qtablewidgetitem10->setText(QCoreApplication::translate("MainWindow", "Seuil", nullptr));
        QTableWidgetItem *___qtablewidgetitem11 = tableTriee->horizontalHeaderItem(4);
        ___qtablewidgetitem11->setText(QCoreApplication::translate("MainWindow", "Date MAJ", nullptr));
        QTableWidgetItem *___qtablewidgetitem12 = tableTriee->horizontalHeaderItem(5);
        ___qtablewidgetitem12->setText(QCoreApplication::translate("MainWindow", "Emplacement", nullptr));
        QTableWidgetItem *___qtablewidgetitem13 = tableTriee->horizontalHeaderItem(6);
        ___qtablewidgetitem13->setText(QCoreApplication::translate("MainWindow", "Etat", nullptr));
        tabWidgetStock->setTabText(tabWidgetStock->indexOf(tabTri), QCoreApplication::translate("MainWindow", "Tri", nullptr));
        groupCriteres->setTitle(QCoreApplication::translate("MainWindow", "Criteres de recherche", nullptr));
        labelRechType->setText(QCoreApplication::translate("MainWindow", "Type d'huile :", nullptr));
        comboRechercheType->setItemText(0, QCoreApplication::translate("MainWindow", "Tous", nullptr));
        comboRechercheType->setItemText(1, QCoreApplication::translate("MainWindow", "Extra vierge", nullptr));
        comboRechercheType->setItemText(2, QCoreApplication::translate("MainWindow", "Vierge", nullptr));
        comboRechercheType->setItemText(3, QCoreApplication::translate("MainWindow", "Lampante", nullptr));

        labelRechEtat->setText(QCoreApplication::translate("MainWindow", "Etat du stock :", nullptr));
        comboRechercheEtat->setItemText(0, QCoreApplication::translate("MainWindow", "Tous", nullptr));
        comboRechercheEtat->setItemText(1, QCoreApplication::translate("MainWindow", "En stock", nullptr));
        comboRechercheEtat->setItemText(2, QCoreApplication::translate("MainWindow", "Stock faible", nullptr));
        comboRechercheEtat->setItemText(3, QCoreApplication::translate("MainWindow", "Rupture", nullptr));

        labelRechEmpl->setText(QCoreApplication::translate("MainWindow", "Emplacement :", nullptr));
        lineRechercheEmplacement->setPlaceholderText(QCoreApplication::translate("MainWindow", "Laisser vide pour tous", nullptr));
        btnRechercher->setText(QCoreApplication::translate("MainWindow", "Rechercher", nullptr));
        btnReinitialiser->setText(QCoreApplication::translate("MainWindow", "Reinitialiser", nullptr));
        groupResultats->setTitle(QCoreApplication::translate("MainWindow", "Resultats de recherche", nullptr));
        QTableWidgetItem *___qtablewidgetitem14 = tableResultats->horizontalHeaderItem(0);
        ___qtablewidgetitem14->setText(QCoreApplication::translate("MainWindow", "ID", nullptr));
        QTableWidgetItem *___qtablewidgetitem15 = tableResultats->horizontalHeaderItem(1);
        ___qtablewidgetitem15->setText(QCoreApplication::translate("MainWindow", "Type d'huile", nullptr));
        QTableWidgetItem *___qtablewidgetitem16 = tableResultats->horizontalHeaderItem(2);
        ___qtablewidgetitem16->setText(QCoreApplication::translate("MainWindow", "Quantite (L)", nullptr));
        QTableWidgetItem *___qtablewidgetitem17 = tableResultats->horizontalHeaderItem(3);
        ___qtablewidgetitem17->setText(QCoreApplication::translate("MainWindow", "Seuil", nullptr));
        QTableWidgetItem *___qtablewidgetitem18 = tableResultats->horizontalHeaderItem(4);
        ___qtablewidgetitem18->setText(QCoreApplication::translate("MainWindow", "Date MAJ", nullptr));
        QTableWidgetItem *___qtablewidgetitem19 = tableResultats->horizontalHeaderItem(5);
        ___qtablewidgetitem19->setText(QCoreApplication::translate("MainWindow", "Emplacement", nullptr));
        QTableWidgetItem *___qtablewidgetitem20 = tableResultats->horizontalHeaderItem(6);
        ___qtablewidgetitem20->setText(QCoreApplication::translate("MainWindow", "Etat", nullptr));
        tabWidgetStock->setTabText(tabWidgetStock->indexOf(tabRecherche), QCoreApplication::translate("MainWindow", "Recherche", nullptr));
        groupFormat->setTitle(QCoreApplication::translate("MainWindow", "Choisir le format", nullptr));
        radioPDF->setText(QCoreApplication::translate("MainWindow", "PDF", nullptr));
        radioExcel->setText(QCoreApplication::translate("MainWindow", "Excel", nullptr));
        groupOptions->setTitle(QCoreApplication::translate("MainWindow", "Options d'export", nullptr));
        checkStats->setText(QCoreApplication::translate("MainWindow", "Inclure statistiques", nullptr));
        checkDateTime->setText(QCoreApplication::translate("MainWindow", "Inclure date et heure", nullptr));
        labelNomFichier->setText(QCoreApplication::translate("MainWindow", "Nom du fichier :", nullptr));
        lineNomFichier->setText(QCoreApplication::translate("MainWindow", "export_stock", nullptr));
        btnExporter->setText(QCoreApplication::translate("MainWindow", "Exporter le stock", nullptr));
        tabWidgetStock->setTabText(tabWidgetStock->indexOf(tabExport), QCoreApplication::translate("MainWindow", "Export", nullptr));
        groupResume->setTitle(QCoreApplication::translate("MainWindow", "Resume rapide", nullptr));
        labelTotalArticles->setText(QCoreApplication::translate("MainWindow", "Total d'articles : 0", nullptr));
        labelValeurTotale->setText(QCoreApplication::translate("MainWindow", "Volume total : 0 L", nullptr));
        groupOptionsStats->setTitle(QCoreApplication::translate("MainWindow", "Options de calcul", nullptr));
        labelStatsPar->setText(QCoreApplication::translate("MainWindow", "Statistiques par :", nullptr));
        comboStatsPar->setItemText(0, QCoreApplication::translate("MainWindow", "Type d'huile", nullptr));
        comboStatsPar->setItemText(1, QCoreApplication::translate("MainWindow", "Quantite", nullptr));
        comboStatsPar->setItemText(2, QCoreApplication::translate("MainWindow", "Etat", nullptr));
        comboStatsPar->setItemText(3, QCoreApplication::translate("MainWindow", "Jour", nullptr));
        comboStatsPar->setItemText(4, QCoreApplication::translate("MainWindow", "Mois", nullptr));

        btnCalculer->setText(QCoreApplication::translate("MainWindow", "Calculer", nullptr));
        groupStats->setTitle(QCoreApplication::translate("MainWindow", "Resume statistique", nullptr));
        tabWidgetStock->setTabText(tabWidgetStock->indexOf(tabStatistiques), QCoreApplication::translate("MainWindow", "Statistiques", nullptr));
        groupFiltrePeriode->setTitle(QCoreApplication::translate("MainWindow", "Filtre par periode", nullptr));
        labelDateDebut->setText(QCoreApplication::translate("MainWindow", "Date debut :", nullptr));
        labelDateFin->setText(QCoreApplication::translate("MainWindow", "Date fin :", nullptr));
        btnAfficherHistorique->setText(QCoreApplication::translate("MainWindow", "Afficher", nullptr));
        groupHistorique->setTitle(QCoreApplication::translate("MainWindow", "Historique des mouvements", nullptr));
        QTableWidgetItem *___qtablewidgetitem21 = tableHistorique->horizontalHeaderItem(0);
        ___qtablewidgetitem21->setText(QCoreApplication::translate("MainWindow", "Date et Heure", nullptr));
        QTableWidgetItem *___qtablewidgetitem22 = tableHistorique->horizontalHeaderItem(1);
        ___qtablewidgetitem22->setText(QCoreApplication::translate("MainWindow", "Type mouvement", nullptr));
        QTableWidgetItem *___qtablewidgetitem23 = tableHistorique->horizontalHeaderItem(2);
        ___qtablewidgetitem23->setText(QCoreApplication::translate("MainWindow", "Quantite (L)", nullptr));
        QTableWidgetItem *___qtablewidgetitem24 = tableHistorique->horizontalHeaderItem(3);
        ___qtablewidgetitem24->setText(QCoreApplication::translate("MainWindow", "Produit", nullptr));
        tabWidgetStock->setTabText(tabWidgetStock->indexOf(tabHistorique), QCoreApplication::translate("MainWindow", "Historique", nullptr));
        groupEtatGlobal->setTitle(QCoreApplication::translate("MainWindow", "Etat global du stock", nullptr));
        labelEtatGlobal->setText(QCoreApplication::translate("MainWindow", "Etat : Normal", nullptr));
        progressStock->setFormat(QCoreApplication::translate("MainWindow", "Niveau global : %p%", nullptr));
        groupProduitsCritiques->setTitle(QCoreApplication::translate("MainWindow", "Produits critiques", nullptr));
        tabWidgetStock->setTabText(tabWidgetStock->indexOf(tabEtatStock), QCoreApplication::translate("MainWindow", "Etat Stock", nullptr));
        groupTempsReel->setTitle(QCoreApplication::translate("MainWindow", "Suivi en temps reel", nullptr));
        labelQuantiteActuelle->setText(QCoreApplication::translate("MainWindow", "Quantite totale actuelle :", nullptr));
        labelUnite->setText(QCoreApplication::translate("MainWindow", "Litres", nullptr));
        labelDerniereMaj->setText(QCoreApplication::translate("MainWindow", "Derniere mise a jour : --", nullptr));
        labelInfoRefresh->setText(QCoreApplication::translate("MainWindow", "Mise a jour automatique toutes les 5 secondes", nullptr));
        tabWidgetStock->setTabText(tabWidgetStock->indexOf(tabTempsReel), QCoreApplication::translate("MainWindow", "Temps Reel", nullptr));
        groupConfigAlertes->setTitle(QCoreApplication::translate("MainWindow", "Configuration des alertes", nullptr));
        labelSeuilDefini->setText(QCoreApplication::translate("MainWindow", "Les alertes sont declenchees automatiquement quand la quantite est inferieure ou egale au seuil d'alerte", nullptr));
        btnVerifierAlertes->setText(QCoreApplication::translate("MainWindow", "Verifier maintenant", nullptr));
        groupStocksSousSeuil->setTitle(QCoreApplication::translate("MainWindow", "Stocks sous le seuil d'alerte", nullptr));
        tabWidgetStock->setTabText(tabWidgetStock->indexOf(tabAlertes), QCoreApplication::translate("MainWindow", "Alertes", nullptr));
        groupOptionsTendances->setTitle(QCoreApplication::translate("MainWindow", "Analyse des tendances", nullptr));
        labelPeriode->setText(QCoreApplication::translate("MainWindow", "Periode :", nullptr));
        comboPeriode->setItemText(0, QCoreApplication::translate("MainWindow", "7 derniers jours", nullptr));
        comboPeriode->setItemText(1, QCoreApplication::translate("MainWindow", "30 derniers jours", nullptr));
        comboPeriode->setItemText(2, QCoreApplication::translate("MainWindow", "3 derniers mois", nullptr));
        comboPeriode->setItemText(3, QCoreApplication::translate("MainWindow", "Cette annee", nullptr));

        btnAnalyserTendances->setText(QCoreApplication::translate("MainWindow", "Analyser", nullptr));
        groupGraphique->setTitle(QCoreApplication::translate("MainWindow", "Graphique de tendances", nullptr));
        labelPlaceholderChart->setText(QCoreApplication::translate("MainWindow", "Zone reservee pour QChartView (Necessite Qt Charts)", nullptr));
        groupAnalyseAuto->setTitle(QCoreApplication::translate("MainWindow", "Analyse automatique", nullptr));
        tabWidgetStock->setTabText(tabWidgetStock->indexOf(tabTendances), QCoreApplication::translate("MainWindow", "Tendances", nullptr));
        groupSelectionRapport->setTitle(QCoreApplication::translate("MainWindow", "Selectionner un stock", nullptr));
        QTableWidgetItem *___qtablewidgetitem25 = tableRapportSelection->horizontalHeaderItem(0);
        ___qtablewidgetitem25->setText(QCoreApplication::translate("MainWindow", "ID", nullptr));
        QTableWidgetItem *___qtablewidgetitem26 = tableRapportSelection->horizontalHeaderItem(1);
        ___qtablewidgetitem26->setText(QCoreApplication::translate("MainWindow", "Type d'huile", nullptr));
        QTableWidgetItem *___qtablewidgetitem27 = tableRapportSelection->horizontalHeaderItem(2);
        ___qtablewidgetitem27->setText(QCoreApplication::translate("MainWindow", "Quantite (L)", nullptr));
        QTableWidgetItem *___qtablewidgetitem28 = tableRapportSelection->horizontalHeaderItem(3);
        ___qtablewidgetitem28->setText(QCoreApplication::translate("MainWindow", "Seuil", nullptr));
        QTableWidgetItem *___qtablewidgetitem29 = tableRapportSelection->horizontalHeaderItem(4);
        ___qtablewidgetitem29->setText(QCoreApplication::translate("MainWindow", "Date MAJ", nullptr));
        QTableWidgetItem *___qtablewidgetitem30 = tableRapportSelection->horizontalHeaderItem(5);
        ___qtablewidgetitem30->setText(QCoreApplication::translate("MainWindow", "Emplacement", nullptr));
        QTableWidgetItem *___qtablewidgetitem31 = tableRapportSelection->horizontalHeaderItem(6);
        ___qtablewidgetitem31->setText(QCoreApplication::translate("MainWindow", "Etat", nullptr));
        btnGenererRapport->setText(QCoreApplication::translate("MainWindow", "Generer rapport", nullptr));
        btnExportRapportPDF->setText(QCoreApplication::translate("MainWindow", "Export PDF", nullptr));
        groupApercuRapport->setTitle(QCoreApplication::translate("MainWindow", "Apercu du rapport", nullptr));
        tabWidgetStock->setTabText(tabWidgetStock->indexOf(tabRapports), QCoreApplication::translate("MainWindow", "Rapports", nullptr));
        groupConnexion->setTitle(QCoreApplication::translate("MainWindow", "Connexion Arduino", nullptr));
        labelEtatConnexion->setText(QCoreApplication::translate("MainWindow", "Non connecte", nullptr));
        btnSynchroniser->setText(QCoreApplication::translate("MainWindow", "Synchroniser", nullptr));
        groupCapteur->setTitle(QCoreApplication::translate("MainWindow", "Donnees du capteur", nullptr));
        labelValeurCapteur->setText(QCoreApplication::translate("MainWindow", "Valeur capteur : -- L", nullptr));
        groupLogs->setTitle(QCoreApplication::translate("MainWindow", "Logs et Anomalies", nullptr));
        tabWidgetStock->setTabText(tabWidgetStock->indexOf(tabArduino), QCoreApplication::translate("MainWindow", "Arduino", nullptr));
        labelStatut->setText(QCoreApplication::translate("MainWindow", "Statut : Systeme operationnel", nullptr));
        labelCopyright->setText(QCoreApplication::translate("MainWindow", "(c) 2026 PRESSIQ - Tous droits reserves", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
