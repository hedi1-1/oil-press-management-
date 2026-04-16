#include "metierswidget.h"
#include "ui_metierswidget.h"
#include <QMessageBox>
#include <QTableWidgetItem>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QDate>
#include <QLocale>
#include <QPainter>
#include <QPdfWriter>
#include <QLabel>
#include <QFont>
#include <QItemSelectionModel>
#include <QPageSize>
#include <QSet>
#include <QStringConverter>
#include <QTextDocument>
#include <algorithm>

MetiersWidget::MetiersWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MetiersWidget)
    , clients(nullptr)
    , chartView(nullptr)
    , fideliteChartView(nullptr)
{
    ui->setupUi(this);
    
    ui->tableView_resultats->setColumnCount(7);
    ui->tableView_resultats->setHorizontalHeaderLabels({"ID", "Nom", "Prénom", "Type", "Total Olives (kg)", "Date", "Statut"});
    ui->tableView_resultats->horizontalHeader()->setStretchLastSection(true);

    ui->tableView_fidelite->setColumnCount(6);
    ui->tableView_fidelite->setHorizontalHeaderLabels({"ID", "Nom", "Prénom", "Total Olives (kg)", "Niveau", "Progression"});
    ui->tableView_fidelite->horizontalHeader()->setStretchLastSection(true);

    initializeStatsChart();
    initializeFideliteChart();
    refreshReportClients();

    connect(ui->comboBox_chartType, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MetiersWidget::onChartConfigChanged);
    connect(ui->comboBox_statData, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MetiersWidget::onChartConfigChanged);
        connect(ui->comboBox_rapportClient, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MetiersWidget::onRapportClientChanged);
}

MetiersWidget::~MetiersWidget()
{
    delete ui;
}

void MetiersWidget::setClientsData(QList<Client> *clientsList)
{
    clients = clientsList;
    updateStatsChart();
    refreshFideliteTableView();
    refreshReportClients();
}

void MetiersWidget::refreshFromClients()
{
    updateStatsChart();
    refreshFideliteTableView();
    refreshReportClients();
    updateStrategicPanels();
    updateInsightPanel();
    updateRapportIdeaPanel();
}

void MetiersWidget::refreshFideliteTableView()
{
    ui->tableView_fidelite->setRowCount(0);
    if (!clients) return;

    int bronze = 0, silver = 0, gold = 0, premium = 0;

    auto levelColor = [](const QString &level) {
        if (level == "Bronze") return QString("#CD7F32");
        if (level == "Silver") return QString("#C0C0C0");
        if (level == "Gold") return QString("#D4AF37");
        return QString("#1B4332");
    };

    auto progressToNext = [](double totalKg) {
        if (totalKg < 2000.0) return int((totalKg / 2000.0) * 100.0);
        if (totalKg < 5000.0) return int(((totalKg - 2000.0) / 3000.0) * 100.0);
        if (totalKg < 8000.0) return int(((totalKg - 5000.0) / 3000.0) * 100.0);
        return 100;
    };

    for (const Client &client : *clients) {
        int row = ui->tableView_fidelite->rowCount();
        ui->tableView_fidelite->insertRow(row);

        QString level = client.getStatutFidelite();
        if (level == "Bronze") bronze++;
        else if (level == "Silver") silver++;
        else if (level == "Gold") gold++;
        else premium++;

        ui->tableView_fidelite->setItem(row, 0, new QTableWidgetItem(QString::number(client.id_client)));
        ui->tableView_fidelite->setItem(row, 1, new QTableWidgetItem(client.nom));
        ui->tableView_fidelite->setItem(row, 2, new QTableWidgetItem(client.prenom));
        ui->tableView_fidelite->setItem(row, 3, new QTableWidgetItem(QString::number(client.total_olives_livrees, 'f', 0)));

        QLabel *badge = new QLabel(level);
        badge->setAlignment(Qt::AlignCenter);
        badge->setStyleSheet(QString("background:%1;color:white;border-radius:8px;padding:2px 8px;font-weight:600;")
                              .arg(levelColor(level)));
        ui->tableView_fidelite->setCellWidget(row, 4, badge);

        QProgressBar *bar = new QProgressBar();
        int progress = progressToNext(client.total_olives_livrees);
        bar->setRange(0, 100);
        bar->setValue(progress);
        bar->setFormat(QString::number(progress) + "%");
        bar->setTextVisible(true);
        bar->setStyleSheet(
            "QProgressBar{border:1px solid #d0d0d0;border-radius:6px;background:#f3f4f6;height:14px;}"
            "QProgressBar::chunk{background:#1B4332;border-radius:6px;}");
        ui->tableView_fidelite->setCellWidget(row, 5, bar);

        if (level == "Premium") {
            for (int col = 0; col < 4; ++col) {
                QTableWidgetItem *item = ui->tableView_fidelite->item(row, col);
                if (item) {
                    item->setBackground(QColor(255, 247, 214));
                    item->setFont(QFont(item->font().family(), item->font().pointSize(), QFont::Bold));
                }
            }
        }
    }

    ui->lblFidBronzeCount->setText(QString::number(bronze));
    ui->lblFidSilverCount->setText(QString::number(silver));
    ui->lblFidGoldCount->setText(QString::number(gold));
    ui->lblFidPremiumCount->setText(QString::number(premium));

    updateFideliteChart();
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
    out << buildClientsPlainText(data);
    
    file.close();
    QMessageBox::information(this, "Succès", "Export TXT réussi!");
}

