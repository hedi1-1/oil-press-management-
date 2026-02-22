#ifndef PRODUCTIONSERVICE_H
#define PRODUCTIONSERVICE_H

#include <QObject>
#include <QTimer>
#include <QDateTime>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QDebug>
#include "productionmodel.h"

// ============================================================================
// ProductionService — Business Logic Layer
// All business rules, validations, and domain logic live here.
// The UI layer (Production widget) delegates to this service.
// ============================================================================
class ProductionService : public QObject
{
    Q_OBJECT

public:
    explicit ProductionService(QObject *parent = nullptr);
    ~ProductionService();

    // ── CRUD ────────────────────────────────────────────────────────────
    bool addProduction(ProductionModel &prod);
    bool updateProduction(ProductionModel &prod);
    bool deleteProduction(int idProduction);
    ProductionModel getProductionById(int idProduction);
    QList<ProductionModel> getAllProductions();

    // ── PLANIFICATION ───────────────────────────────────────────────────
    bool planifierProduction(ProductionModel &prod);
    int  estimerDuree(int quantiteKg, const QString &typePressage);

    // ── LANCEMENT / ARRÊT ───────────────────────────────────────────────
    bool demarrerProduction(int idProduction);
    bool arreterProduction(int idProduction);
    bool terminerProduction(int idProduction, double huileProduiteL);

    // ── CALCUL RENDEMENT ────────────────────────────────────────────────
    double calculerRendement(int idProduction);
    double calculerRendement(double huileL, int olivesKg);

    // ── VALIDATION QUALITÉ ──────────────────────────────────────────────
    bool validerConformite(int idProduction, const QString &qualite);
    bool isConforme(double rendement, const QString &qualite);

    // ── DÉTECTION ANOMALIE ──────────────────────────────────────────────
    struct AnomalieInfo {
        bool   hasAnomalie  = false;
        QString description;
        QString severity;   // "warning" | "critical"
    };
    AnomalieInfo detecterAnomalie(int idProduction);
    AnomalieInfo detecterAnomalie(const ProductionModel &prod);

    // ── RAPPORT ─────────────────────────────────────────────────────────
    struct RapportProduction {
        int    totalProductions   = 0;
        double totalOlivesKg      = 0;
        double totalHuileL        = 0;
        double rendementMoyen     = 0;
        double tauxConformite     = 0;
        int    nbAnomalies        = 0;
        QDate  dateGeneration;
    };
    RapportProduction genererRapportProduction();
    bool exporterRapport(const RapportProduction &rapport, const QString &filePath);

    // ── RECHERCHE & TRI ─────────────────────────────────────────────────
    QList<ProductionModel> rechercherParDate(const QDate &dateDebut, const QDate &dateFin);
    QList<ProductionModel> rechercherParStatut(const QString &statut);
    QList<ProductionModel> trierParRendement(bool ascending = false);
    QList<ProductionModel> trierParQuantite(bool ascending = true);

    // ── STATISTIQUES ────────────────────────────────────────────────────
    double getRendementMoyen();
    int    getProductionCount();
    double getTotalHuileProduced();

signals:
    void productionAdded(int idProduction);
    void productionUpdated(int idProduction);
    void productionDeleted(int idProduction);
    void productionStarted(int idProduction);
    void productionStopped(int idProduction);
    void anomalieDetected(int idProduction, const QString &description);
    void rendementCalculated(int idProduction, double rendement);
    void rapportGenerated(const RapportProduction &rapport);

private:
    QSqlDatabase db();
    QList<ProductionModel> queryToList(QSqlQuery &query);
    ProductionModel rowToModel(QSqlQuery &query);
    bool updateField(int idProduction, const QString &field, const QVariant &value);
    bool updateFields(int idProduction, const QMap<QString, QVariant> &fields);
};

#endif // PRODUCTIONSERVICE_H
