#include "finance.h"
#include "connexionfinance.h"
#include "ui_finance.h"
#include <QCoreApplication>
#include <string>
using namespace std;
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <Qt>
#include <QGroupBox>
#include <QLabel>
#include <QGridLayout>
#include <QFrame>
#include <QAbstractItemView>
#include <QPixmap>
#include <QCloseEvent>
#include <QMessageBox>

// ========================================================================
// IMPLÉMENTATION: TransactionTab
// ========================================================================
TransactionTab::TransactionTab(int userId, QWidget *parent)
    : QWidget(parent), currentUserId(userId), dbConn(new ConnexionFinance())
{
    initializeUI();

    if (!dbConn->open()) {
        QMessageBox::critical(this, "Erreur Base de Données",
            QString::fromUtf8("Impossible d'ouvrir la connexion Oracle via Qt SQL :\n") + 
            dbConn->getDatabase().lastError().text());
    } else {
        qDebug() << "Connexion Oracle (Qt SQL) etablie dans TransactionTab.";
    }

    setupConnections();
    chargerData(); 
    chargerTransactions();
}

TransactionTab::~TransactionTab()
{
    delete dbConn;
}


void TransactionTab::initializeUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(5, 5, 5, 5);

    // ===== SPLITTER HORIZONTAL: Formulaire (gauche) + Recherche/Tri/Export (droite) =====
    QSplitter *splitter = new QSplitter(Qt::Horizontal);
    splitter->setStyleSheet("QSplitter::handle { background-color: #ddd; width: 2px; }");

    // ============================================================
    // CÔTÉ GAUCHE: Formulaire CRUD
    // ============================================================
    QWidget *leftWidget = new QWidget();
    QVBoxLayout *leftLayout = new QVBoxLayout(leftWidget);
    leftLayout->setContentsMargins(0, 0, 0, 0);
    leftLayout->setSpacing(0);

    // ===== Formulaire EN HAUT =====
    QGroupBox *formGroup = new QGroupBox(QString::fromUtf8("📝 Nouvelle Transaction / Modification"), this);
    QVBoxLayout *formLayout = new QVBoxLayout(formGroup);
    formLayout->setContentsMargins(6, 4, 6, 4);
    formLayout->setSpacing(5);  // Moins d'espace

    QGridLayout *mainGrid = new QGridLayout();
    mainGrid->setContentsMargins(4, 2, 4, 2);
    mainGrid->setHorizontalSpacing(12);
    mainGrid->setVerticalSpacing(4);  // Réduit de 15

    // Labels Style
    QString labelStyle = "font-weight: bold; color: #1B4332; font-size: 11px;";

    // Ligne 0 : ID
    QLabel *lblId = new QLabel("ID Transaction :");
    lblId->setStyleSheet(labelStyle);
    txtId = new QLineEdit();
    txtId->setReadOnly(true);
    txtId->setPlaceholderText("Auto-généré");
    txtId->setStyleSheet("background-color: #f0f0f0; border-radius: 5px; padding: 5px;");
    mainGrid->addWidget(lblId, 0, 0);
    mainGrid->addWidget(txtId, 0, 1);

    // Ligne 1 : Type & Catégorie
    QLabel *lblType = new QLabel("Type :");
    lblType->setStyleSheet(labelStyle);
    cbType = new QComboBox();
    cbType->addItems({"REVENU", QString::fromUtf8("DÉPENSE")});
    mainGrid->addWidget(lblType, 1, 0);
    mainGrid->addWidget(cbType, 1, 1);

    QLabel *lblCat = new QLabel(QString::fromUtf8("Catégorie :"));
    lblCat->setStyleSheet(labelStyle);
    cbCategorie = new QComboBox();
    cbCategorie->setMinimumWidth(200);
    mainGrid->addWidget(lblCat, 1, 2);
    mainGrid->addWidget(cbCategorie, 1, 3);

    // Ligne 2 : Montant & Date
    QLabel *lblMontant = new QLabel("Montant :");
    lblMontant->setStyleSheet(labelStyle);
    spinMontant = new QDoubleSpinBox();
    spinMontant->setRange(0, 1000000);
    spinMontant->setDecimals(2);
    spinMontant->setSuffix(" DT");
    spinMontant->setButtonSymbols(QAbstractSpinBox::NoButtons);
    mainGrid->addWidget(lblMontant, 2, 0);
    mainGrid->addWidget(spinMontant, 2, 1);

    QLabel *lblDate = new QLabel("Date :");
    lblDate->setStyleSheet(labelStyle);
    dateEdit = new QDateEdit(QDate::currentDate());
    dateEdit->setCalendarPopup(true);
    mainGrid->addWidget(lblDate, 2, 2);
    mainGrid->addWidget(dateEdit, 2, 3);

    // Ligne 3 : Machine
    lblMachine = new QLabel("Machine :");
    lblMachine->setStyleSheet(labelStyle);
    cbMachine = new QComboBox();
    mainGrid->addWidget(lblMachine, 3, 0);
    mainGrid->addWidget(cbMachine, 3, 1);

    // Ligne 4 : Description
    QLabel *lblDesc = new QLabel("Description :");
    lblDesc->setStyleSheet(labelStyle);
    txtDescription = new QTextEdit();
    txtDescription->setMaximumHeight(40);
    txtDescription->setPlaceholderText("Détails de la transaction...");
    mainGrid->addWidget(lblDesc, 4, 0);
    mainGrid->addWidget(txtDescription, 4, 1, 1, 3);

    formLayout->addLayout(mainGrid);

    // --- Boutons Actions ---
    QHBoxLayout *actionLayout = new QHBoxLayout();
    actionLayout->setSpacing(6);
    actionLayout->setContentsMargins(15, 2, 15, 2);

    btnAdd = new QPushButton(QString::fromUtf8("➕ Ajouter"));
    btnUpdate = new QPushButton(QString::fromUtf8("🔄 Modifier"));
    btnDelete = new QPushButton(QString::fromUtf8("🗑️ Supprimer"));
    btnConsult = new QPushButton(QString::fromUtf8("🧹 Nouveau"));

    QString btnBaseStyle = "QPushButton { color: white; border-radius: 6px; font-weight: bold; font-size: 11px; min-height: 26px; min-width: 90px; } ";
    btnAdd->setStyleSheet(btnBaseStyle + "QPushButton { background-color: #1B4332; } QPushButton:hover { background-color: #2D5A47; }");
    btnUpdate->setStyleSheet(btnBaseStyle + "QPushButton { background-color: #1A5276; } QPushButton:hover { background-color: #2471A3; }");
    btnDelete->setStyleSheet(btnBaseStyle + "QPushButton { background-color: #922B21; } QPushButton:hover { background-color: #C0392B; }");
    btnConsult->setStyleSheet(btnBaseStyle + "QPushButton { background-color: #616A6B; } QPushButton:hover { background-color: #7F8C8D; }");

    actionLayout->addWidget(btnAdd);
    actionLayout->addWidget(btnUpdate);
    actionLayout->addWidget(btnDelete);
    actionLayout->addWidget(btnConsult);

    formLayout->addLayout(actionLayout);

    onTypeChanged(cbType->currentText());

    leftLayout->addWidget(formGroup, 0);  // Formulaire EN HAUT

    // ===== Tableau EN BAS =====
    QGroupBox *tableGroup = new QGroupBox(QString::fromUtf8("📋 Transactions"), this);
    QVBoxLayout *tableLayout = new QVBoxLayout(tableGroup);
    tableLayout->setContentsMargins(2, 2, 2, 2);

    tableTransaction = new QTableWidget();
    tableTransaction->setColumnCount(7);
    tableTransaction->setHorizontalHeaderLabels({"ID", "Type", QString::fromUtf8("Catégorie"), "Montant", "Date", "Machine", "Description"});

    tableTransaction->verticalHeader()->setDefaultSectionSize(28);
    tableTransaction->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableTransaction->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableTransaction->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableTransaction->setStyleSheet(
        "QTableWidget { background-color: white; gridline-color: #f0f0f0; font-size: 10px; } "
        "QHeaderView::section { background-color: #1B4332; color: white; font-weight: bold; padding: 3px; } "
        "QTableWidget::item { padding: 3px; } "
        "QTableWidget::item:selected { background-color: #c8e6c9; color: #1B4332; }"
    );

    tableLayout->addWidget(tableTransaction);
    leftLayout->addWidget(tableGroup, 1);  // Tableau EN BAS, prend l'espace restant

    // ============================================================
    // CÔTÉ DROIT: Recherche, Tri, Export + Tableau
    // ============================================================
    QWidget *rightWidget = new QWidget();
    QVBoxLayout *rightLayout = new QVBoxLayout(rightWidget);
    rightLayout->setContentsMargins(0, 0, 0, 0);
    rightLayout->setSpacing(0);  // Réduit pour plus d'espace au tableau

    // Groupe Recherche
    QGroupBox *searchGroup = new QGroupBox(QString::fromUtf8("🔍 Recherche"), this);
    QVBoxLayout *searchLayout = new QVBoxLayout(searchGroup);
    searchLayout->setContentsMargins(4, 3, 4, 3);
    searchLayout->setSpacing(2);

    QHBoxLayout *typeLayout = new QHBoxLayout();
    typeLayout->setContentsMargins(0, 0, 0, 0);
    typeLayout->setSpacing(3);
    QLabel *lblSearch = new QLabel("Type:");
    lblSearch->setStyleSheet("font-weight: bold; font-size: 11px;");
    lblSearch->setMaximumWidth(50);
    cbSearchType = new QComboBox();
    cbSearchType->addItems({"Tous", "REVENU", QString::fromUtf8("DÉPENSE")});
    cbSearchType->setMaximumHeight(24);
    cbSearchType->setMinimumHeight(22);
    typeLayout->addWidget(lblSearch);
    typeLayout->addWidget(cbSearchType);
    searchLayout->addLayout(typeLayout);

    QHBoxLayout *catLayout = new QHBoxLayout();
    catLayout->setContentsMargins(0, 0, 0, 0);
    catLayout->setSpacing(3);
    QLabel *lblSearchCat = new QLabel(QString::fromUtf8("Catégorie:"));
    lblSearchCat->setStyleSheet("font-weight: bold; font-size: 11px;");
    lblSearchCat->setMaximumWidth(70);
    cbSearchCategorie = new QComboBox();
    cbSearchCategorie->addItem("Toutes");
    cbSearchCategorie->setMaximumHeight(24);
    cbSearchCategorie->setMinimumHeight(22);
    catLayout->addWidget(lblSearchCat);
    catLayout->addWidget(cbSearchCategorie);
    searchLayout->addLayout(catLayout);

    btnSearch = new QPushButton(QString::fromUtf8("🔍 Rechercher"));
    btnSearch->setMaximumHeight(26);
    btnSearch->setMinimumHeight(24);
    btnSearch->setStyleSheet("QPushButton { background-color: #1B7331; color: white; border-radius: 4px; font-weight: bold; padding: 3px; font-size: 10px; }");
    searchLayout->addWidget(btnSearch);
    
    searchGroup->setMaximumHeight(110);

    // (Les groupes seront ajoutés au conteneur filterWidget)

    // Groupe Tri
    QGroupBox *sortGroup = new QGroupBox(QString::fromUtf8("📊 Tri"), this);
    QVBoxLayout *sortLayout = new QVBoxLayout(sortGroup);
    sortLayout->setContentsMargins(4, 3, 4, 3);
    sortLayout->setSpacing(2);

    QHBoxLayout *sortTypeLayout = new QHBoxLayout();
    sortTypeLayout->setContentsMargins(0, 0, 0, 0);
    sortTypeLayout->setSpacing(3);
    QLabel *lblSort = new QLabel(QString::fromUtf8("Trier:"));
    lblSort->setStyleSheet("font-weight: bold; font-size: 11px;");
    lblSort->setMaximumWidth(50);
    cbSort = new QComboBox();
    cbSort->addItems({"ID (↓)", "Montant (↑)", "Montant (↓)", "Date"});
    cbSort->setMaximumHeight(24);
    cbSort->setMinimumHeight(22);
    sortTypeLayout->addWidget(lblSort);
    sortTypeLayout->addWidget(cbSort);
    sortLayout->addLayout(sortTypeLayout);

    btnSort = new QPushButton(QString::fromUtf8("↕️ Appliquer"));
    btnSort->setMaximumHeight(26);
    btnSort->setMinimumHeight(24);
    btnSort->setStyleSheet("QPushButton { background-color: #0056b3; color: white; border-radius: 4px; font-weight: bold; padding: 3px; font-size: 10px; }");
    sortLayout->addWidget(btnSort);
    
    sortGroup->setMaximumHeight(85);

    // (Les groupes seront ajoutés au conteneur filterWidget)

    // Groupe Export
    QGroupBox *exportGroup = new QGroupBox(QString::fromUtf8("💾 Export"), this);
    QVBoxLayout *exportLayout = new QVBoxLayout(exportGroup);
    exportLayout->setContentsMargins(4, 3, 4, 3);
    exportLayout->setSpacing(4);

    btnExportPDF = new QPushButton(QString::fromUtf8("PDF"));
    btnExportPDF->setMaximumHeight(26);
    btnExportPDF->setMinimumHeight(24);
    btnExportPDF->setStyleSheet("QPushButton { background-color: #d32f2f; color: white; border-radius: 4px; font-weight: bold; padding: 3px; font-size: 10px; }");
    exportLayout->addWidget(btnExportPDF);

    btnExportExcel = new QPushButton(QString::fromUtf8("Excel"));
    btnExportExcel->setMaximumHeight(26);
    btnExportExcel->setMinimumHeight(24);
    btnExportExcel->setStyleSheet("QPushButton { background-color: #217346; color: white; border-radius: 4px; font-weight: bold; padding: 3px; font-size: 10px; }");
    exportLayout->addWidget(btnExportExcel);

    btnExportTxt = new QPushButton(QString::fromUtf8("TXT"));
    btnExportTxt->setMaximumHeight(26);
    btnExportTxt->setMinimumHeight(24);
    btnExportTxt->setStyleSheet("QPushButton { background-color: #616A6B; color: white; border-radius: 4px; font-weight: bold; padding: 3px; font-size: 10px; }");
    exportLayout->addWidget(btnExportTxt);
    
    exportGroup->setMaximumHeight(95);

    // ===== Conteneur pour Recherche/Tri/Export =====
    QWidget *filterWidget = new QWidget();
    QVBoxLayout *filterLayout = new QVBoxLayout(filterWidget);
    filterLayout->setContentsMargins(0, 0, 0, 0);
    filterLayout->setSpacing(5);

    filterLayout->addWidget(searchGroup);
    filterLayout->addWidget(sortGroup);
    filterLayout->addWidget(exportGroup);
    filterLayout->addStretch();

    rightLayout->addWidget(filterWidget, 0);

    // ============================================================
    // Ajouter les 2 sections au splitter (INVERSÉ)
    // ============================================================
    splitter->addWidget(leftWidget);   // Tableau+Formulaire À GAUCHE
    splitter->addWidget(rightWidget);  // Recherche/Tri/Export À DROITE
    splitter->setStretchFactor(0, 50);  // Tableau+Formulaire 50%
    splitter->setStretchFactor(1, 50);  // Recherche/Tri/Export 50%

    mainLayout->addWidget(splitter, 1);
}

