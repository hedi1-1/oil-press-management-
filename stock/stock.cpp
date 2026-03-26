#include "stock.h"
#include "ui_stock.h"
#include <QCoreApplication>
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
#include <QTextCharFormat>
#include <QBrush>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QEasingCurve>
#include <algorithm>

Stock::Stock(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Stock)
    , dateTimeTimer(nullptr)
    , calendarRefreshTimer(nullptr)
    , connexion(nullptr)
{
    ui->setupUi(this);
    {
        QPixmap logo;
        for (const QString &p : QStringList{":/logo.png", ":/images/logo.png", "logo.png",
             "../production/logo.png", "../stock/images/logo.png",
             QCoreApplication::applicationDirPath() + "/../../logo.png",
             QCoreApplication::applicationDirPath() + "/../../../production/logo.png"}) {
            logo = QPixmap(p);
            if (!logo.isNull()) break;
        }
        if (!logo.isNull())
            ui->labelLogoImage->setPixmap(logo.scaledToHeight(50, Qt::SmoothTransformation));
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
    connect(ui->btnAnalyserPredictifHuile, &QPushButton::clicked, this, &Stock::onAnalyserPredictifHuileClicked);
    
    // Connect rapport buttons
    connect(ui->btnGenererRapport, &QPushButton::clicked, this, &Stock::onGenererRapportClicked);
    connect(ui->btnExportRapportPDF, &QPushButton::clicked, this, &Stock::onExportRapportPDFClicked);
    
    // Connect synchronisation button
    connect(ui->btnSynchroniser, &QPushButton::clicked, this, &Stock::onSynchroniserClicked);

    // Connect calendrier interactif
    connect(ui->calendarWidgetStock, &QCalendarWidget::selectionChanged, this, [this]() {
        onCalendarDateSelected(ui->calendarWidgetStock->selectedDate());
    });
    connect(ui->calendarWidgetStock, &QCalendarWidget::currentPageChanged,
            this, &Stock::onCalendarShowMonth);
    connect(ui->btnTodayCalendar, &QPushButton::clicked, this, [this]() {
        const QDate today = QDate::currentDate();
        ui->calendarWidgetStock->setSelectedDate(today);
        ui->calendarWidgetStock->showSelectedDate();
        onCalendarDateSelected(today);
    });
    
    // Connect table selection
    connect(ui->tableStocks, &QTableWidget::itemSelectionChanged, this, &Stock::onTableStockSelectionChanged);
    
    // Setup date/time timer
    dateTimeTimer = new QTimer(this);
    connect(dateTimeTimer, &QTimer::timeout, this, &Stock::updateDateTime);
    dateTimeTimer->start(1000);
    updateDateTime();

    // Actualisation automatique du calendrier
    calendarRefreshTimer = new QTimer(this);
    connect(calendarRefreshTimer, &QTimer::timeout, this, &Stock::chargerDatesStockCalendrier);
    calendarRefreshTimer->start(30000);

    initialiserCalendrier();
}

Stock::~Stock()
{
    if (dateTimeTimer) {
        dateTimeTimer->stop();
        delete dateTimeTimer;
    }
    if (calendarRefreshTimer) {
        calendarRefreshTimer->stop();
        delete calendarRefreshTimer;
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

    if (ui->comboPeriodePredictifHuile) {
        ui->comboPeriodePredictifHuile->clear();
        ui->comboPeriodePredictifHuile->addItem("7 derniers jours");
        ui->comboPeriodePredictifHuile->addItem("30 derniers jours");
        ui->comboPeriodePredictifHuile->addItem("90 derniers jours");
    }
    
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
    chargerDatesStockCalendrier();
}

void Stock::initialiserCalendrier()
{
    if (!ui->calendarWidgetStock || !ui->tableCalendarDetails) {
        return;
    }

    ui->calendarWidgetStock->setGridVisible(true);
    ui->calendarWidgetStock->setVerticalHeaderFormat(QCalendarWidget::NoVerticalHeader);

    ui->tableCalendarDetails->setColumnCount(6);
    ui->tableCalendarDetails->setHorizontalHeaderLabels({"Type d'huile", "Quantite (L)", "Seuil (L)", "Emplacement", "Etat", "Date MAJ"});
    ui->tableCalendarDetails->horizontalHeader()->setStretchLastSection(true);
    ui->tableCalendarDetails->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableCalendarDetails->setEditTriggers(QAbstractItemView::NoEditTriggers);

    chargerDatesStockCalendrier();
    onCalendarDateSelected(ui->calendarWidgetStock->selectedDate());
}

void Stock::chargerDatesStockCalendrier()
{
    if (!connexion || !connexion->isOpen() || !ui->calendarWidgetStock) {
        return;
    }

    dateStatutMap.clear();

    QSqlQuery query(connexion->getDatabase());
    query.prepare("SELECT TRUNC(DATE_DERNIERE_MISE_A_JOUR), QUANTITE_ACTUELLE, SEUIL_ALERTE "
                  "FROM STOCK WHERE DATE_DERNIERE_MISE_A_JOUR IS NOT NULL");

    if (!query.exec()) {
        qDebug() << "Erreur chargement dates calendrier:" << query.lastError().text();
        return;
    }

    while (query.next()) {
        QDate date = query.value(0).toDate();
        double quantite = query.value(1).toDouble();
        double seuil = query.value(2).toDouble();

        QString statut = "disponible";
        if (seuil > 0 && quantite <= (seuil * 0.5)) {
            statut = "critique";
        } else if (seuil > 0 && quantite <= seuil) {
            statut = "alerte";
        }

        const QString current = dateStatutMap.value(date);
        if (current == "critique") {
            continue;
        }
        if (current == "alerte" && statut == "disponible") {
            continue;
        }

        dateStatutMap.insert(date, statut);
    }

    mettreEnCouleurCalendrier();
    mettreAJourKpiCalendrier();
}

void Stock::mettreAJourKpiCalendrier()
{
    int alertDates = 0;
    int criticalDates = 0;

    for (auto it = dateStatutMap.constBegin(); it != dateStatutMap.constEnd(); ++it) {
        if (it.value() == "critique") {
            criticalDates++;
        } else if (it.value() == "alerte") {
            alertDates++;
        }
    }

    if (ui->labelKpiDatesActivesValue) {
        ui->labelKpiDatesActivesValue->setText(QString::number(dateStatutMap.size()));
    }
    if (ui->labelKpiAlertesValue) {
        ui->labelKpiAlertesValue->setText(QString::number(alertDates));
    }
    if (ui->labelKpiCritiquesValue) {
        ui->labelKpiCritiquesValue->setText(QString::number(criticalDates));
    }
}

void Stock::mettreEnCouleurCalendrier()
{
    if (!ui->calendarWidgetStock) {
        return;
    }

    ui->calendarWidgetStock->setDateTextFormat(QDate(), QTextCharFormat());

    for (auto it = dateStatutMap.constBegin(); it != dateStatutMap.constEnd(); ++it) {
        QTextCharFormat fmt;

        if (it.value() == "critique") {
            fmt.setBackground(QBrush(QColor("#ef4444")));
            fmt.setForeground(QBrush(Qt::white));
        } else if (it.value() == "alerte") {
            fmt.setBackground(QBrush(QColor("#f59e0b")));
            fmt.setForeground(QBrush(Qt::black));
        } else {
            fmt.setBackground(QBrush(QColor("#22c55e")));
            fmt.setForeground(QBrush(Qt::white));
        }

        fmt.setFontWeight(QFont::DemiBold);
        ui->calendarWidgetStock->setDateTextFormat(it.key(), fmt);
    }
}

void Stock::onCalendarDateSelected(const QDate &date)
{
    afficherStocksParDate(date);
}

void Stock::onCalendarShowMonth(int year, int month)
{
    Q_UNUSED(year);
    Q_UNUSED(month);
    mettreEnCouleurCalendrier();
}

void Stock::afficherStocksParDate(const QDate &date)
{
    if (!connexion || !connexion->isOpen() || !ui->tableCalendarDetails) {
        return;
    }

    QSqlQuery query(connexion->getDatabase());
    query.prepare("SELECT TYPE_HUILE, QUANTITE_ACTUELLE, SEUIL_ALERTE, EMPLACEMENT_STOCKAGE, ETAT_STOCK, DATE_DERNIERE_MISE_A_JOUR "
                  "FROM STOCK "
                  "WHERE TRUNC(DATE_DERNIERE_MISE_A_JOUR) = TO_DATE(:dateValue, 'YYYY-MM-DD') "
                  "ORDER BY TYPE_HUILE");
    query.bindValue(":dateValue", date.toString("yyyy-MM-dd"));

    if (!query.exec()) {
        qDebug() << "Erreur afficherStocksParDate:" << query.lastError().text();
        return;
    }

    ui->tableCalendarDetails->setRowCount(0);

    int row = 0;
    int countDisponible = 0;
    int countAlerte = 0;
    int countCritique = 0;
    while (query.next()) {
        const double quantite = query.value(1).toDouble();
        const double seuil = query.value(2).toDouble();

        ui->tableCalendarDetails->insertRow(row);
        ui->tableCalendarDetails->setItem(row, 0, new QTableWidgetItem(query.value(0).toString()));
        ui->tableCalendarDetails->setItem(row, 1, new QTableWidgetItem(QString::number(quantite, 'f', 2)));
        ui->tableCalendarDetails->setItem(row, 2, new QTableWidgetItem(QString::number(seuil, 'f', 2)));
        ui->tableCalendarDetails->setItem(row, 3, new QTableWidgetItem(query.value(3).toString()));
        ui->tableCalendarDetails->setItem(row, 4, new QTableWidgetItem(query.value(4).toString()));
        ui->tableCalendarDetails->setItem(row, 5, new QTableWidgetItem(query.value(5).toDate().toString("dd/MM/yyyy")));

        QColor rowColor("#dcfce7");
        QColor textColor("#14532d");
        if (seuil > 0 && quantite <= (seuil * 0.5)) {
            rowColor = QColor("#fee2e2");
            textColor = QColor("#991b1b");
            countCritique++;
        } else if (seuil > 0 && quantite <= seuil) {
            rowColor = QColor("#ffedd5");
            textColor = QColor("#92400e");
            countAlerte++;
        } else {
            countDisponible++;
        }

        for (int col = 0; col < ui->tableCalendarDetails->columnCount(); ++col) {
            QTableWidgetItem *item = ui->tableCalendarDetails->item(row, col);
            if (item) {
                item->setBackground(QBrush(rowColor));
                item->setForeground(QBrush(textColor));
            }
        }

        row++;
    }

    if (ui->labelCalendarSummary) {
        ui->labelCalendarSummary->setText(
            QString("📌 Date selectionnee : %1 | %2 enregistrement(s) | "
                    "<span style='color:#14532d;'>🟢 %3</span> "
                    "<span style='color:#92400e;'>🟡 %4</span> "
                    "<span style='color:#991b1b;'>🔴 %5</span>")
                .arg(date.toString("dd/MM/yyyy"))
                .arg(row)
                .arg(countDisponible)
                .arg(countAlerte)
                .arg(countCritique));
        animerResumeCalendrier();
    }
}

void Stock::animerResumeCalendrier()
{
    if (!ui->labelCalendarSummary) {
        return;
    }

    QGraphicsOpacityEffect *effect = qobject_cast<QGraphicsOpacityEffect*>(ui->labelCalendarSummary->graphicsEffect());
    if (!effect) {
        effect = new QGraphicsOpacityEffect(ui->labelCalendarSummary);
        ui->labelCalendarSummary->setGraphicsEffect(effect);
    }

    QPropertyAnimation *animation = new QPropertyAnimation(effect, "opacity", ui->labelCalendarSummary);
    animation->setDuration(280);
    animation->setStartValue(0.35);
    animation->setEndValue(1.0);
    animation->setEasingCurve(QEasingCurve::OutCubic);
    animation->start(QAbstractAnimation::DeleteWhenStopped);
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
    if (!connexion || !connexion->isOpen()) {
        afficherMessage("Erreur", "Pas de connexion à la base de données", true);
        return;
    }

    int periodDays = 30;
    const QString periode = ui->comboPeriode->currentText();
    if (periode.contains("7")) {
        periodDays = 7;
    } else if (periode.contains("90") || periode.contains("3")) {
        periodDays = 90;
    }

    const QDate startDate = QDate::currentDate().addDays(-periodDays);

    QSqlQuery query(connexion->getDatabase());
    query.prepare(
        "SELECT s.TYPE_HUILE, s.QUANTITE_ACTUELLE, s.SEUIL_ALERTE, "
        "       NVL(SUM(CASE WHEN p.DATEPRODUCTION >= :startDate THEN p.HUILEPRODUITEL ELSE 0 END), 0) AS PROD_PERIODE, "
        "       NVL(COUNT(CASE WHEN p.DATEPRODUCTION >= :startDate THEN 1 END), 0) AS NB_OPERATIONS "
        "FROM STOCK s "
        "LEFT JOIN PRODUCTION p ON p.ID_STOCK = s.ID_STOCK "
        "GROUP BY s.TYPE_HUILE, s.QUANTITE_ACTUELLE, s.SEUIL_ALERTE "
        "ORDER BY PROD_PERIODE DESC, s.TYPE_HUILE");
    query.bindValue(":startDate", startDate);

    if (!query.exec()) {
        afficherMessage("Erreur", "Erreur d'analyse prédictive : " + query.lastError().text(), true);
        return;
    }

    struct OilAnalysis {
        QString type;
        double stockCurrent;
        double threshold;
        double producedPeriod;
        int operations;
        double popularityPct;
        double predicted7;
        double predicted30;
        int riskPct;
        int reorderQty;
        int productionQty;
        QString priority;
    };

    QList<OilAnalysis> analyses;
    double totalProduced = 0.0;

    while (query.next()) {
        OilAnalysis item;
        item.type = query.value(0).toString();
        item.stockCurrent = query.value(1).toDouble();
        item.threshold = query.value(2).toDouble();
        item.producedPeriod = query.value(3).toDouble();
        item.operations = query.value(4).toInt();
        item.popularityPct = 0.0;
        item.predicted7 = 0.0;
        item.predicted30 = 0.0;
        item.riskPct = 0;
        item.reorderQty = 0;
        item.productionQty = 0;
        item.priority = "Basse";

        totalProduced += item.producedPeriod;
        analyses.append(item);
    }

    if (analyses.isEmpty()) {
        afficherMessage("Analyse", "Aucune donnée disponible pour l'analyse.", true);
        return;
    }

    for (OilAnalysis &item : analyses) {
        const double avgDailyFromProd = item.producedPeriod / qMax(1, periodDays);
        const double baselineDemand = qMax(1.0, item.threshold / 30.0);
        const double estimatedDailyDemand = qMax(avgDailyFromProd, baselineDemand);

        item.popularityPct = (totalProduced > 0.0)
            ? (item.producedPeriod / totalProduced) * 100.0
            : (100.0 / analyses.size());

        item.predicted7 = estimatedDailyDemand * 7.0;
        item.predicted30 = estimatedDailyDemand * 30.0;

        double stockCoverageDays = item.stockCurrent / estimatedDailyDemand;
        int risk = 15;
        if (stockCoverageDays <= 3.0) {
            risk = 90;
        } else if (stockCoverageDays <= 7.0) {
            risk = 70;
        } else if (stockCoverageDays <= 14.0) {
            risk = 45;
        }

        if (item.threshold > 0.0 && item.stockCurrent <= (item.threshold * 0.5)) {
            risk = qMax(risk, 95);
        } else if (item.threshold > 0.0 && item.stockCurrent <= item.threshold) {
            risk = qMax(risk, 75);
        }

        item.riskPct = risk;

        const double securityStock = qMax(10.0, item.threshold * 0.6);
        item.reorderQty = qMax(0, static_cast<int>(item.predicted30 + securityStock - item.stockCurrent));
        item.productionQty = qMax(0, static_cast<int>(item.predicted7 + (item.popularityPct > 35.0 ? item.predicted7 * 0.2 : 0.0) - item.stockCurrent * 0.25));

        if (item.riskPct >= 80 || item.popularityPct >= 40.0) {
            item.priority = "Haute";
        } else if (item.riskPct >= 50 || item.popularityPct >= 20.0) {
            item.priority = "Moyenne";
        }
    }

    std::sort(analyses.begin(), analyses.end(), [](const OilAnalysis &a, const OilAnalysis &b) {
        if (a.priority != b.priority) {
            if (a.priority == "Haute") return true;
            if (b.priority == "Haute") return false;
            if (a.priority == "Moyenne") return true;
            if (b.priority == "Moyenne") return false;
        }
        if (a.riskPct != b.riskPct) {
            return a.riskPct > b.riskPct;
        }
        return a.popularityPct > b.popularityPct;
    });

    double totalRisk = 0.0;
    int totalReorder = 0;
    for (const OilAnalysis &item : analyses) {
        totalRisk += item.riskPct;
        totalReorder += item.reorderQty;
    }

    const double avgRisk = analyses.isEmpty() ? 0.0 : totalRisk / analyses.size();
    const QString topStrategic = analyses.isEmpty()
        ? QString("--")
        : QString("%1 (%2%)")
              .arg(analyses.first().type)
              .arg(QString::number(analyses.first().popularityPct, 'f', 1));

    if (ui->labelTopStrategiqueValue) {
        ui->labelTopStrategiqueValue->setText(topStrategic);
    }
    if (ui->labelRisqueMoyenGlobalValue) {
        ui->labelRisqueMoyenGlobalValue->setText(QString("%1 %").arg(QString::number(avgRisk, 'f', 1)));
    }
    if (ui->labelBesoinReapproTotalValue) {
        ui->labelBesoinReapproTotalValue->setText(QString("%1 L").arg(totalReorder));
    }

    ui->tableResultatsCombined->clear();
    ui->tableResultatsCombined->setColumnCount(10);
    ui->tableResultatsCombined->setHorizontalHeaderLabels({
        "Type d'huile", "Stock (L)", "Popularité (%)", "Prévision J+7 (L)",
        "Prévision J+30 (L)", "Risque (%)", "Réappro (L)",
        "Production (L)", "Opérations", "Priorité"
    });
    ui->tableResultatsCombined->setRowCount(0);
    ui->tableResultatsCombined->setWordWrap(false);
    ui->tableResultatsCombined->setAlternatingRowColors(true);
    ui->tableResultatsCombined->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    ui->tableResultatsCombined->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    ui->tableResultatsCombined->verticalHeader()->setDefaultSectionSize(32);

    const QList<int> predictiveColumnWidths = {190, 100, 120, 130, 140, 95, 110, 115, 95, 100};
    for (int col = 0; col < predictiveColumnWidths.size() && col < ui->tableResultatsCombined->columnCount(); ++col) {
        ui->tableResultatsCombined->setColumnWidth(col, predictiveColumnWidths[col]);
    }

    for (int col = 0; col < ui->tableResultatsCombined->columnCount(); ++col) {
        QTableWidgetItem* headerItem = ui->tableResultatsCombined->horizontalHeaderItem(col);
        if (headerItem) {
            headerItem->setTextAlignment(Qt::AlignCenter);
        }
    }

    int row = 0;
    for (const OilAnalysis &item : analyses) {
        ui->tableResultatsCombined->insertRow(row);
        ui->tableResultatsCombined->setItem(row, 0, new QTableWidgetItem(item.type));
        ui->tableResultatsCombined->setItem(row, 1, new QTableWidgetItem(QString::number(item.stockCurrent, 'f', 2)));
        ui->tableResultatsCombined->setItem(row, 2, new QTableWidgetItem(QString::number(item.popularityPct, 'f', 1)));
        ui->tableResultatsCombined->setItem(row, 3, new QTableWidgetItem(QString::number(item.predicted7, 'f', 1)));
        ui->tableResultatsCombined->setItem(row, 4, new QTableWidgetItem(QString::number(item.predicted30, 'f', 1)));
        ui->tableResultatsCombined->setItem(row, 5, new QTableWidgetItem(QString::number(item.riskPct)));
        ui->tableResultatsCombined->setItem(row, 6, new QTableWidgetItem(QString::number(item.reorderQty)));
        ui->tableResultatsCombined->setItem(row, 7, new QTableWidgetItem(QString::number(item.productionQty)));
        ui->tableResultatsCombined->setItem(row, 8, new QTableWidgetItem(QString::number(item.operations)));
        ui->tableResultatsCombined->setItem(row, 9, new QTableWidgetItem(item.priority));

        QColor bgColor("#dcfce7");
        QColor fgColor("#14532d");
        if (item.priority == "Haute") {
            bgColor = QColor("#fee2e2");
            fgColor = QColor("#991b1b");
        } else if (item.priority == "Moyenne") {
            bgColor = QColor("#ffedd5");
            fgColor = QColor("#92400e");
        }

        for (int col = 0; col < ui->tableResultatsCombined->columnCount(); ++col) {
            QTableWidgetItem *cell = ui->tableResultatsCombined->item(row, col);
            if (cell) {
                cell->setBackground(QBrush(bgColor));
                cell->setForeground(QBrush(fgColor));
                if (col == 0) {
                    cell->setTextAlignment(Qt::AlignVCenter | Qt::AlignLeft);
                } else {
                    cell->setTextAlignment(Qt::AlignCenter);
                }
            }
        }

        row++;
    }

    ui->tableResultatsCombined->horizontalHeader()->setStretchLastSection(false);
    ui->tableResultatsCombined->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableResultatsCombined->setEditTriggers(QAbstractItemView::NoEditTriggers);

    QString recommendations;
    recommendations += "ANALYSE PREDICTIVE & PRESCRIPTIVE DES TYPES D'HUILE\n";
    recommendations += "Periode: " + periode + "\n";
    recommendations += "Total production analysee: " + QString::number(totalProduced, 'f', 2) + " L\n\n";
    recommendations += "TOP RECOMMANDATIONS:\n";

    const int topN = qMin(3, analyses.size());
    for (int i = 0; i < topN; ++i) {
        const OilAnalysis &item = analyses[i];
        recommendations += QString("%1) %2\n")
            .arg(i + 1)
            .arg(item.type);
        recommendations += QString("   - Popularite: %1% | Risque rupture: %2%\n")
            .arg(QString::number(item.popularityPct, 'f', 1))
            .arg(item.riskPct);
        recommendations += QString("   - Produire: %1 L | Reappro: %2 L\n")
            .arg(item.productionQty)
            .arg(item.reorderQty);
        recommendations += QString("   - Priorite: %1\n\n")
            .arg(item.priority);
    }

    recommendations += "INTERPRETATION:\n";
    recommendations += "- Priorite Haute: action immediate recommandee.\n";
    recommendations += "- Priorite Moyenne: planifier cette semaine.\n";
    recommendations += "- Priorite Basse: suivi normal.\n";

    if (ui->textAnalyseTendances) {
        ui->textAnalyseTendances->setPlainText(recommendations);
    }

    afficherMessage("Analyse", "Analyse prédictive et recommandations générées avec succès !");
}

void Stock::onAnalyserPredictifHuileClicked()
{
    if (ui->comboPeriodePredictifHuile && ui->comboPeriode) {
        ui->comboPeriode->setCurrentText(ui->comboPeriodePredictifHuile->currentText());
    }

    onAnalyserTendancesClicked();

    if (ui->tableResultatsPredictifHuile && ui->tableResultatsCombined) {
        ui->tableResultatsPredictifHuile->clear();
        ui->tableResultatsPredictifHuile->setColumnCount(ui->tableResultatsCombined->columnCount());

        QStringList headers;
        for (int col = 0; col < ui->tableResultatsCombined->columnCount(); ++col) {
            QTableWidgetItem* headerItem = ui->tableResultatsCombined->horizontalHeaderItem(col);
            headers << (headerItem ? headerItem->text() : QString("Colonne %1").arg(col + 1));
        }
        ui->tableResultatsPredictifHuile->setHorizontalHeaderLabels(headers);
        ui->tableResultatsPredictifHuile->setRowCount(0);
        ui->tableResultatsPredictifHuile->setWordWrap(false);
        ui->tableResultatsPredictifHuile->setAlternatingRowColors(true);
        ui->tableResultatsPredictifHuile->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
        ui->tableResultatsPredictifHuile->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
        ui->tableResultatsPredictifHuile->verticalHeader()->setDefaultSectionSize(32);

        for (int row = 0; row < ui->tableResultatsCombined->rowCount(); ++row) {
            ui->tableResultatsPredictifHuile->insertRow(row);
            for (int col = 0; col < ui->tableResultatsCombined->columnCount(); ++col) {
                QTableWidgetItem* source = ui->tableResultatsCombined->item(row, col);
                if (!source) {
                    continue;
                }
                QTableWidgetItem* cloned = source->clone();
                ui->tableResultatsPredictifHuile->setItem(row, col, cloned);
            }
        }

        for (int col = 0; col < ui->tableResultatsPredictifHuile->columnCount(); ++col) {
            ui->tableResultatsPredictifHuile->setColumnWidth(col, ui->tableResultatsCombined->columnWidth(col));
            QTableWidgetItem* headerItem = ui->tableResultatsPredictifHuile->horizontalHeaderItem(col);
            if (headerItem) {
                headerItem->setTextAlignment(Qt::AlignCenter);
            }
        }

        ui->tableResultatsPredictifHuile->horizontalHeader()->setStretchLastSection(false);
        ui->tableResultatsPredictifHuile->setSelectionBehavior(QAbstractItemView::SelectRows);
        ui->tableResultatsPredictifHuile->setEditTriggers(QAbstractItemView::NoEditTriggers);
    }

    if (ui->textAnalysePredictifHuile && ui->textAnalyseTendances) {
        ui->textAnalysePredictifHuile->setPlainText(ui->textAnalyseTendances->toPlainText());
    }

    if (ui->labelTopStrategiqueValue2 && ui->labelTopStrategiqueValue) {
        ui->labelTopStrategiqueValue2->setText(ui->labelTopStrategiqueValue->text());
    }
    if (ui->labelRisqueMoyenGlobalValue2 && ui->labelRisqueMoyenGlobalValue) {
        ui->labelRisqueMoyenGlobalValue2->setText(ui->labelRisqueMoyenGlobalValue->text());
    }
    if (ui->labelBesoinReapproTotalValue2 && ui->labelBesoinReapproTotalValue) {
        ui->labelBesoinReapproTotalValue2->setText(ui->labelBesoinReapproTotalValue->text());
    }
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
    onCalendarDateSelected(ui->calendarWidgetStock->selectedDate());
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
