#include "stock.h"
#include "ui_stock.h"

#include <QComboBox>
#include <QDialog>
#include <QDateTime>
#include <QDesktopServices>
#include <QDialogButtonBox>
#include <QHeaderView>
#include <QLabel>
#include <QRegularExpression>
#include <QSqlError>
#include <QSqlQuery>
#include <QTableWidgetItem>
#include <QUrl>
#include <QUrlQuery>
#include <QVector>
#include <QVBoxLayout>

namespace {

struct CandidateCommande {
    int idStock;
    QString typeHuile;
    double qteActuelle;
    double seuil;
    QString etat;
    double qteDemandee;
    QString fournisseur;
    QString emailFournisseur;
    QString telFournisseur;
};

QString buildEmailSubject(const CandidateCommande &c)
{
    return QString("[ALERTE STOCK] Demande de réapprovisionnement - %1").arg(c.typeHuile);
}

QString buildEmailBody(const CandidateCommande &c, const QString &contactEntreprise)
{
    QString contenu;
    contenu += QString("Bonjour %1,\n\n").arg(c.fournisseur);
    contenu += "Une alerte de stock a été déclenchée automatiquement.\n\n";
    contenu += "Détails de la demande:\n";
    contenu += QString("- Type d'huile: %1\n").arg(c.typeHuile);
    contenu += QString("- Quantité actuelle: %1 L\n").arg(QString::number(c.qteActuelle, 'f', 2));
    contenu += QString("- Seuil d'alerte: %1 L\n").arg(QString::number(c.seuil, 'f', 2));
    contenu += QString("- Quantité demandée: %1 L\n").arg(QString::number(c.qteDemandee, 'f', 2));
    contenu += QString("- État du stock: %1\n").arg(c.etat);
    contenu += QString("- Date de la demande: %1\n\n").arg(QDateTime::currentDateTime().toString("dd/MM/yyyy HH:mm"));
    contenu += "Merci de confirmer la disponibilité et le délai de livraison.\n\n";
    contenu += "Coordonnées entreprise:\n" + contactEntreprise + "\n\n";
    contenu += QString("Contact fournisseur enregistré: %1\n").arg(c.telFournisseur);
    contenu += "Cordialement,\nEquipe Stock PRESSIQ";
    return contenu;
}

QString buildWhatsAppAlertBody(const CandidateCommande &c)
{
    QString waMessage;
    waMessage += QString("Bonjour %1,\n\n").arg(c.fournisseur);
    waMessage += "ALERTE STOCK PRESSIQ\n\n";
    waMessage += "Details de la demande:\n";
    waMessage += QString("- Type d'huile: %1\n").arg(c.typeHuile);
    waMessage += QString("- Quantite actuelle: %1 L\n").arg(QString::number(c.qteActuelle, 'f', 2));
    waMessage += QString("- Seuil d'alerte: %1 L\n").arg(QString::number(c.seuil, 'f', 2));
    waMessage += QString("- Quantite demandee: %1 L\n").arg(QString::number(c.qteDemandee, 'f', 2));
    waMessage += QString("- Etat du stock: %1\n").arg(c.etat);
    waMessage += QString("- Date de la demande: %1\n\n").arg(QDateTime::currentDateTime().toString("dd/MM/yyyy HH:mm"));
    waMessage += "Merci de confirmer la disponibilite et le delai de livraison.\n\n";
    waMessage += "Cordialement,\n";
    waMessage += "Equipe Stock PRESSIQ";
    return waMessage;
}

QString buildWhatsAppTestBody(const CandidateCommande &c)
{
    QString waMessage;
    waMessage += QString("Bonjour %1,\n\n").arg(c.fournisseur);
    waMessage += "Message de test PRESSIQ pour verifier le contact fournisseur.\n";
    waMessage += QString("Huile concernee: %1\n\n").arg(c.typeHuile);
    waMessage += "Cordialement,\n";
    waMessage += "Equipe Stock PRESSIQ";
    return waMessage;
}

int firstSelectedRow(QTableWidget *table)
{
    if (!table) {
        return -1;
    }

    for (int i = 0; i < table->rowCount(); ++i) {
        QTableWidgetItem *checkItem = table->item(i, 0);
        if (checkItem && checkItem->checkState() == Qt::Checked) {
            return i;
        }
    }
    return -1;
}

} // namespace

