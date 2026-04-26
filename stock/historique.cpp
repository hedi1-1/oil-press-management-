#include "historique.h"
#include "ui_stock.h"

#include <QAbstractItemView>
#include <QBrush>
#include <QDate>
#include <QDateTime>
#include <QHeaderView>
#include <QListWidgetItem>
#include <QSqlError>
#include <QSqlQuery>
#include <QTableWidgetItem>

void Stock::onAfficherHistoriqueClicked()
{
    refreshHistoriqueEtatTemps(true);
}

void Stock::refreshHistoriqueEtatTemps(bool userTriggered)
{
    if (!ui->tableHistorique || !ui->listProduitsCritiques || !ui->progressStock || !ui->labelEtatGlobal || !ui->lcdVolume) {
        return;
    }

    if (!connexion || !connexion->isOpen()) {
        if (userTriggered) {
            afficherMessage("Erreur", "Pas de connexion à la base de données", true);
        }
        return;
    }

    QDate startDate = ui->dateDebut ? ui->dateDebut->date() : QDate::currentDate().addDays(-30);
    QDate endDate = ui->dateFin ? ui->dateFin->date() : QDate::currentDate();
    if (startDate > endDate) {
        std::swap(startDate, endDate);
        if (ui->dateDebut) ui->dateDebut->setDate(startDate);
        if (ui->dateFin) ui->dateFin->setDate(endDate);
    }

    ui->tableHistorique->clear();
    ui->tableHistorique->setColumnCount(7);
    ui->tableHistorique->setHorizontalHeaderLabels({"ID", "Type", "Quantité", "Seuil", "Date MAJ", "Emplacement", "État"});
    ui->tableHistorique->setRowCount(0);
    ui->tableHistorique->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableHistorique->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);
    ui->tableHistorique->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableHistorique->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableHistorique->setAlternatingRowColors(true);
    ui->tableHistorique->verticalHeader()->setVisible(false);
    ui->tableHistorique->verticalHeader()->setDefaultSectionSize(32);

    QSqlQuery historyQuery(connexion->getDatabase());
    historyQuery.prepare(
        "SELECT ID_STOCK, TYPE_HUILE, QUANTITE_ACTUELLE, SEUIL_ALERTE, DATE_DERNIERE_MISE_A_JOUR, EMPLACEMENT_STOCKAGE, ETAT_STOCK "
        "FROM STOCK "
        "WHERE DATE_DERNIERE_MISE_A_JOUR IS NOT NULL "
        "AND TRUNC(DATE_DERNIERE_MISE_A_JOUR) BETWEEN TO_DATE(:startDate, 'YYYY-MM-DD') AND TO_DATE(:endDate, 'YYYY-MM-DD') "
        "ORDER BY DATE_DERNIERE_MISE_A_JOUR DESC, ID_STOCK DESC");
    historyQuery.bindValue(":startDate", startDate.toString("yyyy-MM-dd"));
    historyQuery.bindValue(":endDate", endDate.toString("yyyy-MM-dd"));

    if (!historyQuery.exec()) {
        if (userTriggered) {
            afficherMessage("Historique", "Erreur chargement historique : " + historyQuery.lastError().text(), true);
        }
        return;
    }

    int row = 0;
    while (historyQuery.next()) {
        ui->tableHistorique->insertRow(row);

        const double quantite = historyQuery.value(2).toDouble();
        const double seuil = historyQuery.value(3).toDouble();

        ui->tableHistorique->setItem(row, 0, new QTableWidgetItem(historyQuery.value(0).toString()));
        ui->tableHistorique->setItem(row, 1, new QTableWidgetItem(historyQuery.value(1).toString()));
        ui->tableHistorique->setItem(row, 2, new QTableWidgetItem(QString::number(quantite, 'f', 2)));
        ui->tableHistorique->setItem(row, 3, new QTableWidgetItem(QString::number(seuil, 'f', 2)));
        ui->tableHistorique->setItem(row, 4, new QTableWidgetItem(historyQuery.value(4).toDate().toString("dd/MM/yyyy")));
        ui->tableHistorique->setItem(row, 5, new QTableWidgetItem(historyQuery.value(5).toString()));
        ui->tableHistorique->setItem(row, 6, new QTableWidgetItem(historyQuery.value(6).toString()));

        QColor bgColor("#ecfdf3");
        QColor fgColor("#14532d");
        if (quantite <= 0.0) {
            bgColor = QColor("#fee2e2");
            fgColor = QColor("#991b1b");
        } else if (seuil > 0.0 && quantite <= seuil) {
            bgColor = QColor("#ffedd5");
            fgColor = QColor("#92400e");
        }

        for (int col = 0; col < ui->tableHistorique->columnCount(); ++col) {
            QTableWidgetItem *item = ui->tableHistorique->item(row, col);
            if (!item) {
                continue;
            }
            item->setBackground(QBrush(bgColor));
            item->setForeground(QBrush(fgColor));
            item->setTextAlignment(col == 1 || col == 5 ? (Qt::AlignVCenter | Qt::AlignLeft) : Qt::AlignCenter);
        }

        ++row;
    }

    QSqlQuery kpiQuery(connexion->getDatabase());
    kpiQuery.prepare(
        "SELECT NVL(SUM(QUANTITE_ACTUELLE),0) AS TOTAL_QTE, "
        "       NVL(SUM(SEUIL_ALERTE),0) AS TOTAL_SEUIL, "
        "       SUM(CASE WHEN QUANTITE_ACTUELLE = 0 THEN 1 ELSE 0 END) AS NB_RUPTURE, "
        "       SUM(CASE WHEN QUANTITE_ACTUELLE > 0 AND QUANTITE_ACTUELLE <= SEUIL_ALERTE THEN 1 ELSE 0 END) AS NB_FAIBLE, "
        "       COUNT(*) AS NB_TOTAL "
        "FROM STOCK");

    if (kpiQuery.exec() && kpiQuery.next()) {
        const double totalQte = kpiQuery.value(0).toDouble();
        const double totalSeuil = kpiQuery.value(1).toDouble();
        const int nbRupture = kpiQuery.value(2).toInt();
        const int nbFaible = kpiQuery.value(3).toInt();
        const int nbTotal = qMax(1, kpiQuery.value(4).toInt());

        ui->lcdVolume->display(QString::number(totalQte, 'f', 2));
        if (ui->labelDerniereMaj) {
            ui->labelDerniereMaj->setText("Dernière mise à jour : " + QDateTime::currentDateTime().toString("dd/MM/yyyy HH:mm:ss"));
        }

        int progressPct = 100;
        if (totalSeuil > 0.0) {
            progressPct = qBound(0, qRound((totalQte / totalSeuil) * 100.0), 100);
        }

        QString etatText = "État : ✅ Normal";
        QString progressChunkColor = "#22c55e";
        if (nbRupture > 0) {
            etatText = QString("État : ⛔ Critique (%1 rupture%2)")
                           .arg(nbRupture)
                           .arg(nbRupture > 1 ? "s" : "");
            progressChunkColor = "#ef4444";
            progressPct = qMin(progressPct, 30);
        } else if (nbFaible > 0) {
            etatText = QString("État : ⚠️ Attention (%1 faible%2)")
                           .arg(nbFaible)
                           .arg(nbFaible > 1 ? "s" : "");
            progressChunkColor = "#f59e0b";
            progressPct = qMin(progressPct, 70);
        }

        ui->labelEtatGlobal->setText(etatText);
        ui->progressStock->setValue(progressPct);
        ui->progressStock->setFormat(QString("Niveau global : %1% (%2/%3)")
                                     .arg(progressPct)
                                     .arg(nbTotal - nbRupture - nbFaible)
                                     .arg(nbTotal));
        ui->progressStock->setStyleSheet(
            QString("QProgressBar { border: 1px solid #c7dbcf; border-radius: 9px; text-align: center; min-height: 30px; "
                    "background: #f6fbf8; color: #123327; font-weight: 700; }"
                    "QProgressBar::chunk { border-radius: 8px; background-color: %1; }")
                .arg(progressChunkColor));
    }

    ui->listProduitsCritiques->clear();
    QSqlQuery criticalQuery(connexion->getDatabase());
    criticalQuery.prepare(
        "SELECT TYPE_HUILE, QUANTITE_ACTUELLE, SEUIL_ALERTE, EMPLACEMENT_STOCKAGE "
        "FROM STOCK "
        "WHERE QUANTITE_ACTUELLE <= SEUIL_ALERTE OR QUANTITE_ACTUELLE = 0 "
        "ORDER BY CASE WHEN QUANTITE_ACTUELLE = 0 THEN 0 ELSE 1 END, QUANTITE_ACTUELLE ASC");

    if (criticalQuery.exec()) {
        while (criticalQuery.next()) {
            const QString type = criticalQuery.value(0).toString();
            const double quantite = criticalQuery.value(1).toDouble();
            const double seuil = criticalQuery.value(2).toDouble();
            const QString emplacement = criticalQuery.value(3).toString();

            QString prefix = "⚠️";
            QColor color("#92400e");
            if (quantite <= 0.0) {
                prefix = "⛔";
                color = QColor("#991b1b");
            }

            auto *item = new QListWidgetItem(
                QString("%1 %2 | %3 L (seuil %4 L) | %5")
                    .arg(prefix)
                    .arg(type)
                    .arg(QString::number(quantite, 'f', 2))
                    .arg(QString::number(seuil, 'f', 2))
                    .arg(emplacement));
            item->setForeground(QBrush(color));
            ui->listProduitsCritiques->addItem(item);
        }
    }

    if (ui->listProduitsCritiques->count() == 0) {
        auto *item = new QListWidgetItem("✅ Aucun produit critique actuellement");
        item->setForeground(QBrush(QColor("#166534")));
        ui->listProduitsCritiques->addItem(item);
    }

    if (userTriggered) {
        afficherMessage("Historique", QString("Historique mis à jour: %1 ligne(s) sur la période %2 - %3.")
                                         .arg(row)
                                         .arg(startDate.toString("dd/MM/yyyy"))
                                         .arg(endDate.toString("dd/MM/yyyy")));
    }
}
