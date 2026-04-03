#ifndef METIERSWIDGET_H
#define METIERSWIDGET_H

#include <QWidget>
#include <QList>
#include <QMap>
#include <QProgressBar>
#include <QVector>
#include <QString>
#include "client.h"

#include <QtCharts/QChartView>
#include <QtCharts/QChart>
#include <QtCharts/QPieSeries>
#include <QtCharts/QPieSlice>
#include <QtCharts/QHorizontalBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QValueAxis>

namespace Ui {
class MetiersWidget;
}

class MetiersWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MetiersWidget(QWidget *parent = nullptr);
    ~MetiersWidget();
    
    void setClientsData(QList<Client> *clientsList);

public slots:
    void refreshFromClients();

private slots:
    void on_pushButton_rechercher_clicked();
    void on_pushButton_trier_clicked();
    void on_pushButton_pdf_clicked();
    void on_pushButton_excel_clicked();
    void on_pushButton_word_clicked();
    void on_pushButton_txt_clicked();
    void on_pushButton_stat_clicked();
    void on_tableView_resultats_doubleClicked(const QModelIndex &index);
    void on_pushButton_rapport_clicked();
    void on_pushButton_exportRapport_clicked();
    void onRapportClientChanged(int index);
    void onChartConfigChanged();

private:
    struct ReportClient {
        int id;
        QString nom;
        QString prenom;
        QString type;
        QString telephone;
        QString email;
        QString adresse;
        QDate dateInscription;
        double totalOlives;
    };

    Ui::MetiersWidget *ui;
    QList<Client> *clients;
    QChartView *chartView;
    QChartView *fideliteChartView;
    QVector<ReportClient> reportClients;
    
    void refreshResultsTableView(const QList<Client> &filteredClients);
    void refreshFideliteTableView();
    void displayClientInfo(const Client &client);
    void generateReport(const Client &client);
    void exportToTxt(const QList<Client> &data);
    QList<Client> collectExportClients() const;
    QString buildClientsPlainText(const QList<Client> &data) const;
    QString buildClientsHtmlTable(const QList<Client> &data) const;
    void initializeStatsChart();
    void updateStatsChart();
    void initializeFideliteChart();
    void updateFideliteChart();
    void refreshReportClients();
    void renderReportClient(const ReportClient &client);
    void renderReportAllClients();
};

#endif // METIERSWIDGET_H