QList<Client> MetiersWidget::collectExportClients() const
{
    QList<Client> exportData;
    if (!clients || clients->isEmpty()) {
        return exportData;
    }

    QSet<int> idsToExport;
    const QItemSelectionModel *selectionModel = ui->tableView_resultats->selectionModel();

    if (selectionModel) {
        const QModelIndexList selectedRows = selectionModel->selectedRows();
        for (const QModelIndex &index : selectedRows) {
            QTableWidgetItem *idItem = ui->tableView_resultats->item(index.row(), 0);
            if (!idItem) continue;
            bool ok = false;
            const int id = idItem->text().toInt(&ok);
            if (ok) idsToExport.insert(id);
        }
    }

    if (idsToExport.isEmpty() && ui->tableView_resultats->rowCount() > 0) {
        for (int row = 0; row < ui->tableView_resultats->rowCount(); ++row) {
            QTableWidgetItem *idItem = ui->tableView_resultats->item(row, 0);
            if (!idItem) continue;
            bool ok = false;
            const int id = idItem->text().toInt(&ok);
            if (ok) idsToExport.insert(id);
        }
    }

    if (idsToExport.isEmpty()) {
        return *clients;
    }

    for (const Client &client : *clients) {
        if (idsToExport.contains(client.id_client)) {
            exportData.append(client);
        }
    }

    return exportData;
}

QString MetiersWidget::buildClientsPlainText(const QList<Client> &data) const
{
    QString text;
    const int total = data.size();
    int particuliers = 0;
    int societes = 0;
    int cooperatives = 0;
    int autres = 0;
    double totalOlives = 0.0;

    for (const Client &client : data) {
        if (client.type_client == "Particulier") particuliers++;
        else if (client.type_client == "Société") societes++;
        else if (client.type_client == "Coopérative") cooperatives++;
        else autres++;
        totalOlives += client.total_olives_livrees;
    }

    text += "============================================================\n";
    text += "                   LISTE DES CLIENTS\n";
    text += "============================================================\n";
    text += "Date: " + QDate::currentDate().toString("dd/MM/yyyy") + "\n";
    text += "Total exporté: " + QString::number(total) + " client(s)\n";
    text += "Total olives: " + QString::number(totalOlives, 'f', 0) + " kg\n";
    text += "------------------------------------------------------------\n";
    text += "Répartition par type:\n";
    text += "  - Particulier: " + QString::number(particuliers) + " (" + percentString(particuliers, total) + ")\n";
    text += "  - Société: " + QString::number(societes) + " (" + percentString(societes, total) + ")\n";
    if (cooperatives > 0) {
        text += "  - Coopérative: " + QString::number(cooperatives) + " (" + percentString(cooperatives, total) + ")\n";
    }
    if (autres > 0) {
        text += "  - Autres: " + QString::number(autres) + " (" + percentString(autres, total) + ")\n";
    }
    text += "------------------------------------------------------------\n";
    text += "ID   | Nom complet              | Type        | Total (kg) | Statut\n";
    text += "------------------------------------------------------------\n";

    for (const Client &client : data) {
        QString nomComplet = (client.nom + " " + client.prenom).left(24);
        QString type = client.type_client.left(10);
        text += QString("%1 | %2 | %3 | %4 | %5\n")
                    .arg(QString::number(client.id_client).rightJustified(4, ' '))
                    .arg(nomComplet.leftJustified(24, ' '))
                    .arg(type.leftJustified(10, ' '))
                    .arg(QString::number(client.total_olives_livrees, 'f', 0).rightJustified(9, ' '))
                    .arg(client.statut);
    }

    text += "============================================================\n";
    return text;
}