bool Stock::envoyerEmailFournisseur(const QString &destinataire, const QString &sujet, const QString &contenu)
{
    if (destinataire.trimmed().isEmpty()) {
        return false;
    }

    QUrl url;
    url.setScheme("mailto");
    url.setPath(destinataire.trimmed());

    QUrlQuery query;
    query.addQueryItem("subject", sujet);
    query.addQueryItem("body", contenu);
    url.setQuery(query);

    return QDesktopServices::openUrl(url);
}

bool Stock::envoyerWhatsAppFournisseur(const QString &telephone, const QString &contenu)
{
    QString digits = telephone;
    digits.remove(QRegularExpression("[^0-9]"));

    if (digits.startsWith("00")) {
        digits = digits.mid(2);
    }

    // Cas local tunisien courant: 8 chiffres commencant par 0 dans les saisies utilisateurs.
    if (digits.startsWith("0") && digits.length() == 8) {
        digits = QString("216") + digits.mid(1);
    }

    if (digits.isEmpty()) {
        return false;
    }

    QUrl url("https://wa.me/" + digits);
    QUrlQuery query;
    query.addQueryItem("text", contenu);
    url.setQuery(query);

    return QDesktopServices::openUrl(url);
}

void Stock::ensureProcurementSchema()
{
    if (!connexion || !connexion->isOpen()) {
        return;
    }

    // Migration douce: ajoute les colonnes si elles n'existent pas deja.
    const QStringList alterStatements = {
        "ALTER TABLE STOCK ADD (FOURNISSEUR_NOM VARCHAR2(160))",
        "ALTER TABLE STOCK ADD (FOURNISSEUR_EMAIL VARCHAR2(160))",
        "ALTER TABLE STOCK ADD (FOURNISSEUR_TEL VARCHAR2(50))",
        "ALTER TABLE STOCK ADD (DATE_DERNIERE_COMMANDE DATE)"
    };

    QSqlQuery q(connexion->getDatabase());
    for (const QString &stmt : alterStatements) {
        if (!q.exec(stmt)) {
            const QString err = q.lastError().text().toUpper();
            if (!err.contains("ORA-01430") && !err.contains("ORA-01442") && !err.contains("ORA-00904")) {
                qWarning() << "Stock column migration warning:" << q.lastError().text();
            }
        }
    }
}

