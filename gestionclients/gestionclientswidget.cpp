#include "gestionclientswidget.h"
#include "ui_gestionclientswidget.h"
#include <QMessageBox>
#include <QTableWidgetItem>
#include <QBrush>
#include <QRegularExpression>
#include <QMenu>
#include <QWidgetAction>
#include <QLabel>
#include <QTimer>
#include <QClipboard>
#include <QApplication>
#include <QGraphicsDropShadowEffect>
#include <QAbstractItemView>

namespace {
struct SmartAction {
    QString action;
    QString message;
};

QString inferFidelite(double totalOlives)
{
    if (totalOlives >= 5000) return "haute";
    if (totalOlives >= 2000) return "moyenne";
    return "faible";
}

SmartAction computeSmartAction(const QString &prenom, const QString &type,
                               const QString &statut, double totalOlives,
                               const QDate &dateCreation)
{
    const QString fidelite = inferFidelite(totalOlives);
    const bool isImportant = (statut.compare("Important", Qt::CaseInsensitive) == 0);
    const bool isStandard = (statut.compare("Standard", Qt::CaseInsensitive) == 0);
    const bool isSociete = (type.compare("Société", Qt::CaseInsensitive) == 0);
    const bool isVolume = (totalOlives > 1000000);
    const bool isRecent = dateCreation.isValid() && dateCreation.daysTo(QDate::currentDate()) < 30;
    const bool isAncien = dateCreation.isValid() && dateCreation.daysTo(QDate::currentDate()) > 365;

    if (isVolume) {
        return {"🎁 Offre volume",
                QString("Bonjour %1, merci pour votre volume. Une offre spéciale vous attend.")
                    .arg(prenom)};
    }
    if (isRecent) {
        return {"👋 Bienvenue",
                QString("Bonjour %1, bienvenue chez nous ! Voici votre offre de bienvenue.")
                    .arg(prenom)};
    }
    if (isImportant && fidelite == "haute") {
        return {"📞 Appel VIP",
                QString("Bonjour %1, client fidèle, offre spéciale pour vous. On peut vous appeler quand vous voulez.")
                    .arg(prenom)};
    }
    if (isImportant && fidelite == "moyenne") {
        return {"💬 SMS relance",
                QString("Bonjour %1, une offre est en cours. Répondez à ce message pour en profiter.")
                    .arg(prenom)};
    }
    if (isSociete && isStandard) {
        return {"📧 Email offre société",
                QString("Bonjour %1, offre spéciale pour votre société. Souhaitez-vous recevoir le devis par email ?")
                    .arg(prenom)};
    }
    if (isStandard && isAncien) {
        return {"🔔 Réactivation",
                QString("Bonjour %1, cela fait un moment. Souhaitez-vous reprendre vos livraisons d'olives ?")
                    .arg(prenom)};
    }

    return {"💬 Suivi client",
            QString("Bonjour %1, nous restons à votre disposition pour vos prochaines livraisons.")
                .arg(prenom)};
}
}

GestionClientsWidget::GestionClientsWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::GestionClientsWidget)
    , nextClientId(1)
    , idClientSelectionne(-1)
{
    ui->setupUi(this);
    currentSelectedClient.id_client = -1;

    ui->tableWidget_clients->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget_clients->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableWidget_clients->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->tableWidget_clients, &QTableWidget::customContextMenuRequested,
            this, &GestionClientsWidget::on_tableWidget_clients_customContextMenuRequested);

    // Initialiser le dateEdit à la date du jour
    ui->dateEdit_creation->setDate(QDate::currentDate());

    // Désactiver les boutons Modifier et Supprimer au démarrage
    desactiverBoutons();

    // Connecter le signal textChanged de lineEdit_total à calculerStatutEnTempsReel
    connect(ui->lineEdit_total, &QLineEdit::textChanged, this, &GestionClientsWidget::calculerStatutEnTempsReel);

    setupValidators();
    connect(ui->lineEdit_id, &QLineEdit::textChanged, this, &GestionClientsWidget::validateRealtime);
    connect(ui->lineEdit_nom, &QLineEdit::textChanged, this, &GestionClientsWidget::validateRealtime);
    connect(ui->lineEdit_prenom, &QLineEdit::textChanged, this, &GestionClientsWidget::validateRealtime);
    connect(ui->lineEdit_tel, &QLineEdit::textChanged, this, &GestionClientsWidget::validateRealtime);
    connect(ui->lineEdit_adresse, &QLineEdit::textChanged, this, &GestionClientsWidget::validateRealtime);
    connect(ui->lineEdit_email, &QLineEdit::textChanged, this, &GestionClientsWidget::validateRealtime);
    connect(ui->lineEdit_total, &QLineEdit::textChanged, this, &GestionClientsWidget::validateRealtime);
    connect(ui->comboBox_type, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &GestionClientsWidget::validateRealtime);

    // Charger les clients depuis la base de données au démarrage
    afficherClients();
    validateRealtime();
}

