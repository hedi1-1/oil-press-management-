/********************************************************************************
** Form generated from reading UI file 'machine.ui'
**
** Created by: Qt User Interface Compiler version 6.7.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MACHINE_H
#define UI_MACHINE_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDateEdit>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
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
    QToolBar *toolBarMachines;
    QGroupBox *groupRecherche;
    QHBoxLayout *layoutRecherche;
    QLineEdit *recherche_nom_machine;
    QComboBox *filtre_type_machine;
    QComboBox *filtre_etat_marche;
    QComboBox *filtre_type_alerte;
    QComboBox *tri_par;
    QPushButton *btnRechercher;
    QTableView *tableMachines;
    QFrame *frameSummary;
    QHBoxLayout *layoutSummary;
    QLabel *lblMachinesNormales;
    QLabel *lblMachinesAlerte;
    QLabel *lblMachinesPanne;
    QSpacerItem *summarySpacer;
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
    QWidget *tabAlertes;
    QVBoxLayout *layoutAlertes;
    QGroupBox *groupFiltresAlertes;
    QHBoxLayout *layoutFiltresAlertes;
    QComboBox *filtre_alerte_type;
    QComboBox *filtre_alerte_criticite;
    QLabel *lblDateDebut;
    QDateEdit *date_debut;
    QLabel *lblDateFin;
    QDateEdit *date_fin;
    QPushButton *btnFiltrerAlertes;
    QSpacerItem *filtresSpacer;
    QTableView *tableAlertes;
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
    QWidget *tabMaintenance;
    QVBoxLayout *layoutMaintenance;
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
"    background-color: #1B4332;\n"
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
"    border: 3px solid #2D5F47;\n"
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
"    color: #1B4332;\n"
"    font-weight: bold;\n"
"    border-bottom: 3px solid #2D5F47;\n"
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
"    border: 2px solid #2D5F47;\n"
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
"    background-color: #1B4332;\n"
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
"    border: 2px solid #1B4332;\n"
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
"    background-color: #1B4332;\n"
"    color: white;\n"
"    border: none;\n"
"}\n"
"\n"
"QPushButton:hover {\n"
"    background-color: #2D5A45;\n"
"}\n"
"\n"
"QPushButton:pressed {\n"
"    background-color: #0F2A1F;\n"
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
"    background-color: #1B4332;\n"
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
"    border: 2px solid #2D5F47;\n"
"    border-radius: 4px;\n"
"    spacing: 5px;\n"
"    padding: 5px;\n"
"}\n"
"\n"
"QToolButton {\n"
"    background-color: #1B4332;\n"
"    color: white;\n"
"    border: none;\n"
"    border-radius: 4px;\n"
"    padding: 6px 12px;\n"
"    font-weight: bold;\n"
"    font-size: 11px;\n"
"}\n"
"\n"
"QToolButton:hover {\n"
"    background-color: #2D5A45;\n"
"}\n"
"\n"
"/* Frame Summary */\n"
"#frameSummary {\n"
""
                        "    background-color: white;\n"
