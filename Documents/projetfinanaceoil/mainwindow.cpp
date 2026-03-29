/**
 * @file mainwindow.cpp
 * @brief Fenêtre principale de l'application Smart Oil Press Finance
 * 
 * Implémente l'interface utilisateur et la logique métier:
 * - CRUD des transactions (ajout, modification, suppression)
 * - Affichage et recherche dans QTableView
 * - Statistiques financières (revenus, dépenses, profit)
 * - Graphiques circulaires (répartition revenus/dépenses)
 * - Export de données (PDF, Excel/CSV, TXT)
 * - Prévisions et analyses avancées
 */

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QFileDialog>
#include <QTextDocument>
#include <QPrinter>
#include <QFile>
#include <QTextStream>
#include <QDate>
#include <QStandardItem>
#include <QtCharts>
#include <QStringConverter>

MainWindow::MainWindow(const QString &dbPath, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , transactionSelectionnee(-1)
    , totalRevenus(0.0)
    , totalDepenses(0.0)
    , profit(0.0)
{
    ui->setupUi(this);

    // Ouvrir la BDD via DBManager
    if (!dbManager.openDatabase(dbPath)) {
        QMessageBox::critical(this, "Erreur BDD", "Impossible d'ouvrir la base de données.");
        return;
    }
    dbManager.createTables();

    // Initialisation (catégories, UI...)
    initialiserBDD();
    configurerTableView();
    configurerConnexions();
    configurerGraphiques();
    chargerCategories();
    chargerTransactions();
    calculerStatistiques();

    // Configuration interface
    setWindowTitle("PressIQ Finance - Gestion Financière");
    ui->tabWidget->setCurrentIndex(0); // Démarrer sur l'onglet Transactions
}

MainWindow::~MainWindow()
{
    delete ui;
}

// ====================== INITIALISATION BDD ======================
void MainWindow::initialiserBDD()
{
    // La base doit être ouverte par dbManager avant d'appeler cette méthode.
    QSqlQuery query;

    // Création table CATEGORIE (si non existante)
    QString createCategorie = R"(
        CREATE TABLE IF NOT EXISTS CATEGORIE (
            id_categorie INTEGER PRIMARY KEY AUTOINCREMENT,
            nom_categorie TEXT UNIQUE NOT NULL,
            type_categorie TEXT CHECK(type_categorie IN ('Revenu', 'Dépense'))
        )
    )";

    if (!query.exec(createCategorie)) {
        QMessageBox::warning(this, "Avertissement",
                             "Table catégorie : " + query.lastError().text());
    }

    // Insertion catégories par défaut
    QStringList categoriesRevenu = {"Vente d'huile", "Vente de tourteaux", "Services", "Subventions"};
    QStringList categoriesDepense = {"Matières premières", "Électricité", "Maintenance",
                                     "Salaires", "Transport", "Emballages"};

    for (const QString &cat : categoriesRevenu) {
        query.prepare("INSERT OR IGNORE INTO CATEGORIE (nom_categorie, type_categorie) VALUES (?, 'Revenu')");
        query.addBindValue(cat);
        query.exec();
    }

    for (const QString &cat : categoriesDepense) {
        query.prepare("INSERT OR IGNORE INTO CATEGORIE (nom_categorie, type_categorie) VALUES (?, 'Dépense')");
        query.addBindValue(cat);
        query.exec();
    }

    qDebug() << "Catégories initialisées";
}

// ====================== CONFIGURATION TABLEVIEW ======================
void MainWindow::configurerTableView()
{
    tableModel = new QStandardItemModel(this);
    tableModel->setHorizontalHeaderLabels({
        "ID", "Type", "Catégorie", "Montant (DT)", "Date", "Description", "Litres", "Coût/L"
    });

    ui->tableView->setModel(tableModel);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView->horizontalHeader()->setStretchLastSection(true);
    ui->tableView->setAlternatingRowColors(true);

    // Ajuster largeurs colonnes
    ui->tableView->setColumnWidth(0, 50);   // ID
    ui->tableView->setColumnWidth(1, 80);   // Type
    ui->tableView->setColumnWidth(2, 150);  // Catégorie
    ui->tableView->setColumnWidth(3, 100);  // Montant
    ui->tableView->setColumnWidth(4, 100);  // Date
    ui->tableView->setColumnWidth(5, 250);  // Description
}

