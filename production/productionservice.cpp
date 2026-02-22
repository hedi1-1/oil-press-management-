#include "productionservice.h"
#include <QFile>
#include <QTextStream>
#include <cmath>

// ============================================================================
// CONSTRUCTOR / DESTRUCTOR
// ============================================================================

ProductionService::ProductionService(QObject *parent)
    : QObject(parent)
{
}

ProductionService::~ProductionService()
{
}

// ============================================================================
// PRIVATE HELPERS
// ============================================================================

QSqlDatabase ProductionService::db()
{
    return QSqlDatabase::database("production_conn");
}

ProductionModel ProductionService::rowToModel(QSqlQuery &query)
{
    ProductionModel p;
    p.setIdProduction(query.value("IDPRODUCTION").toInt());
    p.setDateProduction(query.value("DATEPRODUCTION").toDate());
    p.setQuantiteOlivesKg(query.value("QUANTITEOLIVESKG").toInt());
    p.setTypePressage(query.value("TYPEPRESSAGE").toString());
    p.setNotesPlanification(query.value("NOTESPLANIFICATION").toString());
    p.setDureeEstimee(query.value("DUREEESTIMEE").toInt());
    p.setTempsEcoule(query.value("TEMPSECOULE").toInt());
    p.setStatut(query.value("STATUT").toString());
    p.setHuileProduiteL(query.value("HUILEPRODUITEL").toDouble());
    p.setRendement(query.value("RENDEMENT").toDouble());
    p.setQualite(query.value("QUALITE").toString());
    p.setConformeNormes(query.value("CONFORMENORMES").toInt() == 1);
    p.setRemarquesQualite(query.value("REMARQUESQUALITE").toString());
    p.setDateGenerationRapport(query.value("DATEGENERATIONRAPPORT").toDate());
    return p;
}

QList<ProductionModel> ProductionService::queryToList(QSqlQuery &query)
{
    QList<ProductionModel> list;
    while (query.next()) {
        list.append(rowToModel(query));
    }
    return list;
}

bool ProductionService::updateField(int idProduction, const QString &field, const QVariant &value)
{
    QMap<QString, QVariant> fields;
    fields[field] = value;
    return updateFields(idProduction, fields);
}

bool ProductionService::updateFields(int idProduction, const QMap<QString, QVariant> &fields)
{
    if (fields.isEmpty()) return false;

    QStringList setClauses;
    for (auto it = fields.cbegin(); it != fields.cend(); ++it) {
        setClauses << (it.key() + " = :" + it.key());
    }

    QSqlQuery query(db());
    query.prepare("UPDATE PRODUCTION SET " + setClauses.join(", ") +
                  " WHERE IDPRODUCTION = :idProduction");

    for (auto it = fields.cbegin(); it != fields.cend(); ++it) {
        query.bindValue(":" + it.key(), it.value());
    }
    query.bindValue(":idProduction", idProduction);

    if (query.exec()) {
        qDebug() << "[ProductionService] Updated ID" << idProduction << "fields:" << fields.keys();
        return true;
    }
    qDebug() << "[ProductionService] Update failed:" << query.lastError().text();
    return false;
}

// ============================================================================
// CRUD
// ============================================================================

bool ProductionService::addProduction(ProductionModel &prod)
{
    if (prod.getQuantiteOlivesKg() <= 0) {
        qDebug() << "[ProductionService] Validation failed: quantity must be > 0";
        return false;
    }
    prod.calculateRendement();
    bool ok = prod.addProduction();
    if (ok) emit productionAdded(prod.getIdProduction());
    return ok;
}

bool ProductionService::updateProduction(ProductionModel &prod)
{
    prod.calculateRendement();
    bool ok = prod.updateProduction();
    if (ok) emit productionUpdated(prod.getIdProduction());
    return ok;
}

bool ProductionService::deleteProduction(int idProduction)
{
    bool ok = ProductionModel::deleteProduction(idProduction);
    if (ok) emit productionDeleted(idProduction);
    return ok;
}

ProductionModel ProductionService::getProductionById(int idProduction)
{
    QSqlQuery query(db());
    query.prepare("SELECT * FROM PRODUCTION WHERE IDPRODUCTION = :id");
    query.bindValue(":id", idProduction);

    if (query.exec() && query.next()) {
        return rowToModel(query);
    }
    qDebug() << "[ProductionService] Production not found. ID:" << idProduction;
    return ProductionModel();
}

QList<ProductionModel> ProductionService::getAllProductions()
{
    QSqlQuery query(db());
    query.exec("SELECT * FROM PRODUCTION ORDER BY IDPRODUCTION DESC");
    return queryToList(query);
}

// ============================================================================
// PLANIFICATION
// ============================================================================

int ProductionService::estimerDuree(int quantiteKg, const QString &typePressage)
{
    // Business rule: duration depends on press type and quantity
    double rateKgPerMin = 5.0; // default
    if (typePressage.contains("froid", Qt::CaseInsensitive))       rateKgPerMin = 4.0;
    else if (typePressage.contains("Centrifug", Qt::CaseInsensitive)) rateKgPerMin = 7.0;
    else if (typePressage.contains("tradition", Qt::CaseInsensitive)) rateKgPerMin = 3.0;

    int minutes = static_cast<int>(std::ceil(quantiteKg / rateKgPerMin));
    return qMax(minutes, 10); // minimum 10 min
}