GestionClientsWidget::~GestionClientsWidget()
{
    delete ui;
}

QList<Client>& GestionClientsWidget::getClients()
{
    return clients;
}

int& GestionClientsWidget::getNextClientId()
{
    return nextClientId;
}

void GestionClientsWidget::afficherClients()
{
    // Configurer le tableau avec 10 colonnes
    ui->tableWidget_clients->setRowCount(0);
    ui->tableWidget_clients->setColumnCount(10);
    ui->tableWidget_clients->setHorizontalHeaderLabels({
        "ID", "Nom", "Prénom", "Tél", "Adresse", "Email",
        "Type", "Total Olives", "Date Création", "Statut"
    });
    ui->tableWidget_clients->horizontalHeader()->setStretchLastSection(true);

    // Vider la liste locale
    clients.clear();

    // Lire les clients depuis la base de données Oracle
    QSqlQuery query(ClientConnection::getInstance().getDatabase());
    query.prepare("SELECT ID_CLIENT, NOM, PRENOM, TELEPHONE, ADRESSE, EMAIL, "
                  "TYPE_CLIENT, TOTAL_OLIVES_LIVREES, DATE_CREATION, STATUT "
                  "FROM CLIENT ORDER BY ID_CLIENT DESC");

    if (query.exec()) {
        int row = 0;
        while (query.next()) {
            ui->tableWidget_clients->insertRow(row);

            // Récupérer les données
            QString id = query.value(0).toString();
            QString nom = query.value(1).toString();
            QString prenom = query.value(2).toString();
            QString tel = query.value(3).toString();
            QString adresse = query.value(4).toString();
            QString email = query.value(5).toString();
            QString type = query.value(6).toString();
            QString totalOlives = query.value(7).toString();
            QString dateCreation = query.value(8).toDate().toString("dd/MM/yyyy");
            QString statut = query.value(9).toString();

            // Remplir les cellules
            ui->tableWidget_clients->setItem(row, 0, new QTableWidgetItem(id));
            ui->tableWidget_clients->setItem(row, 1, new QTableWidgetItem(nom));
            ui->tableWidget_clients->setItem(row, 2, new QTableWidgetItem(prenom));
            ui->tableWidget_clients->setItem(row, 3, new QTableWidgetItem(tel));
            ui->tableWidget_clients->setItem(row, 4, new QTableWidgetItem(adresse));
            ui->tableWidget_clients->setItem(row, 5, new QTableWidgetItem(email));
            ui->tableWidget_clients->setItem(row, 6, new QTableWidgetItem(type));
            ui->tableWidget_clients->setItem(row, 7, new QTableWidgetItem(totalOlives));
            ui->tableWidget_clients->setItem(row, 8, new QTableWidgetItem(dateCreation));
            ui->tableWidget_clients->setItem(row, 9, new QTableWidgetItem(statut));

            // Rendre l'ID non éditable
            ui->tableWidget_clients->item(row, 0)->setFlags(ui->tableWidget_clients->item(row, 0)->flags() & ~Qt::ItemIsEditable);

            // Colorer les lignes selon le statut
            QColor couleur;
            if (statut == "Important") {
                couleur = QColor(200, 255, 200); // Vert clair
            } else {
                couleur = QColor(220, 220, 220); // Gris clair
            }
            for (int col = 0; col < 10; col++) {
                ui->tableWidget_clients->item(row, col)->setBackground(QBrush(couleur));
                ui->tableWidget_clients->item(row, col)->setForeground(QBrush(Qt::black)); // Texte en noir
            }

            // Stocker aussi dans la liste locale
            Client c;
            c.id_client = query.value(0).toInt();
            c.nom = nom;
            c.prenom = prenom;
            c.telephone = tel;
            c.adresse = adresse;
            c.email = email;
            c.type_client = type;
            c.total_olives_livrees = query.value(7).toDouble();
            c.date_creation = query.value(8).toDate();
            c.statut = statut;
            clients.append(c);

            row++;
        }
    } else {
        QMessageBox::critical(this, "Erreur",
            "Erreur lors du chargement des clients :\n" + query.lastError().text());
    }
}