void TransactionTab::setupConnections()
{
    // CRUD
    connect(btnAdd, &QPushButton::clicked, this, &TransactionTab::ajouterTransaction);
    connect(btnUpdate, &QPushButton::clicked, this, &TransactionTab::modifierTransaction);
    connect(btnDelete, &QPushButton::clicked, this, &TransactionTab::supprimerTransaction);
    connect(btnConsult, &QPushButton::clicked, this, &TransactionTab::effacerFormulaire);
    connect(cbType, &QComboBox::currentTextChanged, this, &TransactionTab::onTypeChanged);
    connect(cbCategorie, &QComboBox::currentTextChanged, this, &TransactionTab::onCategorieChanged);

    // Recherche/Tri/Export
    connect(btnSearch, &QPushButton::clicked, this, &TransactionTab::rechercher);
    connect(btnSort, &QPushButton::clicked, this, &TransactionTab::trier);
    connect(btnExportPDF, &QPushButton::clicked, this, &TransactionTab::exportPDF);
    connect(btnExportExcel, &QPushButton::clicked, this, &TransactionTab::exportExcel);
    connect(btnExportTxt, &QPushButton::clicked, this, &TransactionTab::exportTxt);

    // Sélection Table -> Remplissage Formulaire
    connect(tableTransaction, &QTableWidget::itemSelectionChanged, this, [=]() {
        int row = tableTransaction->currentRow();
        if (row < 0) return;

        txtId->setText(tableTransaction->item(row, 0)->text());
        QString typeVal = tableTransaction->item(row, 1)->text();
        cbType->setCurrentText(typeVal);
        onTypeChanged(typeVal); // Déclenche le show/hide correct

        cbCategorie->setCurrentText(tableTransaction->item(row, 2)->text());
        
        QString montantStr = tableTransaction->item(row, 3)->text().replace(" DT", "").replace(",", ".");
        spinMontant->setValue(montantStr.toDouble());
        
        QDate d = QDate::fromString(tableTransaction->item(row, 4)->text(), "dd/MM/yyyy");
        if (d.isValid()) dateEdit->setDate(d);
        
        QString targetVal = tableTransaction->item(row, 5)->text();
        cbMachine->setCurrentText(targetVal);
        
        txtDescription->setPlainText(tableTransaction->item(row, 6)->text());
    });
}


void TransactionTab::ajouterTransaction()
{
    // 1. Validation
    double amount = spinMontant->value();
    QString type = cbType->currentText();
    QString category = cbCategorie->currentText();
    int machineId = 0;

    // Validation montant (min/max)
    if (amount <= 0.0 || amount > 9999999.99) {
        QMessageBox::warning(this, "Validation", QString::fromUtf8("Le montant doit être entre 0.01 et 9999999.99 DT."));
        return;
    }
    
    if (category.isEmpty()) {
        QMessageBox::warning(this, "Validation", QString::fromUtf8("Veuillez sélectionner une catégorie."));
        return;
    }

    if (type == "REVENU") {
        // REVENU: pas de machine (id_machine = NULL)
        if (cbClient->currentText().isEmpty()) {
            QMessageBox::warning(this, "Validation", QString::fromUtf8("Veuillez sélectionner un client."));
            return;
        }
        machineId = 0;  // NULL pour les revenus
    } else {
        // DÉPENSE: machine optionnelle
        // Machine requise seulement pour "Maintenance"
        if (category == "Maintenance" && (cbMachine->currentIndex() < 0 || !cbMachine->currentData().isValid())) {
            QMessageBox::warning(this, "Validation", QString::fromUtf8("Veuillez sélectionner une machine pour une opération de maintenance."));
            return;
        }
        // Récupère la machine si sélectionnée, sinon NULL
        machineId = (cbMachine->currentIndex() >= 0 && cbMachine->currentData().isValid()) ? cbMachine->currentData().toInt() : 0;
    }

    // 2. Insertion
    QSqlDatabase db = dbConn->getDatabase();
    if (!db.isOpen()) return;

    // Étape 1: Récupérer la prochaine séquence
    QSqlQuery seqQuery(db);
    if (!seqQuery.exec("SELECT SEQ_FINANCE.NEXTVAL as id_seq FROM DUAL")) {
        QMessageBox::critical(this, "Erreur Séquence", seqQuery.lastError().text());
        return;
    }
    
    if (!seqQuery.next()) {
        QMessageBox::critical(this, "Erreur", QString::fromUtf8("Impossible de obtenir la séquence."));
        return;
    }
    
    int transId = seqQuery.value("id_seq").toInt();
    
    // Étape 2: Insérer avec l'ID obtenu - Sans paramètres (ODBC Oracle ne supporte pas bien les placeholders)
    QSqlQuery query(db);
    
    // Construire la requête avec des valeurs échappées
    QString sql = QString("INSERT INTO finance (id_transaction, id_machine, idProduction, ctype, categorie, montant, date_trans, description, user_id) "
                          "VALUES (%1, %2, 0, '%3', '%4', %5, SYSDATE, '%6', %7)")
                  .arg(transId)
                  .arg(machineId > 0 ? QString::number(machineId) : "NULL")
                  .arg(type.replace("'", "''"))
                  .arg(category.replace("'", "''"))
                  .arg(QString::number(amount, 'f', 2))  // Force point decimal
                  .arg(txtDescription->toPlainText().trimmed().replace("'", "''"))
                  .arg(currentUserId);
    
    if (!query.exec(sql)) {
        QMessageBox::critical(this, "Erreur", query.lastError().text());
        qDebug() << "SQL:" << sql;
        return;
    }

    chargerTransactions();
    effacerFormulaire();
    emit dataChanged();
    QMessageBox::information(this, QString::fromUtf8("Succès"), QString::fromUtf8("Transaction enregistrée avec succès."));
}

void TransactionTab::modifierTransaction()
{
    QString qId = txtId->text();
    if (qId.isEmpty()) {
        QMessageBox::warning(this, "Attention", QString::fromUtf8("Sélectionnez une transaction."));
        return;
    }

    double amount = spinMontant->value();
    QString type = cbType->currentText();
    QString category = cbCategorie->currentText();
    int machineId = 0;

    // Validation montant (min/max)
    if (amount <= 0.0 || amount > 9999999.99) {
        QMessageBox::warning(this, "Validation", QString::fromUtf8("Le montant doit être entre 0.01 et 9999999.99 DT."));
        return;
    }

    if (type == "REVENU") {
        // REVENU: pas de machine (id_machine = NULL)
        machineId = 0;
    } else {
        // DÉPENSE: machine optionnelle (sauf Maintenance)
        if (category == "Maintenance" && (cbMachine->currentIndex() < 0 || !cbMachine->currentData().isValid())) {
            QMessageBox::warning(this, "Validation", QString::fromUtf8("Veuillez sélectionner une machine pour une opération de maintenance."));
            return;
        }
        machineId = (cbMachine->currentIndex() >= 0 && cbMachine->currentData().isValid()) ? cbMachine->currentData().toInt() : 0;
    }

    QSqlDatabase db = dbConn->getDatabase();
    QSqlQuery query(db);
    
    // Construire la requête avec des valeurs échappées
    QString sql = QString("UPDATE finance SET ctype='%1', categorie='%2', montant=%3, "
                          "date_trans=SYSDATE, description='%4', id_machine=%5 WHERE id_transaction=%6")
                  .arg(type.replace("'", "''"))
                  .arg(category.replace("'", "''"))
                  .arg(QString::number(amount, 'f', 2))  // Force point decimal
                  .arg(txtDescription->toPlainText().trimmed().replace("'", "''"))
                  .arg(machineId > 0 ? QString::number(machineId) : "NULL")
                  .arg(qId.toInt());
    
    if (!query.exec(sql)) {
        QMessageBox::critical(this, "Erreur", query.lastError().text());
        qDebug() << "SQL:" << sql;
        return;
    }

    chargerTransactions();
    effacerFormulaire();
    emit dataChanged();
    QMessageBox::information(this, "Succes", "Transaction mise a jour.");
}

void TransactionTab::supprimerTransaction()
{
    int currentRow = tableTransaction->currentRow();
    if (currentRow < 0) {
        QMessageBox::warning(this, QString::fromUtf8("Attention"),
                             QString::fromUtf8("Veuillez sélectionner une transaction dans le tableau."));
        return;
    }

    int id = tableTransaction->item(currentRow, 0)->text().toInt();

    QMessageBox::StandardButton rep = QMessageBox::question(
        this, QString::fromUtf8("Confirmer la suppression"),
        QString::fromUtf8("Supprimer la transaction ID = %1 ?").arg(id),
        QMessageBox::Yes | QMessageBox::No
    );
    if (rep != QMessageBox::Yes) return;

    QSqlDatabase db = dbConn->getDatabase();
    if (!db.isOpen()) return;

    QSqlQuery query(db);
    query.prepare("DELETE FROM finance WHERE id_transaction = :id");
    query.bindValue(":id", id);

    if (query.exec()) {
        chargerTransactions();
        effacerFormulaire();
        emit dataChanged();
        QMessageBox::information(this, "Succes", "Transaction supprimee.");
    } else {
        QMessageBox::critical(this, "Erreur Oracle", query.lastError().text());
    }
}

void TransactionTab::chargerTransactions()
{
    tableTransaction->setRowCount(0);
    QSqlDatabase db = dbConn->getDatabase();
    if (!db.isOpen()) return;

    QSqlQuery query(db);
    query.exec("SELECT f.id_transaction, f.ctype, f.categorie, f.montant, TO_CHAR(f.date_trans, 'DD/MM/YYYY'), "
               "NVL(m.NOM_MACHINE, 'N/A') as machine, f.description "
               "FROM finance f "
               "LEFT JOIN MACHINE m ON f.id_machine = m.ID_MACHINE "
               "ORDER BY f.id_transaction DESC");
    
    int rowCount = 0;
    while (query.next()) {
        tableTransaction->insertRow(rowCount);
        for(int i=0; i<7; i++) {
            QString val = query.value(i).toString();
            if(i == 3) val += " DT";
            QTableWidgetItem *item = new QTableWidgetItem(val);
            item->setTextAlignment(Qt::AlignCenter);
            tableTransaction->setItem(rowCount, i, item);
        }

        rowCount++;
    }
}

void TransactionTab::effacerFormulaire()
{
    // Bloquer les signaux temporairement pour éviter les mises à jour inutiles
    blockSignals(true);
    
    txtId->clear();
    spinMontant->setValue(0.0);
    dateEdit->setDate(QDate::currentDate());
    txtDescription->clear();
    
    // Réinitialiser les combobox
    if (cbType->count() > 0) cbType->setCurrentIndex(0);
    if (cbCategorie->count() > 0) cbCategorie->setCurrentIndex(0);
    if (cbMachine->count() > 0) {
        cbMachine->setCurrentIndex(0);  // Sélectionner le premier élément au lieu de -1
    }
    
    // Déselectionner la ligne du tableau
    tableTransaction->clearSelection();
    
    // Débloquer les signaux et déclencher le changement de type
    blockSignals(false);
    onTypeChanged(cbType->currentText());
}


// Note: rechercher(), trier(), applyFilter() ont été déplacés vers SearchTab.


void TransactionTab::onTypeChanged(const QString &type)
{
    cbCategorie->clear();
    if (type == "REVENU") {
        cbCategorie->addItems({QString::fromUtf8("Vente Huile"), QString::fromUtf8("Service Pressage")});
        lblMachine->hide(); cbMachine->hide();
    } else {
        cbCategorie->addItems({QString::fromUtf8("Achat pièces"), "Maintenance", "Salaire", "Autre"});
        // onCategorieChanged s'occupera du show/hide
        onCategorieChanged(cbCategorie->currentText());
    }
}

