#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QStandardItemModel>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QFileDialog>
#include <QDate>
#include <QSortFilterProxyModel>
#include <QTableWidget>
#include <algorithm>

// Implémentation des méthodes de la structure Client
QString Client::getStatutFidelite() const {
    if (total_olives_livrees > 8000) return "Premium";
    if (total_olives_livrees > 5000) return "Gold";
    if (total_olives_livrees > 2000) return "Silver";
    return "Bronze";
}

QString Client::getImportance() const {
    if (total_olives_livrees > 3000) return "Important";
    if (total_olives_livrees > 1500) return "Moyen";
    return "Standard";
}

double Client::getScoreFidelite() const {
    return total_olives_livrees / 100.0; // Score simple basé sur la quantité
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("Gestion des Clients - Olive Livery");
    
    setupTableColumns();
    
    // Initialiser et démarrer le timer pour la date/heure
    dateTimeTimer = new QTimer(this);
    // DÉSACTIVÉ : Connexion du timer pour l'interface statique
    // connect(dateTimeTimer, &QTimer::timeout, this, &MainWindow::updateDateTime);
    updateDateTime(); // Mise à jour immédiate (une seule fois)
    // dateTimeTimer->start(1000); // Mise à jour toutes les secondes
    
    // DÉSACTIVÉ : Interface statique - Pas de données de test
    // // Ajouter des données de test
    // Client testClient;
    // testClient.id_client = nextClientId++;
    // testClient.nom = "Dupont";
    // testClient.prenom = "Jean";
    // testClient.telephone = "06 12 34 56 78";
    // testClient.adresse = "123 Rue de Paris, 75000 Paris";
    // testClient.email = "jean.dupont@email.com";
    // testClient.type_client = "Particulier";
    // testClient.total_olives_livrees = 2500;
    // testClient.date_creation = QDate::currentDate();
    // testClient.statut = testClient.getStatutFidelite();
    // testClient.importance = testClient.getImportance();
    // clients.append(testClient);
    // 
    // testClient.id_client = nextClientId++;
    // testClient.nom = "Durand";
    // testClient.prenom = "Marie";
    // testClient.telephone = "06 98 76 54 32";
    // testClient.adresse = "456 Avenue Lyon, 69000 Lyon";
    // testClient.email = "marie.durand@email.com";
    // testClient.type_client = "Société";
    // testClient.total_olives_livrees = 5500;
    // testClient.statut = testClient.getStatutFidelite();
    // testClient.importance = testClient.getImportance();
    // clients.append(testClient);
    // 
    // testClient.id_client = nextClientId++;
    // testClient.nom = "Martin";
    // testClient.prenom = "Pierre";
    // testClient.telephone = "06 11 22 33 44";
    // testClient.adresse = "789 Boulevard Marseille, 13000 Marseille";
    // testClient.email = "pierre.martin@email.com";
    // testClient.type_client = "Particulier";
    // testClient.total_olives_livrees = 8500;
    // testClient.statut = testClient.getStatutFidelite();
    // testClient.importance = testClient.getImportance();
    // clients.append(testClient);
    // 
    // refreshTableView();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupTableColumns() {
    // Configurer les colonnes pour tableView_client - initialiser comme QTableWidget
    // Convertir QTableView en mode avec colonnes
    ui->tableView_client->setColumnCount(7);
    ui->tableView_client->setHorizontalHeaderLabels({"ID", "Nom", "Prénom", "Type", "Total Olives (kg)", "Date", "Statut"});
    ui->tableView_client->horizontalHeader()->setStretchLastSection(true);
    
    // Configurer les colonnes pour tableView_resultats
    ui->tableView_resultats->setColumnCount(7);
    ui->tableView_resultats->setHorizontalHeaderLabels({"ID", "Nom", "Prénom", "Type", "Total Olives (kg)", "Date", "Statut"});
    ui->tableView_resultats->horizontalHeader()->setStretchLastSection(true);
}

void MainWindow::refreshTableView() {
    refreshTableView(clients);
}

