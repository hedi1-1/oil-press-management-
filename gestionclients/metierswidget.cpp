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

static QString csvEscape(QString value)
{
    value.replace('"', "\"\"");
    return '"' + value + '"';
}

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
    text += "========================================\n";
    text += "    LISTE DES CLIENTS\n";
    text += "========================================\n\n";

    for (const Client &client : data) {
        text += "ID: " + QString::number(client.id_client) + "\n";
        text += "Nom: " + client.nom + "\n";
        text += "Prénom: " + client.prenom + "\n";
        text += "Téléphone: " + client.telephone + "\n";
        text += "Email: " + client.email + "\n";
        text += "Type: " + client.type_client + "\n";
        text += "Total Olives: " + QString::number(client.total_olives_livrees, 'f', 0) + " kg\n";
        text += "Statut: " + client.statut + "\n";
        text += "----------------------------------------\n\n";
    }

    text += "Total exporté: " + QString::number(data.size()) + " client(s)\n";
    return text;
}

QString MetiersWidget::buildClientsHtmlTable(const QList<Client> &data) const
{
    QString html;
    html += "<html><head><meta charset=\"UTF-8\"></head><body>";
    html += "<h2>Liste des clients</h2>";
    html += "<table border=\"1\" cellspacing=\"0\" cellpadding=\"4\">";
    html += "<tr><th>ID</th><th>Nom</th><th>Prénom</th><th>Téléphone</th><th>Email</th><th>Type</th><th>Total Olives (kg)</th><th>Statut</th></tr>";

    for (const Client &client : data) {
        html += "<tr>";
        html += "<td>" + QString::number(client.id_client).toHtmlEscaped() + "</td>";
        html += "<td>" + client.nom.toHtmlEscaped() + "</td>";
        html += "<td>" + client.prenom.toHtmlEscaped() + "</td>";
        html += "<td>" + client.telephone.toHtmlEscaped() + "</td>";
        html += "<td>" + client.email.toHtmlEscaped() + "</td>";
        html += "<td>" + client.type_client.toHtmlEscaped() + "</td>";
        html += "<td>" + QString::number(client.total_olives_livrees, 'f', 0).toHtmlEscaped() + "</td>";
        html += "<td>" + client.statut.toHtmlEscaped() + "</td>";
        html += "</tr>";
    }

    html += "</table>";
    html += "<p><b>Total exporté:</b> " + QString::number(data.size()) + " client(s)</p>";
    html += "</body></html>";
    return html;
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

    QString fileName = QFileDialog::getSaveFileName(this, "Exporter en Excel", "", "Excel CSV (*.csv)");
    if (fileName.isEmpty()) return;

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Erreur", "Impossible de créer le fichier!");
        return;
    }

    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);
    out << "\uFEFF";
    out << "ID;Nom;Prénom;Téléphone;Email;Type;Total Olives (kg);Statut\n";

    for (const Client &client : data) {
        out << csvEscape(QString::number(client.id_client)) << ';'
            << csvEscape(client.nom) << ';'
            << csvEscape(client.prenom) << ';'
            << csvEscape(client.telephone) << ';'
            << csvEscape(client.email) << ';'
            << csvEscape(client.type_client) << ';'
            << csvEscape(QString::number(client.total_olives_livrees, 'f', 0)) << ';'
            << csvEscape(client.statut) << '\n';
    }

    file.close();
    QMessageBox::information(this, "Succès", "Export Excel (CSV) réussi!");
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
    stats += "   • Particuliers: " + QString::number(particuliers) + "\n";
    stats += "   • Sociétés: " + QString::number(societes) + "\n\n";
    stats += "🏆 Fidélité:\n";
    stats += "   • Bronze: " + QString::number(bronze) + "\n";
    stats += "   • Silver: " + QString::number(silver) + "\n";
    stats += "   • Gold: " + QString::number(gold) + "\n";
    stats += "   • Premium: " + QString::number(premium) + "\n\n";
    stats += "🧭 Importance (Total olives):\n";
    stats += "   • Standard: " + QString::number(standard) + "\n";
    stats += "   • Moyen: " + QString::number(moyen) + "\n";
    stats += "   • Important: " + QString::number(important) + "\n\n";
    stats += "📅 Créations par mois:\n";
    for (auto it = creationsParMois.constBegin(); it != creationsParMois.constEnd(); ++it) {
        stats += "   • " + QLocale::system().toString(it.key(), "MMMM yyyy") + ": " + QString::number(it.value()) + "\n";
    }
    stats += "\n";
    stats += "🫒 Total Olives: " + QString::number(totalOlives, 'f', 2) + " kg\n";
    stats += "📈 Moyenne: " + QString::number(totalOlives / totalClients, 'f', 2) + " kg/client\n";
    stats += "========================================\n";
    
    ui->textEdit_stats->setPlainText(stats);
    updateStatsChart();
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
        for (int i = 0; i < labels.size(); ++i) {
            if (values[i] <= 0) continue;
            QPieSlice *slice = series->append(labels[i], values[i]);
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
    for (QPieSlice *slice : series->slices()) {
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

void MetiersWidget::onRapportClientChanged(int index)
{
    if (index < 0) {
        ui->textEdit_rapport->setPlainText("Sélectionnez un client pour voir le rapport");
        return;
    }

    int clientId = ui->comboBox_rapportClient->currentData().toInt();
    if (clientId == 0) {
        renderReportAllClients();
        return;
    }

    for (const ReportClient &client : reportClients) {
        if (client.id == clientId) {
            renderReportClient(client);
            return;
        }
    }
    ui->textEdit_rapport->setPlainText("Client introuvable.");
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