void Stock::refreshProcurementUi()
{
    if (!connexion || !connexion->isOpen()) {
        return;
    }

    if (ui->tableFournisseursIndependant) {
        ui->tableFournisseursIndependant->clear();
        ui->tableFournisseursIndependant->setColumnCount(6);
        ui->tableFournisseursIndependant->setHorizontalHeaderLabels(
            {"ID Stock", "Huile", "Fournisseur", "Email", "Téléphone", "Dernière commande"});
        ui->tableFournisseursIndependant->setRowCount(0);
        ui->tableFournisseursIndependant->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        ui->tableFournisseursIndependant->setSelectionBehavior(QAbstractItemView::SelectRows);
        ui->tableFournisseursIndependant->setEditTriggers(QAbstractItemView::NoEditTriggers);
        ui->tableFournisseursIndependant->setAlternatingRowColors(true);

        QSqlQuery qSupp(connexion->getDatabase());
        const QString sqlSupp =
            "SELECT ID_STOCK, TYPE_HUILE, NVL(FOURNISSEUR_NOM, '-'), NVL(FOURNISSEUR_EMAIL, '-'), "
            "       NVL(FOURNISSEUR_TEL, '-'), DATE_DERNIERE_COMMANDE "
            "FROM STOCK "
            "WHERE FOURNISSEUR_NOM IS NOT NULL AND LENGTH(TRIM(FOURNISSEUR_NOM)) > 0 "
            "ORDER BY TYPE_HUILE, ID_STOCK";

        if (qSupp.exec(sqlSupp)) {
            int rowSupp = 0;
            while (qSupp.next()) {
                ui->tableFournisseursIndependant->insertRow(rowSupp);
                ui->tableFournisseursIndependant->setItem(rowSupp, 0, new QTableWidgetItem(qSupp.value(0).toString()));
                ui->tableFournisseursIndependant->setItem(rowSupp, 1, new QTableWidgetItem(qSupp.value(1).toString()));
                ui->tableFournisseursIndependant->setItem(rowSupp, 2, new QTableWidgetItem(qSupp.value(2).toString()));
                ui->tableFournisseursIndependant->setItem(rowSupp, 3, new QTableWidgetItem(qSupp.value(3).toString()));
                ui->tableFournisseursIndependant->setItem(rowSupp, 4, new QTableWidgetItem(qSupp.value(4).toString()));
                const QDate dateCmd = qSupp.value(5).toDate();
                ui->tableFournisseursIndependant->setItem(rowSupp, 5,
                    new QTableWidgetItem(dateCmd.isValid() ? dateCmd.toString("dd/MM/yyyy") : "--"));

                const bool missingContact = qSupp.value(3).toString().trimmed().isEmpty() || qSupp.value(4).toString().trimmed().isEmpty();
                const QColor bg = missingContact ? QColor("#fff7ed") : QColor("#ecfdf3");
                const QColor fg = missingContact ? QColor("#9a3412") : QColor("#14532d");
                for (int col = 0; col < ui->tableFournisseursIndependant->columnCount(); ++col) {
                    QTableWidgetItem *cell = ui->tableFournisseursIndependant->item(rowSupp, col);
                    if (!cell) {
                        continue;
                    }
                    cell->setBackground(QBrush(bg));
                    cell->setForeground(QBrush(fg));
                    cell->setTextAlignment((col == 1 || col == 2 || col == 3 || col == 4)
                        ? (Qt::AlignVCenter | Qt::AlignLeft)
                        : Qt::AlignCenter);
                }
                ++rowSupp;
            }
        }
    }

    if (ui->comboFournisseurAffectation) {
        ui->comboFournisseurAffectation->clear();

        QSqlQuery qFourn(connexion->getDatabase());
        if (qFourn.exec("SELECT DISTINCT FOURNISSEUR_NOM, NVL(FOURNISSEUR_EMAIL, ''), NVL(FOURNISSEUR_TEL, '') "
                        "FROM STOCK "
                        "WHERE FOURNISSEUR_NOM IS NOT NULL "
                        "AND LENGTH(TRIM(FOURNISSEUR_NOM)) > 0 "
                        "ORDER BY FOURNISSEUR_NOM")) {
            while (qFourn.next()) {
                const QString nom = qFourn.value(0).toString().trimmed();
                const QString email = qFourn.value(1).toString().trimmed();
                const QString tel = qFourn.value(2).toString().trimmed();
                ui->comboFournisseurAffectation->addItem(nom, QString("%1|%2").arg(email, tel));
            }
        }

        if (ui->comboFournisseurAffectation->count() == 0) {
            ui->comboFournisseurAffectation->addItem("Aucun fournisseur enregistré", QString());
        }
    }

    if (!ui->tableHistoriqueCommandesAchat) {
        return;
    }

    ui->tableHistoriqueCommandesAchat->clear();
    ui->tableHistoriqueCommandesAchat->setColumnCount(7);
    ui->tableHistoriqueCommandesAchat->setHorizontalHeaderLabels(
        {"ID Stock", "Date Cmd", "Fournisseur", "Huile", "Qté (L)", "Statut", "Contact"});
    ui->tableHistoriqueCommandesAchat->setRowCount(0);
    ui->tableHistoriqueCommandesAchat->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableHistoriqueCommandesAchat->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableHistoriqueCommandesAchat->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableHistoriqueCommandesAchat->setAlternatingRowColors(true);

    QSqlQuery qHist(connexion->getDatabase());
    const QString sql =
        "SELECT ID_STOCK, DATE_DERNIERE_COMMANDE, NVL(FOURNISSEUR_NOM, '-'), TYPE_HUILE, QUANTITE_ACTUELLE, "
        "       CASE WHEN QUANTITE_ACTUELLE = 0 THEN 'Rupture' WHEN QUANTITE_ACTUELLE <= SEUIL_ALERTE THEN 'Faible' ELSE 'Normal' END, "
        "       TRIM(NVL(FOURNISSEUR_EMAIL, '') || ' ' || NVL(FOURNISSEUR_TEL, '')) "
        "FROM STOCK "
        "WHERE DATE_DERNIERE_COMMANDE IS NOT NULL "
        "ORDER BY DATE_DERNIERE_COMMANDE DESC, ID_STOCK DESC";

    if (!qHist.exec(sql)) {
        qWarning() << "ERP history load error:" << qHist.lastError().text();
        return;
    }

    int row = 0;
    while (qHist.next()) {
        ui->tableHistoriqueCommandesAchat->insertRow(row);
        ui->tableHistoriqueCommandesAchat->setItem(row, 0, new QTableWidgetItem(qHist.value(0).toString()));
        ui->tableHistoriqueCommandesAchat->setItem(row, 1, new QTableWidgetItem(qHist.value(1).toDate().toString("dd/MM/yyyy")));
        ui->tableHistoriqueCommandesAchat->setItem(row, 2, new QTableWidgetItem(qHist.value(2).toString()));
        ui->tableHistoriqueCommandesAchat->setItem(row, 3, new QTableWidgetItem(qHist.value(3).toString()));
        ui->tableHistoriqueCommandesAchat->setItem(row, 4, new QTableWidgetItem(QString::number(qHist.value(4).toDouble(), 'f', 2)));
        ui->tableHistoriqueCommandesAchat->setItem(row, 5, new QTableWidgetItem(qHist.value(5).toString()));
        ui->tableHistoriqueCommandesAchat->setItem(row, 6, new QTableWidgetItem(qHist.value(6).toString().trimmed()));

        const QString statut = qHist.value(5).toString();
        QColor bg("#ecfdf3");
        QColor fg("#14532d");
        if (statut == "Rupture") {
            bg = QColor("#fee2e2");
            fg = QColor("#991b1b");
        } else if (statut == "Faible") {
            bg = QColor("#ffedd5");
            fg = QColor("#9a3412");
        }

        for (int col = 0; col < ui->tableHistoriqueCommandesAchat->columnCount(); ++col) {
            QTableWidgetItem *cell = ui->tableHistoriqueCommandesAchat->item(row, col);
            if (!cell) {
                continue;
            }
            cell->setBackground(QBrush(bg));
            cell->setForeground(QBrush(fg));
            cell->setTextAlignment((col == 2 || col == 3 || col == 6)
                ? (Qt::AlignVCenter | Qt::AlignLeft)
                : Qt::AlignCenter);
        }
        ++row;
    }
}

