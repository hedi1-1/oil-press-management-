#include "finance.h"
#include "connexionfinance.h"
#include "historiqueaudit.h"
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
#include <QDesktopServices>
#include <QUrl>
#include <QStandardPaths>
#include <QFileInfo>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QPrinter>
#include <QTextDocument>
#include <QSqlQuery>
#include <QDate>
#include <QApplication>
#include <QMainWindow>
#include <QDebug>
#include <QStringList>
#include <QPainter>
#include <QShowEvent>

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
    mainLayout->setSpacing(8);
    mainLayout->setContentsMargins(10, 10, 10, 10);

    // ===== SECTION SUPÉRIEURE: Formulaire GRAND (Gauche) + Filtres PETIT (Droite) =====
    QHBoxLayout *topLayout = new QHBoxLayout();
    topLayout->setSpacing(10);

    // ===== FORMULAIRE CRUD (Gauche - GRAND) =====
    QGroupBox *formGroup = new QGroupBox(QString::fromUtf8("📝 Nouvelle Transaction / Modification"), this);
    QVBoxLayout *formLayout = new QVBoxLayout(formGroup);
    formLayout->setContentsMargins(12, 12, 12, 12);
    formLayout->setSpacing(10);

    QGridLayout *mainGrid = new QGridLayout();
    mainGrid->setContentsMargins(5, 5, 5, 5);
    mainGrid->setHorizontalSpacing(20);
    mainGrid->setVerticalSpacing(12);

    QString labelStyle = "font-weight: bold; color: #1B4332; font-size: 13px;";

    // ID
    QLabel *lblId = new QLabel("ID Transaction :");
    lblId->setStyleSheet(labelStyle);
    txtId = new QLineEdit();
    txtId->setReadOnly(true);
    txtId->setPlaceholderText("Auto-généré");
    txtId->setMaximumHeight(32);
    txtId->setStyleSheet("background-color: #f0f0f0; border-radius: 5px; padding: 6px; font-size: 12px;");
    mainGrid->addWidget(lblId, 0, 0);
    mainGrid->addWidget(txtId, 0, 1);

    // Type
    QLabel *lblType = new QLabel("Type :");
    lblType->setStyleSheet(labelStyle);
    cbType = new QComboBox();
    cbType->addItems({"REVENU", QString::fromUtf8("DÉPENSE")});
    cbType->setMaximumHeight(32);
    cbType->setStyleSheet("font-size: 12px;");
    mainGrid->addWidget(lblType, 0, 2);
    mainGrid->addWidget(cbType, 0, 3);

    // Catégorie
    QLabel *lblCat = new QLabel(QString::fromUtf8("Catégorie :"));
    lblCat->setStyleSheet(labelStyle);
    cbCategorie = new QComboBox();
    cbCategorie->setMaximumHeight(32);
    cbCategorie->setStyleSheet("font-size: 12px;");
    mainGrid->addWidget(lblCat, 1, 0);
    mainGrid->addWidget(cbCategorie, 1, 1);

    // Montant
    QLabel *lblMontant = new QLabel("Montant :");
    lblMontant->setStyleSheet(labelStyle);
    spinMontant = new QDoubleSpinBox();
    spinMontant->setRange(0, 1000000);
    spinMontant->setDecimals(2);
    spinMontant->setButtonSymbols(QAbstractSpinBox::NoButtons);
    spinMontant->setSuffix(" DT");
    spinMontant->setMaximumHeight(32);
    spinMontant->setStyleSheet("font-size: 12px;");
    mainGrid->addWidget(lblMontant, 1, 2);
    mainGrid->addWidget(spinMontant, 1, 3);

    // Date
    QLabel *lblDate = new QLabel("Date :");
    lblDate->setStyleSheet(labelStyle);
    dateEdit = new QDateEdit(QDate::currentDate());
    dateEdit->setCalendarPopup(false);
    dateEdit->setReadOnly(true);
    dateEdit->setMaximumHeight(32);
    dateEdit->setDisplayFormat("dd/MM/yyyy");  // Format clair
    dateEdit->setStyleSheet("font-size: 12px; background-color: #f0f0f0;");
    mainGrid->addWidget(lblDate, 2, 0);
    mainGrid->addWidget(dateEdit, 2, 1);

    // Client & Machine
    lblEmployee = new QLabel(QString::fromUtf8("Employé :"));
    lblEmployee->setStyleSheet(labelStyle);
    txtEmployee = new QLineEdit();
    txtEmployee->setReadOnly(true);
    txtEmployee->setMaximumHeight(32);
    txtEmployee->setStyleSheet("background-color: #f0f0f0; border-radius: 5px; padding: 6px; font-size: 12px;");
    mainGrid->addWidget(lblEmployee, 2, 2);
    mainGrid->addWidget(txtEmployee, 2, 3);

    lblMachine = new QLabel("Machine :");
    lblMachine->setStyleSheet(labelStyle);
    cbMachine = new QComboBox();
    cbMachine->setMaximumHeight(32);
    cbMachine->setStyleSheet("font-size: 12px;");
    mainGrid->addWidget(lblMachine, 3, 0);
    mainGrid->addWidget(cbMachine, 3, 1);

    // Description
    QLabel *lblDesc = new QLabel("Description :");
    lblDesc->setStyleSheet(labelStyle);
    txtDescription = new QTextEdit();
    txtDescription->setMaximumHeight(60);
    txtDescription->setPlaceholderText("Détails...");
    txtDescription->setStyleSheet("font-size: 12px;");
    mainGrid->addWidget(lblDesc, 4, 0);
    mainGrid->addWidget(txtDescription, 4, 1, 1, 3);

    formLayout->addLayout(mainGrid);

    // Boutons Actions (Plus grands)
    QHBoxLayout *actionLayout = new QHBoxLayout();
    actionLayout->setSpacing(12);
    actionLayout->setContentsMargins(5, 10, 5, 5);

    btnAdd = new QPushButton(QString::fromUtf8("➕ Ajouter"));
    btnUpdate = new QPushButton(QString::fromUtf8("🔄 Modifier"));
    btnDelete = new QPushButton(QString::fromUtf8("🗑️ Supprimer"));
    btnConsult = new QPushButton(QString::fromUtf8("🧹 Nouveau"));

    QString btnBaseStyle = "QPushButton { color: white; border-radius: 6px; font-weight: bold; font-size: 13px; min-height: 40px; } ";
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

    // ===== RECHERCHE/FILTRAGE/TRI/EXPORT (Droite - TRÈS COMPACT) =====
    QGroupBox *searchGroup = new QGroupBox(QString::fromUtf8("🔍 Filtres & Export"), this);
    searchGroup->setMaximumWidth(280);
    QVBoxLayout *searchLayout = new QVBoxLayout(searchGroup);
    searchLayout->setContentsMargins(8, 8, 8, 8);
    searchLayout->setSpacing(6);

    // Filtres
    QLabel *lblFilterTitle = new QLabel("Type :");
    lblFilterTitle->setStyleSheet("font-weight: bold; color: #1B4332; font-size: 11px;");
    cbSearchType = new QComboBox();
    cbSearchType->addItems({"Tous", "REVENU", "DÉPENSE"});
    cbSearchType->setMaximumHeight(26);
    cbSearchType->setStyleSheet("font-size: 10px;");
    searchLayout->addWidget(lblFilterTitle);
    searchLayout->addWidget(cbSearchType);

    QLabel *lblCatFilter = new QLabel(QString::fromUtf8("Catég :"));
    lblCatFilter->setStyleSheet("font-weight: bold; color: #1B4332; font-size: 11px;");
    cbSearchCategory = new QComboBox();
    cbSearchCategory->addItem("Toutes");
    cbSearchCategory->setMaximumHeight(26);
    cbSearchCategory->setStyleSheet("font-size: 10px;");
    searchLayout->addWidget(lblCatFilter);
    searchLayout->addWidget(cbSearchCategory);

    btnSearch = new QPushButton(QString::fromUtf8("🔍 Chercher"));
    btnSearch->setMaximumHeight(30);
    btnSearch->setStyleSheet("background-color: #1B4332; color: white; border-radius: 5px; font-weight: bold; font-size: 11px;");
    searchLayout->addWidget(btnSearch);

    searchLayout->addSpacing(5);

    QLabel *lblSort = new QLabel(QString::fromUtf8("Tri :"));
    lblSort->setStyleSheet("font-weight: bold; color: #1B4332; font-size: 11px;");
    cbSortBy = new QComboBox();
    cbSortBy->addItems({"ID ↓", "Montant", "Date"});
    cbSortBy->setMaximumHeight(26);
    cbSortBy->setStyleSheet("font-size: 10px;");
    searchLayout->addWidget(lblSort);
    searchLayout->addWidget(cbSortBy);

    btnSort = new QPushButton(QString::fromUtf8("Appliquer"));
    btnSort->setMaximumHeight(28);
    btnSort->setStyleSheet("background-color: #2471A3; color: white; border-radius: 5px; font-weight: bold; font-size: 10px;");
    searchLayout->addWidget(btnSort);

    searchLayout->addSpacing(5);

    QLabel *lblExp = new QLabel(QString::fromUtf8("Export :"));
    lblExp->setStyleSheet("font-weight: bold; color: #1B4332; font-size: 11px;");
    searchLayout->addWidget(lblExp);

    btnExportPDF = new QPushButton(QString::fromUtf8("📄 PDF"));
    btnExportPDF->setMaximumHeight(26);
    btnExportPDF->setStyleSheet("background-color: #C0392B; color: white; border-radius: 4px; font-weight: bold; font-size: 10px;");
    searchLayout->addWidget(btnExportPDF);

    btnExportExcel = new QPushButton(QString::fromUtf8("📊 Excel"));
    btnExportExcel->setMaximumHeight(26);
    btnExportExcel->setStyleSheet("background-color: #27ae60; color: white; border-radius: 4px; font-weight: bold; font-size: 10px;");
    searchLayout->addWidget(btnExportExcel);

    btnExportTXT = new QPushButton(QString::fromUtf8("📝 TXT"));
    btnExportTXT->setMaximumHeight(26);
    btnExportTXT->setStyleSheet("background-color: #616A6B; color: white; border-radius: 4px; font-weight: bold; font-size: 10px;");
    searchLayout->addWidget(btnExportTXT);

    searchLayout->addStretch();

    // Ajouter les deux panneaux au layout supérieur
    topLayout->addWidget(formGroup, 3);
    topLayout->addWidget(searchGroup, 0);
    mainLayout->addLayout(topLayout);

    // ===== TABLEAU (Bas, TRÈS GRAND) =====
    QGroupBox *tableGroup = new QGroupBox(QString::fromUtf8("📋 Transactions"), this);
    QVBoxLayout *tableLayout = new QVBoxLayout(tableGroup);
    tableLayout->setContentsMargins(2, 10, 2, 2);

    tableTransaction = new QTableWidget();
    tableTransaction->setColumnCount(7);
    tableTransaction->setHorizontalHeaderLabels({"ID", "Type", QString::fromUtf8("Catégorie"), "Montant", "Date", "Employé", "Description"});

    tableTransaction->verticalHeader()->setDefaultSectionSize(38);
    tableTransaction->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableTransaction->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableTransaction->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableTransaction->setAlternatingRowColors(true);
    tableTransaction->setStyleSheet(
        "QTableWidget { background-color: white; gridline-color: #e8e8e8; alternate-background-color: #f9f9f9; } "
        "QHeaderView::section { background-color: #1B4332; color: white; font-weight: bold; padding: 8px; font-size: 13px; } "
        "QTableWidget::item { padding: 8px; font-size: 12px; } "
        "QTableWidget::item:selected { background-color: #c8e6c9; color: #1B4332; font-weight: bold; }"
    );

    tableLayout->addWidget(tableTransaction);
    mainLayout->addWidget(tableGroup, 1);
}

