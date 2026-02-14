#ifndef GESTIONCLIENTS_H
#define GESTIONCLIENTS_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class GestionClients;
}
QT_END_NAMESPACE

class GestionClients : public QMainWindow
{
    Q_OBJECT

public:
    GestionClients(QWidget *parent = nullptr);
    ~GestionClients();

signals:
    void backToMenu();

private slots:
    void onBackClicked();
    void updateDateTime();

private:
    Ui::GestionClients *ui;
};

#endif // GESTIONCLIENTS_H