bool ProductionService::planifierProduction(ProductionModel &prod)
{
    if (prod.getQuantiteOlivesKg() <= 0) {
        qDebug() << "[ProductionService] planifierProduction: quantity must be > 0";
        return false;
    }

    prod.setStatut("Planifiee");
    prod.setDateProduction(QDate::currentDate());
    prod.setDureeEstimee(estimerDuree(prod.getQuantiteOlivesKg(), prod.getTypePressage()));
    prod.setTempsEcoule(0);
    prod.setHuileProduiteL(0);
    prod.setRendement(0);
    prod.setConformeNormes(false);

    return addProduction(prod);
}

// ============================================================================
// LANCEMENT / ARRÊT
// ============================================================================

bool ProductionService::demarrerProduction(int idProduction)
{
    QMap<QString, QVariant> fields;
    fields["STATUT"]          = "En cours";
    fields["HEUREDEMARRAGE"]  = QDateTime::currentDateTime();
    fields["TEMPSECOULE"]     = 0;

    bool ok = updateFields(idProduction, fields);
    if (ok) emit productionStarted(idProduction);
    return ok;
}

bool ProductionService::arreterProduction(int idProduction)
{
    QMap<QString, QVariant> fields;
    fields["STATUT"] = "Arrete";

    bool ok = updateFields(idProduction, fields);
    if (ok) emit productionStopped(idProduction);
    return ok;
}

bool ProductionService::terminerProduction(int idProduction, double huileProduiteL)
{
    ProductionModel prod = getProductionById(idProduction);
    double rendement = 0;
    if (prod.getQuantiteOlivesKg() > 0) {
        rendement = (huileProduiteL / prod.getQuantiteOlivesKg()) * 100.0;
    }

    QMap<QString, QVariant> fields;
    fields["STATUT"]        = "Termine";
    fields["HUILEPRODUITEL"] = huileProduiteL;
    fields["RENDEMENT"]     = rendement;

    return updateFields(idProduction, fields);
}

// ============================================================================
// CALCUL RENDEMENT
// ============================================================================

double ProductionService::calculerRendement(int idProduction)
{
    ProductionModel prod = getProductionById(idProduction);
    double rendement = calculerRendement(prod.getHuileProduiteL(), prod.getQuantiteOlivesKg());

    updateField(idProduction, "RENDEMENT", rendement);
    emit rendementCalculated(idProduction, rendement);

    if (rendement < 12.0 && rendement > 0) {
        emit anomalieDetected(idProduction,
            "Rendement faible detecte: " + QString::number(rendement, 'f', 2) + "%");
    }
    return rendement;
}

double ProductionService::calculerRendement(double huileL, int olivesKg)
{
    if (olivesKg <= 0) return 0;
    return (huileL / static_cast<double>(olivesKg)) * 100.0;
}

// ============================================================================
// VALIDATION QUALITÉ
// ============================================================================

bool ProductionService::isConforme(double rendement, const QString &qualite)
{
    return (rendement >= 15.0 && qualite.compare("Excellente", Qt::CaseInsensitive) == 0);
}

bool ProductionService::validerConformite(int idProduction, const QString &qualite)
{
    ProductionModel prod = getProductionById(idProduction);
    bool conforme = isConforme(prod.getRendement(), qualite);

    QMap<QString, QVariant> fields;
    fields["QUALITE"]        = qualite;
    fields["CONFORMENORMES"] = conforme ? 1 : 0;
    fields["STATUT"]         = conforme ? "Qualite validee" : "Non conforme";

    return updateFields(idProduction, fields);
}

// ============================================================================
// DÉTECTION ANOMALIE
// ============================================================================

ProductionService::AnomalieInfo ProductionService::detecterAnomalie(int idProduction)
{
    return detecterAnomalie(getProductionById(idProduction));
}

ProductionService::AnomalieInfo ProductionService::detecterAnomalie(const ProductionModel &prod)
{
    AnomalieInfo info;
    QStringList problems;

    // Rule 1: low yield
    if (prod.getRendement() > 0 && prod.getRendement() < 12.0) {
        problems << "Rendement faible (" + QString::number(prod.getRendement(), 'f', 1) + "%)";
        info.severity = "critical";
    }

    // Rule 2: over-time
    if (prod.getTempsEcoule() > 0 && prod.getDureeEstimee() > 0 &&
        prod.getTempsEcoule() > prod.getDureeEstimee()) {
        int overMin = prod.getTempsEcoule() - prod.getDureeEstimee();
        problems << "Depassement de duree de " + QString::number(overMin) + " min";
        if (info.severity.isEmpty()) info.severity = "warning";
    }

    // Rule 3: no oil after long time (> 60 min elapsed but 0 oil)
    if (prod.getTempsEcoule() >= 60 && prod.getHuileProduiteL() <= 0 &&
        prod.getStatut() == "En cours") {
        problems << "Aucune huile produite apres " + QString::number(prod.getTempsEcoule()) + " min";
        info.severity = "critical";
    }

    if (!problems.isEmpty()) {
        info.hasAnomalie  = true;
        info.description  = problems.join(" | ");
    }
    return info;
}

