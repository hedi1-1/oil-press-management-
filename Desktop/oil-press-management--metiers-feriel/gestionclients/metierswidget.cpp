#include "metierswidget.h"
#include "ui_metierswidget.h"
#include <QMessageBox>
#include <QTableWidgetItem>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <algorithm>

MetiersWidget::MetiersWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MetiersWidget)
    , clients(nullptr)
{
    ui->setupUi(this);
    
    ui->tableView_resultats->setColumnCount(7);
    ui->tableView_resultats->setHorizontalHeaderLabels({"ID", "Nom", "Prénom", "Type", "Total Olives (kg)", "Date", "Statut"});
    ui->tableView_resultats->horizontalHeader()->setStretchLastSection(true);
}

MetiersWidget::~MetiersWidget()
{
    delete ui;
}

void MetiersWidget::setClientsData(QList<Client> *clientsList)
{
    clients = clientsList;
}

void MetiersWidget::refreshResultsTableView(const QList<Client> &filteredClients)
{
    ui->tableView_resultats->setRowCount(0);
    
    for (int i = 0; i < filteredClients.count(); ++i) {
        const Client &client = filteredClients[i];
        int row = ui->tableView_resultats->rowCount();
        ui->tableView_resultats->insertRow(row);
        
        ui->tableView_resultats->setItem(row, 0, new QTableWidgetItem(QString::number(client.id_client)));
        ui->tableView_resultats->setItem(row, 1, new QTableWidgetItem(client.nom));
        ui->tableView_resultats->setItem(row, 2, new QTableWidgetItem(client.prenom));
        ui->tableView_resultats->setItem(row, 3, new QTableWidgetItem(client.type_client));
        ui->tableView_resultats->setItem(row, 4, new QTableWidgetItem(QString::number(client.total_olives_livrees)));
        ui->tableView_resultats->setItem(row, 5, new QTableWidgetItem(client.date_creation.toString("dd/MM/yyyy")));
        ui->tableView_resultats->setItem(row, 6, new QTableWidgetItem(client.statut));
    }
}

void MetiersWidget::on_pushButton_rechercher_clicked()
{
    if (!clients) {
        QMessageBox::warning(this, "Erreur", "Aucune donnée client disponible!");
        return;
    }
    
    QString critere = ui->comboBox_critere->currentText();
    QString valeur = ui->lineEdit_recherche->text().toLower();
    
    if (valeur.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Veuillez entrer une valeur de recherche!");
        return;
    }
    
    QList<Client> resultats;
    for (const Client &client : *clients) {
        if (critere == "nom" && client.nom.toLower().contains(valeur)) {
            resultats.append(client);
        } else if (critere == "prenom" && client.prenom.toLower().contains(valeur)) {
            resultats.append(client);
        } else if (critere == "telephone" && client.telephone.contains(valeur)) {
            resultats.append(client);
        } else if (critere == "email" && client.email.toLower().contains(valeur)) {
            resultats.append(client);
        } else if (critere == "type_client" && client.type_client.toLower().contains(valeur)) {
            resultats.append(client);
        }
    }
    
    refreshResultsTableView(resultats);
    QMessageBox::information(this, "Recherche", QString("Trouvé %1 client(s)").arg(resultats.count()));
}