void MainWindow::refreshTableView(const QList<Client> &filteredClients) {
    ui->tableView_client->setRowCount(0);
    ui->tableView_resultats->setRowCount(0);
    
    for (int i = 0; i < filteredClients.count(); ++i) {
        const Client &client = filteredClients[i];
        int row = ui->tableView_client->rowCount();
        ui->tableView_client->insertRow(row);
        
        ui->tableView_client->setItem(row, 0, new QTableWidgetItem(QString::number(client.id_client)));
        ui->tableView_client->setItem(row, 1, new QTableWidgetItem(client.nom));
        ui->tableView_client->setItem(row, 2, new QTableWidgetItem(client.prenom));
        ui->tableView_client->setItem(row, 3, new QTableWidgetItem(client.type_client));
        ui->tableView_client->setItem(row, 4, new QTableWidgetItem(QString::number(client.total_olives_livrees)));
        ui->tableView_client->setItem(row, 5, new QTableWidgetItem(client.date_creation.toString("dd/MM/yyyy")));
        ui->tableView_client->setItem(row, 6, new QTableWidgetItem(client.statut));
    }
}

void MainWindow::clearForm() {
    ui->lineEdit_id->clear();
    ui->lineEdit_nom->clear();
    ui->lineEdit_prenom->clear();
    ui->lineEdit_tel->clear();
    ui->lineEdit_adresse->clear();
    ui->lineEdit_email->clear();
    ui->lineEdit_total->clear();
    ui->comboBox_type->setCurrentIndex(0);
    ui->dateEdit_creation->setDate(QDate::currentDate());
    ui->label_statut->setText("-");
    currentSelectedClient.id_client = -1;
}

void MainWindow::loadClientToForm(const Client &client) {
    ui->lineEdit_id->setText(QString::number(client.id_client));
    ui->lineEdit_nom->setText(client.nom);
    ui->lineEdit_prenom->setText(client.prenom);
    ui->lineEdit_tel->setText(client.telephone);
    ui->lineEdit_adresse->setText(client.adresse);
    ui->lineEdit_email->setText(client.email);
    ui->lineEdit_total->setText(QString::number(client.total_olives_livrees));
    ui->comboBox_type->setCurrentText(client.type_client);
    ui->dateEdit_creation->setDate(client.date_creation);
    ui->label_statut->setText(client.statut);
    currentSelectedClient = client;
}

Client MainWindow::getFormData() {
    Client client;
    client.id_client = ui->lineEdit_id->text().isEmpty() ? nextClientId++ : ui->lineEdit_id->text().toInt();
    client.nom = ui->lineEdit_nom->text();
    client.prenom = ui->lineEdit_prenom->text();
    client.telephone = ui->lineEdit_tel->text();
    client.adresse = ui->lineEdit_adresse->text();
    client.email = ui->lineEdit_email->text();
    client.type_client = ui->comboBox_type->currentText();
    client.total_olives_livrees = ui->lineEdit_total->text().toDouble();
    client.date_creation = ui->dateEdit_creation->date();
    
    validateAndCalculateStatut(client);
    return client;
}

void MainWindow::validateAndCalculateStatut(Client &client) {
    client.statut = client.getStatutFidelite();
    client.importance = client.getImportance();
}

// Slot CRUD - Ajouter
void MainWindow::on_pushButton_ajouter_clicked() {
    // DÉSACTIVÉ : Interface statique - Aucune action
    // if (ui->lineEdit_nom->text().isEmpty() || ui->lineEdit_prenom->text().isEmpty()) {
    //     QMessageBox::warning(this, "Erreur", "Veuillez remplir tous les champs obligatoires!");
    //     return;
    // }
    // 
    // Client newClient = getFormData();
    // clients.append(newClient);
    // 
    // QMessageBox::information(this, "Succès", "Client ajouté avec succès!");
    // clearForm();
    // refreshTableView();
}

// Slot CRUD - Modifier
void MainWindow::on_pushButton_modifier_clicked() {
    // DÉSACTIVÉ : Interface statique - Aucune action
    // if (currentSelectedClient.id_client == -1) {
    //     QMessageBox::warning(this, "Erreur", "Veuillez sélectionner un client à modifier!");
    //     return;
    // }
    // 
    // Client updatedClient = getFormData();
    // for (int i = 0; i < clients.count(); ++i) {
    //     if (clients[i].id_client == updatedClient.id_client) {
    //         clients[i] = updatedClient;
    //         break;
    //     }
    // }
    // 
    // QMessageBox::information(this, "Succès", "Client modifié avec succès!");
    // clearForm();
    // refreshTableView();
}

