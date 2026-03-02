#include "production.h"
#include "ui_production.h"
#include "productioneditdialog.h"
#include <QCoreApplication>
#include <QDateTime>
#include <QPixmap>
#include <QFileDialog>
#include <QPrinter>
#include <QPainter>
#include <QPageLayout>
#include <QPageSize>
#include <QFont>
#include <QFontMetrics>
#include <QRect>
#include <QColor>
#include <QLinearGradient>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QStandardPaths>

Production::Production(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Production)
    , currentProductionId(0)
{
    ui->setupUi(this);
    {
        QPixmap logo;
        for (const QString &p : QStringList{":/logo.png", "logo.png", "../production/logo.png",
             QCoreApplication::applicationDirPath() + "/../../logo.png",
             QCoreApplication::applicationDirPath() + "/../../../production/logo.png"}) {
            logo = QPixmap(p);
            if (!logo.isNull()) break;
        }
        if (!logo.isNull())
            ui->lblLogo->setPixmap(logo.scaledToHeight(50, Qt::SmoothTransformation));
    }

    // Establish database connection via singleton
    if (!Connection::getInstance().createconnect()) {
        showErrorNotification("Impossible de se connecter à la base de données Oracle");
    }
    
    // Connect back button
    connect(ui->btnBackToMenu, &QPushButton::clicked, this, &Production::onBackButtonClicked);
    
    // Connect CRUD buttons
    connect(ui->btnPlanifier, &QPushButton::clicked, this, &Production::onPlanifierClicked);
    connect(ui->btnStart, &QPushButton::clicked, this, &Production::onStartClicked);
    connect(ui->btnStop, &QPushButton::clicked, this, &Production::onStopClicked);
    connect(ui->btnCalculateYield, &QPushButton::clicked, this, &Production::onCalculateYieldClicked);
    connect(ui->btnValidateQuality, &QPushButton::clicked, this, &Production::onValidateQualityClicked);
    connect(ui->btnGenerateReport, &QPushButton::clicked, this, &Production::onGenerateReportClicked);
    
    // Connect history buttons
    connect(ui->btnDeleteProduction, &QPushButton::clicked, this, &Production::onDeleteClicked);
    connect(ui->btnModifyProduction, &QPushButton::clicked, this, &Production::onModifyClicked);
    connect(ui->btnUpdateMonitoring, &QPushButton::clicked, this, &Production::onRefreshHistoryClicked);
    
    // Load production history on startup
    loadProductionHistory();
}

Production::~Production()
{
    delete ui;
}

void Production::onBackButtonClicked()
{
    emit backToMenu();
    this->hide();
}

// ============================================================================
// NOTIFICATION HELPERS
// ============================================================================

void Production::showSuccessNotification(const QString &message)
{
    QMessageBox msgBox(this);
    msgBox.setWindowTitle("✓ Succès");
    msgBox.setText(message);
    msgBox.setIcon(QMessageBox::Information);
    msgBox.setStyleSheet(
        "QMessageBox { background-color: #f0fdf4; }"
        "QMessageBox QLabel { color: #166534; font-size: 14px; font-weight: 600; }"
        "QPushButton { background-color: #1B4332; color: white; padding: 8px 20px; "
        "border-radius: 6px; font-weight: 600; min-width: 80px; }"
        "QPushButton:hover { background-color: #234E3E; }"
    );
    msgBox.exec();
}

void Production::showErrorNotification(const QString &message)
{
    QMessageBox msgBox(this);
    msgBox.setWindowTitle("✗ Erreur");
    msgBox.setText(message);
    msgBox.setIcon(QMessageBox::Critical);
    msgBox.setStyleSheet(
        "QMessageBox { background-color: #fef2f2; }"
        "QMessageBox QLabel { color: #991b1b; font-size: 14px; font-weight: 600; }"
        "QPushButton { background-color: #dc2626; color: white; padding: 8px 20px; "
        "border-radius: 6px; font-weight: 600; min-width: 80px; }"
        "QPushButton:hover { background-color: #b91c1c; }"
    );
    msgBox.exec();
}

// ============================================================================
// CRUD: ADD PRODUCTION (Planifier)
// ============================================================================

