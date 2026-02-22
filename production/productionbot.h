#ifndef PRODUCTIONBOT_H
#define PRODUCTIONBOT_H

#include <QObject>
#include <QTimer>
#include <QDateTime>
#include <QList>
#include "productionservice.h"
#include "smartproductionassistant.h"

// ============================================================================
// BotDecision — Journal d'activité du bot employé
// ============================================================================
struct BotDecision {
    int     idDecision     = 0;
    int     idProduction   = 0;
    QString description;
    QDateTime dateDecision;
    QString impact;        // "positif" | "neutre" | "negatif"
    QString type;          // "planification" | "lancement" | "production" | "qualite" | "anomalie"
};

// ============================================================================
// ProductionBot — Employé Virtuel
//
// Quand activé, le bot travaille comme un employé réel :
//   1. Planification → crée de nouvelles productions (écrit dans la BD)
//   2. Lancement     → démarre les productions planifiées
//   3. Production    → simule le travail (met à jour huile produite)
//   4. Qualité       → contrôle qualité des productions terminées
//   5. Anomalies     → détecte et signale les problèmes
//
// PAS de rapport — ce n'est pas son rôle.
// Toutes les actions sont traçables dans la BD.
// ============================================================================
class ProductionBot : public QObject
{
    Q_OBJECT

public:
    explicit ProductionBot(ProductionService *service,
                           SmartProductionAssistant *assistant,
                           QObject *parent = nullptr);
    ~ProductionBot();

    // ── Controls ────────────────────────────────────────────────────────
    void start(int intervalSec = 10);
    void stop();
    bool isRunning() const;

    // ── Decision log ────────────────────────────────────────────────────
    QList<BotDecision> getDecisionLog() const;
    void clearDecisionLog();

signals:
    void botCycleComplete(int decisionsCount);
    void newDecision(const BotDecision &decision);
    void alerteAnomalie(int idProduction, const QString &message);
    void productionTableChanged();  // signal to refresh UI table

private slots:
    void onCycle();

private:
    ProductionService        *m_service;
    SmartProductionAssistant *m_assistant;
    QTimer                   *m_timer;
    QList<BotDecision>        m_decisions;
    int                       m_nextDecisionId;
    int                       m_cycleCount;

    void logDecision(int idProduction, const QString &description,
                     const QString &impact, const QString &type);

    // ── Étapes du travail de l'employé ──────────────────────────────────
    void etape1_Planifier();           // Crée une production planifiée dans la BD
    void etape2_LancerPlanifiees();    // Démarre les productions planifiées
    void etape3_TravaillerEnCours();   // Simule le travail (huile produite)
    void etape4_ControleQualite();     // Valide la qualité des terminées
    void etape5_DetecterAnomalies();   // Vérifie les anomalies
};

#endif // PRODUCTIONBOT_H