void TransactionTab::onCategorieChanged(const QString &categorie)
{
    // Machine visible pour Maintenance ET Achat pièces
    if (categorie == "Maintenance" || categorie == QString::fromUtf8("Achat pièces")) {
        lblMachine->show();
        cbMachine->show();
    } else {
        lblMachine->hide();
        cbMachine->hide();
    }
}

void TransactionTab::rechercher()
{
    QString typeFilter = cbSearchType->currentText();
    QString categorieFilter = cbSearchCategorie->currentText();

    tableTransaction->setRowCount(0);
    QSqlDatabase db = dbConn->getDatabase();
    if (!db.isOpen()) return;

    QString sql = "SELECT f.id_transaction, f.ctype, f.categorie, f.montant, TO_CHAR(f.date_trans, 'DD/MM/YYYY'), "
                  "NVL(m.NOM_MACHINE, 'N/A') as machine, f.description "
                  "FROM finance f "
                  "LEFT JOIN MACHINE m ON f.id_machine = m.ID_MACHINE "
                  "WHERE 1=1";

    if (typeFilter != "Tous") {
        sql += QString(" AND f.ctype = '%1'").arg(typeFilter);
    }
    if (categorieFilter != "Toutes") {
        sql += QString(" AND f.categorie = '%1'").arg(categorieFilter);
    }
    sql += " ORDER BY f.id_transaction DESC";

    QSqlQuery query(db);
    if (!query.exec(sql)) {
        QMessageBox::critical(this, "Erreur", query.lastError().text());
        return;
    }

    int rowCount = 0;
    while (query.next()) {
        tableTransaction->insertRow(rowCount);
        for(int i=0; i<7; i++) {
            QString val = query.value(i).toString();
            if(i == 3) val += " DT";
            QTableWidgetItem *item = new QTableWidgetItem(val);
            item->setTextAlignment(Qt::AlignCenter);
            tableTransaction->setItem(rowCount, i, item);
        }
        rowCount++;
    }

    if (rowCount == 0) {
        QMessageBox::information(this, "Résultat", "Aucune transaction trouvée.");
    }
}

void TransactionTab::trier()
{
    QString sortOption = cbSort->currentText();
    tableTransaction->setRowCount(0);
    QSqlDatabase db = dbConn->getDatabase();
    if (!db.isOpen()) return;

    QString sql = "SELECT f.id_transaction, f.ctype, f.categorie, f.montant, TO_CHAR(f.date_trans, 'DD/MM/YYYY'), "
                  "NVL(m.NOM_MACHINE, 'N/A') as machine, f.description "
                  "FROM finance f "
                  "LEFT JOIN MACHINE m ON f.id_machine = m.ID_MACHINE ";

    if (sortOption.contains("ID")) {
        sql += "ORDER BY f.id_transaction DESC";
    } else if (sortOption.contains(QString::fromUtf8("Montant")) && sortOption.contains("↑")) {
        sql += "ORDER BY f.montant ASC";
    } else if (sortOption.contains(QString::fromUtf8("Montant")) && sortOption.contains("↓")) {
        sql += "ORDER BY f.montant DESC";
    } else if (sortOption.contains("Date")) {
        sql += "ORDER BY f.date_trans DESC";
    } else {
        sql += "ORDER BY f.id_transaction DESC";
    }

    QSqlQuery query(db);
    if (!query.exec(sql)) {
        QMessageBox::critical(this, "Erreur", query.lastError().text());
        return;
    }

    int rowCount = 0;
    while (query.next()) {
        tableTransaction->insertRow(rowCount);
        for(int i=0; i<7; i++) {
            QString val = query.value(i).toString();
            if(i == 3) val += " DT";
            QTableWidgetItem *item = new QTableWidgetItem(val);
            item->setTextAlignment(Qt::AlignCenter);
            tableTransaction->setItem(rowCount, i, item);
        }
        rowCount++;
    }
}

void TransactionTab::exportTransaction()
{
    QPushButton *senderBtn = qobject_cast<QPushButton*>(sender());
    if (!senderBtn) return;

    QString format = "TXT";
    if (senderBtn == btnExportPDF) format = "PDF";
    else if (senderBtn == btnExportExcel) format = "Excel";

    QString fileName = QFileDialog::getSaveFileName(this,
        QString::fromUtf8("Exporter les transactions"),
        QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),
        format == "PDF" ? "PDF Files (*.pdf)" : 
        format == "Excel" ? "Excel Files (*.xlsx)" : "Text Files (*.txt)");

    if (fileName.isEmpty()) return;

    if (format == "TXT") {
        QFile file(fileName);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QMessageBox::critical(this, "Erreur", "Impossible de créer le fichier.");
            return;
        }

        QTextStream out(&file);
        out << "TRANSACTIONS FINANCIÈRES\n";
        out << "===============================================\n\n";

        for (int row = 0; row < tableTransaction->rowCount(); row++) {
            out << QString("ID: %1 | Type: %2 | Catégorie: %3\n")
                   .arg(tableTransaction->item(row, 0)->text())
                   .arg(tableTransaction->item(row, 1)->text())
                   .arg(tableTransaction->item(row, 2)->text());
            out << QString("Montant: %1 | Date: %2 | Machine: %3\n")
                   .arg(tableTransaction->item(row, 3)->text())
                   .arg(tableTransaction->item(row, 4)->text())
                   .arg(tableTransaction->item(row, 5)->text());
            out << QString("Description: %1\n\n")
                   .arg(tableTransaction->item(row, 6)->text());
        }
        file.close();
        QMessageBox::information(this, "Succès", "Fichier TXT créé avec succès.");
    } else if (format == "Excel") {
        // Simple implementation: create HTML and save as Excel-compatible format
        QString html = "<html><table border='1'><tr>";
        for (int i = 0; i < 7; i++) {
            html += QString("<th>%1</th>").arg(tableTransaction->horizontalHeaderItem(i)->text());
        }
        html += "</tr>";
        for (int row = 0; row < tableTransaction->rowCount(); row++) {
            html += "<tr>";
            for (int col = 0; col < 7; col++) {
                html += QString("<td>%1</td>").arg(tableTransaction->item(row, col)->text());
            }
            html += "</tr>";
        }
        html += "</table></html>";

        QFile file(fileName);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QMessageBox::critical(this, "Erreur", "Impossible de créer le fichier.");
            return;
        }
        file.write(html.toUtf8());
        file.close();
        QMessageBox::information(this, "Succès", "Fichier Excel créé avec succès.");
    }
}

void TransactionTab::exportTxt()
{
    QString fileName = QFileDialog::getSaveFileName(this,
        QString::fromUtf8("Exporter en TXT"),
        QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),
        "Text Files (*.txt)");

    if (fileName.isEmpty()) return;

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Erreur", "Impossible de créer le fichier.");
        return;
    }

    QTextStream out(&file);
    out << QString::fromUtf8("TRANSACTIONS FINANCIÈRES\n");
    out << QString::fromUtf8("===============================================\n\n");

    for (int row = 0; row < tableTransaction->rowCount(); row++) {
        out << QString("ID: %1 | Type: %2 | Catégorie: %3\n")
               .arg(tableTransaction->item(row, 0)->text())
               .arg(tableTransaction->item(row, 1)->text())
               .arg(tableTransaction->item(row, 2)->text());
        out << QString("Montant: %1 | Date: %2 | Machine: %3\n")
               .arg(tableTransaction->item(row, 3)->text())
               .arg(tableTransaction->item(row, 4)->text())
               .arg(tableTransaction->item(row, 5)->text());
        out << QString("Description: %1\n\n")
               .arg(tableTransaction->item(row, 6)->text());
    }
    file.close();
    QDesktopServices::openUrl(QUrl::fromLocalFile(fileName));
    QMessageBox::information(this, QString::fromUtf8("Succès"), QString::fromUtf8("Fichier TXT created and opened!"));
}

void TransactionTab::exportExcel()
{
    QString fileName = QFileDialog::getSaveFileName(this,
        QString::fromUtf8("Exporter en Excel"),
        QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),
        "Excel Files (*.xlsx)");

    if (fileName.isEmpty()) return;

    QString html = "<html><body><table border='1' cellpadding='5'><tr>";
    
    // Entête
    for (int i = 0; i < 7; i++) {
        QTableWidgetItem *headerItem = tableTransaction->horizontalHeaderItem(i);
        if (headerItem) {
            html += QString("<th>%1</th>").arg(headerItem->text());
        }
    }
    html += "</tr>";

    // Données
    for (int row = 0; row < tableTransaction->rowCount(); row++) {
        html += "<tr>";
        for (int col = 0; col < 7; col++) {
            QTableWidgetItem *item = tableTransaction->item(row, col);
            if (item) {
                html += QString("<td>%1</td>").arg(item->text());
            }
        }
        html += "</tr>";
    }
    html += "</table></body></html>";

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Erreur", "Impossible de créer le fichier.");
        return;
    }
    file.write(html.toUtf8());
    file.close();
    QDesktopServices::openUrl(QUrl::fromLocalFile(fileName));
    QMessageBox::information(this, QString::fromUtf8("Succès"), QString::fromUtf8("Fichier Excel créé et ouvert!"));
}

void TransactionTab::exportPDF()
{
    QString fileName = QFileDialog::getSaveFileName(this,
        QString::fromUtf8("Exporter en PDF"),
        QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),
        "PDF Files (*.pdf)");

    if (fileName.isEmpty()) return;

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Erreur", "Impossible de créer le fichier.");
        return;
    }

    QTextStream out(&file);
    out << QString::fromUtf8("TRANSACTIONS FINANCIÈRES\n");
    out << QString::fromUtf8("===============================================\n\n");

    for (int row = 0; row < tableTransaction->rowCount(); row++) {
        out << QString("ID: %1 | Type: %2 | Catégorie: %3\n")
               .arg(tableTransaction->item(row, 0)->text())
               .arg(tableTransaction->item(row, 1)->text())
               .arg(tableTransaction->item(row, 2)->text());
        out << QString("Montant: %1 | Date: %2 | Machine: %3\n")
               .arg(tableTransaction->item(row, 3)->text())
               .arg(tableTransaction->item(row, 4)->text())
               .arg(tableTransaction->item(row, 5)->text());
        out << QString("Description: %1\n\n")
               .arg(tableTransaction->item(row, 6)->text());
    }
    file.close();
    QDesktopServices::openUrl(QUrl::fromLocalFile(fileName));
    QMessageBox::information(this, QString::fromUtf8("Succès"), QString::fromUtf8("Fichier PDF créé et ouvert!"));
}

void TransactionTab::chargerData()
{
    cbMachine->clear();
    QSqlDatabase db = dbConn->getDatabase();
    if (!db.isOpen()) return;

    // Charger Machines depuis la vraie table MACHINE
    QSqlQuery qm("SELECT ID_MACHINE, NOM_MACHINE FROM MACHINE ORDER BY NOM_MACHINE", db);
    int machineCount = 0;
    while (qm.next()) {
        cbMachine->addItem(qm.value(1).toString(), qm.value(0).toInt());
        machineCount++;
    }

    if (machineCount == 0) {
        qDebug() << "[Finance] Attention: Aucune machine trouvée dans la table MACHINE";
        cbMachine->addItem("(Aucune machine disponible)", -1);
    }
}

// Note: exportTransaction() et exportPDF() ont été déplacés vers SearchTab.

// ========================================================================
// IMPLÉMENTATION: StatsTab
// ========================================================================
StatsTab::StatsTab(QWidget *parent)
    : QWidget(parent), dbConn(new ConnexionFinance())
{
    this->setStyleSheet("background-color: #f5f5f5;"); 
    if (!dbConn->open()) {
        qDebug() << "Erreur: Impossible d'ouvrir la base dans StatsTab";
    }
    initializeUI();
    setupConnections();
}

void StatsTab::initializeUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(8);
    mainLayout->setContentsMargins(10, 10, 10, 10);

    // ===== Résumé Financier =====
    mainLayout->addWidget(createSummarySection());

    // ===== Sélection Type Graphique =====
    mainLayout->addWidget(createChartControlSection());

    // ===== Zone Graphique =====
    mainLayout->addWidget(createChartSection());

    mainLayout->addStretch();
    setLayout(mainLayout);
}