void GestionClientsWidget::calculerStatutEnTempsReel()
{
    QString text = ui->lineEdit_total->text().trimmed();

    if (text.isEmpty()) {
        ui->label_statut->setText("-");
        ui->label_statut->setStyleSheet("");
        return;
    }

    bool ok;
    int total = text.toInt(&ok);

    if (!ok) {
        ui->label_statut->setText("-");
        ui->label_statut->setStyleSheet("");
        return;
    }

    if (total > 5000) {
        ui->label_statut->setText("Important");
        ui->label_statut->setStyleSheet("color: #2e7d32; font-weight: bold;");
    } else {
        ui->label_statut->setText("Standard");
        ui->label_statut->setStyleSheet("color: #666;");
    }
}

void GestionClientsWidget::clearForm()
{
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
    ui->label_statut->setStyleSheet("");
    currentSelectedClient.id_client = -1;
    idClientSelectionne = -1;
    desactiverBoutons();
    validateRealtime();
}

bool GestionClientsWidget::idExists(int id) const
{
    for (const Client &client : clients) {
        if (client.id_client == id) return true;
    }
    return false;
}

void GestionClientsWidget::setupValidators()
{
    ui->lineEdit_nom->setMaxLength(50);
    ui->lineEdit_prenom->setMaxLength(50);
    ui->lineEdit_tel->setMaxLength(8);
    ui->lineEdit_adresse->setMaxLength(100);
}

void GestionClientsWidget::setFieldState(QWidget *widget, bool valid)
{
    if (!widget) return;
    widget->setStyleSheet(valid ? "" : "border: 2px solid #d32f2f;");
}

void GestionClientsWidget::setErrorHint(const QString &message)
{
    ui->label_error_hint->setText(message);
}

bool GestionClientsWidget::isIdFormatValid(const QString &value) const
{
    return QRegularExpression("^\\d+$").match(value).hasMatch();
}

bool GestionClientsWidget::isNameValid(const QString &value) const
{
    return QRegularExpression("^[\\p{L} ]{2,50}$").match(value).hasMatch();
}

bool GestionClientsWidget::isTelValid(const QString &value) const
{
    return QRegularExpression("^\\d{8}$").match(value).hasMatch();
}

bool GestionClientsWidget::isEmailValid(const QString &value) const
{
    return QRegularExpression("^[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\\.[A-Za-z]{2,}$").match(value).hasMatch();
}

bool GestionClientsWidget::isAddressValid(const QString &value) const
{
    return value.length() >= 5 && value.length() <= 100;
}

bool GestionClientsWidget::isTotalValid(const QString &value) const
{
    if (value.isEmpty()) return false;
    bool ok = false;
    int total = value.toInt(&ok);
    return ok && total >= 0 && total <= 99999999;
}

