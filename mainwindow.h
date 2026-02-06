#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>
#include <QList>
#include <QDate>
#include <QTime>
#include <QTimer>
#include <QString>
#include <QTableWidget>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

// Structure Client
struct Client {
    int id_client;
    QString nom;
    QString prenom;
    QString telephone;
    QString adresse;
    QString email;
    QString type_client; // "Particulier" ou "Société"
    double total_olives_livrees; // kg
    QDate date_creation;
    QString statut; // Bronze, Silver, Gold, Premium
    QString importance; // Standard, Moyen, Important
    
    // Getters pour fidélité
    QString getStatutFidelite() const;
    QString getImportance() const;
    double getScoreFidelite() const;
    
    // Opérateur d'égalité
    bool operator==(const Client &other) const {
        return id_client == other.id_client;
    }
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // Onglet 1: Gestion Client (CRUD)
    void on_pushButton_ajouter_clicked();
    void on_pushButton_modifier_clicked();
    void on_pushButton_supprimer_clicked();
    void on_pushButton_consulter_clicked();
    void on_pushButton_clear_clicked();
    void on_tableView_client_clicked(const QModelIndex &index);
    
    // Nouveaux boutons de la barre d'action
    void on_btnAjouter_clicked();
    void on_btnModifier_clicked();
    void on_btnSupprimer_clicked();
    
    // Onglet 2: Recherche & Tri
    void on_pushButton_rechercher_clicked();
    void on_pushButton_trier_clicked();
    void on_pushButton_pdf_clicked();
    void on_pushButton_excel_clicked();
    void on_pushButton_word_clicked();
    void on_pushButton_txt_clicked();
    
    // Onglet 3: Statistiques
    void on_pushButton_stat_clicked();
    
    // Onglet 4: Fidélité & Rapport
    void on_tableView_client_doubleClicked(const QModelIndex &index);
    void on_pushButton_rapport_clicked();
    void on_pushButton_exportRapport_clicked();
    
    // Mise à jour automatique date/heure
    void updateDateTime();

private:
    Ui::MainWindow *ui;
    
    // Données en mémoire
    QList<Client> clients;
    int nextClientId = 1;
    Client currentSelectedClient;
    
    // Timer pour la date/heure
    QTimer *dateTimeTimer;
    
    // Méthodes privées
    void refreshTableView();
    void refreshTableView(const QList<Client> &filteredClients);
    void clearForm();
    void loadClientToForm(const Client &client);
    Client getFormData();
    void validateAndCalculateStatut(Client &client);
    void displayClientInfo(const Client &client);
    void generateReport(const Client &client);
    void setupTableColumns();
    
    // Export
    void exportToTxt(const QList<Client> &data);
    void exportToPdf();
    void exportToExcel();
    void exportToWord();
};

#endif // MAINWINDOW_H
