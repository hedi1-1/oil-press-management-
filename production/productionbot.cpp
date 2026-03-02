#include "productionbot.h"
#include <QDebug>
#include <QRandomGenerator>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlDatabase>
#include <utility>  // std::as_const

// ============================================================================
// Constructor / Destructor
// ============================================================================
ProductionBot::ProductionBot(ProductionService *service,
                             SmartProductionAssistant *assistant,
                             QObject *parent)
    : QObject(parent)
    , m_service(service)
    , m_assistant(assistant)
    , m_timer(new QTimer(this))
    , m_nextDecisionId(1)
    , m_cycleCount(0)
{
    connect(m_timer, &QTimer::timeout, this, &ProductionBot::onCycle);
}

ProductionBot::~ProductionBot()
{
    stop();
}

// ============================================================================
// Controls
// ============================================================================
void ProductionBot::start(int intervalSec)
{
    if (!m_timer->isActive()) {
        m_cycleCount = 0;
        m_timer->start(intervalSec * 1000);
        qDebug() << "[Bot Employe] Commence le travail — cycle toutes les" << intervalSec << "s";
    }
}

void ProductionBot::stop()
{
    if (m_timer->isActive()) {
        m_timer->stop();
        qDebug() << "[Bot Employe] Arret du travail";
    }
}

bool ProductionBot::isRunning() const
{
    return m_timer->isActive();
}

// ============================================================================
// Decision Log
// ============================================================================
QList<BotDecision> ProductionBot::getDecisionLog() const
{
    return m_decisions;
}

void ProductionBot::clearDecisionLog()
{
    m_decisions.clear();
    m_nextDecisionId = 1;
}

// ============================================================================
// Timer Cycle — Le bot fait son travail étape par étape
// ============================================================================
void ProductionBot::onCycle()
{
    m_cycleCount++;
    qDebug() << "[Bot Employe] === Cycle" << m_cycleCount << "===" << QDateTime::currentDateTime().toString("HH:mm:ss");
    int before = m_decisions.size();

    // Étape 1 : Planifier (seulement tous les 3 cycles pour ne pas spammer)
    if (m_cycleCount % 3 == 1) {
        etape1_Planifier();
    }

    // Étape 2 : Lancer les productions planifiées
    etape2_LancerPlanifiees();

    // Étape 3 : Travailler sur les productions en cours
    etape3_TravaillerEnCours();

    // Étape 4 : Contrôle qualité
    etape4_ControleQualite();

    // Étape 5 : Détecter anomalies
    etape5_DetecterAnomalies();

    int newDecisions = m_decisions.size() - before;
    if (newDecisions > 0) {
        emit productionTableChanged();
    }
    emit botCycleComplete(newDecisions);
    qDebug() << "[Bot Employe] Cycle" << m_cycleCount << "termine —" << newDecisions << "actions";
}

// ============================================================================
// Étape 1 : PLANIFICATION — Crée une production dans la BD
// ============================================================================
void ProductionBot::etape1_Planifier()
{
    // Types de pressage possibles
    QStringList types = {"Pression a froid", "Centrifugation 2 phases", "Traditionnelle"};

    // Quantités réalistes (200–1000 kg)
    int qty = 200 + (QRandomGenerator::global()->bounded(9)) * 100; // 200,300,...,1000
    QString type = types[QRandomGenerator::global()->bounded(types.size())];

    ProductionModel prod;
    prod.setQuantiteOlivesKg(qty);
    prod.setTypePressage(type);
    prod.setNotesPlanification(QString("Planifie par Bot Employe — Cycle %1").arg(m_cycleCount));
    prod.setDateProduction(QDate::currentDate());
    prod.setStatut("Planifie");
    prod.setHeureDemarrage(QTime::currentTime());
    prod.setIdClient(0);
    prod.setIdUser(0);
    prod.setIdMachine(0);
    prod.setIdStock(0);
    prod.setDureeEstimee(m_service->estimerDuree(qty, type));
    prod.setTempsEcoule(0);
    prod.setHuileProduiteL(0);
    prod.setRendement(0);

    if (prod.addProduction()) {
        logDecision(prod.getIdProduction(),
                    QString("Planification : %1 kg, %2, duree estimee %3 min")
                        .arg(qty).arg(type).arg(prod.getDureeEstimee()),
                    "positif", "planification");
        qDebug() << "[Bot Employe] Production planifiee ID:" << prod.getIdProduction();
    } else {
        qDebug() << "[Bot Employe] ERREUR planification";
    }
}