void MetiersWidget::on_pushButton_trier_clicked()
{
    if (!clients) {
        QMessageBox::warning(this, "Erreur", "Aucune donnée client disponible!");
        return;
    }
    
    QString critere = ui->comboBox_tri->currentText();
    QList<Client> sortedClients = *clients;
    
    if (critere == "nom") {
        std::sort(sortedClients.begin(), sortedClients.end(), 
            [](const Client &a, const Client &b) { return a.nom < b.nom; });
    } else if (critere == "prenom") {
        std::sort(sortedClients.begin(), sortedClients.end(), 
            [](const Client &a, const Client &b) { return a.prenom < b.prenom; });
    } else if (critere == "type_client") {
        std::sort(sortedClients.begin(), sortedClients.end(), 
            [](const Client &a, const Client &b) { return a.type_client < b.type_client; });
    } else if (critere == "total_olives_livrees") {
        std::sort(sortedClients.begin(), sortedClients.end(), 
            [](const Client &a, const Client &b) { return a.total_olives_livrees > b.total_olives_livrees; });
    } else if (critere == "date_creation") {
        std::sort(sortedClients.begin(), sortedClients.end(), 
            [](const Client &a, const Client &b) { return a.date_creation > b.date_creation; });
    }
    
    refreshResultsTableView(sortedClients);
    QMessageBox::information(this, "Tri", "Données triées avec succès!");
}

void MetiersWidget::exportToTxt(const QList<Client> &data)
{
    QString fileName = QFileDialog::getSaveFileName(this, "Exporter en TXT", "", "Text Files (*.txt)");
    if (fileName.isEmpty()) return;
    
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Erreur", "Impossible de créer le fichier!");
        return;
    }
    
    QTextStream out(&file);
    out << "========================================\n";
    out << "    LISTE DES CLIENTS\n";
    out << "========================================\n\n";
    
    for (const Client &client : data) {
        out << "ID: " << client.id_client << "\n";
        out << "Nom: " << client.nom << "\n";
        out << "Prénom: " << client.prenom << "\n";
        out << "Téléphone: " << client.telephone << "\n";
        out << "Email: " << client.email << "\n";
        out << "Type: " << client.type_client << "\n";
        out << "Total Olives: " << client.total_olives_livrees << " kg\n";
        out << "Statut: " << client.statut << "\n";
        out << "----------------------------------------\n\n";
    }
    
    file.close();
    QMessageBox::information(this, "Succès", "Export TXT réussi!");
}

void MetiersWidget::on_pushButton_txt_clicked()
{
    if (!clients || clients->isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Aucune donnée à exporter!");
        return;
    }
    exportToTxt(*clients);
}

void MetiersWidget::on_pushButton_pdf_clicked()
{
    QMessageBox::information(this, "Export PDF", "Fonctionnalité d'export PDF à implémenter!");
}

void MetiersWidget::on_pushButton_excel_clicked()
{
    QMessageBox::information(this, "Export Excel", "Fonctionnalité d'export Excel à implémenter!");
}

void MetiersWidget::on_pushButton_word_clicked()
{
    QMessageBox::information(this, "Export Word", "Fonctionnalité d'export Word à implémenter!");
}

void MetiersWidget::on_pushButton_stat_clicked()
{
    if (!clients || clients->isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Aucune donnée disponible!");
        return;
    }
    
    int totalClients = clients->count();
    int particuliers = 0, societes = 0;
    int bronze = 0, silver = 0, gold = 0, premium = 0;
    double totalOlives = 0;
    
    for (const Client &client : *clients) {
        if (client.type_client == "Particulier") particuliers++;
        else societes++;
        
        if (client.statut == "Bronze") bronze++;
        else if (client.statut == "Silver") silver++;
        else if (client.statut == "Gold") gold++;
        else if (client.statut == "Premium") premium++;
        
        totalOlives += client.total_olives_livrees;
    }
    
    QString stats;
    stats += "========================================\n";
    stats += "        STATISTIQUES CLIENTS\n";
    stats += "========================================\n\n";
    stats += "📊 Total Clients: " + QString::number(totalClients) + "\n\n";
    stats += "👥 Types de Clients:\n";
    stats += "   • Particuliers: " + QString::number(particuliers) + "\n";
    stats += "   • Sociétés: " + QString::number(societes) + "\n\n";
    stats += "🏆 Fidélité:\n";
    stats += "   • Bronze: " + QString::number(bronze) + "\n";
    stats += "   • Silver: " + QString::number(silver) + "\n";
    stats += "   • Gold: " + QString::number(gold) + "\n";
    stats += "   • Premium: " + QString::number(premium) + "\n\n";
    stats += "🫒 Total Olives: " + QString::number(totalOlives, 'f', 2) + " kg\n";
    stats += "📈 Moyenne: " + QString::number(totalOlives / totalClients, 'f', 2) + " kg/client\n";
    stats += "========================================\n";
    
    ui->textEdit_stats->setPlainText(stats);
}

