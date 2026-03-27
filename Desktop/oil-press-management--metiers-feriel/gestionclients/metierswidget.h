#ifndef METIERSWIDGET_H
#define METIERSWIDGET_H

#include <QWidget>
#include <QList>
#include "client.h"

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

private:
    Ui::MetiersWidget *ui;
    QList<Client> *clients;
    
    void refreshResultsTableView(const QList<Client> &filteredClients);
    void displayClientInfo(const Client &client);
    void generateReport(const Client &client);
    void exportToTxt(const QList<Client> &data);
};

#endif // METIERSWIDGET_H
