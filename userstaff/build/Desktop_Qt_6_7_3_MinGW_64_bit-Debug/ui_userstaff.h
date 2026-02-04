/********************************************************************************
** Form generated from reading UI file 'userstaff.ui'
**
** Created by: Qt User Interface Compiler version 6.7.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_USERSTAFF_H
#define UI_USERSTAFF_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_userstaff
{
public:
    QWidget *centralwidget;
    QVBoxLayout *mainLayout;
    QFrame *headerFrame;
    QHBoxLayout *headerLayout;
    QFrame *brandingFrame;
    QVBoxLayout *brandingLayout;
    QLabel *appNameLabel;
    QLabel *subtitleLabel;
    QSpacerItem *headerSpacer;
    QLabel *statusLabel;
    QLabel *versionBadge;
    QFrame *sectionTitleFrame;
    QHBoxLayout *sectionTitleLayout;
    QPushButton *btnBack;
    QLabel *sectionTitleLabel;
    QSpacerItem *sectionSpacer;
    QFrame *contentFrame;
    QVBoxLayout *contentLayout;
    QHBoxLayout *statsLayout;
    QFrame *cardAdmin;
    QVBoxLayout *cardAdminLayout;
    QLabel *cardAdminTitle;
    QLabel *cardAdminValue;
    QFrame *cardOperateur;
    QVBoxLayout *cardOperateurLayout;
    QLabel *cardOperateurTitle;
    QLabel *cardOperateurValue;
    QFrame *cardManager;
    QVBoxLayout *cardManagerLayout;
    QLabel *cardManagerTitle;
    QLabel *cardManagerValue;
    QFrame *actionBarFrame;
    QHBoxLayout *actionBarLayout;
    QLineEdit *searchField;
    QComboBox *roleFilter;
    QSpacerItem *actionSpacer;
    QPushButton *btnAjouter;
    QPushButton *btnExporter;
    QTableWidget *userTable;

    void setupUi(QMainWindow *userstaff)
    {
        if (userstaff->objectName().isEmpty())
            userstaff->setObjectName("userstaff");
        userstaff->resize(1440, 616);
        userstaff->setMinimumSize(QSize(1440, 616));
        userstaff->setStyleSheet(QString::fromUtf8("QMainWindow {\n"
"    background-color: #f3f6f5;\n"
"    font-family: 'Segoe UI', Helvetica, Arial, sans-serif;\n"
"}\n"
"QLabel {\n"
"    color: #111827;\n"
"}"));
        centralwidget = new QWidget(userstaff);
        centralwidget->setObjectName("centralwidget");
        mainLayout = new QVBoxLayout(centralwidget);
        mainLayout->setSpacing(0);
        mainLayout->setObjectName("mainLayout");
        mainLayout->setContentsMargins(0, 0, 0, 0);
        headerFrame = new QFrame(centralwidget);
        headerFrame->setObjectName("headerFrame");
        headerFrame->setMinimumSize(QSize(0, 50));
        headerFrame->setMaximumSize(QSize(16777215, 50));
        headerFrame->setStyleSheet(QString::fromUtf8("QFrame#headerFrame {\n"
"    background-color: #2c3e2e;\n"
"    border: none;\n"
"}"));
        headerLayout = new QHBoxLayout(headerFrame);
        headerLayout->setSpacing(20);
        headerLayout->setObjectName("headerLayout");
        headerLayout->setContentsMargins(20, 0, 20, 0);
        brandingFrame = new QFrame(headerFrame);
        brandingFrame->setObjectName("brandingFrame");
        brandingFrame->setStyleSheet(QString::fromUtf8("background: transparent; border: none;"));
        brandingLayout = new QVBoxLayout(brandingFrame);
        brandingLayout->setSpacing(0);
        brandingLayout->setObjectName("brandingLayout");
        brandingLayout->setContentsMargins(0, 4, 0, 4);
        appNameLabel = new QLabel(brandingFrame);
        appNameLabel->setObjectName("appNameLabel");
        appNameLabel->setStyleSheet(QString::fromUtf8("color: white; font-size: 20px; font-weight: bold; background: transparent;"));

        brandingLayout->addWidget(appNameLabel);

        subtitleLabel = new QLabel(brandingFrame);
        subtitleLabel->setObjectName("subtitleLabel");
        subtitleLabel->setStyleSheet(QString::fromUtf8("color: #d4a63a; font-size: 10px; font-weight: 600; background: transparent;"));

        brandingLayout->addWidget(subtitleLabel);


        headerLayout->addWidget(brandingFrame);

        headerSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        headerLayout->addItem(headerSpacer);

        statusLabel = new QLabel(headerFrame);
        statusLabel->setObjectName("statusLabel");
        statusLabel->setStyleSheet(QString::fromUtf8("color: #4ade80; font-size: 12px; font-weight: 500; background: transparent;"));

        headerLayout->addWidget(statusLabel);

        versionBadge = new QLabel(headerFrame);
        versionBadge->setObjectName("versionBadge");
        versionBadge->setStyleSheet(QString::fromUtf8("background-color: #d4a63a; color: #2c3e2e; font-size: 11px; font-weight: bold; padding: 6px 12px; border-radius: 4px;"));

        headerLayout->addWidget(versionBadge);


        mainLayout->addWidget(headerFrame);

        sectionTitleFrame = new QFrame(centralwidget);
        sectionTitleFrame->setObjectName("sectionTitleFrame");
        sectionTitleFrame->setMinimumSize(QSize(0, 42));
        sectionTitleFrame->setMaximumSize(QSize(16777215, 42));
        sectionTitleFrame->setStyleSheet(QString::fromUtf8("QFrame#sectionTitleFrame {\n"
"    background-color: #2c3e2e;\n"
"    border-bottom: 2px solid #1f2937;\n"
"}"));
        sectionTitleLayout = new QHBoxLayout(sectionTitleFrame);
        sectionTitleLayout->setObjectName("sectionTitleLayout");
        sectionTitleLayout->setContentsMargins(20, 0, 20, 0);
        btnBack = new QPushButton(sectionTitleFrame);
        btnBack->setObjectName("btnBack");
        btnBack->setMinimumSize(QSize(80, 30));
        btnBack->setMaximumSize(QSize(80, 30));
        btnBack->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"    background-color: transparent;\n"
"    color: white;\n"
"    border: 1px solid #4ade80;\n"
"    border-radius: 4px;\n"
"    padding: 4px 12px;\n"
"    font-size: 12px;\n"
"    font-weight: 500;\n"
"}\n"
"QPushButton:hover {\n"
"    background-color: #4ade80;\n"
"    color: #1f2937;\n"
"}"));

        sectionTitleLayout->addWidget(btnBack);

        sectionTitleLabel = new QLabel(sectionTitleFrame);
        sectionTitleLabel->setObjectName("sectionTitleLabel");
        sectionTitleLabel->setStyleSheet(QString::fromUtf8("color: white; font-size: 18px; font-weight: bold; background: transparent; margin-left: 15px;"));

        sectionTitleLayout->addWidget(sectionTitleLabel);

        sectionSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        sectionTitleLayout->addItem(sectionSpacer);


        mainLayout->addWidget(sectionTitleFrame);

        contentFrame = new QFrame(centralwidget);
        contentFrame->setObjectName("contentFrame");
        contentFrame->setStyleSheet(QString::fromUtf8("QFrame#contentFrame {\n"
"    background-color: #f3f6f5;\n"
"    border: none;\n"
"}"));
        contentLayout = new QVBoxLayout(contentFrame);
        contentLayout->setSpacing(16);
        contentLayout->setObjectName("contentLayout");
        contentLayout->setContentsMargins(20, 16, 20, 16);
        statsLayout = new QHBoxLayout();
        statsLayout->setSpacing(16);
        statsLayout->setObjectName("statsLayout");
        cardAdmin = new QFrame(contentFrame);
        cardAdmin->setObjectName("cardAdmin");
        cardAdmin->setMinimumSize(QSize(0, 85));
        cardAdmin->setMaximumSize(QSize(16777215, 85));
        cardAdmin->setStyleSheet(QString::fromUtf8("QFrame#cardAdmin {\n"
"    background-color: #dbeafe;\n"
"    border: 1px solid #bfdbfe;\n"
"    border-radius: 8px;\n"
"}"));
        cardAdminLayout = new QVBoxLayout(cardAdmin);
        cardAdminLayout->setSpacing(4);
        cardAdminLayout->setObjectName("cardAdminLayout");
        cardAdminLayout->setContentsMargins(16, 12, 16, 12);
        cardAdminTitle = new QLabel(cardAdmin);
        cardAdminTitle->setObjectName("cardAdminTitle");
        cardAdminTitle->setStyleSheet(QString::fromUtf8("color: #4b5563; font-size: 12px; font-weight: 500; background: transparent;"));

        cardAdminLayout->addWidget(cardAdminTitle);

        cardAdminValue = new QLabel(cardAdmin);
        cardAdminValue->setObjectName("cardAdminValue");
        cardAdminValue->setStyleSheet(QString::fromUtf8("color: #111827; font-size: 22px; font-weight: bold; background: transparent;"));

        cardAdminLayout->addWidget(cardAdminValue);


        statsLayout->addWidget(cardAdmin);

        cardOperateur = new QFrame(contentFrame);
        cardOperateur->setObjectName("cardOperateur");
        cardOperateur->setMinimumSize(QSize(0, 85));
        cardOperateur->setMaximumSize(QSize(16777215, 85));
        cardOperateur->setStyleSheet(QString::fromUtf8("QFrame#cardOperateur {\n"
"    background-color: #fffbeb;\n"
"    border: 1px solid #fed7aa;\n"
"    border-radius: 8px;\n"
"}"));
        cardOperateurLayout = new QVBoxLayout(cardOperateur);
        cardOperateurLayout->setSpacing(4);
        cardOperateurLayout->setObjectName("cardOperateurLayout");
        cardOperateurLayout->setContentsMargins(16, 12, 16, 12);
        cardOperateurTitle = new QLabel(cardOperateur);
        cardOperateurTitle->setObjectName("cardOperateurTitle");
        cardOperateurTitle->setStyleSheet(QString::fromUtf8("color: #4b5563; font-size: 12px; font-weight: 500; background: transparent;"));

        cardOperateurLayout->addWidget(cardOperateurTitle);

        cardOperateurValue = new QLabel(cardOperateur);
        cardOperateurValue->setObjectName("cardOperateurValue");
        cardOperateurValue->setStyleSheet(QString::fromUtf8("color: #111827; font-size: 22px; font-weight: bold; background: transparent;"));

        cardOperateurLayout->addWidget(cardOperateurValue);


        statsLayout->addWidget(cardOperateur);

        cardManager = new QFrame(contentFrame);
        cardManager->setObjectName("cardManager");
        cardManager->setMinimumSize(QSize(0, 85));
        cardManager->setMaximumSize(QSize(16777215, 85));
        cardManager->setStyleSheet(QString::fromUtf8("QFrame#cardManager {\n"
"    background-color: #dcfce7;\n"
"    border: 1px solid #bbf7d0;\n"
"    border-radius: 8px;\n"
"}"));
        cardManagerLayout = new QVBoxLayout(cardManager);
        cardManagerLayout->setSpacing(4);
        cardManagerLayout->setObjectName("cardManagerLayout");
        cardManagerLayout->setContentsMargins(16, 12, 16, 12);
        cardManagerTitle = new QLabel(cardManager);
        cardManagerTitle->setObjectName("cardManagerTitle");
        cardManagerTitle->setStyleSheet(QString::fromUtf8("color: #4b5563; font-size: 12px; font-weight: 500; background: transparent;"));

        cardManagerLayout->addWidget(cardManagerTitle);

        cardManagerValue = new QLabel(cardManager);
        cardManagerValue->setObjectName("cardManagerValue");
        cardManagerValue->setStyleSheet(QString::fromUtf8("color: #111827; font-size: 22px; font-weight: bold; background: transparent;"));

        cardManagerLayout->addWidget(cardManagerValue);


        statsLayout->addWidget(cardManager);


        contentLayout->addLayout(statsLayout);

        actionBarFrame = new QFrame(contentFrame);
        actionBarFrame->setObjectName("actionBarFrame");
        actionBarFrame->setMinimumSize(QSize(0, 55));
        actionBarFrame->setMaximumSize(QSize(16777215, 55));
        actionBarFrame->setStyleSheet(QString::fromUtf8("QFrame#actionBarFrame {\n"
"    background-color: white;\n"
"    border: 1px solid #e5e7eb;\n"
"    border-radius: 8px;\n"
"}"));
        actionBarLayout = new QHBoxLayout(actionBarFrame);
        actionBarLayout->setSpacing(12);
        actionBarLayout->setObjectName("actionBarLayout");
        actionBarLayout->setContentsMargins(12, 8, 12, 8);
        searchField = new QLineEdit(actionBarFrame);
        searchField->setObjectName("searchField");
        searchField->setMinimumSize(QSize(300, 35));
        searchField->setStyleSheet(QString::fromUtf8("QLineEdit {\n"
"    background-color: #f9fafb;\n"
"    border: 1px solid #e5e7eb;\n"
"    border-radius: 6px;\n"
"    padding: 6px 12px;\n"
"    font-size: 13px;\n"
"    color: #111827;\n"
"}\n"
"QLineEdit:focus {\n"
"    border: 2px solid #22c55e;\n"
"}"));

        actionBarLayout->addWidget(searchField);

        roleFilter = new QComboBox(actionBarFrame);
        roleFilter->addItem(QString());
        roleFilter->addItem(QString());
        roleFilter->addItem(QString());
        roleFilter->addItem(QString());
        roleFilter->setObjectName("roleFilter");
        roleFilter->setMinimumSize(QSize(150, 35));
        roleFilter->setStyleSheet(QString::fromUtf8("QComboBox {\n"
"    background-color: #2c3e2e;\n"
"    color: white;\n"
"    border: 1px solid #14532d;\n"
"    border-radius: 6px;\n"
"    padding: 6px 12px;\n"
"    font-size: 13px;\n"
"}\n"
"QComboBox::drop-down {\n"
"    border: none;\n"
"    width: 20px;\n"
"}\n"
"QComboBox QAbstractItemView {\n"
"    background-color: #2c3e2e;\n"
"    color: white;\n"
"    selection-background-color: #14532d;\n"
"    border: 1px solid #14532d;\n"
"}"));

        actionBarLayout->addWidget(roleFilter);

        actionSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        actionBarLayout->addItem(actionSpacer);

        btnAjouter = new QPushButton(actionBarFrame);
        btnAjouter->setObjectName("btnAjouter");
        btnAjouter->setMinimumSize(QSize(100, 35));
        btnAjouter->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"    background-color: #16a34a;\n"
"    color: white;\n"
"    border: none;\n"
"    border-radius: 6px;\n"
"    padding: 8px 16px;\n"
"    font-size: 13px;\n"
"    font-weight: 600;\n"
"}\n"
"QPushButton:hover {\n"
"    background-color: #22c55e;\n"
"}"));

        actionBarLayout->addWidget(btnAjouter);

        btnExporter = new QPushButton(actionBarFrame);
        btnExporter->setObjectName("btnExporter");
        btnExporter->setMinimumSize(QSize(100, 35));
        btnExporter->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"    background-color: #f59e0b;\n"
"    color: #1f2937;\n"
"    border: none;\n"
"    border-radius: 6px;\n"
"    padding: 8px 16px;\n"
"    font-size: 13px;\n"
"    font-weight: 600;\n"
"}\n"
"QPushButton:hover {\n"
"    background-color: #fbbf24;\n"
"}"));

        actionBarLayout->addWidget(btnExporter);


        contentLayout->addWidget(actionBarFrame);

        userTable = new QTableWidget(contentFrame);
        if (userTable->columnCount() < 6)
            userTable->setColumnCount(6);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        userTable->setHorizontalHeaderItem(0, __qtablewidgetitem);
        QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
        userTable->setHorizontalHeaderItem(1, __qtablewidgetitem1);
        QTableWidgetItem *__qtablewidgetitem2 = new QTableWidgetItem();
        userTable->setHorizontalHeaderItem(2, __qtablewidgetitem2);
        QTableWidgetItem *__qtablewidgetitem3 = new QTableWidgetItem();
        userTable->setHorizontalHeaderItem(3, __qtablewidgetitem3);
        QTableWidgetItem *__qtablewidgetitem4 = new QTableWidgetItem();
        userTable->setHorizontalHeaderItem(4, __qtablewidgetitem4);
        QTableWidgetItem *__qtablewidgetitem5 = new QTableWidgetItem();
        userTable->setHorizontalHeaderItem(5, __qtablewidgetitem5);
        userTable->setObjectName("userTable");
        userTable->setStyleSheet(QString::fromUtf8("QTableWidget {\n"
"    background-color: white;\n"
"    border: 1px solid #e5e7eb;\n"
"    border-radius: 8px;\n"
"    gridline-color: #e5e7eb;\n"
"    font-size: 13px;\n"
"    color: #111827;\n"
"}\n"
"QTableWidget::item {\n"
"    padding: 10px;\n"
"    border-bottom: 1px solid #e5e7eb;\n"
"}\n"
"QTableWidget::item:selected {\n"
"    background-color: #065f46;\n"
"    color: white;\n"
"}\n"
"QTableWidget::item:alternate {\n"
"    background-color: #f9fafb;\n"
"}\n"
"QHeaderView::section {\n"
"    background-color: #2c3e2e;\n"
"    color: white;\n"
"    font-size: 12px;\n"
"    font-weight: 600;\n"
"    padding: 10px;\n"
"    border: none;\n"
"    border-right: 1px solid #1f2937;\n"
"}\n"
"QHeaderView::section:last {\n"
"    border-right: none;\n"
"}"));
        userTable->setAlternatingRowColors(true);
        userTable->setSelectionMode(QAbstractItemView::SingleSelection);
        userTable->setSelectionBehavior(QAbstractItemView::SelectRows);
        userTable->horizontalHeader()->setStretchLastSection(true);
        userTable->verticalHeader()->setVisible(false);

        contentLayout->addWidget(userTable);


        mainLayout->addWidget(contentFrame);

        userstaff->setCentralWidget(centralwidget);

        retranslateUi(userstaff);

        QMetaObject::connectSlotsByName(userstaff);
    } // setupUi

    void retranslateUi(QMainWindow *userstaff)
    {
        userstaff->setWindowTitle(QCoreApplication::translate("userstaff", "PressIQ - User & Staff Management", nullptr));
        appNameLabel->setText(QCoreApplication::translate("userstaff", "PRESSIQ", nullptr));
        subtitleLabel->setText(QCoreApplication::translate("userstaff", "SMART OIL SYSTEMS", nullptr));
        statusLabel->setText(QCoreApplication::translate("userstaff", "\342\227\217 EN LIGNE", nullptr));
        versionBadge->setText(QCoreApplication::translate("userstaff", "v1.0", nullptr));
        btnBack->setText(QCoreApplication::translate("userstaff", "Retour", nullptr));
        sectionTitleLabel->setText(QCoreApplication::translate("userstaff", "User & Staff Management", nullptr));
        cardAdminTitle->setText(QCoreApplication::translate("userstaff", "Administrateurs", nullptr));
        cardAdminValue->setText(QCoreApplication::translate("userstaff", "--", nullptr));
        cardOperateurTitle->setText(QCoreApplication::translate("userstaff", "Op\303\251rateurs", nullptr));
        cardOperateurValue->setText(QCoreApplication::translate("userstaff", "--", nullptr));
        cardManagerTitle->setText(QCoreApplication::translate("userstaff", "Managers", nullptr));
        cardManagerValue->setText(QCoreApplication::translate("userstaff", "--", nullptr));
        searchField->setPlaceholderText(QCoreApplication::translate("userstaff", "Rechercher par nom ou email...", nullptr));
        roleFilter->setItemText(0, QCoreApplication::translate("userstaff", "Tous les r\303\264les", nullptr));
        roleFilter->setItemText(1, QCoreApplication::translate("userstaff", "Admin", nullptr));
        roleFilter->setItemText(2, QCoreApplication::translate("userstaff", "Op\303\251rateur", nullptr));
        roleFilter->setItemText(3, QCoreApplication::translate("userstaff", "Manager", nullptr));

        btnAjouter->setText(QCoreApplication::translate("userstaff", "+ Ajouter", nullptr));
        btnExporter->setText(QCoreApplication::translate("userstaff", "Exporter", nullptr));
        QTableWidgetItem *___qtablewidgetitem = userTable->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QCoreApplication::translate("userstaff", "Nom", nullptr));
        QTableWidgetItem *___qtablewidgetitem1 = userTable->horizontalHeaderItem(1);
        ___qtablewidgetitem1->setText(QCoreApplication::translate("userstaff", "Identifiant", nullptr));
        QTableWidgetItem *___qtablewidgetitem2 = userTable->horizontalHeaderItem(2);
        ___qtablewidgetitem2->setText(QCoreApplication::translate("userstaff", "R\303\264le", nullptr));
        QTableWidgetItem *___qtablewidgetitem3 = userTable->horizontalHeaderItem(3);
        ___qtablewidgetitem3->setText(QCoreApplication::translate("userstaff", "Statut", nullptr));
        QTableWidgetItem *___qtablewidgetitem4 = userTable->horizontalHeaderItem(4);
        ___qtablewidgetitem4->setText(QCoreApplication::translate("userstaff", "Logs", nullptr));
        QTableWidgetItem *___qtablewidgetitem5 = userTable->horizontalHeaderItem(5);
        ___qtablewidgetitem5->setText(QCoreApplication::translate("userstaff", "Actions", nullptr));
    } // retranslateUi

};

namespace Ui {
    class userstaff: public Ui_userstaff {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_USERSTAFF_H