QString MetiersWidget::buildClientsHtmlTable(const QList<Client> &data) const
{
    const int total = data.size();
    int particuliers = 0;
    int societes = 0;
    int cooperatives = 0;
    int autres = 0;
    double totalOlives = 0.0;

    for (const Client &client : data) {
        if (client.type_client == "Particulier") particuliers++;
        else if (client.type_client == "Société") societes++;
        else if (client.type_client == "Coopérative") cooperatives++;
        else autres++;
        totalOlives += client.total_olives_livrees;
    }

    QString html;
            html += "<html><head><meta charset=\"UTF-8\">";
            html += "<style>"
                "body{font-family:'Segoe UI',Arial,sans-serif;color:#111827;margin:26px;background:#ffffff;}"
                ".title{font-size:26px;font-weight:700;text-align:center;margin:0 0 6px 0;}"
                ".date{font-size:13px;text-align:center;margin:0 0 16px 0;color:#4b5563;font-weight:600;}"
                ".summary{display:flex;gap:14px;flex-wrap:wrap;margin:10px 0 14px 0;justify-content:center;}"
                ".card{border:1px solid #e5e7eb;border-radius:10px;padding:10px 14px;background:#f8fafc;min-width:160px;}"
                ".label{font-size:11px;color:#6b7280;text-transform:uppercase;letter-spacing:0.05em;}"
                ".value{font-size:18px;font-weight:700;color:#111827;}"
                ".types{margin:6px auto 16px auto;max-width:820px;border:1px solid #e5e7eb;border-radius:8px;padding:10px 12px;background:#f9fafb;}"
                ".types p{margin:4px 0;font-size:12px;color:#111827;}"
                "table{width:90%;margin:0 auto;border-collapse:collapse;font-size:12px;}"
                "th,td{border:1px solid #999;padding:8px;}"
                "th{background:#4FC3F7;color:#111827;font-weight:700;text-align:center;}"
                "tbody tr:nth-child(odd){background:#F5F5F5;}"
                "tbody tr:nth-child(even){background:#FFFFFF;}"
                "tbody tr:hover{background:#FFF9C4;}"
                ".tag{display:inline-block;padding:2px 9px;border-radius:999px;background:#e5e7eb;font-size:11px;}"
                "footer{margin-top:14px;color:#6b7280;font-size:11px;text-align:center;}"
                "</style></head><body>";
            html += "<h1 class=\"title\">Liste des clients</h1>";
            html += "<div class=\"date\">Généré le " + QDate::currentDate().toString("dd/MM/yyyy") + "</div>";
    html += "<div class=\"summary\">";
    html += "<div class=\"card\"><div class=\"label\">Total clients</div><div class=\"value\">" + QString::number(total) + "</div></div>";
    html += "<div class=\"card\"><div class=\"label\">Total olives</div><div class=\"value\">" + QString::number(totalOlives, 'f', 0) + " kg</div></div>";
    html += "<div class=\"card\"><div class=\"label\">Moyenne</div><div class=\"value\">" + (total > 0 ? QString::number(totalOlives / total, 'f', 0) : "0") + " kg/client</div></div>";
    html += "</div>";
    html += "<div class=\"types\">";
    html += "<p><b>Particulier:</b> " + QString::number(particuliers) + " (" + percentString(particuliers, total) + ")</p>";
    html += "<p><b>Société:</b> " + QString::number(societes) + " (" + percentString(societes, total) + ")</p>";
    if (cooperatives > 0) {
        html += "<p><b>Coopérative:</b> " + QString::number(cooperatives) + " (" + percentString(cooperatives, total) + ")</p>";
    }
    if (autres > 0) {
        html += "<p><b>Autres:</b> " + QString::number(autres) + " (" + percentString(autres, total) + ")</p>";
    }
    html += "</div>";
    html += "<table>";
    html += "<tr><th>ID</th><th>Nom</th><th>Prénom</th><th>Téléphone</th><th>Email</th><th>Type</th><th>Total (kg)</th><th>Statut</th></tr>";

    for (const Client &client : data) {
        html += "<tr>";
        html += "<td>" + QString::number(client.id_client).toHtmlEscaped() + "</td>";
        html += "<td>" + client.nom.toHtmlEscaped() + "</td>";
        html += "<td>" + client.prenom.toHtmlEscaped() + "</td>";
        html += "<td>" + client.telephone.toHtmlEscaped() + "</td>";
        html += "<td>" + client.email.toHtmlEscaped() + "</td>";
        html += "<td>" + client.type_client.toHtmlEscaped() + "</td>";
        html += "<td>" + QString::number(client.total_olives_livrees, 'f', 0).toHtmlEscaped() + "</td>";
        html += "<td><span class=\"tag\">" + client.statut.toHtmlEscaped() + "</span></td>";
        html += "</tr>";
    }

    html += "</table>";
    html += "<footer>Total exporté: " + QString::number(total) + " client(s)</footer>";
    html += "</body></html>";
    return html;
}

QString MetiersWidget::percentString(int part, int total) const
{
    if (total <= 0) return "0 %";
    double percent = (part * 100.0) / total;
    return QString::number(percent, 'f', 0) + " %";
}

void MetiersWidget::on_pushButton_txt_clicked()
{
    const QList<Client> data = collectExportClients();
    if (data.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Aucune donnée à exporter!");
        return;
    }
    exportToTxt(data);
}

void MetiersWidget::on_pushButton_pdf_clicked()
{
    const QList<Client> data = collectExportClients();
    if (data.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Aucune donnée à exporter!");
        return;
    }

    QString fileName = QFileDialog::getSaveFileName(this, "Exporter en PDF", "", "PDF Files (*.pdf)");
    if (fileName.isEmpty()) return;

    QPdfWriter writer(fileName);
    writer.setPageSize(QPageSize(QPageSize::A4));
    writer.setResolution(300);

    QTextDocument document;
    document.setHtml(buildClientsHtmlTable(data));
    document.print(&writer);

    QMessageBox::information(this, "Succès", "Export PDF réussi!");
}