void Stock::onAjouterFournisseurClicked()
{
    if (!connexion || !connexion->isOpen()) {
        afficherMessage("Fournisseurs", "Pas de connexion à la base de données", true);
        return;
    }

    const QString nom = ui->lineNomFournisseur ? ui->lineNomFournisseur->text().trimmed() : QString();
    const QString email = ui->lineEmailFournisseur ? ui->lineEmailFournisseur->text().trimmed() : QString();
    const QString tel = ui->lineTelFournisseur ? ui->lineTelFournisseur->text().trimmed() : QString();
    const QString typeHuile = ui->comboHuileFournisseur ? ui->comboHuileFournisseur->currentText().trimmed() : QString();

    if (nom.isEmpty()) {
        afficherMessage("Fournisseurs", "Le nom du fournisseur est obligatoire.", true);
        return;
    }

    if (typeHuile.isEmpty()) {
        afficherMessage("Fournisseurs", "Sélectionnez d'abord un type d'huile.", true);
        return;
    }

    QSqlQuery qUpd(connexion->getDatabase());
    qUpd.prepare("UPDATE STOCK "
                 "SET FOURNISSEUR_NOM = :nom, FOURNISSEUR_EMAIL = :email, FOURNISSEUR_TEL = :tel "
                 "WHERE TYPE_HUILE = :huile");
    qUpd.bindValue(":nom", nom);
    qUpd.bindValue(":email", email);
    qUpd.bindValue(":tel", tel);
    qUpd.bindValue(":huile", typeHuile);

    if (!qUpd.exec()) {
        afficherMessage("Fournisseurs", "Erreur affectation fournisseur: " + qUpd.lastError().text(), true);
        return;
    }

    if (ui->lineNomFournisseur) ui->lineNomFournisseur->clear();
    if (ui->lineEmailFournisseur) ui->lineEmailFournisseur->clear();
    if (ui->lineTelFournisseur) ui->lineTelFournisseur->clear();

    refreshProcurementUi();
    afficherMessage("Fournisseurs", "Fournisseur enregistré sur les stocks de ce type d'huile.");
}