// ============================================================================
// Étape 2 : LANCEMENT — Démarre les productions au statut "Planifie"
// ============================================================================
void ProductionBot::etape2_LancerPlanifiees()
{
    const QList<ProductionModel> all = m_service->getAllProductions();
    int launched = 0;

    for (const auto &prod : std::as_const(all)) {
        if (prod.getStatut() == "Planifie" || prod.getStatut() == "Planifiee") {
            if (m_service->demarrerProduction(prod.getIdProduction())) {
                logDecision(prod.getIdProduction(),
                            QString("Lancement : Production #%1 demarree (%2 kg)")
                                .arg(prod.getIdProduction()).arg(prod.getQuantiteOlivesKg()),
                            "positif", "lancement");
                launched++;
            }
            // Lance max 2 par cycle
            if (launched >= 2) break;
        }
    }
}

// ============================================================================
// Étape 3 : PRODUCTION — Simule le travail sur les productions en cours
//           Met à jour huile produite et rendement dans la BD
// ============================================================================
void ProductionBot::etape3_TravaillerEnCours()
{
    const QList<ProductionModel> all = m_service->getAllProductions();

    for (const auto &prod : std::as_const(all)) {
        if (prod.getStatut() != "En cours") continue;

        int id = prod.getIdProduction();
        int olivesKg = prod.getQuantiteOlivesKg();

        // Calculer l'huile produite (rendement réaliste 14-22%)
        double rendementCible = 14.0 + (QRandomGenerator::global()->bounded(80)) / 10.0; // 14.0 à 22.0
        double huileFinale = (olivesKg * rendementCible) / 100.0;

        // Use service layer to terminate production properly
        if (m_service->terminerProduction(id, huileFinale)) {
            logDecision(id,
                        QString("Production : #%1 terminee — %2 L d'huile, rendement %3%%")
                            .arg(id)
                            .arg(huileFinale, 0, 'f', 1)
                            .arg(rendementCible, 0, 'f', 1),
                        "positif", "production");
        }
    }
}

// ============================================================================
// Étape 4 : CONTRÔLE QUALITÉ — Valide la qualité des productions terminées
// ============================================================================
void ProductionBot::etape4_ControleQualite()
{
    const QList<ProductionModel> all = m_service->getAllProductions();

    for (const auto &prod : std::as_const(all)) {
        if (prod.getStatut() != "Termine") continue;
        // Déjà validé ?
        if (!prod.getQualite().isEmpty() && prod.getQualite() != "Non verifie") continue;

        int id = prod.getIdProduction();
        double rendement = prod.getRendement();

        // Règle de qualité
        QString qualite;
        if (rendement >= 18.0) {
            qualite = "Excellente";
        } else if (rendement >= 15.0) {
            qualite = "Bonne";
        } else if (rendement >= 12.0) {
            qualite = "Moyenne";
        } else {
            qualite = "Faible";
        }

        // Use service layer for quality validation
        if (m_service->validerConformite(id, qualite)) {
            bool conforme = m_service->isConforme(rendement, qualite);
            QString icon = conforme ? "CONFORME" : "NON CONFORME";
            logDecision(id,
                        QString("Qualite : #%1 — %2 (%3), rendement %4%%")
                            .arg(id).arg(qualite, icon).arg(rendement, 0, 'f', 1),
                        conforme ? "positif" : "negatif", "qualite");
        }
    }
}

// ============================================================================
// Étape 5 : DÉTECTION ANOMALIES
// ============================================================================
void ProductionBot::etape5_DetecterAnomalies()
{
    const QList<ProductionModel> all = m_service->getAllProductions();

    for (const auto &prod : std::as_const(all)) {
        if (prod.getStatut() == "Planifie" || prod.getStatut() == "Planifiee") continue;

        ProductionService::AnomalieInfo anomalie = m_service->detecterAnomalie(prod);
        if (anomalie.hasAnomalie) {
            // Avoid repeated anomaly alerts (check if we already logged for this prod)
            bool alreadyLogged = false;
            for (const auto &d : std::as_const(m_decisions)) {
                if (d.idProduction == prod.getIdProduction() && d.type == "anomalie") {
                    alreadyLogged = true;
                    break;
                }
            }
            if (!alreadyLogged) {
                logDecision(prod.getIdProduction(),
                            QString("Anomalie : #%1 — %2")
                                .arg(prod.getIdProduction()).arg(anomalie.description),
                            (anomalie.severity == "critical") ? "negatif" : "neutre",
                            "anomalie");
                emit alerteAnomalie(prod.getIdProduction(), anomalie.description);
            }
        }
    }
}

// ============================================================================
// Internal Helpers
// ============================================================================
void ProductionBot::logDecision(int idProduction, const QString &description,
                                const QString &impact, const QString &type)
{
    BotDecision d;
    d.idDecision   = m_nextDecisionId++;
    d.idProduction = idProduction;
    d.description  = description;
    d.dateDecision = QDateTime::currentDateTime();
    d.impact       = impact;
    d.type         = type;
    m_decisions.append(d);
    emit newDecision(d);

    qDebug() << "[Bot Employe]" << type << ":" << description;
}