// ====================== CONNEXIONS SIGNAUX/SLOTS ======================
void MainWindow::configurerConnexions()
{
    // Boutons CRUD
    connect(ui->btnAjouter, &QPushButton::clicked, this, &MainWindow::ajouterTransaction);
    connect(ui->btnModifier, &QPushButton::clicked, this, &MainWindow::modifierTransaction);
    connect(ui->btnSupprimer, &QPushButton::clicked, this, &MainWindow::supprimerTransaction);
    connect(ui->btnVider, &QPushButton::clicked, this, &MainWindow::viderFormulaire);

    // Recherche
    connect(ui->lineRecherche, &QLineEdit::textChanged, this, &MainWindow::rechercherTransaction);

    // Sélection tableau
    connect(ui->tableView, &QTableView::clicked, this, &MainWindow::selectionnerTransaction);

    // Export
    connect(ui->btnExportPDF, &QPushButton::clicked, this, &MainWindow::exporterPDF);
    connect(ui->btnExportExcel, &QPushButton::clicked, this, &MainWindow::exporterExcel);
    connect(ui->btnExportTXT, &QPushButton::clicked, this, &MainWindow::exporterTXT);

    // Prévisions
    connect(ui->btnCalculerPrevision, &QPushButton::clicked, this, &MainWindow::calculerPrevisions);
    connect(ui->btnDepensesAnormales, &QPushButton::clicked, this, &MainWindow::detecterDepensesAnormales);
    connect(ui->btnCoutLitre, &QPushButton::clicked, this, &MainWindow::analyserCoutParLitre);

    // Changement de type
    connect(ui->comboType, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::chargerCategories);
}

// ====================== CHARGER CATÉGORIES ======================
void MainWindow::chargerCategories()
{
    QString type = ui->comboType->currentText();
    ui->comboCategorie->clear();

    QSqlQuery query;
    query.prepare("SELECT nom_categorie FROM CATEGORIE WHERE type_categorie = ?");
    query.addBindValue(type);

    if (query.exec()) {
        while (query.next()) {
            ui->comboCategorie->addItem(query.value(0).toString());
        }
    }
}

// ====================== CRUD OPERATIONS ======================

/**
 * Ajoute une nouvelle transaction à la base de données
 * - Valide le formulaire
 * - Calcule le coût par litre (si litres > 0)
 * - Insère la transaction
 * - Rafraîchit l'affichage et les statistiques
 */
void MainWindow::ajouterTransaction()
{
    if (!validerFormulaire()) {
        return;
    }

    QSqlQuery query;
    query.prepare(R"(
        INSERT INTO TRANSACTION (type, categorie, montant, date_trans, description, litres_huile, cout_par_litre)
        VALUES (?, ?, ?, ?, ?, ?, ?)
    )");

    query.addBindValue(ui->comboType->currentText());
    query.addBindValue(ui->comboCategorie->currentText());
    query.addBindValue(ui->spinMontant->value());
    query.addBindValue(ui->dateEdit->date().toString("yyyy-MM-dd"));
    query.addBindValue(ui->textDescription->toPlainText());
    query.addBindValue(ui->spinLitres->value());

    // Calcul coût par litre
    double coutParLitre = 0.0;
    if (ui->spinLitres->value() > 0) {
        coutParLitre = ui->spinMontant->value() / ui->spinLitres->value();
    }
    query.addBindValue(coutParLitre);

    if (query.exec()) {
        afficherMessage("Succès", "Transaction ajoutée avec succès !");
        viderFormulaire();
        chargerTransactions();
        calculerStatistiques();
    } else {
        afficherMessage("Erreur", "Impossible d'ajouter la transaction : " + query.lastError().text());
    }
}

/**
 * Modifie la transaction sélectionnée
 * - Vérifie qu'une transaction est sélectionnée
 * - Valide les données du formulaire
 * - Met à jour la BDD
 * - Rafraîchit l'affichage
 */
