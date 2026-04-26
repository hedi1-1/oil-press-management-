/********************************************************************************
** Form generated from reading UI file 'production.ui'
**
** Created by: Qt User Interface Compiler version 6.7.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PRODUCTION_H
#define UI_PRODUCTION_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
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
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Production
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
    QLabel *lblAppSubtitle;
    QSpacerItem *headerSpacer;
    QLabel *lblDateTime;
    QPushButton *btnDarkMode;
    QPushButton *btnLanguage;
    QLabel *lblOnlineStatus;
    QWidget *dashboardWidget;
    QHBoxLayout *dashboardLayout;
    QWidget *dashCard1;
    QHBoxLayout *dashCard1Layout;
    QLabel *lblDashIcon1;
    QVBoxLayout *dashText1Layout;
    QLabel *lblDashValue1;
    QLabel *lblDashLabel1;
    QWidget *dashCard2;
    QHBoxLayout *dashCard2Layout;
    QLabel *lblDashIcon2;
    QVBoxLayout *dashText2Layout;
    QLabel *lblDashValue2;
    QLabel *lblDashLabel2;
    QWidget *dashCard3;
    QHBoxLayout *dashCard3Layout;
    QLabel *lblDashIcon3;
    QVBoxLayout *dashText3Layout;
    QLabel *lblDashValue3;
    QLabel *lblDashLabel3;
    QWidget *dashCard4;
    QHBoxLayout *dashCard4Layout;
    QLabel *lblDashIcon4;
    QVBoxLayout *dashText4Layout;
    QLabel *lblDashValue4;
    QLabel *lblDashLabel4;
    QWidget *tabContainerWidget;
    QHBoxLayout *tabContainerLayout;
    QTabWidget *tabWidgetProduction;
    QWidget *tabPlanification;
    QHBoxLayout *layoutPlanificationMain;
    QVBoxLayout *layoutPlanLeft;
    QGroupBox *groupProductionParams;
    QFormLayout *formProductionParams;
    QLabel *lblDateProduction;
    QDateEdit *dateEditProduction;
    QLabel *lblOlivesQty;
    QSpinBox *spinOlivesKg;
    QLabel *lblPressType;
    QComboBox *comboPressType;
    QLabel *lblOliveVariety;
    QComboBox *comboOliveVariety;
    QGroupBox *groupNotesPlanification;
    QVBoxLayout *layoutNotes;
    QTextEdit *txtNotesPlanification;
    QSpacerItem *spacerPlanLeft;
    QVBoxLayout *layoutPlanRight;
    QGroupBox *groupPlanStatus;
    QVBoxLayout *layoutPlanStatus;
    QLabel *lblStatusPlanification;
    QLabel *lblEstimatedYield;
    QLabel *lblDuration;
    QGroupBox *groupQuickStats;
    QGridLayout *gridQuickStats;
    QLabel *lblTodayProductions;
    QLabel *lblTodayProductionsValue;
    QLabel *lblWeekProductions;
    QLabel *lblWeekProductionsValue;
    QLabel *lblAvgYield;
    QLabel *lblAvgYieldValue;
    QSpacerItem *spacerPlanRight;
    QPushButton *btnPlanifier;
    QWidget *tabLancement;
    QVBoxLayout *layoutLancement;
    QScrollArea *scrollLancement;
    QWidget *scrollContentsLancement;
    QVBoxLayout *layoutLancementInner;
    QHBoxLayout *layoutRow1;
    QGroupBox *groupSelectProduction;
    QVBoxLayout *layoutSelectProd;
    QHBoxLayout *layoutSelectRow;
    QComboBox *comboSelectProduction;
    QPushButton *btnRefreshPlanned;
    QLabel *lblSelectedProductionInfo;
    QGroupBox *groupLancementStatus;
    QHBoxLayout *layoutLancementStatus;
    QVBoxLayout *layoutCurrentStatus;
    QLabel *lblCurrentStatusTitle;
    QLabel *lblCurrentStatus;
    QVBoxLayout *layoutStartTime;
    QLabel *lblStartTimeTitle;
    QLabel *lblStartTime;
    QVBoxLayout *layoutElapsedTime;
    QLabel *lblElapsedTimeTitle;
    QLabel *lblElapsedTime;
    QGroupBox *groupProgress;
    QVBoxLayout *layoutProgress;
    QProgressBar *progressProduction;
    QHBoxLayout *layoutProgressLabels;
    QLabel *lblPhase;
    QSpacerItem *spacerProgress;
    QLabel *lblProgressEnd;
    QGroupBox *groupControls;
    QHBoxLayout *layoutControls;
    QSpacerItem *spacerControlsLeft;
    QPushButton *btnStart;
    QPushButton *btnStop;
    QPushButton *btnFinishProduction;
    QSpacerItem *spacerControlsRight;
    QSpacerItem *spacerLancement;
    QWidget *tabSuivi;
    QVBoxLayout *layoutSuivi;
    QGroupBox *groupRunningProduction;
    QVBoxLayout *layoutRunningProduction;
    QLabel *lblRunningProduction;
    QWidget *widgetRunningActive;
    QVBoxLayout *layoutRunningActive;
    QHBoxLayout *layoutRunningRow1;
    QLabel *lblRunningStatus;
    QLabel *lblRunningPhase;
    QProgressBar *progressRunning;
    QHBoxLayout *layoutRunningStats;
    QLabel *lblRunningOlives;
    QLabel *lblRunningOil;
    QLabel *lblRunningRendement;
    QLabel *lblRunningSimTime;
    QLabel *lblRunningRealTime;
    QGroupBox *groupHistory;
    QVBoxLayout *layoutHistory;
    QTableWidget *tableProductionHistory;
    QHBoxLayout *layoutHistoryButtons;
    QSpacerItem *spacerHistoryLeft;
    QPushButton *btnModifyProduction;
    QPushButton *btnDeleteProduction;
    QHBoxLayout *layoutSuiviFooter;
    QSpacerItem *spacerSuiviFooter;
    QPushButton *btnUpdateMonitoring;
    QWidget *tabRendement;
    QHBoxLayout *layoutRendementMain;
    QVBoxLayout *layoutRendementLeft;
    QGroupBox *groupSelectTerminated;
    QVBoxLayout *layoutSelectTerminated;
    QHBoxLayout *layoutTerminatedRow;
    QComboBox *comboTerminatedProductions;
    QPushButton *btnRefreshTerminated;
    QLabel *lblTerminatedInfo;
    QGroupBox *groupRendementInput;
    QFormLayout *formRendementInput;
    QLabel *lblOlivesUsedTitle;
    QLabel *lblOlivesUsed;
    QLabel *lblOilProducedTitle;
    QDoubleSpinBox *spinOilProduced;
    QPushButton *btnCalculateYield;
    QSpacerItem *spacerRendementLeft;
    QVBoxLayout *layoutRendementRight;
    QGroupBox *groupRendementResult;
    QVBoxLayout *layoutRendementResult;
    QLabel *lblYield;
    QProgressBar *progressYield;
    QGroupBox *groupYieldMessage;
    QVBoxLayout *layoutYieldMessage;
    QLabel *lblYieldMessage;
    QSpacerItem *spacerRendementRight;
    QWidget *tabQualite;
    QVBoxLayout *layoutQualite;
    QGridLayout *layoutQualityMain;
    QGroupBox *groupQualityEval;
    QVBoxLayout *layoutQualityEval;
    QFormLayout *formQuality;
    QLabel *lblOilQuality;
    QComboBox *comboOilQuality;
    QLabel *lblAcidity;
    QDoubleSpinBox *comboAcidity;
    QCheckBox *checkConformity;
    QGroupBox *groupQualityNotes;
    QVBoxLayout *layoutQualityNotes;
    QTextEdit *txtQualityNotes;
    QGroupBox *groupQualitySummary;
    QVBoxLayout *layoutQualitySummary;
    QLabel *lblQualitySummary;
    QFrame *qualityFooterDivider;
    QHBoxLayout *layoutQualiteFooter;
    QSpacerItem *spacerQualiteFooter;
    QPushButton *btnValidateQuality;
    QWidget *tabRapports;
    QVBoxLayout *layoutRapports;
    QGroupBox *groupReportSummary;
    QVBoxLayout *layoutReportSummary;
    QLabel *lblProductionSummary;
    QGroupBox *groupReportContent;
    QVBoxLayout *layoutReportContent;
    QTextBrowser *textReport;
    QHBoxLayout *layoutReportActions;
    QLabel *lblReportDate;
    QSpacerItem *spacerReportActions;
    QPushButton *btnGenerateReport;
    QPushButton *btnExportPDF;

    void setupUi(QMainWindow *Production)
    {
        if (Production->objectName().isEmpty())
            Production->setObjectName("Production");
        Production->resize(1440, 616);
        Production->setMinimumSize(QSize(1200, 550));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/logo.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        Production->setWindowIcon(icon);
        Production->setStyleSheet(QString::fromUtf8("\n"
"/* \342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\n"
"   PRESSIQ PRODUCTION MODULE - PROFESSIONAL DASHBOARD THEME\n"
"   Version 2.0 - Modern Industrial Design\n"
"\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342"
                        "\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220\342\225\220 */\n"
"\n"
"QMainWindow {\n"
"    background: qlineargradient(x1:0, y1:0, x2:1, y2:1, \n"
"                stop:0 #f0f2f5, stop:1 #e4e7eb);\n"
"}\n"
"\n"
"/* \342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224"
                        "\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\n"
"   HEADER SECTION\n"
"\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200"
                        "\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200 */\n"
"#headerWidget {\n"
"    background: qlineargradient(x1:0, y1:0, x2:1, y2:0, \n"
"                stop:0 #1B4332, stop:0.5 #234E3E, stop:1 #1B4332);\n"
"    border-bottom: 4px solid #C9A227;\n"
"}\n"
"\n"
"#lblAppTitle {\n"
"    color: white;\n"
"    font-size: 26px;\n"
"    font-weight: 700;\n"
"    font-family: 'Segoe UI', Arial, sans-serif;\n"
"    letter-spacing: 1px;\n"
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
"    color: #4ADE80;\n"
"    font-size: 13px;\n"
"    font-weight: bold;\n"
"    font-family: 'Segoe UI', Arial, sans"
                        "-serif;\n"
"    padding: 6px 14px;\n"
"    background-color: rgba(74, 222, 128, 0.15);\n"
"    border-radius: 15px;\n"
"    border: 1px solid rgba(74, 222, 128, 0.3);\n"
"}\n"
"\n"
"#btnBackToMenu {\n"
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
"#btnBackToMenu:hover {\n"
"    background-color: rgba(255, 255, 255, 0.25);\n"
"    border: 1px solid rgba(255, 255, 255, 0.5);\n"
"}\n"
"\n"
"#btnBackToMenu:pressed {\n"
"    background-color: rgba(255, 255, 255, 0.1);\n"
"}\n"
"\n"
"/* \342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200"
                        "\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\n"
"   STATISTICS DASHBOARD CARDS\n"
"\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342"
                        "\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200 */\n"
"#dashboardWidget {\n"
"    background: transparent;\n"
"}\n"
"\n"
"#dashCard1, #dashCard2, #dashCard3, #dashCard4 {\n"
"    background: white;\n"
"    border-radius: 12px;\n"
"    border: none;\n"
"    border-left: 4px solid #1B4332;\n"
"}\n"
"\n"
"#dashCard1 { border-left-color: #4ADE80; }\n"
"#dashCard2 { border-left-color: #3B82F6; }\n"
"#dashCard3 { border-left-color: #F59E0B; }\n"
"#dashCard4 { border-left-color: #EF4444; }\n"
"\n"
"/* \342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200"
                        "\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\n"
"   TAB WIDGET - MODERN DESIGN\n"
"\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342"
                        "\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200 */\n"
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
"\n"
"QTabBar::tab:hover:!selected {\n"
"    background-colo"
                        "r: #f0f0f0;\n"
"    color: #1B4332;\n"
"}\n"
"\n"
"/* \342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\n"
"   GROUP BOX - CARD STYLE\n"
"\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200"
                        "\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200 */\n"
"QGroupBox {\n"
"    font-size: 14px;\n"
"    font-weight: 700;\n"
"    font-family: 'Segoe UI', Arial, sans-serif;\n"
"    color: #1B4332;\n"
"    background-color: white;\n"
"    border: none;\n"
"    border-radius: 12px;\n"
"    margin-top: 20px;\n"
"    padding: 20px 15px 15px 15px;\n"
"}\n"
"\n"
"QGroupBox::title {\n"
"    subcontrol-origin: margin;\n"
"    subcontrol-position: top left"
                        ";\n"
"    left: 20px;\n"
"    top: 0px;\n"
"    padding: 8px 16px;\n"
"    background-color: #1B4332;\n"
"    color: white;\n"
"    border-radius: 8px;\n"
"    font-size: 13px;\n"
"}\n"
"\n"
"/* \342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\n"
"   QUALITY TAB - SPACIOUS VISUAL POLISH\n"
"\342"
                        "\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200 */\n"
"#tabQualite {\n"
"    background: qlineargradient(x1:0, y1:0, x2:1, y2:1,\n"
"                stop:0 rgba(255, 255, 255, 0.60), stop:1 rgba(240, 248, 245, 0.80));\n"
"    border-radius: 12px;\n"
"}\n"
"\n"
"#tabQualite QGroupBox {\n"
"    ba"
                        "ckground-color: rgba(255, 255, 255, 0.96);\n"