void TransactionTab::setupConnections()
{
    // CRUD
    connect(btnAdd, &QPushButton::clicked, this, &TransactionTab::ajouterTransaction);
    connect(btnUpdate, &QPushButton::clicked, this, &TransactionTab::modifierTransaction);
    connect(btnDelete, &QPushButton::clicked, this, &TransactionTab::supprimerTransaction);
    connect(btnConsult, &QPushButton::clicked, this, &TransactionTab::effacerFormulaire);
    connect(cbType, &QComboBox::currentTextChanged, this, &TransactionTab::onTypeChanged);
    
    // Masquer Machine si Salaire est sélectionné
    connect(cbCategorie, &QComboBox::currentTextChanged, this, [=](const QString &category) {
        if (category.isEmpty()) return;  // Évite les appels avec catégorie vide lors de l'init
        if (category == "Salaire") {
            lblMachine->hide();
            cbMachine->hide();
        } else if (cbType->currentText() == QString::fromUtf8("DÉPENSE")) {
            lblMachine->show();
            cbMachine->show();
        }
    });

    // Recherche, Tri, Export
    connect(btnSearch, &QPushButton::clicked, this, &TransactionTab::rechercher);
    connect(btnSort, &QPushButton::clicked, this, &TransactionTab::trier);
    connect(btnExportPDF, &QPushButton::clicked, this, [=]() { exporter("PDF"); });
    connect(btnExportExcel, &QPushButton::clicked, this, [=]() { exporter("CSV"); });
    connect(btnExportTXT, &QPushButton::clicked, this, [=]() { exporter("TXT"); });

    // CHANGE TYPE DANS FILTRES -> Mettre à jour les catégories
    connect(cbSearchType, &QComboBox::currentTextChanged, this, [=](const QString &type) {
        cbSearchCategory->clear();
        cbSearchCategory->addItem("Toutes");
        
        if (type == "REVENU") {
            cbSearchCategory->addItems({QString::fromUtf8("Vente Huile"), QString::fromUtf8("Service Pressage")});
        } else if (type == "DÉPENSE") {
            cbSearchCategory->addItems({QString::fromUtf8("Achat pièces"), "Maintenance", "Salaire", "Autre"});
        }
    });

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
        
        txtEmployee->setText(tableTransaction->item(row, 5)->text());
        
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

    if (amount <= 0.0) {
        QMessageBox::warning(this, "Validation", QString::fromUtf8("Le montant doit être supérieur à 0."));
        return;
    }
    if (category.isEmpty()) {
        QMessageBox::warning(this, "Validation", QString::fromUtf8("Veuillez sélectionner une catégorie."));
        return;
    }

    if (type == "REVENU") {
        // REVENU: l'employee vend l'huile, pas de machine
        machineId = 0;
    } else {
        // DÉPENSE: machine requise SEULEMENT pour Maintenance
        if (category == "Maintenance" && (cbMachine->currentIndex() < 0 || !cbMachine->currentData().isValid())) {
            QMessageBox::warning(this, "Validation", QString::fromUtf8("Veuillez sélectionner une machine pour une opération de maintenance."));
            return;
        }
        // Pour les autres dépenses (Salaire, Achat pièces, Autre), machine est optionnelle
        machineId = (cbMachine->currentIndex() >= 0 && cbMachine->currentData().isValid()) ? cbMachine->currentData().toInt() : 0;
    }

    // 2. Insertion
    QSqlDatabase db = dbConn->getDatabase();
    if (!db.isOpen()) return;

    QSqlQuery query(db);
    // Format montant avec point decimal (pas de virgule locale)
    QString montantStr = QString::number(amount, 'f', 2);
    QString sql = QString("INSERT INTO finance (ctype, categorie, montant, date_trans, description, ID_EMPLOYEE) "
                          "VALUES ('%1', '%2', %3, SYSDATE, '%4', %5)")
                  .arg(type.replace("'", "''"))
                  .arg(category.replace("'", "''"))
                  .arg(montantStr)
                  .arg(txtDescription->toPlainText().trimmed().replace("'", "''"))
                  .arg(currentUserId);
    
    qDebug() << "SQL EXECUTE:" << sql;
    
    if (query.exec(sql)) {
        qDebug() << "✓ INSERT réussi";
        if (!db.commit()) {
            qDebug() << "❌ COMMIT ÉCHOUÉ:" << db.lastError().text();
            db.rollback();
        } else {
            qDebug() << "✓ COMMIT réussi";
        }
        chargerTransactions();
        effacerFormulaire();
        emit dataChanged();
        QMessageBox::information(this, QString::fromUtf8("Succès"), QString::fromUtf8("Transaction enregistrée avec succès."));
    } else {
        qDebug() << "❌ INSERT ÉCHOUÉ:" << query.lastError().text();
        QMessageBox::critical(this, "Erreur", query.lastError().text());
    }
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

    if (amount <= 0.0) {
        QMessageBox::warning(this, "Validation", "Montant doit etre > 0.");
        return;
    }

    if (type == "REVENU") {
        machineId = 0;  // REVENU: pas de machine
    } else {
        // DÉPENSE: machine requise SEULEMENT pour Maintenance
        if (category == "Maintenance" && (cbMachine->currentIndex() < 0 || !cbMachine->currentData().isValid())) {
            QMessageBox::warning(this, "Validation", QString::fromUtf8("Veuillez sélectionner une machine pour une opération de maintenance."));
            return;
        }
        // Pour les autres dépenses (Salaire, Achat pièces, Autre), machine est optionnelle
        machineId = (cbMachine->currentIndex() >= 0 && cbMachine->currentData().isValid()) ? cbMachine->currentData().toInt() : 0;
    }

    QSqlDatabase db = dbConn->getDatabase();
    QSqlQuery query(db);
    
    // Format montant avec point decimal (pas de virgule locale)
    QString montantStr = QString::number(amount, 'f', 2);
    QString sql = QString("UPDATE finance SET ctype='%1', categorie='%2', montant=%3, "
                          "date_trans=SYSDATE, description='%4' WHERE id_transaction=%5")
                  .arg(type.replace("'", "''"))
                  .arg(category.replace("'", "''"))
                  .arg(montantStr)
                  .arg(txtDescription->toPlainText().trimmed().replace("'", "''"))
                  .arg(qId.toInt());
    
    if (query.exec(sql)) {
        db.commit();
        chargerTransactions();
        effacerFormulaire();
        emit dataChanged();
        QMessageBox::information(this, "Succes", "Transaction mise a jour.");
    } else {
        QMessageBox::critical(this, "Erreur", query.lastError().text());
    }
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
        db.commit();
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

    QString sql = "SELECT f.id_transaction, f.ctype, f.categorie, f.montant, TO_CHAR(f.date_trans, 'DD/MM/YYYY'), "
                  "COALESCE(e.USERNAME, TO_CHAR(f.ID_EMPLOYEE)), f.description "
                  "FROM finance f LEFT JOIN EMPLOYEES e ON f.ID_EMPLOYEE = e.USER_ID "
                  "WHERE f.TYPE_LIGNE = 'TRANSACTION' "
                  "ORDER BY f.id_transaction DESC";
    QSqlQuery query(sql, db);
    
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
    txtEmployee->setText(QString::number(currentUserId));
    
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
        lblMachine->hide(); cbMachine->hide();  // Pas de machine pour REVENU
    } else {
        cbCategorie->addItems({QString::fromUtf8("Achat pièces"), "Maintenance", "Salaire", "Autre"});
        // Machine visible par défaut, mais sera caché si Salaire est sélectionné
        if (cbCategorie->currentText() != "Salaire") {
            lblMachine->show(); 
            cbMachine->show();
        } else {
            lblMachine->hide(); 
            cbMachine->hide();
        }
    }
}