"    border: 2px solid #2D5F47;\n"
"    border-radius: 5px;\n"
"    padding: 10px;\n"
"}\n"
"\n"
"#frameSummary QLabel {\n"
"    font-weight: bold;\n"
"    padding: 8px 12px;\n"
"    border-radius: 4px;\n"
"    border: 1px solid #ddd;\n"
"}\n"
"\n"
"#lblMachinesNormales {\n"
"    background-color: #d4edda;\n"
"    color: #155724;\n"
"}\n"
"\n"
"#lblMachinesAlerte {\n"
"    background-color: #fff3cd;\n"
"    color: #856404;\n"
"}\n"
"\n"
"#lblMachinesPanne {\n"
"    background-color: #f8d7da;\n"
"    color: #721c24;\n"
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
        toolBarMachines = new QToolBar(tabParcMachines);
        toolBarMachines->setObjectName("toolBarMachines");
        toolBarMachines->setMovable(false);
        toolBarMachines->setFloatable(false);

        layoutParcMachines->addWidget(toolBarMachines);

        groupRecherche = new QGroupBox(tabParcMachines);
        groupRecherche->setObjectName("groupRecherche");
        layoutRecherche = new QHBoxLayout(groupRecherche);
        layoutRecherche->setObjectName("layoutRecherche");
        layoutRecherche->setContentsMargins(10, 15, 10, 10);
        recherche_nom_machine = new QLineEdit(groupRecherche);
        recherche_nom_machine->setObjectName("recherche_nom_machine");

        layoutRecherche->addWidget(recherche_nom_machine);

        filtre_type_machine = new QComboBox(groupRecherche);
        filtre_type_machine->addItem(QString());
        filtre_type_machine->addItem(QString());
        filtre_type_machine->addItem(QString());
        filtre_type_machine->addItem(QString());
        filtre_type_machine->addItem(QString());
        filtre_type_machine->setObjectName("filtre_type_machine");

        layoutRecherche->addWidget(filtre_type_machine);

        filtre_etat_marche = new QComboBox(groupRecherche);
        filtre_etat_marche->addItem(QString());
        filtre_etat_marche->addItem(QString());
        filtre_etat_marche->addItem(QString());
        filtre_etat_marche->addItem(QString());
        filtre_etat_marche->setObjectName("filtre_etat_marche");

        layoutRecherche->addWidget(filtre_etat_marche);

        filtre_type_alerte = new QComboBox(groupRecherche);
        filtre_type_alerte->addItem(QString());
        filtre_type_alerte->addItem(QString());
        filtre_type_alerte->addItem(QString());
        filtre_type_alerte->addItem(QString());
        filtre_type_alerte->addItem(QString());
        filtre_type_alerte->setObjectName("filtre_type_alerte");

        layoutRecherche->addWidget(filtre_type_alerte);

        tri_par = new QComboBox(groupRecherche);
        tri_par->addItem(QString());
        tri_par->addItem(QString());
        tri_par->addItem(QString());
        tri_par->addItem(QString());
        tri_par->addItem(QString());
        tri_par->addItem(QString());
        tri_par->addItem(QString());
        tri_par->setObjectName("tri_par");

        layoutRecherche->addWidget(tri_par);

        btnRechercher = new QPushButton(groupRecherche);
        btnRechercher->setObjectName("btnRechercher");

        layoutRecherche->addWidget(btnRechercher);


        layoutParcMachines->addWidget(groupRecherche);

        tableMachines = new QTableView(tabParcMachines);
        tableMachines->setObjectName("tableMachines");
        tableMachines->setAlternatingRowColors(true);
        tableMachines->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);

        layoutParcMachines->addWidget(tableMachines);

        frameSummary = new QFrame(tabParcMachines);
        frameSummary->setObjectName("frameSummary");
        layoutSummary = new QHBoxLayout(frameSummary);
        layoutSummary->setObjectName("layoutSummary");
        lblMachinesNormales = new QLabel(frameSummary);
        lblMachinesNormales->setObjectName("lblMachinesNormales");

        layoutSummary->addWidget(lblMachinesNormales);

        lblMachinesAlerte = new QLabel(frameSummary);
        lblMachinesAlerte->setObjectName("lblMachinesAlerte");

        layoutSummary->addWidget(lblMachinesAlerte);

        lblMachinesPanne = new QLabel(frameSummary);
        lblMachinesPanne->setObjectName("lblMachinesPanne");

        layoutSummary->addWidget(lblMachinesPanne);

        summarySpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        layoutSummary->addItem(summarySpacer);


        layoutParcMachines->addWidget(frameSummary);

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
        tabAlertes = new QWidget();
        tabAlertes->setObjectName("tabAlertes");
        layoutAlertes = new QVBoxLayout(tabAlertes);
        layoutAlertes->setSpacing(10);
        layoutAlertes->setObjectName("layoutAlertes");
        layoutAlertes->setContentsMargins(15, 10, 15, 10);
        groupFiltresAlertes = new QGroupBox(tabAlertes);
        groupFiltresAlertes->setObjectName("groupFiltresAlertes");
        layoutFiltresAlertes = new QHBoxLayout(groupFiltresAlertes);
        layoutFiltresAlertes->setObjectName("layoutFiltresAlertes");
        layoutFiltresAlertes->setContentsMargins(10, 15, 10, 10);
        filtre_alerte_type = new QComboBox(groupFiltresAlertes);
        filtre_alerte_type->addItem(QString());
        filtre_alerte_type->addItem(QString());
        filtre_alerte_type->addItem(QString());
        filtre_alerte_type->addItem(QString());
        filtre_alerte_type->setObjectName("filtre_alerte_type");

        layoutFiltresAlertes->addWidget(filtre_alerte_type);

        filtre_alerte_criticite = new QComboBox(groupFiltresAlertes);
        filtre_alerte_criticite->addItem(QString());
        filtre_alerte_criticite->addItem(QString());
        filtre_alerte_criticite->addItem(QString());
        filtre_alerte_criticite->addItem(QString());
        filtre_alerte_criticite->addItem(QString());
        filtre_alerte_criticite->setObjectName("filtre_alerte_criticite");

        layoutFiltresAlertes->addWidget(filtre_alerte_criticite);

        lblDateDebut = new QLabel(groupFiltresAlertes);
        lblDateDebut->setObjectName("lblDateDebut");

        layoutFiltresAlertes->addWidget(lblDateDebut);

        date_debut = new QDateEdit(groupFiltresAlertes);
        date_debut->setObjectName("date_debut");
        date_debut->setCalendarPopup(true);

        layoutFiltresAlertes->addWidget(date_debut);

        lblDateFin = new QLabel(groupFiltresAlertes);
        lblDateFin->setObjectName("lblDateFin");

        layoutFiltresAlertes->addWidget(lblDateFin);

        date_fin = new QDateEdit(groupFiltresAlertes);
        date_fin->setObjectName("date_fin");
        date_fin->setCalendarPopup(true);

        layoutFiltresAlertes->addWidget(date_fin);

        btnFiltrerAlertes = new QPushButton(groupFiltresAlertes);
        btnFiltrerAlertes->setObjectName("btnFiltrerAlertes");

        layoutFiltresAlertes->addWidget(btnFiltrerAlertes);

        filtresSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        layoutFiltresAlertes->addItem(filtresSpacer);


        layoutAlertes->addWidget(groupFiltresAlertes);

        tableAlertes = new QTableView(tabAlertes);
        tableAlertes->setObjectName("tableAlertes");
        tableAlertes->setAlternatingRowColors(true);
        tableAlertes->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);

        layoutAlertes->addWidget(tableAlertes);

        tabWidgetMachineManagement->addTab(tabAlertes, QString());
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

        tabWidgetMachineManagement->addTab(tabStatistiques, QString());
        tabMaintenance = new QWidget();
        tabMaintenance->setObjectName("tabMaintenance");
        layoutMaintenance = new QVBoxLayout(tabMaintenance);
        layoutMaintenance->setSpacing(10);
        layoutMaintenance->setObjectName("layoutMaintenance");
        layoutMaintenance->setContentsMargins(15, 10, 15, 10);
        groupMaintenancePredictive = new QGroupBox(tabMaintenance);
        groupMaintenancePredictive->setObjectName("groupMaintenancePredictive");
        layoutMaintenanceTable = new QVBoxLayout(groupMaintenancePredictive);
        layoutMaintenanceTable->setObjectName("layoutMaintenanceTable");
        layoutMaintenanceTable->setContentsMargins(-1, 15, -1, -1);
        tableMaintenance = new QTableView(groupMaintenancePredictive);
        tableMaintenance->setObjectName("tableMaintenance");
        tableMaintenance->setAlternatingRowColors(true);
        tableMaintenance->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);

        layoutMaintenanceTable->addWidget(tableMaintenance);


        layoutMaintenance->addWidget(groupMaintenancePredictive);

        layoutBtnsMaintenance = new QHBoxLayout();
        layoutBtnsMaintenance->setObjectName("layoutBtnsMaintenance");
        maintenanceSpacerLeft = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        layoutBtnsMaintenance->addItem(maintenanceSpacerLeft);

        btnEnregistrerMaintenance = new QPushButton(tabMaintenance);
        btnEnregistrerMaintenance->setObjectName("btnEnregistrerMaintenance");
        btnEnregistrerMaintenance->setMinimumSize(QSize(118, 35));

        layoutBtnsMaintenance->addWidget(btnEnregistrerMaintenance);

        btnPlanifierMaintenance = new QPushButton(tabMaintenance);
        btnPlanifierMaintenance->setObjectName("btnPlanifierMaintenance");
        btnPlanifierMaintenance->setMinimumSize(QSize(118, 35));

        layoutBtnsMaintenance->addWidget(btnPlanifierMaintenance);

        maintenanceSpacerRight = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        layoutBtnsMaintenance->addItem(maintenanceSpacerRight);


        layoutMaintenance->addLayout(layoutBtnsMaintenance);

        tabWidgetMachineManagement->addTab(tabMaintenance, QString());
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

        tabWidgetMachineManagement->setCurrentIndex(4);


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
        groupRecherche->setTitle(QCoreApplication::translate("machine", "Recherche et tri", nullptr));
        recherche_nom_machine->setPlaceholderText(QCoreApplication::translate("machine", "Rechercher par nom...", nullptr));
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

        tri_par->setItemText(0, QCoreApplication::translate("machine", "-- Trier par --", nullptr));
        tri_par->setItemText(1, QCoreApplication::translate("machine", "nom_machine", nullptr));
        tri_par->setItemText(2, QCoreApplication::translate("machine", "type_machine", nullptr));
        tri_par->setItemText(3, QCoreApplication::translate("machine", "etat_fonctionnement", nullptr));
        tri_par->setItemText(4, QCoreApplication::translate("machine", "temperature_actuelle", nullptr));
        tri_par->setItemText(5, QCoreApplication::translate("machine", "date_derniere_maintenance", nullptr));
        tri_par->setItemText(6, QCoreApplication::translate("machine", "niveau_criticite", nullptr));

        btnRechercher->setText(QCoreApplication::translate("machine", "\360\237\224\215 Rechercher", nullptr));
        lblMachinesNormales->setText(QCoreApplication::translate("machine", "\342\234\223 Machines normales: 0", nullptr));
        lblMachinesAlerte->setText(QCoreApplication::translate("machine", "\342\232\240 Machines en alerte: 0", nullptr));
        lblMachinesPanne->setText(QCoreApplication::translate("machine", "\342\234\227 Machines en panne: 0", nullptr));
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
        btnMiseVeille->setStyleSheet(QCoreApplication::translate("machine", "background-color: #C9A227;", nullptr));
        btnMiseVeille->setText(QCoreApplication::translate("machine", "\360\237\222\244 Mise en veille", nullptr));
        btnArretForce->setStyleSheet(QCoreApplication::translate("machine", "background-color: #dc3545;", nullptr));
        btnArretForce->setText(QCoreApplication::translate("machine", "\342\233\224 Arr\303\252t forc\303\251", nullptr));
        tabWidgetMachineManagement->setTabText(tabWidgetMachineManagement->indexOf(tabDetails), QCoreApplication::translate("machine", "\360\237\223\213 D\303\251tails machine", nullptr));
        groupFiltresAlertes->setTitle(QCoreApplication::translate("machine", "Filtres", nullptr));
        filtre_alerte_type->setItemText(0, QCoreApplication::translate("machine", "-- Type alerte --", nullptr));
        filtre_alerte_type->setItemText(1, QCoreApplication::translate("machine", "Temp\303\251rature", nullptr));
        filtre_alerte_type->setItemText(2, QCoreApplication::translate("machine", "Surcharge", nullptr));
        filtre_alerte_type->setItemText(3, QCoreApplication::translate("machine", "Panne", nullptr));

        filtre_alerte_criticite->setItemText(0, QCoreApplication::translate("machine", "-- Criticit\303\251 --", nullptr));
        filtre_alerte_criticite->setItemText(1, QCoreApplication::translate("machine", "Faible", nullptr));
        filtre_alerte_criticite->setItemText(2, QCoreApplication::translate("machine", "Moyen", nullptr));
        filtre_alerte_criticite->setItemText(3, QCoreApplication::translate("machine", "\303\211lev\303\251", nullptr));
        filtre_alerte_criticite->setItemText(4, QCoreApplication::translate("machine", "Critique", nullptr));

        lblDateDebut->setText(QCoreApplication::translate("machine", "Du:", nullptr));
        lblDateFin->setText(QCoreApplication::translate("machine", "Au:", nullptr));
        btnFiltrerAlertes->setText(QCoreApplication::translate("machine", "\360\237\224\215 Filtrer", nullptr));
        tabWidgetMachineManagement->setTabText(tabWidgetMachineManagement->indexOf(tabAlertes), QCoreApplication::translate("machine", "\342\232\240\357\270\217 Alertes", nullptr));
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
        tabWidgetMachineManagement->setTabText(tabWidgetMachineManagement->indexOf(tabStatistiques), QCoreApplication::translate("machine", "\360\237\223\212 Statistiques", nullptr));
        groupMaintenancePredictive->setTitle(QCoreApplication::translate("machine", "Maintenance pr\303\251dictive - Machines \303\240 surveiller", nullptr));
        btnEnregistrerMaintenance->setText(QCoreApplication::translate("machine", "\360\237\222\276 Enregistrer maintenance", nullptr));
        btnPlanifierMaintenance->setStyleSheet(QCoreApplication::translate("machine", "background-color: #C9A227;", nullptr));
        btnPlanifierMaintenance->setText(QCoreApplication::translate("machine", "\360\237\223\205 Planifier maintenance", nullptr));
        tabWidgetMachineManagement->setTabText(tabWidgetMachineManagement->indexOf(tabMaintenance), QCoreApplication::translate("machine", "\360\237\224\247 Maintenance", nullptr));
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
