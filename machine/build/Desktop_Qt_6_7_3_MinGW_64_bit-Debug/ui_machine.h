/********************************************************************************
** Form generated from reading UI file 'machine.ui'
**
** Created by: Qt User Interface Compiler version 6.7.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MACHINE_H
#define UI_MACHINE_H

#include <QtCore/QDate>
#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDateEdit>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTableView>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_machine
{
public:
    QWidget *centralwidget;
    QVBoxLayout *mainLayout;
    QWidget *headerWidget;
    QHBoxLayout *headerLayout;
    QPushButton *btnBackToMenu;
    QLabel *lblLogo;
    QVBoxLayout *titleLayout;
    QLabel *lblAppTitle;
    QHBoxLayout *subtitleLayout;
    QLabel *lblAppSubtitle;
    QSpacerItem *subtitleSpacer;
    QPushButton *btnGestionStock;
    QSpacerItem *buttonsSpacer;
    QPushButton *btnGestionProduction;
    QSpacerItem *headerSpacer;
    QLabel *lblDateTime;
    QLabel *lblOnlineStatus;
    QTabWidget *tabWidgetMachineManagement;
    QWidget *tabParcMachines;
    QVBoxLayout *layoutParcMachines;
    QFrame *frameSummary;
    QHBoxLayout *layoutSummary;
    QLabel *lblMachinesNormales;
    QLabel *lblMachinesAlerte;
    QLabel *lblMachinesPanne;
    QSpacerItem *summarySpacer;
    QGroupBox *groupRecherche;
    QVBoxLayout *layoutRechercheGlobal;
    QHBoxLayout *layoutRechercheRapide;
    QLabel *lblRechercheRapide;
    QLineEdit *recherche_nom_machine;
    QPushButton *btnRechercher;
    QPushButton *btnHistoriqueToggle;
    QSpacerItem *rechercheRapideSpacer;
    QFrame *separateur;
    QVBoxLayout *layoutFiltresAvances;
    QLabel *lblFiltresAvances;
    QHBoxLayout *layoutFiltresLigne1;
    QComboBox *filtre_type_machine;
    QComboBox *filtre_etat_marche;
    QComboBox *filtre_type_alerte;
    QComboBox *filtre_alerte_criticite;
    QHBoxLayout *layoutFiltresLigne2;
    QLabel *lblPeriodeDu;
    QDateEdit *date_debut;
    QLabel *lblPeriodeAu;
    QDateEdit *date_fin;
    QComboBox *tri_par;
    QPushButton *btnFiltrer;
    QSpacerItem *filtresAvancesSpacer;
    QWidget *widgetZoneCentrale;
    QVBoxLayout *layoutZoneCentrale;
    QToolBar *toolBarActions;
    QWidget *widgetFormulaireParcMachines;
    QHBoxLayout *layoutFormulaireParcMachines;
    QSpacerItem *formParcSpacerLeft;
    QGroupBox *groupFormMachineParc;
    QFormLayout *formLayoutMachineParc;
    QLabel *lblNomMachineParc;
    QLineEdit *nom_machine_parc;
    QLabel *lblTypeMachineParc;
    QComboBox *type_machine_parc;
    QLabel *lblEtatMarcheParc;
    QComboBox *etat_marche_parc;
    QLabel *lblTemperatureParc;
    QDoubleSpinBox *temperature_actuelle_parc;
    QLabel *lblNiveauChargeParc;
    QSpinBox *niveau_charge_parc;
    QLabel *lblEtatFonctionnementParc;
    QComboBox *etat_fonctionnement_parc;
    QLabel *lblTypeAlerteParc;
    QComboBox *type_alerte_parc;
    QLabel *lblNiveauCriticiteParc;
    QComboBox *niveau_criticite_parc;
    QLabel *lblDateMaintenanceParc;
    QDateEdit *date_derniere_maintenance_parc;
    QHBoxLayout *layoutBtnsFormParc;
    QPushButton *btnEnregistrerParc;
    QPushButton *btnAnnulerParc;
    QSpacerItem *formParcSpacerRight;
    QScrollArea *scrollHistoriqueOnOff;
    QWidget *scrollAreaWidgetContents;
    QVBoxLayout *layoutHistoriqueList;
    QHBoxLayout *layoutHistoriqueHeader;
    QLabel *lblHistoriqueTitle;
    QSpacerItem *spacerHistoriqueHeader;
    QPushButton *btnFermerHistorique;
    QFrame *frameMachine1;
    QHBoxLayout *layoutMachine1;
    QLabel *lblMachine1Nom;
    QLabel *lblMachine1On;
    QLabel *lblMachine1Off;
    QSpacerItem *spacerMachine1;
    QFrame *frameMachine2;
    QHBoxLayout *layoutMachine2;
    QLabel *lblMachine2Nom;
    QLabel *lblMachine2On;
    QLabel *lblMachine2Off;
    QSpacerItem *spacerMachine2;
    QFrame *frameMachine3;
    QHBoxLayout *layoutMachine3;
    QLabel *lblMachine3Nom;
    QLabel *lblMachine3On;
    QLabel *lblMachine3Off;
    QSpacerItem *spacerMachine3;
    QFrame *frameMachine4;
    QHBoxLayout *layoutMachine4;
    QLabel *lblMachine4Nom;
    QLabel *lblMachine4On;
    QLabel *lblMachine4Off;
    QSpacerItem *spacerMachine4;
    QFrame *frameMachine5;
    QHBoxLayout *layoutMachine5;
    QLabel *lblMachine5Nom;
    QLabel *lblMachine5On;
    QLabel *lblMachine5Off;
    QSpacerItem *spacerMachine5;
    QSpacerItem *verticalSpacerHistorique;
    QScrollArea *scrollListeMachines;
    QWidget *scrollAreaMachinesContent;
    QVBoxLayout *layoutListeMachines;
    QFrame *cardMachine1;
    QVBoxLayout *layoutCardMachine1;
    QHBoxLayout *layoutMachine1Header;
    QLabel *lblCard1Nom;
    QSpacerItem *spacerCard1Header;
    QLabel *lblCard1EtatMarche;
    QFrame *frameSeparator1;
    QGridLayout *gridCard1Info;
    QLabel *lblCard1TypeLabel;
    QLabel *lblCard1Type;
    QLabel *lblCard1TempLabel;
    QLabel *lblCard1Temp;
    QLabel *lblCard1EtatFonctLabel;
    QLabel *lblCard1EtatFonct;
    QLabel *lblCard1ChargeLabel;
    QLabel *lblCard1Charge;
    QLabel *lblCard1AlerteLabel;
    QLabel *lblCard1Alerte;
    QLabel *lblCard1CriticiteLabel;
    QLabel *lblCard1Criticite;
    QLabel *lblCard1MaintenanceLabel;
    QLabel *lblCard1Maintenance;
    QFrame *cardMachine2;
    QVBoxLayout *layoutCardMachine2;
    QHBoxLayout *layoutMachine2Header;
    QLabel *lblCard2Nom;
    QSpacerItem *spacerCard2Header;
    QLabel *lblCard2EtatMarche;
    QFrame *frameSeparator2;
    QGridLayout *gridCard2Info;
    QLabel *lblCard2TypeLabel;
    QLabel *lblCard2Type;
    QLabel *lblCard2TempLabel;
    QLabel *lblCard2Temp;
    QLabel *lblCard2EtatFonctLabel;
    QLabel *lblCard2EtatFonct;
    QLabel *lblCard2ChargeLabel;
    QLabel *lblCard2Charge;
    QLabel *lblCard2AlerteLabel;
    QLabel *lblCard2Alerte;
    QLabel *lblCard2CriticiteLabel;
    QLabel *lblCard2Criticite;
    QLabel *lblCard2MaintenanceLabel;
    QLabel *lblCard2Maintenance;
    QFrame *cardMachine3;
    QVBoxLayout *layoutCardMachine3;
    QHBoxLayout *layoutMachine3Header;
    QLabel *lblCard3Nom;
    QSpacerItem *spacerCard3Header;
    QLabel *lblCard3EtatMarche;
    QFrame *frameSeparator3;
    QGridLayout *gridCard3Info;
    QLabel *lblCard3TypeLabel;
    QLabel *lblCard3Type;
    QLabel *lblCard3TempLabel;
    QLabel *lblCard3Temp;
    QLabel *lblCard3EtatFonctLabel;
    QLabel *lblCard3EtatFonct;
    QLabel *lblCard3ChargeLabel;
    QLabel *lblCard3Charge;
    QLabel *lblCard3AlerteLabel;
    QLabel *lblCard3Alerte;
    QLabel *lblCard3CriticiteLabel;
    QLabel *lblCard3Criticite;
    QLabel *lblCard3MaintenanceLabel;
    QLabel *lblCard3Maintenance;
    QSpacerItem *verticalSpacerListeMachines;
    QTableView *tableMachines;
    QWidget *tabAjouterModifier;
    QHBoxLayout *layoutAjouterModifier;
    QSpacerItem *formSpacerLeft;
    QGroupBox *groupFormMachine;
    QFormLayout *formLayoutMachine;
    QLabel *lblNomMachine;
    QLineEdit *nom_machine;
    QLabel *lblTypeMachine;
    QComboBox *type_machine;
    QLabel *lblEtatMarche;
    QComboBox *etat_marche;
    QLabel *lblTemperature;
    QDoubleSpinBox *temperature_actuelle;
    QLabel *lblNiveauCharge;
    QSpinBox *niveau_charge;
    QLabel *lblEtatFonctionnement;
    QComboBox *etat_fonctionnement;
    QLabel *lblTypeAlerte;
    QComboBox *type_alerte;
    QLabel *lblNiveauCriticite;
    QComboBox *niveau_criticite;
    QLabel *lblDateMaintenance;
    QDateEdit *date_derniere_maintenance;
    QHBoxLayout *layoutBtnsForm;
    QPushButton *btnEnregistrer;
    QPushButton *btnAnnuler;
    QSpacerItem *formSpacerRight;
    QWidget *tabDetails;
    QHBoxLayout *layoutDetails;
    QGroupBox *groupInfosMachine;
    QFormLayout *formInfos;
    QLabel *lblInfoNom;
    QLabel *lblValeurNom;
    QLabel *lblInfoType;
    QLabel *lblValeurType;
    QLabel *lblInfoEtat;
    QLabel *lblValeurEtat;
    QLabel *lblInfoTemp;
    QLabel *lblValeurTemp;
    QLabel *lblInfoScore;
    QProgressBar *progressScoreSante;
    QGroupBox *groupActions;
    QVBoxLayout *layoutActions;
    QPushButton *btnReinitialiser;
    QPushButton *btnMiseVeille;
    QPushButton *btnArretForce;
    QSpacerItem *actionsSpacer;
    QWidget *tabStatistiques;
    QVBoxLayout *layoutStatistiques;
    QGroupBox *groupOptionsStats;
    QHBoxLayout *layoutOptionsStats;
    QLabel *lblTypeStats;
    QComboBox *type_statistique;
    QLabel *lblTypeGraphique;
    QComboBox *type_graphique;
    QPushButton *btnGenererStats;
    QSpacerItem *statsSpacer;
    QWidget *zoneGraphique;
    QGroupBox *groupMaintenancePredictive;
    QVBoxLayout *layoutMaintenanceTable;
    QTableView *tableMaintenance;
    QHBoxLayout *layoutBtnsMaintenance;
    QSpacerItem *maintenanceSpacerLeft;
    QPushButton *btnEnregistrerMaintenance;
    QPushButton *btnPlanifierMaintenance;
    QSpacerItem *maintenanceSpacerRight;
    QWidget *tabHistorique;
    QVBoxLayout *layoutHistorique;
    QGroupBox *groupHistoriqueOnOff;
    QVBoxLayout *layoutHistoriqueTable;
    QTableView *tableHistorique;
    QFrame *frameTempsTotal;
    QHBoxLayout *layoutTempsTotal;
    QLabel *lblTempsTotalOn;
    QLabel *lblTempsTotalOff;
    QSpacerItem *tempsSpacer;
    QWidget *tabDecisionIA;
    QVBoxLayout *layoutDecisionIA;
    QGroupBox *groupRecommandationIA;
    QVBoxLayout *layoutRecommandation;
    QLabel *lblRecommandationIA;
    QHBoxLayout *layoutConfiance;
    QLabel *lblConfianceLabel;
    QProgressBar *progressConfianceIA;
    QHBoxLayout *layoutBtnsIA;
    QSpacerItem *iaSpacerLeft;
    QPushButton *btnAppliquerDecision;
    QPushButton *btnIgnorerDecision;
    QSpacerItem *iaSpacerRight;
    QSpacerItem *iaVerticalSpacer;

    void setupUi(QMainWindow *machine)
    {
        if (machine->objectName().isEmpty())
            machine->setObjectName("machine");
        machine->resize(1441, 616);
        machine->setMinimumSize(QSize(1200, 600));
        machine->setStyleSheet(QString::fromUtf8("\n"
"QMainWindow {\n"
"    background-color: #f5f5f5;\n"
"}\n"
"\n"
"/* Header Styling */\n"
"#headerWidget {\n"
"    background-color: #1A3C2F;\n"
"    border-bottom: 3px solid #C9A227;\n"
"}\n"
"\n"
"#lblAppTitle {\n"
"    color: white;\n"
"    font-size: 28px;\n"
"    font-weight: bold;\n"
"    font-family: 'Segoe UI', Arial, sans-serif;\n"
"    letter-spacing: 2px;\n"
"}\n"
"\n"
"#lblAppSubtitle {\n"
"    color: #C9A227;\n"
"    font-size: 12px;\n"
"    font-weight: 600;\n"
"    font-family: 'Segoe UI', Arial, sans-serif;\n"
"    letter-spacing: 3px;\n"
"}\n"
"\n"
"#lblOnlineStatus {\n"
"    color: #4CAF50;\n"
"    font-size: 13px;\n"
"    font-weight: 600;\n"
"    font-family: 'Segoe UI', Arial, sans-serif;\n"
"    background-color: #2C5F4F;\n"
"    padding: 8px 16px;\n"
"    border-radius: 6px;\n"
"    border: 1px solid #3D7A60;\n"
"    margin-left: 12px;\n"
"}\n"
"\n"
"#lblDateTime {\n"
"    color: #E8E8E8;\n"
"    font-size: 13px;\n"
"    font-weight: 600;\n"
"    font-family: 'Segoe UI', Arial, sans-s"
                        "erif;\n"
"    background-color: #2C5F4F;\n"
"    padding: 8px 16px;\n"
"    border-radius: 6px;\n"
"    border: 1px solid #3D7A60;\n"
"}\n"
"\n"
"/* Back Button */\n"
"#btnBackToMenu {\n"
"    background-color: #C9A227;\n"
"    color: white;\n"
"    font-size: 13px;\n"
"    font-weight: bold;\n"
"    font-family: 'Segoe UI', Arial, sans-serif;\n"
"    padding: 8px 16px;\n"
"    border-radius: 4px;\n"
"    border: none;\n"
"    min-width: 80px;\n"
"}\n"
"\n"
"#btnBackToMenu:hover {\n"
"    background-color: #B8911F;\n"
"}\n"
"\n"
"#btnBackToMenu:pressed {\n"
"    background-color: #9A7A1A;\n"
"}\n"
"\n"
"/* Tab Widget Styling */\n"
"QTabWidget::pane {\n"
"    border: 3px solid #1A3C2F;\n"
"    background-color: white;\n"
"    border-radius: 5px;\n"
"    margin-top: -1px;\n"
"}\n"
"\n"
"QTabBar::tab {\n"
"    background-color: #f0f0f0;\n"
"    color: #444;\n"
"    padding: 8px 16px;\n"
"    margin-right: 2px;\n"
"    border: 1px solid #ddd;\n"
"    border-bottom: none;\n"
"    border-top-left-radius: 4px;\n"
"  "
                        "  border-top-right-radius: 4px;\n"
