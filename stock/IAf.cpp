#include "IAf.h"
#include "ui_stock.h"

#include <QDate>
#include <QDateTime>
#include <QSqlError>
#include <QSqlQuery>
#include <algorithm>

void Stock::onAnalyserPredictifHuileClicked()
{
    if (!connexion || !connexion->isOpen()) {
        afficherMessage("Erreur", "Pas de connexion a la base de donnees", true);
        return;
    }

    int periodDays = 30;
    const QString periode = ui->comboPeriodePredictifHuile
                                ? ui->comboPeriodePredictifHuile->currentText()
                                : QString("30 derniers jours");
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
        "       MAX(s.DATE_DERNIERE_MISE_A_JOUR) AS LAST_UPDATE, "
        "       NVL(s.FOURNISSEUR_NOM, ''), NVL(s.FOURNISSEUR_EMAIL, ''), NVL(s.FOURNISSEUR_TEL, ''), "
        "       MAX(s.DATE_DERNIERE_COMMANDE) AS LAST_ORDER "
        "FROM STOCK s "
        "LEFT JOIN PRODUCTION p ON p.ID_STOCK = s.ID_STOCK "
        "GROUP BY s.TYPE_HUILE, s.QUANTITE_ACTUELLE, s.SEUIL_ALERTE, s.FOURNISSEUR_NOM, s.FOURNISSEUR_EMAIL, s.FOURNISSEUR_TEL "
        "ORDER BY s.TYPE_HUILE");
    query.bindValue(":startDate", startDate);

    if (!query.exec()) {
        afficherMessage("Erreur", "Erreur analyse recommandations : " + query.lastError().text(), true);
        return;
    }

    struct ExpertRecommendation {
        QString type;
        double stock = 0.0;
        double seuil = 0.0;
        double prodPeriode = 0.0;
        QDate lastUpdate;
        QDate lastOrderDate;
        QString fournisseur;
        QString fournisseurEmail;
        QString fournisseurTel;
        double demandJ7 = 0.0;
        double demandJ30 = 0.0;
        double coverageDays = 0.0;
        int risk = 0;
        int optimalQty = 0;
        int idealDelayDays = 0;
        QString confidence;
        QString why;
        QString recommendationLine;
    };

    QList<ExpertRecommendation> rows;

    while (query.next()) {
        ExpertRecommendation item;
        item.type = query.value(0).toString();
        item.stock = query.value(1).toDouble();
        item.seuil = query.value(2).toDouble();
        item.prodPeriode = query.value(3).toDouble();
        item.lastUpdate = query.value(4).toDateTime().date();
        item.fournisseur = query.value(5).toString().trimmed();
        item.fournisseurEmail = query.value(6).toString().trimmed();
        item.fournisseurTel = query.value(7).toString().trimmed();
        item.lastOrderDate = query.value(8).toDateTime().date();

        const double avgDailyFromProd = item.prodPeriode / qMax(1, periodDays);
        const double baselineDaily = (item.seuil > 0.0) ? (item.seuil / 30.0) : 0.5;
        const double estimatedDailyDemand = qMax(0.5, qMax(avgDailyFromProd, baselineDaily));

        item.demandJ7 = estimatedDailyDemand * 7.0;
        item.demandJ30 = estimatedDailyDemand * 30.0;
        item.coverageDays = item.stock / qMax(0.1, estimatedDailyDemand);

        const double securityStock = qMax(10.0, item.seuil * 0.7);
        item.optimalQty = qMax(0, qRound(item.demandJ30 + securityStock - item.stock));

        if (item.coverageDays <= 2.0) {
            item.idealDelayDays = 1;
        } else if (item.coverageDays <= 5.0) {
            item.idealDelayDays = 2;
        } else if (item.coverageDays <= 10.0) {
            item.idealDelayDays = 4;
        } else {
            item.idealDelayDays = 7;
        }

        int freshnessRisk = 25;
        int daysSinceUpdate = 30;
        if (item.lastUpdate.isValid()) {
            daysSinceUpdate = qAbs(item.lastUpdate.daysTo(QDate::currentDate()));
            if (daysSinceUpdate <= 2) {
                freshnessRisk = 5;
            } else if (daysSinceUpdate <= 7) {
                freshnessRisk = 12;
            } else if (daysSinceUpdate <= 14) {
                freshnessRisk = 20;
            } else {
                freshnessRisk = 35;
            }
        }

        int stockRisk = 20;
        if (item.coverageDays <= 2.0) {
            stockRisk = 95;
        } else if (item.coverageDays <= 5.0) {
            stockRisk = 75;
        } else if (item.coverageDays <= 10.0) {
            stockRisk = 55;
        } else {
            stockRisk = 25;
        }

        item.risk = std::clamp(static_cast<int>(0.7 * stockRisk + 0.3 * freshnessRisk), 0, 100);

        const bool hasFullContact = !item.fournisseur.isEmpty() && !item.fournisseurEmail.isEmpty() && !item.fournisseurTel.isEmpty();
        if (!hasFullContact) {
            item.confidence = "Faible";
        } else if (daysSinceUpdate <= 7) {
            item.confidence = "Elevee";
        } else {
            item.confidence = "Moyenne";
        }

        const QString supplier = item.fournisseur.isEmpty() ? "Fournisseur a definir" : item.fournisseur;
        item.recommendationLine = QString("Commander %1 L chez %2 avant %3 jour(s)")
                                      .arg(item.optimalQty)
                                      .arg(supplier)
                                      .arg(item.idealDelayDays);

        item.why = QString("Couverture: %1 j | Demande J+30: %2 L | Risque: %3% | Confiance: %4")
                       .arg(QString::number(item.coverageDays, 'f', 1))
                       .arg(QString::number(item.demandJ30, 'f', 1))
                       .arg(item.risk)
                       .arg(item.confidence);

        rows.append(item);
    }

    if (rows.isEmpty()) {
        afficherMessage("Analyse", "Aucune donnee disponible pour les recommandations.", true);
        return;
    }

    std::sort(rows.begin(), rows.end(), [](const ExpertRecommendation &a, const ExpertRecommendation &b) {
        if (a.risk != b.risk) {
            return a.risk > b.risk;
        }
        return a.optimalQty > b.optimalQty;
    });

    if (ui->tableResultatsPredictifHuile) {
        ui->tableResultatsPredictifHuile->hide();
    }
    if (ui->groupResultatsPredictiveHuile) {
        ui->groupResultatsPredictiveHuile->hide();
    }

    int totalRisk = 0;
    int totalOptimalOrder = 0;
    for (const ExpertRecommendation &item : rows) {
        totalRisk += item.risk;
        totalOptimalOrder += item.optimalQty;
    }

    const int avgRisk = rows.isEmpty() ? 0 : qRound(static_cast<double>(totalRisk) / rows.size());
    const ExpertRecommendation &priorityOil = rows.first();
    const QString topSupplier = priorityOil.fournisseur.isEmpty() ? "Fournisseur a definir" : priorityOil.fournisseur;

    if (ui->labelTopStrategiqueValue2) {
        ui->labelTopStrategiqueValue2->setText(
            QString("Priorite: Commander %1 L de %2").arg(priorityOil.optimalQty).arg(priorityOil.type));
    }
    if (ui->labelRisqueMoyenGlobalValue2) {
        ui->labelRisqueMoyenGlobalValue2->setText(
            QString("Fournisseur recommande: %1 (%2)").arg(topSupplier).arg(priorityOil.confidence));
    }
    if (ui->labelBesoinReapproTotalValue2) {
        ui->labelBesoinReapproTotalValue2->setText(
            QString("Delai ideal: avant %1 jour(s) | Total: %2 L")
                .arg(priorityOil.idealDelayDays)
                .arg(totalOptimalOrder));
    }

    if (ui->textAnalysePredictifHuile) {
        QString report;
        report += "<h3 style='color:#14532d;'>Recommandation optimisee (niveau expert)</h3>";
        report += "<p><b>Periode:</b> " + periode + "<br/>";
        report += "<b>Principe:</b> quantite optimale + fournisseur recommande + delai ideal.</p>";
        report += "<hr/>";

        const int topN = qMin(3, rows.size());
        for (int i = 0; i < topN; ++i) {
            const ExpertRecommendation &item = rows.at(i);
            const QString supplier = item.fournisseur.isEmpty() ? "Fournisseur a definir" : item.fournisseur;
            const QString cardColor = (i == 0) ? "#fee2e2" : ((i == 1) ? "#ffedd5" : "#ecfdf3");
            report += QString(
                "<div style='background:%1;border:1px solid #cbd5e1;border-radius:10px;padding:10px;margin:10px 0;'>"
                "<p style='margin:0 0 8px 0;'><b>%2) %3</b></p>"
                "<p style='margin:0;'><b>Action:</b> Commander %4 L chez %5 avant %6 jour(s)</p>"
                "<p style='margin:4px 0 0 0;'><b>Justification:</b> %7</p>"
                "</div>")
                .arg(cardColor)
                .arg(i + 1)
                .arg(item.type)
                .arg(item.optimalQty)
                .arg(supplier)
                .arg(item.idealDelayDays)
                .arg(item.why);
        }

        report += QString("<p><b>Risque moyen global:</b> %1%% | <b>Volume total suggere:</b> %2 L</p>")
                      .arg(avgRisk)
                      .arg(totalOptimalOrder);

        ui->textAnalysePredictifHuile->setHtml(report);
    }

    afficherMessage("Recommandations", "Recommandations optimisees generees avec succes.");
}
