/**
 * @file mainwindow.h
 * @brief Fenêtre principale de l'application
 * 
 * Cette classe gère l'interface utilisateur et les interactions avec
 * la base de données et la logique métier.
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QChart>
#include <QChartView>
#include <QBarSeries>
#include "database.h"
#include "businesslogic.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief Constructeur
     */
    MainWindow(QWidget *parent = nullptr);
    
    /**
     * @brief Destructeur
     */
    ~MainWindow();

private slots:
    // ===== TRANSACTIONS =====
    
    /**
     * @brief Ajoute une nouvelle transaction
     */
    void onAddTransaction();
    
    /**
     * @brief Modifie la transaction sélectionnée
     */
    void onModifyTransaction();
    
    /**
     * @brief Supprime la transaction sélectionnée
     */
    void onDeleteTransaction();
    
    /**
     * @brief Vide le formulaire
     */
    void onClearForm();
    
    /**
     * @brief Charge une transaction dans le formulaire
     */
    void onTransactionSelected(int row, int column);
    
    /**
     * @brief Recherche les transactions
     */
    void onSearchTransactions();
    
    // ===== STATISTIQUES =====
    
    /**
     * @brief Met à jour les statistiques et les graphiques
     */
    void updateStatistics();
    
    /**
     * @brief Crée le graphique des revenus/dépenses
     */
    void createChart();
    
    // ===== RAPPORTS =====
    
    /**
     * @brief Exporte en PDF
     */
    void onExportPDF();
    
    /**
     * @brief Exporte en Excel
     */
    void onExportExcel();
    
    /**
     * @brief Exporte en CSV
     */
    void onExportCSV();
    
    /**
     * @brief Exporte en TXT
     */
    void onExportTXT();
    
    // ===== PRÉVISIONS =====
    
    /**
     * @brief Calcule les prévisions
     */
    void onCalculateForecast();

private:
    Ui::MainWindow *ui;         /*!< Interface utilisateur générée */
    Database *m_database;       /*!< Gestionnaire de base de données */
    BusinessLogic *m_logic;     /*!< Logique métier */
    int m_selectedTransactionId; /*!< ID de la transaction sélectionnée */
    
    /**
     * @brief Initialise l'application
     */
    void initializeApplication();
    
    /**
     * @brief Configure les connexions de signaux/slots
     */
    void setupConnections();
    
    /**
     * @brief Charge les catégories dans le ComboBox
     */
    void loadCategories();
    
    /**
     * @brief Charge toutes les transactions dans la table
     */
    void loadTransactions();
    
    /**
     * @brief Charge les transactions filtrées
     */
    void loadFilteredTransactions(const QString &filter);
    
    /**
     * @brief Définit la date d'aujourd'hui dans le QDateEdit
     */
    void setTodayDate();
    
    /**
     * @brief Valide les données du formulaire
     * @return true si valide, false sinon
     */
    bool validateForm();
    
    /**
     * @brief Affiche un message d'erreur
     */
    void showError(const QString &message);
    
    /**
     * @brief Affiche un message de succès
     */
    void showSuccess(const QString &message);
};

#endif // MAINWINDOW_H