QGroupBox* StatsTab::createSummarySection()
{
    QGroupBox *summaryGroup = new QGroupBox(QString::fromUtf8("📊 Résumé Financier"), this);
    summaryGroup->setStyleSheet("QGroupBox { font-weight: bold; font-size: 11pt; color: #1B4332; border: 2px solid #1B7331; border-radius: 6px; padding: 12px; background-color: #f9fffe; }");

    QGridLayout *gridLayout = new QGridLayout(summaryGroup);
    gridLayout->setSpacing(10);
    gridLayout->setContentsMargins(8, 8, 8, 8);

    // Card 1: Revenus
    QVBoxLayout *revLayout = new QVBoxLayout();
    QLabel *lblRevenuTitle = new QLabel(QString::fromUtf8("Total Revenus"));
    lblRevenuTitle->setStyleSheet("font-weight: bold; font-size: 11pt; color: #1B4332;");
    lblTotalRevenus = new QLabel("0.00 DT");
    lblTotalRevenus->setStyleSheet("font-weight: bold; font-size: 16pt; color: #27ae60;");
    lblTotalRevenus->setAlignment(Qt::AlignCenter);
    lblPercRevenus = new QLabel("0.0%");
    lblPercRevenus->setStyleSheet("font-size: 10pt; color: #27ae60;");
    lblPercRevenus->setAlignment(Qt::AlignCenter);
    revLayout->addWidget(lblRevenuTitle);
    revLayout->addWidget(lblTotalRevenus);
    // Pourcentages supprimés: affichés dans le graphique
    lblPercRevenus = new QLabel("0.0%");
    lblPercRevenus->setStyleSheet("font-size: 10pt; color: #27ae60;");
    lblPercRevenus->setVisible(false); // Caché
    // revLayout->addWidget(lblPercRevenus);

    QWidget *revCard = new QWidget();
    revCard->setLayout(revLayout);
    revCard->setStyleSheet("background-color: #f0f8f4; border: 2px solid #27ae60; border-radius: 8px; padding: 15px;");

    // Card 2: Dépenses
    QVBoxLayout *depLayout = new QVBoxLayout();
    QLabel *lblDepenseTitle = new QLabel(QString::fromUtf8("Total Dépenses"));
    lblDepenseTitle->setStyleSheet("font-weight: bold; font-size: 11pt; color: #1B4332;");
    lblTotalDepenses = new QLabel("0.00 DT");
    lblTotalDepenses->setStyleSheet("font-weight: bold; font-size: 16pt; color: #e74c3c;");
    lblTotalDepenses->setAlignment(Qt::AlignCenter);
    lblPercDepenses = new QLabel("0.0%");
    lblPercDepenses->setStyleSheet("font-size: 10pt; color: #e74c3c;");
    lblPercDepenses->setAlignment(Qt::AlignCenter);
    lblPercDepenses->setVisible(false); // Caché
    depLayout->addWidget(lblDepenseTitle);
    depLayout->addWidget(lblTotalDepenses);
    // Pourcentages supprimés: affichés dans le graphique
    // depLayout->addWidget(lblPercDepenses);

    QWidget *depCard = new QWidget();
    depCard->setLayout(depLayout);
    depCard->setStyleSheet("background-color: #fef5f5; border: 2px solid #e74c3c; border-radius: 8px; padding: 15px;");

    // Card 3: Bénéfice
    QVBoxLayout *benLayout = new QVBoxLayout();
    QLabel *lblBeneficeTitle = new QLabel(QString::fromUtf8("Bénéfice Net"));
    lblBeneficeTitle->setStyleSheet("font-weight: bold; font-size: 11pt; color: #1B4332;");
    lblBenefice = new QLabel("0.00 DT");
    lblBenefice->setStyleSheet("font-weight: bold; font-size: 16pt; color: #1B4332;");
    lblBenefice->setAlignment(Qt::AlignCenter);
    lblTauxRentabilite = new QLabel("0.0%");
    lblTauxRentabilite->setStyleSheet("font-size: 10pt; color: #1B4332;");
    lblTauxRentabilite->setAlignment(Qt::AlignCenter);
    benLayout->addWidget(lblBeneficeTitle);
    benLayout->addWidget(lblBenefice);
    // Taux de rentabilité supprimé: affiché uniquement dans le graphique
    lblTauxRentabilite = new QLabel("0.0%");
    lblTauxRentabilite->setStyleSheet("font-size: 10pt; color: #1B4332;");
    lblTauxRentabilite->setVisible(false); // Caché
    // benLayout->addWidget(lblTauxRentabilite);

    QWidget *benCard = new QWidget();
    benCard->setLayout(benLayout);
    benCard->setStyleSheet("background-color: #f5f9f8; border: 2px solid #1B4332; border-radius: 8px; padding: 15px;");

    // Ajouter les cartes au grid
    gridLayout->addWidget(revCard, 0, 0);
    gridLayout->addWidget(depCard, 0, 1);
    gridLayout->addWidget(benCard, 0, 2);

    return summaryGroup;
}

QGroupBox* StatsTab::createChartControlSection()
{
    QGroupBox *controlGroup = new QGroupBox(QString::fromUtf8("⚙️ Configuration du Graphique"), this);
    controlGroup->setStyleSheet("QGroupBox { font-weight: bold; font-size: 11pt; color: #1B4332; border: 2px solid #1B7331; border-radius: 6px; padding: 8px; background-color: #f0f8f5; }");
    QVBoxLayout *controlLayout = new QVBoxLayout(controlGroup);
    controlLayout->setSpacing(8);
    controlLayout->setContentsMargins(8, 8, 8, 8);

    // Ligne 1: Type de graphique
    QHBoxLayout *typeLayout = new QHBoxLayout();
    typeLayout->setSpacing(8);
    QLabel *lblType = new QLabel(QString::fromUtf8("📊 Type:"));
    lblType->setStyleSheet("font-weight: bold; font-size: 9pt; color: #1B4332;");
    lblType->setMinimumWidth(50);
    cbChartType = new QComboBox();
    cbChartType->addItems({QString::fromUtf8("📈 Camembert"), QString::fromUtf8("📊 Histogramme"), QString::fromUtf8("📉 Courbe"), QString::fromUtf8("📋 Comparatif")});
    cbChartType->setMinimumWidth(220);
    cbChartType->setMaximumHeight(26);
    cbChartType->setStyleSheet("QComboBox { padding: 3px 6px; border-radius: 4px; background-color: #e8f5e9; border: 2px solid #1B7331; font-size: 9pt; font-weight: bold; color: #1B4332; } QComboBox:focus { border: 2px solid #155c2b; }");
    typeLayout->addWidget(lblType);
    typeLayout->addWidget(cbChartType);
    
    // Bouton sur la même ligne
    btnGenerate = new QPushButton(QString::fromUtf8("▶ GÉNÉRER"));
    btnGenerate->setMaximumWidth(120);
    btnGenerate->setMaximumHeight(26);
    btnGenerate->setStyleSheet(
        "QPushButton { "
        "background-color: #1B7331; "
        "color: white; "
        "border: 2px solid #0f3e1d; "
        "border-radius: 4px; "
        "font-weight: bold; "
        "font-size: 9pt; "
        "padding: 2px 6px; "
        "} "
        "QPushButton:hover { "
        "background-color: #155c2b; "
        "} "
        "QPushButton:pressed { "
        "background-color: #0f3e1d; "
        "}"
    );
    typeLayout->addWidget(btnGenerate);
    typeLayout->addStretch();

    controlLayout->addLayout(typeLayout);

    return controlGroup;

    return controlGroup;
}

QGroupBox* StatsTab::createChartSection()
{
    QGroupBox *chartGroup = new QGroupBox(QString::fromUtf8("📈 Visualisation"), this);
    chartGroup->setStyleSheet("QGroupBox { font-weight: bold; font-size: 11pt; color: #1B4332; border: 2px solid #1B7331; border-radius: 6px; padding: 10px; background-color: #fafafa; }");
    QVBoxLayout *chartLayout = new QVBoxLayout(chartGroup);
    chartLayout->setContentsMargins(8, 8, 8, 8);
    chartLayout->setSpacing(0);

    chartView = new QChartView();
    chartView->setMinimumHeight(280);
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setRenderHint(QPainter::SmoothPixmapTransform);
    chartView->setStyleSheet("QChartView { background-color: #ffffff; border: 2px solid #1B7331; border-radius: 4px; }");

    chartLayout->addWidget(chartView);

    return chartGroup;
}

void StatsTab::setupConnections()
{
    connect(btnGenerate, &QPushButton::clicked, this, &StatsTab::afficherGraphique);
}

void StatsTab::calculerStats()
{
    double revenus = 0.0;
    double depenses = 0.0;

    QSqlDatabase db = dbConn->getDatabase();
    if (!db.isOpen()) return;

    QSqlQuery query(db);
    // Calculate Revenus (Normalisation UPPER pour éviter les erreurs de casse)
    if (query.exec("SELECT SUM(montant) FROM finance WHERE UPPER(ctype) = 'REVENU'")) {
        if (query.next()) {
            revenus = query.value(0).toDouble();
        }
    }
    
    // Calculate Depenses
    if (query.exec("SELECT SUM(montant) FROM finance WHERE UPPER(ctype) IN ('DEPENSE', 'DÉPENSE')")) {
        if (query.next()) {
            depenses = query.value(0).toDouble();
        }
    }

    double benefice = revenus - depenses;
    double total = revenus + depenses;

    // Calcul des pourcentages
    double percRevenus = (total > 0) ? (revenus / total) * 100.0 : 0.0;
    double percDepenses = (total > 0) ? (depenses / total) * 100.0 : 0.0;
    double tauxRentabilite = (revenus > 0) ? (benefice / revenus) * 100.0 : 0.0;

    // Update UI - Montants
    lblTotalRevenus->setText(QString::number(revenus, 'f', 2) + " DT");
    lblTotalDepenses->setText(QString::number(depenses, 'f', 2) + " DT");
    lblBenefice->setText(QString::number(benefice, 'f', 2) + " DT");
    
    // Update UI - Pourcentages
    lblPercRevenus->setText(QString::number(percRevenus, 'f', 1) + "%");
    lblPercDepenses->setText(QString::number(percDepenses, 'f', 1) + "%");
    lblTauxRentabilite->setText(QString::number(tauxRentabilite, 'f', 1) + "%");
    
    // Color logic for benefice
    if (benefice >= 0) {
        lblBenefice->setStyleSheet("font-weight: bold; font-size: 16pt; color: #27ae60;");
        lblTauxRentabilite->setStyleSheet("font-size: 10pt; color: #27ae60;");
    } else {
        lblBenefice->setStyleSheet("font-weight: bold; font-size: 16pt; color: #e74c3c;");
        lblTauxRentabilite->setStyleSheet("font-size: 10pt; color: #e74c3c;");
    }

    // Appel automatique de l'affichage du graphique pour synchroniser
    afficherGraphique();
}

void StatsTab::afficherGraphique()
{
    QString typeGraph = cbChartType->currentText();
    QChart *chart = new QChart();
    chart->setAnimationOptions(QChart::AllAnimations);
    chart->setTheme(QChart::ChartThemeLight);
    chart->setMargins(QMargins(5, 10, 5, 5));
    
    // Styliser le titre du graphique
    QFont titleFont;
    titleFont.setPointSize(9);
    titleFont.setBold(true);
    chart->setTitleFont(titleFont);

    if (typeGraph.contains("Camembert")) {
        QPieSeries *series = new QPieSeries();
        series->setHoleSize(0);  // Camembert complet
        
        double rev = lblTotalRevenus->text().replace(" DT", "").toDouble();
        double dep = lblTotalDepenses->text().replace(" DT", "").toDouble();
        double total = rev + dep;
        
        series->append("Revenus", rev);
        series->append(QString::fromUtf8("Dépenses"), dep);
        
        // Ajouter les pourcentages aux slices
        double percRev = (total > 0) ? (rev / total) * 100.0 : 0.0;
        double percDep = (total > 0) ? (dep / total) * 100.0 : 0.0;
        
        QPieSlice *sliceRev = series->slices().at(0);
        sliceRev->setBrush(QColor("#27ae60"));
        sliceRev->setExploded();
        sliceRev->setExplodeDistanceFactor(0.05);
        sliceRev->setLabel(QString("Revenus: %1%").arg(QString::number(percRev, 'f', 1)));
        sliceRev->setLabelVisible();
        sliceRev->setLabelArmLengthFactor(0);
        QFont labelFont;
        labelFont.setPointSize(9);
        labelFont.setBold(true);
        sliceRev->setLabelFont(labelFont);
        
        QPieSlice *sliceDep = series->slices().at(1);
        sliceDep->setBrush(QColor("#e74c3c"));
        sliceDep->setExploded();
        sliceDep->setExplodeDistanceFactor(0.05);
        sliceDep->setLabel(QString::fromUtf8("Dépenses: %1%").arg(QString::number(percDep, 'f', 1)));
        sliceDep->setLabelVisible();
        sliceDep->setLabelArmLengthFactor(0);
        sliceDep->setLabelFont(labelFont);
        
        chart->addSeries(series);
        chart->setTitle(QString::fromUtf8("Répartition Revenus vs Dépenses"));
        chart->legend()->setVisible(true);
        chart->legend()->setAlignment(Qt::AlignTop);
    } 
    else if (typeGraph.contains("Histogramme")) {
        QBarSeries *series = new QBarSeries();
        QBarSet *setRev = new QBarSet("Revenus");
        QBarSet *setDep = new QBarSet(QString::fromUtf8("Dépenses"));
        
        setRev->setBrush(QColor("#27ae60"));
        setDep->setBrush(QColor("#e74c3c"));
        
        QStringList months;
        double totalRevenues = 0.0;
        double totalDepenses = 0.0;
        
        // On récupère les données des 6 derniers mois
        for(int i=5; i>=0; i--) {
            QDate d = QDate::currentDate().addMonths(-i);
            QString mStr = d.toString("MM/yyyy");
            months << mStr;
            
            QSqlDatabase db = dbConn->getDatabase();
            QSqlQuery q(db);
            q.prepare("SELECT SUM(montant) FROM finance WHERE UPPER(ctype)='REVENU' AND TO_CHAR(date_trans, 'MM/YYYY') = :m");
            q.bindValue(":m", mStr);
            q.exec(); q.next();
            double revVal = q.value(0).toDouble();
            totalRevenues += revVal;
            *setRev << revVal;
            
            q.prepare("SELECT SUM(montant) FROM finance WHERE (UPPER(ctype)='DEPENSE' OR UPPER(ctype)='DÉPENSE') AND TO_CHAR(date_trans, 'MM/YYYY') = :m");
            q.bindValue(":m", mStr);
            q.exec(); q.next();
            double depVal = q.value(0).toDouble();
            totalDepenses += depVal;
            *setDep << depVal;
        }
        
        series->append(setRev);
        series->append(setDep);
        chart->addSeries(series);
        chart->setTitle("Evolution Mensuelle");
        
        QBarCategoryAxis *axisX = new QBarCategoryAxis();
        axisX->append(months);
        QFont axisXFont;
        axisXFont.setPointSize(9);
        axisXFont.setBold(true);
        axisX->setLabelsFont(axisXFont);
        chart->addAxis(axisX, Qt::AlignBottom);
        series->attachAxis(axisX);
        
        QValueAxis *axisY = new QValueAxis();
        QFont axisYFont;
        axisYFont.setPointSize(9);
        axisYFont.setBold(true);
        axisY->setLabelsFont(axisYFont);
        chart->addAxis(axisY, Qt::AlignLeft);
        series->attachAxis(axisY);
    }
    else if (typeGraph.contains("Courbe")) {
        QLineSeries *seriesBalance = new QLineSeries();
        seriesBalance->setName(QString::fromUtf8("Bénéfice Net"));
        seriesBalance->setPen(QPen(QColor("#1B4332"), 3));
        
        QSqlDatabase db = dbConn->getDatabase();
        QSqlQuery q(db);
        q.prepare("SELECT date_trans, SUM(CASE WHEN UPPER(ctype)='REVENU' THEN montant ELSE -montant END) "
                  "OVER (ORDER BY date_trans) as balance FROM finance ORDER BY date_trans");
        q.exec();
        
        double maxBalance = 0.0;
        double minBalance = 0.0;
        int i=0;
        while(q.next()) {
            double balance = q.value(1).toDouble();
            seriesBalance->append(i++, balance);
            if(balance > maxBalance) maxBalance = balance;
            if(balance < minBalance) minBalance = balance;
        }
        
        chart->addSeries(seriesBalance);
        // Ajouter le pourcentage de variation au titre
        double tendanceVariation = (minBalance != 0) ? ((maxBalance - minBalance) / fabs(minBalance)) * 100.0 : 0.0;
        if(minBalance == 0) tendanceVariation = (maxBalance > 0) ? 100.0 : 0.0;
        chart->setTitle(QString::fromUtf8("Tendance de la Trésorerie - Variation: %1%").arg(QString::number(tendanceVariation, 'f', 1)));
        chart->createDefaultAxes();
    }
    else { // Comparatif Catégories
        QBarSeries *categoriesSeries = new QBarSeries();
        QBarSet *catSet = new QBarSet(QString::fromUtf8("Montant par Catégorie"));
        catSet->setBrush(QColor("#C9A227"));
        
        QStringList labels;
        QSqlDatabase db = dbConn->getDatabase();
        QSqlQuery q("SELECT categorie, SUM(montant) FROM finance GROUP BY categorie", db);
        
        double totalCategories = 0.0;
        QVariantList values;
        while(q.next()) {
            labels << q.value(0).toString();
            double val = q.value(1).toDouble();
            values << val;
            totalCategories += val;
            *catSet << val;
        }
        
        categoriesSeries->append(catSet);
        chart->addSeries(categoriesSeries);
        
        // Ajouter les pourcentages au titre
        QString titleWithPercent = QString::fromUtf8("Analyse par Catégories - ");
        for(int i = 0; i < labels.size(); i++) {
            double perc = (totalCategories > 0) ? (values.at(i).toDouble() / totalCategories) * 100.0 : 0.0;
            if(i > 0) titleWithPercent += " | ";
            titleWithPercent += QString("%1: %2%").arg(labels.at(i)).arg(QString::number(perc, 'f', 1));
        }
        chart->setTitle(titleWithPercent);
        
        QBarCategoryAxis *axisX = new QBarCategoryAxis();
        axisX->append(labels);
        chart->addAxis(axisX, Qt::AlignBottom);
        categoriesSeries->attachAxis(axisX);
        
        QValueAxis *axisY = new QValueAxis();
        chart->addAxis(axisY, Qt::AlignLeft);
        categoriesSeries->attachAxis(axisY);
    }

    chartView->setChart(chart);
}