void MainWindow::modifierTransaction()
{
    if (!validerFormulaire()) {
        return;
    }

    QSqlQuery query;
    query.prepare(R"(
        UPDATE TRANSACTION
        SET type = ?, categorie = ?, montant = ?, date_trans = ?,
            description = ?, litres_huile = ?, cout_par_litre = ?
        WHERE id_transaction = ?
    )");

    query.addBindValue(ui->comboType->currentText());
    query.addBindValue(ui->comboCategorie->currentText());
    query.addBindValue(ui->spinMontant->value());
    query.addBindValue(ui->dateEdit->date().toString("yyyy-MM-dd"));
    query.addBindValue(ui->textDescription->toPlainText());
    query.addBindValue(ui->spinLitres->value());

    double coutParLitre = 0.0;
    if (ui->spinLitres->value() > 0) {
        coutParLitre = ui->spinMontant->value() / ui->spinLitres->value();
    }
    query.addBindValue(coutParLitre);
    query.addBindValue(transactionSelectionnee);

    if (query.exec()) {
        afficherMessage("Succès", "Transaction modifiée avec succès !");
        viderFormulaire();
        chargerTransactions();
        calculerStatistiques();
        transactionSelectionnee = -1;
    } else {
        afficherMessage("Erreur", "Impossible de modifier : " + query.lastError().text());
    }
}

/**
 * Supprime la transaction sélectionnée
 * - Demande confirmation
 * - Supprime de la BDD
 * - Vide le formulaire et rafraîchit l'affichage
 */
void MainWindow::supprimerTransaction()
{
    QMessageBox::StandardButton reply = QMessageBox::question(
        this, "Confirmation", "Voulez-vous vraiment supprimer cette transaction ?",
        QMessageBox::Yes | QMessageBox::No
        );

    if (reply == QMessageBox::Yes) {
        QSqlQuery query;
        query.prepare("DELETE FROM TRANSACTION WHERE id_transaction = ?");
        query.addBindValue(transactionSelectionnee);

        if (query.exec()) {
            afficherMessage("Succès", "Transaction supprimée avec succès !");
            viderFormulaire();
            chargerTransactions();
            calculerStatistiques();
            transactionSelectionnee = -1;
        } else {
            afficherMessage("Erreur", "Impossible de supprimer : " + query.lastError().text());
        }
    }
}

/**
 * Réinitialise tous les champs du formulaire
 */
void MainWindow::viderFormulaire()
{
    ui->comboType->setCurrentIndex(0);
    ui->comboCategorie->setCurrentIndex(0);
    ui->spinMontant->setValue(0.0);
    ui->dateEdit->setDate(QDate::currentDate());
    ui->textDescription->clear();
    ui->spinLitres->setValue(0.0);
    transactionSelectionnee = -1;
}

// ====================== AFFICHAGE ET RECHERCHE ======================

/**
 * Charge toutes les transactions de la BDD et les affiche dans la table
 * - Efface les anciennes lignes
 * - Charge depuis la BDD
 * - Code couleur : vert (Revenu), rose (Dépense)
 */
void MainWindow::chargerTransactions()
{
    tableModel->removeRows(0, tableModel->rowCount());

    QSqlQuery query("SELECT * FROM TRANSACTION ORDER BY date_trans DESC");

    while (query.next()) {
        QList<QStandardItem*> items;
        items << new QStandardItem(query.value("id_transaction").toString());
        items << new QStandardItem(query.value("type").toString());
        items << new QStandardItem(query.value("categorie").toString());
        items << new QStandardItem(QString::number(query.value("montant").toDouble(), 'f', 2));
        items << new QStandardItem(query.value("date_trans").toString());
        items << new QStandardItem(query.value("description").toString());
        items << new QStandardItem(QString::number(query.value("litres_huile").toDouble(), 'f', 2));
        items << new QStandardItem(QString::number(query.value("cout_par_litre").toDouble(), 'f', 3));

        // Coloration selon type
        QColor couleur = (query.value("type").toString() == "Revenu")
                             ? QColor(144, 238, 144) : QColor(255, 182, 193);
        for (auto *item : items) {
            item->setBackground(couleur);
        }

        tableModel->appendRow(items);
    }
}

/**
 * Sélectionne une transaction dans la table et remplit le formulaire
 * @param index Index du modèle de la ligne cliquée
 */