"    font-size: 11px;\n"
"    font-weight: 600;\n"
"    font-family: 'Segoe UI', Arial, sans-serif;\n"
"}\n"
"\n"
"QTabBar::tab:selected {\n"
"    background-color: white;\n"
"    color: #1A3C2F;\n"
"    font-weight: bold;\n"
"    border-bottom: 3px solid #1A3C2F;\n"
"}\n"
"\n"
"QTabBar::tab:hover:!selected {\n"
"    background-color: #e8e8e8;\n"
"}\n"
"\n"
"/* Group Box Styling */\n"
"QGroupBox {\n"
"    font-size: 12px;\n"
"    font-weight: bold;\n"
"    font-family: 'Segoe UI', Arial, sans-serif;\n"
"    color: white;\n"
"    background-color: white;\n"
"    border: 2px solid #1A3C2F;\n"
"    border-radius: 5px;\n"
"    margin-top: 12px;\n"
"    padding-top: 10px;\n"
"}\n"
"\n"
"QGroupBox::title {\n"
"    subcontrol-origin: margin;\n"
"    subcontrol-position: top left;\n"
"    left: 0px;\n"
"    padding: 5px 12px;\n"
"    background-color: #1B4D3E;\n"
"    border-top-left-radius: 5px;\n"
"    border-top-right-radius: 5px;\n"
"}\n"
"\n"
"/* Input Field Styling */\n"
"QLine"
                        "Edit, QSpinBox, QDoubleSpinBox, QDateEdit, QComboBox {\n"
"    padding: 5px 8px;\n"
"    border: 1px solid #ddd;\n"
"    border-radius: 4px;\n"
"    background-color: white;\n"
"    color: #333;\n"
"    font-size: 12px;\n"
"    font-family: 'Segoe UI', Arial, sans-serif;\n"
"    min-height: 16px;\n"
"}\n"
"\n"
"QLineEdit:focus, QSpinBox:focus, QDoubleSpinBox:focus, QDateEdit:focus, QComboBox:focus {\n"
"    border: 2px solid #1A3C2F;\n"
"}\n"
"\n"
"/* Button Styling */\n"
"QPushButton {\n"
"    padding: 6px 14px;\n"
"    border-radius: 4px;\n"
"    font-size: 12px;\n"
"    font-weight: bold;\n"
"    font-family: 'Segoe UI', Arial, sans-serif;\n"
"    min-width: 90px;\n"
"    background-color: #1B4D3E;\n"
"    color: white;\n"
"    border: none;\n"
"}\n"
"\n"
"QPushButton:hover {\n"
"    background-color: #1A3C2F;\n"
"}\n"
"\n"
"QPushButton:pressed {\n"
"    background-color: #0D3B2E;\n"
"}\n"
"\n"
"/* Table Styling */\n"
"QTableView, QTableWidget {\n"
"    border: 1px solid #ddd;\n"
"    border-radius: 4px;\n"
""
                        "    background-color: white;\n"
"    gridline-color: #eee;\n"
"    font-size: 11px;\n"
"}\n"
"\n"
"QTableView::item, QTableWidget::item {\n"
"    padding: 6px;\n"
"}\n"
"\n"
"QHeaderView::section {\n"
"    background-color: #1B4D3E;\n"
"    color: white;\n"
"    padding: 8px;\n"
"    border: none;\n"
"    font-weight: bold;\n"
"    font-size: 11px;\n"
"}\n"
"\n"
"/* Label Styling */\n"
"QLabel {\n"
"    color: #333;\n"
"    font-size: 12px;\n"
"    font-family: 'Segoe UI', Arial, sans-serif;\n"
"}\n"
"\n"
"/* ToolBar */\n"
"QToolBar {\n"
"    background-color: #f8f9fa;\n"
"    border: 2px solid #1A3C2F;\n"
"    border-radius: 4px;\n"
"    spacing: 5px;\n"
"    padding: 5px;\n"
"}\n"
"\n"
"QToolButton {\n"
"    background-color: #1B4D3E;\n"
"    color: white;\n"
"    border: none;\n"
"    border-radius: 4px;\n"
"    padding: 6px 12px;\n"
"    font-weight: bold;\n"
"    font-size: 11px;\n"
"}\n"
"\n"
"QToolButton:hover {\n"
"    background-color: #1A3C2F;\n"
"}\n"
"\n"
"/* Frame Summary - Modern Material Design "
                        "*/\n"
"#frameSummary {\n"
"    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #FAFAFA, stop:1 #F5F5F5);\n"
"    border: 2px solid #1A3C2F;\n"
"    border-radius: 12px;\n"
"    padding: 12px;\n"
"}\n"
"\n"
"#frameSummary QLabel {\n"
"    font-family: 'Segoe UI', 'Roboto', Arial, sans-serif;\n"
"    font-size: 14px;\n"
"    font-weight: 600;\n"
"    padding: 10px 16px;\n"
"    border-radius: 10px;\n"
"    border: none;\n"
"    min-height: 50px;\n"
"    letter-spacing: 0.3px;\n"
"}\n"
"\n"
"#lblMachinesNormales {\n"
"    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #E8F5E9, stop:1 #C8E6C9);\n"
"    color: #1B5E20;\n"
"    border-left: 5px solid #4CAF50;\n"
"}\n"
"\n"
"#lblMachinesNormales:hover {\n"
"    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #C8E6C9, stop:1 #A5D6A7);\n"
"    border-left: 5px solid #388E3C;\n"
"}\n"
"\n"
"#lblMachinesAlerte {\n"
"    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #FFF9C4, stop:1 #FFF59D);\n"
"    color: #F57F17;\n"
"    bo"
                        "rder-left: 5px solid #FFC107;\n"