// ============================================================================
// RAPPORT
// ============================================================================

ProductionService::RapportProduction ProductionService::genererRapportProduction()
{
    RapportProduction rapport;
    rapport.dateGeneration = QDate::currentDate();

    QSqlQuery query(db());
    query.exec("SELECT COUNT(*), "
               "NVL(SUM(QUANTITEOLIVESKG),0), "
               "NVL(SUM(HUILEPRODUITEL),0), "
               "NVL(AVG(RENDEMENT),0), "
               "NVL(SUM(CASE WHEN CONFORMENORMES = 1 THEN 1 ELSE 0 END),0), "
               "NVL(SUM(CASE WHEN RENDEMENT < 12 AND RENDEMENT > 0 THEN 1 ELSE 0 END),0) "
               "FROM PRODUCTION");

    if (query.next()) {
        rapport.totalProductions = query.value(0).toInt();
        rapport.totalOlivesKg    = query.value(1).toDouble();
        rapport.totalHuileL      = query.value(2).toDouble();
        rapport.rendementMoyen   = query.value(3).toDouble();
        int conformeCount        = query.value(4).toInt();
        rapport.nbAnomalies      = query.value(5).toInt();

        if (rapport.totalProductions > 0) {
            rapport.tauxConformite = (conformeCount * 100.0) / rapport.totalProductions;
        }
    }

    emit rapportGenerated(rapport);
    return rapport;
}

bool ProductionService::exporterRapport(const RapportProduction &rapport, const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) return false;

    QTextStream out(&file);
    out << "================================================================\n";
    out << "       PRESS IQ - Rapport de Production\n";
    out << "       Date: " << rapport.dateGeneration.toString("dd/MM/yyyy") << "\n";
    out << "================================================================\n\n";
    out << "Total productions  : " << rapport.totalProductions << "\n";
    out << "Total olives (kg)  : " << QString::number(rapport.totalOlivesKg, 'f', 0) << "\n";
    out << "Total huile (L)    : " << QString::number(rapport.totalHuileL, 'f', 2) << "\n";
    out << "Rendement moyen    : " << QString::number(rapport.rendementMoyen, 'f', 2) << "%\n";
    out << "Taux de conformite : " << QString::number(rapport.tauxConformite, 'f', 1) << "%\n";
    out << "Anomalies          : " << rapport.nbAnomalies << "\n";
    out << "================================================================\n";
    file.close();
    return true;
}

// ============================================================================
// RECHERCHE & TRI
// ============================================================================

QList<ProductionModel> ProductionService::rechercherParDate(const QDate &dateDebut, const QDate &dateFin)
{
    QSqlQuery query(db());
    query.prepare("SELECT * FROM PRODUCTION "
                  "WHERE DATEPRODUCTION BETWEEN :d1 AND :d2 "
                  "ORDER BY DATEPRODUCTION DESC");
    query.bindValue(":d1", dateDebut);
    query.bindValue(":d2", dateFin);
    query.exec();
    return queryToList(query);
}

QList<ProductionModel> ProductionService::rechercherParStatut(const QString &statut)
{
    QSqlQuery query(db());
    query.prepare("SELECT * FROM PRODUCTION WHERE UPPER(STATUT) = UPPER(:statut) "
                  "ORDER BY IDPRODUCTION DESC");
    query.bindValue(":statut", statut);
    query.exec();
    return queryToList(query);
}

QList<ProductionModel> ProductionService::trierParRendement(bool ascending)
{
    QString order = ascending ? "ASC" : "DESC";
    QSqlQuery query(db());
    query.exec("SELECT * FROM PRODUCTION ORDER BY RENDEMENT " + order);
    return queryToList(query);
}

QList<ProductionModel> ProductionService::trierParQuantite(bool ascending)
{
    QString order = ascending ? "ASC" : "DESC";
    QSqlQuery query(db());
    query.exec("SELECT * FROM PRODUCTION ORDER BY QUANTITEOLIVESKG " + order);
    return queryToList(query);
}

// ============================================================================
// STATISTIQUES
// ============================================================================

double ProductionService::getRendementMoyen()
{
    QSqlQuery query(db());
    query.exec("SELECT NVL(AVG(RENDEMENT),0) FROM PRODUCTION WHERE RENDEMENT > 0");
    if (query.next()) return query.value(0).toDouble();
    return 0;
}

int ProductionService::getProductionCount()
{
    QSqlQuery query(db());
    query.exec("SELECT COUNT(*) FROM PRODUCTION");
    if (query.next()) return query.value(0).toInt();
    return 0;
}

double ProductionService::getTotalHuileProduced()
{
    QSqlQuery query(db());
    query.exec("SELECT NVL(SUM(HUILEPRODUITEL),0) FROM PRODUCTION");
    if (query.next()) return query.value(0).toDouble();
    return 0;
}