void Production::onPlanifierClicked()
{
    ProductionModel prod;
    
    // Get values from UI
    prod.setQuantiteOlivesKg(ui->spinOlivesKg->value());
    prod.setTypePressage(ui->comboPressType->currentText());
    prod.setNotesPlanification(ui->txtNotesPlanification->toPlainText());
    prod.setDateProduction(QDate::currentDate());
    prod.setStatut("Planifie");
    prod.setHeureDemarrage(QTime::currentTime());
    
    // Set FK values to 0 (will become NULL in database - no FK constraints)
    prod.setIdClient(0);
    prod.setIdUser(0);
    prod.setIdMachine(0);
    prod.setIdStock(0);
    
    // Estimate duration based on quantity
    int estimatedMinutes = prod.getQuantiteOlivesKg() / 5; // ~5kg per minute
    prod.setDureeEstimee(estimatedMinutes);
    prod.setTempsEcoule(0);
    prod.setHuileProduiteL(0);
    prod.setRendement(0);
    
    qDebug() << "Attempting to add production...";
    qDebug() << "Quantity:" << prod.getQuantiteOlivesKg();
    qDebug() << "Type:" << prod.getTypePressage();
    
    if (prod.addProduction()) {
        showSuccessNotification("Production planifiée avec succès!\n\n"
                               "Quantité: " + QString::number(prod.getQuantiteOlivesKg()) + " kg\n"
                               "Type: " + prod.getTypePressage() + "\n"
                               "Durée estimée: " + QString::number(estimatedMinutes) + " min");
        
        // Update UI status
        ui->lblStatusPlanification->setText("✓  Production planifiée");
        ui->lblStatusPlanification->setStyleSheet("background-color: #DCFCE7; color: #166534; border: 2px solid #4ADE80; border-radius: 8px; font-size: 15px; padding: 10px;");
        ui->lblDuration->setText("⏱ Durée estimée : " + QString::number(estimatedMinutes) + " min");
        
        currentProductionId = prod.getIdProduction();
        qDebug() << "Production created with ID:" << currentProductionId;
        
        // Refresh history table
        loadProductionHistory();
    } else {
        showErrorNotification("Échec de la planification!\n\nConsultez la console Application Output pour les détails.");
    }
}

// ============================================================================
// CRUD: UPDATE STATUS - START
// ============================================================================

void Production::onStartClicked()
{
    if (currentProductionId == 0) {
        showErrorNotification("Aucune production planifiée!\n\nPlanifiez d'abord une production.");
        return;
    }
    
    currentProduction.setIdProduction(currentProductionId);
    currentProduction.setStatut("En cours");
    currentProduction.setHeureDemarrage(QTime::currentTime());
    currentProduction.setTempsEcoule(0);
    
    // Get current form values
    currentProduction.setQuantiteOlivesKg(ui->spinOlivesKg->value());
    currentProduction.setTypePressage(ui->comboPressType->currentText());
    
    if (currentProduction.updateProduction()) {
        showSuccessNotification("▶ Production démarrée!\n\n"
                               "Heure de démarrage: " + QTime::currentTime().toString("HH:mm:ss") + "\n"
                               "Statut: En cours");
    } else {
        showErrorNotification("Échec du démarrage de la production!");
    }
}

// ============================================================================
// CRUD: UPDATE STATUS - STOP
// ============================================================================

void Production::onStopClicked()
{
    if (currentProductionId == 0) {
        showErrorNotification("Aucune production en cours!");
        return;
    }
    
    currentProduction.setIdProduction(currentProductionId);
    currentProduction.setStatut("Arrêté");
    
    if (currentProduction.updateProduction()) {
        showSuccessNotification("⏹ Production arrêtée!\n\n"
                               "La production a été mise en pause.");
    } else {
        showErrorNotification("Échec de l'arrêt de la production!");
    }
}

// ============================================================================
// CRUD: CALCULATE YIELD
// ============================================================================

