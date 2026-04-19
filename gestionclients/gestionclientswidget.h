#ifndef GESTIONCLIENTSWIDGET_H
#define GESTIONCLIENTSWIDGET_H

#include <QWidget>
#include <QList>
#include <QModelIndex>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlQueryModel>
#include <QRegularExpression>
#include "client.h"
#include "clientconnection.h"
// Note: Connection class is ClientConnection (renamed to avoid conflict with production)

namespace Ui {
class GestionClientsWidget;
}

class GestionClientsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit GestionClientsWidget(QWidget *parent = nullptr);
    ~GestionClientsWidget();
    
    QList<Client>& getClients();
    int& getNextClientId();
    void afficherClients();

private slots:
    void on_pushButton_ajouter_clicked();
    void on_pushButton_modifier_clicked();
    void on_pushButton_supprimer_clicked();
    void on_pushButton_historique_clicked();
    void on_tableWidget_clients_clicked(const QModelIndex &index);
    void on_tableWidget_clients_customContextMenuRequested(const QPoint &pos);
    void calculerStatutEnTempsReel();

signals:
    void clientsUpdated();
    void historyUpdated();

private:
    Ui::GestionClientsWidget *ui;
    QList<Client> clients;
    int nextClientId;
    int idClientSelectionne;
    Client currentSelectedClient;
    
    void clearForm();
    void loadClientToForm(const Client &client);
    void desactiverBoutons();
    void activerBoutons();
    Client getFormData();
    void validateAndCalculateStatut(Client &client);
    void displayClientInfo(const Client &client);
    bool idExists(int id) const;
    bool validateForm(bool isAdd, bool isModify);
    void setupValidators();
    void validateRealtime();
    bool isNameValid(const QString &value) const;
    bool isTelValid(const QString &value) const;
    bool isEmailValid(const QString &value) const;
    bool isAddressValid(const QString &value) const;
    bool isIdFormatValid(const QString &value) const;
    bool isTotalValid(const QString &value) const;
    void setFieldState(QWidget *widget, bool valid);
    void setErrorHint(const QString &message);
    void showToastMessage(const QString &message);
    void updateClientLastAction(const Client &client, const QString &actionType, const QString &details);
    QString buildChangeDetails(const Client &before, const Client &after) const;
    Client clientFromRow(int row) const;
    void addRemarkForClient(int id, const QString &nom, const QString &prenom);
    void openHistoryDialogForClient(int id, const QString &nom, const QString &prenom);
};

#endif // GESTIONCLIENTSWIDGET_H