void MetiersWidget::on_pushButton_excel_clicked()
{
    const QList<Client> data = collectExportClients();
    if (data.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Aucune donnée à exporter!");
        return;
    }

    QString fileName = QFileDialog::getSaveFileName(this, "Exporter en Excel", "", "Excel Files (*.xls)");
    if (fileName.isEmpty()) return;

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Erreur", "Impossible de créer le fichier!");
        return;
    }

    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);
    out << "\uFEFF";
    out << buildClientsHtmlTable(data);

    file.close();
    QMessageBox::information(this, "Succès", "Export Excel réussi!");
}

void MetiersWidget::on_pushButton_word_clicked()
{
    const QList<Client> data = collectExportClients();
    if (data.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Aucune donnée à exporter!");
        return;
    }

    QString fileName = QFileDialog::getSaveFileName(this, "Exporter en Word", "", "Word Document (*.doc)");
    if (fileName.isEmpty()) return;

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Erreur", "Impossible de créer le fichier!");
        return;
    }

    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);
    out << buildClientsHtmlTable(data);
    file.close();

    QMessageBox::information(this, "Succès", "Export Word réussi!");
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
    int standard = 0, moyen = 0, important = 0;
    QMap<QDate, int> creationsParMois;
    double totalOlives = 0;

    
    for (const Client &client : *clients) {
        if (client.type_client == "Particulier") particuliers++;
        else societes++;
        
        if (client.statut == "Bronze") bronze++;
        else if (client.statut == "Silver") silver++;
        else if (client.statut == "Gold") gold++;
        else if (client.statut == "Premium") premium++;

        QString importance = client.getImportance();
        if (importance == "Standard") standard++;
        else if (importance == "Moyen") moyen++;
        else if (importance == "Important") important++;

        if (client.date_creation.isValid()) {
            QDate key(client.date_creation.year(), client.date_creation.month(), 1);
            creationsParMois[key] += 1;
        }
        
        totalOlives += client.total_olives_livrees;
    }
    
    QString stats;
    stats += "========================================\n";
    stats += "        STATISTIQUES CLIENTS\n";
    stats += "========================================\n\n";
    stats += "📊 Total Clients: " + QString::number(totalClients) + "\n\n";
    stats += "👥 Types de Clients:\n";
    stats += "   • Particuliers: " + QString::number(particuliers) + " (" + percentString(particuliers, totalClients) + ")\n";
    stats += "   • Sociétés: " + QString::number(societes) + " (" + percentString(societes, totalClients) + ")\n\n";
    stats += "🏆 Fidélité:\n";
    stats += "   • Bronze: " + QString::number(bronze) + " (" + percentString(bronze, totalClients) + ")\n";
    stats += "   • Silver: " + QString::number(silver) + " (" + percentString(silver, totalClients) + ")\n";
    stats += "   • Gold: " + QString::number(gold) + " (" + percentString(gold, totalClients) + ")\n";
    stats += "   • Premium: " + QString::number(premium) + " (" + percentString(premium, totalClients) + ")\n\n";
    stats += "🧭 Importance (Total olives):\n";
    stats += "   • Standard: " + QString::number(standard) + " (" + percentString(standard, totalClients) + ")\n";
    stats += "   • Moyen: " + QString::number(moyen) + " (" + percentString(moyen, totalClients) + ")\n";
    stats += "   • Important: " + QString::number(important) + " (" + percentString(important, totalClients) + ")\n\n";
    stats += "📅 Créations par mois:\n";
    for (auto it = creationsParMois.constBegin(); it != creationsParMois.constEnd(); ++it) {
        stats += "   • " + QLocale::system().toString(it.key(), "MMMM yyyy") + ": " + QString::number(it.value()) + " (" + percentString(it.value(), totalClients) + ")\n";
    }
    stats += "\n";
    stats += "🫒 Total Olives: " + QString::number(totalOlives, 'f', 2) + " kg\n";
    stats += "📈 Moyenne: " + QString::number(totalOlives / totalClients, 'f', 2) + " kg/client\n";
    stats += "========================================\n";

    
    ui->textEdit_stats->setPlainText(stats);
    updateStatsChart();
    updateStrategicPanels();
    updateInsightPanel();
}