void Production::onCalculateYieldClicked()
{
    int olivesKg = ui->spinOlivesKg->value();
    double oilLiters = ui->spinOilProduced->value();
    
    if (olivesKg <= 0) {
        showErrorNotification("Quantité d'olives invalide!\n\nEntrez une quantité supérieure à 0.");
        return;
    }
    
    double rendement = (oilLiters / static_cast<double>(olivesKg)) * 100.0;
    
    // Update UI
    ui->lblYield->setText(QString::number(rendement, 'f', 2) + " %");
    ui->progressYield->setValue(static_cast<int>(rendement));
    
    // Update production in database
    if (currentProductionId > 0) {
        currentProduction.setIdProduction(currentProductionId);
        currentProduction.setHuileProduiteL(oilLiters);
        currentProduction.setRendement(rendement);
        currentProduction.setQuantiteOlivesKg(olivesKg);
        
        if (currentProduction.updateProduction()) {
            QString evaluation;
            if (rendement >= 18) {
                evaluation = "🌟 Excellent rendement!";
            } else if (rendement >= 15) {
                evaluation = "✓ Bon rendement";
            } else if (rendement >= 10) {
                evaluation = "⚠ Rendement moyen";
            } else {
                evaluation = "⚠ Rendement faible";
            }
            
            showSuccessNotification("📊 Rendement calculé et enregistré!\n\n"
                                   "Olives: " + QString::number(olivesKg) + " kg\n"
                                   "Huile: " + QString::number(oilLiters, 'f', 2) + " L\n"
                                   "Rendement: " + QString::number(rendement, 'f', 2) + " %\n\n"
                                   + evaluation);
        }
    } else {
        showSuccessNotification("📊 Rendement calculé!\n\n"
                               "Rendement: " + QString::number(rendement, 'f', 2) + " %\n\n"
                               "Note: Planifiez une production pour enregistrer.");
    }
}

// ============================================================================
// CRUD: VALIDATE QUALITY
// ============================================================================

void Production::onValidateQualityClicked()
{
    if (currentProductionId == 0) {
        showErrorNotification("Aucune production à valider!\n\nPlanifiez d'abord une production.");
        return;
    }
    
    currentProduction.setIdProduction(currentProductionId);
    currentProduction.setQualite("Extra Vierge");
    currentProduction.setConformeNormes(true);
    currentProduction.setStatut("Qualité validée");
    
    if (currentProduction.updateProduction()) {
        showSuccessNotification("✓ Qualité validée avec succès!\n\n"
                               "Classification: Extra Vierge\n"
                               "Conforme aux normes: Oui\n"
                               "Statut: Validé");
    } else {
        showErrorNotification("Échec de la validation qualité!");
    }
}

// ============================================================================
// GENERATE REPORT → update DB + export professional PDF
// ============================================================================

void Production::onGenerateReportClicked()
{
    if (currentProductionId == 0) {
        showErrorNotification("Aucune production pour le rapport!\n\nPlanifiez d'abord une production.");
        return;
    }

    // Update DB: mark report generated and set status to Terminé
    currentProduction.setIdProduction(currentProductionId);
    currentProduction.setDateGenerationRapport(QDate::currentDate());
    currentProduction.setStatut("Termine");
    currentProduction.updateProduction();

    // Ask where to save the PDF
    QString defaultName = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)
                        + "/Rapport_Production_" + QDate::currentDate().toString("yyyyMMdd") + ".pdf";
    QString filePath = QFileDialog::getSaveFileName(
        this,
        "Enregistrer le rapport PDF",
        defaultName,
        "Fichiers PDF (*.pdf)"
    );

    if (filePath.isEmpty()) return;

    generatePdf(filePath);

    showSuccessNotification("📄 Rapport PDF généré avec succès!\n\n"
                            "Fichier : " + filePath + "\n"
                            "Date  : " + QDate::currentDate().toString("dd/MM/yyyy"));

    currentProductionId = 0;
    loadProductionHistory();
}

// ============================================================================
// GENERATE PROFESSIONAL PDF
// ============================================================================