void GestionClientsWidget::validateRealtime()
{
    QString id = ui->lineEdit_id->text().trimmed();
    QString nom = ui->lineEdit_nom->text().trimmed();
    QString prenom = ui->lineEdit_prenom->text().trimmed();
    QString tel = ui->lineEdit_tel->text().trimmed();
    QString adresse = ui->lineEdit_adresse->text().trimmed();
    QString email = ui->lineEdit_email->text().trimmed();
    QString total = ui->lineEdit_total->text().trimmed();
    QString type = ui->comboBox_type->currentText();

    bool idOk = id.isEmpty() || isIdFormatValid(id);
    bool nomOk = nom.isEmpty() || isNameValid(nom);
    bool prenomOk = prenom.isEmpty() || isNameValid(prenom);
    bool telOk = tel.isEmpty() || isTelValid(tel);
    bool adresseOk = adresse.isEmpty() || isAddressValid(adresse);
    bool emailOk = email.isEmpty() || isEmailValid(email);
    bool totalOk = total.isEmpty() || isTotalValid(total);
    bool typeOk = (type == "Particulier" || type == "Société");

    setFieldState(ui->lineEdit_id, idOk);
    setFieldState(ui->lineEdit_nom, nomOk);
    setFieldState(ui->lineEdit_prenom, prenomOk);
    setFieldState(ui->lineEdit_tel, telOk);
    setFieldState(ui->lineEdit_adresse, adresseOk);
    setFieldState(ui->lineEdit_email, emailOk);
    setFieldState(ui->lineEdit_total, totalOk);
    setFieldState(ui->comboBox_type, typeOk);

    if (!idOk) {
        setErrorHint("ID: uniquement des chiffres.");
    } else if (!nomOk) {
        setErrorHint("Nom: 2 à 50 lettres, espaces autorisés.");
    } else if (!prenomOk) {
        setErrorHint("Prénom: 2 à 50 lettres, espaces autorisés.");
    } else if (!telOk) {
        setErrorHint("Téléphone: exactement 8 chiffres.");
    } else if (!adresseOk) {
        setErrorHint("Adresse: 5 à 100 caractères.");
    } else if (!emailOk) {
        setErrorHint("Email: format valide (nom@domaine.com).");
    } else if (!typeOk) {
        setErrorHint("Type: Particulier ou Société.");
    } else if (!totalOk) {
        setErrorHint("Total olives: entier positif.");
    } else {
        setErrorHint("");
    }
}

bool GestionClientsWidget::validateForm(bool isAdd, bool isModify)
{

    QString idText = ui->lineEdit_id->text().trimmed();
    if (idText.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "ID obligatoire.");
        return false;
    }

    bool okId = false;
    int id = idText.toInt(&okId);
    if (!okId || id <= 0) {
        QMessageBox::warning(this, "Erreur", "ID doit être un entier positif.");
        return false;
    }

    if (isAdd && idExists(id)) {
        QMessageBox::warning(this, "Erreur", "ID existe déjà. Utilisez un autre ID.");
        return false;
    }

    if (isModify && !idExists(id)) {
        QMessageBox::warning(this, "Erreur", "ID introuvable pour modification.");
        return false;
    }

    QString nom = ui->lineEdit_nom->text().trimmed();
    QString prenom = ui->lineEdit_prenom->text().trimmed();
    QRegularExpression nameRegex("^[\\p{L} ]{2,50}$");
    if (nom.isEmpty() || !nameRegex.match(nom).hasMatch()) {
        QMessageBox::warning(this, "Erreur", "Nom invalide (2-50 lettres, espaces autorisés).");
        return false;
    }
    if (prenom.isEmpty() || !nameRegex.match(prenom).hasMatch()) {
        QMessageBox::warning(this, "Erreur", "Prénom invalide (2-50 lettres, espaces autorisés).");
        return false;
    }

    QString tel = ui->lineEdit_tel->text().trimmed();
    QRegularExpression telRegex("^\\d{8}$");
    if (tel.isEmpty() || !telRegex.match(tel).hasMatch()) {
        QMessageBox::warning(this, "Erreur", "Téléphone invalide (8 chiffres).");
        return false;
    }

    QString adresse = ui->lineEdit_adresse->text().trimmed();
    if (adresse.length() < 5 || adresse.length() > 100) {
        QMessageBox::warning(this, "Erreur", "Adresse invalide (5 à 100 caractères).");
        return false;
    }

    QString email = ui->lineEdit_email->text().trimmed();
    QRegularExpression emailRegex("^[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\\.[A-Za-z]{2,}$");
    if (email.isEmpty() || !emailRegex.match(email).hasMatch()) {
        QMessageBox::warning(this, "Erreur", "Email invalide (ex: nom@domaine.com).");
        return false;
    }

    QString type = ui->comboBox_type->currentText();
    if (type != "Particulier" && type != "Société") {
        QMessageBox::warning(this, "Erreur", "Type doit être Particulier ou Société.");
        return false;
    }

    bool okTotal = false;
    int totalOlives = ui->lineEdit_total->text().toInt(&okTotal);
    if (!okTotal || totalOlives < 0) {
        QMessageBox::warning(this, "Erreur", "Total olives invalide (entier positif).");
        return false;
    }
    if (totalOlives > 99999999) {
        QMessageBox::warning(this, "Erreur", "Total olives ne peut pas dépasser 99 999 999 kg.");
        return false;
    }

    return true;
}