void MainWindow::selectionnerTransaction(const QModelIndex &index)
{
    int row = index.row();
    transactionSelectionnee = tableModel->item(row, 0)->text().toInt();

    // Remplir le formulaire
    ui->comboType->setCurrentText(tableModel->item(row, 1)->text());
    ui->comboCategorie->setCurrentText(tableModel->item(row, 2)->text());
    ui->spinMontant->setValue(tableModel->item(row, 3)->text().toDouble());
    ui->dateEdit->setDate(QDate::fromString(tableModel->item(row, 4)->text(), "yyyy-MM-dd"));
    ui->textDescription->setPlainText(tableModel->item(row, 5)->text());
    ui->spinLitres->setValue(tableModel->item(row, 6)->text().toDouble());
}

/**
 * Recherche dans les transactions en temps réel
 * - Filtre par type/catégorie/description
 * - Si vide, recharge toutes les transactions
 */
void MainWindow::rechercherTransaction()
{
    QString recherche = ui->lineRecherche->text();

    if (recherche.isEmpty()) {
        chargerTransactions();
        return;
    }

    tableModel->removeRows(0, tableModel->rowCount());

    QSqlQuery query;
    query.prepare(R"(
        SELECT * FROM TRANSACTION
        WHERE type LIKE ? OR categorie LIKE ? OR description LIKE ?
        ORDER BY date_trans DESC
    )");

    QString pattern = "%" + recherche + "%";
    query.addBindValue(pattern);
    query.addBindValue(pattern);
    query.addBindValue(pattern);

    if (query.exec()) {
        while (query.next()) {
            QList<QStandardItem*> items;
            items << new QStandardItem(query.value("id_transaction").toString());
            items << new QStandardItem(query.value("type").toString());
            items << new QStandardItem(query.value("categorie").toString());
            items << new QStandardItem(QString::number(query.value("montant").toDouble(), 'f', 2));
            items << new QStandardItem(query.value("date_trans").toString());
            items << new QStandardItem(query.value("description").toString());
            items << new QStandardItem(QString::number(query.value("litres_huile").toDouble(), 'f', 2));
            items << new QStandardItem(QString::number(query.value("cout_par_litre").toDouble(), 'f', 3));

            tableModel->appendRow(items);
        }
    }
}

// ====================== STATISTIQUES ET GRAPHIQUES ======================

/**
 * Calcule et affiche les statistiques financières
 * - Total revenus (type='Revenu')
 * - Total dépenses (type='Dépense')
 * - Profit (Revenus - Dépenses)
 * - Code couleur profit : vert >= 0, rouge < 0
 * - Appelle afficherGraphiques()
 */
void MainWindow::calculerStatistiques()
{
    QSqlQuery query;

    // Total revenus
    query.exec("SELECT SUM(montant) FROM TRANSACTION WHERE type = 'Revenu'");
    if (query.next()) {
        totalRevenus = query.value(0).toDouble();
    }

    // Total dépenses
    query.exec("SELECT SUM(montant) FROM TRANSACTION WHERE type = 'Dépense'");
    if (query.next()) {
        totalDepenses = query.value(0).toDouble();
    }

    profit = totalRevenus - totalDepenses;

    // Mise à jour des labels
    ui->labelTotalRevenus->setText(QString("Total Revenus : %1 DT").arg(totalRevenus, 0, 'f', 2));
    ui->labelTotalDepenses->setText(QString("Total Dépenses : %1 DT").arg(totalDepenses, 0, 'f', 2));
    ui->labelProfit->setText(QString("Profit : %1 DT").arg(profit, 0, 'f', 2));

    // Couleur profit
    if (profit >= 0) {
        ui->labelProfit->setStyleSheet("color: green; font-weight: bold; font-size: 16pt;");
    } else {
        ui->labelProfit->setStyleSheet("color: red; font-weight: bold; font-size: 16pt;");
    }

    afficherGraphiques();
}