void Production::generatePdf(const QString &filePath)
{
    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(filePath);
    printer.setPageSize(QPageSize(QPageSize::A4));
    printer.setPageOrientation(QPageLayout::Portrait);
    printer.setPageMargins(QMarginsF(15, 15, 15, 15), QPageLayout::Millimeter);

    QPainter p;
    if (!p.begin(&printer)) {
        showErrorNotification("Impossible d'ouvrir le fichier PDF pour écriture.");
        return;
    }

    // ── Coordinates & helpers ────────────────────────────────────────────────
    QRectF page = QRectF(p.viewport());  // full page in device pixels
    const qreal W = page.width();
    const qreal H = page.height();

    auto scale = [&](qreal mm) -> qreal {
        return mm * printer.logicalDpiX() / 25.4;
    };

    // DPI-aware sizes
    qreal lm = scale(8);          // left margin
    qreal rm = W - scale(8);      // right limit
    qreal usableW = rm - lm;
    qreal y = scale(8);           // current Y cursor

    // ── Colours ──────────────────────────────────────────────────────────────
    QColor darkGreen(27, 67, 50);
    QColor medGreen(35, 78, 62);
    QColor accentGold(212, 172, 13);
    QColor lightGreen(220, 252, 231);
    QColor grayText(107, 114, 128);
    QColor rowAlt(249, 250, 251);
    QColor borderColor(229, 231, 235);

    // ── HEADER BANNER ────────────────────────────────────────────────────────
    qreal hdrH = scale(28);
    QLinearGradient hdrGrad(lm, y, rm, y);
    hdrGrad.setColorAt(0, darkGreen);
    hdrGrad.setColorAt(1, medGreen);
    p.setBrush(hdrGrad);
    p.setPen(Qt::NoPen);
    p.drawRoundedRect(QRectF(lm, y, usableW, hdrH), scale(3), scale(3));

    // Gold accent stripe
    p.setBrush(accentGold);
    p.drawRect(QRectF(lm, y + hdrH - scale(1.5), usableW, scale(1.5)));

    // Title text
    QFont titleFont("Arial", 0, QFont::Bold);
    titleFont.setPixelSize(static_cast<int>(scale(8)));
    p.setFont(titleFont);
    p.setPen(Qt::white);
    p.drawText(QRectF(lm + scale(6), y, usableW * 0.65, hdrH),
               Qt::AlignVCenter | Qt::AlignLeft, "PressIQ — Rapport de Production");

    // Subtitle
    QFont subFont("Arial");
    subFont.setPixelSize(static_cast<int>(scale(4)));
    p.setFont(subFont);
    p.setPen(QColor(134, 239, 172));
    p.drawText(QRectF(lm + scale(6), y + scale(11), usableW * 0.65, hdrH * 0.5),
               Qt::AlignTop | Qt::AlignLeft, "MODULE DE PRODUCTION INTELLIGENT");

    // Date badge (top right)
    QString reportDate = "Généré le " + QDate::currentDate().toString("dd/MM/yyyy");
    QRectF dateBadge(rm - scale(46), y + scale(7), scale(44), scale(12));
    p.setBrush(accentGold);
    p.setPen(Qt::NoPen);
    p.drawRoundedRect(dateBadge, scale(2), scale(2));
    QFont dateFont("Arial", 0, QFont::Bold);
    dateFont.setPixelSize(static_cast<int>(scale(4)));
    p.setFont(dateFont);
    p.setPen(darkGreen);
    p.drawText(dateBadge, Qt::AlignCenter, reportDate);

    y += hdrH + scale(8);

    // ── INFO ROW ─────────────────────────────────────────────────────────────
    QFont labelFont("Arial");
    labelFont.setPixelSize(static_cast<int>(scale(3.5)));
    QFont valueFont("Arial", 0, QFont::Bold);
    valueFont.setPixelSize(static_cast<int>(scale(3.5)));

    // Fetch totals from DB
    QSqlQuery totals(QSqlDatabase::database("production_conn"));
    totals.exec(
        "SELECT COUNT(*), SUM(QUANTITEOLIVESKG), SUM(HUILEPRODUITEL), AVG(RENDEMENT) "
        "FROM PRODUCTION"
    );
    int    totalProds  = 0;
    double totalOlives = 0, totalHuile = 0, avgRend = 0;
    if (totals.next()) {
        totalProds  = totals.value(0).toInt();
        totalOlives = totals.value(1).toDouble();
        totalHuile  = totals.value(2).toDouble();
        avgRend     = totals.value(3).toDouble();
    }

    // Draw 4 KPI cards
    struct KpiCard { QString icon; QString value; QString label; QColor bg; };
    QList<KpiCard> kpis = {
        {"🌿", QString::number(totalProds), "Productions totales", QColor(239,246,255)},
        {"⚖",  QString::number(totalOlives,'f',0) + " kg", "Olives traitées", lightGreen},
        {"💧", QString::number(totalHuile,'f',1) + " L",   "Huile produite",  QColor(254,249,195)},
        {"📊", QString::number(avgRend,'f',2) + " %",       "Rendement moyen", QColor(254,226,226)}
    };

    qreal cardW    = (usableW - scale(3) * 3) / 4.0;
    qreal cardH    = scale(20);
    qreal cardX    = lm;

    for (const auto &kpi : kpis) {
        // Card background
        p.setBrush(kpi.bg);
        p.setPen(borderColor);
        p.drawRoundedRect(QRectF(cardX, y, cardW, cardH), scale(2.5), scale(2.5));

        // Icon
        QFont iconFont("Segoe UI Emoji");
        iconFont.setPixelSize(static_cast<int>(scale(7)));
        p.setFont(iconFont);
        p.setPen(darkGreen);
        p.drawText(QRectF(cardX, y + scale(2), cardW, scale(9)),
                   Qt::AlignTop | Qt::AlignHCenter, kpi.icon);

        // Value
        QFont kpiValFont("Arial", 0, QFont::Bold);
        kpiValFont.setPixelSize(static_cast<int>(scale(5)));
        p.setFont(kpiValFont);
        p.setPen(darkGreen);
        p.drawText(QRectF(cardX, y + scale(11), cardW, scale(6)),
                   Qt::AlignTop | Qt::AlignHCenter, kpi.value);

        // Label
        QFont kpiLblFont("Arial");
        kpiLblFont.setPixelSize(static_cast<int>(scale(3)));
        p.setFont(kpiLblFont);
        p.setPen(grayText);
        p.drawText(QRectF(cardX, y + scale(16), cardW, scale(5)),
                   Qt::AlignTop | Qt::AlignHCenter, kpi.label);

        cardX += cardW + scale(3);
    }

    y += cardH + scale(8);

    // ── SECTION TITLE: Historique ─────────────────────────────────────────────
    QFont secFont("Arial", 0, QFont::Bold);
    secFont.setPixelSize(static_cast<int>(scale(5)));
    p.setFont(secFont);
    p.setPen(darkGreen);
    p.drawText(QRectF(lm, y, usableW, scale(7)), Qt::AlignVCenter | Qt::AlignLeft,
               "Historique des Productions");

    // Gold underline
    p.setBrush(accentGold);
    p.setPen(Qt::NoPen);
    p.drawRect(QRectF(lm, y + scale(6.5), scale(40), scale(1)));

    y += scale(9);

    // ── TABLE HEADER ─────────────────────────────────────────────────────────
    QList<QString> headers = {"ID", "Date", "Olives (kg)", "Huile (L)", "Rendement", "Statut"};
    QList<qreal>   colFrac = {0.05, 0.13, 0.14, 0.13, 0.13, 0.15};  // fractions of usableW

    // Normalise so they sum to <=1 using remaining proportion
    QList<qreal> colW;
    for (qreal f : colFrac) colW << f * usableW;

    qreal rowH = scale(8);

    // Header row background
    p.setBrush(darkGreen);
    p.setPen(Qt::NoPen);
    p.drawRoundedRect(QRectF(lm, y, usableW, rowH), scale(1.5), scale(1.5));

    QFont tblHdrFont("Arial", 0, QFont::Bold);
    tblHdrFont.setPixelSize(static_cast<int>(scale(3.5)));
    p.setFont(tblHdrFont);
    p.setPen(Qt::white);

    qreal cx = lm;
    for (int i = 0; i < headers.size(); i++) {
        p.drawText(QRectF(cx + scale(1), y, colW[i], rowH),
                   Qt::AlignVCenter | Qt::AlignLeft, headers[i]);
        cx += colW[i];
    }
    y += rowH;

    // ── TABLE ROWS ────────────────────────────────────────────────────────────
    QSqlQuery hist(QSqlDatabase::database("production_conn"));
    hist.exec(
        "SELECT IDPRODUCTION, DATEPRODUCTION, QUANTITEOLIVESKG, HUILEPRODUITEL, RENDEMENT, STATUT "
        "FROM PRODUCTION ORDER BY IDPRODUCTION DESC"
    );

    QFont rowFont("Arial");
    rowFont.setPixelSize(static_cast<int>(scale(3.5)));
    p.setFont(rowFont);

    int rowIdx = 0;
    while (hist.next()) {
        // Alternate background
        if (rowIdx % 2 == 0) {
            p.setBrush(rowAlt);
        } else {
            p.setBrush(Qt::white);
        }
        p.setPen(Qt::NoPen);
        p.drawRect(QRectF(lm, y, usableW, rowH));

        // Bottom line
        p.setPen(QPen(borderColor, 0.5));
        p.drawLine(QPointF(lm, y + rowH), QPointF(rm, y + rowH));

        QString statut = hist.value(5).toString();
        cx = lm;

        QList<QString> cells = {
            hist.value(0).toString(),
            hist.value(1).toDate().toString("dd/MM/yyyy"),
            QString::number(hist.value(2).toInt()) + " kg",
            QString::number(hist.value(3).toDouble(), 'f', 2) + " L",
            QString::number(hist.value(4).toDouble(), 'f', 2) + " %",
            statut
        };

        for (int i = 0; i < cells.size(); i++) {
            // Statut pill
            if (i == 5) {
                QColor pillBg, pillFg;
                if (statut.contains("Termin", Qt::CaseInsensitive)) {
                    pillBg = QColor(220,252,231); pillFg = QColor(22,101,52);
                } else if (statut == "En cours") {
                    pillBg = QColor(219,234,254); pillFg = QColor(30,64,175);
                } else if (statut.contains("Planifi", Qt::CaseInsensitive)) {
                    pillBg = QColor(254,243,199); pillFg = QColor(146,64,14);
                } else {
                    pillBg = QColor(254,226,226); pillFg = QColor(185,28,28);
                }
                qreal pillH = scale(5);
                qreal pillY = y + (rowH - pillH) / 2.0;
                p.setBrush(pillBg);
                p.setPen(Qt::NoPen);
                QFontMetrics fm(rowFont);
                qreal pillW = qMin(fm.horizontalAdvance(statut) + scale(5),
                                   (double)colW[i] - scale(2));
                p.drawRoundedRect(QRectF(cx + scale(1), pillY, pillW, pillH),
                                  scale(1.5), scale(1.5));
                p.setPen(pillFg);
                p.setFont(rowFont);
                p.drawText(QRectF(cx + scale(1), pillY, pillW, pillH),
                           Qt::AlignCenter, statut);
            } else {
                p.setPen(QColor(31, 41, 55));
                p.setFont(rowFont);
                p.drawText(QRectF(cx + scale(1), y, colW[i] - scale(2), rowH),
                           Qt::AlignVCenter | Qt::AlignLeft, cells[i]);
            }
            cx += colW[i];
        }

        y += rowH;
        rowIdx++;

        // Page break
        if (y + rowH > H - scale(20)) {
            printer.newPage();
            y = scale(8);
        }
    }

    // ── FOOTER ───────────────────────────────────────────────────────────────
    y = H - scale(14);
    p.setBrush(Qt::NoBrush);
    p.setPen(QPen(borderColor, 0.7));
    p.drawLine(QPointF(lm, y), QPointF(rm, y));

    QFont footerFont("Arial");
    footerFont.setPixelSize(static_cast<int>(scale(3)));
    p.setFont(footerFont);
    p.setPen(grayText);
    p.drawText(QRectF(lm, y + scale(2), usableW / 2, scale(8)),
               Qt::AlignVCenter | Qt::AlignLeft,
               "PressIQ — Gestion de la Production  |  Module Intelligent");
    p.drawText(QRectF(lm, y + scale(2), usableW, scale(8)),
               Qt::AlignVCenter | Qt::AlignRight,
               "Rapport généré automatiquement  •  " +
               QDateTime::currentDateTime().toString("dd/MM/yyyy HH:mm"));

    p.end();
}