void GestionClientsWidget::desactiverBoutons()
{
    ui->pushButton_modifier->setEnabled(false);
    ui->pushButton_supprimer->setEnabled(false);
}

void GestionClientsWidget::activerBoutons()
{
    ui->pushButton_modifier->setEnabled(true);
    ui->pushButton_supprimer->setEnabled(true);
}

void GestionClientsWidget::loadClientToForm(const Client &client)
{
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

Client GestionClientsWidget::getFormData()
{
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

void GestionClientsWidget::validateAndCalculateStatut(Client &client)
{
    client.statut = client.getStatutFidelite();
    client.importance = client.getImportance();
}

void GestionClientsWidget::displayClientInfo(const Client &client)
{
    QString info;
    info += "========================================\n";
    info += "        INFORMATIONS CLIENT\n";
    info += "========================================\n\n";
    info += "ID: " + QString::number(client.id_client) + "\n";
    info += "Nom: " + client.nom + "\n";
    info += "Prénom: " + client.prenom + "\n";
    info += "Téléphone: " + client.telephone + "\n";
    info += "Adresse: " + client.adresse + "\n";
    info += "Email: " + client.email + "\n";
    info += "Type: " + client.type_client + "\n";
    info += "Total Olives: " + QString::number(client.total_olives_livrees) + " kg\n";
    info += "Date: " + client.date_creation.toString("dd/MM/yyyy") + "\n";
    info += "Statut: " + client.statut + "\n";
    info += "Importance: " + client.importance + "\n";
    info += "========================================\n";
    
    QMessageBox::information(this, "Consultation Client", info);
}

void GestionClientsWidget::on_pushButton_ajouter_clicked()
{
    if (!validateForm(true, false)) return;

    bool ok;
    int totalOlives = ui->lineEdit_total->text().toInt(&ok);

    // === Calcul automatique du STATUT ===
    QString statut = (totalOlives > 5000) ? "Important" : "Standard";

    // === INSERT préparé dans la table CLIENT ===
    QSqlQuery query(ClientConnection::getInstance().getDatabase());
    query.prepare("INSERT INTO CLIENT (ID_CLIENT, NOM, PRENOM, TELEPHONE, ADRESSE, EMAIL, "
                  "TYPE_CLIENT, TOTAL_OLIVES_LIVREES, DATE_CREATION, STATUT) "
                  "VALUES (:id, :nom, :prenom, :tel, :adresse, :email, "
                  ":type, :total, TO_DATE(:date, 'DD/MM/YYYY'), :statut)");

    query.bindValue(":id", ui->lineEdit_id->text().toInt());
    query.bindValue(":nom", ui->lineEdit_nom->text().trimmed());
    query.bindValue(":prenom", ui->lineEdit_prenom->text().trimmed());
    query.bindValue(":tel", ui->lineEdit_tel->text().trimmed());
    query.bindValue(":adresse", ui->lineEdit_adresse->text().trimmed());
    query.bindValue(":email", ui->lineEdit_email->text().trimmed());
    query.bindValue(":type", ui->comboBox_type->currentText());
    query.bindValue(":total", totalOlives);
    query.bindValue(":date", ui->dateEdit_creation->date().toString("dd/MM/yyyy"));
    query.bindValue(":statut", statut);

    if (query.exec()) {
        afficherClients();
        clearForm();
        emit clientsUpdated();
        QMessageBox::information(this, "Succès", "Client ajouté avec succès dans la base de données !");
    } else {
        QMessageBox::critical(this, "Erreur",
            "Échec de l'ajout du client :\n" + query.lastError().text());
    }
}

void GestionClientsWidget::on_pushButton_modifier_clicked()
{
    if (idClientSelectionne == -1) {
        QMessageBox::warning(this, "Erreur", "Veuillez sélectionner un client à modifier !");
        return;
    }
    if (!validateForm(false, true)) return;

    bool ok;
    int totalOlives = ui->lineEdit_total->text().toInt(&ok);

    // === Calcul automatique du STATUT ===
    QString statut = (totalOlives > 5000) ? "Important" : "Standard";

    // === UPDATE préparé dans la table CLIENT ===
    QSqlQuery query(ClientConnection::getInstance().getDatabase());
    query.prepare("UPDATE CLIENT SET NOM = :nom, PRENOM = :prenom, TELEPHONE = :tel, "
                  "ADRESSE = :adresse, EMAIL = :email, TYPE_CLIENT = :type, "
                  "TOTAL_OLIVES_LIVREES = :total, DATE_CREATION = TO_DATE(:date, 'DD/MM/YYYY'), "
                  "STATUT = :statut WHERE ID_CLIENT = :id");

    query.bindValue(":nom", ui->lineEdit_nom->text().trimmed());
    query.bindValue(":prenom", ui->lineEdit_prenom->text().trimmed());
    query.bindValue(":tel", ui->lineEdit_tel->text().trimmed());
    query.bindValue(":adresse", ui->lineEdit_adresse->text().trimmed());
    query.bindValue(":email", ui->lineEdit_email->text().trimmed());
    query.bindValue(":type", ui->comboBox_type->currentText());
    query.bindValue(":total", totalOlives);
    query.bindValue(":date", ui->dateEdit_creation->date().toString("dd/MM/yyyy"));
    query.bindValue(":statut", statut);
    query.bindValue(":id", idClientSelectionne);

    if (query.exec()) {
        afficherClients();
        clearForm();
        emit clientsUpdated();
        QMessageBox::information(this, "Succès", "Client modifié avec succès dans la base de données !");
    } else {
        QMessageBox::critical(this, "Erreur",
            "Échec de la modification du client :\n" + query.lastError().text());
    }
}

void GestionClientsWidget::on_pushButton_supprimer_clicked()
{
    QString idText = ui->lineEdit_id->text().trimmed();
    bool okId = false;
    int id = idText.toInt(&okId);
    if (idText.isEmpty() || !okId || id <= 0 || !idExists(id)) {
        QMessageBox::warning(this, "Erreur", "Veuillez sélectionner un client à supprimer !");
        return;
    }

    // Demander confirmation
    QMessageBox::StandardButton reply = QMessageBox::question(this, "Confirmation",
        "Voulez-vous vraiment supprimer ce client (ID: " + QString::number(id) + ") ?",
        QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        QSqlQuery query(ClientConnection::getInstance().getDatabase());
        query.prepare("DELETE FROM CLIENT WHERE ID_CLIENT = :id");
        query.bindValue(":id", id);

        if (query.exec()) {
            // Commit explicite
            QSqlQuery commitQuery(ClientConnection::getInstance().getDatabase());
            commitQuery.exec("COMMIT");

            afficherClients();
            clearForm();
            emit clientsUpdated();
            QMessageBox::information(this, "Succès", "Client supprimé avec succès de la base de données !");
        } else {
            QMessageBox::critical(this, "Erreur",
                "Échec de la suppression du client :\n" + query.lastError().text());
        }
    }
}

void GestionClientsWidget::on_tableWidget_clients_clicked(const QModelIndex &index)
{
    if (!index.isValid()) return;

    int row = index.row();
    idClientSelectionne = ui->tableWidget_clients->item(row, 0)->text().toInt();

    // Charger les données du client sélectionné dans le formulaire
    ui->lineEdit_id->setText(ui->tableWidget_clients->item(row, 0)->text());
    ui->lineEdit_nom->setText(ui->tableWidget_clients->item(row, 1)->text());
    ui->lineEdit_prenom->setText(ui->tableWidget_clients->item(row, 2)->text());
    ui->lineEdit_tel->setText(ui->tableWidget_clients->item(row, 3)->text());
    ui->lineEdit_adresse->setText(ui->tableWidget_clients->item(row, 4)->text());
    ui->lineEdit_email->setText(ui->tableWidget_clients->item(row, 5)->text());
    ui->comboBox_type->setCurrentText(ui->tableWidget_clients->item(row, 6)->text());
    ui->lineEdit_total->setText(ui->tableWidget_clients->item(row, 7)->text());
    ui->dateEdit_creation->setDate(QDate::fromString(ui->tableWidget_clients->item(row, 8)->text(), "dd/MM/yyyy"));
    ui->label_statut->setText(ui->tableWidget_clients->item(row, 9)->text());

    currentSelectedClient.id_client = idClientSelectionne;

    // Activer les boutons Modifier et Supprimer
    activerBoutons();
}

void GestionClientsWidget::on_tableWidget_clients_customContextMenuRequested(const QPoint &pos)
{
    QTableWidgetItem *item = ui->tableWidget_clients->itemAt(pos);
    if (!item) return;

    const int row = item->row();
    ui->tableWidget_clients->selectRow(row);
    on_tableWidget_clients_clicked(ui->tableWidget_clients->model()->index(row, 0));

    const QString nom = ui->tableWidget_clients->item(row, 1)->text();
    const QString prenom = ui->tableWidget_clients->item(row, 2)->text();
    const QString type = ui->tableWidget_clients->item(row, 6)->text();
    const QString totalText = ui->tableWidget_clients->item(row, 7)->text();
    const QString dateText = ui->tableWidget_clients->item(row, 8)->text();
    const QString statut = ui->tableWidget_clients->item(row, 9)->text();

    bool okTotal = false;
    const double totalOlives = totalText.toDouble(&okTotal);
    const QDate dateCreation = QDate::fromString(dateText, "dd/MM/yyyy");

    const SmartAction smart = computeSmartAction(prenom, type, statut, okTotal ? totalOlives : 0, dateCreation);

    QMenu menu(this);
    menu.setFont(QFont("Arial", 10));
    menu.setStyleSheet(
        "QMenu { background: #ffffff; border: 1px solid #bdbdbd; }"
        "QMenu::item { padding: 6px 12px; }"
        "QMenu::item:selected { background: #e8f5e9; }"
    );

    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(&menu);
    shadow->setBlurRadius(12);
    shadow->setOffset(0, 2);
    shadow->setColor(QColor(0, 0, 0, 60));
    menu.setGraphicsEffect(shadow);

    QWidgetAction *nameAction = new QWidgetAction(&menu);
    QLabel *nameLabel = new QLabel(QString("Client: %1 %2").arg(prenom, nom));
    nameLabel->setStyleSheet("padding: 6px 10px; font-weight: 700;");
    nameLabel->setMinimumWidth(320);
    nameAction->setDefaultWidget(nameLabel);
    menu.addAction(nameAction);

    QWidgetAction *actionInfo = new QWidgetAction(&menu);
    QLabel *actionLabel = new QLabel(QString("Action recommandée: %1").arg(smart.action));
    actionLabel->setStyleSheet("padding: 4px 10px;");
    actionLabel->setWordWrap(true);
    actionInfo->setDefaultWidget(actionLabel);
    menu.addAction(actionInfo);

    QWidgetAction *messageInfo = new QWidgetAction(&menu);
    QLabel *messageLabel = new QLabel(QString("Message: %1").arg(smart.message));
    messageLabel->setStyleSheet("padding: 4px 10px; color: #333;");
    messageLabel->setWordWrap(true);
    messageLabel->setMinimumWidth(320);
    messageInfo->setDefaultWidget(messageLabel);
    menu.addAction(messageInfo);

    menu.addSeparator();
    QAction *copyAction = menu.addAction("📋 Copier le message");

    QAction *selected = menu.exec(ui->tableWidget_clients->viewport()->mapToGlobal(pos));
    if (selected == copyAction) {
        QClipboard *clipboard = QApplication::clipboard();
        clipboard->setText(smart.message);
        showToastMessage("Message copié !");
    }
}

void GestionClientsWidget::showToastMessage(const QString &message)
{
    QLabel *toast = new QLabel(message, this);
    toast->setStyleSheet("background: rgba(0,0,0,180); color: white; padding: 6px 14px; border-radius: 6px;");
    toast->setAttribute(Qt::WA_TransparentForMouseEvents);
    toast->adjustSize();
    const int x = (width() - toast->width()) / 2;
    const int y = height() - toast->height() - 20;
    toast->move(x, y);
    toast->raise();
    toast->show();

    QTimer::singleShot(2000, toast, &QWidget::deleteLater);
}