void MainWindow::configurerGraphiques()
{
    // Graphique circulaire revenus
    chartRevenus = new QChart();
    chartRevenus->setTitle("Répartition des Revenus");
    chartRevenus->setAnimationOptions(QChart::SeriesAnimations);

    chartViewRevenus = new QChartView(chartRevenus);
    chartViewRevenus->setRenderHint(QPainter::Antialiasing);

    // Graphique circulaire dépenses
    chartDepenses = new QChart();
    chartDepenses->setTitle("Répartition des Dépenses");
    chartDepenses->setAnimationOptions(QChart::SeriesAnimations);

    chartViewDepenses = new QChartView(chartDepenses);
    chartViewDepenses->setRenderHint(QPainter::Antialiasing);

    // Ajouter aux layouts (si vous avez des layouts dans l'UI)
    if (ui->layoutGraphiqueRevenus) {
        ui->layoutGraphiqueRevenus->addWidget(chartViewRevenus);
    }
    if (ui->layoutGraphiqueDepenses) {
        ui->layoutGraphiqueDepenses->addWidget(chartViewDepenses);
    }
}

/**
 * Affiche les graphiques circulaires
 * - Répartition des revenus par catégorie
 * - Répartition des dépenses par catégorie
 * - Étiquettes : "Catégorie: Montant DT"
 */
void MainWindow::afficherGraphiques()
{
    // Graphique Revenus
    QPieSeries *seriesRevenus = new QPieSeries();
    QSqlQuery queryRevenus("SELECT categorie, SUM(montant) FROM TRANSACTION WHERE type = 'Revenu' GROUP BY categorie");

    while (queryRevenus.next()) {
        QString cat = queryRevenus.value(0).toString();
        double montant = queryRevenus.value(1).toDouble();
        QPieSlice *slice = seriesRevenus->append(cat, montant);
        slice->setLabel(QString("%1: %2 DT").arg(cat).arg(montant, 0, 'f', 2));
    }

    chartRevenus->removeAllSeries();
    chartRevenus->addSeries(seriesRevenus);

    // Graphique Dépenses
    QPieSeries *seriesDepenses = new QPieSeries();
    QSqlQuery queryDepenses("SELECT categorie, SUM(montant) FROM TRANSACTION WHERE type = 'Dépense' GROUP BY categorie");

    while (queryDepenses.next()) {
        QString cat = queryDepenses.value(0).toString();
        double montant = queryDepenses.value(1).toDouble();
        QPieSlice *slice = seriesDepenses->append(cat, montant);
        slice->setLabel(QString("%1: %2 DT").arg(cat).arg(montant, 0, 'f', 2));
    }

    chartDepenses->removeAllSeries();
    chartDepenses->addSeries(seriesDepenses);
}

// ====================== EXPORT ======================

/**
 * Exporte un rapport PDF contenant:
 * - Titre et statistiques (revenus, dépenses, profit)
 * - Tableau de toutes les transactions
 * @see QPrinter
 */
void MainWindow::exporterPDF()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Exporter en PDF", "", "PDF Files (*.pdf)");
    if (fileName.isEmpty()) return;

    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(fileName);

    QTextDocument doc;
    QString html = "<h1>Rapport Financier - PressIQ</h1>";
    html += "<h2>Statistiques</h2>";
    html += QString("<p><b>Total Revenus:</b> %1 DT</p>").arg(totalRevenus, 0, 'f', 2);
    html += QString("<p><b>Total Dépenses:</b> %1 DT</p>").arg(totalDepenses, 0, 'f', 2);
    html += QString("<p><b>Profit:</b> %1 DT</p>").arg(profit, 0, 'f', 2);

    html += "<h2>Transactions</h2>";
    html += "<table border='1' cellpadding='5'><tr><th>Type</th><th>Catégorie</th><th>Montant</th><th>Date</th></tr>";

    QSqlQuery query("SELECT type, categorie, montant, date_trans FROM TRANSACTION ORDER BY date_trans DESC");
    while (query.next()) {
        html += "<tr>";
        html += "<td>" + query.value(0).toString() + "</td>";
        html += "<td>" + query.value(1).toString() + "</td>";
        html += "<td>" + QString::number(query.value(2).toDouble(), 'f', 2) + " DT</td>";
        html += "<td>" + query.value(3).toString() + "</td>";
        html += "</tr>";
    }
    html += "</table>";

    doc.setHtml(html);
    doc.print(&printer);

    afficherMessage("Succès", "Rapport PDF exporté avec succès !");
}

