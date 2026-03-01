#include "stock.h"
#include "ui_stock.h"
#include <QDate>
#include <QPixmap>
#include <QTime>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QFileDialog>
#include <QAbstractSpinBox>
#include <QHeaderView>
#include <QScrollArea>
#include <QScrollBar>
#include <QVBoxLayout>
#include <QTabWidget>
#include <QFrame>

Stock::Stock(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Stock)
    , dateTimeTimer(nullptr)
    , connexion(nullptr)
{
    ui->setupUi(this);
    {
        QPixmap logo(":/logo.png");
        if (!logo.isNull())
            ui->labelLogoImage->setPixmap(logo.scaled(50, 50, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }

    // Obtenir la connexion
    connexion = Connexion::getInstance();
    
    // Vérifier la connexion
    if (!connexion->isOpen()) {
        connexion->createConnection();
    }
    
    // Initialiser l'interface
    initialiserInterface();
    
    // Activer le scrolling sur tous les onglets
    activerScrolling();
    
    // Charger les données
    chargerDonneesTable();
    
    // Connect back button
    connect(ui->btnBackToMenu, &QPushButton::clicked, this, &Stock::onBackClicked);
    
    // Connect CRUD buttons
    connect(ui->btnAjouter, &QPushButton::clicked, this, &Stock::onAjouterClicked);
    connect(ui->btnModifier, &QPushButton::clicked, this, &Stock::onModifierClicked);
    connect(ui->btnSupprimer, &QPushButton::clicked, this, &Stock::onSupprimerClicked);
    connect(ui->btnConsulter, &QPushButton::clicked, this, &Stock::onConsulterClicked);
    
    // Connect recherche buttons
    connect(ui->btnRechercher, &QPushButton::clicked, this, &Stock::onRechercherClicked);
    connect(ui->btnReinitialiser, &QPushButton::clicked, this, &Stock::onReinitialiserClicked);
    
    // Connect tri button
    connect(ui->btnAppliquerTri, &QPushButton::clicked, this, &Stock::onAppliquerTriClicked);
    
    // Connect export button
    connect(ui->btnExporter, &QPushButton::clicked, this, &Stock::onExporterClicked);
    
    // Connect statistiques button
    connect(ui->btnCalculer, &QPushButton::clicked, this, &Stock::onCalculerClicked);
    
    // Connect alertes buttons
    connect(ui->btnVerifierAlertes, &QPushButton::clicked, this, &Stock::onVerifierAlertesClicked);
    connect(ui->btnAfficherHistorique, &QPushButton::clicked, this, &Stock::onAfficherHistoriqueClicked);
    connect(ui->btnAnalyserTendances, &QPushButton::clicked, this, &Stock::onAnalyserTendancesClicked);
    
    // Connect rapport buttons
    connect(ui->btnGenererRapport, &QPushButton::clicked, this, &Stock::onGenererRapportClicked);
    connect(ui->btnExportRapportPDF, &QPushButton::clicked, this, &Stock::onExportRapportPDFClicked);
    
    // Connect synchronisation button
    connect(ui->btnSynchroniser, &QPushButton::clicked, this, &Stock::onSynchroniserClicked);
    
    // Connect table selection
    connect(ui->tableStocks, &QTableWidget::itemSelectionChanged, this, &Stock::onTableStockSelectionChanged);
    
    // Setup date/time timer
    dateTimeTimer = new QTimer(this);
    connect(dateTimeTimer, &QTimer::timeout, this, &Stock::updateDateTime);
    dateTimeTimer->start(1000);
    updateDateTime();
}

Stock::~Stock()
{
    if (dateTimeTimer) {
        dateTimeTimer->stop();
        delete dateTimeTimer;
    }
    delete ui;
}

void Stock::initialiserInterface()
{
    // Configurer la date actuelle
    ui->dateEditMaj->setDate(QDate::currentDate());
    
    // Remplir les combobox
    ui->comboTypeHuile->clear();
    ui->comboTypeHuile->addItem("Huile d'olive extra vierge");
    ui->comboTypeHuile->addItem("Huile d'olive vierge");
    ui->comboTypeHuile->addItem("Huile d'arachide");
    ui->comboTypeHuile->addItem("Huile de tournesol");
    ui->comboTypeHuile->addItem("Huile de colza");
    ui->comboTypeHuile->addItem("Huile de sésame");
    ui->comboTypeHuile->addItem("Huile de noix");
    ui->comboTypeHuile->addItem("Huile de mais");
    
    ui->comboEtatStock->clear();
    ui->comboEtatStock->addItem("Disponible");
    ui->comboEtatStock->addItem("Faible");
    ui->comboEtatStock->addItem("Rupture");
    ui->comboEtatStock->addItem("En commande");
    
    // Combobox de recherche
    ui->comboRechercheType->clear();
    ui->comboRechercheType->addItem("Tous");
    ui->comboRechercheType->addItems({"Huile d'olive extra vierge", "Huile d'olive vierge", 
                                       "Huile d'arachide", "Huile de tournesol", 
                                       "Huile de colza", "Huile de sésame"});
    
    ui->comboRechercheEtat->clear();
    ui->comboRechercheEtat->addItem("Tous");
    ui->comboRechercheEtat->addItems({"Disponible", "Faible", "Rupture", "En commande"});
    
    // Combobox de tri
    ui->comboTriCritere->clear();
    ui->comboTriCritere->addItem("ID Stock");
    ui->comboTriCritere->addItem("Type d'huile");
    ui->comboTriCritere->addItem("Quantité actuelle");
    ui->comboTriCritere->addItem("Seuil d'alerte");
    ui->comboTriCritere->addItem("Date de mise à jour");
    
    // Combobox de statistiques
    ui->comboStatsPar->clear();
    ui->comboStatsPar->addItem("Type d'huile");
    ui->comboStatsPar->addItem("État du stock");
    ui->comboStatsPar->addItem("Emplacement");
    
    // Combobox de période
    ui->comboPeriode->clear();
    ui->comboPeriode->addItem("7 derniers jours");
    ui->comboPeriode->addItem("30 derniers jours");
    ui->comboPeriode->addItem("90 derniers jours");
    
    // Configurer la table
    configurerTableStocks();
    
    // Configuration des spin boxes
    ui->spinQuantite->setRange(0, 100000.00);
    ui->spinQuantite->setDecimals(2);
    ui->spinQuantite->setSingleStep(1.0);
    ui->spinQuantite->setValue(0);
    
    ui->spinSeuilAlerte->setRange(0, 10000);
    ui->spinSeuilAlerte->setSingleStep(1);
    ui->spinSeuilAlerte->setValue(50);
    
    // Activer les boutons des spinbox
    ui->spinQuantite->setButtonSymbols(QAbstractSpinBox::UpDownArrows);
    ui->spinSeuilAlerte->setButtonSymbols(QAbstractSpinBox::UpDownArrows);
    ui->spinQuantite->setEnabled(true);
    ui->spinSeuilAlerte->setEnabled(true);
    
    // Connecter les changements de valeur pour mettre à jour l'état automatiquement
    connect(ui->spinQuantite, QOverload<double>::of(&QDoubleSpinBox::valueChanged), 
            [this]() {
        double quantite = ui->spinQuantite->value();
        double seuil = ui->spinSeuilAlerte->value();
        QString etat = getEtatStock(quantite, seuil);
        ui->comboEtatStock->setCurrentText(etat);
    });
    
    connect(ui->spinSeuilAlerte, QOverload<int>::of(&QSpinBox::valueChanged), 
            [this]() {
        double quantite = ui->spinQuantite->value();
        double seuil = ui->spinSeuilAlerte->value();
        QString etat = getEtatStock(quantite, seuil);
        ui->comboEtatStock->setCurrentText(etat);
    });
    
    // ID en lecture seule
    ui->lineEditIdStock->setReadOnly(true);
    ui->lineEditIdStock->setPlaceholderText("Auto-généré");
}

void Stock::configurerTableStocks()
{
    ui->tableStocks->setColumnCount(7);
    ui->tableStocks->setHorizontalHeaderLabels({"ID", "Type d'huile", "Quantité (L)", 
                                                  "Seuil (L)", "Dernière MAJ", 
                                                  "Emplacement", "État"});
    
    ui->tableStocks->horizontalHeader()->setStretchLastSection(true);
    ui->tableStocks->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableStocks->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableStocks->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void Stock::activerScrolling()
{
    // Obtenir le QTabWidget principal
    QTabWidget* tabWidget = ui->tabWidgetStock;
    if (!tabWidget) {
        qDebug() << "TabWidget non trouvé";
        return;
    }
    
    // Parcourir tous les onglets
    for (int i = 0; i < tabWidget->count(); ++i) {
        QWidget* tab = tabWidget->widget(i);
        
        // Vérifier si l'onglet n'a pas déjà de QScrollArea
        QScrollArea* existingScrollArea = tab->findChild<QScrollArea*>();
        if (existingScrollArea) {
            // Configurer la scrollarea existante
            existingScrollArea->setWidgetResizable(true);
            existingScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
            existingScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
            continue;
        }
        
        // Pour les onglets sans scrollarea, en ajouter une
        QLayout* oldLayout = tab->layout();
        if (!oldLayout) continue;
        
        // Créer une nouvelle scrollarea
        QScrollArea* scrollArea = new QScrollArea(tab);
        scrollArea->setWidgetResizable(true);
        scrollArea->setFrameShape(QFrame::NoFrame);
        scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        
        // Créer un widget conteneur
        QWidget* scrollContent = new QWidget();
        
        // Obtenir tous les widgets enfants
        QList<QWidget*> children;
        for (QObject* child : tab->children()) {
            if (QWidget* widget = qobject_cast<QWidget*>(child)) {
                if (widget != scrollArea && widget->parent() == tab) {
                    children.append(widget);
                }
            }
        }
        
        // Créer une nouvelle layout pour le contenu
        QVBoxLayout* contentLayout = new QVBoxLayout(scrollContent);
        contentLayout->setContentsMargins(10, 10, 10, 10);
        
        // Déplacer les widgets dans le nouveau conteneur
        for (QWidget* child : children) {
            child->setParent(scrollContent);
            contentLayout->addWidget(child);
        }
        
        scrollArea->setWidget(scrollContent);
        
        // Supprimer l'ancienne layout et en créer une nouvelle
        delete oldLayout;
        QVBoxLayout* newLayout = new QVBoxLayout(tab);
        newLayout->setContentsMargins(0, 0, 0, 0);
        newLayout->addWidget(scrollArea);
        
        qDebug() << "Scrolling activé pour l'onglet:" << tabWidget->tabText(i);
    }
    
    qDebug() << "✓ Scrolling activé sur tous les onglets";
}

void Stock::onBackClicked()
{
    emit backToMenu();
    this->hide();
}

void Stock::chargerDonneesTable()
{
    if (!connexion || !connexion->isOpen()) {
        afficherMessage("Erreur", "Pas de connexion à la base de données", true);
        return;
    }
    
    QSqlQuery query(connexion->getDatabase());
    query.exec("SELECT ID_STOCK, TYPE_HUILE, QUANTITE_ACTUELLE, SEUIL_ALERTE, "
               "DATE_DERNIERE_MISE_A_JOUR, EMPLACEMENT_STOCKAGE, ETAT_STOCK "
               "FROM STOCK ORDER BY ID_STOCK");
    
    ui->tableStocks->setRowCount(0);
    
    int row = 0;
    while (query.next()) {
        ui->tableStocks->insertRow(row);
        
        ui->tableStocks->setItem(row, 0, new QTableWidgetItem(query.value(0).toString()));
        ui->tableStocks->setItem(row, 1, new QTableWidgetItem(query.value(1).toString()));
        ui->tableStocks->setItem(row, 2, new QTableWidgetItem(QString::number(query.value(2).toDouble(), 'f', 2)));
        ui->tableStocks->setItem(row, 3, new QTableWidgetItem(query.value(3).toString()));
        ui->tableStocks->setItem(row, 4, new QTableWidgetItem(query.value(4).toDate().toString("dd/MM/yyyy")));
        ui->tableStocks->setItem(row, 5, new QTableWidgetItem(query.value(5).toString()));
        ui->tableStocks->setItem(row, 6, new QTableWidgetItem(query.value(6).toString()));
        
        row++;
    }
    
    qDebug() << "Données chargées:" << row << "lignes";
}

void Stock::viderChamps()
{
    ui->lineEditIdStock->clear();
    ui->comboTypeHuile->setCurrentIndex(0);
    ui->spinQuantite->setValue(0);
    ui->spinSeuilAlerte->setValue(0);
    ui->dateEditMaj->setDate(QDate::currentDate());
    ui->lineEditEmplacement->clear();
    ui->comboEtatStock->setCurrentIndex(0);
}

void Stock::remplirChamps(int row)
{
    if (row < 0 || row >= ui->tableStocks->rowCount()) return;
    
    ui->lineEditIdStock->setText(ui->tableStocks->item(row, 0)->text());
    ui->comboTypeHuile->setCurrentText(ui->tableStocks->item(row, 1)->text());
    ui->spinQuantite->setValue(ui->tableStocks->item(row, 2)->text().toDouble());
    ui->spinSeuilAlerte->setValue(ui->tableStocks->item(row, 3)->text().toInt());
    ui->dateEditMaj->setDate(QDate::fromString(ui->tableStocks->item(row, 4)->text(), "dd/MM/yyyy"));
    ui->lineEditEmplacement->setText(ui->tableStocks->item(row, 5)->text());
    ui->comboEtatStock->setCurrentText(ui->tableStocks->item(row, 6)->text());
}

bool Stock::validerChamps()
{
    if (ui->comboTypeHuile->currentText().isEmpty()) {
        afficherMessage("Validation", "Veuillez sélectionner un type d'huile", true);
        return false;
    }
    
    if (ui->lineEditEmplacement->text().trimmed().isEmpty()) {
        afficherMessage("Validation", "Veuillez entrer un emplacement", true);
        return false;
    }
    
    return true;
}

void Stock::afficherMessage(const QString& titre, const QString& message, bool isError)
{
    if (isError) {
        QMessageBox::critical(this, titre, message);
    } else {
        QMessageBox::information(this, titre, message);
    }
}

QString Stock::getEtatStock(double quantite, double seuil)
{
    if (quantite == 0) return "Rupture";
    if (quantite < seuil) return "Faible";
    return "Disponible";
}

// ==================== CRUD OPERATIONS ====================

void Stock::onAjouterClicked()
{
    if (!validerChamps()) return;
    
    if (!connexion || !connexion->isOpen()) {
        afficherMessage("Erreur", "Pas de connexion à la base de données", true);
        return;
    }
    
    QSqlQuery query(connexion->getDatabase());
    query.prepare("INSERT INTO STOCK (TYPE_HUILE, QUANTITE_ACTUELLE, SEUIL_ALERTE, "
                  "DATE_DERNIERE_MISE_A_JOUR, EMPLACEMENT_STOCKAGE, ETAT_STOCK) "
                  "VALUES (:type, :quantite, :seuil, :date, :emplacement, :etat)");
    
    query.bindValue(":type", ui->comboTypeHuile->currentText());
    query.bindValue(":quantite", ui->spinQuantite->value());
    query.bindValue(":seuil", ui->spinSeuilAlerte->value());
    query.bindValue(":date", ui->dateEditMaj->date());
    query.bindValue(":emplacement", ui->lineEditEmplacement->text());
    query.bindValue(":etat", ui->comboEtatStock->currentText());
    
    if (query.exec()) {
        afficherMessage("Succès", "Article ajouté avec succès !");
        chargerDonneesTable();
        viderChamps();
    } else {
        afficherMessage("Erreur", "Erreur lors de l'ajout : " + query.lastError().text(), true);
    }
}

void Stock::onModifierClicked()
{
    if (!validerChamps()) return;
    
    QString id = ui->lineEditIdStock->text();
    if (id.isEmpty()) {
        afficherMessage("Attention", "Veuillez sélectionner un article à modifier", true);
        return;
    }
    
    if (!connexion || !connexion->isOpen()) {
        afficherMessage("Erreur", "Pas de connexion à la base de données", true);
        return;
    }
    
    QSqlQuery query(connexion->getDatabase());
    query.prepare("UPDATE STOCK SET TYPE_HUILE = :type, QUANTITE_ACTUELLE = :quantite, "
                  "SEUIL_ALERTE = :seuil, DATE_DERNIERE_MISE_A_JOUR = :date, "
                  "EMPLACEMENT_STOCKAGE = :emplacement, ETAT_STOCK = :etat "
                  "WHERE ID_STOCK = :id");
    
    query.bindValue(":type", ui->comboTypeHuile->currentText());
    query.bindValue(":quantite", ui->spinQuantite->value());
    query.bindValue(":seuil", ui->spinSeuilAlerte->value());
    query.bindValue(":date", ui->dateEditMaj->date());
    query.bindValue(":emplacement", ui->lineEditEmplacement->text());
    query.bindValue(":etat", ui->comboEtatStock->currentText());
    query.bindValue(":id", id);
    
    if (query.exec()) {
        afficherMessage("Succès", "Article modifié avec succès !");
        chargerDonneesTable();
        viderChamps();
    } else {
        afficherMessage("Erreur", "Erreur lors de la modification : " + query.lastError().text(), true);
    }
}

void Stock::onSupprimerClicked()
{
    QString id = ui->lineEditIdStock->text();
    if (id.isEmpty()) {
        afficherMessage("Attention", "Veuillez sélectionner un article à supprimer", true);
        return;
    }
    
    QMessageBox::StandardButton reply = QMessageBox::question(this, 
        "Confirmation", 
        "Êtes-vous sûr de vouloir supprimer cet article ?",
        QMessageBox::Yes | QMessageBox::No);
    
    if (reply == QMessageBox::No) return;
    
    if (!connexion || !connexion->isOpen()) {
        afficherMessage("Erreur", "Pas de connexion à la base de données", true);
        return;
    }
    
    QSqlQuery query(connexion->getDatabase());
    query.prepare("DELETE FROM STOCK WHERE ID_STOCK = :id");
    query.bindValue(":id", id);
    
    if (query.exec()) {
        afficherMessage("Succès", "Article supprimé avec succès !");
        chargerDonneesTable();
        viderChamps();
    } else {
        afficherMessage("Erreur", "Erreur lors de la suppression : " + query.lastError().text(), true);
    }
}

void Stock::onConsulterClicked()
{
    chargerDonneesTable();
    afficherMessage("Rafraîchissement", "Données actualisées !");
}

// ==================== RECHERCHE ET FILTRES ====================

void Stock::onRechercherClicked()
{
    if (!connexion || !connexion->isOpen()) {
        afficherMessage("Erreur", "Pas de connexion à la base de données", true);
        return;
    }
    
    QString typeRecherche = ui->comboRechercheType->currentText();
    QString etatRecherche = ui->comboRechercheEtat->currentText();
    
    QString sql = "SELECT ID_STOCK, TYPE_HUILE, QUANTITE_ACTUELLE, SEUIL_ALERTE, "
                  "DATE_DERNIERE_MISE_A_JOUR, EMPLACEMENT_STOCKAGE, ETAT_STOCK "
                  "FROM STOCK WHERE 1=1";
    
    if (typeRecherche != "Tous") {
        sql += " AND TYPE_HUILE = :type";
    }
    
    if (etatRecherche != "Tous") {
        sql += " AND ETAT_STOCK = :etat";
    }
    
    sql += " ORDER BY ID_STOCK";
    
    QSqlQuery query(connexion->getDatabase());
    query.prepare(sql);
    
    if (typeRecherche != "Tous") {
        query.bindValue(":type", typeRecherche);
    }
    if (etatRecherche != "Tous") {
        query.bindValue(":etat", etatRecherche);
    }
    
    if (!query.exec()) {
        afficherMessage("Erreur", "Erreur de recherche : " + query.lastError().text(), true);
        return;
    }
    
    // Afficher dans la table des résultats de l'onglet Tri/Recherche
    ui->tableResultatsCombined->clear();
    ui->tableResultatsCombined->setColumnCount(7);
    ui->tableResultatsCombined->setHorizontalHeaderLabels({"ID", "Type d'huile", "Quantité (L)", 
                                                            "Seuil (L)", "Date MAJ", "Emplacement", "État"});
    ui->tableResultatsCombined->setRowCount(0);
    
    int row = 0;
    while (query.next()) {
        ui->tableResultatsCombined->insertRow(row);
        ui->tableResultatsCombined->setItem(row, 0, new QTableWidgetItem(query.value(0).toString()));
        ui->tableResultatsCombined->setItem(row, 1, new QTableWidgetItem(query.value(1).toString()));
        ui->tableResultatsCombined->setItem(row, 2, new QTableWidgetItem(QString::number(query.value(2).toDouble(), 'f', 2)));
        ui->tableResultatsCombined->setItem(row, 3, new QTableWidgetItem(query.value(3).toString()));
        ui->tableResultatsCombined->setItem(row, 4, new QTableWidgetItem(query.value(4).toDate().toString("dd/MM/yyyy")));
        ui->tableResultatsCombined->setItem(row, 5, new QTableWidgetItem(query.value(5).toString()));
        ui->tableResultatsCombined->setItem(row, 6, new QTableWidgetItem(query.value(6).toString()));
        row++;
    }
    
    // Configurer l'apparence de la table
    ui->tableResultatsCombined->horizontalHeader()->setStretchLastSection(true);
    ui->tableResultatsCombined->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableResultatsCombined->setEditTriggers(QAbstractItemView::NoEditTriggers);
    
    afficherMessage("Recherche", QString("✅ %1 résultat(s) trouvé(s) !").arg(row));
}

void Stock::onReinitialiserClicked()
{
    ui->comboRechercheType->setCurrentIndex(0);
    ui->comboRechercheEtat->setCurrentIndex(0);
    ui->tableResultatsCombined->clear();
    ui->tableResultatsCombined->setRowCount(0);
    afficherMessage("Réinitialisation", "Filtres réinitialisés !");
}

// ==================== TRI ====================

void Stock::onAppliquerTriClicked()
{
    if (!connexion || !connexion->isOpen()) {
        afficherMessage("Erreur", "Pas de connexion à la base de données", true);
        return;
    }
    
    QString critere = ui->comboTriCritere->currentText();
    QString colonne = "ID_STOCK";
    
    if (critere == "Type d'huile") colonne = "TYPE_HUILE";
    else if (critere == "Quantité actuelle") colonne = "QUANTITE_ACTUELLE";
    else if (critere == "Seuil d'alerte") colonne = "SEUIL_ALERTE";
    else if (critere == "Date de mise à jour") colonne = "DATE_DERNIERE_MISE_A_JOUR";
    
    QString sql = QString("SELECT ID_STOCK, TYPE_HUILE, QUANTITE_ACTUELLE, SEUIL_ALERTE, "
                         "DATE_DERNIERE_MISE_A_JOUR, EMPLACEMENT_STOCKAGE, ETAT_STOCK "
                         "FROM STOCK ORDER BY %1").arg(colonne);
    
    QSqlQuery query(connexion->getDatabase());
    if (!query.exec(sql)) {
        afficherMessage("Erreur", "Erreur de tri : " + query.lastError().text(), true);
        return;
    }
    
    // Afficher dans la table des résultats
    ui->tableResultatsCombined->clear();
    ui->tableResultatsCombined->setColumnCount(7);
    ui->tableResultatsCombined->setHorizontalHeaderLabels({"ID", "Type d'huile", "Quantité (L)", 
                                                            "Seuil (L)", "Date MAJ", "Emplacement", "État"});
    ui->tableResultatsCombined->setRowCount(0);
    
    int row = 0;
    while (query.next()) {
        ui->tableResultatsCombined->insertRow(row);
        ui->tableResultatsCombined->setItem(row, 0, new QTableWidgetItem(query.value(0).toString()));
        ui->tableResultatsCombined->setItem(row, 1, new QTableWidgetItem(query.value(1).toString()));
        ui->tableResultatsCombined->setItem(row, 2, new QTableWidgetItem(QString::number(query.value(2).toDouble(), 'f', 2)));
        ui->tableResultatsCombined->setItem(row, 3, new QTableWidgetItem(query.value(3).toString()));
        ui->tableResultatsCombined->setItem(row, 4, new QTableWidgetItem(query.value(4).toDate().toString("dd/MM/yyyy")));
        ui->tableResultatsCombined->setItem(row, 5, new QTableWidgetItem(query.value(5).toString()));
        ui->tableResultatsCombined->setItem(row, 6, new QTableWidgetItem(query.value(6).toString()));
        row++;
    }
    
    // Configurer l'apparence de la table
    ui->tableResultatsCombined->horizontalHeader()->setStretchLastSection(true);
    ui->tableResultatsCombined->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableResultatsCombined->setEditTriggers(QAbstractItemView::NoEditTriggers);
    
    afficherMessage("Tri", QString("✅ Données triées par %1 - %2 résultat(s)").arg(critere).arg(row));
}

// ==================== EXPORT ====================

void Stock::onExporterClicked()
{
    // Vérifier qu'il y a des données à exporter
    if (ui->tableResultatsCombined->rowCount() == 0) {
        afficherMessage("Export", "⚠️  Aucune donnée à exporter !\n\n"
                                   "Veuillez d'abord faire une recherche ou un tri.", true);
        return;
    }
    
    QString fileName = QFileDialog::getSaveFileName(this, "Exporter les résultats", 
                                                     "", "Fichiers CSV (*.csv)");
    
    if (fileName.isEmpty()) return;
    
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        afficherMessage("Erreur", "Impossible d'ouvrir le fichier pour l'export", true);
        return;
    }
    
    QTextStream out(&file);
    // UTF-8 par défaut dans Qt moderne
    
    // En-têtes
    out << "ID,Type d'huile,Quantité (L),Seuil (L),Date MAJ,Emplacement,État\n";
    
    // Données
    for (int i = 0; i < ui->tableResultatsCombined->rowCount(); ++i) {
        for (int j = 0; j < ui->tableResultatsCombined->columnCount(); ++j) {
            QTableWidgetItem* item = ui->tableResultatsCombined->item(i, j);
            if (item) {
                QString text = item->text();
                // Gérer les virgules dans le texte
                if (text.contains(",") || text.contains("\"")) {
                    text = "\"" + text.replace("\"", "\"\"") + "\"";
                }
                out << text;
            }
            if (j < ui->tableResultatsCombined->columnCount() - 1) out << ",";
        }
        out << "\n";
    }
    
    file.close();
    afficherMessage("Succès", QString("✅ Export réussi !\n\n"
                                      "%1 ligne(s) exportée(s) vers :\n%2")
                                      .arg(ui->tableResultatsCombined->rowCount())
                                      .arg(fileName));
}

// ==================== STATISTIQUES ====================

void Stock::onCalculerClicked()
{
    if (!connexion || !connexion->isOpen()) return;
    
    QString critere = ui->comboStatsPar->currentText();
    QString colonne = "TYPE_HUILE";
    
    if (critere == "État du stock") colonne = "ETAT_STOCK";
    else if (critere == "Emplacement") colonne = "EMPLACEMENT_STOCKAGE";
    
    QSqlQuery query(connexion->getDatabase());
    QString sql = QString("SELECT %1, COUNT(*) as NB, SUM(QUANTITE_ACTUELLE) as TOTAL "
                         "FROM STOCK GROUP BY %1 ORDER BY NB DESC").arg(colonne);
    
    query.exec(sql);
    
    ui->tableResultatsCombined->clear();
    ui->tableResultatsCombined->setColumnCount(3);
    ui->tableResultatsCombined->setHorizontalHeaderLabels({critere, "Nombre", "Total (L)"});
    ui->tableResultatsCombined->setRowCount(0);
    
    int row = 0;
    while (query.next()) {
        ui->tableResultatsCombined->insertRow(row);
        ui->tableResultatsCombined->setItem(row, 0, new QTableWidgetItem(query.value(0).toString()));
        ui->tableResultatsCombined->setItem(row, 1, new QTableWidgetItem(query.value(1).toString()));
        ui->tableResultatsCombined->setItem(row, 2, new QTableWidgetItem(QString::number(query.value(2).toDouble(), 'f', 2)));
        row++;
    }
    
    afficherMessage("Statistiques", "Statistiques calculées avec succès !");
}

// ==================== ALERTES ====================

void Stock::onVerifierAlertesClicked()
{
    if (!connexion || !connexion->isOpen()) return;
    
    QSqlQuery query(connexion->getDatabase());
    query.exec("SELECT TYPE_HUILE, QUANTITE_ACTUELLE, SEUIL_ALERTE, EMPLACEMENT_STOCKAGE "
               "FROM STOCK WHERE QUANTITE_ACTUELLE < SEUIL_ALERTE OR QUANTITE_ACTUELLE = 0 "
               "ORDER BY QUANTITE_ACTUELLE");
    
    QString message = "=== ALERTES STOCK ===\n\n";
    int count = 0;
    
    while (query.next()) {
        count++;
        QString type = query.value(0).toString();
        double quantite = query.value(1).toDouble();
        double seuil = query.value(2).toDouble();
        QString emplacement = query.value(3).toString();
        
        if (quantite == 0) {
            message += QString("⛔ RUPTURE : %1 (%2)\n").arg(type).arg(emplacement);
        } else {
            message += QString("⚠️  FAIBLE : %1 - %2 L (seuil: %3 L) - %4\n")
                          .arg(type).arg(quantite).arg(seuil).arg(emplacement);
        }
    }
    
    if (count == 0) {
        message = "✅ Aucune alerte ! Tous les stocks sont au-dessus des seuils.";
    } else {
        message += QString("\n📊 Total : %1 alerte(s)").arg(count);
    }
    
    QMessageBox::warning(this, "Vérification des alertes", message);
}

// ==================== HISTORIQUE ====================

void Stock::onAfficherHistoriqueClicked()
{
    chargerDonneesTable();
    
    ui->tableHistorique->clear();
    ui->tableHistorique->setColumnCount(7);
    ui->tableHistorique->setHorizontalHeaderLabels({"ID", "Type", "Quantité", "Seuil", 
                                                     "Date MAJ", "Emplacement", "État"});
    ui->tableHistorique->setRowCount(0);
    
    // Copier les données de tableStocks vers tableHistorique
    for (int i = 0; i < ui->tableStocks->rowCount(); ++i) {
        ui->tableHistorique->insertRow(i);
        for (int j = 0; j < 7; ++j) {
            ui->tableHistorique->setItem(i, j, new QTableWidgetItem(ui->tableStocks->item(i, j)->text()));
        }
    }
    
    afficherMessage("Historique", "Historique chargé avec succès !");
}

void Stock::onAnalyserTendancesClicked()
{
    if (!connexion || !connexion->isOpen()) return;
    
    QSqlQuery query(connexion->getDatabase());
    query.exec("SELECT TYPE_HUILE, AVG(QUANTITE_ACTUELLE) as MOY, "
               "MIN(QUANTITE_ACTUELLE) as MIN, MAX(QUANTITE_ACTUELLE) as MAX "
               "FROM STOCK GROUP BY TYPE_HUILE ORDER BY MOY DESC");
    
    ui->tableResultatsCombined->clear();
    ui->tableResultatsCombined->setColumnCount(4);
    ui->tableResultatsCombined->setHorizontalHeaderLabels({"Type d'huile", "Moyenne (L)", "Min (L)", "Max (L)"});
    ui->tableResultatsCombined->setRowCount(0);
    
    int row = 0;
    while (query.next()) {
        ui->tableResultatsCombined->insertRow(row);
        ui->tableResultatsCombined->setItem(row, 0, new QTableWidgetItem(query.value(0).toString()));
        ui->tableResultatsCombined->setItem(row, 1, new QTableWidgetItem(QString::number(query.value(1).toDouble(), 'f', 2)));
        ui->tableResultatsCombined->setItem(row, 2, new QTableWidgetItem(QString::number(query.value(2).toDouble(), 'f', 2)));
        ui->tableResultatsCombined->setItem(row, 3, new QTableWidgetItem(QString::number(query.value(3).toDouble(), 'f', 2)));
        row++;
    }
    
    afficherMessage("Tendances", "Analyse des tendances terminée !");
}

// ==================== RAPPORTS ====================

void Stock::onGenererRapportClicked()
{
    chargerDonneesTable();
    
    ui->tableRapportSelection->clear();
    ui->tableRapportSelection->setColumnCount(7);
    ui->tableRapportSelection->setHorizontalHeaderLabels({"ID", "Type", "Quantité", "Seuil", 
                                                           "Date MAJ", "Emplacement", "État"});
    ui->tableRapportSelection->setRowCount(0);
    
    // Copier toutes les données vers le rapport
    for (int i = 0; i < ui->tableStocks->rowCount(); ++i) {
        ui->tableRapportSelection->insertRow(i);
        for (int j = 0; j < 7; ++j) {
            ui->tableRapportSelection->setItem(i, j, new QTableWidgetItem(ui->tableStocks->item(i, j)->text()));
        }
    }
    
    afficherMessage("Rapport", "Rapport généré avec succès !");
}

void Stock::onExportRapportPDFClicked()
{
    afficherMessage("Export PDF", "Fonctionnalité d'export PDF en cours de développement.\n"
                                   "Utilisez l'export CSV pour le moment.");
}

// ==================== SYNCHRONISATION ====================

void Stock::onSynchroniserClicked()
{
    chargerDonneesTable();
    afficherMessage("Synchronisation", "Données synchronisées avec la base de données !");
}

// ==================== SELECTION TABLE ====================

void Stock::onTableStockSelectionChanged()
{
    QList<QTableWidgetItem*> items = ui->tableStocks->selectedItems();
    if (items.isEmpty()) return;
    
    int row = items.first()->row();
    remplirChamps(row);
}

// ==================== DATE/TIME UPDATE ====================

void Stock::updateDateTime()
{
    QDate currentDate = QDate::currentDate();
    QTime currentTime = QTime::currentTime();
    
    // Format date in French
    QStringList moisFr = {"", "janvier", "février", "mars", "avril", "mai", "juin",
                          "juillet", "août", "septembre", "octobre", "novembre", "décembre"};
    
    QString dateStr = QString("📅 %1 %2 %3")
                          .arg(currentDate.day(), 2, 10, QChar('0'))
                          .arg(moisFr[currentDate.month()])
                          .arg(currentDate.year());
    
    QString timeStr = currentTime.toString("🕐 HH:mm:ss");
    
    if (ui->lblDateTime) {
        ui->lblDateTime->setText(dateStr + " | " + timeStr);
    }
}