// ============================================================================
// LOAD PRODUCTION HISTORY
// ============================================================================

void Production::loadProductionHistory()
{
    QSqlQuery query(QSqlDatabase::database("production_conn"));
    query.exec("SELECT IDPRODUCTION, DATEPRODUCTION, QUANTITEOLIVESKG, HUILEPRODUITEL, RENDEMENT, STATUT "
               "FROM PRODUCTION ORDER BY IDPRODUCTION DESC");
    
    // Clear existing rows
    ui->tableProductionHistory->setRowCount(0);
    
    // Hide vertical header (row numbers)
    ui->tableProductionHistory->verticalHeader()->setVisible(false);
    
    int row = 0;
    while (query.next()) {
        ui->tableProductionHistory->insertRow(row);
        
        // ID
        QTableWidgetItem* idItem = new QTableWidgetItem(query.value(0).toString());
        idItem->setTextAlignment(Qt::AlignCenter);
        idItem->setForeground(QColor("#1f2937"));
        ui->tableProductionHistory->setItem(row, 0, idItem);
        
        // Date
        QTableWidgetItem* dateItem = new QTableWidgetItem(query.value(1).toDate().toString("dd/MM/yyyy"));
        dateItem->setTextAlignment(Qt::AlignCenter);
        dateItem->setForeground(QColor("#1f2937"));
        ui->tableProductionHistory->setItem(row, 1, dateItem);
        
        // Olives (kg)
        QTableWidgetItem* olivesItem = new QTableWidgetItem(QString::number(query.value(2).toInt()) + " kg");
        olivesItem->setTextAlignment(Qt::AlignCenter);
        olivesItem->setForeground(QColor("#1f2937"));
        ui->tableProductionHistory->setItem(row, 2, olivesItem);
        
        // Huile (L)
        QTableWidgetItem* huileItem = new QTableWidgetItem(QString::number(query.value(3).toDouble(), 'f', 2) + " L");
        huileItem->setTextAlignment(Qt::AlignCenter);
        huileItem->setForeground(QColor("#1f2937"));
        ui->tableProductionHistory->setItem(row, 3, huileItem);
        
        // Rendement
        QTableWidgetItem* rendementItem = new QTableWidgetItem(QString::number(query.value(4).toDouble(), 'f', 2) + " %");
        rendementItem->setTextAlignment(Qt::AlignCenter);
        rendementItem->setForeground(QColor("#1f2937"));
        ui->tableProductionHistory->setItem(row, 4, rendementItem);
        
        // Statut with color badge
        QString statut = query.value(5).toString();
        QTableWidgetItem* statutItem = new QTableWidgetItem(statut);
        statutItem->setTextAlignment(Qt::AlignCenter);
        
        // Color code statut - background and text
        if (statut == "Termine" || statut.contains("Termin")) {
            statutItem->setBackground(QColor("#DCFCE7"));
            statutItem->setForeground(QColor("#166534"));
        } else if (statut == "En cours") {
            statutItem->setBackground(QColor("#DBEAFE"));
            statutItem->setForeground(QColor("#1E40AF"));
        } else if (statut == "Planifie" || statut.contains("Planifi")) {
            statutItem->setBackground(QColor("#FEF3C7"));
            statutItem->setForeground(QColor("#92400E"));
        } else {
            statutItem->setForeground(QColor("#1f2937"));
        }
        ui->tableProductionHistory->setItem(row, 5, statutItem);
        
        row++;
    }
    
    // Resize columns to content
    ui->tableProductionHistory->resizeColumnsToContents();
    ui->tableProductionHistory->horizontalHeader()->setStretchLastSection(true);
    
    qDebug() << "Loaded" << row << "productions into history table";
}

