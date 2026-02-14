#ifndef PRODUCTION_H
#define PRODUCTION_H

#include <QMainWindow>
#include <QMessageBox>
#include <QtSql/QSqlQueryModel>
#include <QtSql/QSqlTableModel>
#include <QtSql/QSqlQuery>
#include <QTableWidgetItem>
#include <QHeaderView>
#include "connection.h"
#include "productionmodel.h"

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

private:
    Ui::Production *ui;
    Connection conn;
    ProductionModel currentProduction;
    int currentProductionId;
};
#endif // PRODUCTION_H
