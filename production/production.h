#ifndef PRODUCTION_H
#define PRODUCTION_H

#include <QMainWindow>
#include <QMessageBox>
#include <QtSql/QSqlQueryModel>
#include <QtSql/QSqlTableModel>
#include <QtSql/QSqlQuery>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QListWidget>
#include <QComboBox>
#include "connection.h"
#include "productionmodel.h"
#include "productionservice.h"
#include "smartproductionassistant.h"
#include "productionbot.h"
#include "voiceassistant.h"

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

signals:
    void backToMenu();

private slots:
    void onBackButtonClicked();
    
    // CRUD Slots
    void onPlanifierClicked();          // ADD production
    void onStartClicked();              // UPDATE status to "En cours"
    void onStopClicked();               // UPDATE status to "Arrêté"
    void onCalculateYieldClicked();     // Calculate rendement
    void onValidateQualityClicked();    // UPDATE quality info
    void onGenerateReportClicked();     // UPDATE report date
    void onDeleteClicked();             // DELETE production
    void onModifyClicked();             // MODIFY selected production
    void onRefreshHistoryClicked();     // Refresh table

    // Table selection
    void onTableRowSelected(int row);   // Select production from table

    // AI Assistant Slots
    void onSendAiCommand();
    void onVoiceToggle();
    void onBotToggle();
    void onAiAnalyzeClicked();
    void onAiOptimizeClicked();
    void onVoiceResponse(const QString &response);
    void onBotDecision(const BotDecision &decision);

private:
    void setupAiTab();
    void setupLancementSelector();
    void populateLaunchCombo();
    void updateLancementDisplay(int idProduction);
    void appendAiChat(const QString &sender, const QString &message, const QString &color = "#1f2937");
    void updateButtonStates(const QString &statut);

    Ui::Production *ui;
    Connection conn;
    ProductionModel currentProduction;
    int currentProductionId;

    // Smart services
    ProductionService        *m_productionService;
    SmartProductionAssistant *m_smartAssistant;
    ProductionBot            *m_productionBot;
    VoiceAssistant           *m_voiceAssistant;

    // AI tab widgets (created in code)
    QTextEdit    *m_aiChatDisplay;
    QLineEdit    *m_aiCommandInput;
    QPushButton  *m_btnSendAi;
    QPushButton  *m_btnVoice;
    QPushButton  *m_btnBotToggle;
    QPushButton  *m_btnAiAnalyze;
    QPushButton  *m_btnAiOptimize;
    QLabel       *m_lblBotStatus;
    QLabel       *m_lblVoiceStatus;
    QListWidget  *m_botDecisionList;

    // Lancement tab — production selector
    QComboBox    *m_comboSelectProd;
    QLabel       *m_lblProdDate;
    QLabel       *m_lblProdQty;
    QLabel       *m_lblProdType;
    QLabel       *m_lblProdDuree;
};
#endif // PRODUCTION_H
