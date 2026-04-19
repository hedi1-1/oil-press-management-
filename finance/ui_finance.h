/********************************************************************************
** Form generated from reading UI file 'finance.ui'
**
** Created by: Qt User Interface Compiler version 6.7.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FINANCE_H
#define UI_FINANCE_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Finance
{
public:
    QWidget *centralwidget;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *Finance)
    {
        if (Finance->objectName().isEmpty())
            Finance->setObjectName("Finance");
        Finance->resize(1441, 616);
        centralwidget = new QWidget(Finance);
        centralwidget->setObjectName("centralwidget");
        Finance->setCentralWidget(centralwidget);
        menubar = new QMenuBar(Finance);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 1441, 25));
        Finance->setMenuBar(menubar);
        statusbar = new QStatusBar(Finance);
        statusbar->setObjectName("statusbar");
        Finance->setStatusBar(statusbar);

        retranslateUi(Finance);

        QMetaObject::connectSlotsByName(Finance);
    } // setupUi

    void retranslateUi(QMainWindow *Finance)
    {
        Finance->setWindowTitle(QCoreApplication::translate("Finance", "Finance", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Finance: public Ui_Finance {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FINANCE_H