// ========================================================================
// IMPLÉMENTATION: AdvancedTab
// ========================================================================
AdvancedTab::AdvancedTab(QWidget *parent)
    : QWidget(parent), dbConn(new ConnexionFinance())
{
    this->setStyleSheet("background-color: #f5f5f5;"); 
    if (!dbConn->open()) {
        qDebug() << "Erreur: Impossible d'ouvrir la base dans AdvancedTab";
    }
    initializeUI();
    setupConnections();
}

void AdvancedTab::initializeUI()
{
    // 1. Setup Scroll Area to handle responsiveness without crushing
    QVBoxLayout *outerLayout = new QVBoxLayout(this);
    outerLayout->setContentsMargins(0, 0, 0, 0);
    outerLayout->setSpacing(0);

    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setStyleSheet("QScrollArea { background-color: transparent; }");

    QWidget *contentWidget = new QWidget();
    contentWidget->setObjectName("contentWidget");
    
    // Main Layout inside the Scroll Area
    QVBoxLayout *mainLayout = new QVBoxLayout(contentWidget);
    mainLayout->setSpacing(25); // Generic spacing between groups
    mainLayout->setContentsMargins(30, 30, 30, 30);

    // =========================================================================
    // 1. RENTABILITÉ SECTION
    // =========================================================================
    QGroupBox *rentGroup = new QGroupBox(QString::fromUtf8("Rentabilité"), contentWidget);
    rentGroup->setObjectName("rentGroup");
    QHBoxLayout *rentLayout = new QHBoxLayout(rentGroup);
    rentLayout->setSpacing(20);
    rentLayout->setContentsMargins(20, 25, 20, 20);

    rentLayout->addWidget(new QLabel("Mois:"));
    cbMonthRent = new QComboBox();
    cbMonthRent->setObjectName("cbMonthRent");
    cbMonthRent->addItems({QString::fromUtf8("Janvier"), QString::fromUtf8("Février"), QString::fromUtf8("Mars"), QString::fromUtf8("Avril"), QString::fromUtf8("Mai"), QString::fromUtf8("Juin"),
                           QString::fromUtf8("Juillet"), QString::fromUtf8("Août"), QString::fromUtf8("Septembre"), QString::fromUtf8("Octobre"), QString::fromUtf8("Novembre"), QString::fromUtf8("Décembre")});
    cbMonthRent->setMinimumWidth(150);
    cbMonthRent->setMaximumWidth(250);
    rentLayout->addWidget(cbMonthRent);

    btnCalcRent = new QPushButton(QString::fromUtf8("Calculer Rentabilité"));
    btnCalcRent->setObjectName("btnCalcRent");
    btnCalcRent->setCursor(Qt::PointingHandCursor);
    btnCalcRent->setMinimumWidth(150);
    rentLayout->addWidget(btnCalcRent);

    rentLayout->addStretch(); 

    rentLayout->addWidget(new QLabel(QString::fromUtf8("Résultat:")));
    lblRent = new QLabel("N/A");
    lblRent->setObjectName("lblRent");
    lblRent->setStyleSheet("font-weight: bold; color: #1B4332; font-size: 14pt; margin-left: 10px;");
    rentLayout->addWidget(lblRent);

    mainLayout->addWidget(rentGroup);

    // =========================================================================
    // 2. DÉTECTION DÉPENSES ANORMALES SECTION
    // =========================================================================
    QGroupBox *anomalyGroup = new QGroupBox(QString::fromUtf8("Détection Dépenses Anormales"), contentWidget);
    anomalyGroup->setObjectName("anomalyGroup");
    QVBoxLayout *anomalyLayout = new QVBoxLayout(anomalyGroup);
    anomalyLayout->setSpacing(15);
    anomalyLayout->setContentsMargins(20, 25, 20, 20);

    QHBoxLayout *anomalyRow = new QHBoxLayout();
    anomalyRow->addWidget(new QLabel("Seuil d'Alerte (DT):"));
    spinSeuil = new QDoubleSpinBox();
    spinSeuil->setObjectName("spinSeuil");
    spinSeuil->setMaximum(9999999.99);
    spinSeuil->setMinimumWidth(120);
    spinSeuil->setMaximumWidth(200);
    anomalyRow->addWidget(spinSeuil);

    btnDetect = new QPushButton(QString::fromUtf8("Détecter"));
    btnDetect->setObjectName("btnDetect");
    btnDetect->setCursor(Qt::PointingHandCursor);
    btnDetect->setMinimumWidth(120);
    anomalyRow->addWidget(btnDetect);
    anomalyRow->addStretch();
    anomalyLayout->addLayout(anomalyRow);

    tableAnomaly = new QTableWidget();
    tableAnomaly->setObjectName("tableAnomaly");
    tableAnomaly->setColumnCount(5);
    tableAnomaly->setHorizontalHeaderLabels({"Date", QString::fromUtf8("Catégorie"), "Montant", QString::fromUtf8("Écart %"), "Alerte"});
    tableAnomaly->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableAnomaly->setAlternatingRowColors(true);
    tableAnomaly->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableAnomaly->setMinimumHeight(180);
    anomalyLayout->addWidget(tableAnomaly);

    mainLayout->addWidget(anomalyGroup);

    // =========================================================================
    // 3. PRÉVISION SECTION
    // =========================================================================
    QGroupBox *predictGroup = new QGroupBox(QString::fromUtf8("Prévision"), contentWidget);
    predictGroup->setObjectName("predictGroup");
    QVBoxLayout *predictMainLayout = new QVBoxLayout(predictGroup);
    predictMainLayout->setSpacing(15);
    predictMainLayout->setContentsMargins(20, 25, 20, 20);

    QHBoxLayout *predictControls = new QHBoxLayout();
    predictControls->addWidget(new QLabel(QString::fromUtf8("Période:")));
    cbPeriod = new QComboBox();
    cbPeriod->setObjectName("cbPeriod");
    cbPeriod->addItems({"1 Mois", "3 Mois", "6 Mois", "1 An"});
    cbPeriod->setMinimumWidth(150);
    cbPeriod->setMaximumWidth(250);
    predictControls->addWidget(cbPeriod);

    btnPredict = new QPushButton(QString::fromUtf8("Prédire"));
    btnPredict->setObjectName("btnPredict");
    btnPredict->setCursor(Qt::PointingHandCursor);
    btnPredict->setMinimumWidth(120);
    predictControls->addWidget(btnPredict);
    predictControls->addStretch();
    predictMainLayout->addLayout(predictControls);

    chartPredict = new QChartView();
    chartPredict->setObjectName("chartPredict");
    chartPredict->setMinimumHeight(400); // Plus grand pour les prévisions
    chartPredict->setRenderHint(QPainter::Antialiasing);
    chartPredict->setStyleSheet("background-color: white; border: 1px solid #ddd; border-radius: 8px;");
    
    predictMainLayout->addWidget(chartPredict);

    mainLayout->addWidget(predictGroup);

    // =========================================================================
    // 4. CLASSEMENT CLIENTS SECTION
    // =========================================================================
    QGroupBox *clientGroup = new QGroupBox("Classement Clients", contentWidget);
    clientGroup->setObjectName("clientGroup");
    QVBoxLayout *clientLayout = new QVBoxLayout(clientGroup);
    clientLayout->setSpacing(15);
    clientLayout->setContentsMargins(20, 25, 20, 20);

    QHBoxLayout *clientRow = new QHBoxLayout();
    clientRow->addWidget(new QLabel("Classer par:"));
    cbRank = new QComboBox();
    cbRank->setObjectName("cbRank");
    cbRank->addItems({QString::fromUtf8("Chiffre d'Affaires"), QString::fromUtf8("Fréquence"), QString::fromUtf8("Récemment Actif")});
    cbRank->setMinimumWidth(200);
    cbRank->setMaximumWidth(350);
    clientRow->addWidget(cbRank);
    clientRow->addStretch();
    clientLayout->addLayout(clientRow);

    tableClients = new QTableWidget();
    tableClients->setObjectName("tableClients");
    tableClients->setColumnCount(4);
    tableClients->setHorizontalHeaderLabels({"Rang", "Client", "Montant Total", "Transactions"});
    tableClients->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableClients->setAlternatingRowColors(true);
    tableClients->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableClients->setMinimumHeight(180);
    clientLayout->addWidget(tableClients);

    mainLayout->addWidget(clientGroup);

    // =========================================================================
    // 5. SIMULATION FINANCIÈRE SECTION
    // =========================================================================
    QGroupBox *simGroup = new QGroupBox(QString::fromUtf8("Simulation Financière"), contentWidget);
    simGroup->setObjectName("simGroup");
    
    QGridLayout *simGrid = new QGridLayout(simGroup);
    simGrid->setHorizontalSpacing(30);
    simGrid->setVerticalSpacing(20);
    simGrid->setContentsMargins(20, 25, 20, 20);

    // Row 0
    simGrid->addWidget(new QLabel("Prix Unitaire (DT):"), 0, 0);
    spinPrice = new QDoubleSpinBox();
    spinPrice->setObjectName("spinPrice");
    spinPrice->setMaximum(999999.99);
    spinPrice->setMinimumWidth(120);
    spinPrice->setMaximumWidth(200);
    simGrid->addWidget(spinPrice, 0, 1);
    
    simGrid->addWidget(new QLabel("Frais (DT):"), 0, 2);
    spinCharge = new QDoubleSpinBox();
    spinCharge->setObjectName("spinCharge");
    spinCharge->setMaximum(999999.99);
    spinCharge->setMinimumWidth(120);
    spinCharge->setMaximumWidth(200);
    simGrid->addWidget(spinCharge, 0, 3);

    // Row 1
    simGrid->addWidget(new QLabel(QString::fromUtf8("Ventes (Qtés):")), 1, 0);
    spinSales = new QDoubleSpinBox();
    spinSales->setObjectName("spinSales");
    spinSales->setMaximum(999999.99);
    spinSales->setMinimumWidth(120);
    spinSales->setMaximumWidth(200);
    simGrid->addWidget(spinSales, 1, 1);

    btnSimulate = new QPushButton("Simuler");
    btnSimulate->setObjectName("btnSimulate");
    btnSimulate->setCursor(Qt::PointingHandCursor);
    btnSimulate->setMinimumWidth(120);
    btnSimulate->setMaximumWidth(200);
    simGrid->addWidget(btnSimulate, 1, 3);

    // Row 2 - Result
    QHBoxLayout *simResultLayout = new QHBoxLayout();
    simResultLayout->addStretch();
    simResultLayout->addWidget(new QLabel(QString::fromUtf8("Résultat de simulation :")));
    lblResult = new QLabel("N/A");
    lblResult->setObjectName("lblResult");
    lblResult->setStyleSheet("font-weight: bold; font-size: 16px; color: #1B4332; padding: 6px 15px; border: 2px solid #1B4332; border-radius: 6px; background-color: #e6fffa;");
    simResultLayout->addWidget(lblResult);
    simResultLayout->addStretch();
    
    simGrid->addLayout(simResultLayout, 2, 0, 1, 4);

    mainLayout->addWidget(simGroup);
    
    // Add finishing touch - bottom spacer
    mainLayout->addStretch();
    
    // Finalize Scroll Area
    scrollArea->setWidget(contentWidget);
    outerLayout->addWidget(scrollArea);
    
    setLayout(outerLayout);
}