// ============================================================================
// CRUD: DELETE SELECTED PRODUCTION
// ============================================================================

void Production::onDeleteClicked()
{
    // Get selected row
    int selectedRow = ui->tableProductionHistory->currentRow();
    if (selectedRow < 0) {
        showErrorNotification("Sélectionnez une production dans le tableau!");
        return;
    }
    
    // Get ID from first column
    QTableWidgetItem* idItem = ui->tableProductionHistory->item(selectedRow, 0);
    if (!idItem) {
        showErrorNotification("Erreur: Impossible de récupérer l'ID!");
        return;
    }
    
    int productionId = idItem->text().toInt();
    
    QMessageBox::StandardButton reply = QMessageBox::question(this, 
        "Confirmer la suppression",
        "Êtes-vous sûr de vouloir supprimer la production ID: " + QString::number(productionId) + "?",
        QMessageBox::Yes | QMessageBox::No);
    
    if (reply == QMessageBox::Yes) {
        if (ProductionModel::deleteProduction(productionId)) {
            showSuccessNotification("Production supprimée!\n\nID: " + QString::number(productionId));
            loadProductionHistory(); // Refresh table
        } else {
            showErrorNotification("Échec de la suppression!");
        }
    }
}

// ============================================================================
// CRUD: MODIFY SELECTED PRODUCTION  →  popup dialog
// ============================================================================

