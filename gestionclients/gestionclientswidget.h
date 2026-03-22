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
    void on_tableWidget_clients_clicked(const QModelIndex &index);
    void calculerStatutEnTempsReel();

signals:
    void clientsUpdated();

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
};

#endif // GESTIONCLIENTSWIDGET_H