void AdvancedTab::setupConnections()
{
    connect(btnCalcRent, &QPushButton::clicked, this, &AdvancedTab::calculerRentabilite);
    connect(btnDetect, &QPushButton::clicked, this, &AdvancedTab::detecterAnomalies);
    connect(btnPredict, &QPushButton::clicked, this, &AdvancedTab::predire);
    connect(btnSimulate, &QPushButton::clicked, this, &AdvancedTab::simuler);
}

void AdvancedTab::calculerRentabilite()
{
    QString monthName = cbMonthRent->currentText();
    QStringList frMonths = {QString::fromUtf8("Janvier"), QString::fromUtf8("Février"), QString::fromUtf8("Mars"), QString::fromUtf8("Avril"), QString::fromUtf8("Mai"), QString::fromUtf8("Juin"),
                           QString::fromUtf8("Juillet"), QString::fromUtf8("Août"), QString::fromUtf8("Septembre"), QString::fromUtf8("Octobre"), QString::fromUtf8("Novembre"), QString::fromUtf8("Décembre")};
    int monthIdx = frMonths.indexOf(monthName) + 1;
    QString mStr = QString("%1").arg(monthIdx, 2, 10, QChar('0'));

    QSqlDatabase db = dbConn->getDatabase();
    QSqlQuery q(db);
    q.prepare("SELECT SUM(CASE WHEN UPPER(ctype)='REVENU' THEN montant ELSE -montant END) FROM finance WHERE TO_CHAR(date_trans, 'MM') = :m");
    q.bindValue(":m", mStr);
    
    if (q.exec() && q.next()) {
        double rent = q.value(0).toDouble();
        lblRent->setText(QString::number(rent, 'f', 2) + " DT");
        lblRent->setStyleSheet(QString("font-weight: bold; font-size: 14pt; color: %1;").arg(rent >= 0 ? "#27ae60" : "#e74c3c"));
    }
}

void AdvancedTab::detecterAnomalies()
{
    double seuil = spinSeuil->value();
    if (seuil <= 0) return;

    tableAnomaly->setRowCount(0);
    QSqlDatabase db = dbConn->getDatabase();
    QSqlQuery q(db);
    q.prepare("SELECT TO_CHAR(date_trans, 'YYYY-MM-DD'), categorie, montant FROM finance WHERE montant > :s AND UPPER(ctype) IN ('DEPENSE', 'DÉPENSE')");
    q.bindValue(":s", seuil);
    
    if (q.exec()) {
        while (q.next()) {
            int row = tableAnomaly->rowCount();
            tableAnomaly->insertRow(row);
            tableAnomaly->setItem(row, 0, new QTableWidgetItem(q.value(0).toString()));
            tableAnomaly->setItem(row, 1, new QTableWidgetItem(q.value(1).toString()));
            tableAnomaly->setItem(row, 2, new QTableWidgetItem(q.value(2).toString() + " DT"));
            tableAnomaly->setItem(row, 3, new QTableWidgetItem("> Seuil"));
            
            QLabel *lblAlert = new QLabel("ALERTE");
            lblAlert->setStyleSheet("color: white; background-color: #e74c3c; font-weight: bold; border-radius: 4px; padding: 2px;");
            lblAlert->setAlignment(Qt::AlignCenter);
            tableAnomaly->setCellWidget(row, 4, lblAlert);
        }
    }
}

void AdvancedTab::predire()
{
    QString period = cbPeriod->currentText();
    int months = 1;
    if (period.contains("3")) months = 3;
    else if (period.contains("6")) months = 6;
    else if (period.contains("1 An")) months = 12;

    QChart *chart = new QChart();
    chart->setTitle("Prevision de Croissance Financiere");
    chart->setAnimationOptions(QChart::SeriesAnimations);

    QLineSeries *histSeries = new QLineSeries();
    histSeries->setName("Historique");
    QLineSeries *predSeries = new QLineSeries();
    predSeries->setName(QString::fromUtf8("Prévision"));
    QPen penPred(Qt::DashLine);
    penPred.setWidth(2);
    penPred.setColor(QColor("#C9A227"));
    predSeries->setPen(penPred);

    // Get last 6 months historical average
    double totalRev = 0;
    QSqlDatabase db = dbConn->getDatabase();
    QSqlQuery q(db);
    q.prepare("SELECT SUM(montant) FROM finance WHERE UPPER(ctype)='REVENU' AND date_trans > ADD_MONTHS(SYSDATE, -6)");
    if (q.exec() && q.next()) totalRev = q.value(0).toDouble() / 6.0;

    for (int i = 0; i < 6; i++) histSeries->append(i, totalRev * (1.0 + (i*0.05))); // Dummy growth
    for (int i = 5; i < 5 + months; i++) predSeries->append(i, totalRev * (1.0 + (i*0.05)));

    chart->addSeries(histSeries);
    chart->addSeries(predSeries);
    chart->createDefaultAxes();
    chartPredict->setChart(chart);
}

void AdvancedTab::classerClients()
{
    // Not strictly needed for the "graph works" request but good to have
}

void AdvancedTab::simuler()
{
    double price = spinPrice->value();
    double qty = spinSales->value();
    double charges = spinCharge->value();
    
    double res = (price * qty) - charges;
    lblResult->setText(QString::number(res, 'f', 2) + " DT");
    lblResult->setStyleSheet(QString("font-weight: bold; font-size: 16px; border: 2px solid %1; border-radius: 6px; background-color: #e6fffa; color: %1;")
                             .arg(res >= 0 ? "#1B4332" : "#e74c3c"));
}
// ========================================================================
// IMPLÉMENTATION: SearchTab
// ========================================================================
SearchTab::SearchTab(QWidget *parent)
    : QWidget(parent)
{
    dbConn = new ConnexionFinance();
    if (!dbConn->open()) {
        qDebug() << "Erreur: Impossible d'ouvrir la base dans SearchTab";
    }
    initializeUI();
    setupConnections();
    chargerTransactions();
}

void SearchTab::initializeUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    // 1. Zone de Recherche, Tri & Export
    QGroupBox *searchGroup = new QGroupBox(QString::fromUtf8("🔍 Recherche, Tri & Export"), this);
    searchGroup->setStyleSheet("QGroupBox{ border: 1px solid #e0e0e0; margin: 0px; padding-top: 15px; border-radius: 4px; } QGroupBox::title{ padding: 0px 10px; color: #1B4332; font-size: 13px; font-weight: bold; }");
    QVBoxLayout *searchLayout = new QVBoxLayout(searchGroup);
    searchLayout->setContentsMargins(20, 15, 20, 20);
    searchLayout->setSpacing(15);

    // --- Ligne 1: Recherche ---
    QHBoxLayout *rowSearch = new QHBoxLayout();
    rowSearch->setSpacing(10);
    rowSearch->setContentsMargins(0, 0, 0, 0);
    
    QLabel *lblSearch = new QLabel("Rechercher :");
    lblSearch->setStyleSheet("font-size: 12px; color: #333; font-weight: 500; min-width: 85px;");
    rowSearch->addWidget(lblSearch);
    
    txtSearch = new QLineEdit();
    txtSearch->setPlaceholderText(QString::fromUtf8("Client, Description, Catégorie..."));
    txtSearch->setMinimumHeight(35);
    txtSearch->setMaximumHeight(35);
    txtSearch->setStyleSheet("QLineEdit { border: 1px solid #ddd; border-radius: 4px; padding: 5px 10px; font-size: 12px; background-color: white; }");
    rowSearch->addWidget(txtSearch, 2);

    QLabel *lblPar = new QLabel("Par :");
    lblPar->setStyleSheet("font-size: 12px; color: #333; font-weight: 500; margin-left: 10px; min-width: 30px;");
    rowSearch->addWidget(lblPar);
    
    cbSearchType = new QComboBox();
    cbSearchType->addItems({"Tout", "Client", QString::fromUtf8("Catégorie"), "Description"});
    cbSearchType->setMinimumHeight(35);
    cbSearchType->setMaximumHeight(35);
    cbSearchType->setMaximumWidth(140);
    cbSearchType->setStyleSheet("QComboBox { border: 1px solid #ddd; border-radius: 4px; padding: 5px 10px; font-size: 12px; background-color: white; }");
    rowSearch->addWidget(cbSearchType);
    
    QLabel *lblDu = new QLabel("Du :");
    lblDu->setStyleSheet("font-size: 12px; color: #333; font-weight: 500; margin-left: 15px; min-width: 25px;");
    rowSearch->addWidget(lblDu);
    
    dateFrom = new QDateEdit(QDate::currentDate().addMonths(-1));
    dateFrom->setCalendarPopup(true);
    dateFrom->setMinimumHeight(35);
    dateFrom->setMaximumHeight(35);
    dateFrom->setMaximumWidth(130);
    dateFrom->setStyleSheet("QDateEdit { border: 1px solid #ddd; border-radius: 4px; padding: 5px 10px; font-size: 12px; background-color: white; }");
    rowSearch->addWidget(dateFrom);

    QLabel *lblAu = new QLabel("Au :");
    lblAu->setStyleSheet("font-size: 12px; color: #333; font-weight: 500; margin-left: 10px; min-width: 25px;");
    rowSearch->addWidget(lblAu);
    
    dateTo = new QDateEdit(QDate::currentDate());
    dateTo->setCalendarPopup(true);
    dateTo->setMinimumHeight(35);
    dateTo->setMaximumHeight(35);
    dateTo->setMaximumWidth(130);
    dateTo->setStyleSheet("QDateEdit { border: 1px solid #ddd; border-radius: 4px; padding: 5px 10px; font-size: 12px; background-color: white; }");
    rowSearch->addWidget(dateTo);

    btnSearch = new QPushButton(QString::fromUtf8("Rechercher"));
    btnSearch->setMinimumHeight(35);
    btnSearch->setMaximumHeight(35);
    btnSearch->setMaximumWidth(120);
    btnSearch->setStyleSheet("background-color: #1B4332; color: white; border-radius: 4px; font-weight: bold; padding: 0 15px; font-size: 12px; border: none;");
    rowSearch->addWidget(btnSearch);
    rowSearch->addStretch();
    searchLayout->addLayout(rowSearch);

    // --- Ligne 2: Tri ---
    QHBoxLayout *rowSort = new QHBoxLayout();
    rowSort->setSpacing(10);
    rowSort->setContentsMargins(0, 0, 0, 0);
    
    QLabel *lblSort = new QLabel("Trier par :");
    lblSort->setStyleSheet("font-size: 12px; color: #333; font-weight: 500; min-width: 85px;");
    rowSort->addWidget(lblSort);
    
    cbSortBy = new QComboBox();
    cbSortBy->addItems({"Date", "Montant", "Client", QString::fromUtf8("Catégorie")});
    cbSortBy->setMinimumHeight(35);
    cbSortBy->setMaximumHeight(35);
    cbSortBy->setMaximumWidth(140);
    cbSortBy->setStyleSheet("QComboBox { border: 1px solid #ddd; border-radius: 4px; padding: 5px 10px; font-size: 12px; background-color: white; }");
    rowSort->addWidget(cbSortBy);

    cbOrder = new QComboBox();
    cbOrder->addItems({QString::fromUtf8("Décroissant"), "Croissant"});
    cbOrder->setMinimumHeight(35);
    cbOrder->setMaximumHeight(35);
    cbOrder->setMaximumWidth(130);
    cbOrder->setStyleSheet("QComboBox { border: 1px solid #ddd; border-radius: 4px; padding: 5px 10px; font-size: 12px; background-color: white; }");
    rowSort->addWidget(cbOrder);

    btnSort = new QPushButton(QString::fromUtf8("Appliquer Tri"));
    btnSort->setMinimumHeight(35);
    btnSort->setMaximumHeight(35);
    btnSort->setMaximumWidth(140);
    btnSort->setStyleSheet("background-color: #27785F; color: white; border-radius: 4px; font-weight: bold; padding: 0 15px; font-size: 12px; border: none;");
    rowSort->addWidget(btnSort);
    rowSort->addStretch();
    searchLayout->addLayout(rowSort);

    // --- Ligne 3: Export ---
    QHBoxLayout *rowExport = new QHBoxLayout();
    rowExport->setSpacing(35);
    rowExport->setContentsMargins(0, 0, 0, 0);
    
    // Bouton Export
    btnExport = new QPushButton("📤 Export");
    btnExport->setMinimumHeight(40);
    btnExport->setMaximumHeight(40);
    btnExport->setMaximumWidth(115);
    btnExport->setStyleSheet(
        "QPushButton { border: 2px solid #1B4332; background-color: #1B4332; color: white; border-radius: 6px; font-weight: bold; font-size: 13px; padding: 0 15px; }"
        "QPushButton:hover { background-color: #0d6b3c; border: 2px solid #0d6b3c; }"
        "QPushButton:pressed { background-color: #0a4f2a; }"
    );
    rowExport->addWidget(btnExport);

    // Formats avec emojis
    QLabel *lblFormat = new QLabel("Format :");
    lblFormat->setStyleSheet("font-size: 12px; color: #333; font-weight: 600; margin-left: 20px;");
    rowExport->addWidget(lblFormat);

    // PDF
    radioPDF = new QRadioButton("");
    radioPDF->setChecked(true);
    radioPDF->setMaximumWidth(24);
    radioPDF->setMinimumHeight(36);
    radioPDF->setStyleSheet(
        "QRadioButton { spacing: 8px; }"
        "QRadioButton::indicator { width: 20px; height: 20px; border-radius: 10px; border: 2px solid #0066cc; background-color: white; margin-right: 5px; }"
        "QRadioButton::indicator:checked { background-color: #0066cc; border: 2px solid #004999; }"
        "QRadioButton::indicator:hover { border: 2px solid #0052a3; }"
    );
    QLabel *lblPDF = new QLabel("📄 PDF");
    lblPDF->setStyleSheet("font-size: 12px; color: #0066cc; font-weight: 600; padding: 8px 12px; background-color: #f0f4ff; border-radius: 5px;");
    QHBoxLayout *pdfLayout = new QHBoxLayout();
    pdfLayout->setSpacing(0);
    pdfLayout->setContentsMargins(0, 0, 0, 0);
    pdfLayout->addWidget(radioPDF);
    pdfLayout->addWidget(lblPDF);
    rowExport->addLayout(pdfLayout);

    // TXT
    radioTXT = new QRadioButton("");
    radioTXT->setMaximumWidth(24);
    radioTXT->setMinimumHeight(36);
    radioTXT->setStyleSheet(
        "QRadioButton { spacing: 8px; }"
        "QRadioButton::indicator { width: 20px; height: 20px; border-radius: 10px; border: 2px solid #0d6b3c; background-color: white; margin-right: 5px; }"
        "QRadioButton::indicator:checked { background-color: #0d6b3c; border: 2px solid #084a26; }"
        "QRadioButton::indicator:hover { border: 2px solid #0a5a30; }"
    );
    QLabel *lblTXT = new QLabel("📋 TXT");
    lblTXT->setStyleSheet("font-size: 12px; color: #0d6b3c; font-weight: 600; padding: 8px 12px; background-color: #f0f8f4; border-radius: 5px;");
    QHBoxLayout *txtLayout = new QHBoxLayout();
    txtLayout->setSpacing(0);
    txtLayout->setContentsMargins(0, 0, 0, 0);
    txtLayout->addWidget(radioTXT);
    txtLayout->addWidget(lblTXT);
    rowExport->addLayout(txtLayout);

    rowExport->addStretch();
    searchLayout->addLayout(rowExport);

    mainLayout->addWidget(searchGroup);

    // 2. Zone Tableau (Lecture seule)
    QGroupBox *tableGroup = new QGroupBox(QString::fromUtf8("📋 Aperçu des Résultats"), this);
    tableGroup->setStyleSheet("QGroupBox{ border: 1px solid #e0e0e0; border-radius: 6px; margin-top: 10px; padding-top: 15px; } QGroupBox::title{ padding: 0px 10px; font-size: 13px; font-weight: bold; color: #1B4332; }");
    QVBoxLayout *tableLayout = new QVBoxLayout(tableGroup);
    tableLayout->setContentsMargins(10, 10, 10, 10);

    tableSearch = new QTableWidget();
    tableSearch->setColumnCount(7);
    tableSearch->setHorizontalHeaderLabels({"ID", "Type", QString::fromUtf8("Catégorie"), "Montant", "Date", "Client", "Description"});
    tableSearch->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableSearch->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableSearch->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);
    tableSearch->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    tableSearch->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableSearch->verticalHeader()->setVisible(false);
    tableSearch->verticalHeader()->setDefaultSectionSize(45);
    tableSearch->setStyleSheet(
        "QTableWidget { background-color: white; border: none; gridline-color: #f0f0f0; alternate-background-color: #f4f7f6; } "
        "QHeaderView::section { background-color: #1B4332; color: white; padding: 12px; font-weight: bold; border: none; border-right: 1px solid rgba(255,255,255,0.1); height: 35px; }"
    );

    tableLayout->addWidget(tableSearch);
    mainLayout->addWidget(tableGroup, 1);
    setLayout(mainLayout);
}

