#include "smartproductionassistant.h"

// ============================================================================
// CONSTRUCTOR
// ============================================================================

SmartProductionAssistant::SmartProductionAssistant(ProductionService *service, QObject *parent)
    : QObject(parent), m_service(service)
{
}

// ============================================================================
// PRIVATE HELPERS
// ============================================================================

QString SmartProductionAssistant::evaluerNiveau(double rendement)
{
    if (rendement >= 20.0) return "excellent";
    if (rendement >= 15.0) return "bon";
    if (rendement >= 12.0) return "moyen";
    if (rendement >= 5.0)  return "faible";
    return "critique";
}

double SmartProductionAssistant::calculerScore(const ProductionModel &prod)
{
    double score = 0;

    // Rendement component (0-40 points)
    double r = prod.getRendement();
    if (r >= 20) score += 40;
    else if (r >= 15) score += 30;
    else if (r >= 12) score += 20;
    else if (r > 0) score += 10;

    // Time efficiency (0-25 points)
    if (prod.getDureeEstimee() > 0 && prod.getTempsEcoule() > 0) {
        double ratio = static_cast<double>(prod.getTempsEcoule()) / prod.getDureeEstimee();
        if (ratio <= 1.0)      score += 25;
        else if (ratio <= 1.1) score += 20;
        else if (ratio <= 1.3) score += 10;
    } else {
        score += 15; // neutral
    }

    // Conformity (0-20 points)
    if (prod.getConformeNormes()) score += 20;

    // Quality (0-15 points)
    QString q = prod.getQualite().toLower();
    if (q.contains("excellent"))      score += 15;
    else if (q.contains("extra"))     score += 15;
    else if (q.contains("bon"))       score += 10;
    else if (q.contains("standard"))  score += 5;

    return qBound(0.0, score, 100.0);
}

// ============================================================================
// ANALYSIS
// ============================================================================

SmartProductionAssistant::AnalysisResult SmartProductionAssistant::analyserProduction(int idProduction)
{
    return analyserProduction(m_service->getProductionById(idProduction));
}

SmartProductionAssistant::AnalysisResult SmartProductionAssistant::analyserProduction(const ProductionModel &prod)
{
    AnalysisResult result;
    result.score  = calculerScore(prod);
    result.niveau = evaluerNiveau(prod.getRendement());

    // Build summary
    result.summary = "Production #" + QString::number(prod.getIdProduction()) +
                     " | Rendement: " + QString::number(prod.getRendement(), 'f', 1) + "%" +
                     " | Score: " + QString::number(result.score, 'f', 0) + "/100" +
                     " | Niveau: " + result.niveau;

    // Gather suggestions
    result.suggestions = suggererAmelioration(prod);

    // Anomaly detection
    auto anomalie = m_service->detecterAnomalie(prod);
    if (anomalie.hasAnomalie) {
        result.warnings << anomalie.description;
    }

    emit analysisComplete(prod.getIdProduction(), result);
    return result;
}

// ============================================================================
// SUGGESTIONS
// ============================================================================

QStringList SmartProductionAssistant::suggererAmelioration(int idProduction)
{
    return suggererAmelioration(m_service->getProductionById(idProduction));
}

QStringList SmartProductionAssistant::suggererAmelioration(const ProductionModel &prod)
{
    QStringList suggestions;
    double rendement = prod.getRendement();

    // ── Yield-based suggestions ─────────────────────────────────────────
    if (rendement > 0 && rendement < 10) {
        suggestions << "Rendement critique. Verifier l'etat de la presse et la qualite des olives.";
        suggestions << "Envisager un changement de machine.";
        suggestions << "Augmenter le temps de malaxage de 15-20 minutes.";
        suggestions << "Verifier la temperature du malaxeur (idealement 27-30 C).";
    } else if (rendement >= 10 && rendement < 12) {
        suggestions << "Rendement sous la moyenne. Augmenter le temps de malaxage.";
        suggestions << "Verifier la maturite des olives (indice optimal: 3-4).";
        suggestions << "Ajuster la vitesse de centrifugation.";
    } else if (rendement >= 12 && rendement < 15) {
        suggestions << "Rendement acceptable. Optimiser la temperature de malaxage.";
        suggestions << "Considerer le pressage a froid pour une meilleure qualite.";
    } else if (rendement >= 15 && rendement < 20) {
        suggestions << "Bon rendement. Maintenir les parametres actuels.";
        suggestions << "Documenter les conditions pour reproductibilite.";
    } else if (rendement >= 20) {
        suggestions << "Excellent rendement! Conserver cette configuration.";
    }

    // ── Press-type suggestions ──────────────────────────────────────────
    QString type = prod.getTypePressage().toLower();
    if (type.contains("tradition") && rendement < 15) {
        suggestions << "Passer d'un pressage traditionnel a la centrifugation pour de meilleurs rendements.";
    }

    // ── Time-based suggestions ──────────────────────────────────────────
    if (prod.getTempsEcoule() > 0 && prod.getDureeEstimee() > 0) {
        double ratio = static_cast<double>(prod.getTempsEcoule()) / prod.getDureeEstimee();
        if (ratio > 1.3) {
            suggestions << "Le temps de production depasse 130% de l'estimation. Revoir le processus.";
        }
    }

    // ── Zero oil suggestions ────────────────────────────────────────────
    if (prod.getHuileProduiteL() <= 0 && prod.getStatut() != "Planifiee") {
        suggestions << "Aucune huile enregistree. Verifier le capteur de debit ou saisir les donnees.";
    }

    if (suggestions.isEmpty()) {
        suggestions << "Aucune suggestion particuliere. La production semble bien se derouler.";
    }

    return suggestions;
}