/**
 * Exporte les données en CSV (compatible Excel)
 * Colonnes: ID, Type, Catégorie, Montant, Date, Description, Litres, Coût/Litre
 * Séparateur: point-virgule (;)
 */
void MainWindow::exporterExcel()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Exporter en CSV (Excel)", "", "CSV Files (*.csv)");
    if (fileName.isEmpty()) return;

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        afficherMessage("Erreur", "Impossible de créer le fichier.");
        return;
    }

    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);

    // En-têtes
    out << "ID;Type;Catégorie;Montant;Date;Description;Litres;Coût/Litre\n";

    // Données
    QSqlQuery query("SELECT * FROM TRANSACTION ORDER BY date_trans DESC");
    while (query.next()) {
        out << query.value(0).toString() << ";";
        out << query.value(1).toString() << ";";
        out << query.value(2).toString() << ";";
        out << query.value(3).toString() << ";";
        out << query.value(4).toString() << ";";
        out << query.value(5).toString() << ";";
        out << query.value(6).toString() << ";";
        out << query.value(7).toString() << "\n";
    }

    file.close();
    afficherMessage("Succès", "Données exportées en CSV avec succès !");
}

/**
 * Exporte un rapport TXT formaté
 * Contient: statistiques et liste détaillée de toutes les transactions
 */
void MainWindow::exporterTXT()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Exporter en TXT", "", "Text Files (*.txt)");
    if (fileName.isEmpty()) return;

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        afficherMessage("Erreur", "Impossible de créer le fichier.");
        return;
    }

    QTextStream out(&file);
    out << "========================================\n";
    out << "   RAPPORT FINANCIER - PRESSIQ FINANCE  \n";
    out << "========================================\n\n";

    out << QString("Total Revenus:  %1 DT\n").arg(totalRevenus, 0, 'f', 2);
    out << QString("Total Dépenses: %1 DT\n").arg(totalDepenses, 0, 'f', 2);
    out << QString("Profit:         %1 DT\n\n").arg(profit, 0, 'f', 2);

    out << "DÉTAIL DES TRANSACTIONS:\n";
    out << "----------------------------------------\n";

    QSqlQuery query("SELECT * FROM TRANSACTION ORDER BY date_trans DESC");
    while (query.next()) {
        out << QString("[%1] %2 - %3\n")
        .arg(query.value(4).toString())
            .arg(query.value(1).toString())
            .arg(query.value(2).toString());
        out << QString("    Montant: %1 DT\n").arg(query.value(3).toDouble(), 0, 'f', 2);
        out << QString("    Description: %1\n\n").arg(query.value(5).toString());
    }

    file.close();
    afficherMessage("Succès", "Rapport TXT exporté avec succès !");
}

// ====================== PRÉVISIONS ET ANALYSES ======================

/**
 * Calcule les prévisions financières basées sur les 3 derniers mois
 * - Moyenne revenus / moyenne dépenses
 * - Prévision profit / mois
 * - Extrapolation 3, 6, 12 mois
 */
void MainWindow::calculerPrevisions()
{
    // Moyenne des 3 derniers mois
    QSqlQuery query(R"(
        SELECT AVG(montant) as moyenne, type
        FROM TRANSACTION
        WHERE date_trans >= date('now', '-3 months')
        GROUP BY type
    )");

    double moyenneRevenus = 0, moyenneDepenses = 0;

    while (query.next()) {
        if (query.value(1).toString() == "Revenu") {
            moyenneRevenus = query.value(0).toDouble();
        } else {
            moyenneDepenses = query.value(0).toDouble();
        }
    }

    double previsionProfit = moyenneRevenus - moyenneDepenses;

    QString resultat = QString(
                           "=== PRÉVISION FINANCIÈRE (3 MOIS) ===\n\n"
                           "Moyenne Revenus:  %1 DT/mois\n"
                           "Moyenne Dépenses: %2 DT/mois\n"
                           "Profit Prévu:     %3 DT/mois\n\n"
                           "Prévision 3 mois: %4 DT\n"
                           "Prévision 6 mois: %5 DT\n"
                           "Prévision annuelle: %6 DT"
                           ).arg(moyenneRevenus, 0, 'f', 2)
                           .arg(moyenneDepenses, 0, 'f', 2)
                           .arg(previsionProfit, 0, 'f', 2)
                           .arg(previsionProfit * 3, 0, 'f', 2)
                           .arg(previsionProfit * 6, 0, 'f', 2)
                           .arg(previsionProfit * 12, 0, 'f', 2);

    ui->textResultatPrevision->setPlainText(resultat);
}