void TransactionTab::rechercher()
{
    QString typeFilter = cbSearchType->currentText();
    QString categoryFilter = cbSearchCategory->currentText();

    tableTransaction->setRowCount(0);
    QSqlDatabase db = dbConn->getDatabase();
    if (!db.isOpen()) return;

    QString sql = "SELECT f.id_transaction, f.ctype, f.categorie, f.montant, TO_CHAR(f.date_trans, 'DD/MM/YYYY'), "
                  "COALESCE(e.USERNAME, TO_CHAR(f.ID_EMPLOYEE)), f.description "
                  "FROM finance f LEFT JOIN EMPLOYEES e ON f.ID_EMPLOYEE = e.USER_ID "
                  "WHERE f.TYPE_LIGNE = 'TRANSACTION'";

    if (typeFilter != "Tous") {
        sql += " AND f.ctype = :type";
    }
    if (categoryFilter != "Toutes") {
        sql += " AND f.categorie = :category";
    }
    sql += " ORDER BY f.id_transaction DESC";

    QSqlQuery query(db);
    query.prepare(sql);
    if (typeFilter != "Tous") {
        query.bindValue(":type", typeFilter);
    }
    if (categoryFilter != "Toutes") {
        query.bindValue(":category", categoryFilter);
    }
    if (!query.exec()) {
        QMessageBox::critical(this, "Erreur", "Recherche échouée:\n" + query.lastError().text());
        return;
    }

    int rowCount = 0;
    while (query.next()) {
        tableTransaction->insertRow(rowCount);
        for(int i = 0; i < 7; i++) {
            QString val = query.value(i).toString();
            if(i == 3) val += " DT";  // Ajoute " DT" au montant
            QTableWidgetItem *item = new QTableWidgetItem(val);
            item->setTextAlignment(Qt::AlignCenter);
            tableTransaction->setItem(rowCount, i, item);
        }
        rowCount++;
    }
}

void TransactionTab::trier()
{
    QString sortOption = cbSortBy->currentText();
    
    tableTransaction->setRowCount(0);
    QSqlDatabase db = dbConn->getDatabase();
    if (!db.isOpen()) return;

    QString sql = "SELECT f.id_transaction, f.ctype, f.categorie, f.montant, TO_CHAR(f.date_trans, 'DD/MM/YYYY'), "
                  "COALESCE(e.USERNAME, TO_CHAR(f.ID_EMPLOYEE)), f.description "
                  "FROM finance f LEFT JOIN EMPLOYEES e ON f.ID_EMPLOYEE = e.USER_ID "
                  "WHERE f.TYPE_LIGNE = 'TRANSACTION'";

    // Appliquer les filtres actuels
    QString typeFilter = cbSearchType->currentText();
    QString categoryFilter = cbSearchCategory->currentText();
    
    if (typeFilter != "Tous") {
        sql += " AND f.ctype = :type";
    }
    if (categoryFilter != "Toutes") {
        sql += " AND f.categorie = :category";
    }

    // Appliquer le tri selon l'option sélectionnée
    if (sortOption == "ID ↓") {
        sql += " ORDER BY f.id_transaction DESC";
    } else if (sortOption == "Montant") {
        sql += " ORDER BY f.montant DESC";
    } else if (sortOption == "Date") {
        sql += " ORDER BY f.date_trans DESC";
    } else {
        sql += " ORDER BY f.id_transaction DESC";
    }

    QSqlQuery query(db);
    query.prepare(sql);
    if (typeFilter != "Tous") {
        query.bindValue(":type", typeFilter);
    }
    if (categoryFilter != "Toutes") {
        query.bindValue(":category", categoryFilter);
    }
    if (!query.exec()) {
        QMessageBox::critical(this, "Erreur", "Tri échoué:\n" + query.lastError().text());
        return;
    }

    int rowCount = 0;
    while (query.next()) {
        tableTransaction->insertRow(rowCount);
        for(int i = 0; i < 7; i++) {
            QString val = query.value(i).toString();
            if(i == 3) val += " DT";  // Ajoute " DT" au montant
            QTableWidgetItem *item = new QTableWidgetItem(val);
            item->setTextAlignment(Qt::AlignCenter);
            tableTransaction->setItem(rowCount, i, item);
        }
        rowCount++;
    }
}

void TransactionTab::exporter(const QString &format)
{
    if (format == "PDF") {
        QString downloadPath = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
        QString fileName = downloadPath + "/Transactions_" + QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss") + ".pdf";

        QPrinter printer(QPrinter::PrinterResolution);
        printer.setOutputFormat(QPrinter::PdfFormat);
        printer.setPageSize(QPageSize(QPageSize::A4));
        printer.setOutputFileName(fileName);

        QTextDocument doc;
        QString html = "<html><head><style>table{width:100%;border-collapse:collapse;}th{background-color:#1B4332;color:white;padding:8px;}td{padding:8px;border:1px solid #ddd;}</style></head><body>"
                       "<h2 style='text-align:center;color:#1B4332;'>RAPPORT FINANCIER</h2>"
                       "<table><tr><th>ID</th><th>Type</th><th>Catégorie</th><th>Montant</th><th>Date</th><th>Employé</th><th>Description</th></tr>";

        for (int i = 0; i < tableTransaction->rowCount(); ++i) {
            html += "<tr>";
            for (int j = 0; j < 7; ++j) {
                html += "<td>" + tableTransaction->item(i, j)->text() + "</td>";
            }
            html += "</tr>";
        }
        html += "</table></body></html>";

        doc.setHtml(html);
        doc.print(&printer);
        
        QMessageBox::information(this, "Succès", QString::fromUtf8("PDF exporté : ") + QFileInfo(fileName).fileName());
        QDesktopServices::openUrl(QUrl::fromLocalFile(fileName));
    } else if (format == "CSV") {
        QString downloadPath = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
        QString fileName = downloadPath + "/Transactions_" + QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss") + ".csv";

        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            out << "ID;Type;Categorie;Montant;Date;Employe;Description\n";
            for (int i = 0; i < tableTransaction->rowCount(); ++i) {
                for (int j = 0; j < 7; ++j) {
                    out << tableTransaction->item(i, j)->text() << (j == 6 ? "" : ";");
                }
                out << "\n";
            }
            file.close();
            QMessageBox::information(this, "Succès", QString::fromUtf8("CSV exporté : ") + QFileInfo(fileName).fileName());
            QDesktopServices::openUrl(QUrl::fromLocalFile(fileName));
        }
    } else if (format == "TXT") {
        QString downloadPath = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
        QString fileName = downloadPath + "/Transactions_" + QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss") + ".txt";

        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            out << "========== RAPPORT FINANCIER ==========\n\n";
            for (int i = 0; i < tableTransaction->rowCount(); ++i) {
                out << "Transaction ID: " << tableTransaction->item(i, 0)->text() << "\n";
                out << "Type: " << tableTransaction->item(i, 1)->text() << "\n";
                out << "Montant: " << tableTransaction->item(i, 3)->text() << "\n";
                out << "Date: " << tableTransaction->item(i, 4)->text() << "\n\n";
            }
            file.close();
            QMessageBox::information(this, "Succès", QString::fromUtf8("TXT exporté : ") + QFileInfo(fileName).fileName());
            QDesktopServices::openUrl(QUrl::fromLocalFile(fileName));
        }
    }
}

