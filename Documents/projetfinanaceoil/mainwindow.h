#pragma once

#include <QMainWindow>
#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QtCharts>
#include <QStandardItemModel>
#include <QChartView>
#include <QPieSeries>
#include <QPieSlice>
#include "dbmanager.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(const QString &dbPath, QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // CRUD Operations
    void ajouterTransaction();
    void modifierTransaction();
    void supprimerTransaction();
    void viderFormulaire();

    // Recherche et filtrage
    void rechercherTransaction();
    void chargerTransactions();
    void selectionnerTransaction(const QModelIndex &index);

    // Statistiques et graphiques
    void calculerStatistiques();
    void afficherGraphiques();

    // Export
    void exporterPDF();
    void exporterExcel();
    void exporterTXT();

    // Prévisions
    void calculerPrevisions();
    void detecterDepensesAnormales();
    void analyserCoutParLitre();

    // Gestion des catégories
    void chargerCategories();
    void ajouterCategorie();
private:
    Ui::MainWindow *ui;
    QSqlDatabase db; // utilisé par l'implémentation existante
    QSqlTableModel *model;
    QStandardItemModel *tableModel;

    // Variables statistiques
    double totalRevenus;
    double totalDepenses;
    double profit;

    // ID de la transaction sélectionnée
    int transactionSelectionnee;

    // Méthodes privées
    void initialiserBDD();
    void configurerTableView();
    void configurerConnexions();
    void configurerGraphiques();
    bool validerFormulaire();
    void afficherMessage(const QString &titre, const QString &message);

    // Graphiques
    QChartView *chartViewRevenus;
    QChartView *chartViewDepenses;
    QChart *chartRevenus;
    QChart *chartDepenses;

    // Manager additionnel (utilitaire)
    DBManager dbManager;
};