void MetiersWidget::on_tableView_resultats_doubleClicked(const QModelIndex &index)
{
    if (!index.isValid() || !clients) return;
    
    int row = index.row();
    int clientId = ui->tableView_resultats->item(row, 0)->text().toInt();
    
    for (const Client &client : *clients) {
        if (client.id_client == clientId) {
            displayClientInfo(client);
            break;
        }
    }
}

void MetiersWidget::displayClientInfo(const Client &client)
{
    QString info;
    info += "========================================\n";
    info += "        INFORMATIONS CLIENT\n";
    info += "========================================\n\n";
    info += "ID: " + QString::number(client.id_client) + "\n";
    info += "Nom: " + client.nom + " " + client.prenom + "\n";
    info += "Type: " + client.type_client + "\n";
    info += "Téléphone: " + client.telephone + "\n";
    info += "Email: " + client.email + "\n\n";
    info += "📊 FIDÉLITÉ:\n";
    info += "Statut: " + client.statut + "\n";
    info += "Importance: " + client.importance + "\n";
    info += "Score: " + QString::number(client.getScoreFidelite(), 'f', 1) + "\n";
    info += "Total Olives: " + QString::number(client.total_olives_livrees) + " kg\n";
    info += "========================================\n";
    
    QMessageBox::information(this, "Détails Client", info);
}

void MetiersWidget::generateReport(const Client &client)
{
    QString report;
    report += "========================================\n";
    report += "        RAPPORT CLIENT DÉTAILLÉ\n";
    report += "========================================\n\n";
    report += "📋 INFORMATIONS GÉNÉRALES\n";
    report += "ID: " + QString::number(client.id_client) + "\n";
    report += "Nom complet: " + client.nom + " " + client.prenom + "\n";
    report += "Type: " + client.type_client + "\n";
    report += "Date d'inscription: " + client.date_creation.toString("dd/MM/yyyy") + "\n\n";
    report += "📞 CONTACT\n";
    report += "Téléphone: " + client.telephone + "\n";
    report += "Email: " + client.email + "\n";
    report += "Adresse: " + client.adresse + "\n\n";
    report += "📊 ACTIVITÉ\n";
    report += "Total Olives Livrées: " + QString::number(client.total_olives_livrees) + " kg\n";
    report += "Statut Fidélité: " + client.statut + "\n";
    report += "Niveau Importance: " + client.importance + "\n";
    report += "Score Fidélité: " + QString::number(client.getScoreFidelite(), 'f', 2) + "\n\n";
    report += "========================================\n";
    report += "Rapport généré le: " + QDate::currentDate().toString("dd/MM/yyyy") + "\n";
    report += "========================================\n";
    
    ui->textEdit_rapport->setPlainText(report);
}

void MetiersWidget::on_pushButton_rapport_clicked()
{
    if (!clients || clients->isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Aucune donnée disponible!");
        return;
    }
    
    generateReport(clients->first());
    QMessageBox::information(this, "Rapport", "Rapport généré avec succès!");
}

void MetiersWidget::on_pushButton_exportRapport_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Exporter le Rapport", "", "Text Files (*.txt)");
    if (fileName.isEmpty()) return;
    
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Erreur", "Impossible de créer le fichier!");
        return;
    }
    
    QTextStream out(&file);
    out << ui->textEdit_rapport->toPlainText();
    file.close();
    
    QMessageBox::information(this, "Succès", "Rapport exporté avec succès!");
}