void TransactionTab::chargerData()
{
    cbMachine->clear();
    QSqlDatabase db = dbConn->getDatabase();
    if (!db.isOpen()) return;

    // Charger Machines depuis la vraie table MACHINE
    QSqlQuery qm("SELECT ID_MACHINE, NOM_MACHINE FROM MACHINE ORDER BY NOM_MACHINE", db);
    bool machineFound = false;
    while (qm.next()) {
        cbMachine->addItem(qm.value(1).toString(), qm.value(0).toInt());
        machineFound = true;
    }

    // Fallback si la table MACHINE est vide ou inaccessible
    if (!machineFound) {
        cbMachine->addItem("Presse Principale", 1);
        cbMachine->addItem("Broyeur Alpha", 2);
        cbMachine->addItem("Centrifugeuse Beta", 3);
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
    mainLayout->setContentsMargins(10, 5, 10, 5);

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
    summaryGroup = new QGroupBox(QString::fromUtf8("📊 Résumé Financier"), this);
    summaryGroup->setStyleSheet("QGroupBox { font-weight: bold; font-size: 11pt; color: #1B4332; border: 2px solid #1B7331; border-radius: 6px; padding: 12px; background-color: #f9fffe; }");

    QGridLayout *gridLayout = new QGridLayout(summaryGroup);
    gridLayout->setSpacing(10);
    gridLayout->setContentsMargins(8, 8, 8, 8);

    // Card 1: Revenus
    QVBoxLayout *revLayout = new QVBoxLayout();
    lblRevenuTitle = new QLabel(QString::fromUtf8("Total Revenus"));
    lblRevenuTitle->setStyleSheet("font-weight: bold; font-size: 11pt; color: #1B4332;");
    lblTotalRevenus = new QLabel("0.00 DT");
    lblTotalRevenus->setStyleSheet("font-weight: bold; font-size: 16pt; color: #27ae60;");
    lblTotalRevenus->setAlignment(Qt::AlignCenter);
    revLayout->addWidget(lblRevenuTitle);
    revLayout->addWidget(lblTotalRevenus);

    revCard = new QWidget();
    revCard->setLayout(revLayout);
    revCard->setStyleSheet("background-color: #f0f8f4; border: 2px solid #27ae60; border-radius: 8px; padding: 15px;");

    // Card 2: Dépenses
    QVBoxLayout *depLayout = new QVBoxLayout();
    lblDepenseTitle = new QLabel(QString::fromUtf8("Total Dépenses"));
    lblDepenseTitle->setStyleSheet("font-weight: bold; font-size: 11pt; color: #1B4332;");
    lblTotalDepenses = new QLabel("0.00 DT");
    lblTotalDepenses->setStyleSheet("font-weight: bold; font-size: 16pt; color: #e74c3c;");
    lblTotalDepenses->setAlignment(Qt::AlignCenter);
    depLayout->addWidget(lblDepenseTitle);
    depLayout->addWidget(lblTotalDepenses);

    depCard = new QWidget();
    depCard->setLayout(depLayout);
    depCard->setStyleSheet("background-color: #fef5f5; border: 2px solid #e74c3c; border-radius: 8px; padding: 15px;");

    // Card 3: Bénéfice
    QVBoxLayout *benLayout = new QVBoxLayout();
    lblBeneficeTitle = new QLabel(QString::fromUtf8("Bénéfice Net"));
    lblBeneficeTitle->setStyleSheet("font-weight: bold; font-size: 11pt; color: #1B4332;");
    lblBenefice = new QLabel("0.00 DT");
    lblBenefice->setStyleSheet("font-weight: bold; font-size: 16pt; color: #1B4332;");
    lblBenefice->setAlignment(Qt::AlignCenter);
    benLayout->addWidget(lblBeneficeTitle);
    benLayout->addWidget(lblBenefice);

    benCard = new QWidget();
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
    controlGroup = new QGroupBox(QString::fromUtf8("⚙️ Configuration du Graphique"), this);
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
}

QGroupBox* StatsTab::createChartSection()
{
    chartGroup = new QGroupBox(QString::fromUtf8("📈 Statistiques"), this);
    chartGroup->setStyleSheet("QGroupBox { font-weight: bold; font-size: 11pt; color: #1B4332; border: 2px solid #1B7331; border-radius: 6px; padding: 10px; background-color: #fafafa; }");
    QVBoxLayout *chartLayout = new QVBoxLayout(chartGroup);
    chartLayout->setContentsMargins(8, 8, 8, 8);
    chartLayout->setSpacing(0);

    // Graphiques remplacés par des statistiques texte
    QLabel *statsLabel = new QLabel();
    statsLabel->setText(QString::fromUtf8("Graphiques - Disponible après génération"));
    statsLabel->setMinimumHeight(280);
    statsLabel->setAlignment(Qt::AlignCenter);
    statsLabel->setStyleSheet("background-color: #ffffff; border: 2px solid #1B7331; border-radius: 4px; color: #999;");

    chartLayout->addWidget(statsLabel);

    return chartGroup;
}

void StatsTab::setupConnections()
{
    connect(btnGenerate, &QPushButton::clicked, this, &StatsTab::afficherGraphique);
}

void StatsTab::setDarkMode(bool isDark)
{
    m_isDarkMode = isDark;
    applyDarkModeStyles();
}

void StatsTab::applyDarkModeStyles()
{
    if (!summaryGroup || !controlGroup) return;

    if (m_isDarkMode) {
        // DARK MODE
        // Summary Group
        summaryGroup->setStyleSheet("QGroupBox { font-weight: bold; font-size: 11pt; color: #e6edf3; border: 2px solid #30363d; border-radius: 6px; padding: 12px; background-color: #0d1117; }");
        
        // Summary Cards - Dark theme
        revCard->setStyleSheet("background-color: #1c3d2b; border: 2px solid #1B7331; border-radius: 8px; padding: 15px;");
        if (lblRevenuTitle) lblRevenuTitle->setStyleSheet("font-weight: bold; font-size: 11pt; color: #e6edf3;");
        if (lblTotalRevenus) lblTotalRevenus->setStyleSheet("font-weight: bold; font-size: 16pt; color: #4ade80;");
        
        depCard->setStyleSheet("background-color: #3d1c1c; border: 2px solid #c1121f; border-radius: 8px; padding: 15px;");
        if (lblDepenseTitle) lblDepenseTitle->setStyleSheet("font-weight: bold; font-size: 11pt; color: #e6edf3;");
        if (lblTotalDepenses) lblTotalDepenses->setStyleSheet("font-weight: bold; font-size: 16pt; color: #ff6b6b;");
        
        benCard->setStyleSheet("background-color: #1c2d3d; border: 2px solid #30363d; border-radius: 8px; padding: 15px;");
        if (lblBeneficeTitle) lblBeneficeTitle->setStyleSheet("font-weight: bold; font-size: 11pt; color: #e6edf3;");
        if (lblBenefice) lblBenefice->setStyleSheet("font-weight: bold; font-size: 16pt; color: #e6edf3;");
        
        // Control Group
        controlGroup->setStyleSheet("QGroupBox { font-weight: bold; font-size: 11pt; color: #e6edf3; border: 2px solid #30363d; border-radius: 6px; padding: 8px; background-color: #0d1117; } "
                                   "QLabel { color: #e6edf3; } "
                                   "QComboBox { padding: 3px 6px; border-radius: 4px; background-color: #1c2d3d; border: 2px solid #30363d; font-size: 9pt; font-weight: bold; color: #e6edf3; } "
                                   "QComboBox:focus { border: 2px solid #58a6ff; } "
                                   "QPushButton { background-color: #1B7331; color: white; border: 2px solid #0f3e1d; border-radius: 4px; font-weight: bold; font-size: 9pt; padding: 2px 6px; } "
                                   "QPushButton:hover { background-color: #155c2b; } "
                                   "QPushButton:pressed { background-color: #0f3e1d; }");
        
        // Chart Group
        if (chartGroup) chartGroup->setStyleSheet("QGroupBox { font-weight: bold; font-size: 11pt; color: #e6edf3; border: 2px solid #30363d; border-radius: 6px; padding: 10px; background-color: #0d1117; }");
        
        setStyleSheet("background-color: #0d1117;");
    } else {
        // LIGHT MODE
        // Summary Group
        summaryGroup->setStyleSheet("QGroupBox { font-weight: bold; font-size: 11pt; color: #1B4332; border: 2px solid #1B7331; border-radius: 6px; padding: 12px; background-color: #f9fffe; }");
        
        // Summary Cards - Light theme
        revCard->setStyleSheet("background-color: #f0f8f4; border: 2px solid #27ae60; border-radius: 8px; padding: 15px;");
        if (lblRevenuTitle) lblRevenuTitle->setStyleSheet("font-weight: bold; font-size: 11pt; color: #1B4332;");
        if (lblTotalRevenus) lblTotalRevenus->setStyleSheet("font-weight: bold; font-size: 16pt; color: #27ae60;");
        
        depCard->setStyleSheet("background-color: #fef5f5; border: 2px solid #e74c3c; border-radius: 8px; padding: 15px;");
        if (lblDepenseTitle) lblDepenseTitle->setStyleSheet("font-weight: bold; font-size: 11pt; color: #1B4332;");
        if (lblTotalDepenses) lblTotalDepenses->setStyleSheet("font-weight: bold; font-size: 16pt; color: #e74c3c;");
        
        benCard->setStyleSheet("background-color: #f5f9f8; border: 2px solid #1B4332; border-radius: 8px; padding: 15px;");
        if (lblBeneficeTitle) lblBeneficeTitle->setStyleSheet("font-weight: bold; font-size: 11pt; color: #1B4332;");
        if (lblBenefice) lblBenefice->setStyleSheet("font-weight: bold; font-size: 16pt; color: #1B4332;");
        
        // Control Group
        controlGroup->setStyleSheet("QGroupBox { font-weight: bold; font-size: 11pt; color: #1B4332; border: 2px solid #1B7331; border-radius: 6px; padding: 8px; background-color: #f0f8f5; } "
                                   "QLabel { color: #1B4332; } "
                                   "QComboBox { padding: 3px 6px; border-radius: 4px; background-color: #e8f5e9; border: 2px solid #1B7331; font-size: 9pt; font-weight: bold; color: #1B4332; } "
                                   "QComboBox:focus { border: 2px solid #155c2b; } "
                                   "QPushButton { background-color: #1B7331; color: white; border: 2px solid #0f3e1d; border-radius: 4px; font-weight: bold; font-size: 9pt; padding: 2px 6px; } "
                                   "QPushButton:hover { background-color: #155c2b; } "
                                   "QPushButton:pressed { background-color: #0f3e1d; }");
        
        // Chart Group
        if (chartGroup) chartGroup->setStyleSheet("QGroupBox { font-weight: bold; font-size: 11pt; color: #1B4332; border: 2px solid #1B7331; border-radius: 6px; padding: 10px; background-color: #fafafa; }");
        
        setStyleSheet("background-color: #f5f5f5;");
    }
}

void StatsTab::calculerStats()
{
    double revenus = 0.0;
    double depenses = 0.0;

    QSqlDatabase db = dbConn->getDatabase();
    if (!db.isOpen()) return;

    QSqlQuery query(db);
    // Calculate Revenus (Normalisation UPPER pour éviter les erreurs de casse)
    if (query.exec("SELECT SUM(montant) FROM finance WHERE UPPER(ctype) = 'REVENU' AND TYPE_LIGNE = 'TRANSACTION'")) {
        if (query.next()) {
            revenus = query.value(0).toDouble();
        }
    }
    
    // Calculate Depenses
    if (query.exec("SELECT SUM(montant) FROM finance WHERE UPPER(ctype) IN ('DEPENSE', 'DÉPENSE') AND TYPE_LIGNE = 'TRANSACTION'")) {
        if (query.next()) {
            depenses = query.value(0).toDouble();
        }
    }

    double benefice = revenus - depenses;

    // Update UI
    lblTotalRevenus->setText(QString::number(revenus, 'f', 2) + " DT");
    lblTotalDepenses->setText(QString::number(depenses, 'f', 2) + " DT");
    lblBenefice->setText(QString::number(benefice, 'f', 2) + " DT");
    
    // Color logic for benefice
    if (benefice >= 0) {
        lblBenefice->setStyleSheet("font-weight: bold; font-size: 16pt; color: #27ae60;");
    } else {
        lblBenefice->setStyleSheet("font-weight: bold; font-size: 16pt; color: #e74c3c;");
    }

    // Appel automatique de l'affichage du graphique pour synchroniser
    afficherGraphique();
}

#include <QtCharts/QChartView>
#include <QtCharts/QPieSeries>
#include <QtCharts/QPieSlice>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>

void StatsTab::afficherGraphique()
{
    if (!chartGroup || !chartGroup->layout()) return;
    
    QLayout *layout = chartGroup->layout();
    
    // Clear existing widgets
    QLayoutItem *child;
    while ((child = layout->takeAt(0)) != nullptr) {
        if (child->widget()) {
            child->widget()->deleteLater();
        }
        delete child;
    }
    
    QChart *chart = new QChart();
    
    QSqlDatabase db = dbConn->getDatabase();
    if (!db.isOpen()) return;
    
    int type = cbChartType->currentIndex();
    QString titleText;

    if (type == 0) {
        // 📈 Camembert : Revenus vs Dépenses Globaux
        double revenus = 0.0;
        double depenses = 0.0;
        
        QSqlQuery query(db);
        if (query.exec("SELECT SUM(montant) FROM finance WHERE UPPER(ctype) = 'REVENU' AND TYPE_LIGNE = 'TRANSACTION'")) {
            if (query.next()) revenus = query.value(0).toDouble();
        }
        if (query.exec("SELECT SUM(montant) FROM finance WHERE UPPER(ctype) IN ('DEPENSE', 'DÉPENSE') AND TYPE_LIGNE = 'TRANSACTION'")) {
            if (query.next()) depenses = query.value(0).toDouble();
        }
        
        double total = revenus + depenses;
        
        QPieSeries *series = new QPieSeries();
        QPieSlice *sliceRev = new QPieSlice("Revenus", revenus);
        sliceRev->setBrush(QColor("#27ae60")); // Vert
        
        QPieSlice *sliceDep = new QPieSlice(QString::fromUtf8("Dépenses"), depenses);
        sliceDep->setBrush(QColor("#e74c3c")); // Rouge
        
        if (total > 0) {
            double pctRev = (revenus / total) * 100.0;
            double pctDep = (depenses / total) * 100.0;
            sliceRev->setLabel(QString("Revenus: %1%").arg(QString::number(pctRev, 'f', 1)));
            sliceDep->setLabel(QString::fromUtf8("Dépenses: %1%").arg(QString::number(pctDep, 'f', 1)));
        }
        
        sliceRev->setLabelVisible(true);
        sliceDep->setLabelVisible(true);
        
        series->append(sliceRev);
        series->append(sliceDep);
        
        chart->addSeries(series);
        titleText = QString::fromUtf8("Répartition Globale (Camembert)");
    } 
    else if (type == 1) {
        // 📊 Histogramme : Revenus vs Dépenses par type (Mensuel)
        QBarSeries *series = new QBarSeries();
        QBarSet *setRev = new QBarSet("Revenus");
        QBarSet *setDep = new QBarSet(QString::fromUtf8("Dépenses"));
        setRev->setColor(QColor("#27ae60"));
        setDep->setColor(QColor("#e74c3c"));
        
        QStringList categories;
        
        QSqlQuery q(db);
        // Compatible avec Oracle et SQLite
        bool isOracle = db.driverName().contains("QOCI") || db.driverName().contains("QODBC");
        QString dateExpr = isOracle ? "TO_CHAR(date_trans, 'YYYY-MM')" : "strftime('%Y-%m', date_trans)";
        
        q.exec(QString("SELECT %1 AS mois, "
               "SUM(CASE WHEN UPPER(CTYPE)='REVENU' THEN MONTANT ELSE 0 END) AS rev, "
               "SUM(CASE WHEN UPPER(CTYPE) IN ('DEPENSE','DÉPENSE') THEN MONTANT ELSE 0 END) AS dep "
               "FROM FINANCE WHERE TYPE_LIGNE='TRANSACTION' "
               "GROUP BY %1 ORDER BY mois ASC").arg(dateExpr));
               
        double maxVal = 0;
        int count = 0;
        while(q.next() && count < 12) {
            QString mois = q.value(0).toString();
            double rev = q.value(1).toDouble();
            double dep = q.value(2).toDouble();
            
            categories << mois;
            *setRev << rev;
            *setDep << dep;
            
            if (rev > maxVal) maxVal = rev;
            if (dep > maxVal) maxVal = dep;
            count++;
        }
        if (categories.isEmpty()) { categories << "N/A"; *setRev << 0; *setDep << 0; }
        if (maxVal == 0) maxVal = 100;
        
        series->append(setRev);
        series->append(setDep);
        series->setLabelsVisible(true);
        
        chart->addSeries(series);
        
        QBarCategoryAxis *axisX = new QBarCategoryAxis();
        if (categories.isEmpty()) categories << "N/A";
        axisX->append(categories);
        chart->addAxis(axisX, Qt::AlignBottom);
        series->attachAxis(axisX);
        
        QValueAxis *axisY = new QValueAxis();
        axisY->setLabelFormat("%.0f DT");
        axisY->setRange(0, maxVal * 1.1);
        chart->addAxis(axisY, Qt::AlignLeft);
        series->attachAxis(axisY);
        
        titleText = QString::fromUtf8("Évolution Revenus vs Dépenses (Histogramme)");
    }
    else if (type == 2) {
        // 📉 Courbe : Évolution du Bénéfice
        QLineSeries *series = new QLineSeries();
        series->setName(QString::fromUtf8("Bénéfice (Revenus - Dépenses)"));
        QPen pen(QColor("#3498db"));
        pen.setWidth(4);
        series->setPen(pen);
        
        QStringList categories;
        
        QSqlQuery q(db);
        bool isOracle = db.driverName().contains("QOCI") || db.driverName().contains("QODBC");
        QString dateExpr = isOracle ? "TO_CHAR(date_trans, 'YYYY-MM')" : "strftime('%Y-%m', date_trans)";
        
        q.exec(QString("SELECT %1 AS mois, "
               "SUM(CASE WHEN UPPER(CTYPE)='REVENU' THEN MONTANT ELSE -MONTANT END) AS benefice "
               "FROM FINANCE WHERE TYPE_LIGNE='TRANSACTION' "
               "GROUP BY %1 ORDER BY mois ASC").arg(dateExpr));
               
        int idx = 0;
        double minVal = 0, maxVal = 0;
        while(q.next() && idx < 12) {
            QString mois = q.value(0).toString();
            double benefice = q.value(1).toDouble();
            
            categories << mois;
            series->append(idx, benefice);
            
            if (idx == 0) { minVal = maxVal = benefice; }
            else {
                if (benefice < minVal) minVal = benefice;
                if (benefice > maxVal) maxVal = benefice;
            }
            idx++;
        }
        if (categories.isEmpty()) { categories << "N/A"; series->append(0, 0); }
        
        chart->addSeries(series);
        
        QBarCategoryAxis *axisX = new QBarCategoryAxis();
        if (categories.isEmpty()) categories << "N/A";
        axisX->append(categories);
        chart->addAxis(axisX, Qt::AlignBottom);
        series->attachAxis(axisX);
        
        QValueAxis *axisY = new QValueAxis();
        axisY->setLabelFormat("%.0f DT");
        
        double padding = (maxVal - minVal) * 0.1;
        if (padding == 0) padding = 100;
        axisY->setRange(minVal - padding, maxVal + padding);
        
        chart->addAxis(axisY, Qt::AlignLeft);
        series->attachAxis(axisY);
        
        titleText = QString::fromUtf8("Évolution du Bénéfice (Courbe)");
    }
    else if (type == 3) {
        // 📋 Comparatif : Comparatif global
        QBarSeries *series = new QBarSeries();
        QBarSet *setRev = new QBarSet("Total Revenus");
        QBarSet *setDep = new QBarSet(QString::fromUtf8("Total Dépenses"));
        QBarSet *setBen = new QBarSet(QString::fromUtf8("Bénéfice Net"));
        
        setRev->setColor(QColor("#27ae60"));
        setDep->setColor(QColor("#e74c3c"));
        setBen->setColor(QColor("#3498db"));
        
        double revenus = 0.0;
        double depenses = 0.0;
        QSqlQuery query(db);
        if (query.exec("SELECT SUM(montant) FROM finance WHERE UPPER(ctype) = 'REVENU' AND TYPE_LIGNE = 'TRANSACTION'")) {
            if (query.next()) revenus = query.value(0).toDouble();
        }
        if (query.exec("SELECT SUM(montant) FROM finance WHERE UPPER(ctype) IN ('DEPENSE', 'DÉPENSE') AND TYPE_LIGNE = 'TRANSACTION'")) {
            if (query.next()) depenses = query.value(0).toDouble();
        }
        double benefice = revenus - depenses;
        
        *setRev << revenus;
        *setDep << depenses;
        *setBen << benefice;
        
        float zeroFixedBen = benefice < 0 ? 0 : benefice;
        
        series->append(setRev);
        series->append(setDep);
        series->append(setBen);
        series->setLabelsVisible(true);
        
        chart->addSeries(series);
        
        QBarCategoryAxis *axisX = new QBarCategoryAxis();
        axisX->append(QStringList() << "Bilan Global");
        chart->addAxis(axisX, Qt::AlignBottom);
        series->attachAxis(axisX);
        
        QValueAxis *axisY = new QValueAxis();
        axisY->setLabelFormat("%.0f DT");
        
        double maxVal = revenus > depenses ? revenus : depenses;
        if (benefice > maxVal) maxVal = benefice;
        double minVal = benefice < 0 ? benefice : 0;
        if (maxVal == 0 && minVal == 0) maxVal = 100;
        axisY->setRange(minVal * 1.15, maxVal * 1.15);
        
        chart->addAxis(axisY, Qt::AlignLeft);
        series->attachAxis(axisY);
        
        titleText = QString::fromUtf8("Comparatif Global : Revenus, Dépenses et Bénéfice");
    }
    
    chart->setTitle(titleText);
    chart->setAnimationOptions(QChart::SeriesAnimations);
    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);
    
    chart->setBackgroundBrush(QBrush(QColor("#ffffff")));
    
    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setMinimumHeight(280); // Hauteur ajustée (plus petit)
    chartView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    
    layout->addWidget(chartView);
}

