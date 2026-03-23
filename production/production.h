#ifndef PRODUCTION_H
#define PRODUCTION_H

#include <QMainWindow>
#include <QMessageBox>
#include <QtSql/QSqlQueryModel>
#include <QtSql/QSqlTableModel>
#include <QtSql/QSqlQuery>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QFileDialog>
#include <QPrinter>
#include <QPainter>
#include <QTextDocument>
#include <QTimer>
#include <QTime>
#include <QDateTime>
#include <QDateEdit>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QLabel>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QPieSeries>
#include <QtCharts/QValueAxis>
#include "connection.h"
#include "productionmodel.h"
#include "productioneditdialog.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class Production;
}
QT_END_NAMESPACE

class Production : public QMainWindow
{
    Q_OBJECT

public:
    Production(QWidget *parent = nullptr);
    ~Production();

    void loadProductionHistory();
    void refreshTable();
    void clearForm();
    void showSuccessNotification(const QString &message);
    void showErrorNotification(const QString &message);
    void generatePdf(const QString &filePath,
                     const QString &whereClause = QString(),
                     const QString &filterText = QString());

signals:
    void backToMenu();

private slots:
    void onBackButtonClicked();

    // CRUD Slots
    void onPlanifierClicked();
    void onStartClicked();
    void onStopClicked();
    void onCalculateYieldClicked();
    void onValidateQualityClicked();
    void onGenerateReportClicked();
    void onDeleteClicked();
    void onModifyClicked();
    void onRefreshHistoryClicked();

    // Simulation Slots
    void onRefreshPlannedClicked();
    void onSimTick();
    void onFinishProductionClicked();

    // Rendement Slots
    void onTerminatedProductionSelected(int index);
    void onRefreshTerminatedClicked();
    void onQualityProductionSelected(int index);
    void onRefreshQualityProductions();
    void onApplyReportFilters();
    void onReportPeriodChanged(int index);
    void refreshStatistics();

    // Theme & Language Slots
    void onToggleDarkMode();
    void onToggleLanguage();

private:
    Ui::Production *ui;
    ProductionModel currentProduction;
    int currentProductionId;
    int m_rendementProductionId;
    int m_qualityProductionId;

    // ── Theme & Language ──────────────────────────────────────────────────
    bool    m_isDarkMode;
    int     m_langIndex;          // 0=FR 1=EN 2=AR
    QString m_lightStyleSheet;    // captured from .ui after setupUi()

    void applyTheme();
    void applyTranslations();

    // ── Simulation engine ─────────────────────────────────────────────────
    QTimer     *m_simTimer;
    QTimer     *m_statsTimer;
    int         m_simTicks;           // real ticks (500 ms each)
    int         m_totalDurationMin;   // production duration in simulated minutes
    int         m_olivesKgTotal;      // total olives for active production
    int         m_simProductionId;    // DB id of running production
    bool        m_isRunning;
    bool        m_isPaused;
    double      m_oilProducedL;       // oil produced so far (simulated)
    QTime       m_realStartTime;      // wall-clock start for elapsed display

    // ── Advanced quality/report widgets (created programmatically) ─────────
    QComboBox      *m_comboQualityProduction;
    QPushButton    *m_btnRefreshQualityProduction;
    QDoubleSpinBox *m_spinAcidityPercent;
    QLabel         *m_lblAiQualityScore;
    QLabel         *m_lblAiRecommendations;

    QComboBox      *m_comboReportPeriod;
    QDateEdit      *m_dateReportFrom;
    QDateEdit      *m_dateReportTo;
    QComboBox      *m_comboReportQuality;
    QPushButton    *m_btnApplyReportFilters;

    // ── Statistics tab widgets ─────────────────────────────────────────────
    QWidget                 *m_tabStatistiques;
    QLabel                  *m_lblStatProdToday;
    QLabel                  *m_lblStatRendAvg;
    QLabel                  *m_lblStatAcidAvg;
    QLabel                  *m_lblStatConformity;
    QChartView    *m_lineChartView;
    QChartView    *m_pieChartView;
    QLineSeries   *m_seriesOlives;
    QLineSeries   *m_seriesHuile;
    QLineSeries   *m_seriesRendement;
    QValueAxis    *m_axisStatsX;
    QValueAxis    *m_axisStatsYVolume;
    QValueAxis    *m_axisStatsYRendement;
    QPieSeries    *m_qualityPie;
    int                      m_statsTick;

    void updateSimUI(int progressPct, int prodMinutesElapsed);
    void updateDashboard(const QString &status, int olivesKg,
                         double rendPct, int alertCount);
    void loadPlannedProductions();
    void loadTerminatedProductions();
    void loadQualityEvaluableProductions();
    void setupAdvancedQualityUI();
    void setupAdvancedReportsUI();
    void buildStatisticsTab();
    QString buildReportWhereClause(QString *prettyPeriod = nullptr) const;
    void updateReportPreview();
    void refreshQualityPie();
    bool autoEvaluateQuality(bool persistToDb, bool showSuccessNotification);
};
#endif // PRODUCTION_H