void MetiersWidget::updateStrategicPanels()
{
    if (!ui->textEdit_metiersA || !ui->textEdit_metiersB) return;

    if (!clients || clients->isEmpty()) {
        ui->textEdit_metiersA->setPlainText("Aucune donnée disponible.");
        ui->textEdit_metiersB->setPlainText("Aucune donnée disponible.");
        return;
    }

    QStringList hautePriorite;
    QStringList moyennePriorite;
    QStringList bassePriorite;

    int vipASoigner = 0;
    int aFideliser = 0;
    int aReactiver = 0;
    int grosVolumes = 0;

    const QDate today = QDate::currentDate();

    auto fideliteLabel = [](const Client &client) {
        const QString niveau = client.getStatutFidelite();
        if (niveau == "Premium" || niveau == "Gold") return QString("haute");
        if (niveau == "Silver") return QString("moyenne");
        return QString("faible");
    };

    for (const Client &client : *clients) {
        const QString fidelite = fideliteLabel(client);
        const bool isImportant = (client.statut == "Important");
        const bool isStandard = (client.statut == "Standard");
        const bool isSociete = (client.type_client == "Société");
        const bool isParticulier = (client.type_client == "Particulier");
        const bool olderThanYear = client.date_creation.isValid() && client.date_creation.daysTo(today) > 365;
        const bool newerThan30Days = client.date_creation.isValid() && client.date_creation.daysTo(today) < 30;
        const bool hugeVolume = client.total_olives_livrees > 1000000.0;

        const QString fullName = client.nom + " " + client.prenom;

        if (isImportant && fidelite == "haute") {
            vipASoigner++;
            hautePriorite << QString("%1 → Appel prioritaire").arg(fullName);
        } else if (isImportant && fidelite == "moyenne") {
            aFideliser++;
            moyennePriorite << QString("%1 → SMS relance").arg(fullName);
        }

        if (isStandard && isSociete) {
            moyennePriorite << QString("%1 → Email offre société").arg(fullName);
        }

        if (isStandard && isParticulier && olderThanYear) {
            aReactiver++;
            bassePriorite << QString("%1 → Alerte réactivation").arg(fullName);
        }

        if (hugeVolume) {
            grosVolumes++;
            hautePriorite << QString("%1 → Offre spéciale volume").arg(fullName);
        }

        if (newerThan30Days) {
            hautePriorite << QString("%1 → Bienvenue + appel découverte").arg(fullName);
        }
    }

    QString htmlA;
    htmlA += "<div><span style='color:#D32F2F;font-weight:700;'>🔴 Haute priorité</span></div>";
    if (hautePriorite.isEmpty()) {
        htmlA += "<div>Aucune action</div>";
    } else {
        htmlA += "<ul>";
        for (const QString &item : hautePriorite) {
            htmlA += "<li>" + item.toHtmlEscaped() + "</li>";
        }
        htmlA += "</ul>";
    }

    htmlA += "<div style='margin-top:6px;'><span style='color:#F9A825;font-weight:700;'>🟡 Moyenne priorité</span></div>";
    if (moyennePriorite.isEmpty()) {
        htmlA += "<div>Aucune action</div>";
    } else {
        htmlA += "<ul>";
        for (const QString &item : moyennePriorite) {
            htmlA += "<li>" + item.toHtmlEscaped() + "</li>";
        }
        htmlA += "</ul>";
    }

    htmlA += "<div style='margin-top:6px;'><span style='color:#388E3C;font-weight:700;'>🟢 Basse priorité</span></div>";
    if (bassePriorite.isEmpty()) {
        htmlA += "<div>Aucune action</div>";
    } else {
        htmlA += "<ul>";
        for (const QString &item : bassePriorite) {
            htmlA += "<li>" + item.toHtmlEscaped() + "</li>";
        }
        htmlA += "</ul>";
    }

    QString recommandation;
    if (vipASoigner > 0) {
        recommandation = "Prioriser les VIP pour maintenir le volume et la fidélité.";
    } else if (aReactiver > 0) {
        recommandation = "Lancer une campagne de réactivation pour les anciens clients.";
    } else if (grosVolumes > 0) {
        recommandation = "Remercier les gros volumes avec une offre dédiée.";
    } else if (aFideliser > 0) {
        recommandation = "Proposer des avantages pour convertir les clients en VIP.";
    } else {
        recommandation = "Continuer la prospection et le suivi régulier des clients.";
    }

    QString htmlB;
    htmlB += "<div><b>VIP à soigner :</b> " + QString::number(vipASoigner) + "</div>";
    htmlB += "<div><b>À fidéliser :</b> " + QString::number(aFideliser) + "</div>";
    htmlB += "<div><b>À réactiver :</b> " + QString::number(aReactiver) + "</div>";
    htmlB += "<div><b>Gros volumes :</b> " + QString::number(grosVolumes) + "</div>";
    htmlB += "<div style='margin-top:8px;'><b>Recommandation :</b> " + recommandation.toHtmlEscaped() + "</div>";

    ui->textEdit_metiersA->setHtml(htmlA);
    ui->textEdit_metiersB->setHtml(htmlB);
}