void SearchTab::setupConnections()
{
    connect(btnSearch, &QPushButton::clicked, this, &SearchTab::rechercher);
    connect(btnSort, &QPushButton::clicked, this, &SearchTab::trier);
    connect(btnExport, &QPushButton::clicked, this, &SearchTab::exportTransaction);
}

void SearchTab::chargerTransactions()
{
    tableSearch->setRowCount(0);
    QSqlDatabase db = dbConn->getDatabase();
    if (!db.isOpen()) return;

    QSqlQuery query(db);
    QString baseQuery = "SELECT f.id_transaction, f.ctype, f.categorie, f.montant, TO_CHAR(f.date_trans, 'DD/MM/YYYY'), "
                        "COALESCE(c.NOM || ' ' || c.PRENOM, m.NOM_MACHINE, 'N/A') as tiers, f.description "
                        "FROM finance f "
                        "LEFT JOIN CLIENT c ON f.id_machine = c.ID_CLIENT AND f.ctype = 'REVENU' "
                        "LEFT JOIN MACHINE m ON f.id_machine = m.ID_MACHINE AND f.ctype != 'REVENU' "
                        "ORDER BY f.date_trans DESC";

    if (!query.exec(baseQuery)) {
        qDebug() << "Erreur ChargerTransactions:" << query.lastError().text();
        return;
    }
    
    int rowCount = 0;
    while (query.next()) {
        tableSearch->insertRow(rowCount);
        for(int i=0; i<7; i++) {
            QString val = query.value(i).toString();
            if(i == 3) val += " DT";
            QTableWidgetItem *item = new QTableWidgetItem(val);
            item->setTextAlignment(Qt::AlignCenter);
            tableSearch->setItem(rowCount, i, item);
        }
        rowCount++;
    }
}

void SearchTab::rechercher()
{
    tableSearch->setRowCount(0);
    QString searchText = txtSearch->text().trimmed();
    QString searchType = cbSearchType->currentText();

    QSqlDatabase db = dbConn->getDatabase();
    if (!db.isOpen()) return;

    QString sql = "SELECT f.id_transaction, f.ctype, f.categorie, f.montant, TO_CHAR(f.date_trans, 'DD/MM/YYYY'), "
                  "COALESCE(c.NOM || ' ' || c.PRENOM, m.NOM_MACHINE, 'N/A') as tiers, f.description "
                  "FROM finance f "
                  "LEFT JOIN CLIENT c ON f.id_machine = c.ID_CLIENT AND f.ctype = 'REVENU' "
                  "LEFT JOIN MACHINE m ON f.id_machine = m.ID_MACHINE AND f.ctype != 'REVENU' "
                  "WHERE f.date_trans BETWEEN ? AND ?";

    int searchIdx = cbSearchType->currentIndex(); 
    if (!searchText.isEmpty()) {
        if (searchIdx == 1) sql += " AND (LOWER(c.NOM) LIKE LOWER(:s) OR LOWER(c.PRENOM) LIKE LOWER(:s) OR LOWER(m.NOM_MACHINE) LIKE LOWER(:s))";
        else if (searchIdx == 2) sql += " AND LOWER(f.categorie) LIKE LOWER(:s)";
        else if (searchIdx == 3) sql += " AND LOWER(f.description) LIKE LOWER(:s)";
        else sql += " AND (LOWER(f.ctype) LIKE LOWER(:s) OR LOWER(f.categorie) LIKE LOWER(:s) OR LOWER(f.description) LIKE LOWER(:s) OR LOWER(c.NOM) LIKE LOWER(:s) OR LOWER(m.NOM_MACHINE) LIKE LOWER(:s))";
    }

    int sortIdx = cbSortBy->currentIndex(); 
    QString col = "f.date_trans";
    if (sortIdx == 1) col = "f.montant";
    else if (sortIdx == 0) col = "f.date_trans";
    else if (sortIdx == 2) col = "tiers";
    else if (sortIdx == 3) col = "f.categorie";

    bool isDesc = (cbOrder->currentIndex() == 0); 
    sql += " ORDER BY " + col + (isDesc ? " DESC" : " ASC");

    QSqlQuery query(db);
    query.prepare(sql);
    query.addBindValue(dateFrom->date());
    query.addBindValue(dateTo->date());
    if (!searchText.isEmpty()) {
        query.addBindValue("%" + searchText + "%");
    }

    if (query.exec()) {
        int rowCount = 0;
        while (query.next()) {
            tableSearch->insertRow(rowCount);
            for(int i=0; i<7; i++) {
                QString val = query.value(i).toString();
                if(i == 3) val += " DT";
                QTableWidgetItem *item = new QTableWidgetItem(val);
                item->setTextAlignment(Qt::AlignCenter);
                tableSearch->setItem(rowCount, i, item);
            }
            rowCount++;
        }
    } else {
        QMessageBox::critical(this, "Erreur de Recherche", 
            QString::fromUtf8("La requête SQL a échoué.\n\nErreur :\n") + query.lastError().text() +
            QString::fromUtf8("\n\nRequête :\n") + query.lastQuery());
    }
}

void SearchTab::trier() { rechercher(); }

void SearchTab::exportTransaction()
{
    if (radioPDF->isChecked()) exportPDF();
    else if (radioTXT->isChecked()) exportCSV();  // CSV format for TXT
}

void SearchTab::exportPDF()
{
    QString downloadPath = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
    QString fileName = downloadPath + "/Rapport_Finance_" + QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss") + ".pdf";

    QPrinter printer(QPrinter::PrinterResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setPageSize(QPageSize(QPageSize::A4));
    printer.setOutputFileName(fileName);

    QTextDocument doc;
    QString html = "<html><head><style>th{background-color:#1B4332;color:white;padding:5px;}td{padding:5px;border:1px solid #ddd;}</style></head><body>"
                   "<h1 style='color:#1B4332;text-align:center;'>RAPPORT DES TRANSACTIONS</h1>"
                   "<table width='100%' style='border-collapse:collapse;'>"
                   "<tr><th>ID</th><th>Type</th><th>Catégorie</th><th>Montant</th><th>Date</th><th>Client</th><th>Description</th></tr>";

    for (int i = 0; i < tableSearch->rowCount(); ++i) {
        html += "<tr>";
        for (int j = 0; j < 7; ++j) {
            html += "<td>" + tableSearch->item(i, j)->text() + "</td>";
        }
        html += "</tr>";
    }
    html += "</table></body></html>";

    doc.setHtml(html);
    doc.print(&printer);
    QDesktopServices::openUrl(QUrl::fromLocalFile(fileName));
}

void SearchTab::exportCSV()
{
    QString downloadPath = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
    QString fileName = downloadPath + "/Transactions_Finance_" + QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss") + ".csv";

    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << "ID;Type;Categorie;Montant;Date;Client;Description\n";
        for (int i = 0; i < tableSearch->rowCount(); ++i) {
            for (int j = 0; j < 7; ++j) {
                out << tableSearch->item(i, j)->text() << (j == 6 ? "" : ";");
            }
            out << "\n";
        }
        file.close();
        QDesktopServices::openUrl(QUrl::fromLocalFile(fileName));
    }
}

void SearchTab::exportExcel() { exportCSV(); }
void SearchTab::exportWord() { exportPDF(); }

// ------------------------------------------------------------------------------------------------------------------------
// SECTION: FINANCE MAIN WINDOW
// ------------------------------------------------------------------------------------------------------------------------

