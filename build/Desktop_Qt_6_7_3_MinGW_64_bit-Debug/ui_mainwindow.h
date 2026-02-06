/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.7.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QDate>
#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDateEdit>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStatusBar>
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
    QVBoxLayout *verticalLayout;
    QWidget *headerWidget;
    QHBoxLayout *horizontalLayout_header;
    QPushButton *pushButton_retour;
    QVBoxLayout *verticalLayout_title;
    QLabel *label_titre_principal;
    QLabel *label_titre_sous;
    QSpacerItem *horizontalSpacer_left;
    QWidget *dateTimeWidget;
    QHBoxLayout *dateTimeLayout;
    QLabel *calendarIconLabel;
    QLabel *dateTimeLabel;
    QLabel *label_en_ligne;
    QTabWidget *tabClient;
    QWidget *tab_gestionClient;
    QHBoxLayout *horizontalLayout_tab1;
    QGroupBox *groupBox_formulaire;
    QFormLayout *formLayout;
    QLabel *label_id;
    QLineEdit *lineEdit_id;
    QLabel *label_nom;
    QLineEdit *lineEdit_nom;
    QLabel *label_prenom;
    QLineEdit *lineEdit_prenom;
    QLabel *label_tel;
    QLineEdit *lineEdit_tel;
    QLabel *label_adresse;
    QLineEdit *lineEdit_adresse;
    QLabel *label_email;
    QLineEdit *lineEdit_email;
    QLabel *label_type;
    QComboBox *comboBox_type;
    QLabel *label_total;
    QLineEdit *lineEdit_total;
    QLabel *label_date;
    QDateEdit *dateEdit_creation;
    QLabel *label_statut_label;
    QLabel *label_statut;
    QSpacerItem *verticalSpacer_form;
    QGroupBox *groupBox_boutons;
    QGridLayout *gridLayout_boutons;
    QPushButton *pushButton_ajouter;
    QPushButton *pushButton_modifier;
    QPushButton *pushButton_supprimer;
    QPushButton *pushButton_consulter;
    QPushButton *pushButton_clear;
    QGroupBox *groupBox_tableau;
    QVBoxLayout *verticalLayout_tableau;
    QTableWidget *tableView_client;
    QWidget *tab_rechercheTri;
    QVBoxLayout *verticalLayout_tab2;
    QGroupBox *groupBox_recherche;
    QHBoxLayout *horizontalLayout_recherche;
    QLabel *label_critere;
    QComboBox *comboBox_critere;
    QLabel *label_valeur;
    QLineEdit *lineEdit_recherche;
    QPushButton *pushButton_rechercher;
    QGroupBox *groupBox_tri;
    QHBoxLayout *horizontalLayout_tri;
    QLabel *label_trier;
    QComboBox *comboBox_tri;
    QPushButton *pushButton_trier;
    QSpacerItem *horizontalSpacer_tri;
    QGroupBox *groupBox_export;
    QHBoxLayout *horizontalLayout_export;
    QPushButton *pushButton_pdf;
    QPushButton *pushButton_excel;
    QPushButton *pushButton_word;
    QPushButton *pushButton_txt;
    QSpacerItem *horizontalSpacer_export;
    QGroupBox *groupBox_resultats;
    QVBoxLayout *verticalLayout_resultats;
    QTableWidget *tableView_resultats;
    QSpacerItem *verticalSpacer_tab2;
    QWidget *tab_statistiques;
    QVBoxLayout *verticalLayout_tab3;
    QGroupBox *groupBox_statistiques;
    QHBoxLayout *horizontalLayout_stat;
    QLabel *label_stat_type;
    QComboBox *comboBox_stat;
    QPushButton *pushButton_stat;
    QSpacerItem *horizontalSpacer_stat;
    QGroupBox *groupBox_graph;
    QVBoxLayout *verticalLayout_graph;
    QLabel *label_chart_placeholder;
    QSpacerItem *verticalSpacer_tab3;
    QWidget *tab_fideliteRapport;
    QVBoxLayout *verticalLayout_tab4;
    QGroupBox *groupBox_classification;
    QGridLayout *gridLayout_classification;
    QLabel *label_score_title;
    QLabel *label_score_value;
    QLabel *label_statut_titre;
    QLabel *label_statut_value;
    QLabel *label_importance_titre;
    QLabel *label_importance_value;
    QGroupBox *groupBox_rapport;
    QVBoxLayout *verticalLayout_rapport;
    QTextEdit *textEdit_rapport;
    QHBoxLayout *horizontalLayout_rapport_boutons;
    QPushButton *pushButton_rapport;
    QPushButton *pushButton_exportRapport;
    QSpacerItem *horizontalSpacer_rapport;
    QSpacerItem *verticalSpacer_tab4;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(1441, 616);
        MainWindow->setMinimumSize(QSize(1200, 550));
        MainWindow->setStyleSheet(QString::fromUtf8("/* \342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\n"
"   GESTION CLIENTS - PROFESSIONAL DASHBOARD THEME\n"
"   Version 2.0 - Modern Industrial Design (Style from Production)\n"
"\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225"
                        "\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220 */\n"
"\n"
"QMainWindow {\n"
"    background: qlineargradient(x1:0, y1:0, x2:1, y2:1, \n"
"                stop:0 #f8f9fa, stop:0.5 #e9ecef, stop:1 #f8f9fa);\n"
"}\n"
"\n"
"/* \342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342"
                        "\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\n"
"   HEADER SECTION\n"
"\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224"
                        "\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200 */\n"
"#headerWidget {\n"
"    background: qlineargradient(x1:0, y1:0, x2:1, y2:0, \n"
"                stop:0 #1B4332, stop:0.5 #2D5A45, stop:1 #1B4332);\n"
"    border-bottom: 4px solid #C9A227;\n"
"}\n"
"\n"
"#label_titre_principal {\n"
"    color: white;\n"
"    font-size: 36px;\n"
"    font-weight: 800;\n"
"    font-family: 'Segoe UI', 'Arial Black', sans-serif;\n"
"    letter-spacing: 2px;\n"
"}\n"
"\n"
"#label_titre_sous {\n"
"    color: #C9A227;\n"
"    font-size: 13px;\n"
"    font-weight: 600;\n"
"    font-family: 'Segoe UI', Arial, sans-serif;\n"
"    letter-spacing: 6px;\n"
"}\n"
"\n"
"#label_en_ligne {\n"
"    color: #4ADE80;\n"
"    font-size: 14px;\n"
"    font-weight: bol"
                        "d;\n"
"    font-family: 'Segoe UI', Arial, sans-serif;\n"
"    padding: 8px 16px;\n"
"    background-color: rgba(74, 222, 128, 0.15);\n"
"    border-radius: 20px;\n"
"    border: 1px solid rgba(74, 222, 128, 0.3);\n"
"}\n"
"\n"
"#pushButton_retour {\n"
"    background-color: rgba(255, 255, 255, 0.15);\n"
"    color: white;\n"
"    font-size: 13px;\n"
"    font-weight: 600;\n"
"    font-family: 'Segoe UI', Arial, sans-serif;\n"
"    padding: 10px 20px;\n"
"    border-radius: 8px;\n"
"    border: 1px solid rgba(255, 255, 255, 0.3);\n"
"    min-width: 100px;\n"
"}\n"
"\n"
"#pushButton_retour:hover {\n"
"    background-color: rgba(255, 255, 255, 0.25);\n"
"    border: 1px solid rgba(255, 255, 255, 0.5);\n"
"}\n"
"\n"
"#pushButton_retour:pressed {\n"
"    background-color: rgba(255, 255, 255, 0.1);\n"
"}\n"
"\n"
"/* \342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200"
                        "\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\n"
"   TAB WIDGET - MODERN DESIGN\n"
"\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342"
                        "\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200 */\n"
"QTabWidget::pane {\n"
"    border: none;\n"
"    background-color: transparent;\n"
"    border-radius: 12px;\n"
"    margin-top: 5px;\n"
"}\n"
"\n"
"QTabBar {\n"
"    background: transparent;\n"
"}\n"
"\n"
"QTabBar::tab {\n"
"    background-color: white;\n"
"    color: #666;\n"
"    padding: 12px 24px;\n"
"    margin-right: 4px;\n"
"    border: none;\n"
"    border-radius: 8px 8px 0 0;\n"
"    font-size: 13px;\n"
"    font-weight: 600;\n"
"    font-family: 'Segoe UI', Arial, sans-serif;\n"
"    min-width: 120px;\n"
"}\n"
"\n"
"QTabBar::tab:selected {\n"
"    background-color: #1B4332;\n"
"    color: white;\n"
"    font-weight: 700;\n"
"}\n"
""
                        "\n"
"QTabBar::tab:hover:!selected {\n"
"    background-color: #f0f0f0;\n"
"    color: #1B4332;\n"
"}\n"
"\n"
"/* \342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\n"
"   GROUP BOX - CARD STYLE (Statistics Cards Style)\n"
"\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200"
                        "\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200 */\n"
"QGroupBox {\n"
"    font-size: 14px;\n"
"    font-weight: 700;\n"
"    font-family: 'Segoe UI', Arial, sans-serif;\n"
"    color: #1B4332;\n"
"    background-color: white;\n"
"    border: 1px solid #e0e0e0;\n"
"    border-radius: 12px;\n"
"    margin-top: 20px;\n"
"    padding: 20px 15px 15px 15px;\n"
"}\n"
""
                        "\n"
"QGroupBox:hover {\n"
"    border: 1px solid #1B4332;\n"
"}\n"
"\n"
"QGroupBox::title {\n"
"    subcontrol-origin: margin;\n"
"    subcontrol-position: top left;\n"
"    left: 20px;\n"
"    top: 0px;\n"
"    padding: 8px 16px;\n"
"    background-color: #1B4332;\n"
"    color: white;\n"
"    border-radius: 8px;\n"
"    font-size: 13px;\n"
"}\n"
"\n"
"/* \342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224"
                        "\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\n"
"   INPUT FIELDS - MODERN STYLE\n"
"\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200 */\n"
"QLineEdit, QSpinBox, QDoubleSpinBox, QDateEdit, QComboBox {\n"
"    padding: 10px 14p"
                        "x;\n"
"    border: 2px solid #e0e0e0;\n"
"    border-radius: 8px;\n"
"    background-color: #fafafa;\n"
"    color: #333;\n"
"    font-size: 14px;\n"
"    font-family: 'Segoe UI', Arial, sans-serif;\n"
"    min-height: 20px;\n"
"    selection-background-color: #1B4332;\n"
"}\n"
"\n"
"QLineEdit:focus, QSpinBox:focus, QDoubleSpinBox:focus, QDateEdit:focus, QComboBox:focus {\n"
"    border: 2px solid #1B4332;\n"
"    background-color: white;\n"
"}\n"
"\n"
"QLineEdit:hover, QSpinBox:hover, QDoubleSpinBox:hover, QDateEdit:hover, QComboBox:hover {\n"
"    border: 2px solid #4ADE80;\n"
"}\n"
"\n"
"QComboBox::drop-down {\n"
"    border: none;\n"
"    padding-right: 15px;\n"
"    width: 30px;\n"
"}\n"
"\n"
"QComboBox QAbstractItemView {\n"
"    border: 2px solid #1B4332;\n"
"    border-radius: 8px;\n"
"    background-color: white;\n"
"    selection-background-color: #1B4332;\n"
"    selection-color: white;\n"
"    padding: 5px;\n"
"}\n"
"\n"
"/* \342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342"
                        "\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\n"
"   TEXT EDIT - MODERN STYLE\n"
"\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224"
                        "\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200 */\n"
"QTextEdit, QTextBrowser {\n"
"    border: 2px solid #e0e0e0;\n"
"    border-radius: 8px;\n"
"    background-color: #fafafa;\n"
"    color: #333;\n"
"    font-size: 14px;\n"
"    font-family: 'Segoe UI', Arial, sans-serif;\n"
"    padding: 12px;\n"
"}\n"
"\n"
"QTextEdit:focus, QTextBrowser:focus {\n"
"    border: 2px solid #1B4332;\n"
"    background-color: white;\n"
"}\n"
"\n"
"/* \342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224"
                        "\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\n"
"   BUTTONS - PROFESSIONAL STYLE (Module Card Button Style)\n"
"\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224"
                        "\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200 */\n"
"QPushButton {\n"
"    padding: 12px 24px;\n"
"    border-radius: 8px;\n"
"    font-size: 14px;\n"
"    font-weight: 600;\n"
"    font-family: 'Segoe UI', Arial, sans-serif;\n"
"    min-width: 120px;\n"
"    border: none;\n"
"}\n"
"\n"
"/* Primary Buttons */\n"
"#pushButton_ajouter, \n"
"#pushButton_modifier, \n"
"#pushButton_consulter, \n"
"#pushButton_rechercher,\n"
"#pushButton_trier,\n"
"#pushButton_stat,\n"
"#pushButton_rapport {\n"
"    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, \n"
"                stop:0 #234E3E, stop:1 #1B4332);\n"
" "
                        "   color: white;\n"
"    border: none;\n"
"}\n"
"\n"
"#pushButton_ajouter:hover, \n"
"#pushButton_modifier:hover, \n"
"#pushButton_consulter:hover, \n"
"#pushButton_rechercher:hover,\n"
"#pushButton_trier:hover,\n"
"#pushButton_stat:hover,\n"
"#pushButton_rapport:hover {\n"
"    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, \n"
"                stop:0 #2D5A45, stop:1 #234E3E);\n"
"}\n"
"\n"
"#pushButton_ajouter:pressed, \n"
"#pushButton_modifier:pressed, \n"
"#pushButton_consulter:pressed, \n"
"#pushButton_rechercher:pressed,\n"
"#pushButton_trier:pressed,\n"
"#pushButton_stat:pressed,\n"
"#pushButton_rapport:pressed {\n"
"    background: #0F2A1F;\n"
"}\n"
"\n"
"/* Danger Button */\n"
"#pushButton_supprimer {\n"
"    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, \n"
"                stop:0 #EF4444, stop:1 #DC2626);\n"
"    color: white;\n"
"}\n"
"\n"
"#pushButton_supprimer:hover {\n"
"    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, \n"
"                stop:0 #F87171, stop:1 #EF4444);\n"
"}"
                        "\n"
"\n"
"#pushButton_supprimer:pressed {\n"
"    background: #B91C1C;\n"
"}\n"
"\n"
"/* Export Buttons */\n"
"#pushButton_pdf, #pushButton_exportRapport {\n"
"    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, \n"
"                stop:0 #E53935, stop:1 #C62828);\n"
"    color: white;\n"
"}\n"
"\n"
"#pushButton_pdf:hover, #pushButton_exportRapport:hover {\n"
"    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, \n"
"                stop:0 #EF5350, stop:1 #E53935);\n"
"}\n"
"\n"
"#pushButton_excel {\n"
"    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, \n"
"                stop:0 #2E7D32, stop:1 #1B5E20);\n"
"    color: white;\n"
"}\n"
"\n"
"#pushButton_excel:hover {\n"
"    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, \n"
"                stop:0 #43A047, stop:1 #2E7D32);\n"
"}\n"
"\n"
"#pushButton_word {\n"
"    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, \n"
"                stop:0 #1976D2, stop:1 #1565C0);\n"
"    color: white;\n"
"}\n"
"\n"
"#pushButton_word:hover {\n"
"    bac"
                        "kground: qlineargradient(x1:0, y1:0, x2:0, y2:1, \n"
"                stop:0 #1E88E5, stop:1 #1976D2);\n"
"}\n"
"\n"
"#pushButton_txt {\n"
"    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, \n"
"                stop:0 #455A64, stop:1 #37474F);\n"
"    color: white;\n"
"}\n"
"\n"
"#pushButton_txt:hover {\n"
"    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, \n"
"                stop:0 #546E7A, stop:1 #455A64);\n"
"}\n"
"\n"
"/* Clear/Reset Button */\n"
"#pushButton_clear {\n"
"    background-color: #6B7280;\n"
"    color: white;\n"
"}\n"
"\n"
"#pushButton_clear:hover {\n"
"    background-color: #4B5563;\n"
"}\n"
"\n"
"/* \342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342"
                        "\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\n"
"   PROGRESS BAR - MODERN ANIMATED STYLE\n"
"\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224"
                        "\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200 */\n"
"QProgressBar {\n"
"    border: none;\n"
"    border-radius: 10px;\n"
"    text-align: center;\n"
"    font-weight: 700;\n"
"    font-size: 13px;\n"
"    background-color: #e5e7eb;\n"
"    min-height: 24px;\n"
"    color: white;\n"
"}\n"
"\n"
"QProgressBar::chunk {\n"
"    background: qlineargradient(x1:0, y1:0, x2:1, y2:0, \n"
"                stop:0 #1B4332, stop:0.5 #234E3E, stop:1 #1B4332);\n"
"    border-radius: 10px;\n"
"}\n"
"\n"
"/* \342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224"
                        "\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\n"
"   TABLE WIDGET - PROFESSIONAL STYLE\n"
"\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342"
                        "\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200 */\n"
"QTableWidget {\n"
"    border: none;\n"
"    border-radius: 8px;\n"
"    background-color: white;\n"
"    gridline-color: #f0f0f0;\n"
"    font-size: 13px;\n"
"    font-family: 'Segoe UI', Arial, sans-serif;\n"
"}\n"
"\n"
"QTableWidget::item {\n"
"    padding: 12px;\n"
"    border-bottom: 1px solid #f0f0f0;\n"
"}\n"
"\n"
"QTableWidget::item:selected {\n"
"    background-color: rgba(27, 67, 50, 0.1);\n"
"    color: #1B4332;\n"
"}\n"
"\n"
"QTableWidget::item:hover {\n"
"    background-color: rgba(27, 67, 50, 0.05);\n"
"}\n"
"\n"
"QHeaderView::section {\n"
"    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, \n"
"                stop:0 #1B4332, stop:1 #163829);\n"
"    color: white;\n"
"    padding: 14px 12px;\n"
"    border: none;\n"
"    font-weight: 700;\n"
"    font-size: 12px;\n"
"    text-transform:"
                        " uppercase;\n"
"    letter-spacing: 1px;\n"
"}\n"
"\n"
"/* \342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\n"
"   LABELS - TYPOGRAPHY\n"
"\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200"
                        "\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200 */\n"
"QLabel {\n"
"    color: #333;\n"
"    font-size: 14px;\n"
"    font-family: 'Segoe UI', Arial, sans-serif;\n"
"}\n"
"\n"
"/* \342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342"
                        "\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\n"
"   CHECKBOX - MODERN STYLE\n"
"\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200"
                        "\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200 */\n"
"QCheckBox {\n"
"    font-size: 14px;\n"
"    font-family: 'Segoe UI', Arial, sans-serif;\n"
"    color: #374151;\n"
"    spacing: 12px;\n"
"    padding: 8px;\n"
"}\n"
"\n"
"QCheckBox::indicator {\n"
"    width: 22px;\n"
"    height: 22px;\n"
"    border-radius: 6px;\n"
"}\n"
"\n"
"QCheckBox::indicator:unchecked {\n"
"    background-color: #fafafa;\n"
"    border: 2px solid #d1d5db;\n"
"}\n"
"\n"
"QCheckBox::indicator:unchecked:hover {\n"
"    border: 2px solid #1B4332;\n"
"}\n"
"\n"
"QCheckBox::indicator:checked {\n"
"    background-color: #1B4332;\n"
"    border: 2px solid #1B4332;\n"
"}\n"
"\n"
"/* \342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342"
                        "\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\n"
"   SCROLLBAR - MINIMAL STYLE\n"
"\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224"
                        "\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200 */\n"
"QScrollBar:vertical {\n"
"    border: none;\n"
"    background: #f0f0f0;\n"
"    width: 10px;\n"
"    border-radius: 5px;\n"
"    margin: 0;\n"
"}\n"
"\n"
"QScrollBar::handle:vertical {\n"
"    background: #c0c0c0;\n"
"    border-radius: 5px;\n"
"    min-height: 30px;\n"
"}\n"
"\n"
"QScrollBar::handle:vertical:hover {\n"
"    background: #1B4332;\n"
"}\n"
"\n"
"QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {\n"
"    height: 0;\n"
"}\n"
"\n"
"QScrollBar:horizontal {\n"
"    border: none;\n"
"    background: #f0f0f0;\n"
"    height: 10px;"
                        "\n"
"    border-radius: 5px;\n"
"}\n"
"\n"
"QScrollBar::handle:horizontal {\n"
"    background: #c0c0c0;\n"
"    border-radius: 5px;\n"
"    min-width: 30px;\n"
"}\n"
"\n"
"QScrollBar::handle:horizontal:hover {\n"
"    background: #1B4332;\n"
"}\n"
"   "));
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        verticalLayout = new QVBoxLayout(centralwidget);
        verticalLayout->setObjectName("verticalLayout");
        headerWidget = new QWidget(centralwidget);
        headerWidget->setObjectName("headerWidget");
        headerWidget->setMinimumHeight(65);
        headerWidget->setMaximumHeight(65);
        horizontalLayout_header = new QHBoxLayout(headerWidget);
        horizontalLayout_header->setSpacing(20);
        horizontalLayout_header->setObjectName("horizontalLayout_header");
        horizontalLayout_header->setContentsMargins(25, 10, 25, 10);
        pushButton_retour = new QPushButton(headerWidget);
        pushButton_retour->setObjectName("pushButton_retour");
        pushButton_retour->setMaximumWidth(150);

        horizontalLayout_header->addWidget(pushButton_retour);

        verticalLayout_title = new QVBoxLayout();
        verticalLayout_title->setSpacing(2);
        verticalLayout_title->setObjectName("verticalLayout_title");
        label_titre_principal = new QLabel(headerWidget);
        label_titre_principal->setObjectName("label_titre_principal");
        label_titre_principal->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);

        verticalLayout_title->addWidget(label_titre_principal);

        label_titre_sous = new QLabel(headerWidget);
        label_titre_sous->setObjectName("label_titre_sous");
        label_titre_sous->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);

        verticalLayout_title->addWidget(label_titre_sous);


        horizontalLayout_header->addLayout(verticalLayout_title);

        horizontalSpacer_left = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_header->addItem(horizontalSpacer_left);

        dateTimeWidget = new QWidget(headerWidget);
        dateTimeWidget->setObjectName("dateTimeWidget");
        dateTimeWidget->setMinimumSize(QSize(220, 40));
        dateTimeWidget->setMaximumSize(QSize(220, 40));
        dateTimeWidget->setStyleSheet(QString::fromUtf8("#dateTimeWidget {\n"
"    background-color: #2C5F4F;\n"
"    border-radius: 8px;\n"
"    padding: 6px 12px;\n"
"}\n"
"\n"
"#calendarIconLabel {\n"
"    background-color: white;\n"
"    border-radius: 4px;\n"
"    padding: 4px 6px;\n"
"    font-size: 14px;\n"
"    font-weight: 800;\n"
"    color: #0056b3;\n"
"    min-width: 24px;\n"
"    max-width: 24px;\n"
"    min-height: 24px;\n"
"    max-height: 24px;\n"
"}\n"
"\n"
"#dateTimeLabel {\n"
"    color: white;\n"
"    font-size: 12px;\n"
"    font-weight: 600;\n"
"    font-family: 'Segoe UI', Arial, sans-serif;\n"
"}"));
        dateTimeLayout = new QHBoxLayout(dateTimeWidget);
        dateTimeLayout->setSpacing(8);
        dateTimeLayout->setObjectName("dateTimeLayout");
        dateTimeLayout->setContentsMargins(8, 6, 8, 6);
        calendarIconLabel = new QLabel(dateTimeWidget);
        calendarIconLabel->setObjectName("calendarIconLabel");
        calendarIconLabel->setAlignment(Qt::AlignCenter);

        dateTimeLayout->addWidget(calendarIconLabel);

        dateTimeLabel = new QLabel(dateTimeWidget);
        dateTimeLabel->setObjectName("dateTimeLabel");

        dateTimeLayout->addWidget(dateTimeLabel);


        horizontalLayout_header->addWidget(dateTimeWidget);

        label_en_ligne = new QLabel(headerWidget);
        label_en_ligne->setObjectName("label_en_ligne");

        horizontalLayout_header->addWidget(label_en_ligne);


        verticalLayout->addWidget(headerWidget);

        tabClient = new QTabWidget(centralwidget);
        tabClient->setObjectName("tabClient");
        tab_gestionClient = new QWidget();
        tab_gestionClient->setObjectName("tab_gestionClient");
        horizontalLayout_tab1 = new QHBoxLayout(tab_gestionClient);
        horizontalLayout_tab1->setObjectName("horizontalLayout_tab1");
        groupBox_formulaire = new QGroupBox(tab_gestionClient);
        groupBox_formulaire->setObjectName("groupBox_formulaire");
        formLayout = new QFormLayout(groupBox_formulaire);
        formLayout->setObjectName("formLayout");
        label_id = new QLabel(groupBox_formulaire);
        label_id->setObjectName("label_id");

        formLayout->setWidget(0, QFormLayout::LabelRole, label_id);

        lineEdit_id = new QLineEdit(groupBox_formulaire);
        lineEdit_id->setObjectName("lineEdit_id");
        lineEdit_id->setReadOnly(true);

        formLayout->setWidget(0, QFormLayout::FieldRole, lineEdit_id);

        label_nom = new QLabel(groupBox_formulaire);
        label_nom->setObjectName("label_nom");

        formLayout->setWidget(1, QFormLayout::LabelRole, label_nom);

        lineEdit_nom = new QLineEdit(groupBox_formulaire);
        lineEdit_nom->setObjectName("lineEdit_nom");

        formLayout->setWidget(1, QFormLayout::FieldRole, lineEdit_nom);

        label_prenom = new QLabel(groupBox_formulaire);
        label_prenom->setObjectName("label_prenom");

        formLayout->setWidget(2, QFormLayout::LabelRole, label_prenom);

        lineEdit_prenom = new QLineEdit(groupBox_formulaire);
        lineEdit_prenom->setObjectName("lineEdit_prenom");

        formLayout->setWidget(2, QFormLayout::FieldRole, lineEdit_prenom);

        label_tel = new QLabel(groupBox_formulaire);
        label_tel->setObjectName("label_tel");

        formLayout->setWidget(3, QFormLayout::LabelRole, label_tel);

        lineEdit_tel = new QLineEdit(groupBox_formulaire);
        lineEdit_tel->setObjectName("lineEdit_tel");

        formLayout->setWidget(3, QFormLayout::FieldRole, lineEdit_tel);

        label_adresse = new QLabel(groupBox_formulaire);
        label_adresse->setObjectName("label_adresse");

        formLayout->setWidget(4, QFormLayout::LabelRole, label_adresse);

        lineEdit_adresse = new QLineEdit(groupBox_formulaire);
        lineEdit_adresse->setObjectName("lineEdit_adresse");

        formLayout->setWidget(4, QFormLayout::FieldRole, lineEdit_adresse);

        label_email = new QLabel(groupBox_formulaire);
        label_email->setObjectName("label_email");

        formLayout->setWidget(5, QFormLayout::LabelRole, label_email);

        lineEdit_email = new QLineEdit(groupBox_formulaire);
        lineEdit_email->setObjectName("lineEdit_email");

        formLayout->setWidget(5, QFormLayout::FieldRole, lineEdit_email);

        label_type = new QLabel(groupBox_formulaire);
        label_type->setObjectName("label_type");

        formLayout->setWidget(6, QFormLayout::LabelRole, label_type);

        comboBox_type = new QComboBox(groupBox_formulaire);
        comboBox_type->addItem(QString());
        comboBox_type->addItem(QString());
        comboBox_type->setObjectName("comboBox_type");

        formLayout->setWidget(6, QFormLayout::FieldRole, comboBox_type);

        label_total = new QLabel(groupBox_formulaire);
        label_total->setObjectName("label_total");

        formLayout->setWidget(7, QFormLayout::LabelRole, label_total);

        lineEdit_total = new QLineEdit(groupBox_formulaire);
        lineEdit_total->setObjectName("lineEdit_total");

        formLayout->setWidget(7, QFormLayout::FieldRole, lineEdit_total);

        label_date = new QLabel(groupBox_formulaire);
        label_date->setObjectName("label_date");

        formLayout->setWidget(8, QFormLayout::LabelRole, label_date);

        dateEdit_creation = new QDateEdit(groupBox_formulaire);
        dateEdit_creation->setObjectName("dateEdit_creation");
        dateEdit_creation->setCalendarPopup(true);
        dateEdit_creation->setDate(QDate(2026, 2, 1));

        formLayout->setWidget(8, QFormLayout::FieldRole, dateEdit_creation);

        label_statut_label = new QLabel(groupBox_formulaire);
        label_statut_label->setObjectName("label_statut_label");

        formLayout->setWidget(9, QFormLayout::LabelRole, label_statut_label);

        label_statut = new QLabel(groupBox_formulaire);
        label_statut->setObjectName("label_statut");

        formLayout->setWidget(9, QFormLayout::FieldRole, label_statut);

        verticalSpacer_form = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        formLayout->setItem(10, QFormLayout::SpanningRole, verticalSpacer_form);

        groupBox_boutons = new QGroupBox(groupBox_formulaire);
        groupBox_boutons->setObjectName("groupBox_boutons");
        gridLayout_boutons = new QGridLayout(groupBox_boutons);
        gridLayout_boutons->setObjectName("gridLayout_boutons");
        pushButton_ajouter = new QPushButton(groupBox_boutons);
        pushButton_ajouter->setObjectName("pushButton_ajouter");

        gridLayout_boutons->addWidget(pushButton_ajouter, 0, 0, 1, 1);

        pushButton_modifier = new QPushButton(groupBox_boutons);
        pushButton_modifier->setObjectName("pushButton_modifier");

        gridLayout_boutons->addWidget(pushButton_modifier, 0, 1, 1, 1);

        pushButton_supprimer = new QPushButton(groupBox_boutons);
        pushButton_supprimer->setObjectName("pushButton_supprimer");

        gridLayout_boutons->addWidget(pushButton_supprimer, 1, 0, 1, 1);

        pushButton_consulter = new QPushButton(groupBox_boutons);
        pushButton_consulter->setObjectName("pushButton_consulter");

        gridLayout_boutons->addWidget(pushButton_consulter, 1, 1, 1, 1);

        pushButton_clear = new QPushButton(groupBox_boutons);
        pushButton_clear->setObjectName("pushButton_clear");

        gridLayout_boutons->addWidget(pushButton_clear, 2, 0, 1, 2);


        formLayout->setWidget(11, QFormLayout::SpanningRole, groupBox_boutons);


        horizontalLayout_tab1->addWidget(groupBox_formulaire);

        groupBox_tableau = new QGroupBox(tab_gestionClient);
        groupBox_tableau->setObjectName("groupBox_tableau");
        verticalLayout_tableau = new QVBoxLayout(groupBox_tableau);
        verticalLayout_tableau->setObjectName("verticalLayout_tableau");
        tableView_client = new QTableWidget(groupBox_tableau);
        tableView_client->setObjectName("tableView_client");
        tableView_client->setAlternatingRowColors(true);

        verticalLayout_tableau->addWidget(tableView_client);


        horizontalLayout_tab1->addWidget(groupBox_tableau);

        tabClient->addTab(tab_gestionClient, QString());
        tab_rechercheTri = new QWidget();
        tab_rechercheTri->setObjectName("tab_rechercheTri");
        verticalLayout_tab2 = new QVBoxLayout(tab_rechercheTri);
        verticalLayout_tab2->setObjectName("verticalLayout_tab2");
        groupBox_recherche = new QGroupBox(tab_rechercheTri);
        groupBox_recherche->setObjectName("groupBox_recherche");
        horizontalLayout_recherche = new QHBoxLayout(groupBox_recherche);
        horizontalLayout_recherche->setObjectName("horizontalLayout_recherche");
        label_critere = new QLabel(groupBox_recherche);
        label_critere->setObjectName("label_critere");

        horizontalLayout_recherche->addWidget(label_critere);

        comboBox_critere = new QComboBox(groupBox_recherche);
        comboBox_critere->addItem(QString());
        comboBox_critere->addItem(QString());
        comboBox_critere->addItem(QString());
        comboBox_critere->addItem(QString());
        comboBox_critere->addItem(QString());
        comboBox_critere->setObjectName("comboBox_critere");

        horizontalLayout_recherche->addWidget(comboBox_critere);

        label_valeur = new QLabel(groupBox_recherche);
        label_valeur->setObjectName("label_valeur");

        horizontalLayout_recherche->addWidget(label_valeur);

        lineEdit_recherche = new QLineEdit(groupBox_recherche);
        lineEdit_recherche->setObjectName("lineEdit_recherche");

        horizontalLayout_recherche->addWidget(lineEdit_recherche);

        pushButton_rechercher = new QPushButton(groupBox_recherche);
        pushButton_rechercher->setObjectName("pushButton_rechercher");

        horizontalLayout_recherche->addWidget(pushButton_rechercher);


        verticalLayout_tab2->addWidget(groupBox_recherche);

        groupBox_tri = new QGroupBox(tab_rechercheTri);
        groupBox_tri->setObjectName("groupBox_tri");
        horizontalLayout_tri = new QHBoxLayout(groupBox_tri);
        horizontalLayout_tri->setObjectName("horizontalLayout_tri");
        label_trier = new QLabel(groupBox_tri);
        label_trier->setObjectName("label_trier");

        horizontalLayout_tri->addWidget(label_trier);

        comboBox_tri = new QComboBox(groupBox_tri);
        comboBox_tri->addItem(QString());
        comboBox_tri->addItem(QString());
        comboBox_tri->addItem(QString());
        comboBox_tri->addItem(QString());
        comboBox_tri->addItem(QString());
        comboBox_tri->setObjectName("comboBox_tri");

        horizontalLayout_tri->addWidget(comboBox_tri);

        pushButton_trier = new QPushButton(groupBox_tri);
        pushButton_trier->setObjectName("pushButton_trier");

        horizontalLayout_tri->addWidget(pushButton_trier);

        horizontalSpacer_tri = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_tri->addItem(horizontalSpacer_tri);


        verticalLayout_tab2->addWidget(groupBox_tri);

        groupBox_export = new QGroupBox(tab_rechercheTri);
        groupBox_export->setObjectName("groupBox_export");
        horizontalLayout_export = new QHBoxLayout(groupBox_export);
        horizontalLayout_export->setObjectName("horizontalLayout_export");
        pushButton_pdf = new QPushButton(groupBox_export);
        pushButton_pdf->setObjectName("pushButton_pdf");

        horizontalLayout_export->addWidget(pushButton_pdf);

        pushButton_excel = new QPushButton(groupBox_export);
        pushButton_excel->setObjectName("pushButton_excel");

        horizontalLayout_export->addWidget(pushButton_excel);

        pushButton_word = new QPushButton(groupBox_export);
        pushButton_word->setObjectName("pushButton_word");

        horizontalLayout_export->addWidget(pushButton_word);

        pushButton_txt = new QPushButton(groupBox_export);
        pushButton_txt->setObjectName("pushButton_txt");

        horizontalLayout_export->addWidget(pushButton_txt);

        horizontalSpacer_export = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_export->addItem(horizontalSpacer_export);


        verticalLayout_tab2->addWidget(groupBox_export);

        groupBox_resultats = new QGroupBox(tab_rechercheTri);
        groupBox_resultats->setObjectName("groupBox_resultats");
        verticalLayout_resultats = new QVBoxLayout(groupBox_resultats);
        verticalLayout_resultats->setObjectName("verticalLayout_resultats");
        tableView_resultats = new QTableWidget(groupBox_resultats);
        tableView_resultats->setObjectName("tableView_resultats");
        tableView_resultats->setAlternatingRowColors(true);

        verticalLayout_resultats->addWidget(tableView_resultats);


        verticalLayout_tab2->addWidget(groupBox_resultats);

        verticalSpacer_tab2 = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout_tab2->addItem(verticalSpacer_tab2);

        tabClient->addTab(tab_rechercheTri, QString());
        tab_statistiques = new QWidget();
        tab_statistiques->setObjectName("tab_statistiques");
        verticalLayout_tab3 = new QVBoxLayout(tab_statistiques);
        verticalLayout_tab3->setObjectName("verticalLayout_tab3");
        groupBox_statistiques = new QGroupBox(tab_statistiques);
        groupBox_statistiques->setObjectName("groupBox_statistiques");
        horizontalLayout_stat = new QHBoxLayout(groupBox_statistiques);
        horizontalLayout_stat->setObjectName("horizontalLayout_stat");
        label_stat_type = new QLabel(groupBox_statistiques);
        label_stat_type->setObjectName("label_stat_type");

        horizontalLayout_stat->addWidget(label_stat_type);

        comboBox_stat = new QComboBox(groupBox_statistiques);
        comboBox_stat->addItem(QString());
        comboBox_stat->addItem(QString());
        comboBox_stat->addItem(QString());
        comboBox_stat->addItem(QString());
        comboBox_stat->addItem(QString());
        comboBox_stat->setObjectName("comboBox_stat");

        horizontalLayout_stat->addWidget(comboBox_stat);

        pushButton_stat = new QPushButton(groupBox_statistiques);
        pushButton_stat->setObjectName("pushButton_stat");

        horizontalLayout_stat->addWidget(pushButton_stat);

        horizontalSpacer_stat = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_stat->addItem(horizontalSpacer_stat);


        verticalLayout_tab3->addWidget(groupBox_statistiques);

        groupBox_graph = new QGroupBox(tab_statistiques);
        groupBox_graph->setObjectName("groupBox_graph");
        verticalLayout_graph = new QVBoxLayout(groupBox_graph);
        verticalLayout_graph->setObjectName("verticalLayout_graph");
        label_chart_placeholder = new QLabel(groupBox_graph);
        label_chart_placeholder->setObjectName("label_chart_placeholder");
        label_chart_placeholder->setAlignment(Qt::AlignmentFlag::AlignCenter);

        verticalLayout_graph->addWidget(label_chart_placeholder);


        verticalLayout_tab3->addWidget(groupBox_graph);

        verticalSpacer_tab3 = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout_tab3->addItem(verticalSpacer_tab3);

        tabClient->addTab(tab_statistiques, QString());
        tab_fideliteRapport = new QWidget();
        tab_fideliteRapport->setObjectName("tab_fideliteRapport");
        verticalLayout_tab4 = new QVBoxLayout(tab_fideliteRapport);
        verticalLayout_tab4->setObjectName("verticalLayout_tab4");
        groupBox_classification = new QGroupBox(tab_fideliteRapport);
        groupBox_classification->setObjectName("groupBox_classification");
        gridLayout_classification = new QGridLayout(groupBox_classification);
        gridLayout_classification->setObjectName("gridLayout_classification");
        label_score_title = new QLabel(groupBox_classification);
        label_score_title->setObjectName("label_score_title");

        gridLayout_classification->addWidget(label_score_title, 0, 0, 1, 1);

        label_score_value = new QLabel(groupBox_classification);
        label_score_value->setObjectName("label_score_value");
        label_score_value->setAlignment(Qt::AlignmentFlag::AlignCenter);

        gridLayout_classification->addWidget(label_score_value, 0, 1, 1, 1);

        label_statut_titre = new QLabel(groupBox_classification);
        label_statut_titre->setObjectName("label_statut_titre");

        gridLayout_classification->addWidget(label_statut_titre, 1, 0, 1, 1);

        label_statut_value = new QLabel(groupBox_classification);
        label_statut_value->setObjectName("label_statut_value");
        label_statut_value->setAlignment(Qt::AlignmentFlag::AlignCenter);

        gridLayout_classification->addWidget(label_statut_value, 1, 1, 1, 1);

        label_importance_titre = new QLabel(groupBox_classification);
        label_importance_titre->setObjectName("label_importance_titre");

        gridLayout_classification->addWidget(label_importance_titre, 2, 0, 1, 1);

        label_importance_value = new QLabel(groupBox_classification);
        label_importance_value->setObjectName("label_importance_value");
        label_importance_value->setAlignment(Qt::AlignmentFlag::AlignCenter);

        gridLayout_classification->addWidget(label_importance_value, 2, 1, 1, 1);


        verticalLayout_tab4->addWidget(groupBox_classification);

        groupBox_rapport = new QGroupBox(tab_fideliteRapport);
        groupBox_rapport->setObjectName("groupBox_rapport");
        verticalLayout_rapport = new QVBoxLayout(groupBox_rapport);
        verticalLayout_rapport->setObjectName("verticalLayout_rapport");
        textEdit_rapport = new QTextEdit(groupBox_rapport);
        textEdit_rapport->setObjectName("textEdit_rapport");
        textEdit_rapport->setReadOnly(true);

        verticalLayout_rapport->addWidget(textEdit_rapport);

        horizontalLayout_rapport_boutons = new QHBoxLayout();
        horizontalLayout_rapport_boutons->setObjectName("horizontalLayout_rapport_boutons");
        pushButton_rapport = new QPushButton(groupBox_rapport);
        pushButton_rapport->setObjectName("pushButton_rapport");

        horizontalLayout_rapport_boutons->addWidget(pushButton_rapport);

        pushButton_exportRapport = new QPushButton(groupBox_rapport);
        pushButton_exportRapport->setObjectName("pushButton_exportRapport");

        horizontalLayout_rapport_boutons->addWidget(pushButton_exportRapport);

        horizontalSpacer_rapport = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_rapport_boutons->addItem(horizontalSpacer_rapport);


        verticalLayout_rapport->addLayout(horizontalLayout_rapport_boutons);


        verticalLayout_tab4->addWidget(groupBox_rapport);

        verticalSpacer_tab4 = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout_tab4->addItem(verticalSpacer_tab4);

        tabClient->addTab(tab_fideliteRapport, QString());

        verticalLayout->addWidget(tabClient);

        MainWindow->setCentralWidget(centralwidget);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        tabClient->setCurrentIndex(2);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "Gestion des Clients - Olive Livery", nullptr));
        headerWidget->setStyleSheet(QString());
        pushButton_retour->setText(QCoreApplication::translate("MainWindow", "\342\206\220 Retour au menu", nullptr));
        label_titre_principal->setText(QCoreApplication::translate("MainWindow", "GESTION DES CLIENTS", nullptr));
        label_titre_sous->setText(QCoreApplication::translate("MainWindow", "ACCOMPAGNEMENT CLIENTS", nullptr));
        calendarIconLabel->setText(QCoreApplication::translate("MainWindow", "05", nullptr));
        dateTimeLabel->setText(QCoreApplication::translate("MainWindow", "05 F\303\251vrier 2026  \342\200\242  13:14", nullptr));
        label_en_ligne->setText(QCoreApplication::translate("MainWindow", "\342\227\217 EN LIGNE", nullptr));
        groupBox_formulaire->setTitle(QCoreApplication::translate("MainWindow", "Informations Client", nullptr));
        label_id->setText(QCoreApplication::translate("MainWindow", "ID Client:", nullptr));
        label_nom->setText(QCoreApplication::translate("MainWindow", "Nom:", nullptr));
        label_prenom->setText(QCoreApplication::translate("MainWindow", "Pr\303\251nom:", nullptr));
        label_tel->setText(QCoreApplication::translate("MainWindow", "T\303\251l\303\251phone:", nullptr));
        label_adresse->setText(QCoreApplication::translate("MainWindow", "Adresse:", nullptr));
        label_email->setText(QCoreApplication::translate("MainWindow", "Email:", nullptr));
        label_type->setText(QCoreApplication::translate("MainWindow", "Type Client:", nullptr));
        comboBox_type->setItemText(0, QCoreApplication::translate("MainWindow", "Particulier", nullptr));
        comboBox_type->setItemText(1, QCoreApplication::translate("MainWindow", "Soci\303\251t\303\251", nullptr));

        label_total->setText(QCoreApplication::translate("MainWindow", "Total Olives (kg):", nullptr));
        label_date->setText(QCoreApplication::translate("MainWindow", "Date Cr\303\251ation:", nullptr));
        label_statut_label->setText(QCoreApplication::translate("MainWindow", "Statut:", nullptr));
        label_statut->setText(QCoreApplication::translate("MainWindow", "-", nullptr));
        groupBox_boutons->setTitle(QCoreApplication::translate("MainWindow", "Actions", nullptr));
        pushButton_ajouter->setText(QCoreApplication::translate("MainWindow", "Ajouter", nullptr));
        pushButton_modifier->setText(QCoreApplication::translate("MainWindow", "Modifier", nullptr));
        pushButton_supprimer->setText(QCoreApplication::translate("MainWindow", "Supprimer", nullptr));
        pushButton_consulter->setText(QCoreApplication::translate("MainWindow", "Consulter", nullptr));
        pushButton_clear->setText(QCoreApplication::translate("MainWindow", "Vider", nullptr));
        groupBox_tableau->setTitle(QCoreApplication::translate("MainWindow", "Liste des Clients", nullptr));
        tabClient->setTabText(tabClient->indexOf(tab_gestionClient), QCoreApplication::translate("MainWindow", "Gestion Client", nullptr));
        groupBox_recherche->setTitle(QCoreApplication::translate("MainWindow", "Recherche Client", nullptr));
        label_critere->setText(QCoreApplication::translate("MainWindow", "Crit\303\250re:", nullptr));
        comboBox_critere->setItemText(0, QCoreApplication::translate("MainWindow", "nom", nullptr));
        comboBox_critere->setItemText(1, QCoreApplication::translate("MainWindow", "prenom", nullptr));
        comboBox_critere->setItemText(2, QCoreApplication::translate("MainWindow", "telephone", nullptr));
        comboBox_critere->setItemText(3, QCoreApplication::translate("MainWindow", "email", nullptr));
        comboBox_critere->setItemText(4, QCoreApplication::translate("MainWindow", "type_client", nullptr));

        label_valeur->setText(QCoreApplication::translate("MainWindow", "Valeur:", nullptr));
        pushButton_rechercher->setText(QCoreApplication::translate("MainWindow", "Rechercher", nullptr));
        groupBox_tri->setTitle(QCoreApplication::translate("MainWindow", "Tri", nullptr));
        label_trier->setText(QCoreApplication::translate("MainWindow", "Trier par:", nullptr));
        comboBox_tri->setItemText(0, QCoreApplication::translate("MainWindow", "nom", nullptr));
        comboBox_tri->setItemText(1, QCoreApplication::translate("MainWindow", "prenom", nullptr));
        comboBox_tri->setItemText(2, QCoreApplication::translate("MainWindow", "type_client", nullptr));
        comboBox_tri->setItemText(3, QCoreApplication::translate("MainWindow", "total_olives_livrees", nullptr));
        comboBox_tri->setItemText(4, QCoreApplication::translate("MainWindow", "date_creation", nullptr));

        pushButton_trier->setText(QCoreApplication::translate("MainWindow", "Trier", nullptr));
        groupBox_export->setTitle(QCoreApplication::translate("MainWindow", "Exporter", nullptr));
        pushButton_pdf->setText(QCoreApplication::translate("MainWindow", "Export PDF", nullptr));
        pushButton_excel->setText(QCoreApplication::translate("MainWindow", "Export Excel", nullptr));
        pushButton_word->setText(QCoreApplication::translate("MainWindow", "Export Word", nullptr));
        pushButton_txt->setText(QCoreApplication::translate("MainWindow", "Export TXT", nullptr));
        groupBox_resultats->setTitle(QCoreApplication::translate("MainWindow", "R\303\251sultats", nullptr));
        tabClient->setTabText(tabClient->indexOf(tab_rechercheTri), QCoreApplication::translate("MainWindow", "Recherche & Tri", nullptr));
        groupBox_statistiques->setTitle(QCoreApplication::translate("MainWindow", "G\303\251n\303\251rer Statistiques", nullptr));
        label_stat_type->setText(QCoreApplication::translate("MainWindow", "Type de Statistique:", nullptr));
        comboBox_stat->setItemText(0, QCoreApplication::translate("MainWindow", "Nombre par type_client", nullptr));
        comboBox_stat->setItemText(1, QCoreApplication::translate("MainWindow", "Total olives par type_client", nullptr));
        comboBox_stat->setItemText(2, QCoreApplication::translate("MainWindow", "Clients par date_creation", nullptr));
        comboBox_stat->setItemText(3, QCoreApplication::translate("MainWindow", "Statut fid\303\251lit\303\251", nullptr));
        comboBox_stat->setItemText(4, QCoreApplication::translate("MainWindow", "Importance (par quantit\303\251)", nullptr));

        pushButton_stat->setText(QCoreApplication::translate("MainWindow", "G\303\251n\303\251rer", nullptr));
        groupBox_graph->setTitle(QCoreApplication::translate("MainWindow", "Graphiques", nullptr));
        label_chart_placeholder->setText(QCoreApplication::translate("MainWindow", "Graphique sera affich\303\251 ici", nullptr));
        tabClient->setTabText(tabClient->indexOf(tab_statistiques), QCoreApplication::translate("MainWindow", "Statistiques", nullptr));
        groupBox_classification->setTitle(QCoreApplication::translate("MainWindow", "Classification Automatique", nullptr));
        label_score_title->setText(QCoreApplication::translate("MainWindow", "Score Fid\303\251lit\303\251:", nullptr));
        label_score_value->setText(QCoreApplication::translate("MainWindow", "-", nullptr));
        label_statut_titre->setText(QCoreApplication::translate("MainWindow", "Statut Fid\303\251lit\303\251:", nullptr));
        label_statut_value->setText(QCoreApplication::translate("MainWindow", "-", nullptr));
        label_importance_titre->setText(QCoreApplication::translate("MainWindow", "Importance:", nullptr));
        label_importance_value->setText(QCoreApplication::translate("MainWindow", "-", nullptr));
        groupBox_rapport->setTitle(QCoreApplication::translate("MainWindow", "Rapport Client", nullptr));
        pushButton_rapport->setText(QCoreApplication::translate("MainWindow", "G\303\251n\303\251rer Rapport", nullptr));
        pushButton_exportRapport->setText(QCoreApplication::translate("MainWindow", "Export PDF", nullptr));
        tabClient->setTabText(tabClient->indexOf(tab_fideliteRapport), QCoreApplication::translate("MainWindow", "Fid\303\251lit\303\251 & Rapport", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