"    border: 1px solid #e6ece8;\n"
"    border-radius: 12px;\n"
"}\n"
"\n"
"#groupQualityEval::title,\n"
"#groupQualityNotes::title,\n"
"#groupQualitySummary::title {\n"
"    background-color: #1E5A45;\n"
"    padding: 8px 16px;\n"
"}\n"
"\n"
"#tabQualite QLabel#lblOilQuality,\n"
"#tabQualite QLabel#lblAcidity {\n"
"    font-size: 15px;\n"
"    color: #22312b;\n"
"    min-width: 170px;\n"
"}\n"
"\n"
"#txtQualityNotes {\n"
"    background-color: #fcfefd;\n"
"    border: 1px solid #dbe6df;\n"
"    padding: 16px;\n"
"}\n"
"\n"
"#tabQualite QComboBox,\n"
"#tabQualite QDoubleSpinBox {\n"
"    border: 1px solid #dbe3dd;\n"
"    border-radius: 8px;\n"
"    min-height: 40px;\n"
"}\n"
"\n"
"#tabQualite QComboBox:focus,\n"
"#tabQualite QDoubleSpinBox:focus,\n"
"#tabQualite QTextEdit:focus {\n"
"    border: 2px solid #1E5A45;\n"
"}\n"
"\n"
"#checkConformity {\n"
"    padding: 12px;\n"
"    border-radius: 10px;\n"
"}\n"
"\n"
"#qualityFooterDivider {\n"
"    color: #e5e7eb;\n"
""
                        "    background-color: #e5e7eb;\n"
"    min-height: 1px;\n"
"    max-height: 1px;\n"
"    border: none;\n"
"}\n"
"\n"
"#lblQualitySummary {\n"
"    background-color: #edf7f1;\n"
"    border-left: 5px solid #1E5A45;\n"
"    padding: 22px;\n"
"    border-radius: 0 10px 10px 0;\n"
"    color: #274338;\n"
"}\n"
"\n"
"/* \342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200"
                        "\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\n"
"   INPUT FIELDS - MODERN STYLE\n"
"\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200 */\n"
"QLineEdit, QSpinBox, QDoubleSpinBox, QDateEdit, QComboBox {\n"
"    padding: 10px 14px;\n"
"    border: 2px solid #e0e0e0;\n"
""
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
"/* \342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224"
                        "\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\n"
"   TEXT EDIT - MODERN STYLE\n"
"\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200"
                        "\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200 */\n"
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
"/* \342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200"
                        "\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\n"
"   BUTTONS - PROFESSIONAL STYLE\n"
"\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200"
                        "\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200 */\n"
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
"QPushButton#btnPlanifier, \n"
"QPushButton#btnStart, \n"
"QPushButton#btnValidateQuality, \n"
"QPushButton#btnGenerateReport, \n"
"QPushButton#btnUpdateMonitoring,\n"
"QPushButton#btnSearch,\n"
"QPushButton#btnApplySort,\n"
"QPushButton#btnCalculateYield {\n"
"    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, \n"
"                stop:0 #234E3E, stop:"
                        "1 #1B4332);\n"
"    color: white;\n"
"    border: none;\n"
"}\n"
"\n"
"QPushButton#btnPlanifier:hover, \n"
"QPushButton#btnStart:hover, \n"
"QPushButton#btnValidateQuality:hover, \n"
"QPushButton#btnGenerateReport:hover, \n"
"QPushButton#btnUpdateMonitoring:hover,\n"
"QPushButton#btnSearch:hover,\n"
"QPushButton#btnApplySort:hover,\n"
"QPushButton#btnCalculateYield:hover {\n"
"    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, \n"
"                stop:0 #2D5A45, stop:1 #234E3E);\n"
"}\n"
"\n"
"QPushButton#btnPlanifier:pressed, \n"
"QPushButton#btnStart:pressed, \n"
"QPushButton#btnValidateQuality:pressed, \n"
"QPushButton#btnGenerateReport:pressed, \n"
"QPushButton#btnUpdateMonitoring:pressed,\n"
"QPushButton#btnSearch:pressed,\n"
"QPushButton#btnApplySort:pressed,\n"
"QPushButton#btnCalculateYield:pressed {\n"
"    background: #0F2A1F;\n"
"}\n"
"\n"
"/* Danger Button */\n"
"QPushButton#btnStop {\n"
"    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, \n"
"                stop:0 #EF4444, stop:1 #DC26"
                        "26);\n"
"    color: white;\n"
"}\n"
"\n"
"QPushButton#btnStop:hover {\n"
"    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, \n"
"                stop:0 #F87171, stop:1 #EF4444);\n"
"}\n"
"\n"
"QPushButton#btnStop:pressed {\n"
"    background: #B91C1C;\n"
"}\n"
"\n"
"/* Secondary/Export Button */\n"
"QPushButton#btnExportPDF {\n"
"    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, \n"
"                stop:0 #D4A732, stop:1 #C9A227);\n"
"    color: white;\n"
"}\n"
"\n"
"QPushButton#btnExportPDF:hover {\n"
"    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, \n"
"                stop:0 #E5B833, stop:1 #D4A732);\n"
"}\n"
"\n"
"/* Clear/Reset Button */\n"
"QPushButton#btnClearSearch {\n"
"    background-color: #6B7280;\n"
"    color: white;\n"
"}\n"
"\n"
"QPushButton#btnClearSearch:hover {\n"
"    background-color: #4B5563;\n"
"}\n"
"\n"
"/* \342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342"
                        "\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\n"
"   PROGRESS BAR - MODERN ANIMATED STYLE\n"
"\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224"
                        "\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200 */\n"
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
"/* \342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224"
                        "\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\n"
"   TABLE WIDGET - PROFESSIONAL STYLE\n"
"\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342"
                        "\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200 */\n"
"QTableWidget {\n"
"    border: none;\n"
"    border-radius: 8px;\n"
"    background-color: white;\n"
"    gridline-color: #e5e7eb;\n"
"    font-size: 13px;\n"
"    font-family: 'Segoe UI', Arial, sans-serif;\n"
"    color: #1f2937;\n"
"    selection-background-color: #dbeafe;\n"
"    selection-color: #1e40af;\n"
"}\n"
"\n"
"QTableWidget::item {\n"
"    padding: 12px;\n"
"    border-bottom: 1px solid #e5e7eb;\n"
"    color: #1f2937;\n"
"    background-color: white;\n"
"}\n"
"\n"
"QTableWidget::item:selected {\n"
"    background-color: #dbeafe;\n"
"    color: #1e40af;\n"
"}\n"
"\n"
"Q"
                        "TableWidget::item:hover {\n"
"    background-color: #f0fdf4;\n"
"}\n"
"\n"
"QTableWidget::item:alternate {\n"
"    background-color: #f9fafb;\n"
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
"    text-transform: uppercase;\n"
"    letter-spacing: 1px;\n"
"}\n"
"\n"
"QHeaderView::section:first {\n"
"    border-top-left-radius: 8px;\n"
"}\n"
"\n"
"QHeaderView::section:last {\n"
"    border-top-right-radius: 8px;\n"
"}\n"
"\n"
"/* \342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342"
                        "\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\n"
"   LABELS - TYPOGRAPHY\n"
"\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342"
                        "\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200 */\n"
"QLabel {\n"
"    color: #374151;\n"
"    font-size: 14px;\n"
"    font-family: 'Segoe UI', Arial, sans-serif;\n"
"}\n"
"\n"
"/* \342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224"
                        "\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\n"
"   CHECKBOX - MODERN STYLE\n"
"\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200 */\n"
"QCheckBox {\n"
"    font-size: 14px;\n"
"    font-family: 'Segoe UI', Arial, sans-serif;\n"
"    color: #374151;\n"
""
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
"/* \342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224"
                        "\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\n"
"   STATUS INDICATORS\n"
"\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224"
                        "\200\342\224\200\342\224\200 */\n"
"#lblStatusPlanification, #lblCurrentStatus, #lblRunningProduction {\n"
"    font-weight: 700;\n"
"    padding: 12px 20px;\n"
"    border-radius: 8px;\n"
"    font-size: 14px;\n"
"}\n"
"\n"
"/* \342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\n"
"   SCROLLBA"
                        "R - MINIMAL STYLE\n"