// Slot CRUD - Supprimer
void MainWindow::on_pushButton_supprimer_clicked() {
    // DÉSACTIVÉ : Interface statique - Aucune action
    // if (currentSelectedClient.id_client == -1) {
    //     QMessageBox::warning(this, "Erreur", "Veuillez sélectionner un client à supprimer!");
    //     return;
    // }
    // 
    // QMessageBox::StandardButton reply = QMessageBox::question(this, "Confirmation", 
    //     "Êtes-vous sûr de vouloir supprimer ce client?");
    // 
    // if (reply == QMessageBox::Yes) {
    //     // Supprimer par ID au lieu de l'objet complet
    //     for (int i = 0; i < clients.count(); ++i) {
    //         if (clients[i].id_client == currentSelectedClient.id_client) {
    //             clients.removeAt(i);
    //             break;
    //         }
    //     }
    //     QMessageBox::information(this, "Succès", "Client supprimé avec succès!");
    //     clearForm();
    //     refreshTableView();
    // }
}

// Slot CRUD - Consulter
void MainWindow::on_pushButton_consulter_clicked() {
    // DÉSACTIVÉ : Interface statique - Aucune action
    // if (currentSelectedClient.id_client == -1) {
    //     QMessageBox::warning(this, "Erreur", "Veuillez sélectionner un client!");
    //     return;
    // }
    // 
    // displayClientInfo(currentSelectedClient);
}

void MainWindow::on_pushButton_clear_clicked() {
    // DÉSACTIVÉ : Interface statique - Aucune action
    // clearForm();
}

void MainWindow::on_tableView_client_clicked(const QModelIndex &index) {
    // DÉSACTIVÉ : Interface statique - Aucune action
    // int row = index.row();
    // if (row >= 0 && row < clients.count()) {
    //     loadClientToForm(clients[row]);
    // }
}

// Nouveaux slots pour les boutons de la barre d'action
void MainWindow::on_btnAjouter_clicked() {
    // DÉSACTIVÉ : Interface statique - Aucune action
    // on_pushButton_ajouter_clicked();
}

void MainWindow::on_btnModifier_clicked() {
    // DÉSACTIVÉ : Interface statique - Aucune action
    // on_pushButton_modifier_clicked();
}

void MainWindow::on_btnSupprimer_clicked() {
    // DÉSACTIVÉ : Interface statique - Aucune action
    // on_pushButton_supprimer_clicked();
}

// Onglet 2: Recherche & Tri
void MainWindow::on_pushButton_rechercher_clicked() {
    // DÉSACTIVÉ : Interface statique - Aucune action
}

void MainWindow::on_pushButton_trier_clicked() {
    // DÉSACTIVÉ : Interface statique - Aucune action
}

// Exports simples
void MainWindow::on_pushButton_pdf_clicked() {
    // DÉSACTIVÉ : Interface statique - Aucune action
}

void MainWindow::on_pushButton_excel_clicked() {
    // DÉSACTIVÉ : Interface statique - Aucune action
}

void MainWindow::on_pushButton_word_clicked() {
    // DÉSACTIVÉ : Interface statique - Aucune action
}

void MainWindow::on_pushButton_txt_clicked() {
    // DÉSACTIVÉ : Interface statique - Aucune action
}

void MainWindow::exportToTxt(const QList<Client> &data) {
    QString filePath = QFileDialog::getSaveFileName(this, "Exporter en TXT", "", "Fichiers texte (*.txt)");
    if (filePath.isEmpty()) return;
    
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Erreur", "Impossible d'ouvrir le fichier!");
        return;
    }
    
    QTextStream out(&file);
    out << "========== RAPPORT DES CLIENTS ==========\n\n";
    
    for (const Client &client : data) {
        out << "ID: " << client.id_client << "\n";
        out << "Nom: " << client.nom << "\n";
        out << "Prénom: " << client.prenom << "\n";
        out << "Téléphone: " << client.telephone << "\n";
        out << "Adresse: " << client.adresse << "\n";
        out << "Email: " << client.email << "\n";
        out << "Type: " << client.type_client << "\n";
        out << "Total Olives: " << client.total_olives_livrees << " kg\n";
        out << "Date: " << client.date_creation.toString("dd/MM/yyyy") << "\n";
        out << "Statut: " << client.statut << "\n";
        out << "Importance: " << client.importance << "\n";
        out << "------------------------------------\n\n";
    }
    
    file.close();
}

