#include "calendrier.h"
#include "ui_stock.h"

#include <QBrush>
#include <QCalendarWidget>
#include <QDateTime>
#include <QDebug>
#include <QGraphicsOpacityEffect>
#include <QHeaderView>
#include <QPropertyAnimation>
#include <QSqlError>
#include <QSqlQuery>
#include <QTableWidget>
#include <QTextCharFormat>

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