void MetiersWidget::updateInsightPanel()
{
    if (!ui->textEdit_insights) return;

    if (!clients || clients->isEmpty()) {
        ui->textEdit_insights->setPlainText("Aucune donnée disponible.");
        return;
    }

    QList<Client> sorted = *clients;
    std::sort(sorted.begin(), sorted.end(), [](const Client &a, const Client &b) {
        return a.total_olives_livrees > b.total_olives_livrees;
    });

    const QDate today = QDate::currentDate();
    QStringList nouveaux;
    QStringList aReactiver;
    int particuliers = 0;
    int societes = 0;

    for (const Client &client : *clients) {
        if (client.type_client == "Particulier") particuliers++;
        else if (client.type_client == "Société") societes++;

        if (client.date_creation.isValid()) {
            int days = client.date_creation.daysTo(today);
            if (days >= 0 && days <= 30) {
                nouveaux << (client.nom + " " + client.prenom);
            }
            if (days > 365 && client.statut == "Standard") {
                aReactiver << (client.nom + " " + client.prenom);
            }
        }
    }

    QString html;
    html += "<div style='font-weight:700;'>Top 3 volumes</div>";
    html += "<ol>";
    for (int i = 0; i < sorted.size() && i < 3; ++i) {
        const Client &c = sorted[i];
        html += "<li>" + (c.nom + " " + c.prenom).toHtmlEscaped()
              + " — " + QString::number(c.total_olives_livrees, 'f', 0)
              + " kg</li>";
    }
    html += "</ol>";

    html += "<div style='margin-top:6px;font-weight:700;'>Nouveaux (30 jours)</div>";
    if (nouveaux.isEmpty()) {
        html += "<div>Aucun nouveau client</div>";
    } else {
        html += "<div>" + nouveaux.join(", ").toHtmlEscaped() + "</div>";
    }

    html += "<div style='margin-top:6px;font-weight:700;'>A reactivier</div>";
    if (aReactiver.isEmpty()) {
        html += "<div>Aucun client a reactivier</div>";
    } else {
        html += "<div>" + aReactiver.join(", ").toHtmlEscaped() + "</div>";
    }

    html += "<div style='margin-top:8px;display:flex;gap:6px;flex-wrap:wrap;'>";
    html += "<span style='background:#e8f5e9;border:1px solid #c8e6c9;border-radius:999px;padding:2px 8px;'>Particulier: " + QString::number(particuliers) + "</span>";
    html += "<span style='background:#e3f2fd;border:1px solid #bbdefb;border-radius:999px;padding:2px 8px;'>Société: " + QString::number(societes) + "</span>";
    html += "</div>";

    ui->textEdit_insights->setHtml(html);
}

void MetiersWidget::initializeStatsChart()
{
    if (!ui->statsChartLayout) return;

    chartView = new QChartView();
    chartView->setRenderHint(QPainter::Antialiasing);
    ui->statsChartLayout->addWidget(chartView);
    updateStatsChart();
}

void MetiersWidget::initializeFideliteChart()
{
    if (!ui->fideliteChartLayout) return;

    fideliteChartView = new QChartView();
    fideliteChartView->setRenderHint(QPainter::Antialiasing);
    ui->fideliteChartLayout->addWidget(fideliteChartView);
    updateFideliteChart();
}
void MetiersWidget::onChartConfigChanged()
{
    updateStatsChart();
}

void MetiersWidget::updateStatsChart()
{
    if (!chartView) return;

    QChart *chart = new QChart();
    chart->setAnimationOptions(QChart::AllAnimations);
    chart->setTheme(QChart::ChartThemeLight);

    if (!clients || clients->isEmpty()) {
        chart->setTitle("Aucune donnée disponible");
        chartView->setChart(chart);
        return;
    }

    QString chartType = ui->comboBox_chartType->currentText();
    QString dataType = ui->comboBox_statData->currentText();

    QString title;
    QStringList labels;
    QList<double> values;

    if (dataType.contains("Type client")) {
        title = "Répartition par type de client";
        QMap<QString, int> counts;
        QStringList order = {"Particulier", "Société", "Coopérative"};
        for (const QString &k : order) counts[k] = 0;
        int autres = 0;
        for (const Client &client : *clients) {
            if (counts.contains(client.type_client)) {
                counts[client.type_client] += 1;
            } else {
                autres++;
            }
        }
        for (const QString &k : order) {
            labels << k;
            values << counts[k];
        }
        if (autres > 0) {
            labels << "Autres";
            values << autres;
        }
    } else if (dataType.contains("Total olives")) {
        title = "Répartition par importance";
        QMap<QString, int> counts;
        QStringList order = {"Standard", "Moyen", "Important"};
        for (const QString &k : order) counts[k] = 0;
        for (const Client &client : *clients) {
            counts[client.getImportance()] += 1;
        }
        for (const QString &k : order) {
            labels << k;
            values << counts[k];
        }
    } else {
        title = "Créations par mois";
        QMap<QDate, int> counts;
        for (const Client &client : *clients) {
            if (!client.date_creation.isValid()) continue;
            QDate key(client.date_creation.year(), client.date_creation.month(), 1);
            counts[key] += 1;
        }
        for (auto it = counts.constBegin(); it != counts.constEnd(); ++it) {
            labels << QLocale::system().toString(it.key(), "MMMM yyyy");
            values << it.value();
        }
    }

    if (chartType.contains("Camembert")) {
        QPieSeries *series = new QPieSeries();
        double sum = 0.0;
        for (int i = 0; i < labels.size(); ++i) {
            if (values[i] <= 0) continue;
            series->append(labels[i], values[i]);
            sum += values[i];
        }
        for (QPieSlice *slice : series->slices()) {
            double percent = (sum > 0.0) ? (slice->value() * 100.0 / sum) : 0.0;
            slice->setLabel(QString("%1 (%2%)").arg(slice->label()).arg(percent, 0, 'f', 0));
            slice->setLabelVisible();
        }
        chart->addSeries(series);
        chart->setTitle(title);
        chart->legend()->setAlignment(Qt::AlignRight);
    } else {
        QHorizontalBarSeries *series = new QHorizontalBarSeries();
        QBarSet *set = new QBarSet("Total");
        for (double v : values) {
            *set << v;
        }
        series->append(set);
        chart->addSeries(series);
        chart->setTitle(title);

        QBarCategoryAxis *axisY = new QBarCategoryAxis();
        axisY->append(labels);
        chart->addAxis(axisY, Qt::AlignLeft);
        series->attachAxis(axisY);

        QValueAxis *axisX = new QValueAxis();
        axisX->setLabelFormat("%d");
        axisX->applyNiceNumbers();
        chart->addAxis(axisX, Qt::AlignBottom);
        series->attachAxis(axisX);

        chart->legend()->setVisible(false);
    }

    chartView->setChart(chart);
}