void MainWindow::exportToPdf() {
    // À implémenter avec Qt PDF
}

void MainWindow::exportToExcel() {
    // À implémenter avec Qt5Xlsx
}

void MainWindow::exportToWord() {
    // À implémenter avec une bibliothèque Word
}

// Onglet 3: Statistiques
void MainWindow::on_pushButton_stat_clicked() {
    // DÉSACTIVÉ : Interface statique - Aucune action
}

// Onglet 4: Fidélité & Rapport
void MainWindow::displayClientInfo(const Client &client) {
    ui->label_score_value->setText(QString::number(client.getScoreFidelite(), 'f', 2));
    ui->label_statut_value->setText(client.getStatutFidelite());
    ui->label_importance_value->setText(client.getImportance());
}

void MainWindow::on_tableView_client_doubleClicked(const QModelIndex &index) {
    // DÉSACTIVÉ : Interface statique - Aucune action
}

void MainWindow::generateReport(const Client &client) {
    QString report;
    report += "========================================\n";
    report += "        RAPPORT CLIENT COMPLET\n";
    report += "========================================\n\n";
    report += "INFORMATIONS PERSONNELLES\n";
    report += "------------------------\n";
    report += "ID Client: " + QString::number(client.id_client) + "\n";
    report += "Nom: " + client.nom + "\n";
    report += "Prénom: " + client.prenom + "\n";
    report += "Téléphone: " + client.telephone + "\n";
    report += "Adresse: " + client.adresse + "\n";
    report += "Email: " + client.email + "\n";
    report += "Type: " + client.type_client + "\n\n";
    
    report += "STATISTIQUES DE LIVRAISON\n";
    report += "------------------------\n";
    report += "Total Olives Livrées: " + QString::number(client.total_olives_livrees) + " kg\n";
    report += "Date d'Inscription: " + client.date_creation.toString("dd/MM/yyyy") + "\n\n";
    
    report += "CLASSIFICATION\n";
    report += "------------------------\n";
    report += "Statut Fidélité: " + client.getStatutFidelite() + "\n";
    report += "Score Fidélité: " + QString::number(client.getScoreFidelite(), 'f', 2) + "\n";
    report += "Importance: " + client.getImportance() + "\n\n";
    
    report += "========================================\n";
    report += "Généré le: " + QDate::currentDate().toString("dd/MM/yyyy") + "\n";
    report += "========================================\n";
    
    ui->textEdit_rapport->setText(report);
}

void MainWindow::on_pushButton_rapport_clicked() {
    // DÉSACTIVÉ : Interface statique - Aucune action
}

void MainWindow::on_pushButton_exportRapport_clicked() {
    // DÉSACTIVÉ : Interface statique - Aucune action
}

// Mise à jour automatique de la date et l'heure
void MainWindow::updateDateTime() {
    // Récupérer la date et l'heure actuelles
    QDate currentDate = QDate::currentDate();
    QTime currentTime = QTime::currentTime();
    
    // Tableau des noms de mois en français
    QStringList monthNames = {"Janvier", "Février", "Mars", "Avril", "Mai", "Juin",
                              "Juillet", "Août", "Septembre", "Octobre", "Novembre", "Décembre"};
    
    // Mettre à jour l'icône du calendrier avec le jour du mois
    ui->calendarIconLabel->setText(QString("%1").arg(currentDate.day(), 2, 10, QChar('0')));
    
    // Formater la date et l'heure : "05 Février 2026  •  14:30"
    QString formattedDateTime = QString("%1 %2 %3  •  %4")
                                    .arg(currentDate.day(), 2, 10, QChar('0'))
                                    .arg(monthNames[currentDate.month() - 1])
                                    .arg(currentDate.year())
                                    .arg(currentTime.toString("HH:mm"));
    
    ui->dateTimeLabel->setText(formattedDateTime);
}
