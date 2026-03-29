#ifndef GESTIONCLIENTS_H
#define GESTIONCLIENTS_H

#include <QMainWindow>
#include <QTimer>
#include "gestionclientswidget.h"
#include "metierswidget.h"

namespace Ui {
class GestionClients;
}

class GestionClients : public QMainWindow
{
    Q_OBJECT

public:
    explicit GestionClients(QWidget *parent = nullptr);
    ~GestionClients();

signals:
    void backToMenu();

private slots:
    void updateDateTime();
    void updateStatCards();

private:
    Ui::GestionClients *ui;
    GestionClientsWidget *gestionClientsWidget;
    MetiersWidget *metiersWidget;
    QTimer *dateTimeTimer;
};

#endif // GESTIONCLIENTS_H