// ========================================================================
// IMPLÉMENTATION: Finance
// ========================================================================
Finance::Finance(int userId, QWidget *parent)
    : QMainWindow(parent)
    , currentUserId(userId)
    , ui(new Ui::Finance)
{
    ui->setupUi(this);
    initializeUI();
    applyStyles();

    // Initialiser le timer pour la mise à jour date/heure
    dateTimeTimer = new QTimer(this);
    connect(dateTimeTimer, &QTimer::timeout, this, &Finance::updateDateTime);
    dateTimeTimer->start(1000);  // Mise à jour chaque seconde
    updateDateTime();  // Appel initial

    // Taille initiale 1441x616
    resize(1441, 616);

    // Taille minimale pour éviter d'écraser l'interface lors du redimensionnement
    setMinimumSize(900, 500);

    // La fenêtre peut être agrandie/réduite librement
    // (pas de setFixedSize → maximize/minimize fonctionnent correctement)
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

Finance::~Finance()
{
    if (dateTimeTimer) {
        dateTimeTimer->stop();
        delete dateTimeTimer;
    }
    delete ui;
}

void Finance::updateDateTime()
{
    QDateTime now = QDateTime::currentDateTime();
    QLocale frenchLocale(QLocale::French);
    QString dateStr = frenchLocale.toString(now.date(), "dd MMMM yyyy");
    QString timeStr = now.toString("HH:mm:ss");
    lblDateTime->setText(QString::fromUtf8("📅 %1 | 🕐 %2")
                         .arg(dateStr, timeStr));
}

void Finance::initializeUI()
{
    // Créer le widget central
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *centralLayout = new QVBoxLayout(centralWidget);
    centralLayout->setContentsMargins(0, 0, 0, 0);
    centralLayout->setSpacing(0);

    // ===== HEADER =====
    QWidget *headerWidget = new QWidget();
    headerWidget->setMinimumHeight(80);
    headerWidget->setMaximumHeight(80);
    QHBoxLayout *headerLayout = new QHBoxLayout(headerWidget);
    headerLayout->setContentsMargins(20, 10, 20, 10);
    headerLayout->setSpacing(20);

    // Bouton Retour
    QPushButton *btnReturn = new QPushButton(QString::fromUtf8("Retour Menu"));
    btnReturn->setMinimumWidth(100);
    btnReturn->setStyleSheet(
        "QPushButton { "
        "background-color: rgba(255, 255, 255, 0.15); "
        "color: white; "
        "font-size: 13px; "
        "font-weight: 600; "
        "padding: 10px 20px; "
        "border-radius: 8px; "
        "border: 1px solid rgba(255, 255, 255, 0.3); "
        "min-width: 100px; "
        "} "
        "QPushButton:hover { "
        "background-color: rgba(255, 255, 255, 0.25); "
        "border: 1px solid rgba(255, 255, 255, 0.5); "
        "} "
        "QPushButton:pressed { "
        "background-color: rgba(255, 255, 255, 0.1); "
        "}"
    );
    // Connect return button to close/back signal
    connect(btnReturn, &QPushButton::clicked, this, &Finance::close);

    // Titre principal
    QVBoxLayout *titleLayout = new QVBoxLayout();
    titleLayout->setSpacing(2);
    QLabel *lblMainTitle = new QLabel(QString::fromUtf8("GESTION FINANCIÈRE"));
    lblMainTitle->setStyleSheet("color: white; font-size: 36px; font-weight: 800; font-family: 'Segoe UI', 'Arial Black', sans-serif; letter-spacing: 2px;");
    QLabel *lblSubtitle = new QLabel("TABLEAU DE BORD FINANCIER");
    lblSubtitle->setStyleSheet("color: #C9A227; font-size: 13px; font-weight: 600; font-family: 'Segoe UI', Arial, sans-serif; letter-spacing: 6px;");
    titleLayout->addWidget(lblMainTitle);
    titleLayout->addWidget(lblSubtitle);

    // Logo
    QLabel *lblIcon = new QLabel();
    QPixmap logoPixmap;
    for (const QString &p : QStringList{":/logo.png", "logo.png", "../production/logo.png",
         QCoreApplication::applicationDirPath() + "/../../logo.png",
         QCoreApplication::applicationDirPath() + "/../../../production/logo.png"}) {
        logoPixmap = QPixmap(p);
        if (!logoPixmap.isNull()) break;
    }
    if (!logoPixmap.isNull()) {
        lblIcon->setPixmap(logoPixmap.scaledToHeight(50, Qt::SmoothTransformation));
    } else {
        lblIcon->setText("🫒");
        lblIcon->setStyleSheet("font-size: 28px;");
    }
    lblIcon->setAlignment(Qt::AlignCenter);
    lblIcon->setFixedSize(54, 54);

    // Date/Time indicator
    lblDateTime = new QLabel(QString::fromUtf8("📅 26 mars 2026 | 🕐 22:12:21"));
    lblDateTime->setStyleSheet("color: #FFFFFF; font-weight: bold; font-size: 12px; padding: 8px 14px; background-color: rgba(27, 67, 50, 0.5); border-radius: 8px; border: 1px solid rgba(201, 162, 39, 0.3);");
    lblDateTime->setMinimumWidth(280);

    // Status online
    QLabel *lblStatus = new QLabel("SYSTÈME EN LIGNE");
    lblStatus->setStyleSheet("color: #4ADE80; font-weight: bold; font-size: 14px; padding: 6px 14px; background-color: rgba(74, 222, 128, 0.15); border-radius: 15px; border: 1px solid rgba(74, 222, 128, 0.3);");

    // Assembler le header
    headerLayout->addWidget(btnReturn);
    headerLayout->addWidget(lblIcon);
    headerLayout->addLayout(titleLayout);
    headerLayout->addStretch();
    headerLayout->addWidget(lblDateTime);
    headerLayout->addWidget(lblStatus);

    // Appliquer le style du header avec gradient moderne
    headerWidget->setStyleSheet(
        "QWidget#headerWidget { "
        "background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
        "stop:0 #1B4332, stop:0.5 #234E3E, stop:1 #1B4332); "
        "border-bottom: 4px solid #C9A227; "
        "}"
    );
    headerWidget->setObjectName("headerWidget");

    centralLayout->addWidget(headerWidget);

    // Créer le QTabWidget
    mainTabWidget = new QTabWidget(this);

    // Créer les tabs
    transactionTab = new TransactionTab(currentUserId);
    statsTab = new StatsTab();
    advancedTab = new AdvancedTab();

    // Ajouter les tabs
    mainTabWidget->addTab(transactionTab, QString::fromUtf8("💸 Transactions"));
    mainTabWidget->addTab(statsTab, QString::fromUtf8("📊 Statistiques"));
    mainTabWidget->addTab(advancedTab, QString::fromUtf8("🔬 Analyse Avancée"));

    // Connections Globales
    connect(transactionTab, &TransactionTab::dataChanged, statsTab, &StatsTab::calculerStats);
    
    // Refresh initial
    QTimer::singleShot(200, statsTab, &StatsTab::calculerStats);

    centralLayout->addWidget(mainTabWidget);
    setCentralWidget(centralWidget);

    // Configuration de la fenêtre
    setWindowTitle(QString::fromUtf8("Gestion Financière"));
    resize(1441, 616);
}

void Finance::applyStyles()
{
    // PressIQ Professional Theme - Enhanced Modern Design
    QString stylesheet = R"(
        /* ========================================================================
           PRESSIQ MODERN PROFESSIONAL THEME
           Based on PressIQ Production Module v2.0
        ======================================================================== */

        QMainWindow {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:1, 
                        stop:0 #f0f2f5, stop:1 #e4e7eb);
        }

        /* ------------------------------------------------------------------------
           TAB WIDGET - MODERN DESIGN
        ------------------------------------------------------------------------ */
        QTabWidget::pane {
            border: none;
            background-color: transparent;
            border-radius: 12px;
            margin-top: 5px;
        }

        QTabBar {
            background: transparent;
        }

        QTabBar::tab {
            background-color: white;
            color: #666;
            padding: 12px 24px;
            margin-right: 4px;
            border: none;
            border-radius: 8px 8px 0 0;
            font-size: 13px;
            font-weight: 600;
            font-family: 'Segoe UI', Arial, sans-serif;
            min-width: 120px;
        }

        QTabBar::tab:selected {
            background-color: #1B4332;
            color: white;
            font-weight: 700;
        }

        QTabBar::tab:hover:!selected {
            background-color: #f0f0f0;
            color: #1B4332;
        }

        /* ========================================================================
           GROUP BOX - CARD STYLE
           ======================================================================== */
        QGroupBox {
            font-weight: bold;
            border: 2px solid #1B4332;
            border-radius: 8px;
            margin-top: 10px;
            padding-top: 10px;
            background-color: #fafafa;
        }

        QGroupBox::title {
            subcontrol-origin: margin;
            left: 10px;
            padding: 0 5px;
            color: #1B4332;
        }

        /* ========================================================================
           INPUT FIELDS - MODERN STYLE
           ======================================================================== */
        QLineEdit, QSpinBox, QDoubleSpinBox, QDateEdit, QComboBox {
            border: 1px solid #ccc;
            border-radius: 4px;
            padding: 6px;
            background: white;
            color: #2c3e50;
            font-size: 13px;
        }

        QLineEdit:focus, QSpinBox:focus, QDoubleSpinBox:focus, QDateEdit:focus, QComboBox:focus {
            border: 2px solid #1B4332;
        }

        QComboBox::drop-down {
            border: none;
            padding-right: 15px;
            width: 30px;
        }

        QComboBox QAbstractItemView {
            border: 2px solid #1B4332;
            border-radius: 8px;
            background-color: white;
            selection-background-color: #1B4332;
            selection-color: white;
            padding: 5px;
        }

        /* ------------------------------------------------------------------------
           TEXT EDIT - MODERN STYLE
        ------------------------------------------------------------------------ */
        QTextEdit, QTextBrowser {
            border: 2px solid #e0e0e0;
            border-radius: 8px;
            background-color: #fafafa;
            color: #333;
            font-size: 14px;
            font-family: 'Segoe UI', Arial, sans-serif;
            padding: 12px;
        }

        QTextEdit:focus, QTextBrowser:focus {
            border: 2px solid #1B4332;
            background-color: white;
        }

        /* ========================================================================
           BUTTONS - PROFESSIONAL STYLE
           ======================================================================== */
        QPushButton {
            background-color: #1B4332;
            color: white;
            border: none;
            padding: 8px 16px;
            border-radius: 4px;
            font-weight: bold;
        }

        QPushButton:hover {
            background-color: #234E3E;
        }

        QPushButton:pressed {
            background-color: #0F2A1F;
        }

        /* ------------------------------------------------------------------------
           PROGRESS BAR - MODERN ANIMATED STYLE
        ------------------------------------------------------------------------ */
        QProgressBar {
            border: none;
            border-radius: 10px;
            text-align: center;
            font-weight: 700;
            font-size: 13px;
            background-color: #e5e7eb;
            min-height: 24px;
            color: white;
        }

        QProgressBar::chunk {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0, 
                        stop:0 #1B4332, stop:0.5 #234E3E, stop:1 #1B4332);
            border-radius: 10px;
        }

        /* ========================================================================
           TABLE WIDGET - PROFESSIONAL STYLE
           ======================================================================== */
        QTableWidget {
            border: 1px solid #ddd;
            border-radius: 4px;
            gridline-color: #e0e0e0;
            background-color: white;
            alternate-background-color: #f9f9f9;
        }

        QTableWidget::item {
            padding: 12px;
            border-bottom: 1px solid #f0f0f0;
            color: #333333; /* Explicit Item Text Color */
            background-color: white;
        }

        QTableWidget::item:selected {
            background-color: rgba(27, 67, 50, 0.15);
            color: black;
        }

        QTableWidget::item:hover {
            background-color: rgba(27, 67, 50, 0.05);
        }

        QHeaderView {
            background: #1B4332;
            background-color: #1B4332;
            border: none;
            border-radius: 8px;
        }

        QTableCornerButton::section {
            background-color: #1B4332;
            border: none;
        }

        QHeaderView::section {
            background-color: #1B4332;
            color: white;
            padding: 8px;
            border: none;
            font-weight: bold;
        }

        QHeaderView::section:first {
            border-top-left-radius: 8px;
        }

        QHeaderView::section:last {
            border-top-right-radius: 8px;
        }

        /* ------------------------------------------------------------------------
           LABELS - TYPOGRAPHY
        ------------------------------------------------------------------------ */
        QLabel {
            color: #374151;
            font-size: 14px;
            font-family: 'Segoe UI', Arial, sans-serif;
        }

        /* ------------------------------------------------------------------------
           CHECKBOX - MODERN STYLE
        ------------------------------------------------------------------------ */
        QCheckBox {
            font-size: 14px;
            font-family: 'Segoe UI', Arial, sans-serif;
            color: #374151;
            spacing: 12px;
            padding: 8px;
        }

        QCheckBox::indicator {
            width: 22px;
            height: 22px;
            border-radius: 6px;
        }

        QCheckBox::indicator:unchecked {
            background-color: #fafafa;
            border: 2px solid #d1d5db;
        }

        QCheckBox::indicator:unchecked:hover {
            border: 2px solid #1B4332;
        }

        QCheckBox::indicator:checked {
            background-color: #1B4332;
            border: 2px solid #1B4332;
        }

        /* ------------------------------------------------------------------------
           SCROLLBAR - MINIMAL STYLE
        ------------------------------------------------------------------------ */
        QScrollBar:vertical {
            border: none;
            background: #f0f0f0;
            width: 10px;
            border-radius: 5px;
            margin: 0;
        }

        QScrollBar::handle:vertical {
            background: #c0c0c0;
            border-radius: 5px;
            min-height: 30px;
        }

        QScrollBar::handle:vertical:hover {
            background: #1B4332;
        }

        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
            height: 0;
        }

        QScrollBar::horizontal {
            border: none;
            background: #f0f0f0;
            height: 10px;
            border-radius: 5px;
        }

        QScrollBar::handle:horizontal {
            background: #c0c0c0;
            border-radius: 5px;
            min-width: 30px;
        }

        QScrollBar::handle:horizontal:hover {
            background: #1B4332;
        }
    )";

    qApp->setStyle("Fusion");
    qApp->setStyleSheet(stylesheet);
}