/**
 * Détecte les dépenses anormales
 * Seuil: 150% de la moyenne des dépenses
 * Affiche une alerte (⚠) pour chaque dépense > seuil
 */
void MainWindow::detecterDepensesAnormales()
{
    // Détection des dépenses > 150% de la moyenne
    QSqlQuery queryMoyenne("SELECT AVG(montant) FROM TRANSACTION WHERE type = 'Dépense'");
    double moyenne = 0;
    if (queryMoyenne.next()) {
        moyenne = queryMoyenne.value(0).toDouble();
    }

    double seuil = moyenne * 1.5;

    QSqlQuery query;
    query.prepare("SELECT * FROM TRANSACTION WHERE type = 'Dépense' AND montant > ? ORDER BY montant DESC");
    query.addBindValue(seuil);

    QString resultat = QString("=== DÉPENSES ANORMALES ===\n\n");
    resultat += QString("Moyenne dépenses: %1 DT\n").arg(moyenne, 0, 'f', 2);
    resultat += QString("Seuil d'alerte (150%%): %1 DT\n\n").arg(seuil, 0, 'f', 2);

    bool found = false;
    if (query.exec()) {
        while (query.next()) {
            found = true;
            resultat += QString("⚠ %1 - %2 DT - %3\n")
                            .arg(query.value("date_trans").toString())
                            .arg(query.value("montant").toDouble(), 0, 'f', 2)
                            .arg(query.value("categorie").toString());
        }
    }

    if (!found) {
        resultat += "Aucune dépense anormale détectée.";
    }

    ui->textResultatPrevision->setPlainText(resultat);
}

/**
 * Analyse le coût par litre de production
 * Affiche toutes les transactions avec litres > 0
 * Calcule le coût moyen par litre sur toutes les transactions
 * 
 * MÉTIER AVANCÉ : Aide à l'optimisation des coûts de production
 */
void MainWindow::analyserCoutParLitre()
{
    QSqlQuery query(R"(
        SELECT date_trans, categorie, montant, litres_huile, cout_par_litre
        FROM TRANSACTION
        WHERE litres_huile > 0
        ORDER BY date_trans DESC
    )");

    QString resultat = "=== ANALYSE COÛT PAR LITRE ===\n\n";

    double sommeCouts = 0;
    int count = 0;

    while (query.next()) {
        resultat += QString("%1 | %2 | %3 L → %4 DT/L\n")
                        .arg(query.value(0).toString())
                        .arg(query.value(1).toString())
                        .arg(query.value(3).toDouble(), 0, 'f', 2)
                        .arg(query.value(4).toDouble(), 0, 'f', 3);

        sommeCouts += query.value(4).toDouble();
        count++;
    }

    if (count > 0) {
        resultat += QString("\n\nCoût moyen: %1 DT/L").arg(sommeCouts / count, 0, 'f', 3);
    }

    ui->textResultatPrevision->setPlainText(resultat);
}
/**
 * Valide les données du formulaire avant CRUD
 * - Montant > 0
 * - Catégorie sélectionnée
 * @return true si valide, false sinon
 */
bool MainWindow::validerFormulaire()
{
    if (ui->spinMontant->value() <= 0) {
        afficherMessage("Erreur", "Le montant doit être supérieur à 0.");
        return false;
    }

    if (ui->comboCategorie->currentText().isEmpty()) {
        afficherMessage("Erreur", "Veuillez sélectionner une catégorie.");
        return false;
    }

    return true;
}

/**
 * Affiche un message d'information à l'utilisateur
 * @param titre Titre de la boîte de dialogue
 * @param message Texte du message
 */
void MainWindow::afficherMessage(const QString &titre, const QString &message)
{
    QMessageBox::information(this, titre, message);
}