void StatsTab::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    calculerStats();
}

// ========================================================================
// IMPLÉMENTATION: AdvancedTab
// ========================================================================
// L'implémentation de AdvancedTab a été déplacée dans advancedtab.cpp

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
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(20, 20, 20, 20);

    // Style unifié "PressIQ"
    const QString cardStyle = 
        "QGroupBox { background-color: white; border: 1px solid #e0e0e0; border-radius: 8px; margin-top: 15px; font-weight: bold; font-size: 14px; color: #1B4332; } "
        "QGroupBox::title { subcontrol-origin: margin; subcontrol-position: top left; left: 15px; padding: 4px 12px; background-color: white; border: 1px solid #1B4332; border-radius: 10px; }";

    // 1. Zone de Recherche, Tri & Export
    QGroupBox *searchGroup = new QGroupBox(QString::fromUtf8("🔍 Recherche, Tri & Export"), this);
    searchGroup->setStyleSheet(cardStyle);
    searchGroup->setMinimumHeight(220);
    QVBoxLayout *searchLayout = new QVBoxLayout(searchGroup);
    searchLayout->setContentsMargins(20, 30, 20, 20);
    searchLayout->setSpacing(18);

    // --- Ligne 1: Recherche ---
    QHBoxLayout *rowSearch = new QHBoxLayout();
    rowSearch->setSpacing(15);
    QLabel *lblSearch = new QLabel("Rechercher :");
    lblSearch->setMinimumWidth(120);
    lblSearch->setStyleSheet("font-weight: bold;");
    rowSearch->addWidget(lblSearch);
    txtSearch = new QLineEdit();
    txtSearch->setPlaceholderText(QString::fromUtf8("Client, Description, Catégorie..."));
    txtSearch->setMinimumHeight(45);
    rowSearch->addWidget(txtSearch, 3);

    QLabel *lblBy = new QLabel("Par :");
    lblBy->setMinimumWidth(60);
    lblBy->setStyleSheet("font-weight: bold;");
    rowSearch->addWidget(lblBy);
    cbSearchType = new QComboBox();
    cbSearchType->addItems({"Tout", "Client", QString::fromUtf8("Catégorie"), "Description"});
    cbSearchType->setMinimumHeight(45);
    cbSearchType->setMinimumWidth(140);
    rowSearch->addWidget(cbSearchType, 1);
    
    QLabel *lblFrom = new QLabel("Du :");
    lblFrom->setMinimumWidth(50);
    lblFrom->setStyleSheet("font-weight: bold;");
    rowSearch->addWidget(lblFrom);
    dateFrom = new QDateEdit(QDate::currentDate().addMonths(-1));
    dateFrom->setCalendarPopup(true);
    dateFrom->setMinimumHeight(45);
    dateFrom->setDisplayFormat("dd/MM/yyyy");
    dateFrom->setMinimumWidth(150);
    rowSearch->addWidget(dateFrom);

    QLabel *lblTo = new QLabel("Au :");
    lblTo->setMinimumWidth(50);
    lblTo->setStyleSheet("font-weight: bold;");
    rowSearch->addWidget(lblTo);
    dateTo = new QDateEdit(QDate::currentDate());
    dateTo->setCalendarPopup(true);
    dateTo->setMinimumHeight(45);
    dateTo->setDisplayFormat("dd/MM/yyyy");
    dateTo->setMinimumWidth(150);
    rowSearch->addWidget(dateTo);

    btnSearch = new QPushButton(QString::fromUtf8("🔍 Chercher"));
    btnSearch->setMinimumHeight(45);
    btnSearch->setMinimumWidth(150);
    btnSearch->setStyleSheet("background-color: #1B4332; color: white; border-radius: 6px; font-weight: bold; padding: 0 15px; font-size: 13px;");
    rowSearch->addWidget(btnSearch);
    searchLayout->addLayout(rowSearch);

    // --- Ligne 2: Tri & Export ---
    QHBoxLayout *rowAction = new QHBoxLayout();
    rowAction->setSpacing(15);
    QLabel *lblSort = new QLabel("Trier par :");
    lblSort->setMinimumWidth(120);
    lblSort->setStyleSheet("font-weight: bold;");
    rowAction->addWidget(lblSort);
    cbSortBy = new QComboBox();
    cbSortBy->addItems({"Date", "Montant", "Client", QString::fromUtf8("Catégorie")});
    cbSortBy->setMinimumHeight(45);
    cbSortBy->setMinimumWidth(140);
    rowAction->addWidget(cbSortBy);

    cbOrder = new QComboBox();
    cbOrder->addItems({QString::fromUtf8("Décroissant"), "Croissant"});
    cbOrder->setMinimumHeight(45);
    cbOrder->setMinimumWidth(140);
    rowAction->addWidget(cbOrder);

    btnSort = new QPushButton(QString::fromUtf8("↻ Appliquer Tri"));
    btnSort->setMinimumHeight(45);
    btnSort->setMinimumWidth(160);
    btnSort->setStyleSheet("background-color: #2D5A47; color: white; border-radius: 6px; padding: 0 15px; font-weight: bold; font-size: 13px;");
    rowAction->addWidget(btnSort);

    rowAction->addSpacing(40);

    QLabel *lblExport = new QLabel("Export :");
    lblExport->setMinimumWidth(80);
    lblExport->setStyleSheet("font-weight: bold;");
    rowAction->addWidget(lblExport);
    cbExportFormat = new QComboBox();
    cbExportFormat->addItems({"📄 PDF", "📊 Excel", "📝 TXT"});
    cbExportFormat->setMinimumHeight(45);
    cbExportFormat->setMinimumWidth(140);
    rowAction->addWidget(cbExportFormat);

    btnExport = new QPushButton(QString::fromUtf8("💾 Exporter"));
    btnExport->setMinimumHeight(45);
    btnExport->setMinimumWidth(160);
    btnExport->setStyleSheet("background-color: #1B7331; color: white; border-radius: 6px; padding: 0 15px; font-weight: bold; font-size: 13px;");
    rowAction->addWidget(btnExport);
    rowAction->addStretch();

    searchLayout->addLayout(rowAction);

    lblExportStatus = new QLabel("");
    lblExportStatus->setStyleSheet("color: #666; font-size: 12px; font-style: italic;");
    searchLayout->addWidget(lblExportStatus);

    mainLayout->addWidget(searchGroup);

    // 2. Zone Tableau (Lecture seule)
    QGroupBox *tableGroup = new QGroupBox(QString::fromUtf8("📋 Aperçu des Résultats"), this);
    tableGroup->setStyleSheet(cardStyle);
    QVBoxLayout *tableLayout = new QVBoxLayout(tableGroup);
    tableLayout->setContentsMargins(10, 25, 10, 10);

    tableSearch = new QTableWidget();
    tableSearch->setColumnCount(7);
    tableSearch->setHorizontalHeaderLabels({"ID", "Type", QString::fromUtf8("Catégorie"), "Montant", "Date", "Client", "Description"});
    tableSearch->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableSearch->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableSearch->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);
    tableSearch->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents); // ID
    tableSearch->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableSearch->verticalHeader()->setVisible(false);
    tableSearch->verticalHeader()->setDefaultSectionSize(45);
    tableSearch->setStyleSheet(
        "QTableWidget { background-color: white; border: none; gridline-color: #f0f0f0; alternate-background-color: #f4f7f6; } "
        "QHeaderView::section { background-color: #1B4332; color: white; padding: 12px; font-weight: bold; border: none; border-right: 1px solid rgba(255,255,255,0.1); height: 35px; }"
    );

    tableLayout->addWidget(tableSearch);
    mainLayout->addWidget(tableGroup, 1);
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
                        "COALESCE(e.USERNAME, 'N/A') as tiers, f.description "
                        "FROM finance f "
                        "LEFT JOIN EMPLOYEES e ON f.ID_EMPLOYEE = e.USER_ID "
                        "WHERE f.TYPE_LIGNE = 'TRANSACTION' "
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
                  "COALESCE(e.USERNAME, 'N/A') as tiers, f.description "
                  "FROM finance f "
                  "LEFT JOIN EMPLOYEES e ON f.ID_EMPLOYEE = e.USER_ID "
                  "WHERE f.TYPE_LIGNE = 'TRANSACTION' AND TRUNC(f.date_trans) BETWEEN :d1 AND :d2";

    int searchIdx = cbSearchType->currentIndex(); 
    if (!searchText.isEmpty()) {
        if (searchIdx == 1) sql += " AND (LOWER(e.USERNAME) LIKE LOWER(:s))";
        else if (searchIdx == 2) sql += " AND LOWER(f.categorie) LIKE LOWER(:s)";
        else if (searchIdx == 3) sql += " AND LOWER(f.description) LIKE LOWER(:s)";
        else sql += " AND (LOWER(f.ctype) LIKE LOWER(:s) OR LOWER(f.categorie) LIKE LOWER(:s) OR LOWER(f.description) LIKE LOWER(:s) OR LOWER(e.USERNAME) LIKE LOWER(:s))";
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
    query.bindValue(":d1", dateFrom->date());
    query.bindValue(":d2", dateTo->date());
    if (!searchText.isEmpty()) {
        query.bindValue(":s", "%" + searchText + "%");
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
    QString format = cbExportFormat->currentText();
    if (format.contains("PDF")) exportPDF();
    else if (format.contains("CSV")) exportCSV();
    else exportPDF(); 
}

void SearchTab::exportPDF()
{
    lblExportStatus->setText("Génération PDF en cours...");
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
    lblExportStatus->setText(QString::fromUtf8("PDF exporté : ") + QFileInfo(fileName).fileName());
    QDesktopServices::openUrl(QUrl::fromLocalFile(fileName));
}

void SearchTab::exportCSV()
{
    lblExportStatus->setText("Génération CSV en cours...");
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
        lblExportStatus->setText(QString::fromUtf8("CSV exporté : ") + QFileInfo(fileName).fileName());
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
Finance::Finance(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Finance)
    , dateTimeTimer(nullptr)
    , m_isDarkMode(false)
    , btnDarkMode(nullptr)
{
    ui->setupUi(this);
    initializeUI();
    applyStyles();
    
    // Delay the stylesheet capture to ensure all widgets are fully styled
    QTimer::singleShot(100, this, [this]() {
        m_lightStyleSheet = qApp->styleSheet();
    });

    // Initialize DateTime Display
    dateTimeTimer = new QTimer(this);
    connect(dateTimeTimer, &QTimer::timeout, this, &Finance::updateDateTime);
    dateTimeTimer->start(1000);
    updateDateTime();

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
    QDate currentDate = QDate::currentDate();
    QTime currentTime = QTime::currentTime();
    
    QStringList moisFr = {
        "", "janvier", "février", "mars", "avril", "mai", "juin",
        "juillet", "août", "septembre", "octobre", "novembre", "décembre"
    };
    
    QString dateStr = QString("📅 %1 %2 %3")
                          .arg(currentDate.day(), 2, 10, QChar('0'))
                          .arg(moisFr[currentDate.month()])
                          .arg(currentDate.year());
    
    QString timeStr = currentTime.toString("🕐 HH:mm:ss");
    
    if (lblDateTime) {
        lblDateTime->setText(dateStr + " | " + timeStr);
    }
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
    QLabel *lblStatus = new QLabel("EN LIGNE");
    lblStatus->setStyleSheet("color: #4ADE80; font-weight: bold; font-size: 14px; padding: 6px 14px; background-color: rgba(74, 222, 128, 0.15); border-radius: 15px; border: 1px solid rgba(74, 222, 128, 0.3);");

    // Dark Mode Button
    btnDarkMode = new QPushButton(QString::fromUtf8("🌙  Mode Sombre"));
    btnDarkMode->setMinimumWidth(120);
    btnDarkMode->setStyleSheet(
        "QPushButton { "
        "background-color: rgba(255, 255, 255, 0.15); "
        "color: white; "
        "font-size: 12px; "
        "font-weight: 600; "
        "padding: 8px 14px; "
        "border-radius: 6px; "
        "border: 1px solid rgba(255, 255, 255, 0.3); "
        "} "
        "QPushButton:hover { "
        "background-color: rgba(255, 255, 255, 0.25); "
        "border: 1px solid rgba(255, 255, 255, 0.5); "
        "} "
        "QPushButton:pressed { "
        "background-color: rgba(255, 255, 255, 0.1); "
        "}"
    );
    connect(btnDarkMode, &QPushButton::clicked, this, &Finance::onToggleDarkMode);

    // Assembler le header
    headerLayout->addWidget(btnReturn);
    headerLayout->addWidget(lblIcon);
    headerLayout->addLayout(titleLayout);
    headerLayout->addStretch();
    headerLayout->addWidget(btnDarkMode);
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
    transactionTab = new TransactionTab(1);  // userId = 1 (utilisateur par défaut/admin)
    statsTab = new StatsTab();
    advancedTab = new AdvancedTab();
    historiqueAuditTab = new HistoriqueAuditTab();

    // Ajouter les tabs
    mainTabWidget->addTab(transactionTab, QString::fromUtf8("💸 Transactions"));
    mainTabWidget->addTab(statsTab, QString::fromUtf8("📊 Statistiques"));
    mainTabWidget->addTab(advancedTab, QString::fromUtf8("🔬 Analyse Avancée"));
    mainTabWidget->addTab(historiqueAuditTab, QString::fromUtf8("📋 Historique Audit"));

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

// ============================================================================
// DARK MODE SUPPORT
// ============================================================================

void Finance::applyTheme()
{
    static const QString DARK_SS = R"(
        QMainWindow {
            background: #0d1117;
        }
        
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
            background-color: #21262d;
            color: #e6edf3;
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
            background-color: #30363d;
            color: #e6edf3;
        }

        QGroupBox {
            font-weight: bold;
            border: 2px solid #30363d;
            border-radius: 8px;
            margin-top: 10px;
            padding-top: 10px;
            background-color: #161b22;
        }

        QGroupBox::title {
            subcontrol-origin: margin;
            left: 10px;
            padding: 0 5px;
            color: #e6edf3;
        }

        QLineEdit, QSpinBox, QDoubleSpinBox, QDateEdit, QComboBox {
            border: 1px solid #30363d;
            border-radius: 4px;
            padding: 6px;
            background: #0d1117;
            color: #e6edf3;
            font-size: 13px;
        }

        QLineEdit:focus, QSpinBox:focus, QDoubleSpinBox:focus, QDateEdit:focus, QComboBox:focus {
            border: 2px solid #58a6ff;
        }

        QComboBox::drop-down {
            border: none;
            padding-right: 15px;
            width: 30px;
        }

        QComboBox QAbstractItemView {
            border: 2px solid #30363d;
            border-radius: 8px;
            background-color: #0d1117;
            selection-background-color: #1e3a5f;
            selection-color: #93c5fd;
            padding: 5px;
        }

        QTextEdit, QTextBrowser {
            border: 2px solid #30363d;
            border-radius: 8px;
            background-color: #0d1117;
            color: #e6edf3;
            font-size: 14px;
            font-family: 'Segoe UI', Arial, sans-serif;
            padding: 12px;
        }

        QTextEdit:focus, QTextBrowser:focus {
            border: 2px solid #58a6ff;
            background-color: #0d1117;
        }

        QPushButton {
            background-color: #1B4332;
            color: white;
            border: none;
            padding: 8px 16px;
            border-radius: 4px;
            font-weight: bold;
        }

        QPushButton:hover {
            background-color: #2D5A45;
        }

        QPushButton:pressed {
            background-color: #0F2A1F;
        }

        QProgressBar {
            border: none;
            border-radius: 10px;
            text-align: center;
            font-weight: 700;
            font-size: 13px;
            background-color: #21262d;
            min-height: 24px;
            color: #e6edf3;
        }

        QProgressBar::chunk {
            background: qlineargradient(x1:0,y1:0,x2:1,y2:0,
                        stop:0 #1B4332,stop:0.5 #234E3E,stop:1 #1B4332);
            border-radius: 10px;
        }

        QTableWidget {
            border: 1px solid #30363d;
            border-radius: 4px;
            gridline-color: #30363d;
            background-color: #0d1117;
            alternate-background-color: #161b22;
        }

        QTableWidget::item {
            padding: 12px;
            border-bottom: 1px solid #21262d;
            color: #e6edf3;
            background-color: #0d1117;
        }

        QTableWidget::item:selected {
            background-color: #1e3a5f;
            color: #93c5fd;
        }

        QTableWidget::item:hover {
            background-color: #21262d;
        }

        QHeaderView {
            background: #1B4332;
            background-color: #1B4332;
            border: none;
            border-radius: 8px;
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

        QLabel {
            color: #e6edf3;
            font-size: 14px;
            font-family: 'Segoe UI', Arial, sans-serif;
        }

        QCheckBox {
            font-size: 14px;
            font-family: 'Segoe UI', Arial, sans-serif;
            color: #e6edf3;
            spacing: 12px;
            padding: 8px;
        }

        QCheckBox::indicator {
            width: 22px;
            height: 22px;
            border-radius: 6px;
        }

        QCheckBox::indicator:unchecked {
            background-color: #21262d;
            border: 2px solid #30363d;
        }

        QCheckBox::indicator:unchecked:hover {
            border: 2px solid #58a6ff;
        }

        QCheckBox::indicator:checked {
            background-color: #1B4332;
            border: 2px solid #1B4332;
        }

        QScrollBar:vertical {
            border: none;
            background: #0d1117;
            width: 10px;
            border-radius: 5px;
            margin: 0;
        }

        QScrollBar::handle:vertical {
            background: #30363d;
            border-radius: 5px;
            min-height: 30px;
        }

        QScrollBar::handle:vertical:hover {
            background: #484f58;
        }

        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
            height: 0;
        }

        QScrollBar::horizontal {
            border: none;
            background: #0d1117;
            height: 10px;
            border-radius: 5px;
        }

        QScrollBar::handle:horizontal {
            background: #30363d;
            border-radius: 5px;
            min-width: 30px;
        }

        QScrollBar::handle:horizontal:hover {
            background: #484f58;
        }
    )";

    if (m_isDarkMode) {
        qApp->setStyleSheet(DARK_SS);
    } else {
        qApp->setStyleSheet(m_lightStyleSheet);
    }
    
    // Propagate dark mode to Statistics tab
    if (statsTab) statsTab->setDarkMode(m_isDarkMode);
    if (advancedTab) advancedTab->setDarkMode(m_isDarkMode);
    if (historiqueAuditTab) historiqueAuditTab->setDarkMode(m_isDarkMode);
}

void Finance::onToggleDarkMode()
{
    m_isDarkMode = !m_isDarkMode;
    
    // Update button text and style
    if (btnDarkMode) {
        btnDarkMode->setText(m_isDarkMode ? QString::fromUtf8("☀  Mode Clair") : QString::fromUtf8("🌙  Mode Sombre"));
        
        // Update button style based on mode
        if (m_isDarkMode) {
            btnDarkMode->setStyleSheet(
                "QPushButton { "
                "background-color: rgba(230, 237, 243, 0.15); "
                "color: #e6edf3; "
                "font-size: 12px; "
                "font-weight: 600; "
                "padding: 8px 14px; "
                "border-radius: 6px; "
                "border: 1px solid rgba(230, 237, 243, 0.3); "
                "} "
                "QPushButton:hover { "
                "background-color: rgba(230, 237, 243, 0.25); "
                "border: 1px solid rgba(230, 237, 243, 0.5); "
                "} "
                "QPushButton:pressed { "
                "background-color: rgba(230, 237, 243, 0.1); "
                "}"
            );
        } else {
            btnDarkMode->setStyleSheet(
                "QPushButton { "
                "background-color: rgba(255, 255, 255, 0.15); "
                "color: white; "
                "font-size: 12px; "
                "font-weight: 600; "
                "padding: 8px 14px; "
                "border-radius: 6px; "
                "border: 1px solid rgba(255, 255, 255, 0.3); "
                "} "
                "QPushButton:hover { "
                "background-color: rgba(255, 255, 255, 0.25); "
                "border: 1px solid rgba(255, 255, 255, 0.5); "
                "} "
                "QPushButton:pressed { "
                "background-color: rgba(255, 255, 255, 0.1); "
                "}"
            );
        }
    }
    
    applyTheme();
}