void MetiersWidget::updateFideliteChart()
{
    if (!fideliteChartView) return;

    QChart *chart = new QChart();
    chart->setAnimationOptions(QChart::AllAnimations);
    chart->setTheme(QChart::ChartThemeLight);

    if (!clients || clients->isEmpty()) {
        chart->setTitle("Aucune donnée de fidélité");
        fideliteChartView->setChart(chart);
        return;
    }

    int bronze = 0, silver = 0, gold = 0, premium = 0;
    for (const Client &client : *clients) {
        QString level = client.getStatutFidelite();
        if (level == "Bronze") bronze++;
        else if (level == "Silver") silver++;
        else if (level == "Gold") gold++;
        else premium++;
    }

    QPieSeries *series = new QPieSeries();
    if (bronze > 0) series->append("Bronze", bronze)->setBrush(QColor("#CD7F32"));
    if (silver > 0) series->append("Silver", silver)->setBrush(QColor("#C0C0C0"));
    if (gold > 0) series->append("Gold", gold)->setBrush(QColor("#D4AF37"));
    if (premium > 0) series->append("Premium", premium)->setBrush(QColor("#1B4332"));
    double sum = series->sum();
    for (QPieSlice *slice : series->slices()) {
        double percent = (sum > 0.0) ? (slice->value() * 100.0 / sum) : 0.0;
        slice->setLabel(QString("%1 (%2%)").arg(slice->label()).arg(percent, 0, 'f', 0));
        slice->setLabelVisible();
        slice->setLabelColor(QColor("#2c3e50"));
    }

    chart->addSeries(series);
    chart->setTitle("Répartition des niveaux de fidélité");
    chart->legend()->setAlignment(Qt::AlignRight);

    fideliteChartView->setChart(chart);
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
    onRapportClientChanged(ui->comboBox_rapportClient->currentIndex());
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

void MetiersWidget::on_pushButton_exportIdea_clicked()
{
    if (!ui->textEdit_rapportIdea) return;

    QString fileName = QFileDialog::getSaveFileName(this, "Exporter l'idée", "", "Text Files (*.txt)");
    if (fileName.isEmpty()) return;

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Erreur", "Impossible de créer le fichier!");
        return;
    }

    QTextStream out(&file);
    out << ui->textEdit_rapportIdea->toPlainText();
    file.close();

    QMessageBox::information(this, "Succès", "Idée exportée avec succès!");
}

void MetiersWidget::onRapportClientChanged(int index)
{
    if (index < 0) {
        ui->textEdit_rapport->setPlainText("Sélectionnez un client pour voir le rapport");
        updateRapportIdeaPanel();
        return;
    }

    int clientId = ui->comboBox_rapportClient->currentData().toInt();
    if (clientId == 0) {
        renderReportAllClients();
        updateRapportIdeaPanel();
        return;
    }

    for (const ReportClient &client : reportClients) {
        if (client.id == clientId) {
            renderReportClient(client);
            updateRapportIdeaPanel();
            return;
        }
    }
    ui->textEdit_rapport->setPlainText("Client introuvable.");
    updateRapportIdeaPanel();
}

void MetiersWidget::refreshReportClients()
{
    reportClients.clear();
    ui->comboBox_rapportClient->blockSignals(true);
    ui->comboBox_rapportClient->clear();
    ui->comboBox_rapportClient->addItem("Tous les clients", 0);

    if (clients) {
        for (const Client &client : *clients) {
            ReportClient item;
            item.id = client.id_client;
            item.nom = client.nom;
            item.prenom = client.prenom;
            item.type = client.type_client;
            item.telephone = client.telephone;
            item.email = client.email;
            item.adresse = client.adresse;
            item.dateInscription = client.date_creation;
            item.totalOlives = client.total_olives_livrees;
            reportClients.append(item);

            ui->comboBox_rapportClient->addItem(QString("ID %1").arg(client.id_client), client.id_client);
        }
    }

    ui->comboBox_rapportClient->setCurrentIndex(0);
    ui->comboBox_rapportClient->blockSignals(false);
    renderReportAllClients();
}