void Stock::onLierHuileFournisseurClicked()
{
    if (!connexion || !connexion->isOpen()) {
        afficherMessage("Fournisseurs", "Pas de connexion à la base de données", true);
        return;
    }

    const QString typeHuile = ui->comboHuileFournisseur ? ui->comboHuileFournisseur->currentText().trimmed() : QString();
    const QString fournisseurNom = ui->comboFournisseurAffectation ? ui->comboFournisseurAffectation->currentText().trimmed() : QString();
    const QString contactRaw = ui->comboFournisseurAffectation ? ui->comboFournisseurAffectation->currentData().toString() : QString();
    const QString fournisseurEmail = contactRaw.section('|', 0, 0).trimmed();
    const QString fournisseurTel = contactRaw.section('|', 1, 1).trimmed();

    if (typeHuile.isEmpty() || fournisseurNom.isEmpty()) {
        afficherMessage("Fournisseurs", "Sélectionnez un type d'huile et un fournisseur.", true);
        return;
    }

    QSqlQuery qUpd(connexion->getDatabase());
    qUpd.prepare("UPDATE STOCK "
                 "SET FOURNISSEUR_NOM = :nom, FOURNISSEUR_EMAIL = :email, FOURNISSEUR_TEL = :tel "
                 "WHERE TYPE_HUILE = :huile");
    qUpd.bindValue(":nom", fournisseurNom);
    qUpd.bindValue(":email", fournisseurEmail);
    qUpd.bindValue(":tel", fournisseurTel);
    qUpd.bindValue(":huile", typeHuile);

    if (!qUpd.exec()) {
        afficherMessage("Fournisseurs", "Erreur liaison: " + qUpd.lastError().text(), true);
        return;
    }

    refreshProcurementUi();
    afficherMessage("Fournisseurs", "Liaison huile/fournisseur enregistrée.");
}