"}\n"
"\n"
"#lblMachinesAlerte:hover {\n"
"    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #FFF59D, stop:1 #FFF176);\n"
"    border-left: 5px solid #FFA000;\n"
"}\n"
"\n"
"#lblMachinesPanne {\n"
"    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #FFEBEE, stop:1 #FFCDD2);\n"
"    color: #B71C1C;\n"
"    border-left: 5px solid #F44336;\n"
"}\n"
"\n"
"#lblMachinesPanne:hover {\n"
"    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #FFCDD2, stop:1 #EF9A9A);\n"
"    border-left: 5px solid #D32F2F;\n"
"}\n"
"   "));
        centralwidget = new QWidget(machine);
        centralwidget->setObjectName("centralwidget");
        mainLayout = new QVBoxLayout(centralwidget);
        mainLayout->setSpacing(0);
        mainLayout->setObjectName("mainLayout");
        mainLayout->setContentsMargins(0, 0, 0, 0);
        headerWidget = new QWidget(centralwidget);
        headerWidget->setObjectName("headerWidget");
        headerWidget->setMinimumSize(QSize(0, 90));
        headerWidget->setMaximumSize(QSize(16777215, 90));
        headerLayout = new QHBoxLayout(headerWidget);
        headerLayout->setSpacing(15);
        headerLayout->setObjectName("headerLayout");
        headerLayout->setContentsMargins(20, 5, 20, 5);
        btnBackToMenu = new QPushButton(headerWidget);
        btnBackToMenu->setObjectName("btnBackToMenu");

        headerLayout->addWidget(btnBackToMenu);

        lblLogo = new QLabel(headerWidget);
        lblLogo->setObjectName("lblLogo");
        lblLogo->setMinimumSize(QSize(120, 80));
        lblLogo->setMaximumSize(QSize(120, 80));
        lblLogo->setPixmap(QPixmap(QString::fromUtf8(":/assets/logo.png")));
        lblLogo->setScaledContents(true);

        headerLayout->addWidget(lblLogo);

        titleLayout = new QVBoxLayout();
        titleLayout->setSpacing(2);
        titleLayout->setObjectName("titleLayout");
        lblAppTitle = new QLabel(headerWidget);
        lblAppTitle->setObjectName("lblAppTitle");

        titleLayout->addWidget(lblAppTitle);

        subtitleLayout = new QHBoxLayout();
        subtitleLayout->setObjectName("subtitleLayout");
        lblAppSubtitle = new QLabel(headerWidget);
        lblAppSubtitle->setObjectName("lblAppSubtitle");

        subtitleLayout->addWidget(lblAppSubtitle);

        subtitleSpacer = new QSpacerItem(200, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        subtitleLayout->addItem(subtitleSpacer);

        btnGestionStock = new QPushButton(headerWidget);
        btnGestionStock->setObjectName("btnGestionStock");
        btnGestionStock->setMinimumSize(QSize(118, 30));
        btnGestionStock->setMaximumSize(QSize(200, 30));

        subtitleLayout->addWidget(btnGestionStock);

        buttonsSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        subtitleLayout->addItem(buttonsSpacer);

        btnGestionProduction = new QPushButton(headerWidget);
        btnGestionProduction->setObjectName("btnGestionProduction");
        btnGestionProduction->setMinimumSize(QSize(118, 30));
        btnGestionProduction->setMaximumSize(QSize(220, 30));

        subtitleLayout->addWidget(btnGestionProduction);


        titleLayout->addLayout(subtitleLayout);


        headerLayout->addLayout(titleLayout);

        headerSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        headerLayout->addItem(headerSpacer);

        lblDateTime = new QLabel(headerWidget);
        lblDateTime->setObjectName("lblDateTime");
        lblDateTime->setMaximumSize(QSize(350, 35));
        lblDateTime->setAlignment(Qt::AlignmentFlag::AlignRight|Qt::AlignmentFlag::AlignTrailing|Qt::AlignmentFlag::AlignVCenter);

        headerLayout->addWidget(lblDateTime);

        lblOnlineStatus = new QLabel(headerWidget);
        lblOnlineStatus->setObjectName("lblOnlineStatus");
        lblOnlineStatus->setMaximumSize(QSize(120, 35));
        lblOnlineStatus->setAlignment(Qt::AlignmentFlag::AlignRight|Qt::AlignmentFlag::AlignTrailing|Qt::AlignmentFlag::AlignVCenter);

        headerLayout->addWidget(lblOnlineStatus);


        mainLayout->addWidget(headerWidget);

        tabWidgetMachineManagement = new QTabWidget(centralwidget);
        tabWidgetMachineManagement->setObjectName("tabWidgetMachineManagement");
        tabParcMachines = new QWidget();
        tabParcMachines->setObjectName("tabParcMachines");
        layoutParcMachines = new QVBoxLayout(tabParcMachines);
        layoutParcMachines->setSpacing(8);
        layoutParcMachines->setObjectName("layoutParcMachines");
        layoutParcMachines->setContentsMargins(15, 10, 15, 10);
        frameSummary = new QFrame(tabParcMachines);
        frameSummary->setObjectName("frameSummary");
        frameSummary->setFrameShape(QFrame::Shape::NoFrame);
        layoutSummary = new QHBoxLayout(frameSummary);
        layoutSummary->setSpacing(12);
        layoutSummary->setObjectName("layoutSummary");
        lblMachinesNormales = new QLabel(frameSummary);
        lblMachinesNormales->setObjectName("lblMachinesNormales");
        lblMachinesNormales->setMinimumSize(QSize(0, 70));
        lblMachinesNormales->setAlignment(Qt::AlignmentFlag::AlignCenter);
        lblMachinesNormales->setWordWrap(true);

        layoutSummary->addWidget(lblMachinesNormales);

        lblMachinesAlerte = new QLabel(frameSummary);
        lblMachinesAlerte->setObjectName("lblMachinesAlerte");
        lblMachinesAlerte->setMinimumSize(QSize(0, 70));
        lblMachinesAlerte->setAlignment(Qt::AlignmentFlag::AlignCenter);
        lblMachinesAlerte->setWordWrap(true);

        layoutSummary->addWidget(lblMachinesAlerte);

        lblMachinesPanne = new QLabel(frameSummary);
        lblMachinesPanne->setObjectName("lblMachinesPanne");
        lblMachinesPanne->setMinimumSize(QSize(0, 70));
        lblMachinesPanne->setAlignment(Qt::AlignmentFlag::AlignCenter);
        lblMachinesPanne->setWordWrap(true);

        layoutSummary->addWidget(lblMachinesPanne);

        summarySpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        layoutSummary->addItem(summarySpacer);


        layoutParcMachines->addWidget(frameSummary);

        groupRecherche = new QGroupBox(tabParcMachines);
        groupRecherche->setObjectName("groupRecherche");
        layoutRechercheGlobal = new QVBoxLayout(groupRecherche);
        layoutRechercheGlobal->setSpacing(12);
        layoutRechercheGlobal->setObjectName("layoutRechercheGlobal");
        layoutRechercheGlobal->setContentsMargins(15, 18, 15, 15);
        layoutRechercheRapide = new QHBoxLayout();
        layoutRechercheRapide->setSpacing(10);
        layoutRechercheRapide->setObjectName("layoutRechercheRapide");
        lblRechercheRapide = new QLabel(groupRecherche);
        lblRechercheRapide->setObjectName("lblRechercheRapide");

        layoutRechercheRapide->addWidget(lblRechercheRapide);

        recherche_nom_machine = new QLineEdit(groupRecherche);
        recherche_nom_machine->setObjectName("recherche_nom_machine");

        layoutRechercheRapide->addWidget(recherche_nom_machine);

        btnRechercher = new QPushButton(groupRecherche);
        btnRechercher->setObjectName("btnRechercher");

        layoutRechercheRapide->addWidget(btnRechercher);

        btnHistoriqueToggle = new QPushButton(groupRecherche);
        btnHistoriqueToggle->setObjectName("btnHistoriqueToggle");
        btnHistoriqueToggle->setCheckable(true);

        layoutRechercheRapide->addWidget(btnHistoriqueToggle);

        rechercheRapideSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        layoutRechercheRapide->addItem(rechercheRapideSpacer);


        layoutRechercheGlobal->addLayout(layoutRechercheRapide);

        separateur = new QFrame(groupRecherche);
        separateur->setObjectName("separateur");
        separateur->setMaximumSize(QSize(16777215, 1));
        separateur->setFrameShape(QFrame::Shape::HLine);

        layoutRechercheGlobal->addWidget(separateur);

        layoutFiltresAvances = new QVBoxLayout();
        layoutFiltresAvances->setSpacing(8);
        layoutFiltresAvances->setObjectName("layoutFiltresAvances");
        lblFiltresAvances = new QLabel(groupRecherche);
        lblFiltresAvances->setObjectName("lblFiltresAvances");

        layoutFiltresAvances->addWidget(lblFiltresAvances);

        layoutFiltresLigne1 = new QHBoxLayout();
        layoutFiltresLigne1->setSpacing(10);
        layoutFiltresLigne1->setObjectName("layoutFiltresLigne1");
        filtre_type_machine = new QComboBox(groupRecherche);
        filtre_type_machine->addItem(QString());
        filtre_type_machine->addItem(QString());
        filtre_type_machine->addItem(QString());
        filtre_type_machine->addItem(QString());
        filtre_type_machine->addItem(QString());
        filtre_type_machine->setObjectName("filtre_type_machine");

        layoutFiltresLigne1->addWidget(filtre_type_machine);

        filtre_etat_marche = new QComboBox(groupRecherche);
        filtre_etat_marche->addItem(QString());
        filtre_etat_marche->addItem(QString());
        filtre_etat_marche->addItem(QString());
        filtre_etat_marche->addItem(QString());
        filtre_etat_marche->setObjectName("filtre_etat_marche");

        layoutFiltresLigne1->addWidget(filtre_etat_marche);

        filtre_type_alerte = new QComboBox(groupRecherche);
        filtre_type_alerte->addItem(QString());
        filtre_type_alerte->addItem(QString());
        filtre_type_alerte->addItem(QString());
        filtre_type_alerte->addItem(QString());
        filtre_type_alerte->addItem(QString());
        filtre_type_alerte->setObjectName("filtre_type_alerte");

        layoutFiltresLigne1->addWidget(filtre_type_alerte);

        filtre_alerte_criticite = new QComboBox(groupRecherche);
        filtre_alerte_criticite->addItem(QString());
        filtre_alerte_criticite->addItem(QString());
        filtre_alerte_criticite->addItem(QString());
        filtre_alerte_criticite->addItem(QString());
        filtre_alerte_criticite->addItem(QString());
        filtre_alerte_criticite->setObjectName("filtre_alerte_criticite");

        layoutFiltresLigne1->addWidget(filtre_alerte_criticite);


        layoutFiltresAvances->addLayout(layoutFiltresLigne1);

        layoutFiltresLigne2 = new QHBoxLayout();
        layoutFiltresLigne2->setSpacing(10);
        layoutFiltresLigne2->setObjectName("layoutFiltresLigne2");
        lblPeriodeDu = new QLabel(groupRecherche);
        lblPeriodeDu->setObjectName("lblPeriodeDu");

        layoutFiltresLigne2->addWidget(lblPeriodeDu);

        date_debut = new QDateEdit(groupRecherche);
        date_debut->setObjectName("date_debut");
        date_debut->setCalendarPopup(true);

        layoutFiltresLigne2->addWidget(date_debut);

        lblPeriodeAu = new QLabel(groupRecherche);
        lblPeriodeAu->setObjectName("lblPeriodeAu");

        layoutFiltresLigne2->addWidget(lblPeriodeAu);

        date_fin = new QDateEdit(groupRecherche);
        date_fin->setObjectName("date_fin");
        date_fin->setCalendarPopup(true);

        layoutFiltresLigne2->addWidget(date_fin);

        tri_par = new QComboBox(groupRecherche);
        tri_par->addItem(QString());
        tri_par->addItem(QString());
        tri_par->addItem(QString());
        tri_par->addItem(QString());
        tri_par->addItem(QString());
        tri_par->addItem(QString());
        tri_par->addItem(QString());
        tri_par->setObjectName("tri_par");

        layoutFiltresLigne2->addWidget(tri_par);

        btnFiltrer = new QPushButton(groupRecherche);
        btnFiltrer->setObjectName("btnFiltrer");

        layoutFiltresLigne2->addWidget(btnFiltrer);

        filtresAvancesSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        layoutFiltresLigne2->addItem(filtresAvancesSpacer);


        layoutFiltresAvances->addLayout(layoutFiltresLigne2);


        layoutRechercheGlobal->addLayout(layoutFiltresAvances);


        layoutParcMachines->addWidget(groupRecherche);

        widgetZoneCentrale = new QWidget(tabParcMachines);
        widgetZoneCentrale->setObjectName("widgetZoneCentrale");
        layoutZoneCentrale = new QVBoxLayout(widgetZoneCentrale);
        layoutZoneCentrale->setSpacing(10);
        layoutZoneCentrale->setObjectName("layoutZoneCentrale");
        layoutZoneCentrale->setContentsMargins(0, 0, 0, 0);
        toolBarActions = new QToolBar(widgetZoneCentrale);
        toolBarActions->setObjectName("toolBarActions");
        toolBarActions->setMovable(false);
        toolBarActions->setFloatable(false);

        layoutZoneCentrale->addWidget(toolBarActions);

        widgetFormulaireParcMachines = new QWidget(widgetZoneCentrale);
        widgetFormulaireParcMachines->setObjectName("widgetFormulaireParcMachines");
        widgetFormulaireParcMachines->setVisible(false);
        layoutFormulaireParcMachines = new QHBoxLayout(widgetFormulaireParcMachines);
        layoutFormulaireParcMachines->setObjectName("layoutFormulaireParcMachines");
        formParcSpacerLeft = new QSpacerItem(100, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        layoutFormulaireParcMachines->addItem(formParcSpacerLeft);

        groupFormMachineParc = new QGroupBox(widgetFormulaireParcMachines);
        groupFormMachineParc->setObjectName("groupFormMachineParc");
        groupFormMachineParc->setMinimumSize(QSize(600, 0));
        groupFormMachineParc->setMaximumSize(QSize(650, 16777215));
        formLayoutMachineParc = new QFormLayout(groupFormMachineParc);
        formLayoutMachineParc->setObjectName("formLayoutMachineParc");
        formLayoutMachineParc->setHorizontalSpacing(15);
        formLayoutMachineParc->setVerticalSpacing(12);
        formLayoutMachineParc->setContentsMargins(20, 20, 20, 20);
        lblNomMachineParc = new QLabel(groupFormMachineParc);
        lblNomMachineParc->setObjectName("lblNomMachineParc");

        formLayoutMachineParc->setWidget(0, QFormLayout::LabelRole, lblNomMachineParc);

        nom_machine_parc = new QLineEdit(groupFormMachineParc);
        nom_machine_parc->setObjectName("nom_machine_parc");

        formLayoutMachineParc->setWidget(0, QFormLayout::FieldRole, nom_machine_parc);

        lblTypeMachineParc = new QLabel(groupFormMachineParc);
        lblTypeMachineParc->setObjectName("lblTypeMachineParc");

        formLayoutMachineParc->setWidget(1, QFormLayout::LabelRole, lblTypeMachineParc);

        type_machine_parc = new QComboBox(groupFormMachineParc);
        type_machine_parc->addItem(QString());
        type_machine_parc->addItem(QString());
        type_machine_parc->addItem(QString());
        type_machine_parc->addItem(QString());
        type_machine_parc->setObjectName("type_machine_parc");

        formLayoutMachineParc->setWidget(1, QFormLayout::FieldRole, type_machine_parc);

        lblEtatMarcheParc = new QLabel(groupFormMachineParc);
        lblEtatMarcheParc->setObjectName("lblEtatMarcheParc");

        formLayoutMachineParc->setWidget(2, QFormLayout::LabelRole, lblEtatMarcheParc);

        etat_marche_parc = new QComboBox(groupFormMachineParc);
        etat_marche_parc->addItem(QString());
        etat_marche_parc->addItem(QString());
        etat_marche_parc->addItem(QString());
        etat_marche_parc->setObjectName("etat_marche_parc");

        formLayoutMachineParc->setWidget(2, QFormLayout::FieldRole, etat_marche_parc);

        lblTemperatureParc = new QLabel(groupFormMachineParc);
        lblTemperatureParc->setObjectName("lblTemperatureParc");

        formLayoutMachineParc->setWidget(3, QFormLayout::LabelRole, lblTemperatureParc);

        temperature_actuelle_parc = new QDoubleSpinBox(groupFormMachineParc);
        temperature_actuelle_parc->setObjectName("temperature_actuelle_parc");
        temperature_actuelle_parc->setMaximum(200.000000000000000);
        temperature_actuelle_parc->setSingleStep(0.500000000000000);

        formLayoutMachineParc->setWidget(3, QFormLayout::FieldRole, temperature_actuelle_parc);

        lblNiveauChargeParc = new QLabel(groupFormMachineParc);
        lblNiveauChargeParc->setObjectName("lblNiveauChargeParc");

        formLayoutMachineParc->setWidget(4, QFormLayout::LabelRole, lblNiveauChargeParc);

        niveau_charge_parc = new QSpinBox(groupFormMachineParc);
        niveau_charge_parc->setObjectName("niveau_charge_parc");
        niveau_charge_parc->setMaximum(100);

        formLayoutMachineParc->setWidget(4, QFormLayout::FieldRole, niveau_charge_parc);

        lblEtatFonctionnementParc = new QLabel(groupFormMachineParc);
        lblEtatFonctionnementParc->setObjectName("lblEtatFonctionnementParc");

        formLayoutMachineParc->setWidget(5, QFormLayout::LabelRole, lblEtatFonctionnementParc);

        etat_fonctionnement_parc = new QComboBox(groupFormMachineParc);
        etat_fonctionnement_parc->addItem(QString());
        etat_fonctionnement_parc->addItem(QString());
        etat_fonctionnement_parc->addItem(QString());
        etat_fonctionnement_parc->setObjectName("etat_fonctionnement_parc");

        formLayoutMachineParc->setWidget(5, QFormLayout::FieldRole, etat_fonctionnement_parc);

        lblTypeAlerteParc = new QLabel(groupFormMachineParc);
        lblTypeAlerteParc->setObjectName("lblTypeAlerteParc");

        formLayoutMachineParc->setWidget(6, QFormLayout::LabelRole, lblTypeAlerteParc);

        type_alerte_parc = new QComboBox(groupFormMachineParc);
        type_alerte_parc->addItem(QString());
        type_alerte_parc->addItem(QString());
        type_alerte_parc->addItem(QString());
        type_alerte_parc->addItem(QString());
        type_alerte_parc->setObjectName("type_alerte_parc");

        formLayoutMachineParc->setWidget(6, QFormLayout::FieldRole, type_alerte_parc);

        lblNiveauCriticiteParc = new QLabel(groupFormMachineParc);
        lblNiveauCriticiteParc->setObjectName("lblNiveauCriticiteParc");

        formLayoutMachineParc->setWidget(7, QFormLayout::LabelRole, lblNiveauCriticiteParc);

        niveau_criticite_parc = new QComboBox(groupFormMachineParc);
        niveau_criticite_parc->addItem(QString());
        niveau_criticite_parc->addItem(QString());
        niveau_criticite_parc->addItem(QString());
        niveau_criticite_parc->addItem(QString());
        niveau_criticite_parc->setObjectName("niveau_criticite_parc");

        formLayoutMachineParc->setWidget(7, QFormLayout::FieldRole, niveau_criticite_parc);

        lblDateMaintenanceParc = new QLabel(groupFormMachineParc);
        lblDateMaintenanceParc->setObjectName("lblDateMaintenanceParc");

        formLayoutMachineParc->setWidget(8, QFormLayout::LabelRole, lblDateMaintenanceParc);

        date_derniere_maintenance_parc = new QDateEdit(groupFormMachineParc);
        date_derniere_maintenance_parc->setObjectName("date_derniere_maintenance_parc");
        date_derniere_maintenance_parc->setCalendarPopup(true);
        date_derniere_maintenance_parc->setDate(QDate(2000, 1, 1));

        formLayoutMachineParc->setWidget(8, QFormLayout::FieldRole, date_derniere_maintenance_parc);

        layoutBtnsFormParc = new QHBoxLayout();
        layoutBtnsFormParc->setObjectName("layoutBtnsFormParc");
        btnEnregistrerParc = new QPushButton(groupFormMachineParc);
        btnEnregistrerParc->setObjectName("btnEnregistrerParc");

        layoutBtnsFormParc->addWidget(btnEnregistrerParc);

        btnAnnulerParc = new QPushButton(groupFormMachineParc);
        btnAnnulerParc->setObjectName("btnAnnulerParc");

        layoutBtnsFormParc->addWidget(btnAnnulerParc);


        formLayoutMachineParc->setLayout(9, QFormLayout::FieldRole, layoutBtnsFormParc);


        layoutFormulaireParcMachines->addWidget(groupFormMachineParc);

        formParcSpacerRight = new QSpacerItem(100, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        layoutFormulaireParcMachines->addItem(formParcSpacerRight);


        layoutZoneCentrale->addWidget(widgetFormulaireParcMachines);


        layoutParcMachines->addWidget(widgetZoneCentrale);

        scrollHistoriqueOnOff = new QScrollArea(tabParcMachines);
        scrollHistoriqueOnOff->setObjectName("scrollHistoriqueOnOff");
        scrollHistoriqueOnOff->setVisible(false);
        scrollHistoriqueOnOff->setWidgetResizable(true);
        scrollAreaWidgetContents = new QWidget();
        scrollAreaWidgetContents->setObjectName("scrollAreaWidgetContents");
        scrollAreaWidgetContents->setGeometry(QRect(0, 0, 725, 627));
        layoutHistoriqueList = new QVBoxLayout(scrollAreaWidgetContents);
        layoutHistoriqueList->setSpacing(15);
        layoutHistoriqueList->setObjectName("layoutHistoriqueList");
        layoutHistoriqueList->setContentsMargins(20, 20, 20, 20);
        layoutHistoriqueHeader = new QHBoxLayout();
        layoutHistoriqueHeader->setSpacing(15);
        layoutHistoriqueHeader->setObjectName("layoutHistoriqueHeader");
        lblHistoriqueTitle = new QLabel(scrollAreaWidgetContents);
        lblHistoriqueTitle->setObjectName("lblHistoriqueTitle");

        layoutHistoriqueHeader->addWidget(lblHistoriqueTitle);

        spacerHistoriqueHeader = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        layoutHistoriqueHeader->addItem(spacerHistoriqueHeader);

        btnFermerHistorique = new QPushButton(scrollAreaWidgetContents);
        btnFermerHistorique->setObjectName("btnFermerHistorique");
        btnFermerHistorique->setMinimumSize(QSize(118, 40));
        btnFermerHistorique->setMaximumSize(QSize(40, 40));

        layoutHistoriqueHeader->addWidget(btnFermerHistorique);


        layoutHistoriqueList->addLayout(layoutHistoriqueHeader);

        frameMachine1 = new QFrame(scrollAreaWidgetContents);
        frameMachine1->setObjectName("frameMachine1");
        layoutMachine1 = new QHBoxLayout(frameMachine1);
        layoutMachine1->setSpacing(15);
        layoutMachine1->setObjectName("layoutMachine1");
        lblMachine1Nom = new QLabel(frameMachine1);
        lblMachine1Nom->setObjectName("lblMachine1Nom");
        lblMachine1Nom->setMinimumSize(QSize(200, 0));

        layoutMachine1->addWidget(lblMachine1Nom);

        lblMachine1On = new QLabel(frameMachine1);
        lblMachine1On->setObjectName("lblMachine1On");

        layoutMachine1->addWidget(lblMachine1On);

        lblMachine1Off = new QLabel(frameMachine1);
        lblMachine1Off->setObjectName("lblMachine1Off");

        layoutMachine1->addWidget(lblMachine1Off);

        spacerMachine1 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        layoutMachine1->addItem(spacerMachine1);


        layoutHistoriqueList->addWidget(frameMachine1);

        frameMachine2 = new QFrame(scrollAreaWidgetContents);
        frameMachine2->setObjectName("frameMachine2");
        layoutMachine2 = new QHBoxLayout(frameMachine2);
        layoutMachine2->setSpacing(15);
        layoutMachine2->setObjectName("layoutMachine2");
        lblMachine2Nom = new QLabel(frameMachine2);
        lblMachine2Nom->setObjectName("lblMachine2Nom");
        lblMachine2Nom->setMinimumSize(QSize(200, 0));

        layoutMachine2->addWidget(lblMachine2Nom);

        lblMachine2On = new QLabel(frameMachine2);
        lblMachine2On->setObjectName("lblMachine2On");

        layoutMachine2->addWidget(lblMachine2On);

        lblMachine2Off = new QLabel(frameMachine2);
        lblMachine2Off->setObjectName("lblMachine2Off");

        layoutMachine2->addWidget(lblMachine2Off);

        spacerMachine2 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        layoutMachine2->addItem(spacerMachine2);


        layoutHistoriqueList->addWidget(frameMachine2);

        frameMachine3 = new QFrame(scrollAreaWidgetContents);
        frameMachine3->setObjectName("frameMachine3");
        layoutMachine3 = new QHBoxLayout(frameMachine3);
        layoutMachine3->setSpacing(15);
        layoutMachine3->setObjectName("layoutMachine3");
        lblMachine3Nom = new QLabel(frameMachine3);
        lblMachine3Nom->setObjectName("lblMachine3Nom");
        lblMachine3Nom->setMinimumSize(QSize(200, 0));

        layoutMachine3->addWidget(lblMachine3Nom);

        lblMachine3On = new QLabel(frameMachine3);
        lblMachine3On->setObjectName("lblMachine3On");

        layoutMachine3->addWidget(lblMachine3On);

        lblMachine3Off = new QLabel(frameMachine3);
        lblMachine3Off->setObjectName("lblMachine3Off");

        layoutMachine3->addWidget(lblMachine3Off);

        spacerMachine3 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        layoutMachine3->addItem(spacerMachine3);


        layoutHistoriqueList->addWidget(frameMachine3);

        frameMachine4 = new QFrame(scrollAreaWidgetContents);
        frameMachine4->setObjectName("frameMachine4");
        layoutMachine4 = new QHBoxLayout(frameMachine4);
        layoutMachine4->setSpacing(15);
        layoutMachine4->setObjectName("layoutMachine4");
        lblMachine4Nom = new QLabel(frameMachine4);
        lblMachine4Nom->setObjectName("lblMachine4Nom");
        lblMachine4Nom->setMinimumSize(QSize(200, 0));

        layoutMachine4->addWidget(lblMachine4Nom);

        lblMachine4On = new QLabel(frameMachine4);
        lblMachine4On->setObjectName("lblMachine4On");

        layoutMachine4->addWidget(lblMachine4On);

        lblMachine4Off = new QLabel(frameMachine4);
        lblMachine4Off->setObjectName("lblMachine4Off");

        layoutMachine4->addWidget(lblMachine4Off);

        spacerMachine4 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        layoutMachine4->addItem(spacerMachine4);


        layoutHistoriqueList->addWidget(frameMachine4);

        frameMachine5 = new QFrame(scrollAreaWidgetContents);
        frameMachine5->setObjectName("frameMachine5");
        layoutMachine5 = new QHBoxLayout(frameMachine5);
        layoutMachine5->setSpacing(15);
        layoutMachine5->setObjectName("layoutMachine5");
        lblMachine5Nom = new QLabel(frameMachine5);
        lblMachine5Nom->setObjectName("lblMachine5Nom");
        lblMachine5Nom->setMinimumSize(QSize(200, 0));

        layoutMachine5->addWidget(lblMachine5Nom);

        lblMachine5On = new QLabel(frameMachine5);
        lblMachine5On->setObjectName("lblMachine5On");

        layoutMachine5->addWidget(lblMachine5On);

        lblMachine5Off = new QLabel(frameMachine5);
        lblMachine5Off->setObjectName("lblMachine5Off");

        layoutMachine5->addWidget(lblMachine5Off);

        spacerMachine5 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        layoutMachine5->addItem(spacerMachine5);


        layoutHistoriqueList->addWidget(frameMachine5);

        verticalSpacerHistorique = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        layoutHistoriqueList->addItem(verticalSpacerHistorique);

        scrollHistoriqueOnOff->setWidget(scrollAreaWidgetContents);

        layoutParcMachines->addWidget(scrollHistoriqueOnOff);

        scrollListeMachines = new QScrollArea(tabParcMachines);
        scrollListeMachines->setObjectName("scrollListeMachines");
        scrollListeMachines->setWidgetResizable(true);
        scrollAreaMachinesContent = new QWidget();
        scrollAreaMachinesContent->setObjectName("scrollAreaMachinesContent");
        scrollAreaMachinesContent->setGeometry(QRect(0, 0, 1386, 747));
        layoutListeMachines = new QVBoxLayout(scrollAreaMachinesContent);
        layoutListeMachines->setSpacing(15);
        layoutListeMachines->setObjectName("layoutListeMachines");
        layoutListeMachines->setContentsMargins(5, 10, 5, 10);
        cardMachine1 = new QFrame(scrollAreaMachinesContent);
        cardMachine1->setObjectName("cardMachine1");
        layoutCardMachine1 = new QVBoxLayout(cardMachine1);
        layoutCardMachine1->setSpacing(12);
        layoutCardMachine1->setObjectName("layoutCardMachine1");
        layoutMachine1Header = new QHBoxLayout();
        layoutMachine1Header->setObjectName("layoutMachine1Header");
        lblCard1Nom = new QLabel(cardMachine1);
        lblCard1Nom->setObjectName("lblCard1Nom");

        layoutMachine1Header->addWidget(lblCard1Nom);

        spacerCard1Header = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        layoutMachine1Header->addItem(spacerCard1Header);

        lblCard1EtatMarche = new QLabel(cardMachine1);
        lblCard1EtatMarche->setObjectName("lblCard1EtatMarche");

        layoutMachine1Header->addWidget(lblCard1EtatMarche);


        layoutCardMachine1->addLayout(layoutMachine1Header);

        frameSeparator1 = new QFrame(cardMachine1);
        frameSeparator1->setObjectName("frameSeparator1");
        frameSeparator1->setMaximumSize(QSize(16777215, 1));

        layoutCardMachine1->addWidget(frameSeparator1);

        gridCard1Info = new QGridLayout();
        gridCard1Info->setObjectName("gridCard1Info");
        gridCard1Info->setHorizontalSpacing(20);
        gridCard1Info->setVerticalSpacing(10);
        lblCard1TypeLabel = new QLabel(cardMachine1);
        lblCard1TypeLabel->setObjectName("lblCard1TypeLabel");

        gridCard1Info->addWidget(lblCard1TypeLabel, 0, 0, 1, 1);

        lblCard1Type = new QLabel(cardMachine1);
        lblCard1Type->setObjectName("lblCard1Type");

        gridCard1Info->addWidget(lblCard1Type, 0, 1, 1, 1);

        lblCard1TempLabel = new QLabel(cardMachine1);
        lblCard1TempLabel->setObjectName("lblCard1TempLabel");

        gridCard1Info->addWidget(lblCard1TempLabel, 0, 2, 1, 1);

        lblCard1Temp = new QLabel(cardMachine1);
        lblCard1Temp->setObjectName("lblCard1Temp");

        gridCard1Info->addWidget(lblCard1Temp, 0, 3, 1, 1);

        lblCard1EtatFonctLabel = new QLabel(cardMachine1);
        lblCard1EtatFonctLabel->setObjectName("lblCard1EtatFonctLabel");

        gridCard1Info->addWidget(lblCard1EtatFonctLabel, 1, 0, 1, 1);

        lblCard1EtatFonct = new QLabel(cardMachine1);
        lblCard1EtatFonct->setObjectName("lblCard1EtatFonct");

        gridCard1Info->addWidget(lblCard1EtatFonct, 1, 1, 1, 1);

        lblCard1ChargeLabel = new QLabel(cardMachine1);
        lblCard1ChargeLabel->setObjectName("lblCard1ChargeLabel");

        gridCard1Info->addWidget(lblCard1ChargeLabel, 1, 2, 1, 1);

        lblCard1Charge = new QLabel(cardMachine1);
        lblCard1Charge->setObjectName("lblCard1Charge");

        gridCard1Info->addWidget(lblCard1Charge, 1, 3, 1, 1);

        lblCard1AlerteLabel = new QLabel(cardMachine1);
        lblCard1AlerteLabel->setObjectName("lblCard1AlerteLabel");

        gridCard1Info->addWidget(lblCard1AlerteLabel, 2, 0, 1, 1);

        lblCard1Alerte = new QLabel(cardMachine1);
        lblCard1Alerte->setObjectName("lblCard1Alerte");

        gridCard1Info->addWidget(lblCard1Alerte, 2, 1, 1, 1);

        lblCard1CriticiteLabel = new QLabel(cardMachine1);
        lblCard1CriticiteLabel->setObjectName("lblCard1CriticiteLabel");

        gridCard1Info->addWidget(lblCard1CriticiteLabel, 2, 2, 1, 1);

        lblCard1Criticite = new QLabel(cardMachine1);
        lblCard1Criticite->setObjectName("lblCard1Criticite");

        gridCard1Info->addWidget(lblCard1Criticite, 2, 3, 1, 1);

        lblCard1MaintenanceLabel = new QLabel(cardMachine1);
        lblCard1MaintenanceLabel->setObjectName("lblCard1MaintenanceLabel");

        gridCard1Info->addWidget(lblCard1MaintenanceLabel, 3, 0, 1, 1);

        lblCard1Maintenance = new QLabel(cardMachine1);
        lblCard1Maintenance->setObjectName("lblCard1Maintenance");

        gridCard1Info->addWidget(lblCard1Maintenance, 3, 1, 1, 3);


        layoutCardMachine1->addLayout(gridCard1Info);


        layoutListeMachines->addWidget(cardMachine1);

        cardMachine2 = new QFrame(scrollAreaMachinesContent);
        cardMachine2->setObjectName("cardMachine2");
        layoutCardMachine2 = new QVBoxLayout(cardMachine2);
        layoutCardMachine2->setSpacing(12);
        layoutCardMachine2->setObjectName("layoutCardMachine2");
        layoutMachine2Header = new QHBoxLayout();
        layoutMachine2Header->setObjectName("layoutMachine2Header");
        lblCard2Nom = new QLabel(cardMachine2);
        lblCard2Nom->setObjectName("lblCard2Nom");

        layoutMachine2Header->addWidget(lblCard2Nom);

        spacerCard2Header = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        layoutMachine2Header->addItem(spacerCard2Header);

        lblCard2EtatMarche = new QLabel(cardMachine2);
        lblCard2EtatMarche->setObjectName("lblCard2EtatMarche");

        layoutMachine2Header->addWidget(lblCard2EtatMarche);


        layoutCardMachine2->addLayout(layoutMachine2Header);

        frameSeparator2 = new QFrame(cardMachine2);
        frameSeparator2->setObjectName("frameSeparator2");
        frameSeparator2->setMaximumSize(QSize(16777215, 1));

        layoutCardMachine2->addWidget(frameSeparator2);

        gridCard2Info = new QGridLayout();
        gridCard2Info->setObjectName("gridCard2Info");
        gridCard2Info->setHorizontalSpacing(20);
        gridCard2Info->setVerticalSpacing(10);
        lblCard2TypeLabel = new QLabel(cardMachine2);
        lblCard2TypeLabel->setObjectName("lblCard2TypeLabel");

        gridCard2Info->addWidget(lblCard2TypeLabel, 0, 0, 1, 1);

        lblCard2Type = new QLabel(cardMachine2);
        lblCard2Type->setObjectName("lblCard2Type");

        gridCard2Info->addWidget(lblCard2Type, 0, 1, 1, 1);

        lblCard2TempLabel = new QLabel(cardMachine2);
        lblCard2TempLabel->setObjectName("lblCard2TempLabel");

        gridCard2Info->addWidget(lblCard2TempLabel, 0, 2, 1, 1);

        lblCard2Temp = new QLabel(cardMachine2);
        lblCard2Temp->setObjectName("lblCard2Temp");

        gridCard2Info->addWidget(lblCard2Temp, 0, 3, 1, 1);

        lblCard2EtatFonctLabel = new QLabel(cardMachine2);
        lblCard2EtatFonctLabel->setObjectName("lblCard2EtatFonctLabel");

        gridCard2Info->addWidget(lblCard2EtatFonctLabel, 1, 0, 1, 1);

        lblCard2EtatFonct = new QLabel(cardMachine2);
        lblCard2EtatFonct->setObjectName("lblCard2EtatFonct");

        gridCard2Info->addWidget(lblCard2EtatFonct, 1, 1, 1, 1);

        lblCard2ChargeLabel = new QLabel(cardMachine2);
        lblCard2ChargeLabel->setObjectName("lblCard2ChargeLabel");

        gridCard2Info->addWidget(lblCard2ChargeLabel, 1, 2, 1, 1);

        lblCard2Charge = new QLabel(cardMachine2);
        lblCard2Charge->setObjectName("lblCard2Charge");

        gridCard2Info->addWidget(lblCard2Charge, 1, 3, 1, 1);

        lblCard2AlerteLabel = new QLabel(cardMachine2);
        lblCard2AlerteLabel->setObjectName("lblCard2AlerteLabel");

        gridCard2Info->addWidget(lblCard2AlerteLabel, 2, 0, 1, 1);

        lblCard2Alerte = new QLabel(cardMachine2);
        lblCard2Alerte->setObjectName("lblCard2Alerte");

        gridCard2Info->addWidget(lblCard2Alerte, 2, 1, 1, 1);

        lblCard2CriticiteLabel = new QLabel(cardMachine2);
        lblCard2CriticiteLabel->setObjectName("lblCard2CriticiteLabel");

        gridCard2Info->addWidget(lblCard2CriticiteLabel, 2, 2, 1, 1);

        lblCard2Criticite = new QLabel(cardMachine2);
        lblCard2Criticite->setObjectName("lblCard2Criticite");

        gridCard2Info->addWidget(lblCard2Criticite, 2, 3, 1, 1);

        lblCard2MaintenanceLabel = new QLabel(cardMachine2);
        lblCard2MaintenanceLabel->setObjectName("lblCard2MaintenanceLabel");

        gridCard2Info->addWidget(lblCard2MaintenanceLabel, 3, 0, 1, 1);

        lblCard2Maintenance = new QLabel(cardMachine2);
        lblCard2Maintenance->setObjectName("lblCard2Maintenance");

        gridCard2Info->addWidget(lblCard2Maintenance, 3, 1, 1, 3);


        layoutCardMachine2->addLayout(gridCard2Info);


        layoutListeMachines->addWidget(cardMachine2);

        cardMachine3 = new QFrame(scrollAreaMachinesContent);
        cardMachine3->setObjectName("cardMachine3");
        layoutCardMachine3 = new QVBoxLayout(cardMachine3);
        layoutCardMachine3->setSpacing(12);
        layoutCardMachine3->setObjectName("layoutCardMachine3");
        layoutMachine3Header = new QHBoxLayout();
        layoutMachine3Header->setObjectName("layoutMachine3Header");
        lblCard3Nom = new QLabel(cardMachine3);
        lblCard3Nom->setObjectName("lblCard3Nom");

        layoutMachine3Header->addWidget(lblCard3Nom);

        spacerCard3Header = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        layoutMachine3Header->addItem(spacerCard3Header);

        lblCard3EtatMarche = new QLabel(cardMachine3);
        lblCard3EtatMarche->setObjectName("lblCard3EtatMarche");

        layoutMachine3Header->addWidget(lblCard3EtatMarche);


        layoutCardMachine3->addLayout(layoutMachine3Header);

        frameSeparator3 = new QFrame(cardMachine3);
        frameSeparator3->setObjectName("frameSeparator3");
        frameSeparator3->setMaximumSize(QSize(16777215, 1));

        layoutCardMachine3->addWidget(frameSeparator3);

        gridCard3Info = new QGridLayout();
        gridCard3Info->setObjectName("gridCard3Info");
        gridCard3Info->setHorizontalSpacing(20);
        gridCard3Info->setVerticalSpacing(10);
        lblCard3TypeLabel = new QLabel(cardMachine3);
        lblCard3TypeLabel->setObjectName("lblCard3TypeLabel");

        gridCard3Info->addWidget(lblCard3TypeLabel, 0, 0, 1, 1);

        lblCard3Type = new QLabel(cardMachine3);
        lblCard3Type->setObjectName("lblCard3Type");

        gridCard3Info->addWidget(lblCard3Type, 0, 1, 1, 1);

        lblCard3TempLabel = new QLabel(cardMachine3);
        lblCard3TempLabel->setObjectName("lblCard3TempLabel");

        gridCard3Info->addWidget(lblCard3TempLabel, 0, 2, 1, 1);

        lblCard3Temp = new QLabel(cardMachine3);
        lblCard3Temp->setObjectName("lblCard3Temp");

        gridCard3Info->addWidget(lblCard3Temp, 0, 3, 1, 1);

        lblCard3EtatFonctLabel = new QLabel(cardMachine3);
        lblCard3EtatFonctLabel->setObjectName("lblCard3EtatFonctLabel");

        gridCard3Info->addWidget(lblCard3EtatFonctLabel, 1, 0, 1, 1);

        lblCard3EtatFonct = new QLabel(cardMachine3);
        lblCard3EtatFonct->setObjectName("lblCard3EtatFonct");

        gridCard3Info->addWidget(lblCard3EtatFonct, 1, 1, 1, 1);

        lblCard3ChargeLabel = new QLabel(cardMachine3);
        lblCard3ChargeLabel->setObjectName("lblCard3ChargeLabel");

        gridCard3Info->addWidget(lblCard3ChargeLabel, 1, 2, 1, 1);

        lblCard3Charge = new QLabel(cardMachine3);
        lblCard3Charge->setObjectName("lblCard3Charge");

        gridCard3Info->addWidget(lblCard3Charge, 1, 3, 1, 1);

        lblCard3AlerteLabel = new QLabel(cardMachine3);
        lblCard3AlerteLabel->setObjectName("lblCard3AlerteLabel");

        gridCard3Info->addWidget(lblCard3AlerteLabel, 2, 0, 1, 1);

        lblCard3Alerte = new QLabel(cardMachine3);
        lblCard3Alerte->setObjectName("lblCard3Alerte");

        gridCard3Info->addWidget(lblCard3Alerte, 2, 1, 1, 1);

        lblCard3CriticiteLabel = new QLabel(cardMachine3);
        lblCard3CriticiteLabel->setObjectName("lblCard3CriticiteLabel");

        gridCard3Info->addWidget(lblCard3CriticiteLabel, 2, 2, 1, 1);

        lblCard3Criticite = new QLabel(cardMachine3);
        lblCard3Criticite->setObjectName("lblCard3Criticite");

        gridCard3Info->addWidget(lblCard3Criticite, 2, 3, 1, 1);

        lblCard3MaintenanceLabel = new QLabel(cardMachine3);
        lblCard3MaintenanceLabel->setObjectName("lblCard3MaintenanceLabel");

        gridCard3Info->addWidget(lblCard3MaintenanceLabel, 3, 0, 1, 1);

        lblCard3Maintenance = new QLabel(cardMachine3);
        lblCard3Maintenance->setObjectName("lblCard3Maintenance");

        gridCard3Info->addWidget(lblCard3Maintenance, 3, 1, 1, 3);


        layoutCardMachine3->addLayout(gridCard3Info);


        layoutListeMachines->addWidget(cardMachine3);

        verticalSpacerListeMachines = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        layoutListeMachines->addItem(verticalSpacerListeMachines);

        scrollListeMachines->setWidget(scrollAreaMachinesContent);

        layoutParcMachines->addWidget(scrollListeMachines);

        tableMachines = new QTableView(tabParcMachines);
        tableMachines->setObjectName("tableMachines");
        tableMachines->setVisible(false);
        tableMachines->setAlternatingRowColors(true);
        tableMachines->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);

        layoutParcMachines->addWidget(tableMachines);

        tabWidgetMachineManagement->addTab(tabParcMachines, QString());
        tabAjouterModifier = new QWidget();
        tabAjouterModifier->setObjectName("tabAjouterModifier");
        layoutAjouterModifier = new QHBoxLayout(tabAjouterModifier);
        layoutAjouterModifier->setObjectName("layoutAjouterModifier");
        layoutAjouterModifier->setContentsMargins(20, 15, 20, 15);
        formSpacerLeft = new QSpacerItem(100, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        layoutAjouterModifier->addItem(formSpacerLeft);

        groupFormMachine = new QGroupBox(tabAjouterModifier);
        groupFormMachine->setObjectName("groupFormMachine");
        groupFormMachine->setMinimumSize(QSize(500, 0));
        formLayoutMachine = new QFormLayout(groupFormMachine);
        formLayoutMachine->setObjectName("formLayoutMachine");
        formLayoutMachine->setHorizontalSpacing(15);
        formLayoutMachine->setVerticalSpacing(12);
        formLayoutMachine->setContentsMargins(20, 20, 20, 20);
        lblNomMachine = new QLabel(groupFormMachine);
        lblNomMachine->setObjectName("lblNomMachine");

        formLayoutMachine->setWidget(0, QFormLayout::LabelRole, lblNomMachine);

        nom_machine = new QLineEdit(groupFormMachine);
        nom_machine->setObjectName("nom_machine");

        formLayoutMachine->setWidget(0, QFormLayout::FieldRole, nom_machine);

        lblTypeMachine = new QLabel(groupFormMachine);
        lblTypeMachine->setObjectName("lblTypeMachine");

        formLayoutMachine->setWidget(1, QFormLayout::LabelRole, lblTypeMachine);

        type_machine = new QComboBox(groupFormMachine);
        type_machine->addItem(QString());
        type_machine->addItem(QString());
        type_machine->addItem(QString());
        type_machine->addItem(QString());
        type_machine->setObjectName("type_machine");

        formLayoutMachine->setWidget(1, QFormLayout::FieldRole, type_machine);

        lblEtatMarche = new QLabel(groupFormMachine);
        lblEtatMarche->setObjectName("lblEtatMarche");

        formLayoutMachine->setWidget(2, QFormLayout::LabelRole, lblEtatMarche);

        etat_marche = new QComboBox(groupFormMachine);
        etat_marche->addItem(QString());
        etat_marche->addItem(QString());
        etat_marche->addItem(QString());
        etat_marche->setObjectName("etat_marche");

        formLayoutMachine->setWidget(2, QFormLayout::FieldRole, etat_marche);

        lblTemperature = new QLabel(groupFormMachine);
        lblTemperature->setObjectName("lblTemperature");

        formLayoutMachine->setWidget(3, QFormLayout::LabelRole, lblTemperature);

        temperature_actuelle = new QDoubleSpinBox(groupFormMachine);
        temperature_actuelle->setObjectName("temperature_actuelle");
        temperature_actuelle->setMaximum(200.000000000000000);
        temperature_actuelle->setSingleStep(0.500000000000000);

        formLayoutMachine->setWidget(3, QFormLayout::FieldRole, temperature_actuelle);

        lblNiveauCharge = new QLabel(groupFormMachine);
        lblNiveauCharge->setObjectName("lblNiveauCharge");

        formLayoutMachine->setWidget(4, QFormLayout::LabelRole, lblNiveauCharge);

        niveau_charge = new QSpinBox(groupFormMachine);
        niveau_charge->setObjectName("niveau_charge");
        niveau_charge->setMaximum(100);

        formLayoutMachine->setWidget(4, QFormLayout::FieldRole, niveau_charge);

        lblEtatFonctionnement = new QLabel(groupFormMachine);
        lblEtatFonctionnement->setObjectName("lblEtatFonctionnement");

        formLayoutMachine->setWidget(5, QFormLayout::LabelRole, lblEtatFonctionnement);

        etat_fonctionnement = new QComboBox(groupFormMachine);
        etat_fonctionnement->addItem(QString());
        etat_fonctionnement->addItem(QString());
        etat_fonctionnement->addItem(QString());
        etat_fonctionnement->setObjectName("etat_fonctionnement");

        formLayoutMachine->setWidget(5, QFormLayout::FieldRole, etat_fonctionnement);

        lblTypeAlerte = new QLabel(groupFormMachine);
        lblTypeAlerte->setObjectName("lblTypeAlerte");

        formLayoutMachine->setWidget(6, QFormLayout::LabelRole, lblTypeAlerte);

        type_alerte = new QComboBox(groupFormMachine);
        type_alerte->addItem(QString());
        type_alerte->addItem(QString());
        type_alerte->addItem(QString());
        type_alerte->addItem(QString());
        type_alerte->setObjectName("type_alerte");

        formLayoutMachine->setWidget(6, QFormLayout::FieldRole, type_alerte);

        lblNiveauCriticite = new QLabel(groupFormMachine);
        lblNiveauCriticite->setObjectName("lblNiveauCriticite");

        formLayoutMachine->setWidget(7, QFormLayout::LabelRole, lblNiveauCriticite);

        niveau_criticite = new QComboBox(groupFormMachine);
        niveau_criticite->addItem(QString());
        niveau_criticite->addItem(QString());
        niveau_criticite->addItem(QString());
        niveau_criticite->addItem(QString());
        niveau_criticite->setObjectName("niveau_criticite");

        formLayoutMachine->setWidget(7, QFormLayout::FieldRole, niveau_criticite);

        lblDateMaintenance = new QLabel(groupFormMachine);
        lblDateMaintenance->setObjectName("lblDateMaintenance");

        formLayoutMachine->setWidget(8, QFormLayout::LabelRole, lblDateMaintenance);

        date_derniere_maintenance = new QDateEdit(groupFormMachine);
        date_derniere_maintenance->setObjectName("date_derniere_maintenance");
        date_derniere_maintenance->setCalendarPopup(true);

        formLayoutMachine->setWidget(8, QFormLayout::FieldRole, date_derniere_maintenance);

        layoutBtnsForm = new QHBoxLayout();
        layoutBtnsForm->setObjectName("layoutBtnsForm");
        btnEnregistrer = new QPushButton(groupFormMachine);
        btnEnregistrer->setObjectName("btnEnregistrer");

        layoutBtnsForm->addWidget(btnEnregistrer);

        btnAnnuler = new QPushButton(groupFormMachine);
        btnAnnuler->setObjectName("btnAnnuler");

        layoutBtnsForm->addWidget(btnAnnuler);


        formLayoutMachine->setLayout(9, QFormLayout::FieldRole, layoutBtnsForm);


        layoutAjouterModifier->addWidget(groupFormMachine);

        formSpacerRight = new QSpacerItem(100, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        layoutAjouterModifier->addItem(formSpacerRight);

        tabWidgetMachineManagement->addTab(tabAjouterModifier, QString());
        tabDetails = new QWidget();
        tabDetails->setObjectName("tabDetails");
        layoutDetails = new QHBoxLayout(tabDetails);
        layoutDetails->setSpacing(15);
        layoutDetails->setObjectName("layoutDetails");
        layoutDetails->setContentsMargins(20, 15, 20, 15);
        groupInfosMachine = new QGroupBox(tabDetails);
        groupInfosMachine->setObjectName("groupInfosMachine");
        formInfos = new QFormLayout(groupInfosMachine);
        formInfos->setObjectName("formInfos");
        formInfos->setVerticalSpacing(12);
        formInfos->setContentsMargins(15, 20, 15, 15);
        lblInfoNom = new QLabel(groupInfosMachine);
        lblInfoNom->setObjectName("lblInfoNom");

        formInfos->setWidget(0, QFormLayout::LabelRole, lblInfoNom);

        lblValeurNom = new QLabel(groupInfosMachine);
        lblValeurNom->setObjectName("lblValeurNom");

        formInfos->setWidget(0, QFormLayout::FieldRole, lblValeurNom);

        lblInfoType = new QLabel(groupInfosMachine);
        lblInfoType->setObjectName("lblInfoType");

        formInfos->setWidget(1, QFormLayout::LabelRole, lblInfoType);

        lblValeurType = new QLabel(groupInfosMachine);
        lblValeurType->setObjectName("lblValeurType");

        formInfos->setWidget(1, QFormLayout::FieldRole, lblValeurType);

        lblInfoEtat = new QLabel(groupInfosMachine);
        lblInfoEtat->setObjectName("lblInfoEtat");

        formInfos->setWidget(2, QFormLayout::LabelRole, lblInfoEtat);

        lblValeurEtat = new QLabel(groupInfosMachine);
        lblValeurEtat->setObjectName("lblValeurEtat");

        formInfos->setWidget(2, QFormLayout::FieldRole, lblValeurEtat);

        lblInfoTemp = new QLabel(groupInfosMachine);
        lblInfoTemp->setObjectName("lblInfoTemp");

        formInfos->setWidget(3, QFormLayout::LabelRole, lblInfoTemp);

        lblValeurTemp = new QLabel(groupInfosMachine);
        lblValeurTemp->setObjectName("lblValeurTemp");

        formInfos->setWidget(3, QFormLayout::FieldRole, lblValeurTemp);

        lblInfoScore = new QLabel(groupInfosMachine);
        lblInfoScore->setObjectName("lblInfoScore");

        formInfos->setWidget(4, QFormLayout::LabelRole, lblInfoScore);

        progressScoreSante = new QProgressBar(groupInfosMachine);
        progressScoreSante->setObjectName("progressScoreSante");
        progressScoreSante->setValue(0);

        formInfos->setWidget(4, QFormLayout::FieldRole, progressScoreSante);


        layoutDetails->addWidget(groupInfosMachine);

        groupActions = new QGroupBox(tabDetails);
        groupActions->setObjectName("groupActions");
        layoutActions = new QVBoxLayout(groupActions);
        layoutActions->setSpacing(15);
        layoutActions->setObjectName("layoutActions");
        layoutActions->setContentsMargins(20, 25, 20, 20);
        btnReinitialiser = new QPushButton(groupActions);
        btnReinitialiser->setObjectName("btnReinitialiser");
        btnReinitialiser->setMinimumSize(QSize(118, 40));

        layoutActions->addWidget(btnReinitialiser);

        btnMiseVeille = new QPushButton(groupActions);
        btnMiseVeille->setObjectName("btnMiseVeille");
        btnMiseVeille->setMinimumSize(QSize(118, 40));

        layoutActions->addWidget(btnMiseVeille);

        btnArretForce = new QPushButton(groupActions);
        btnArretForce->setObjectName("btnArretForce");
        btnArretForce->setMinimumSize(QSize(118, 40));

        layoutActions->addWidget(btnArretForce);

        actionsSpacer = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        layoutActions->addItem(actionsSpacer);


        layoutDetails->addWidget(groupActions);

        tabWidgetMachineManagement->addTab(tabDetails, QString());
        tabStatistiques = new QWidget();
        tabStatistiques->setObjectName("tabStatistiques");
        layoutStatistiques = new QVBoxLayout(tabStatistiques);
        layoutStatistiques->setSpacing(10);
        layoutStatistiques->setObjectName("layoutStatistiques");
        layoutStatistiques->setContentsMargins(15, 10, 15, 10);
        groupOptionsStats = new QGroupBox(tabStatistiques);
        groupOptionsStats->setObjectName("groupOptionsStats");
        layoutOptionsStats = new QHBoxLayout(groupOptionsStats);
        layoutOptionsStats->setSpacing(8);
        layoutOptionsStats->setObjectName("layoutOptionsStats");
        layoutOptionsStats->setContentsMargins(10, 5, 10, 5);
        lblTypeStats = new QLabel(groupOptionsStats);
        lblTypeStats->setObjectName("lblTypeStats");

        layoutOptionsStats->addWidget(lblTypeStats);

        type_statistique = new QComboBox(groupOptionsStats);
        type_statistique->addItem(QString());
        type_statistique->addItem(QString());
        type_statistique->addItem(QString());
        type_statistique->setObjectName("type_statistique");

        layoutOptionsStats->addWidget(type_statistique);

        lblTypeGraphique = new QLabel(groupOptionsStats);
        lblTypeGraphique->setObjectName("lblTypeGraphique");

        layoutOptionsStats->addWidget(lblTypeGraphique);

        type_graphique = new QComboBox(groupOptionsStats);
        type_graphique->addItem(QString());
        type_graphique->addItem(QString());
        type_graphique->addItem(QString());
        type_graphique->setObjectName("type_graphique");

        layoutOptionsStats->addWidget(type_graphique);

        btnGenererStats = new QPushButton(groupOptionsStats);
        btnGenererStats->setObjectName("btnGenererStats");

        layoutOptionsStats->addWidget(btnGenererStats);

        statsSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        layoutOptionsStats->addItem(statsSpacer);


        layoutStatistiques->addWidget(groupOptionsStats);

        zoneGraphique = new QWidget(tabStatistiques);
        zoneGraphique->setObjectName("zoneGraphique");
        zoneGraphique->setMinimumSize(QSize(0, 300));

        layoutStatistiques->addWidget(zoneGraphique);

        groupMaintenancePredictive = new QGroupBox(tabStatistiques);
        groupMaintenancePredictive->setObjectName("groupMaintenancePredictive");
        layoutMaintenanceTable = new QVBoxLayout(groupMaintenancePredictive);
        layoutMaintenanceTable->setObjectName("layoutMaintenanceTable");
        layoutMaintenanceTable->setContentsMargins(-1, 15, -1, -1);
        tableMaintenance = new QTableView(groupMaintenancePredictive);
        tableMaintenance->setObjectName("tableMaintenance");
        tableMaintenance->setAlternatingRowColors(true);
        tableMaintenance->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);

        layoutMaintenanceTable->addWidget(tableMaintenance);


        layoutStatistiques->addWidget(groupMaintenancePredictive);

        layoutBtnsMaintenance = new QHBoxLayout();
        layoutBtnsMaintenance->setObjectName("layoutBtnsMaintenance");
        maintenanceSpacerLeft = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        layoutBtnsMaintenance->addItem(maintenanceSpacerLeft);

        btnEnregistrerMaintenance = new QPushButton(tabStatistiques);
        btnEnregistrerMaintenance->setObjectName("btnEnregistrerMaintenance");
        btnEnregistrerMaintenance->setMinimumSize(QSize(118, 35));

        layoutBtnsMaintenance->addWidget(btnEnregistrerMaintenance);

        btnPlanifierMaintenance = new QPushButton(tabStatistiques);
        btnPlanifierMaintenance->setObjectName("btnPlanifierMaintenance");
        btnPlanifierMaintenance->setMinimumSize(QSize(118, 35));

        layoutBtnsMaintenance->addWidget(btnPlanifierMaintenance);

        maintenanceSpacerRight = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        layoutBtnsMaintenance->addItem(maintenanceSpacerRight);


        layoutStatistiques->addLayout(layoutBtnsMaintenance);

        tabWidgetMachineManagement->addTab(tabStatistiques, QString());
        tabHistorique = new QWidget();
        tabHistorique->setObjectName("tabHistorique");
        layoutHistorique = new QVBoxLayout(tabHistorique);
        layoutHistorique->setSpacing(10);
        layoutHistorique->setObjectName("layoutHistorique");
        layoutHistorique->setContentsMargins(15, 10, 15, 10);
        groupHistoriqueOnOff = new QGroupBox(tabHistorique);
        groupHistoriqueOnOff->setObjectName("groupHistoriqueOnOff");
        layoutHistoriqueTable = new QVBoxLayout(groupHistoriqueOnOff);
        layoutHistoriqueTable->setObjectName("layoutHistoriqueTable");
        layoutHistoriqueTable->setContentsMargins(-1, 15, -1, -1);
        tableHistorique = new QTableView(groupHistoriqueOnOff);
        tableHistorique->setObjectName("tableHistorique");
        tableHistorique->setAlternatingRowColors(true);
        tableHistorique->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);

        layoutHistoriqueTable->addWidget(tableHistorique);


        layoutHistorique->addWidget(groupHistoriqueOnOff);

        frameTempsTotal = new QFrame(tabHistorique);
        frameTempsTotal->setObjectName("frameTempsTotal");
        layoutTempsTotal = new QHBoxLayout(frameTempsTotal);
        layoutTempsTotal->setObjectName("layoutTempsTotal");
        lblTempsTotalOn = new QLabel(frameTempsTotal);
        lblTempsTotalOn->setObjectName("lblTempsTotalOn");

        layoutTempsTotal->addWidget(lblTempsTotalOn);

        lblTempsTotalOff = new QLabel(frameTempsTotal);
        lblTempsTotalOff->setObjectName("lblTempsTotalOff");

        layoutTempsTotal->addWidget(lblTempsTotalOff);

        tempsSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        layoutTempsTotal->addItem(tempsSpacer);


        layoutHistorique->addWidget(frameTempsTotal);

        tabWidgetMachineManagement->addTab(tabHistorique, QString());
        tabDecisionIA = new QWidget();
        tabDecisionIA->setObjectName("tabDecisionIA");
        layoutDecisionIA = new QVBoxLayout(tabDecisionIA);
        layoutDecisionIA->setSpacing(15);
        layoutDecisionIA->setObjectName("layoutDecisionIA");
        layoutDecisionIA->setContentsMargins(50, 30, 50, 30);
        groupRecommandationIA = new QGroupBox(tabDecisionIA);
        groupRecommandationIA->setObjectName("groupRecommandationIA");
        layoutRecommandation = new QVBoxLayout(groupRecommandationIA);
        layoutRecommandation->setSpacing(15);
        layoutRecommandation->setObjectName("layoutRecommandation");
        layoutRecommandation->setContentsMargins(20, 20, 20, 20);
        lblRecommandationIA = new QLabel(groupRecommandationIA);
        lblRecommandationIA->setObjectName("lblRecommandationIA");
        lblRecommandationIA->setWordWrap(true);

        layoutRecommandation->addWidget(lblRecommandationIA);

        layoutConfiance = new QHBoxLayout();
        layoutConfiance->setObjectName("layoutConfiance");
        lblConfianceLabel = new QLabel(groupRecommandationIA);
        lblConfianceLabel->setObjectName("lblConfianceLabel");

        layoutConfiance->addWidget(lblConfianceLabel);

        progressConfianceIA = new QProgressBar(groupRecommandationIA);
        progressConfianceIA->setObjectName("progressConfianceIA");
        progressConfianceIA->setValue(0);
        progressConfianceIA->setTextVisible(true);

        layoutConfiance->addWidget(progressConfianceIA);


        layoutRecommandation->addLayout(layoutConfiance);

        layoutBtnsIA = new QHBoxLayout();
        layoutBtnsIA->setObjectName("layoutBtnsIA");
        iaSpacerLeft = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        layoutBtnsIA->addItem(iaSpacerLeft);

        btnAppliquerDecision = new QPushButton(groupRecommandationIA);
        btnAppliquerDecision->setObjectName("btnAppliquerDecision");
        btnAppliquerDecision->setMinimumSize(QSize(118, 40));

        layoutBtnsIA->addWidget(btnAppliquerDecision);

        btnIgnorerDecision = new QPushButton(groupRecommandationIA);
        btnIgnorerDecision->setObjectName("btnIgnorerDecision");
        btnIgnorerDecision->setMinimumSize(QSize(118, 40));

        layoutBtnsIA->addWidget(btnIgnorerDecision);

        iaSpacerRight = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        layoutBtnsIA->addItem(iaSpacerRight);


        layoutRecommandation->addLayout(layoutBtnsIA);


        layoutDecisionIA->addWidget(groupRecommandationIA);

        iaVerticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        layoutDecisionIA->addItem(iaVerticalSpacer);

        tabWidgetMachineManagement->addTab(tabDecisionIA, QString());

        mainLayout->addWidget(tabWidgetMachineManagement);

        machine->setCentralWidget(centralwidget);

        retranslateUi(machine);

        tabWidgetMachineManagement->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(machine);
    } // setupUi

    void retranslateUi(QMainWindow *machine)
    {
        machine->setWindowTitle(QCoreApplication::translate("machine", "Gestion des Machines & Alertes", nullptr));
        btnBackToMenu->setText(QCoreApplication::translate("machine", "\342\206\220 Retour", nullptr));
        lblLogo->setText(QString());
        lblAppTitle->setText(QCoreApplication::translate("machine", "GESTION DES MACHINES", nullptr));
        lblAppSubtitle->setText(QCoreApplication::translate("machine", "SURVEILLANCE & ALERTES", nullptr));
        btnGestionStock->setStyleSheet(QCoreApplication::translate("machine", "QPushButton { background-color: transparent; color: #C9A227; font-weight: 600; border: none; font-size: 12px; font-family: 'Segoe UI', Arial, sans-serif; } QPushButton:hover { text-decoration: underline; } QPushButton:pressed { opacity: 0.7; }", nullptr));
        btnGestionStock->setText(QCoreApplication::translate("machine", "\360\237\223\246 Gestion De Stock", nullptr));
        btnGestionProduction->setStyleSheet(QCoreApplication::translate("machine", "QPushButton { background-color: transparent; color: #C9A227; font-weight: 600; border: none; font-size: 12px; font-family: 'Segoe UI', Arial, sans-serif; } QPushButton:hover { text-decoration: underline; } QPushButton:pressed { opacity: 0.7; }", nullptr));
        btnGestionProduction->setText(QCoreApplication::translate("machine", "\360\237\217\255 Gestion De Production", nullptr));
        lblDateTime->setText(QCoreApplication::translate("machine", "\360\237\223\205 04 F\303\251vrier 2026 \342\200\242 14:30:45", nullptr));
        lblOnlineStatus->setText(QCoreApplication::translate("machine", "\342\227\217 EN LIGNE", nullptr));
        lblMachinesNormales->setText(QCoreApplication::translate("machine", "<html><head/><body><p style=\"margin:0; padding:0;\"><span style=\" font-size:14pt; font-weight:700; color:#2E7D32;\">\342\234\223</span><span style=\" font-size:11pt;\"> Machines normales</span></p><p style=\"margin:2px 0 0 0; padding:0;\"><span style=\" font-size:20pt; font-weight:700; color:#1B5E20;\">0</span></p></body></html>", nullptr));
        lblMachinesAlerte->setText(QCoreApplication::translate("machine", "<html><head/><body><p style=\"margin:0; padding:0;\"><span style=\" font-size:14pt; font-weight:700; color:#F9A825;\">\342\232\240</span><span style=\" font-size:11pt;\"> Machines en alerte</span></p><p style=\"margin:2px 0 0 0; padding:0;\"><span style=\" font-size:20pt; font-weight:700; color:#F57F17;\">0</span></p></body></html>", nullptr));
        lblMachinesPanne->setText(QCoreApplication::translate("machine", "<html><head/><body><p><span style=\" font-size:16pt; font-weight:700; color:#D32F2F;\">\342\234\227</span><span style=\" font-size:12pt;\"> Machines en panne</span></p><p><span style=\" font-size:24pt; font-weight:700; color:#B71C1C;\">0</span></p></body></html>", nullptr));
        groupRecherche->setTitle(QCoreApplication::translate("machine", "Recherche et tri", nullptr));
        lblRechercheRapide->setStyleSheet(QCoreApplication::translate("machine", "font-weight: bold; color: #1B4D3E; font-size: 12px;", nullptr));
        lblRechercheRapide->setText(QCoreApplication::translate("machine", "\360\237\224\215 Recherche rapide:", nullptr));
        recherche_nom_machine->setPlaceholderText(QCoreApplication::translate("machine", "Rechercher par nom...", nullptr));
        btnRechercher->setText(QCoreApplication::translate("machine", "\360\237\224\215 Rechercher", nullptr));
        btnHistoriqueToggle->setText(QCoreApplication::translate("machine", "\360\237\223\234 Historique ON/OFF", nullptr));
        separateur->setStyleSheet(QCoreApplication::translate("machine", "background-color: #d0d0d0;", nullptr));
        lblFiltresAvances->setStyleSheet(QCoreApplication::translate("machine", "font-weight: bold; color: #1B4D3E; font-size: 12px;", nullptr));
        lblFiltresAvances->setText(QCoreApplication::translate("machine", "\342\232\231\357\270\217 Filtres avanc\303\251s:", nullptr));
        filtre_type_machine->setItemText(0, QCoreApplication::translate("machine", "-- Type machine --", nullptr));
        filtre_type_machine->setItemText(1, QCoreApplication::translate("machine", "Presse", nullptr));
        filtre_type_machine->setItemText(2, QCoreApplication::translate("machine", "Broyeur", nullptr));
        filtre_type_machine->setItemText(3, QCoreApplication::translate("machine", "Malaxeur", nullptr));
        filtre_type_machine->setItemText(4, QCoreApplication::translate("machine", "Chauffeur", nullptr));

        filtre_etat_marche->setItemText(0, QCoreApplication::translate("machine", "-- \303\211tat marche --", nullptr));
        filtre_etat_marche->setItemText(1, QCoreApplication::translate("machine", "ON", nullptr));
        filtre_etat_marche->setItemText(2, QCoreApplication::translate("machine", "OFF", nullptr));
        filtre_etat_marche->setItemText(3, QCoreApplication::translate("machine", "VEILLE", nullptr));

        filtre_type_alerte->setItemText(0, QCoreApplication::translate("machine", "-- Type alerte --", nullptr));
        filtre_type_alerte->setItemText(1, QCoreApplication::translate("machine", "Aucune", nullptr));
        filtre_type_alerte->setItemText(2, QCoreApplication::translate("machine", "Temp\303\251rature", nullptr));
        filtre_type_alerte->setItemText(3, QCoreApplication::translate("machine", "Surcharge", nullptr));
        filtre_type_alerte->setItemText(4, QCoreApplication::translate("machine", "Panne", nullptr));

        filtre_alerte_criticite->setItemText(0, QCoreApplication::translate("machine", "-- Criticit\303\251 --", nullptr));
        filtre_alerte_criticite->setItemText(1, QCoreApplication::translate("machine", "Faible", nullptr));
        filtre_alerte_criticite->setItemText(2, QCoreApplication::translate("machine", "Moyen", nullptr));
        filtre_alerte_criticite->setItemText(3, QCoreApplication::translate("machine", "\303\211lev\303\251", nullptr));
        filtre_alerte_criticite->setItemText(4, QCoreApplication::translate("machine", "Critique", nullptr));

        lblPeriodeDu->setText(QCoreApplication::translate("machine", "P\303\251riode Du:", nullptr));
        lblPeriodeAu->setText(QCoreApplication::translate("machine", "Au:", nullptr));
        tri_par->setItemText(0, QCoreApplication::translate("machine", "-- Trier par --", nullptr));
        tri_par->setItemText(1, QCoreApplication::translate("machine", "nom_machine", nullptr));
        tri_par->setItemText(2, QCoreApplication::translate("machine", "type_machine", nullptr));
        tri_par->setItemText(3, QCoreApplication::translate("machine", "etat_fonctionnement", nullptr));
        tri_par->setItemText(4, QCoreApplication::translate("machine", "temperature_actuelle", nullptr));
        tri_par->setItemText(5, QCoreApplication::translate("machine", "date_derniere_maintenance", nullptr));
        tri_par->setItemText(6, QCoreApplication::translate("machine", "niveau_criticite", nullptr));

        btnFiltrer->setText(QCoreApplication::translate("machine", "\342\232\231\357\270\217 Filtrer", nullptr));
        groupFormMachineParc->setTitle(QCoreApplication::translate("machine", "Informations de la machine", nullptr));
        lblNomMachineParc->setText(QCoreApplication::translate("machine", "Nom de la machine:", nullptr));
        nom_machine_parc->setPlaceholderText(QCoreApplication::translate("machine", "Entrez le nom de la machine", nullptr));
        lblTypeMachineParc->setText(QCoreApplication::translate("machine", "Type de machine:", nullptr));
        type_machine_parc->setItemText(0, QCoreApplication::translate("machine", "Presse", nullptr));
        type_machine_parc->setItemText(1, QCoreApplication::translate("machine", "Broyeur", nullptr));
        type_machine_parc->setItemText(2, QCoreApplication::translate("machine", "Malaxeur", nullptr));
        type_machine_parc->setItemText(3, QCoreApplication::translate("machine", "Chauffeur", nullptr));

        lblEtatMarcheParc->setText(QCoreApplication::translate("machine", "\303\211tat marche:", nullptr));
        etat_marche_parc->setItemText(0, QCoreApplication::translate("machine", "ON", nullptr));
        etat_marche_parc->setItemText(1, QCoreApplication::translate("machine", "OFF", nullptr));
        etat_marche_parc->setItemText(2, QCoreApplication::translate("machine", "VEILLE", nullptr));

        lblTemperatureParc->setText(QCoreApplication::translate("machine", "Temp\303\251rature actuelle (\302\260C):", nullptr));
        lblNiveauChargeParc->setText(QCoreApplication::translate("machine", "Niveau de charge (%):", nullptr));
        lblEtatFonctionnementParc->setText(QCoreApplication::translate("machine", "\303\211tat fonctionnement:", nullptr));
        etat_fonctionnement_parc->setItemText(0, QCoreApplication::translate("machine", "Normal", nullptr));
        etat_fonctionnement_parc->setItemText(1, QCoreApplication::translate("machine", "Alerte", nullptr));
        etat_fonctionnement_parc->setItemText(2, QCoreApplication::translate("machine", "Panne", nullptr));

        lblTypeAlerteParc->setText(QCoreApplication::translate("machine", "Type d'alerte:", nullptr));
        type_alerte_parc->setItemText(0, QCoreApplication::translate("machine", "Aucune", nullptr));
        type_alerte_parc->setItemText(1, QCoreApplication::translate("machine", "Temp\303\251rature", nullptr));
        type_alerte_parc->setItemText(2, QCoreApplication::translate("machine", "Surcharge", nullptr));
        type_alerte_parc->setItemText(3, QCoreApplication::translate("machine", "Panne", nullptr));

        lblNiveauCriticiteParc->setText(QCoreApplication::translate("machine", "Niveau de criticit\303\251:", nullptr));
        niveau_criticite_parc->setItemText(0, QCoreApplication::translate("machine", "Faible", nullptr));
        niveau_criticite_parc->setItemText(1, QCoreApplication::translate("machine", "Moyen", nullptr));
        niveau_criticite_parc->setItemText(2, QCoreApplication::translate("machine", "\303\211lev\303\251", nullptr));
        niveau_criticite_parc->setItemText(3, QCoreApplication::translate("machine", "Critique", nullptr));

        lblDateMaintenanceParc->setText(QCoreApplication::translate("machine", "Derni\303\250re maintenance:", nullptr));
        btnEnregistrerParc->setText(QCoreApplication::translate("machine", "\360\237\222\276 Enregistrer", nullptr));
        btnAnnulerParc->setStyleSheet(QCoreApplication::translate("machine", "background-color: #6c757d;", nullptr));
        btnAnnulerParc->setText(QCoreApplication::translate("machine", "\342\234\227 Annuler", nullptr));
        scrollHistoriqueOnOff->setStyleSheet(QCoreApplication::translate("machine", "QScrollArea { background-color: white; border: 1px solid #ddd; }", nullptr));
        lblHistoriqueTitle->setStyleSheet(QCoreApplication::translate("machine", "font-weight: bold; font-size: 16px; color: #1B4D3E;", nullptr));
        lblHistoriqueTitle->setText(QCoreApplication::translate("machine", "\360\237\223\234 Historique ON / OFF des machines", nullptr));
        btnFermerHistorique->setStyleSheet(QCoreApplication::translate("machine", "QPushButton { \n"
"    background-color: #dc3545; \n"
"    color: white; \n"
"    border: none; \n"
"    border-radius: 20px; \n"
"    font-weight: bold; \n"
"    font-size: 18px;\n"
"}\n"
"QPushButton:hover { \n"
"    background-color: #c82333; \n"
"}\n"
"QPushButton:pressed { \n"
"    background-color: #bd2130; \n"
"}", nullptr));
        btnFermerHistorique->setText(QCoreApplication::translate("machine", "\342\234\225", nullptr));
        frameMachine1->setStyleSheet(QCoreApplication::translate("machine", "QFrame { background-color: #f8f9fa; border: 1px solid #dee2e6; border-radius: 8px; padding: 12px; }", nullptr));
        lblMachine1Nom->setStyleSheet(QCoreApplication::translate("machine", "font-weight: bold; font-size: 14px; color: #1B4D3E; border: none;", nullptr));
        lblMachine1Nom->setText(QCoreApplication::translate("machine", "\360\237\217\255 Presse Hydraulique P-001", nullptr));
        lblMachine1On->setStyleSheet(QCoreApplication::translate("machine", "font-weight: bold; color: #155724; background-color: #d4edda; padding: 8px 15px; border-radius: 4px; border: none;", nullptr));
        lblMachine1On->setText(QCoreApplication::translate("machine", "\342\217\261\357\270\217 Temps total ON: 8h 45m", nullptr));
        lblMachine1Off->setStyleSheet(QCoreApplication::translate("machine", "font-weight: bold; color: #721c24; background-color: #f8d7da; padding: 8px 15px; border-radius: 4px; border: none;", nullptr));
        lblMachine1Off->setText(QCoreApplication::translate("machine", "\342\217\261\357\270\217 Temps total OFF: 3h 15m", nullptr));
        frameMachine2->setStyleSheet(QCoreApplication::translate("machine", "QFrame { background-color: #f8f9fa; border: 1px solid #dee2e6; border-radius: 8px; padding: 12px; }", nullptr));
        lblMachine2Nom->setStyleSheet(QCoreApplication::translate("machine", "font-weight: bold; font-size: 14px; color: #1B4D3E; border: none;", nullptr));
        lblMachine2Nom->setText(QCoreApplication::translate("machine", "\360\237\217\255 Broyeur Industriel B-007", nullptr));
        lblMachine2On->setStyleSheet(QCoreApplication::translate("machine", "font-weight: bold; color: #155724; background-color: #d4edda; padding: 8px 15px; border-radius: 4px; border: none;", nullptr));
        lblMachine2On->setText(QCoreApplication::translate("machine", "\342\217\261\357\270\217 Temps total ON: 12h 30m", nullptr));
        lblMachine2Off->setStyleSheet(QCoreApplication::translate("machine", "font-weight: bold; color: #721c24; background-color: #f8d7da; padding: 8px 15px; border-radius: 4px; border: none;", nullptr));
        lblMachine2Off->setText(QCoreApplication::translate("machine", "\342\217\261\357\270\217 Temps total OFF: 1h 05m", nullptr));
        frameMachine3->setStyleSheet(QCoreApplication::translate("machine", "QFrame { background-color: #f8f9fa; border: 1px solid #dee2e6; border-radius: 8px; padding: 12px; }", nullptr));
        lblMachine3Nom->setStyleSheet(QCoreApplication::translate("machine", "font-weight: bold; font-size: 14px; color: #1B4D3E; border: none;", nullptr));
        lblMachine3Nom->setText(QCoreApplication::translate("machine", "\360\237\217\255 Malaxeur Rotatif M-103", nullptr));
        lblMachine3On->setStyleSheet(QCoreApplication::translate("machine", "font-weight: bold; color: #155724; background-color: #d4edda; padding: 8px 15px; border-radius: 4px; border: none;", nullptr));
        lblMachine3On->setText(QCoreApplication::translate("machine", "\342\217\261\357\270\217 Temps total ON: 6h 20m", nullptr));
        lblMachine3Off->setStyleSheet(QCoreApplication::translate("machine", "font-weight: bold; color: #721c24; background-color: #f8d7da; padding: 8px 15px; border-radius: 4px; border: none;", nullptr));
        lblMachine3Off->setText(QCoreApplication::translate("machine", "\342\217\261\357\270\217 Temps total OFF: 5h 40m", nullptr));
        frameMachine4->setStyleSheet(QCoreApplication::translate("machine", "QFrame { background-color: #f8f9fa; border: 1px solid #dee2e6; border-radius: 8px; padding: 12px; }", nullptr));
        lblMachine4Nom->setStyleSheet(QCoreApplication::translate("machine", "font-weight: bold; font-size: 14px; color: #1B4D3E; border: none;", nullptr));
        lblMachine4Nom->setText(QCoreApplication::translate("machine", "\360\237\217\255 Chauffeur \303\211lectrique C-025", nullptr));
        lblMachine4On->setStyleSheet(QCoreApplication::translate("machine", "font-weight: bold; color: #155724; background-color: #d4edda; padding: 8px 15px; border-radius: 4px; border: none;", nullptr));
        lblMachine4On->setText(QCoreApplication::translate("machine", "\342\217\261\357\270\217 Temps total ON: 10h 15m", nullptr));
        lblMachine4Off->setStyleSheet(QCoreApplication::translate("machine", "font-weight: bold; color: #721c24; background-color: #f8d7da; padding: 8px 15px; border-radius: 4px; border: none;", nullptr));
        lblMachine4Off->setText(QCoreApplication::translate("machine", "\342\217\261\357\270\217 Temps total OFF: 2h 30m", nullptr));
        frameMachine5->setStyleSheet(QCoreApplication::translate("machine", "QFrame { background-color: #f8f9fa; border: 1px solid #dee2e6; border-radius: 8px; padding: 12px; }", nullptr));
        lblMachine5Nom->setStyleSheet(QCoreApplication::translate("machine", "font-weight: bold; font-size: 14px; color: #1B4D3E; border: none;", nullptr));
        lblMachine5Nom->setText(QCoreApplication::translate("machine", "\360\237\217\255 Presse M\303\251canique P-042", nullptr));
        lblMachine5On->setStyleSheet(QCoreApplication::translate("machine", "font-weight: bold; color: #155724; background-color: #d4edda; padding: 8px 15px; border-radius: 4px; border: none;", nullptr));
        lblMachine5On->setText(QCoreApplication::translate("machine", "\342\217\261\357\270\217 Temps total ON: 4h 50m", nullptr));
        lblMachine5Off->setStyleSheet(QCoreApplication::translate("machine", "font-weight: bold; color: #721c24; background-color: #f8d7da; padding: 8px 15px; border-radius: 4px; border: none;", nullptr));
        lblMachine5Off->setText(QCoreApplication::translate("machine", "\342\217\261\357\270\217 Temps total OFF: 7h 10m", nullptr));
        scrollListeMachines->setStyleSheet(QCoreApplication::translate("machine", "QScrollArea { background-color: white; border: none; }", nullptr));
        cardMachine1->setStyleSheet(QCoreApplication::translate("machine", "QFrame#cardMachine1 { background-color: white; border: 2px solid #2D5F47; border-radius: 12px; padding: 15px; }\n"
"QFrame#cardMachine1:hover { border-color: #4CAF50; }", nullptr));
        lblCard1Nom->setStyleSheet(QCoreApplication::translate("machine", "font-weight: bold; font-size: 16px; color: #1B4D3E; border: none;", nullptr));
        lblCard1Nom->setText(QCoreApplication::translate("machine", "\360\237\217\255 Presse Hydraulique P-001", nullptr));
        lblCard1EtatMarche->setStyleSheet(QCoreApplication::translate("machine", "font-weight: bold; color: white; background-color: #4CAF50; padding: 6px 12px; border-radius: 6px; border: none;", nullptr));
        lblCard1EtatMarche->setText(QCoreApplication::translate("machine", "\360\237\224\265 ON", nullptr));
        frameSeparator1->setStyleSheet(QCoreApplication::translate("machine", "background-color: #e0e0e0; border: none;", nullptr));
        lblCard1TypeLabel->setStyleSheet(QCoreApplication::translate("machine", "font-weight: 600; color: #555; border: none;", nullptr));
        lblCard1TypeLabel->setText(QCoreApplication::translate("machine", "Type :", nullptr));
        lblCard1Type->setStyleSheet(QCoreApplication::translate("machine", "color: #1B4D3E; border: none;", nullptr));
        lblCard1Type->setText(QCoreApplication::translate("machine", "Presse", nullptr));
        lblCard1TempLabel->setStyleSheet(QCoreApplication::translate("machine", "font-weight: 600; color: #555; border: none;", nullptr));
        lblCard1TempLabel->setText(QCoreApplication::translate("machine", "Temp\303\251rature :", nullptr));
        lblCard1Temp->setStyleSheet(QCoreApplication::translate("machine", "color: #1B4D3E; border: none;", nullptr));
        lblCard1Temp->setText(QCoreApplication::translate("machine", "\360\237\214\241\357\270\217 45.5\302\260C", nullptr));
        lblCard1EtatFonctLabel->setStyleSheet(QCoreApplication::translate("machine", "font-weight: 600; color: #555; border: none;", nullptr));
        lblCard1EtatFonctLabel->setText(QCoreApplication::translate("machine", "\303\211tat :", nullptr));
        lblCard1EtatFonct->setStyleSheet(QCoreApplication::translate("machine", "font-weight: bold; color: #155724; background-color: #d4edda; padding: 4px 10px; border-radius: 4px; border: none;", nullptr));
        lblCard1EtatFonct->setText(QCoreApplication::translate("machine", "\342\234\223 Normal", nullptr));
        lblCard1ChargeLabel->setStyleSheet(QCoreApplication::translate("machine", "font-weight: 600; color: #555; border: none;", nullptr));
        lblCard1ChargeLabel->setText(QCoreApplication::translate("machine", "Charge :", nullptr));
        lblCard1Charge->setStyleSheet(QCoreApplication::translate("machine", "color: #1B4D3E; border: none;", nullptr));
        lblCard1Charge->setText(QCoreApplication::translate("machine", "\342\232\241 78%", nullptr));
        lblCard1AlerteLabel->setStyleSheet(QCoreApplication::translate("machine", "font-weight: 600; color: #555; border: none;", nullptr));
        lblCard1AlerteLabel->setText(QCoreApplication::translate("machine", "Alerte :", nullptr));
        lblCard1Alerte->setStyleSheet(QCoreApplication::translate("machine", "color: #1B4D3E; border: none;", nullptr));
        lblCard1Alerte->setText(QCoreApplication::translate("machine", "Aucune", nullptr));
        lblCard1CriticiteLabel->setStyleSheet(QCoreApplication::translate("machine", "font-weight: 600; color: #555; border: none;", nullptr));
        lblCard1CriticiteLabel->setText(QCoreApplication::translate("machine", "Criticit\303\251 :", nullptr));
        lblCard1Criticite->setStyleSheet(QCoreApplication::translate("machine", "color: #1B4D3E; border: none;", nullptr));
        lblCard1Criticite->setText(QCoreApplication::translate("machine", "Faible", nullptr));
        lblCard1MaintenanceLabel->setStyleSheet(QCoreApplication::translate("machine", "font-weight: 600; color: #555; border: none;", nullptr));
        lblCard1MaintenanceLabel->setText(QCoreApplication::translate("machine", "Derni\303\250re maintenance :", nullptr));
        lblCard1Maintenance->setStyleSheet(QCoreApplication::translate("machine", "color: #1B4D3E; border: none;", nullptr));
        lblCard1Maintenance->setText(QCoreApplication::translate("machine", "\360\237\223\205 15 Janvier 2026", nullptr));
        cardMachine2->setStyleSheet(QCoreApplication::translate("machine", "QFrame#cardMachine2 { background-color: white; border: 2px solid #2D5F47; border-radius: 12px; padding: 15px; }\n"
"QFrame#cardMachine2:hover { border-color: #4CAF50; }", nullptr));
        lblCard2Nom->setStyleSheet(QCoreApplication::translate("machine", "font-weight: bold; font-size: 16px; color: #1B4D3E; border: none;", nullptr));
        lblCard2Nom->setText(QCoreApplication::translate("machine", "\360\237\217\255 Broyeur Industriel B-007", nullptr));
        lblCard2EtatMarche->setStyleSheet(QCoreApplication::translate("machine", "font-weight: bold; color: white; background-color: #FFC107; padding: 6px 12px; border-radius: 6px; border: none;", nullptr));
        lblCard2EtatMarche->setText(QCoreApplication::translate("machine", "\342\217\270\357\270\217 VEILLE", nullptr));
        frameSeparator2->setStyleSheet(QCoreApplication::translate("machine", "background-color: #e0e0e0; border: none;", nullptr));
        lblCard2TypeLabel->setStyleSheet(QCoreApplication::translate("machine", "font-weight: 600; color: #555; border: none;", nullptr));
        lblCard2TypeLabel->setText(QCoreApplication::translate("machine", "Type :", nullptr));
        lblCard2Type->setStyleSheet(QCoreApplication::translate("machine", "color: #1B4D3E; border: none;", nullptr));
        lblCard2Type->setText(QCoreApplication::translate("machine", "Broyeur", nullptr));
        lblCard2TempLabel->setStyleSheet(QCoreApplication::translate("machine", "font-weight: 600; color: #555; border: none;", nullptr));
        lblCard2TempLabel->setText(QCoreApplication::translate("machine", "Temp\303\251rature :", nullptr));
        lblCard2Temp->setStyleSheet(QCoreApplication::translate("machine", "color: #1B4D3E; border: none;", nullptr));
        lblCard2Temp->setText(QCoreApplication::translate("machine", "\360\237\214\241\357\270\217 32.8\302\260C", nullptr));
        lblCard2EtatFonctLabel->setStyleSheet(QCoreApplication::translate("machine", "font-weight: 600; color: #555; border: none;", nullptr));
        lblCard2EtatFonctLabel->setText(QCoreApplication::translate("machine", "\303\211tat :", nullptr));
        lblCard2EtatFonct->setStyleSheet(QCoreApplication::translate("machine", "font-weight: bold; color: #856404; background-color: #fff3cd; padding: 4px 10px; border-radius: 4px; border: none;", nullptr));
        lblCard2EtatFonct->setText(QCoreApplication::translate("machine", "\342\232\240 Alerte", nullptr));
        lblCard2ChargeLabel->setStyleSheet(QCoreApplication::translate("machine", "font-weight: 600; color: #555; border: none;", nullptr));
        lblCard2ChargeLabel->setText(QCoreApplication::translate("machine", "Charge :", nullptr));
        lblCard2Charge->setStyleSheet(QCoreApplication::translate("machine", "color: #1B4D3E; border: none;", nullptr));
        lblCard2Charge->setText(QCoreApplication::translate("machine", "\342\232\241 45%", nullptr));
        lblCard2AlerteLabel->setStyleSheet(QCoreApplication::translate("machine", "font-weight: 600; color: #555; border: none;", nullptr));
        lblCard2AlerteLabel->setText(QCoreApplication::translate("machine", "Alerte :", nullptr));
        lblCard2Alerte->setStyleSheet(QCoreApplication::translate("machine", "color: #F39C12; font-weight: bold; border: none;", nullptr));
        lblCard2Alerte->setText(QCoreApplication::translate("machine", "\342\232\240 Maintenance", nullptr));
        lblCard2CriticiteLabel->setStyleSheet(QCoreApplication::translate("machine", "font-weight: 600; color: #555; border: none;", nullptr));
        lblCard2CriticiteLabel->setText(QCoreApplication::translate("machine", "Criticit\303\251 :", nullptr));
        lblCard2Criticite->setStyleSheet(QCoreApplication::translate("machine", "color: #F39C12; font-weight: bold; border: none;", nullptr));
        lblCard2Criticite->setText(QCoreApplication::translate("machine", "Moyen", nullptr));
        lblCard2MaintenanceLabel->setStyleSheet(QCoreApplication::translate("machine", "font-weight: 600; color: #555; border: none;", nullptr));
        lblCard2MaintenanceLabel->setText(QCoreApplication::translate("machine", "Derni\303\250re maintenance :", nullptr));
        lblCard2Maintenance->setStyleSheet(QCoreApplication::translate("machine", "color: #1B4D3E; border: none;", nullptr));
        lblCard2Maintenance->setText(QCoreApplication::translate("machine", "\360\237\223\205 10 D\303\251cembre 2025", nullptr));
        cardMachine3->setStyleSheet(QCoreApplication::translate("machine", "QFrame#cardMachine3 { background-color: white; border: 2px solid #2D5F47; border-radius: 12px; padding: 15px; }\n"
"QFrame#cardMachine3:hover { border-color: #4CAF50; }", nullptr));
        lblCard3Nom->setStyleSheet(QCoreApplication::translate("machine", "font-weight: bold; font-size: 16px; color: #1B4D3E; border: none;", nullptr));
        lblCard3Nom->setText(QCoreApplication::translate("machine", "\360\237\217\255 Malaxeur Rotatif M-103", nullptr));
        lblCard3EtatMarche->setStyleSheet(QCoreApplication::translate("machine", "font-weight: bold; color: white; background-color: #dc3545; padding: 6px 12px; border-radius: 6px; border: none;", nullptr));
        lblCard3EtatMarche->setText(QCoreApplication::translate("machine", "\342\255\225 OFF", nullptr));
        frameSeparator3->setStyleSheet(QCoreApplication::translate("machine", "background-color: #e0e0e0; border: none;", nullptr));
        lblCard3TypeLabel->setStyleSheet(QCoreApplication::translate("machine", "font-weight: 600; color: #555; border: none;", nullptr));
        lblCard3TypeLabel->setText(QCoreApplication::translate("machine", "Type :", nullptr));
        lblCard3Type->setStyleSheet(QCoreApplication::translate("machine", "color: #1B4D3E; border: none;", nullptr));
        lblCard3Type->setText(QCoreApplication::translate("machine", "Malaxeur", nullptr));
        lblCard3TempLabel->setStyleSheet(QCoreApplication::translate("machine", "font-weight: 600; color: #555; border: none;", nullptr));
        lblCard3TempLabel->setText(QCoreApplication::translate("machine", "Temp\303\251rature :", nullptr));
        lblCard3Temp->setStyleSheet(QCoreApplication::translate("machine", "color: #1B4D3E; border: none;", nullptr));
        lblCard3Temp->setText(QCoreApplication::translate("machine", "\360\237\214\241\357\270\217 88.2\302\260C", nullptr));
        lblCard3EtatFonctLabel->setStyleSheet(QCoreApplication::translate("machine", "font-weight: 600; color: #555; border: none;", nullptr));
        lblCard3EtatFonctLabel->setText(QCoreApplication::translate("machine", "\303\211tat :", nullptr));
        lblCard3EtatFonct->setStyleSheet(QCoreApplication::translate("machine", "font-weight: bold; color: #721c24; background-color: #f8d7da; padding: 4px 10px; border-radius: 4px; border: none;", nullptr));
        lblCard3EtatFonct->setText(QCoreApplication::translate("machine", "\342\234\227 Panne", nullptr));
        lblCard3ChargeLabel->setStyleSheet(QCoreApplication::translate("machine", "font-weight: 600; color: #555; border: none;", nullptr));
        lblCard3ChargeLabel->setText(QCoreApplication::translate("machine", "Charge :", nullptr));
        lblCard3Charge->setStyleSheet(QCoreApplication::translate("machine", "color: #1B4D3E; border: none;", nullptr));
        lblCard3Charge->setText(QCoreApplication::translate("machine", "\342\232\241 0%", nullptr));
        lblCard3AlerteLabel->setStyleSheet(QCoreApplication::translate("machine", "font-weight: 600; color: #555; border: none;", nullptr));
        lblCard3AlerteLabel->setText(QCoreApplication::translate("machine", "Alerte :", nullptr));
        lblCard3Alerte->setStyleSheet(QCoreApplication::translate("machine", "color: #dc3545; font-weight: bold; border: none;", nullptr));
        lblCard3Alerte->setText(QCoreApplication::translate("machine", "\360\237\224\264 Temp\303\251rature", nullptr));
        lblCard3CriticiteLabel->setStyleSheet(QCoreApplication::translate("machine", "font-weight: 600; color: #555; border: none;", nullptr));
        lblCard3CriticiteLabel->setText(QCoreApplication::translate("machine", "Criticit\303\251 :", nullptr));
        lblCard3Criticite->setStyleSheet(QCoreApplication::translate("machine", "color: #dc3545; font-weight: bold; border: none;", nullptr));
        lblCard3Criticite->setText(QCoreApplication::translate("machine", "\303\211lev\303\251", nullptr));
        lblCard3MaintenanceLabel->setStyleSheet(QCoreApplication::translate("machine", "font-weight: 600; color: #555; border: none;", nullptr));
        lblCard3MaintenanceLabel->setText(QCoreApplication::translate("machine", "Derni\303\250re maintenance :", nullptr));
        lblCard3Maintenance->setStyleSheet(QCoreApplication::translate("machine", "color: #1B4D3E; border: none;", nullptr));
        lblCard3Maintenance->setText(QCoreApplication::translate("machine", "\360\237\223\205 05 F\303\251vrier 2026", nullptr));
        tabWidgetMachineManagement->setTabText(tabWidgetMachineManagement->indexOf(tabParcMachines), QCoreApplication::translate("machine", "\360\237\217\255 Parc machines", nullptr));
        groupFormMachine->setTitle(QCoreApplication::translate("machine", "Informations de la machine", nullptr));
        lblNomMachine->setText(QCoreApplication::translate("machine", "Nom de la machine:", nullptr));
        nom_machine->setPlaceholderText(QCoreApplication::translate("machine", "Entrez le nom de la machine", nullptr));
        lblTypeMachine->setText(QCoreApplication::translate("machine", "Type de machine:", nullptr));
        type_machine->setItemText(0, QCoreApplication::translate("machine", "Presse", nullptr));
        type_machine->setItemText(1, QCoreApplication::translate("machine", "Broyeur", nullptr));
        type_machine->setItemText(2, QCoreApplication::translate("machine", "Malaxeur", nullptr));
        type_machine->setItemText(3, QCoreApplication::translate("machine", "Chauffeur", nullptr));

        lblEtatMarche->setText(QCoreApplication::translate("machine", "\303\211tat marche:", nullptr));
        etat_marche->setItemText(0, QCoreApplication::translate("machine", "ON", nullptr));
        etat_marche->setItemText(1, QCoreApplication::translate("machine", "OFF", nullptr));
        etat_marche->setItemText(2, QCoreApplication::translate("machine", "VEILLE", nullptr));

        lblTemperature->setText(QCoreApplication::translate("machine", "Temp\303\251rature actuelle (\302\260C):", nullptr));
        lblNiveauCharge->setText(QCoreApplication::translate("machine", "Niveau de charge (%):", nullptr));
        lblEtatFonctionnement->setText(QCoreApplication::translate("machine", "\303\211tat fonctionnement:", nullptr));
        etat_fonctionnement->setItemText(0, QCoreApplication::translate("machine", "Normal", nullptr));
        etat_fonctionnement->setItemText(1, QCoreApplication::translate("machine", "Alerte", nullptr));
        etat_fonctionnement->setItemText(2, QCoreApplication::translate("machine", "Panne", nullptr));

        lblTypeAlerte->setText(QCoreApplication::translate("machine", "Type d'alerte:", nullptr));
        type_alerte->setItemText(0, QCoreApplication::translate("machine", "Aucune", nullptr));
        type_alerte->setItemText(1, QCoreApplication::translate("machine", "Temp\303\251rature", nullptr));
        type_alerte->setItemText(2, QCoreApplication::translate("machine", "Surcharge", nullptr));
        type_alerte->setItemText(3, QCoreApplication::translate("machine", "Panne", nullptr));

        lblNiveauCriticite->setText(QCoreApplication::translate("machine", "Niveau de criticit\303\251:", nullptr));
        niveau_criticite->setItemText(0, QCoreApplication::translate("machine", "Faible", nullptr));
        niveau_criticite->setItemText(1, QCoreApplication::translate("machine", "Moyen", nullptr));
        niveau_criticite->setItemText(2, QCoreApplication::translate("machine", "\303\211lev\303\251", nullptr));
        niveau_criticite->setItemText(3, QCoreApplication::translate("machine", "Critique", nullptr));

        lblDateMaintenance->setText(QCoreApplication::translate("machine", "Derni\303\250re maintenance:", nullptr));
        btnEnregistrer->setText(QCoreApplication::translate("machine", "\360\237\222\276 Enregistrer", nullptr));
        btnAnnuler->setStyleSheet(QCoreApplication::translate("machine", "background-color: #6c757d;", nullptr));
        btnAnnuler->setText(QCoreApplication::translate("machine", "\342\234\227 Annuler", nullptr));
        tabWidgetMachineManagement->setTabText(tabWidgetMachineManagement->indexOf(tabAjouterModifier), QCoreApplication::translate("machine", "\342\236\225 Ajouter / Modifier", nullptr));
        groupInfosMachine->setTitle(QCoreApplication::translate("machine", "Informations de la machine", nullptr));
        lblInfoNom->setStyleSheet(QCoreApplication::translate("machine", "font-weight: bold;", nullptr));
        lblInfoNom->setText(QCoreApplication::translate("machine", "Nom:", nullptr));
        lblValeurNom->setText(QCoreApplication::translate("machine", "--", nullptr));
        lblInfoType->setStyleSheet(QCoreApplication::translate("machine", "font-weight: bold;", nullptr));
        lblInfoType->setText(QCoreApplication::translate("machine", "Type:", nullptr));
        lblValeurType->setText(QCoreApplication::translate("machine", "--", nullptr));
        lblInfoEtat->setStyleSheet(QCoreApplication::translate("machine", "font-weight: bold;", nullptr));
        lblInfoEtat->setText(QCoreApplication::translate("machine", "\303\211tat marche:", nullptr));
        lblValeurEtat->setText(QCoreApplication::translate("machine", "--", nullptr));
        lblInfoTemp->setStyleSheet(QCoreApplication::translate("machine", "font-weight: bold;", nullptr));
        lblInfoTemp->setText(QCoreApplication::translate("machine", "Temp\303\251rature:", nullptr));
        lblValeurTemp->setText(QCoreApplication::translate("machine", "-- \302\260C", nullptr));
        lblInfoScore->setStyleSheet(QCoreApplication::translate("machine", "font-weight: bold;", nullptr));
        lblInfoScore->setText(QCoreApplication::translate("machine", "Score sant\303\251:", nullptr));
        groupActions->setTitle(QCoreApplication::translate("machine", "Actions disponibles", nullptr));
        btnReinitialiser->setText(QCoreApplication::translate("machine", "\360\237\224\204 R\303\251initialiser machine", nullptr));
        btnMiseVeille->setStyleSheet(QCoreApplication::translate("machine", "background-color: #F39C12;", nullptr));
        btnMiseVeille->setText(QCoreApplication::translate("machine", "\360\237\222\244 Mise en veille", nullptr));
        btnArretForce->setStyleSheet(QCoreApplication::translate("machine", "background-color: #dc3545;", nullptr));
        btnArretForce->setText(QCoreApplication::translate("machine", "\342\233\224 Arr\303\252t forc\303\251", nullptr));
        tabWidgetMachineManagement->setTabText(tabWidgetMachineManagement->indexOf(tabDetails), QCoreApplication::translate("machine", "\360\237\223\213 D\303\251tails machine", nullptr));
        groupOptionsStats->setTitle(QCoreApplication::translate("machine", "Options de visualisation", nullptr));
        lblTypeStats->setText(QCoreApplication::translate("machine", "Type de statistique:", nullptr));
        type_statistique->setItemText(0, QCoreApplication::translate("machine", "\303\211tat fonctionnement", nullptr));
        type_statistique->setItemText(1, QCoreApplication::translate("machine", "Types alertes", nullptr));
        type_statistique->setItemText(2, QCoreApplication::translate("machine", "Temp\303\251rature moyenne", nullptr));

        lblTypeGraphique->setText(QCoreApplication::translate("machine", "Type de graphique:", nullptr));
        type_graphique->setItemText(0, QCoreApplication::translate("machine", "BarChart", nullptr));
        type_graphique->setItemText(1, QCoreApplication::translate("machine", "PieChart", nullptr));
        type_graphique->setItemText(2, QCoreApplication::translate("machine", "LineChart", nullptr));

        btnGenererStats->setText(QCoreApplication::translate("machine", "\360\237\223\210 G\303\251n\303\251rer", nullptr));
        zoneGraphique->setStyleSheet(QCoreApplication::translate("machine", "background-color: white; border: 1px solid #ddd; border-radius: 4px;", nullptr));
        groupMaintenancePredictive->setTitle(QCoreApplication::translate("machine", "Maintenance pr\303\251dictive - Machines \303\240 surveiller", nullptr));
        btnEnregistrerMaintenance->setText(QCoreApplication::translate("machine", "\360\237\222\276 Enregistrer maintenance", nullptr));
        btnPlanifierMaintenance->setStyleSheet(QCoreApplication::translate("machine", "background-color: #F39C12;", nullptr));
        btnPlanifierMaintenance->setText(QCoreApplication::translate("machine", "\360\237\223\205 Planifier maintenance", nullptr));
        tabWidgetMachineManagement->setTabText(tabWidgetMachineManagement->indexOf(tabStatistiques), QCoreApplication::translate("machine", "\360\237\223\212 Statistiques", nullptr));
        groupHistoriqueOnOff->setTitle(QCoreApplication::translate("machine", "Historique ON / OFF des machines", nullptr));
        frameTempsTotal->setStyleSheet(QCoreApplication::translate("machine", "background-color: #f8f9fa; border: 1px solid #ddd; border-radius: 4px; padding: 10px;", nullptr));
        lblTempsTotalOn->setStyleSheet(QCoreApplication::translate("machine", "font-weight: bold; color: #155724; background-color: #d4edda; padding: 8px 15px; border-radius: 4px;", nullptr));
        lblTempsTotalOn->setText(QCoreApplication::translate("machine", "\342\217\261\357\270\217 Temps total ON: 0h 0m", nullptr));
        lblTempsTotalOff->setStyleSheet(QCoreApplication::translate("machine", "font-weight: bold; color: #721c24; background-color: #f8d7da; padding: 8px 15px; border-radius: 4px;", nullptr));
        lblTempsTotalOff->setText(QCoreApplication::translate("machine", "\342\217\261\357\270\217 Temps total OFF: 0h 0m", nullptr));
        tabWidgetMachineManagement->setTabText(tabWidgetMachineManagement->indexOf(tabHistorique), QCoreApplication::translate("machine", "\360\237\223\234 Historique", nullptr));
        groupRecommandationIA->setTitle(QCoreApplication::translate("machine", "Recommandation de l'Intelligence Artificielle", nullptr));
        lblRecommandationIA->setStyleSheet(QCoreApplication::translate("machine", "font-size: 16px; padding: 20px; background-color: #e8f4fd; border: 1px solid #bee5eb; border-radius: 6px;", nullptr));
        lblRecommandationIA->setText(QCoreApplication::translate("machine", "\360\237\224\215 Analyse en cours...", nullptr));
        lblConfianceLabel->setStyleSheet(QCoreApplication::translate("machine", "font-weight: bold;", nullptr));
        lblConfianceLabel->setText(QCoreApplication::translate("machine", "Niveau de confiance:", nullptr));
        btnAppliquerDecision->setText(QCoreApplication::translate("machine", "\342\234\223 Appliquer d\303\251cision", nullptr));
        btnIgnorerDecision->setStyleSheet(QCoreApplication::translate("machine", "background-color: #6c757d;", nullptr));
        btnIgnorerDecision->setText(QCoreApplication::translate("machine", "\342\234\227 Ignorer", nullptr));
        tabWidgetMachineManagement->setTabText(tabWidgetMachineManagement->indexOf(tabDecisionIA), QCoreApplication::translate("machine", "\360\237\244\226 D\303\251cision IA", nullptr));
    } // retranslateUi

};

namespace Ui {
    class machine: public Ui_machine {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MACHINE_H