void MetiersWidget::renderReportClient(const ReportClient &client)
{
    Client temp;
    temp.total_olives_livrees = client.totalOlives;
    QString statutFidelite = temp.getStatutFidelite();
    QString importance = temp.getImportance();
    QString score = QString::number(temp.getScoreFidelite(), 'f', 2);

    QString report;
    report += "========================================\n";
    report += "        RAPPORT CLIENT DÉTAILLÉ\n";
    report += "========================================\n\n";
    report += "📋 INFORMATIONS GÉNÉRALES\n";
    report += "ID: " + QString::number(client.id) + "\n";
    report += "Nom complet: " + client.nom + " " + client.prenom + "\n";
    report += "Type: " + client.type + "\n";
    report += "Date d'inscription: " + client.dateInscription.toString("dd/MM/yyyy") + "\n\n";
    report += "📞 CONTACT\n";
    report += "Téléphone: " + client.telephone + "\n";
    report += "Email: " + client.email + "\n";
    report += "Adresse: " + client.adresse + "\n\n";
    report += "📊 ACTIVITÉ\n";
    report += "Total Olives Livrées: " + QString::number(client.totalOlives, 'f', 0) + " kg\n";
    report += "Statut Fidélité: " + statutFidelite + "\n";
    report += "Niveau Importance: " + importance + "\n";
    report += "Score Fidélité: " + score + "\n\n";
    report += "========================================\n";
    report += "Rapport généré le: " + QDate::currentDate().toString("dd/MM/yyyy") + "\n";
    report += "========================================\n";

    ui->textEdit_rapport->setPlainText(report);
}

void MetiersWidget::renderReportAllClients()
{
    if (reportClients.isEmpty()) {
        ui->textEdit_rapport->setPlainText("Aucun client disponible.");
        return;
    }

    QString report;
    report += "========================================\n";
    report += "        SYNTHÈSE DES CLIENTS\n";
    report += "========================================\n\n";
    report += "ID | Nom complet | Total (kg) | Fidélité | Score\n";
    report += "----------------------------------------\n";

    for (const ReportClient &client : reportClients) {
        Client temp;
        temp.total_olives_livrees = client.totalOlives;
        QString statutFidelite = temp.getStatutFidelite();
        QString score = QString::number(temp.getScoreFidelite(), 'f', 2);

        report += QString("%1 | %2 %3 | %4 | %5 | %6\n")
                  .arg(client.id)
                  .arg(client.nom)
                  .arg(client.prenom)
                  .arg(QString::number(client.totalOlives, 'f', 0))
                  .arg(statutFidelite)
                  .arg(score);
    }

    report += "\n========================================\n";
    report += "Rapport généré le: " + QDate::currentDate().toString("dd/MM/yyyy") + "\n";
    report += "========================================\n";

    ui->textEdit_rapport->setPlainText(report);
}

void MetiersWidget::updateRapportIdeaPanel()
{
    if (!ui->textEdit_rapportIdea) return;

    if (!clients || clients->isEmpty()) {
        ui->textEdit_rapportIdea->setPlainText("Aucune donnée disponible.");
        return;
    }

    QList<Client> sorted = *clients;
    std::sort(sorted.begin(), sorted.end(), [](const Client &a, const Client &b) {
        return a.total_olives_livrees > b.total_olives_livrees;
    });

    const QDate today = QDate::currentDate();
    int nouveaux30 = 0;
    int reactivation = 0;
    QStringList clientsRisque;
    double totalOlives = 0.0;

    for (const Client &client : *clients) {
        totalOlives += client.total_olives_livrees;
        if (client.date_creation.isValid()) {
            int days = client.date_creation.daysTo(today);
            if (days >= 0 && days <= 30) nouveaux30++;
            if (days > 365 && client.statut == "Standard") reactivation++;
            if (days > 365 && client.statut == "Standard" && client.getStatutFidelite() == "Bronze") {
                clientsRisque << (client.nom + " " + client.prenom);
            }
        }
    }

    QString html;
    html += "<div style='font-weight:700;'>Boussole rapide</div>";
    html += "<div style='margin:6px 0;'>Total olives: <b>" + QString::number(totalOlives, 'f', 0) + " kg</b></div>";
    html += "<div>Nouveaux (30j): <b>" + QString::number(nouveaux30) + "</b></div>";
    html += "<div>À réactiver: <b>" + QString::number(reactivation) + "</b></div>";

    html += "<div style='margin-top:8px;font-weight:700;'>Top 2 volumes</div>";
    html += "<ol>";
    for (int i = 0; i < sorted.size() && i < 2; ++i) {
        const Client &c = sorted[i];
        html += "<li>" + (c.nom + " " + c.prenom).toHtmlEscaped() + " — "
              + QString::number(c.total_olives_livrees, 'f', 0) + " kg</li>";
    }
    html += "</ol>";

    QString focus;
    if (!sorted.isEmpty()) {
        focus = "Focus du jour: appeler " + sorted.first().nom + " " + sorted.first().prenom + " (plus gros volume).";
    } else {
        focus = "Focus du jour: analyser les tendances clients.";
    }
    html += "<div style='margin-top:8px;'><b>Conseil:</b> " + focus.toHtmlEscaped() + "</div>";

    html += "<div style='margin-top:10px;font-weight:700;'>Détecteur de risque de départ</div>";
    if (clientsRisque.isEmpty()) {
        html += "<div>Aucun client à risque</div>";
    } else {
        html += "<ul>";
        for (const QString &name : clientsRisque) {
            html += "<li>" + name.toHtmlEscaped() + "</li>";
        }
        html += "</ul>";
    }

    ui->textEdit_rapportIdea->setHtml(html);
}