// ============================================================================
// EXPLAIN LOW YIELD
// ============================================================================

QString SmartProductionAssistant::expliquerRendementFaible(const ProductionModel &prod)
{
    double rendement = prod.getRendement();
    if (rendement >= 15) {
        return "Le rendement de " + QString::number(rendement, 'f', 1) +
               "% est dans la norme. Aucune explication necessaire.";
    }

    QString explanation = "Analyse du rendement faible (" +
                          QString::number(rendement, 'f', 1) + "%):\n\n";

    explanation += "CAUSES POSSIBLES:\n";
    explanation += "1. Qualite des olives: maturite insuffisante ou exces d'eau.\n";
    explanation += "2. Temperature de malaxage trop basse (< 25 C) ou trop haute (> 35 C).\n";
    explanation += "3. Duree de malaxage insuffisante.\n";

    if (prod.getTypePressage().contains("tradition", Qt::CaseInsensitive)) {
        explanation += "4. Le pressage traditionnel a un rendement inferieur a la centrifugation.\n";
    }

    if (prod.getTempsEcoule() > prod.getDureeEstimee() && prod.getDureeEstimee() > 0) {
        explanation += "5. Depassement de temps: la presse peut etre usee ou mal calibree.\n";
    }

    explanation += "\nRECOMMANDATIONS:\n";
    explanation += "- Controler la temperature du malaxeur (27-30 C optimal).\n";
    explanation += "- Verifier l'etat des couteaux et du decanteur.\n";
    explanation += "- Envisager un lot d'olives de meilleure qualite.\n";

    return explanation;
}

// ============================================================================
// OPTIMIZATION
// ============================================================================

SmartProductionAssistant::OptimisationPlan SmartProductionAssistant::proposerOptimisation(int idProduction)
{
    return proposerOptimisation(m_service->getProductionById(idProduction));
}

SmartProductionAssistant::OptimisationPlan SmartProductionAssistant::proposerOptimisation(const ProductionModel &prod)
{
    OptimisationPlan plan;
    double rendement = prod.getRendement();

    // Recommend press type
    if (rendement < 12) {
        plan.typePressageRecommande = "Centrifugation 2 phases";
        plan.actions << "Changer le type de pressage pour centrifugation 2 phases.";
    } else if (rendement < 15) {
        plan.typePressageRecommande = "Pression a froid";
        plan.actions << "Optimiser le temps de malaxage (+10 min).";
    } else {
        plan.typePressageRecommande = prod.getTypePressage();
        plan.actions << "Maintenir la configuration actuelle.";
    }

    // Optimal duration
    plan.dureeOptimale = m_service->estimerDuree(prod.getQuantiteOlivesKg(),
                                                  plan.typePressageRecommande);

    // Target yield
    plan.rendementCible = 18.0; // industry target

    // Additional actions
    if (rendement < 15) {
        plan.actions << "Augmenter la temperature du malaxeur a 28 C.";
        plan.actions << "Allonger le malaxage de 10-15 minutes.";
        plan.actions << "Verifier la calibration du separateur.";
    }

    return plan;
}

// ============================================================================
// QUICK RESPONSES
// ============================================================================

QString SmartProductionAssistant::getQuickAnalysis(int idProduction)
{
    auto result = analyserProduction(idProduction);
    return result.summary;
}

QString SmartProductionAssistant::getStatusBrief()
{
    int count = m_service->getProductionCount();
    double avgRendement = m_service->getRendementMoyen();
    double totalHuile = m_service->getTotalHuileProduced();

    return "Etat global: " + QString::number(count) + " productions, " +
           "rendement moyen " + QString::number(avgRendement, 'f', 1) + "%, " +
           "total huile " + QString::number(totalHuile, 'f', 0) + " litres.";
}