void Production::onModifyClicked()
{
    // ── 1. Need a selected row ────────────────────────────────────────────────
    int selectedRow = ui->tableProductionHistory->currentRow();
    if (selectedRow < 0) {
        showErrorNotification("Sélectionnez une production dans le tableau!");
        return;
    }

    int productionId = ui->tableProductionHistory->item(selectedRow, 0)->text().toInt();

    // ── 2. Load full row from database ───────────────────────────────────────
    QSqlQuery q(QSqlDatabase::database("production_conn"));
    q.prepare("SELECT DATEPRODUCTION, QUANTITEOLIVESKG, HUILEPRODUITEL, RENDEMENT, "
              "STATUT, TYPEPRESSAGE, QUALITE, NOTESPLANIFICATION "
              "FROM PRODUCTION WHERE IDPRODUCTION = :id");
    q.bindValue(":id", productionId);
    if (!q.exec() || !q.next()) {
        showErrorNotification("Impossible de charger la production ID " +
                              QString::number(productionId));
        return;
    }

    // ── 3. Build dialog ──────────────────────────────────────────────────────
    ProductionEditDialog dlg(this);
    dlg.setProductionId(productionId);
    dlg.setDate(q.value(0).toDate());
    dlg.setOlivesKg(q.value(1).toInt());
    dlg.setHuileL(q.value(2).toDouble());
    dlg.setRendement(q.value(3).toDouble());
    dlg.setStatut(q.value(4).toString());
    dlg.setTypePressage(q.value(5).toString());
    dlg.setQualite(q.value(6).toString());
    dlg.setNotes(q.value(7).toString());

    // Center dialog over main window
    dlg.adjustSize();
    QPoint center = this->geometry().center();
    dlg.move(center.x() - dlg.width() / 2, center.y() - dlg.height() / 2);

    // ── 4. Execute ───────────────────────────────────────────────────────────
    if (dlg.exec() != QDialog::Accepted)
        return;

    // ── 5. Build UPDATE query with new values ────────────────────────────────
    double newOlives = dlg.getOlivesKg();
    double newHuile  = dlg.getHuileL();
    double newRend   = (newOlives > 0) ? (newHuile / newOlives * 100.0) : 0.0;

    QSqlQuery upd(QSqlDatabase::database("production_conn"));
    upd.prepare(
        "UPDATE PRODUCTION SET "
        "  DATEPRODUCTION     = :date, "
        "  QUANTITEOLIVESKG   = :olives, "
        "  HUILEPRODUITEL     = :huile, "
        "  RENDEMENT          = :rend, "
        "  STATUT             = :statut, "
        "  TYPEPRESSAGE       = :type, "
        "  QUALITE            = :qualite, "
        "  NOTESPLANIFICATION = :notes "
        "WHERE IDPRODUCTION   = :id"
    );
    upd.bindValue(":date",   dlg.getDate().toString("yyyy-MM-dd"));
    upd.bindValue(":olives", dlg.getOlivesKg());
    upd.bindValue(":huile",  dlg.getHuileL());
    upd.bindValue(":rend",   newRend);
    upd.bindValue(":statut", dlg.getStatut());
    upd.bindValue(":type",   dlg.getTypePressage());
    QString qual = dlg.getQualite();
    upd.bindValue(":qualite", qual.isEmpty() ? QVariant() : QVariant(qual));
    upd.bindValue(":notes",  dlg.getNotes());
    upd.bindValue(":id",     productionId);

    if (upd.exec()) {
        showSuccessNotification(
            "✔  Production modifiée avec succès!\n\n"
            "ID : " + QString::number(productionId) + "\n"
            "Olives : " + QString::number(dlg.getOlivesKg()) + " kg\n"
            "Huile  : " + QString::number(dlg.getHuileL(), 'f', 2) + " L\n"
            "Rendement : " + QString::number(newRend, 'f', 2) + " %"
        );
        loadProductionHistory();
    } else {
        showErrorNotification("Échec de la mise à jour!\n\n" + upd.lastError().text());
    }
}

// ============================================================================
// REFRESH HISTORY
// ============================================================================

void Production::onRefreshHistoryClicked()
{
    loadProductionHistory();
    showSuccessNotification("Historique actualisé!");
}

void Production::refreshTable()
{
    loadProductionHistory();
}

void Production::clearForm()
{
    ui->spinOlivesKg->setValue(500);
    ui->spinOilProduced->setValue(0);
    ui->txtNotesPlanification->clear();
    ui->comboPressType->setCurrentIndex(0);
    currentProductionId = 0;
}
