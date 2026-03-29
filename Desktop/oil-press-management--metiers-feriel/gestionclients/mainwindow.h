#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class GestionClientsWidget;
class MetiersWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void updateDateTime();

private:
    Ui::MainWindow *ui;
    GestionClientsWidget *gestionClientsWidget;
    MetiersWidget *metiersWidget;
    QTimer *dateTimeTimer;
    
    void setupWidgets();
};

#endif // MAINWINDOW_H