"\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200\342\224\200 */\n"
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
""
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
"    height: 10px;\n"
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
"\n"
"   "));
        centralwidget = new QWidget(Production);
        centralwidget->setObjectName("centralwidget");
        mainLayout = new QVBoxLayout(centralwidget);
        mainLayout->setSpacing(0);
        mainLayout->setObjectName("mainLayout");
        mainLayout->setContentsMargins(0, 0, 0, 0);
        headerWidget = new QWidget(centralwidget);
        headerWidget->setObjectName("headerWidget");
        headerWidget->setMinimumSize(QSize(0, 65));
        headerWidget->setMaximumSize(QSize(16777215, 65));
        headerLayout = new QHBoxLayout(headerWidget);
        headerLayout->setSpacing(20);
        headerLayout->setObjectName("headerLayout");
        headerLayout->setContentsMargins(25, 10, 25, 10);
        btnBackToMenu = new QPushButton(headerWidget);
        btnBackToMenu->setObjectName("btnBackToMenu");

        headerLayout->addWidget(btnBackToMenu);

        lblLogo = new QLabel(headerWidget);
        lblLogo->setObjectName("lblLogo");
        lblLogo->setMinimumSize(QSize(84, 50));
        lblLogo->setMaximumSize(QSize(84, 50));
        lblLogo->setPixmap(QPixmap(QString::fromUtf8(":/logo.png")));
        lblLogo->setScaledContents(false);
        lblLogo->setAlignment(Qt::AlignmentFlag::AlignCenter);

        headerLayout->addWidget(lblLogo);

        titleLayout = new QVBoxLayout();
        titleLayout->setSpacing(2);
        titleLayout->setObjectName("titleLayout");
        lblAppTitle = new QLabel(headerWidget);
        lblAppTitle->setObjectName("lblAppTitle");

        titleLayout->addWidget(lblAppTitle);

        lblAppSubtitle = new QLabel(headerWidget);
        lblAppSubtitle->setObjectName("lblAppSubtitle");

        titleLayout->addWidget(lblAppSubtitle);


        headerLayout->addLayout(titleLayout);

        headerSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        headerLayout->addItem(headerSpacer);

        lblDateTime = new QLabel(headerWidget);
        lblDateTime->setObjectName("lblDateTime");

        headerLayout->addWidget(lblDateTime);

        btnDarkMode = new QPushButton(headerWidget);
        btnDarkMode->setObjectName("btnDarkMode");
        btnDarkMode->setMinimumSize(QSize(30, 34));
        btnDarkMode->setCheckable(false);

        headerLayout->addWidget(btnDarkMode);

        btnLanguage = new QPushButton(headerWidget);
        btnLanguage->setObjectName("btnLanguage");
        btnLanguage->setMinimumSize(QSize(30, 34));

        headerLayout->addWidget(btnLanguage);

        lblOnlineStatus = new QLabel(headerWidget);
        lblOnlineStatus->setObjectName("lblOnlineStatus");

        headerLayout->addWidget(lblOnlineStatus);


        mainLayout->addWidget(headerWidget);

        dashboardWidget = new QWidget(centralwidget);
        dashboardWidget->setObjectName("dashboardWidget");
        dashboardWidget->setMinimumSize(QSize(0, 90));
        dashboardWidget->setMaximumSize(QSize(16777215, 100));
        dashboardLayout = new QHBoxLayout(dashboardWidget);
        dashboardLayout->setSpacing(12);
        dashboardLayout->setObjectName("dashboardLayout");
        dashboardLayout->setContentsMargins(20, 8, 20, 8);
        dashCard1 = new QWidget(dashboardWidget);
        dashCard1->setObjectName("dashCard1");
        dashCard1Layout = new QHBoxLayout(dashCard1);
        dashCard1Layout->setSpacing(10);
        dashCard1Layout->setObjectName("dashCard1Layout");
        dashCard1Layout->setContentsMargins(12, 8, 12, 8);
        lblDashIcon1 = new QLabel(dashCard1);
        lblDashIcon1->setObjectName("lblDashIcon1");

        dashCard1Layout->addWidget(lblDashIcon1);

        dashText1Layout = new QVBoxLayout();
        dashText1Layout->setSpacing(2);
        dashText1Layout->setObjectName("dashText1Layout");
        lblDashValue1 = new QLabel(dashCard1);
        lblDashValue1->setObjectName("lblDashValue1");

        dashText1Layout->addWidget(lblDashValue1);

        lblDashLabel1 = new QLabel(dashCard1);
        lblDashLabel1->setObjectName("lblDashLabel1");

        dashText1Layout->addWidget(lblDashLabel1);


        dashCard1Layout->addLayout(dashText1Layout);


        dashboardLayout->addWidget(dashCard1);

        dashCard2 = new QWidget(dashboardWidget);
        dashCard2->setObjectName("dashCard2");
        dashCard2Layout = new QHBoxLayout(dashCard2);
        dashCard2Layout->setSpacing(10);
        dashCard2Layout->setObjectName("dashCard2Layout");
        dashCard2Layout->setContentsMargins(12, 8, 12, 8);
        lblDashIcon2 = new QLabel(dashCard2);
        lblDashIcon2->setObjectName("lblDashIcon2");

        dashCard2Layout->addWidget(lblDashIcon2);

        dashText2Layout = new QVBoxLayout();
        dashText2Layout->setSpacing(2);
        dashText2Layout->setObjectName("dashText2Layout");
        lblDashValue2 = new QLabel(dashCard2);
        lblDashValue2->setObjectName("lblDashValue2");

        dashText2Layout->addWidget(lblDashValue2);

        lblDashLabel2 = new QLabel(dashCard2);
        lblDashLabel2->setObjectName("lblDashLabel2");

        dashText2Layout->addWidget(lblDashLabel2);


        dashCard2Layout->addLayout(dashText2Layout);


        dashboardLayout->addWidget(dashCard2);

        dashCard3 = new QWidget(dashboardWidget);
        dashCard3->setObjectName("dashCard3");
        dashCard3Layout = new QHBoxLayout(dashCard3);
        dashCard3Layout->setSpacing(10);
        dashCard3Layout->setObjectName("dashCard3Layout");
        dashCard3Layout->setContentsMargins(12, 8, 12, 8);
        lblDashIcon3 = new QLabel(dashCard3);
        lblDashIcon3->setObjectName("lblDashIcon3");

        dashCard3Layout->addWidget(lblDashIcon3);

        dashText3Layout = new QVBoxLayout();
        dashText3Layout->setSpacing(2);
        dashText3Layout->setObjectName("dashText3Layout");
        lblDashValue3 = new QLabel(dashCard3);
        lblDashValue3->setObjectName("lblDashValue3");

        dashText3Layout->addWidget(lblDashValue3);

        lblDashLabel3 = new QLabel(dashCard3);
        lblDashLabel3->setObjectName("lblDashLabel3");

        dashText3Layout->addWidget(lblDashLabel3);


        dashCard3Layout->addLayout(dashText3Layout);


        dashboardLayout->addWidget(dashCard3);

        dashCard4 = new QWidget(dashboardWidget);
        dashCard4->setObjectName("dashCard4");
        dashCard4Layout = new QHBoxLayout(dashCard4);
        dashCard4Layout->setSpacing(10);
        dashCard4Layout->setObjectName("dashCard4Layout");
        dashCard4Layout->setContentsMargins(12, 8, 12, 8);
        lblDashIcon4 = new QLabel(dashCard4);
        lblDashIcon4->setObjectName("lblDashIcon4");

        dashCard4Layout->addWidget(lblDashIcon4);

        dashText4Layout = new QVBoxLayout();
        dashText4Layout->setSpacing(2);
        dashText4Layout->setObjectName("dashText4Layout");
        lblDashValue4 = new QLabel(dashCard4);
        lblDashValue4->setObjectName("lblDashValue4");

        dashText4Layout->addWidget(lblDashValue4);

        lblDashLabel4 = new QLabel(dashCard4);
        lblDashLabel4->setObjectName("lblDashLabel4");

        dashText4Layout->addWidget(lblDashLabel4);


        dashCard4Layout->addLayout(dashText4Layout);


        dashboardLayout->addWidget(dashCard4);


        mainLayout->addWidget(dashboardWidget);

        tabContainerWidget = new QWidget(centralwidget);
        tabContainerWidget->setObjectName("tabContainerWidget");
        tabContainerLayout = new QHBoxLayout(tabContainerWidget);
        tabContainerLayout->setSpacing(0);
        tabContainerLayout->setObjectName("tabContainerLayout");
        tabContainerLayout->setContentsMargins(25, 5, 25, 10);
        tabWidgetProduction = new QTabWidget(tabContainerWidget);
        tabWidgetProduction->setObjectName("tabWidgetProduction");
        tabPlanification = new QWidget();
        tabPlanification->setObjectName("tabPlanification");
        layoutPlanificationMain = new QHBoxLayout(tabPlanification);
        layoutPlanificationMain->setSpacing(15);
        layoutPlanificationMain->setObjectName("layoutPlanificationMain");
        layoutPlanificationMain->setContentsMargins(15, 15, 15, 15);
        layoutPlanLeft = new QVBoxLayout();
        layoutPlanLeft->setSpacing(15);
        layoutPlanLeft->setObjectName("layoutPlanLeft");
        groupProductionParams = new QGroupBox(tabPlanification);
        groupProductionParams->setObjectName("groupProductionParams");
        formProductionParams = new QFormLayout(groupProductionParams);
        formProductionParams->setObjectName("formProductionParams");
        formProductionParams->setLabelAlignment(Qt::AlignmentFlag::AlignRight|Qt::AlignmentFlag::AlignTrailing|Qt::AlignmentFlag::AlignVCenter);
        formProductionParams->setHorizontalSpacing(20);
        formProductionParams->setVerticalSpacing(15);
        formProductionParams->setContentsMargins(25, 30, 25, 20);
        lblDateProduction = new QLabel(groupProductionParams);
        lblDateProduction->setObjectName("lblDateProduction");

        formProductionParams->setWidget(0, QFormLayout::LabelRole, lblDateProduction);

        dateEditProduction = new QDateEdit(groupProductionParams);
        dateEditProduction->setObjectName("dateEditProduction");
        dateEditProduction->setMinimumSize(QSize(220, 44));
        dateEditProduction->setCalendarPopup(true);

        formProductionParams->setWidget(0, QFormLayout::FieldRole, dateEditProduction);

        lblOlivesQty = new QLabel(groupProductionParams);
        lblOlivesQty->setObjectName("lblOlivesQty");

        formProductionParams->setWidget(1, QFormLayout::LabelRole, lblOlivesQty);

        spinOlivesKg = new QSpinBox(groupProductionParams);
        spinOlivesKg->setObjectName("spinOlivesKg");
        spinOlivesKg->setMinimumSize(QSize(220, 44));
        spinOlivesKg->setMaximum(100000);
        spinOlivesKg->setValue(500);

        formProductionParams->setWidget(1, QFormLayout::FieldRole, spinOlivesKg);

        lblPressType = new QLabel(groupProductionParams);
        lblPressType->setObjectName("lblPressType");

        formProductionParams->setWidget(2, QFormLayout::LabelRole, lblPressType);

        comboPressType = new QComboBox(groupProductionParams);
        comboPressType->addItem(QString());
        comboPressType->addItem(QString());
        comboPressType->setObjectName("comboPressType");
        comboPressType->setMinimumSize(QSize(220, 44));

        formProductionParams->setWidget(2, QFormLayout::FieldRole, comboPressType);

        lblOliveVariety = new QLabel(groupProductionParams);
        lblOliveVariety->setObjectName("lblOliveVariety");

        formProductionParams->setWidget(3, QFormLayout::LabelRole, lblOliveVariety);

        comboOliveVariety = new QComboBox(groupProductionParams);
        comboOliveVariety->addItem(QString());
        comboOliveVariety->addItem(QString());
        comboOliveVariety->addItem(QString());
        comboOliveVariety->addItem(QString());
        comboOliveVariety->addItem(QString());
        comboOliveVariety->setObjectName("comboOliveVariety");
        comboOliveVariety->setMinimumSize(QSize(220, 44));

        formProductionParams->setWidget(3, QFormLayout::FieldRole, comboOliveVariety);


        layoutPlanLeft->addWidget(groupProductionParams);

        groupNotesPlanification = new QGroupBox(tabPlanification);
        groupNotesPlanification->setObjectName("groupNotesPlanification");
        layoutNotes = new QVBoxLayout(groupNotesPlanification);
        layoutNotes->setObjectName("layoutNotes");
        layoutNotes->setContentsMargins(20, 30, 20, 15);
        txtNotesPlanification = new QTextEdit(groupNotesPlanification);
        txtNotesPlanification->setObjectName("txtNotesPlanification");
        txtNotesPlanification->setMinimumSize(QSize(0, 100));

        layoutNotes->addWidget(txtNotesPlanification);


        layoutPlanLeft->addWidget(groupNotesPlanification);

        spacerPlanLeft = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        layoutPlanLeft->addItem(spacerPlanLeft);


        layoutPlanificationMain->addLayout(layoutPlanLeft);

        layoutPlanRight = new QVBoxLayout();
        layoutPlanRight->setSpacing(15);
        layoutPlanRight->setObjectName("layoutPlanRight");
        groupPlanStatus = new QGroupBox(tabPlanification);
        groupPlanStatus->setObjectName("groupPlanStatus");
        layoutPlanStatus = new QVBoxLayout(groupPlanStatus);
        layoutPlanStatus->setSpacing(15);
        layoutPlanStatus->setObjectName("layoutPlanStatus");
        layoutPlanStatus->setContentsMargins(20, 30, 20, 20);
        lblStatusPlanification = new QLabel(groupPlanStatus);
        lblStatusPlanification->setObjectName("lblStatusPlanification");
        lblStatusPlanification->setAlignment(Qt::AlignmentFlag::AlignCenter);

        layoutPlanStatus->addWidget(lblStatusPlanification);

        lblEstimatedYield = new QLabel(groupPlanStatus);
        lblEstimatedYield->setObjectName("lblEstimatedYield");
        lblEstimatedYield->setAlignment(Qt::AlignmentFlag::AlignCenter);

        layoutPlanStatus->addWidget(lblEstimatedYield);

        lblDuration = new QLabel(groupPlanStatus);
        lblDuration->setObjectName("lblDuration");
        lblDuration->setAlignment(Qt::AlignmentFlag::AlignCenter);

        layoutPlanStatus->addWidget(lblDuration);


        layoutPlanRight->addWidget(groupPlanStatus);

        groupQuickStats = new QGroupBox(tabPlanification);
        groupQuickStats->setObjectName("groupQuickStats");
        gridQuickStats = new QGridLayout(groupQuickStats);
        gridQuickStats->setObjectName("gridQuickStats");
        gridQuickStats->setHorizontalSpacing(15);
        gridQuickStats->setVerticalSpacing(10);
        gridQuickStats->setContentsMargins(20, 30, 20, 15);
        lblTodayProductions = new QLabel(groupQuickStats);
        lblTodayProductions->setObjectName("lblTodayProductions");

        gridQuickStats->addWidget(lblTodayProductions, 0, 0, 1, 1);

        lblTodayProductionsValue = new QLabel(groupQuickStats);
        lblTodayProductionsValue->setObjectName("lblTodayProductionsValue");
        lblTodayProductionsValue->setAlignment(Qt::AlignmentFlag::AlignRight|Qt::AlignmentFlag::AlignTrailing|Qt::AlignmentFlag::AlignVCenter);

        gridQuickStats->addWidget(lblTodayProductionsValue, 0, 1, 1, 1);

        lblWeekProductions = new QLabel(groupQuickStats);
        lblWeekProductions->setObjectName("lblWeekProductions");

        gridQuickStats->addWidget(lblWeekProductions, 1, 0, 1, 1);

        lblWeekProductionsValue = new QLabel(groupQuickStats);
        lblWeekProductionsValue->setObjectName("lblWeekProductionsValue");
        lblWeekProductionsValue->setAlignment(Qt::AlignmentFlag::AlignRight|Qt::AlignmentFlag::AlignTrailing|Qt::AlignmentFlag::AlignVCenter);

        gridQuickStats->addWidget(lblWeekProductionsValue, 1, 1, 1, 1);

        lblAvgYield = new QLabel(groupQuickStats);
        lblAvgYield->setObjectName("lblAvgYield");

        gridQuickStats->addWidget(lblAvgYield, 2, 0, 1, 1);

        lblAvgYieldValue = new QLabel(groupQuickStats);
        lblAvgYieldValue->setObjectName("lblAvgYieldValue");
        lblAvgYieldValue->setAlignment(Qt::AlignmentFlag::AlignRight|Qt::AlignmentFlag::AlignTrailing|Qt::AlignmentFlag::AlignVCenter);

        gridQuickStats->addWidget(lblAvgYieldValue, 2, 1, 1, 1);


        layoutPlanRight->addWidget(groupQuickStats);

        spacerPlanRight = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        layoutPlanRight->addItem(spacerPlanRight);

        btnPlanifier = new QPushButton(tabPlanification);
        btnPlanifier->setObjectName("btnPlanifier");
        btnPlanifier->setMinimumSize(QSize(168, 50));

        layoutPlanRight->addWidget(btnPlanifier);


        layoutPlanificationMain->addLayout(layoutPlanRight);

        tabWidgetProduction->addTab(tabPlanification, QString());
        tabLancement = new QWidget();
        tabLancement->setObjectName("tabLancement");
        layoutLancement = new QVBoxLayout(tabLancement);
        layoutLancement->setSpacing(0);
        layoutLancement->setObjectName("layoutLancement");
        layoutLancement->setContentsMargins(0, 0, 0, 0);
        scrollLancement = new QScrollArea(tabLancement);
        scrollLancement->setObjectName("scrollLancement");
        scrollLancement->setFrameShape(QFrame::Shape::NoFrame);
        scrollLancement->setWidgetResizable(true);
        scrollContentsLancement = new QWidget();
        scrollContentsLancement->setObjectName("scrollContentsLancement");
        scrollContentsLancement->setGeometry(QRect(0, 0, 1380, 596));
        layoutLancementInner = new QVBoxLayout(scrollContentsLancement);
        layoutLancementInner->setSpacing(16);
        layoutLancementInner->setObjectName("layoutLancementInner");
        layoutLancementInner->setContentsMargins(20, 16, 20, 20);
        layoutRow1 = new QHBoxLayout();
        layoutRow1->setSpacing(16);
        layoutRow1->setObjectName("layoutRow1");
        groupSelectProduction = new QGroupBox(scrollContentsLancement);
        groupSelectProduction->setObjectName("groupSelectProduction");
        groupSelectProduction->setMinimumSize(QSize(0, 160));
        groupSelectProduction->setAutoFillBackground(false);
        layoutSelectProd = new QVBoxLayout(groupSelectProduction);
        layoutSelectProd->setSpacing(12);
        layoutSelectProd->setObjectName("layoutSelectProd");
        layoutSelectProd->setContentsMargins(20, 18, 20, 18);
        layoutSelectRow = new QHBoxLayout();
        layoutSelectRow->setSpacing(10);
        layoutSelectRow->setObjectName("layoutSelectRow");
        comboSelectProduction = new QComboBox(groupSelectProduction);
        comboSelectProduction->setObjectName("comboSelectProduction");
        comboSelectProduction->setMinimumSize(QSize(0, 40));

        layoutSelectRow->addWidget(comboSelectProduction);

        btnRefreshPlanned = new QPushButton(groupSelectProduction);
        btnRefreshPlanned->setObjectName("btnRefreshPlanned");
        btnRefreshPlanned->setMinimumSize(QSize(172, 44));
        btnRefreshPlanned->setMaximumSize(QSize(44, 44));

        layoutSelectRow->addWidget(btnRefreshPlanned);


        layoutSelectProd->addLayout(layoutSelectRow);

        lblSelectedProductionInfo = new QLabel(groupSelectProduction);
        lblSelectedProductionInfo->setObjectName("lblSelectedProductionInfo");
        lblSelectedProductionInfo->setWordWrap(true);

        layoutSelectProd->addWidget(lblSelectedProductionInfo);


        layoutRow1->addWidget(groupSelectProduction);

        groupLancementStatus = new QGroupBox(scrollContentsLancement);
        groupLancementStatus->setObjectName("groupLancementStatus");
        groupLancementStatus->setMinimumSize(QSize(0, 160));
        layoutLancementStatus = new QHBoxLayout(groupLancementStatus);
        layoutLancementStatus->setSpacing(20);
        layoutLancementStatus->setObjectName("layoutLancementStatus");
        layoutLancementStatus->setContentsMargins(20, 18, 20, 18);
        layoutCurrentStatus = new QVBoxLayout();
        layoutCurrentStatus->setSpacing(6);
        layoutCurrentStatus->setObjectName("layoutCurrentStatus");
        lblCurrentStatusTitle = new QLabel(groupLancementStatus);
        lblCurrentStatusTitle->setObjectName("lblCurrentStatusTitle");

        layoutCurrentStatus->addWidget(lblCurrentStatusTitle);

        lblCurrentStatus = new QLabel(groupLancementStatus);
        lblCurrentStatus->setObjectName("lblCurrentStatus");
        lblCurrentStatus->setMinimumSize(QSize(0, 52));
        lblCurrentStatus->setAlignment(Qt::AlignmentFlag::AlignCenter);

        layoutCurrentStatus->addWidget(lblCurrentStatus);


        layoutLancementStatus->addLayout(layoutCurrentStatus);

        layoutStartTime = new QVBoxLayout();
        layoutStartTime->setSpacing(6);
        layoutStartTime->setObjectName("layoutStartTime");
        lblStartTimeTitle = new QLabel(groupLancementStatus);
        lblStartTimeTitle->setObjectName("lblStartTimeTitle");

        layoutStartTime->addWidget(lblStartTimeTitle);

        lblStartTime = new QLabel(groupLancementStatus);
        lblStartTime->setObjectName("lblStartTime");
        lblStartTime->setMinimumSize(QSize(0, 52));
        lblStartTime->setAlignment(Qt::AlignmentFlag::AlignCenter);

        layoutStartTime->addWidget(lblStartTime);


        layoutLancementStatus->addLayout(layoutStartTime);

        layoutElapsedTime = new QVBoxLayout();
        layoutElapsedTime->setSpacing(6);
        layoutElapsedTime->setObjectName("layoutElapsedTime");
        lblElapsedTimeTitle = new QLabel(groupLancementStatus);
        lblElapsedTimeTitle->setObjectName("lblElapsedTimeTitle");

        layoutElapsedTime->addWidget(lblElapsedTimeTitle);

        lblElapsedTime = new QLabel(groupLancementStatus);
        lblElapsedTime->setObjectName("lblElapsedTime");
        lblElapsedTime->setMinimumSize(QSize(0, 52));
        lblElapsedTime->setAlignment(Qt::AlignmentFlag::AlignCenter);

        layoutElapsedTime->addWidget(lblElapsedTime);


        layoutLancementStatus->addLayout(layoutElapsedTime);


        layoutRow1->addWidget(groupLancementStatus);


        layoutLancementInner->addLayout(layoutRow1);

        groupProgress = new QGroupBox(scrollContentsLancement);
        groupProgress->setObjectName("groupProgress");
        groupProgress->setMinimumSize(QSize(0, 140));
        layoutProgress = new QVBoxLayout(groupProgress);
        layoutProgress->setSpacing(14);
        layoutProgress->setObjectName("layoutProgress");
        layoutProgress->setContentsMargins(24, 18, 24, 20);
        progressProduction = new QProgressBar(groupProgress);
        progressProduction->setObjectName("progressProduction");
        progressProduction->setMinimumSize(QSize(0, 24));
        progressProduction->setValue(0);

        layoutProgress->addWidget(progressProduction);

        layoutProgressLabels = new QHBoxLayout();
        layoutProgressLabels->setSpacing(12);
        layoutProgressLabels->setObjectName("layoutProgressLabels");
        lblPhase = new QLabel(groupProgress);
        lblPhase->setObjectName("lblPhase");

        layoutProgressLabels->addWidget(lblPhase);

        spacerProgress = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        layoutProgressLabels->addItem(spacerProgress);

        lblProgressEnd = new QLabel(groupProgress);
        lblProgressEnd->setObjectName("lblProgressEnd");

        layoutProgressLabels->addWidget(lblProgressEnd);


        layoutProgress->addLayout(layoutProgressLabels);


        layoutLancementInner->addWidget(groupProgress);

        groupControls = new QGroupBox(scrollContentsLancement);
        groupControls->setObjectName("groupControls");
        groupControls->setMinimumSize(QSize(0, 120));
        layoutControls = new QHBoxLayout(groupControls);
        layoutControls->setSpacing(16);
        layoutControls->setObjectName("layoutControls");
        layoutControls->setContentsMargins(24, 18, 24, 18);
        spacerControlsLeft = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        layoutControls->addItem(spacerControlsLeft);

        btnStart = new QPushButton(groupControls);
        btnStart->setObjectName("btnStart");
        btnStart->setMinimumSize(QSize(168, 52));

        layoutControls->addWidget(btnStart);

        btnStop = new QPushButton(groupControls);
        btnStop->setObjectName("btnStop");
        btnStop->setMinimumSize(QSize(168, 52));

        layoutControls->addWidget(btnStop);

        btnFinishProduction = new QPushButton(groupControls);
        btnFinishProduction->setObjectName("btnFinishProduction");
        btnFinishProduction->setEnabled(false);
        btnFinishProduction->setMinimumSize(QSize(168, 52));

        layoutControls->addWidget(btnFinishProduction);

        spacerControlsRight = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        layoutControls->addItem(spacerControlsRight);


        layoutLancementInner->addWidget(groupControls);

        spacerLancement = new QSpacerItem(20, 20, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        layoutLancementInner->addItem(spacerLancement);

        scrollLancement->setWidget(scrollContentsLancement);

        layoutLancement->addWidget(scrollLancement);

        tabWidgetProduction->addTab(tabLancement, QString());
        tabSuivi = new QWidget();
        tabSuivi->setObjectName("tabSuivi");
        layoutSuivi = new QVBoxLayout(tabSuivi);
        layoutSuivi->setSpacing(15);
        layoutSuivi->setObjectName("layoutSuivi");
        layoutSuivi->setContentsMargins(15, 15, 15, 15);
        groupRunningProduction = new QGroupBox(tabSuivi);
        groupRunningProduction->setObjectName("groupRunningProduction");
        groupRunningProduction->setAutoFillBackground(false);
        layoutRunningProduction = new QVBoxLayout(groupRunningProduction);
        layoutRunningProduction->setSpacing(12);
        layoutRunningProduction->setObjectName("layoutRunningProduction");
        layoutRunningProduction->setContentsMargins(20, 16, 20, 16);
        lblRunningProduction = new QLabel(groupRunningProduction);
        lblRunningProduction->setObjectName("lblRunningProduction");
        lblRunningProduction->setWordWrap(true);

        layoutRunningProduction->addWidget(lblRunningProduction);

        widgetRunningActive = new QWidget(groupRunningProduction);
        widgetRunningActive->setObjectName("widgetRunningActive");
        widgetRunningActive->setVisible(false);
        layoutRunningActive = new QVBoxLayout(widgetRunningActive);
        layoutRunningActive->setSpacing(10);
        layoutRunningActive->setObjectName("layoutRunningActive");
        layoutRunningActive->setContentsMargins(0, 0, 0, 0);
        layoutRunningRow1 = new QHBoxLayout();
        layoutRunningRow1->setSpacing(12);
        layoutRunningRow1->setObjectName("layoutRunningRow1");
        lblRunningStatus = new QLabel(widgetRunningActive);
        lblRunningStatus->setObjectName("lblRunningStatus");
        lblRunningStatus->setMinimumSize(QSize(0, 44));
        lblRunningStatus->setAlignment(Qt::AlignmentFlag::AlignCenter);

        layoutRunningRow1->addWidget(lblRunningStatus);

        lblRunningPhase = new QLabel(widgetRunningActive);
        lblRunningPhase->setObjectName("lblRunningPhase");
        lblRunningPhase->setMinimumSize(QSize(0, 44));

        layoutRunningRow1->addWidget(lblRunningPhase);


        layoutRunningActive->addLayout(layoutRunningRow1);

        progressRunning = new QProgressBar(widgetRunningActive);
        progressRunning->setObjectName("progressRunning");
        progressRunning->setMinimumSize(QSize(0, 24));
        progressRunning->setValue(0);

        layoutRunningActive->addWidget(progressRunning);

        layoutRunningStats = new QHBoxLayout();
        layoutRunningStats->setSpacing(10);
        layoutRunningStats->setObjectName("layoutRunningStats");
        lblRunningOlives = new QLabel(widgetRunningActive);
        lblRunningOlives->setObjectName("lblRunningOlives");
        lblRunningOlives->setMinimumSize(QSize(0, 56));
        lblRunningOlives->setAlignment(Qt::AlignmentFlag::AlignCenter);

        layoutRunningStats->addWidget(lblRunningOlives);

        lblRunningOil = new QLabel(widgetRunningActive);
        lblRunningOil->setObjectName("lblRunningOil");
        lblRunningOil->setMinimumSize(QSize(0, 56));
        lblRunningOil->setAlignment(Qt::AlignmentFlag::AlignCenter);

        layoutRunningStats->addWidget(lblRunningOil);

        lblRunningRendement = new QLabel(widgetRunningActive);
        lblRunningRendement->setObjectName("lblRunningRendement");
        lblRunningRendement->setMinimumSize(QSize(0, 56));
        lblRunningRendement->setAlignment(Qt::AlignmentFlag::AlignCenter);

        layoutRunningStats->addWidget(lblRunningRendement);

        lblRunningSimTime = new QLabel(widgetRunningActive);
        lblRunningSimTime->setObjectName("lblRunningSimTime");
        lblRunningSimTime->setMinimumSize(QSize(0, 56));
        lblRunningSimTime->setAlignment(Qt::AlignmentFlag::AlignCenter);

        layoutRunningStats->addWidget(lblRunningSimTime);

        lblRunningRealTime = new QLabel(widgetRunningActive);
        lblRunningRealTime->setObjectName("lblRunningRealTime");
        lblRunningRealTime->setMinimumSize(QSize(0, 56));
        lblRunningRealTime->setAlignment(Qt::AlignmentFlag::AlignCenter);

        layoutRunningStats->addWidget(lblRunningRealTime);


        layoutRunningActive->addLayout(layoutRunningStats);


        layoutRunningProduction->addWidget(widgetRunningActive);


        layoutSuivi->addWidget(groupRunningProduction);

        groupHistory = new QGroupBox(tabSuivi);
        groupHistory->setObjectName("groupHistory");
        groupHistory->setAutoFillBackground(false);
        layoutHistory = new QVBoxLayout(groupHistory);
        layoutHistory->setObjectName("layoutHistory");
        layoutHistory->setContentsMargins(15, 16, 15, 15);
        tableProductionHistory = new QTableWidget(groupHistory);
        if (tableProductionHistory->columnCount() < 6)
            tableProductionHistory->setColumnCount(6);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        tableProductionHistory->setHorizontalHeaderItem(0, __qtablewidgetitem);
        QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
        tableProductionHistory->setHorizontalHeaderItem(1, __qtablewidgetitem1);
        QTableWidgetItem *__qtablewidgetitem2 = new QTableWidgetItem();
        tableProductionHistory->setHorizontalHeaderItem(2, __qtablewidgetitem2);
        QTableWidgetItem *__qtablewidgetitem3 = new QTableWidgetItem();
        tableProductionHistory->setHorizontalHeaderItem(3, __qtablewidgetitem3);
        QTableWidgetItem *__qtablewidgetitem4 = new QTableWidgetItem();
        tableProductionHistory->setHorizontalHeaderItem(4, __qtablewidgetitem4);
        QTableWidgetItem *__qtablewidgetitem5 = new QTableWidgetItem();
        tableProductionHistory->setHorizontalHeaderItem(5, __qtablewidgetitem5);
        tableProductionHistory->setObjectName("tableProductionHistory");
        tableProductionHistory->setMinimumSize(QSize(0, 200));
        tableProductionHistory->setAlternatingRowColors(true);
        tableProductionHistory->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
        tableProductionHistory->setShowGrid(false);

        layoutHistory->addWidget(tableProductionHistory);

        layoutHistoryButtons = new QHBoxLayout();
        layoutHistoryButtons->setObjectName("layoutHistoryButtons");
        spacerHistoryLeft = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        layoutHistoryButtons->addItem(spacerHistoryLeft);

        btnModifyProduction = new QPushButton(groupHistory);
        btnModifyProduction->setObjectName("btnModifyProduction");
        btnModifyProduction->setMinimumSize(QSize(168, 40));

        layoutHistoryButtons->addWidget(btnModifyProduction);

        btnDeleteProduction = new QPushButton(groupHistory);
        btnDeleteProduction->setObjectName("btnDeleteProduction");
        btnDeleteProduction->setMinimumSize(QSize(168, 40));

        layoutHistoryButtons->addWidget(btnDeleteProduction);


        layoutHistory->addLayout(layoutHistoryButtons);


        layoutSuivi->addWidget(groupHistory);

        layoutSuiviFooter = new QHBoxLayout();
        layoutSuiviFooter->setObjectName("layoutSuiviFooter");
        spacerSuiviFooter = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        layoutSuiviFooter->addItem(spacerSuiviFooter);

        btnUpdateMonitoring = new QPushButton(tabSuivi);
        btnUpdateMonitoring->setObjectName("btnUpdateMonitoring");
        btnUpdateMonitoring->setMinimumSize(QSize(168, 45));

        layoutSuiviFooter->addWidget(btnUpdateMonitoring);


        layoutSuivi->addLayout(layoutSuiviFooter);

        tabWidgetProduction->addTab(tabSuivi, QString());
        tabRendement = new QWidget();
        tabRendement->setObjectName("tabRendement");
        layoutRendementMain = new QHBoxLayout(tabRendement);
        layoutRendementMain->setSpacing(15);
        layoutRendementMain->setObjectName("layoutRendementMain");
        layoutRendementMain->setContentsMargins(15, 15, 15, 15);
        layoutRendementLeft = new QVBoxLayout();
        layoutRendementLeft->setSpacing(15);
        layoutRendementLeft->setObjectName("layoutRendementLeft");
        groupSelectTerminated = new QGroupBox(tabRendement);
        groupSelectTerminated->setObjectName("groupSelectTerminated");
        groupSelectTerminated->setMinimumSize(QSize(0, 130));
        groupSelectTerminated->setAutoFillBackground(false);
        layoutSelectTerminated = new QVBoxLayout(groupSelectTerminated);
        layoutSelectTerminated->setSpacing(10);
        layoutSelectTerminated->setObjectName("layoutSelectTerminated");
        layoutSelectTerminated->setContentsMargins(16, 16, 16, 14);
        layoutTerminatedRow = new QHBoxLayout();
        layoutTerminatedRow->setSpacing(10);
        layoutTerminatedRow->setObjectName("layoutTerminatedRow");
        comboTerminatedProductions = new QComboBox(groupSelectTerminated);
        comboTerminatedProductions->setObjectName("comboTerminatedProductions");
        comboTerminatedProductions->setMinimumSize(QSize(0, 40));

        layoutTerminatedRow->addWidget(comboTerminatedProductions);

        btnRefreshTerminated = new QPushButton(groupSelectTerminated);
        btnRefreshTerminated->setObjectName("btnRefreshTerminated");
        btnRefreshTerminated->setMinimumSize(QSize(172, 42));
        btnRefreshTerminated->setMaximumSize(QSize(42, 42));

        layoutTerminatedRow->addWidget(btnRefreshTerminated);


        layoutSelectTerminated->addLayout(layoutTerminatedRow);

        lblTerminatedInfo = new QLabel(groupSelectTerminated);
        lblTerminatedInfo->setObjectName("lblTerminatedInfo");
        lblTerminatedInfo->setWordWrap(true);

        layoutSelectTerminated->addWidget(lblTerminatedInfo);


        layoutRendementLeft->addWidget(groupSelectTerminated);

        groupRendementInput = new QGroupBox(tabRendement);
        groupRendementInput->setObjectName("groupRendementInput");
        formRendementInput = new QFormLayout(groupRendementInput);
        formRendementInput->setObjectName("formRendementInput");
        formRendementInput->setLabelAlignment(Qt::AlignmentFlag::AlignRight|Qt::AlignmentFlag::AlignTrailing|Qt::AlignmentFlag::AlignVCenter);
        formRendementInput->setHorizontalSpacing(20);
        formRendementInput->setVerticalSpacing(20);
        formRendementInput->setContentsMargins(25, 35, 25, 25);
        lblOlivesUsedTitle = new QLabel(groupRendementInput);
        lblOlivesUsedTitle->setObjectName("lblOlivesUsedTitle");

        formRendementInput->setWidget(0, QFormLayout::LabelRole, lblOlivesUsedTitle);

        lblOlivesUsed = new QLabel(groupRendementInput);
        lblOlivesUsed->setObjectName("lblOlivesUsed");

        formRendementInput->setWidget(0, QFormLayout::FieldRole, lblOlivesUsed);

        lblOilProducedTitle = new QLabel(groupRendementInput);
        lblOilProducedTitle->setObjectName("lblOilProducedTitle");

        formRendementInput->setWidget(1, QFormLayout::LabelRole, lblOilProducedTitle);

        spinOilProduced = new QDoubleSpinBox(groupRendementInput);
        spinOilProduced->setObjectName("spinOilProduced");
        spinOilProduced->setMinimumSize(QSize(200, 44));
        spinOilProduced->setDecimals(2);
        spinOilProduced->setMaximum(99999.990000000005239);

        formRendementInput->setWidget(1, QFormLayout::FieldRole, spinOilProduced);


        layoutRendementLeft->addWidget(groupRendementInput);

        btnCalculateYield = new QPushButton(tabRendement);
        btnCalculateYield->setObjectName("btnCalculateYield");
        btnCalculateYield->setMinimumSize(QSize(168, 50));

        layoutRendementLeft->addWidget(btnCalculateYield);

        spacerRendementLeft = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        layoutRendementLeft->addItem(spacerRendementLeft);


        layoutRendementMain->addLayout(layoutRendementLeft);

        layoutRendementRight = new QVBoxLayout();
        layoutRendementRight->setSpacing(15);
        layoutRendementRight->setObjectName("layoutRendementRight");
        groupRendementResult = new QGroupBox(tabRendement);
        groupRendementResult->setObjectName("groupRendementResult");
        layoutRendementResult = new QVBoxLayout(groupRendementResult);
        layoutRendementResult->setSpacing(20);
        layoutRendementResult->setObjectName("layoutRendementResult");
        layoutRendementResult->setContentsMargins(25, 35, 25, 25);
        lblYield = new QLabel(groupRendementResult);
        lblYield->setObjectName("lblYield");
        lblYield->setAlignment(Qt::AlignmentFlag::AlignCenter);

        layoutRendementResult->addWidget(lblYield);

        progressYield = new QProgressBar(groupRendementResult);
        progressYield->setObjectName("progressYield");
        progressYield->setMinimumSize(QSize(0, 24));
        progressYield->setValue(0);

        layoutRendementResult->addWidget(progressYield);


        layoutRendementRight->addWidget(groupRendementResult);

        groupYieldMessage = new QGroupBox(tabRendement);
        groupYieldMessage->setObjectName("groupYieldMessage");
        layoutYieldMessage = new QVBoxLayout(groupYieldMessage);
        layoutYieldMessage->setObjectName("layoutYieldMessage");
        layoutYieldMessage->setContentsMargins(20, 30, 20, 15);
        lblYieldMessage = new QLabel(groupYieldMessage);
        lblYieldMessage->setObjectName("lblYieldMessage");
        lblYieldMessage->setWordWrap(true);

        layoutYieldMessage->addWidget(lblYieldMessage);


        layoutRendementRight->addWidget(groupYieldMessage);

        spacerRendementRight = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        layoutRendementRight->addItem(spacerRendementRight);


        layoutRendementMain->addLayout(layoutRendementRight);

        tabWidgetProduction->addTab(tabRendement, QString());
        tabQualite = new QWidget();
        tabQualite->setObjectName("tabQualite");
        layoutQualite = new QVBoxLayout(tabQualite);
        layoutQualite->setSpacing(24);
        layoutQualite->setObjectName("layoutQualite");
        layoutQualite->setContentsMargins(24, 24, 24, 24);
        layoutQualityMain = new QGridLayout();
        layoutQualityMain->setSpacing(24);
        layoutQualityMain->setObjectName("layoutQualityMain");
        groupQualityEval = new QGroupBox(tabQualite);
        groupQualityEval->setObjectName("groupQualityEval");
        QSizePolicy sizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(groupQualityEval->sizePolicy().hasHeightForWidth());
        groupQualityEval->setSizePolicy(sizePolicy);
        layoutQualityEval = new QVBoxLayout(groupQualityEval);
        layoutQualityEval->setSpacing(24);
        layoutQualityEval->setObjectName("layoutQualityEval");
        layoutQualityEval->setContentsMargins(24, 32, 24, 24);
        formQuality = new QFormLayout();
        formQuality->setObjectName("formQuality");
        formQuality->setFieldGrowthPolicy(QFormLayout::FieldGrowthPolicy::ExpandingFieldsGrow);
        formQuality->setLabelAlignment(Qt::AlignmentFlag::AlignLeading|Qt::AlignmentFlag::AlignLeft|Qt::AlignmentFlag::AlignVCenter);
        formQuality->setHorizontalSpacing(16);
        formQuality->setVerticalSpacing(16);
        lblOilQuality = new QLabel(groupQualityEval);
        lblOilQuality->setObjectName("lblOilQuality");

        formQuality->setWidget(0, QFormLayout::LabelRole, lblOilQuality);

        comboOilQuality = new QComboBox(groupQualityEval);
        comboOilQuality->addItem(QString());
        comboOilQuality->addItem(QString());
        comboOilQuality->addItem(QString());
        comboOilQuality->addItem(QString());
        comboOilQuality->setObjectName("comboOilQuality");
        comboOilQuality->setMinimumSize(QSize(260, 62));

        formQuality->setWidget(0, QFormLayout::FieldRole, comboOilQuality);

        lblAcidity = new QLabel(groupQualityEval);
        lblAcidity->setObjectName("lblAcidity");

        formQuality->setWidget(1, QFormLayout::LabelRole, lblAcidity);

        comboAcidity = new QDoubleSpinBox(groupQualityEval);
        comboAcidity->setObjectName("comboAcidity");
        comboAcidity->setMinimumSize(QSize(260, 62));
        comboAcidity->setDecimals(2);
        comboAcidity->setMinimum(0.100000000000000);
        comboAcidity->setMaximum(5.000000000000000);
        comboAcidity->setSingleStep(0.050000000000000);
        comboAcidity->setValue(0.800000000000000);

        formQuality->setWidget(1, QFormLayout::FieldRole, comboAcidity);


        layoutQualityEval->addLayout(formQuality);

        checkConformity = new QCheckBox(groupQualityEval);
        checkConformity->setObjectName("checkConformity");

        layoutQualityEval->addWidget(checkConformity);


        layoutQualityMain->addWidget(groupQualityEval, 0, 0, 1, 1);

        groupQualityNotes = new QGroupBox(tabQualite);
        groupQualityNotes->setObjectName("groupQualityNotes");
        sizePolicy.setHeightForWidth(groupQualityNotes->sizePolicy().hasHeightForWidth());
        groupQualityNotes->setSizePolicy(sizePolicy);
        layoutQualityNotes = new QVBoxLayout(groupQualityNotes);
        layoutQualityNotes->setObjectName("layoutQualityNotes");
        layoutQualityNotes->setContentsMargins(24, 32, 24, 24);
        txtQualityNotes = new QTextEdit(groupQualityNotes);
        txtQualityNotes->setObjectName("txtQualityNotes");
        QSizePolicy sizePolicy1(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(txtQualityNotes->sizePolicy().hasHeightForWidth());
        txtQualityNotes->setSizePolicy(sizePolicy1);
        txtQualityNotes->setMinimumSize(QSize(0, 200));

        layoutQualityNotes->addWidget(txtQualityNotes);


        layoutQualityMain->addWidget(groupQualityNotes, 0, 1, 1, 1);


        layoutQualite->addLayout(layoutQualityMain);

        groupQualitySummary = new QGroupBox(tabQualite);
        groupQualitySummary->setObjectName("groupQualitySummary");
        layoutQualitySummary = new QVBoxLayout(groupQualitySummary);
        layoutQualitySummary->setObjectName("layoutQualitySummary");
        layoutQualitySummary->setContentsMargins(24, 32, 24, 24);
        lblQualitySummary = new QLabel(groupQualitySummary);
        lblQualitySummary->setObjectName("lblQualitySummary");
        lblQualitySummary->setWordWrap(true);

        layoutQualitySummary->addWidget(lblQualitySummary);


        layoutQualite->addWidget(groupQualitySummary);

        qualityFooterDivider = new QFrame(tabQualite);
        qualityFooterDivider->setObjectName("qualityFooterDivider");
        qualityFooterDivider->setFrameShape(QFrame::Shape::HLine);
        qualityFooterDivider->setFrameShadow(QFrame::Shadow::Plain);

        layoutQualite->addWidget(qualityFooterDivider);

        layoutQualiteFooter = new QHBoxLayout();
        layoutQualiteFooter->setSpacing(16);
        layoutQualiteFooter->setObjectName("layoutQualiteFooter");
        layoutQualiteFooter->setContentsMargins(-1, 16, -1, -1);
        spacerQualiteFooter = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        layoutQualiteFooter->addItem(spacerQualiteFooter);

        btnValidateQuality = new QPushButton(tabQualite);
        btnValidateQuality->setObjectName("btnValidateQuality");
        btnValidateQuality->setMinimumSize(QSize(168, 56));

        layoutQualiteFooter->addWidget(btnValidateQuality);


        layoutQualite->addLayout(layoutQualiteFooter);

        tabWidgetProduction->addTab(tabQualite, QString());
        tabRapports = new QWidget();
        tabRapports->setObjectName("tabRapports");
        layoutRapports = new QVBoxLayout(tabRapports);
        layoutRapports->setSpacing(15);
        layoutRapports->setObjectName("layoutRapports");
        layoutRapports->setContentsMargins(15, 15, 15, 15);
        groupReportSummary = new QGroupBox(tabRapports);
        groupReportSummary->setObjectName("groupReportSummary");
        layoutReportSummary = new QVBoxLayout(groupReportSummary);
        layoutReportSummary->setObjectName("layoutReportSummary");
        layoutReportSummary->setContentsMargins(20, 30, 20, 15);
        lblProductionSummary = new QLabel(groupReportSummary);
        lblProductionSummary->setObjectName("lblProductionSummary");
        lblProductionSummary->setWordWrap(true);

        layoutReportSummary->addWidget(lblProductionSummary);


        layoutRapports->addWidget(groupReportSummary);

        groupReportContent = new QGroupBox(tabRapports);
        groupReportContent->setObjectName("groupReportContent");
        layoutReportContent = new QVBoxLayout(groupReportContent);
        layoutReportContent->setObjectName("layoutReportContent");
        layoutReportContent->setContentsMargins(15, 30, 15, 15);
        textReport = new QTextBrowser(groupReportContent);
        textReport->setObjectName("textReport");
        textReport->setMinimumSize(QSize(0, 200));

        layoutReportContent->addWidget(textReport);


        layoutRapports->addWidget(groupReportContent);

        layoutReportActions = new QHBoxLayout();
        layoutReportActions->setSpacing(15);
        layoutReportActions->setObjectName("layoutReportActions");
        lblReportDate = new QLabel(tabRapports);
        lblReportDate->setObjectName("lblReportDate");

        layoutReportActions->addWidget(lblReportDate);

        spacerReportActions = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        layoutReportActions->addItem(spacerReportActions);

        btnGenerateReport = new QPushButton(tabRapports);
        btnGenerateReport->setObjectName("btnGenerateReport");
        btnGenerateReport->setMinimumSize(QSize(168, 50));

        layoutReportActions->addWidget(btnGenerateReport);

        btnExportPDF = new QPushButton(tabRapports);
        btnExportPDF->setObjectName("btnExportPDF");
        btnExportPDF->setMinimumSize(QSize(168, 50));

        layoutReportActions->addWidget(btnExportPDF);


        layoutRapports->addLayout(layoutReportActions);

        tabWidgetProduction->addTab(tabRapports, QString());

        tabContainerLayout->addWidget(tabWidgetProduction);


        mainLayout->addWidget(tabContainerWidget);

        Production->setCentralWidget(centralwidget);

        retranslateUi(Production);

        tabWidgetProduction->setCurrentIndex(4);


        QMetaObject::connectSlotsByName(Production);
    } // setupUi

    void retranslateUi(QMainWindow *Production)
    {
        Production->setWindowTitle(QCoreApplication::translate("Production", "PressIQ - Gestion de la Production v2.0", nullptr));
        btnBackToMenu->setText(QCoreApplication::translate("Production", "\342\206\220 Retour au menu", nullptr));
        lblLogo->setText(QString());
        lblAppTitle->setText(QCoreApplication::translate("Production", "Gestion de la Production", nullptr));
        lblAppSubtitle->setText(QCoreApplication::translate("Production", "MODULE DE PRODUCTION INTELLIGENT", nullptr));
        lblDateTime->setStyleSheet(QCoreApplication::translate("Production", "color: rgba(255,255,255,0.8); font-size: 13px; padding: 8px 15px; background: rgba(0,0,0,0.2); border-radius: 15px;", nullptr));
        lblDateTime->setText(QCoreApplication::translate("Production", "\360\237\223\205 --/--/---- | --:--", nullptr));
        btnDarkMode->setStyleSheet(QCoreApplication::translate("Production", "QPushButton { background: rgba(255,255,255,0.15); color: white; font-size: 12px; font-weight: 600; padding: 6px 14px; border-radius: 8px; border: 1px solid rgba(255,255,255,0.3); min-width: 0; } QPushButton:hover { background: rgba(255,255,255,0.28); border-color: rgba(255,255,255,0.55); } QPushButton:pressed { background: rgba(201,162,39,0.35); border-color: #C9A227; color: #C9A227; }", nullptr));
        btnDarkMode->setText(QCoreApplication::translate("Production", "\360\237\214\231  Mode Sombre", nullptr));
        btnLanguage->setStyleSheet(QCoreApplication::translate("Production", "QPushButton { background: rgba(255,255,255,0.15); color: white; font-size: 12px; font-weight: 600; padding: 6px 14px; border-radius: 8px; border: 1px solid rgba(255,255,255,0.3); min-width: 0; } QPushButton:hover { background: rgba(255,255,255,0.28); border-color: rgba(255,255,255,0.55); } QPushButton:pressed { background: rgba(59,130,246,0.35); border-color: #3B82F6; color: #93c5fd; }", nullptr));
        btnLanguage->setText(QCoreApplication::translate("Production", "\360\237\214\220  FR", nullptr));
        lblOnlineStatus->setText(QCoreApplication::translate("Production", "\342\227\217 EN LIGNE", nullptr));
        lblDashIcon1->setStyleSheet(QCoreApplication::translate("Production", "font-size: 20px; color: #4ADE80; background: rgba(74, 222, 128, 0.1); padding: 6px 10px; border-radius: 6px;", nullptr));
        lblDashIcon1->setText(QCoreApplication::translate("Production", "\342\226\266", nullptr));
        lblDashValue1->setStyleSheet(QCoreApplication::translate("Production", "color: #1B4332; font-size: 15px; font-weight: 700;", nullptr));
        lblDashValue1->setText(QCoreApplication::translate("Production", "En attente", nullptr));
        lblDashLabel1->setStyleSheet(QCoreApplication::translate("Production", "color: #9ca3af; font-size: 10px; font-weight: 600; letter-spacing: 1px;", nullptr));
        lblDashLabel1->setText(QCoreApplication::translate("Production", "STATUT PRODUCTION", nullptr));
        lblDashIcon2->setStyleSheet(QCoreApplication::translate("Production", "font-size: 20px; background: rgba(59, 130, 246, 0.1); padding: 6px 10px; border-radius: 6px;", nullptr));
        lblDashIcon2->setText(QCoreApplication::translate("Production", "\360\237\253\222", nullptr));
        lblDashValue2->setStyleSheet(QCoreApplication::translate("Production", "color: #3B82F6; font-size: 15px; font-weight: 700;", nullptr));
        lblDashValue2->setText(QCoreApplication::translate("Production", "0 kg", nullptr));
        lblDashLabel2->setStyleSheet(QCoreApplication::translate("Production", "color: #9ca3af; font-size: 10px; font-weight: 600; letter-spacing: 1px;", nullptr));
        lblDashLabel2->setText(QCoreApplication::translate("Production", "OLIVES PLANIFI\303\211ES", nullptr));
        lblDashIcon3->setStyleSheet(QCoreApplication::translate("Production", "font-size: 20px; background: rgba(245, 158, 11, 0.1); padding: 6px 10px; border-radius: 6px;", nullptr));
        lblDashIcon3->setText(QCoreApplication::translate("Production", "\360\237\223\212", nullptr));
        lblDashValue3->setStyleSheet(QCoreApplication::translate("Production", "color: #F59E0B; font-size: 15px; font-weight: 700;", nullptr));
        lblDashValue3->setText(QCoreApplication::translate("Production", "-- %", nullptr));
        lblDashLabel3->setStyleSheet(QCoreApplication::translate("Production", "color: #9ca3af; font-size: 10px; font-weight: 600; letter-spacing: 1px;", nullptr));
        lblDashLabel3->setText(QCoreApplication::translate("Production", "RENDEMENT ESTIM\303\211", nullptr));
        lblDashIcon4->setStyleSheet(QCoreApplication::translate("Production", "font-size: 20px; background: rgba(239, 68, 68, 0.1); padding: 6px 10px; border-radius: 6px;", nullptr));
        lblDashIcon4->setText(QCoreApplication::translate("Production", "\360\237\224\224", nullptr));
        lblDashValue4->setStyleSheet(QCoreApplication::translate("Production", "color: #4ADE80; font-size: 15px; font-weight: 700;", nullptr));
        lblDashValue4->setText(QCoreApplication::translate("Production", "0", nullptr));
        lblDashLabel4->setStyleSheet(QCoreApplication::translate("Production", "color: #9ca3af; font-size: 10px; font-weight: 600; letter-spacing: 1px;", nullptr));
        lblDashLabel4->setText(QCoreApplication::translate("Production", "ALERTES ACTIVES", nullptr));
        groupProductionParams->setTitle(QCoreApplication::translate("Production", "  Param\303\250tres de production", nullptr));
        lblDateProduction->setStyleSheet(QCoreApplication::translate("Production", "font-weight: 600;", nullptr));
        lblDateProduction->setText(QCoreApplication::translate("Production", "\360\237\223\205 Date de production :", nullptr));
        lblOlivesQty->setStyleSheet(QCoreApplication::translate("Production", "font-weight: 600;", nullptr));
        lblOlivesQty->setText(QCoreApplication::translate("Production", "\342\232\226 Quantit\303\251 d'olives :", nullptr));
        spinOlivesKg->setSuffix(QCoreApplication::translate("Production", "  kg", nullptr));
        lblPressType->setStyleSheet(QCoreApplication::translate("Production", "font-weight: 600;", nullptr));
        lblPressType->setText(QCoreApplication::translate("Production", "\360\237\224\247 Type de pressage :", nullptr));
        comboPressType->setItemText(0, QCoreApplication::translate("Production", "\342\235\204  Pressage \303\240 froid", nullptr));
        comboPressType->setItemText(1, QCoreApplication::translate("Production", "\360\237\224\245  Pressage \303\240 chaud", nullptr));

        lblOliveVariety->setStyleSheet(QCoreApplication::translate("Production", "font-weight: 600;", nullptr));
        lblOliveVariety->setText(QCoreApplication::translate("Production", "\360\237\253\222 Vari\303\251t\303\251 d'olive :", nullptr));
        comboOliveVariety->setItemText(0, QCoreApplication::translate("Production", "Chemlali", nullptr));
        comboOliveVariety->setItemText(1, QCoreApplication::translate("Production", "Ch\303\251toui", nullptr));
        comboOliveVariety->setItemText(2, QCoreApplication::translate("Production", "Oueslati", nullptr));
        comboOliveVariety->setItemText(3, QCoreApplication::translate("Production", "Zalmati", nullptr));
        comboOliveVariety->setItemText(4, QCoreApplication::translate("Production", "M\303\251lange", nullptr));

        groupNotesPlanification->setTitle(QCoreApplication::translate("Production", "  Notes et instructions", nullptr));
        txtNotesPlanification->setPlaceholderText(QCoreApplication::translate("Production", "Saisir les notes de planification (qualit\303\251 des olives, conditions particuli\303\250res, instructions sp\303\251ciales...)\n"
"\n"
"Exemple: Olives fra\303\256chement r\303\251colt\303\251es, maturit\303\251 optimale, premi\303\250re pression de la saison...", nullptr));
        groupPlanStatus->setTitle(QCoreApplication::translate("Production", "  \303\211tat de la planification", nullptr));
        lblStatusPlanification->setStyleSheet(QCoreApplication::translate("Production", "background-color: #FEF3C7; color: #92400E; border: 2px solid #FCD34D; border-radius: 8px; font-size: 15px;", nullptr));
        lblStatusPlanification->setText(QCoreApplication::translate("Production", "\342\217\263  En attente de planification", nullptr));
        lblEstimatedYield->setStyleSheet(QCoreApplication::translate("Production", "background-color: #f0fdf4; color: #166534; padding: 12px; border-radius: 8px; border: 1px solid #86efac;", nullptr));
        lblEstimatedYield->setText(QCoreApplication::translate("Production", "\360\237\223\212 Rendement estim\303\251 : -- %", nullptr));
        lblDuration->setStyleSheet(QCoreApplication::translate("Production", "background-color: #eff6ff; color: #1e40af; padding: 12px; border-radius: 8px; border: 1px solid #93c5fd;", nullptr));
        lblDuration->setText(QCoreApplication::translate("Production", "\342\217\261 Dur\303\251e estim\303\251e : --", nullptr));
        groupQuickStats->setTitle(QCoreApplication::translate("Production", "  Statistiques rapides", nullptr));
        lblTodayProductions->setStyleSheet(QCoreApplication::translate("Production", "color: #6b7280;", nullptr));
        lblTodayProductions->setText(QCoreApplication::translate("Production", "Productions aujourd'hui", nullptr));
        lblTodayProductionsValue->setStyleSheet(QCoreApplication::translate("Production", "font-weight: 700; color: #1B4332; font-size: 16px;", nullptr));
        lblTodayProductionsValue->setText(QCoreApplication::translate("Production", "--", nullptr));
        lblWeekProductions->setStyleSheet(QCoreApplication::translate("Production", "color: #6b7280;", nullptr));
        lblWeekProductions->setText(QCoreApplication::translate("Production", "Cette semaine", nullptr));
        lblWeekProductionsValue->setStyleSheet(QCoreApplication::translate("Production", "font-weight: 700; color: #1B4332; font-size: 16px;", nullptr));
        lblWeekProductionsValue->setText(QCoreApplication::translate("Production", "--", nullptr));
        lblAvgYield->setStyleSheet(QCoreApplication::translate("Production", "color: #6b7280;", nullptr));
        lblAvgYield->setText(QCoreApplication::translate("Production", "Rendement moyen", nullptr));
        lblAvgYieldValue->setStyleSheet(QCoreApplication::translate("Production", "font-weight: 700; color: #059669; font-size: 16px;", nullptr));
        lblAvgYieldValue->setText(QCoreApplication::translate("Production", "-- %", nullptr));
        btnPlanifier->setText(QCoreApplication::translate("Production", "\342\234\223  Planifier la production", nullptr));
        tabWidgetProduction->setTabText(tabWidgetProduction->indexOf(tabPlanification), QCoreApplication::translate("Production", "\360\237\223\213  Planification", nullptr));
        scrollLancement->setStyleSheet(QCoreApplication::translate("Production", "QScrollArea { background: transparent; } QScrollArea > QWidget > QWidget { background: transparent; }", nullptr));
        scrollContentsLancement->setStyleSheet(QCoreApplication::translate("Production", "background: transparent;", nullptr));
        groupSelectProduction->setStyleSheet(QCoreApplication::translate("Production", "QGroupBox { background-color: #f8fafc; border: 2px solid #1B4332; border-radius: 12px; margin-top: 22px; } QGroupBox::title { subcontrol-origin: margin; subcontrol-position: top left; left: 14px; top: -11px; padding: 5px 16px; background-color: #1B4332; color: #ffffff; font-size: 13px; font-weight: 700; border-radius: 8px; } QComboBox { background-color: #ffffff; color: #111827; border: 2px solid #1B4332; border-radius: 10px; padding: 8px 14px; font-size: 13px; font-weight: 600; } QComboBox:hover { border-color: #234E3E; background-color: #f0fdf4; } QComboBox::drop-down { border: none; width: 30px; background: transparent; } QComboBox QAbstractItemView { background-color: #ffffff; color: #111827; border: 2px solid #1B4332; border-radius: 8px; selection-background-color: #1B4332; selection-color: #ffffff; font-size: 13px; padding: 4px; } QPushButton#btnRefreshPlanned { background-color: #ffffff; border: 2px solid #1B4332; border-radius: 10px; font-size: 20px; color: #1B4332; } QPushButton#btnRefreshPlanned:hove"
                        "r { background-color: #f0fdf4; }", nullptr));
        groupSelectProduction->setTitle(QCoreApplication::translate("Production", " S\303\251lectionner une production planifi\303\251e ", nullptr));
#if QT_CONFIG(tooltip)
        btnRefreshPlanned->setToolTip(QCoreApplication::translate("Production", "Actualiser la liste", nullptr));
#endif // QT_CONFIG(tooltip)
        btnRefreshPlanned->setText(QCoreApplication::translate("Production", "\342\206\273", nullptr));
        lblSelectedProductionInfo->setStyleSheet(QCoreApplication::translate("Production", "color: #374151; font-size: 12px; font-weight: 600; padding: 6px 10px; background-color: #ffffff; border-radius: 6px; border: 1px solid #d1d5db;", nullptr));
        lblSelectedProductionInfo->setText(QCoreApplication::translate("Production", "Aucune production s\303\251lectionn\303\251e", nullptr));
        groupLancementStatus->setStyleSheet(QCoreApplication::translate("Production", "QGroupBox { background: white; border: 2px solid #d1fae5; border-radius: 12px; margin-top: 20px; } QGroupBox::title { subcontrol-origin: margin; subcontrol-position: top left; left: 14px; top: -2px; padding: 5px 14px; background: #1B4332; color: white; font-size: 13px; font-weight: 700; border-radius: 8px; }", nullptr));
        groupLancementStatus->setTitle(QCoreApplication::translate("Production", " \303\211tat de la production ", nullptr));
        lblCurrentStatusTitle->setStyleSheet(QCoreApplication::translate("Production", "font-weight: 700; color: #6b7280; font-size: 11px; letter-spacing: 1.5px; text-transform: uppercase;", nullptr));
        lblCurrentStatusTitle->setText(QCoreApplication::translate("Production", "STATUT ACTUEL", nullptr));
        lblCurrentStatus->setStyleSheet(QCoreApplication::translate("Production", "background-color: #D1FAE5; color: #065F46; border: 2px solid #34D399; font-size: 15px; font-weight: 700; padding: 10px 18px; border-radius: 10px;", nullptr));
        lblCurrentStatus->setText(QCoreApplication::translate("Production", "\360\237\237\242  Pr\303\252t \303\240 d\303\251marrer", nullptr));
        lblStartTimeTitle->setStyleSheet(QCoreApplication::translate("Production", "font-weight: 700; color: #6b7280; font-size: 11px; letter-spacing: 1.5px;", nullptr));
        lblStartTimeTitle->setText(QCoreApplication::translate("Production", "TEMPS SIMUL\303\211", nullptr));
        lblStartTime->setStyleSheet(QCoreApplication::translate("Production", "background-color: #f3f4f6; color: #1f2937; padding: 10px 18px; font-size: 15px; font-weight: 700; border-radius: 10px; border: 2px solid #e5e7eb;", nullptr));
        lblStartTime->setText(QCoreApplication::translate("Production", "\360\237\217\255  0h 00min", nullptr));
        lblElapsedTimeTitle->setStyleSheet(QCoreApplication::translate("Production", "font-weight: 700; color: #6b7280; font-size: 11px; letter-spacing: 1.5px;", nullptr));
        lblElapsedTimeTitle->setText(QCoreApplication::translate("Production", "TEMPS R\303\211EL \303\211COUL\303\211", nullptr));
        lblElapsedTime->setStyleSheet(QCoreApplication::translate("Production", "background-color: #FEF3C7; color: #92400E; padding: 10px 18px; font-size: 15px; font-weight: 700; border-radius: 10px; border: 2px solid #FCD34D;", nullptr));
        lblElapsedTime->setText(QCoreApplication::translate("Production", "\342\217\261  00:00:00", nullptr));
        groupProgress->setStyleSheet(QCoreApplication::translate("Production", "QGroupBox { background: white; border: 2px solid #d1fae5; border-radius: 12px; margin-top: 20px; } QGroupBox::title { subcontrol-origin: margin; subcontrol-position: top left; left: 14px; top: -2px; padding: 5px 14px; background: #1B4332; color: white; font-size: 13px; font-weight: 700; border-radius: 8px; }", nullptr));
        groupProgress->setTitle(QCoreApplication::translate("Production", " Progression de la production ", nullptr));
        progressProduction->setStyleSheet(QCoreApplication::translate("Production", "QProgressBar { background: #e5e7eb; border-radius: 10px; text-align: center; font-weight: 700; font-size: 13px; color: #1f2937; } QProgressBar::chunk { background: qlineargradient(x1:0,y1:0,x2:1,y2:0,stop:0 #22c55e,stop:1 #1B4332); border-radius: 10px; }", nullptr));
        progressProduction->setFormat(QCoreApplication::translate("Production", "%p% compl\303\251t\303\251", nullptr));
        lblPhase->setStyleSheet(QCoreApplication::translate("Production", "color: #374151; font-weight: 600; font-size: 13px;", nullptr));
        lblPhase->setText(QCoreApplication::translate("Production", "Phase : En attente", nullptr));
        lblProgressEnd->setStyleSheet(QCoreApplication::translate("Production", "color: #1B4332; font-weight: 700; font-size: 14px;", nullptr));
        lblProgressEnd->setText(QCoreApplication::translate("Production", "0 / 500 kg trait\303\251s", nullptr));
        groupControls->setStyleSheet(QCoreApplication::translate("Production", "QGroupBox { background: white; border: 2px solid #d1fae5; border-radius: 12px; margin-top: 20px; } QGroupBox::title { subcontrol-origin: margin; subcontrol-position: top left; left: 14px; top: -2px; padding: 5px 14px; background: #1B4332; color: white; font-size: 13px; font-weight: 700; border-radius: 8px; }", nullptr));
        groupControls->setTitle(QCoreApplication::translate("Production", " Contr\303\264les de production ", nullptr));
        btnStart->setStyleSheet(QCoreApplication::translate("Production", "QPushButton { background: #1B4332; color: white; font-size: 15px; font-weight: 700; border-radius: 10px; border: none; } QPushButton:hover { background: #234E3E; } QPushButton:pressed { background: #14532d; } QPushButton:disabled { background: #d1d5db; color: #9ca3af; }", nullptr));
        btnStart->setText(QCoreApplication::translate("Production", "\342\226\266  D\303\251marrer", nullptr));
        btnStop->setStyleSheet(QCoreApplication::translate("Production", "QPushButton { background: #dc2626; color: white; font-size: 15px; font-weight: 700; border-radius: 10px; border: none; } QPushButton:hover { background: #b91c1c; } QPushButton:pressed { background: #991b1b; } QPushButton:disabled { background: #d1d5db; color: #9ca3af; }", nullptr));
        btnStop->setText(QCoreApplication::translate("Production", "\342\217\270  Pause", nullptr));
        btnFinishProduction->setText(QCoreApplication::translate("Production", "\342\234\224  Terminer", nullptr));
        tabWidgetProduction->setTabText(tabWidgetProduction->indexOf(tabLancement), QCoreApplication::translate("Production", "\342\226\266  Lancement", nullptr));
        groupRunningProduction->setStyleSheet(QCoreApplication::translate("Production", "QGroupBox { background-color: #f8fafc; border: 2px solid #1B4332; border-radius: 12px; margin-top: 22px; } QGroupBox::title { subcontrol-origin: margin; subcontrol-position: top left; left: 14px; top: -11px; padding: 5px 16px; background-color: #1B4332; color: #ffffff; font-size: 13px; font-weight: 700; border-radius: 8px; }", nullptr));
        groupRunningProduction->setTitle(QCoreApplication::translate("Production", " Production en cours ", nullptr));
        lblRunningProduction->setStyleSheet(QCoreApplication::translate("Production", "background-color: #EFF6FF; color: #1E40AF; border: 2px solid #93C5FD; padding: 14px; font-size: 14px; border-radius: 10px;", nullptr));
        lblRunningProduction->setText(QCoreApplication::translate("Production", "\342\204\271  Aucune production en cours actuellement. Planifiez et lancez une production pour voir les d\303\251tails ici.", nullptr));
        lblRunningStatus->setStyleSheet(QCoreApplication::translate("Production", "background-color: #DBEAFE; color: #1E40AF; border: 2px solid #93C5FD; font-size: 14px; font-weight: 700; padding: 8px 16px; border-radius: 10px;", nullptr));
        lblRunningStatus->setText(QCoreApplication::translate("Production", "\342\226\266  En cours", nullptr));
        lblRunningPhase->setStyleSheet(QCoreApplication::translate("Production", "background-color: #F3F4F6; color: #374151; border: 2px solid #E5E7EB; font-size: 14px; font-weight: 600; padding: 8px 16px; border-radius: 10px;", nullptr));
        lblRunningPhase->setText(QCoreApplication::translate("Production", "Phase : \342\217\263 Chargement", nullptr));
        progressRunning->setStyleSheet(QCoreApplication::translate("Production", "QProgressBar { background: #e5e7eb; border-radius: 8px; text-align: center; font-weight: 700; font-size: 12px; color: #1f2937; } QProgressBar::chunk { background: qlineargradient(x1:0,y1:0,x2:1,y2:0,stop:0 #22c55e,stop:1 #1B4332); border-radius: 8px; }", nullptr));
        progressRunning->setFormat(QCoreApplication::translate("Production", "%p% \342\200\224 En cours...", nullptr));
        lblRunningOlives->setStyleSheet(QCoreApplication::translate("Production", "background: white; border: 2px solid #d1fae5; border-radius: 10px; font-size: 13px; font-weight: 700; color: #1B4332; padding: 8px 12px;", nullptr));
        lblRunningOlives->setText(QCoreApplication::translate("Production", "\360\237\253\222 Olives\n"
"0 kg", nullptr));
        lblRunningOil->setStyleSheet(QCoreApplication::translate("Production", "background: white; border: 2px solid #d1fae5; border-radius: 10px; font-size: 13px; font-weight: 700; color: #1B4332; padding: 8px 12px;", nullptr));
        lblRunningOil->setText(QCoreApplication::translate("Production", "\360\237\233\242 Huile\n"
"0.00 L", nullptr));
        lblRunningRendement->setStyleSheet(QCoreApplication::translate("Production", "background: white; border: 2px solid #d1fae5; border-radius: 10px; font-size: 13px; font-weight: 700; color: #1B4332; padding: 8px 12px;", nullptr));
        lblRunningRendement->setText(QCoreApplication::translate("Production", "\360\237\223\212 Rendement\n"
"0.00 %", nullptr));
        lblRunningSimTime->setStyleSheet(QCoreApplication::translate("Production", "background: white; border: 2px solid #FCD34D; border-radius: 10px; font-size: 13px; font-weight: 700; color: #92400E; padding: 8px 12px;", nullptr));
        lblRunningSimTime->setText(QCoreApplication::translate("Production", "\360\237\217\255 Simul\303\251\n"
"0h 00min", nullptr));
        lblRunningRealTime->setStyleSheet(QCoreApplication::translate("Production", "background: white; border: 2px solid #FCD34D; border-radius: 10px; font-size: 13px; font-weight: 700; color: #92400E; padding: 8px 12px;", nullptr));
        lblRunningRealTime->setText(QCoreApplication::translate("Production", "\342\217\261 R\303\251el\n"
"00:00:00", nullptr));
        groupHistory->setStyleSheet(QCoreApplication::translate("Production", "QGroupBox { background-color: #f8fafc; border: 2px solid #1B4332; border-radius: 12px; margin-top: 22px; } QGroupBox::title { subcontrol-origin: margin; subcontrol-position: top left; left: 14px; top: -11px; padding: 5px 16px; background-color: #1B4332; color: #ffffff; font-size: 13px; font-weight: 700; border-radius: 8px; }", nullptr));
        groupHistory->setTitle(QCoreApplication::translate("Production", " Historique des productions ", nullptr));
        QTableWidgetItem *___qtablewidgetitem = tableProductionHistory->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QCoreApplication::translate("Production", "ID", nullptr));
        QTableWidgetItem *___qtablewidgetitem1 = tableProductionHistory->horizontalHeaderItem(1);
        ___qtablewidgetitem1->setText(QCoreApplication::translate("Production", "Date", nullptr));
        QTableWidgetItem *___qtablewidgetitem2 = tableProductionHistory->horizontalHeaderItem(2);
        ___qtablewidgetitem2->setText(QCoreApplication::translate("Production", "Olives (kg)", nullptr));
        QTableWidgetItem *___qtablewidgetitem3 = tableProductionHistory->horizontalHeaderItem(3);
        ___qtablewidgetitem3->setText(QCoreApplication::translate("Production", "Huile (L)", nullptr));
        QTableWidgetItem *___qtablewidgetitem4 = tableProductionHistory->horizontalHeaderItem(4);
        ___qtablewidgetitem4->setText(QCoreApplication::translate("Production", "Rendement", nullptr));
        QTableWidgetItem *___qtablewidgetitem5 = tableProductionHistory->horizontalHeaderItem(5);
        ___qtablewidgetitem5->setText(QCoreApplication::translate("Production", "Statut", nullptr));
        btnModifyProduction->setStyleSheet(QCoreApplication::translate("Production", "background-color: #3B82F6; color: white;", nullptr));
        btnModifyProduction->setText(QCoreApplication::translate("Production", "\342\234\217  Modifier", nullptr));
        btnDeleteProduction->setStyleSheet(QCoreApplication::translate("Production", "background-color: #EF4444; color: white;", nullptr));
        btnDeleteProduction->setText(QCoreApplication::translate("Production", "\360\237\227\221  Supprimer", nullptr));
        btnUpdateMonitoring->setText(QCoreApplication::translate("Production", "\360\237\224\204  Actualiser le suivi", nullptr));
        tabWidgetProduction->setTabText(tabWidgetProduction->indexOf(tabSuivi), QCoreApplication::translate("Production", "\360\237\223\215  Suivi", nullptr));
        groupSelectTerminated->setStyleSheet(QCoreApplication::translate("Production", "QGroupBox { background-color: #f8fafc; border: 2px solid #1B4332; border-radius: 12px; margin-top: 22px; } QGroupBox::title { subcontrol-origin: margin; subcontrol-position: top left; left: 14px; top: -11px; padding: 5px 16px; background-color: #1B4332; color: #ffffff; font-size: 13px; font-weight: 700; border-radius: 8px; } QComboBox { background-color: #ffffff; color: #111827; border: 2px solid #1B4332; border-radius: 10px; padding: 8px 14px; font-size: 13px; font-weight: 600; } QComboBox:hover { border-color: #234E3E; background-color: #f0fdf4; } QComboBox::drop-down { border: none; width: 30px; background: transparent; } QComboBox QAbstractItemView { background-color: #ffffff; color: #111827; border: 2px solid #1B4332; border-radius: 8px; selection-background-color: #1B4332; selection-color: #ffffff; font-size: 13px; padding: 4px; }", nullptr));
        groupSelectTerminated->setTitle(QCoreApplication::translate("Production", " S\303\251lectionner une production termin\303\251e ", nullptr));
#if QT_CONFIG(tooltip)
        btnRefreshTerminated->setToolTip(QCoreApplication::translate("Production", "Actualiser la liste", nullptr));
#endif // QT_CONFIG(tooltip)
        btnRefreshTerminated->setStyleSheet(QCoreApplication::translate("Production", "QPushButton { background: #ffffff; border: 2px solid #1B4332; border-radius: 10px; font-size: 20px; color: #1B4332; } QPushButton:hover { background: #f0fdf4; }", nullptr));
        btnRefreshTerminated->setText(QCoreApplication::translate("Production", "\342\206\273", nullptr));
        lblTerminatedInfo->setStyleSheet(QCoreApplication::translate("Production", "color: #6b7280; font-size: 11px; font-weight: 500; padding: 3px 4px;", nullptr));
        lblTerminatedInfo->setText(QCoreApplication::translate("Production", "S\303\251lectionnez une production pour charger ses donn\303\251es automatiquement", nullptr));
        groupRendementInput->setTitle(QCoreApplication::translate("Production", "  Donn\303\251es de production", nullptr));
        lblOlivesUsedTitle->setStyleSheet(QCoreApplication::translate("Production", "font-weight: 600;", nullptr));
        lblOlivesUsedTitle->setText(QCoreApplication::translate("Production", "\360\237\253\222 Olives utilis\303\251es :", nullptr));
        lblOlivesUsed->setStyleSheet(QCoreApplication::translate("Production", "background-color: #f3f4f6; padding: 12px 20px; border-radius: 8px; font-size: 18px; font-weight: 700; color: #1B4332; border: 2px solid #e5e7eb;", nullptr));
        lblOlivesUsed->setText(QCoreApplication::translate("Production", "500 kg", nullptr));
        lblOilProducedTitle->setStyleSheet(QCoreApplication::translate("Production", "font-weight: 600;", nullptr));
        lblOilProducedTitle->setText(QCoreApplication::translate("Production", "\360\237\233\242 Huile produite :", nullptr));
        spinOilProduced->setSuffix(QCoreApplication::translate("Production", "  L", nullptr));
        btnCalculateYield->setText(QCoreApplication::translate("Production", "\360\237\223\212  Calculer le rendement", nullptr));
        groupRendementResult->setTitle(QCoreApplication::translate("Production", "  R\303\251sultat du rendement", nullptr));
        lblYield->setStyleSheet(QCoreApplication::translate("Production", "background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #1B4332, stop:1 #234E3E); color: white; padding: 25px 40px; border-radius: 12px; font-size: 36px; font-weight: 800;", nullptr));
        lblYield->setText(QCoreApplication::translate("Production", "0.00 %", nullptr));
        progressYield->setFormat(QCoreApplication::translate("Production", "Rendement: %p%", nullptr));
        groupYieldMessage->setTitle(QCoreApplication::translate("Production", "  \303\211valuation", nullptr));
        lblYieldMessage->setStyleSheet(QCoreApplication::translate("Production", "background-color: #f8fafc; padding: 18px; border-left: 5px solid #C9A227; font-size: 14px; border-radius: 0 8px 8px 0;", nullptr));
        lblYieldMessage->setText(QCoreApplication::translate("Production", "\342\204\271  Entrez la quantit\303\251 d'huile produite et cliquez sur \"Calculer\" pour obtenir le rendement.\n"
"\n"
"Un bon rendement se situe entre 15% et 22% selon la vari\303\251t\303\251 et la maturit\303\251 des olives.", nullptr));
        tabWidgetProduction->setTabText(tabWidgetProduction->indexOf(tabRendement), QCoreApplication::translate("Production", "\360\237\223\212  Rendement", nullptr));
        groupQualityEval->setTitle(QCoreApplication::translate("Production", "  \303\211valuation de la qualit\303\251", nullptr));
        lblOilQuality->setStyleSheet(QCoreApplication::translate("Production", "font-weight: 600;", nullptr));
        lblOilQuality->setText(QCoreApplication::translate("Production", "\342\255\220 Qualit\303\251 de l'huile :", nullptr));
        comboOilQuality->setItemText(0, QCoreApplication::translate("Production", "\342\255\220\342\255\220\342\255\220  Excellente", nullptr));
        comboOilQuality->setItemText(1, QCoreApplication::translate("Production", "\342\255\220\342\255\220  Bonne", nullptr));
        comboOilQuality->setItemText(2, QCoreApplication::translate("Production", "\342\255\220  Moyenne", nullptr));
        comboOilQuality->setItemText(3, QCoreApplication::translate("Production", "\342\234\227  Faible", nullptr));

        lblAcidity->setStyleSheet(QCoreApplication::translate("Production", "font-weight: 600;", nullptr));
        lblAcidity->setText(QCoreApplication::translate("Production", "\360\237\247\252 Acidit\303\251 :", nullptr));
        comboAcidity->setSuffix(QCoreApplication::translate("Production", " %", nullptr));
        checkConformity->setStyleSheet(QCoreApplication::translate("Production", "font-weight: 600; padding: 10px; background-color: #f0fdf4; border-radius: 8px;", nullptr));
        checkConformity->setText(QCoreApplication::translate("Production", "\342\234\223  Conforme aux normes de qualit\303\251", nullptr));
        groupQualityNotes->setTitle(QCoreApplication::translate("Production", "  Remarques qualit\303\251", nullptr));
        txtQualityNotes->setPlaceholderText(QCoreApplication::translate("Production", "Saisir les remarques sur la qualit\303\251 de l'huile...\n"
"\n"
"\342\200\242 Couleur (vert dor\303\251, jaune, etc.)\n"
"\342\200\242 Ar\303\264me (fruit\303\251, herbac\303\251, etc.)\n"
"\342\200\242 Go\303\273t (amer, piquant, doux)\n"
"\342\200\242 Autres observations", nullptr));
        groupQualitySummary->setTitle(QCoreApplication::translate("Production", "  R\303\251sum\303\251 qualit\303\251", nullptr));
        lblQualitySummary->setStyleSheet(QCoreApplication::translate("Production", "background-color: #edf7f1; padding: 22px; border-left: 5px solid #1E5A45; font-size: 14px; border-radius: 0 10px 10px 0; color: #274338;", nullptr));
        lblQualitySummary->setText(QCoreApplication::translate("Production", "\342\204\271  Aucune \303\251valuation de qualit\303\251 enregistr\303\251e. Remplissez le formulaire ci-dessus et validez pour enregistrer l'\303\251valuation.", nullptr));
        btnValidateQuality->setText(QCoreApplication::translate("Production", "\342\234\223  Valider l'\303\251valuation qualit\303\251", nullptr));
        tabWidgetProduction->setTabText(tabWidgetProduction->indexOf(tabQualite), QCoreApplication::translate("Production", "\342\234\223  Qualit\303\251", nullptr));
        groupReportSummary->setTitle(QCoreApplication::translate("Production", "  R\303\251sum\303\251 de la production", nullptr));
        lblProductionSummary->setStyleSheet(QCoreApplication::translate("Production", "background-color: #D1FAE5; color: #065F46; padding: 15px; border: 2px solid #34D399; border-radius: 10px; font-size: 14px;", nullptr));
        lblProductionSummary->setText(QCoreApplication::translate("Production", "\360\237\223\213  Aucun rapport g\303\251n\303\251r\303\251. Cliquez sur \"G\303\251n\303\251rer rapport\" pour cr\303\251er un rapport d\303\251taill\303\251 de la production.", nullptr));
        groupReportContent->setTitle(QCoreApplication::translate("Production", "  Contenu du rapport", nullptr));
        textReport->setHtml(QCoreApplication::translate("Production", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><meta charset=\"utf-8\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"hr { height: 1px; border-width: 0; }\n"
"li.unchecked::marker { content: \"\\2610\"; }\n"
"li.checked::marker { content: \"\\2612\"; }\n"
"</style></head><body style=\" font-family:'Segoe UI','Arial','sans-serif'; font-size:14px; font-weight:400; font-style:normal;\" bgcolor=\"#fafafa\">\n"
"<h1 style=\" margin-top:18px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:xx-large; font-weight:700; color:#1b4332;\">\360\237\223\213 Rapport de Production</span></h1>\n"
"<p style=\" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; line-height:180%;\"><span style=\" font-style:italic; color:#9ca3af;\">Le rapport d\303\251taill\303\251 appara\303"
                        "\256tra ici apr\303\250s g\303\251n\303\251ration...</span></p>\n"
"<p style=\" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; line-height:180%;\"><span style=\" color:#666666;\">Cliquez sur le bouton &quot;G\303\251n\303\251rer rapport&quot; pour cr\303\251er un rapport complet incluant :</span></p>\n"
"<ul style=\"margin-top: 0px; margin-bottom: 0px; margin-left: 0px; margin-right: 0px; -qt-list-indent: 1;\">\n"
"<li style=\" margin-top:12px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">D\303\251tails de la production</li>\n"
"<li style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Quantit\303\251s et rendements</li>\n"
"<li style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">\303\211valuation de la qualit\303\251</li>\n"
"<li style=\" margin-top:0px; margin-bottom:12px;"
                        " margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Observations et notes</li></ul></body></html>", nullptr));
        lblReportDate->setStyleSheet(QCoreApplication::translate("Production", "color: #6b7280; font-style: italic; padding: 8px 15px; background: #f3f4f6; border-radius: 8px;", nullptr));
        lblReportDate->setText(QCoreApplication::translate("Production", "\360\237\223\205  Derni\303\250re g\303\251n\303\251ration : --/--/----", nullptr));
        btnGenerateReport->setText(QCoreApplication::translate("Production", "\360\237\223\212  G\303\251n\303\251rer rapport", nullptr));
        btnExportPDF->setText(QCoreApplication::translate("Production", "\360\237\223\245  Exporter PDF", nullptr));
        tabWidgetProduction->setTabText(tabWidgetProduction->indexOf(tabRapports), QCoreApplication::translate("Production", "\360\237\223\204  Rapports", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Production: public Ui_Production {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PRODUCTION_H