void Stock::onGenererCommandeAlerteClicked()
{
    if (!connexion || !connexion->isOpen()) {
        afficherMessage("Commandes", "Pas de connexion à la base de données", true);
        return;
    }

    QSqlQuery qStocks(connexion->getDatabase());
    const QString sql =
        "SELECT ID_STOCK, TYPE_HUILE, QUANTITE_ACTUELLE, SEUIL_ALERTE, "
        "       NVL(FOURNISSEUR_NOM, ''), NVL(FOURNISSEUR_EMAIL, ''), NVL(FOURNISSEUR_TEL, '') "
        "FROM STOCK "
        "WHERE (QUANTITE_ACTUELLE <= SEUIL_ALERTE OR QUANTITE_ACTUELLE = 0)";

    if (!qStocks.exec(sql)) {
        afficherMessage("Commandes", "Erreur lecture stocks critiques: " + qStocks.lastError().text(), true);
        return;
    }

    QVector<CandidateCommande> candidates;
    int skippedNoSupplier = 0;

    const QString contactEntreprise =
        "PRESSIQ - Service Approvisionnement\n"
        "Email: achats@pressiq.local\n"
        "Téléphone: +216 70 000 000";

    while (qStocks.next()) {
        const int idStock = qStocks.value(0).toInt();
        const QString typeHuile = qStocks.value(1).toString();
        const double qteActuelle = qStocks.value(2).toDouble();
        const double seuil = qStocks.value(3).toDouble();
        const QString fournisseur = qStocks.value(4).toString().trimmed();
        const QString emailFournisseur = qStocks.value(5).toString().trimmed();
        const QString telFournisseur = qStocks.value(6).toString().trimmed();

        if (fournisseur.isEmpty() || emailFournisseur.isEmpty()) {
            ++skippedNoSupplier;
            continue;
        }

        candidates.push_back({
            idStock,
            typeHuile,
            qteActuelle,
            seuil,
            (qteActuelle <= 0.0) ? "Critique" : "Faible",
            qMax(1.0, (seuil * 2.0) - qteActuelle),
            fournisseur,
            emailFournisseur,
            telFournisseur
        });
    }

    if (candidates.isEmpty()) {
        afficherMessage(
            "Commandes & Email",
            QString("Aucun fournisseur/email exploitable trouvé pour les stocks en alerte.\n"
                    "Lignes ignorées (sans fournisseur/email): %1")
                .arg(skippedNoSupplier),
            true);
        return;
    }

    QDialog selectionDialog(this);
    selectionDialog.setWindowTitle("Sélection des destinataires");
    selectionDialog.setModal(true);
    selectionDialog.resize(980, 420);

    QVBoxLayout *layout = new QVBoxLayout(&selectionDialog);
    QLabel *info = new QLabel(
        QString("Sélectionnez les fournisseurs à contacter pour les ruptures/faibles stocks. "
                "(%1 proposition(s), %2 ignorée(s) sans email/fournisseur)")
            .arg(candidates.size())
            .arg(skippedNoSupplier),
        &selectionDialog);
    info->setWordWrap(true);
    layout->addWidget(info);

    QLabel *modeLabel = new QLabel("Canal d'envoi:", &selectionDialog);
    layout->addWidget(modeLabel);

    QComboBox *comboModeEnvoi = new QComboBox(&selectionDialog);
    comboModeEnvoi->addItem("Email (client mail)", "email");
    comboModeEnvoi->addItem("WhatsApp Web", "whatsapp");
    comboModeEnvoi->addItem("Email + WhatsApp", "both");
    layout->addWidget(comboModeEnvoi);

    QTableWidget *tableSelection = new QTableWidget(candidates.size(), 8, &selectionDialog);
    tableSelection->setHorizontalHeaderLabels(
        {"Envoyer", "ID Stock", "Huile", "Fournisseur", "Email", "Qté Actuelle (L)", "Seuil (L)", "État"});
    tableSelection->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableSelection->setSelectionMode(QAbstractItemView::NoSelection);
    tableSelection->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableSelection->setAlternatingRowColors(true);

    for (int i = 0; i < candidates.size(); ++i) {
        const CandidateCommande &c = candidates[i];

        QTableWidgetItem *checkItem = new QTableWidgetItem();
        checkItem->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
        checkItem->setCheckState(Qt::Checked);
        tableSelection->setItem(i, 0, checkItem);

        tableSelection->setItem(i, 1, new QTableWidgetItem(QString::number(c.idStock)));
        tableSelection->setItem(i, 2, new QTableWidgetItem(c.typeHuile));
        tableSelection->setItem(i, 3, new QTableWidgetItem(c.fournisseur));
        tableSelection->setItem(i, 4, new QTableWidgetItem(c.emailFournisseur));
        tableSelection->setItem(i, 5, new QTableWidgetItem(QString::number(c.qteActuelle, 'f', 2)));
        tableSelection->setItem(i, 6, new QTableWidgetItem(QString::number(c.seuil, 'f', 2)));
        tableSelection->setItem(i, 7, new QTableWidgetItem(c.etat));
    }

    layout->addWidget(tableSelection);

    QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &selectionDialog);
    buttons->button(QDialogButtonBox::Ok)->setText("Envoyer aux sélectionnés");
    buttons->button(QDialogButtonBox::Cancel)->setText("Annuler");
    QPushButton *btnTestWhatsApp = buttons->addButton("Tester WhatsApp", QDialogButtonBox::ActionRole);

    QObject::connect(btnTestWhatsApp, &QPushButton::clicked, this, [this, tableSelection, &candidates]() {
        const int selectedIndex = firstSelectedRow(tableSelection);

        if (selectedIndex < 0) {
            afficherMessage("Test WhatsApp", "Cochez au moins un fournisseur pour tester WhatsApp.", true);
            return;
        }

        const CandidateCommande &c = candidates[selectedIndex];
        const QString waMessage = buildWhatsAppTestBody(c);

        const bool ok = envoyerWhatsAppFournisseur(c.telFournisseur, waMessage);
        if (ok) {
            afficherMessage("Test WhatsApp", QString("Test lancé vers %1 (%2).")
                                               .arg(c.fournisseur, c.telFournisseur));
        } else {
            afficherMessage("Test WhatsApp", "Impossible d'ouvrir WhatsApp. Vérifiez le numéro du fournisseur.", true);
        }
    });

    QObject::connect(buttons, &QDialogButtonBox::accepted, &selectionDialog, &QDialog::accept);
    QObject::connect(buttons, &QDialogButtonBox::rejected, &selectionDialog, &QDialog::reject);
    layout->addWidget(buttons);

    if (selectionDialog.exec() != QDialog::Accepted) {
        return;
    }

    int sent = 0;
    int sentEmail = 0;
    int sentWhatsApp = 0;
    int failed = 0;
    int notSelected = 0;

    const QString modeEnvoi = comboModeEnvoi->currentData().toString();

    for (int i = 0; i < candidates.size(); ++i) {
        QTableWidgetItem *checkItem = tableSelection->item(i, 0);
        const bool selected = checkItem && checkItem->checkState() == Qt::Checked;
        if (!selected) {
            ++notSelected;
            continue;
        }

        const CandidateCommande &c = candidates[i];

        const QString sujet = buildEmailSubject(c);
        const QString contenu = buildEmailBody(c, contactEntreprise);

        bool anySendOk = false;

        if (modeEnvoi == "email" || modeEnvoi == "both") {
            const bool emailOk = envoyerEmailFournisseur(c.emailFournisseur, sujet, contenu);
            if (emailOk) {
                anySendOk = true;
                ++sentEmail;
            }
        }

        if (modeEnvoi == "whatsapp" || modeEnvoi == "both") {
            const QString waMessage = buildWhatsAppAlertBody(c);
            const bool waOk = envoyerWhatsAppFournisseur(c.telFournisseur, waMessage);
            if (waOk) {
                anySendOk = true;
                ++sentWhatsApp;
            }
        }

        if (!anySendOk) {
            ++failed;
            continue;
        }

        QSqlQuery qUpd(connexion->getDatabase());
        qUpd.prepare("UPDATE STOCK SET DATE_DERNIERE_COMMANDE = SYSDATE WHERE ID_STOCK = :idStock");
        qUpd.bindValue(":idStock", c.idStock);
        if (qUpd.exec()) {
            ++sent;
        } else {
            ++failed;
        }
    }

    refreshProcurementUi();

    afficherMessage(
        "Commandes & Email",
        QString("Traitement terminé:\n- Envois validés: %1\n- Emails déclenchés: %2\n- WhatsApp déclenchés: %3\n- Non sélectionnés: %4\n- Ignorés (sans fournisseur/email): %5\n- Échecs d'envoi/mise à jour: %6")
            .arg(sent)
            .arg(sentEmail)
            .arg(sentWhatsApp)
            .arg(notSelected)
            .arg(skippedNoSupplier)
            .arg(failed));
}
