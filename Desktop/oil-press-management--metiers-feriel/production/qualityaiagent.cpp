#include "qualityaiagent.h"

#include <QtGlobal>

namespace {

static QString qualityFromAcidity(double acidity)
{
    if (acidity < 0.8) {
        return "Extra Vierge";
    }
    if (acidity <= 2.0) {
        return "Vierge";
    }
    return "Lampante";
}

static double acidityScore(double acidity)
{
    if (acidity < 0.8) {
        return 45.0;
    }
    if (acidity <= 2.0) {
        const double ratio = (acidity - 0.8) / 1.2;
        return 30.0 - ratio * 15.0;
    }
    if (acidity <= 3.5) {
        const double ratio = (acidity - 2.0) / 1.5;
        return 15.0 - ratio * 10.0;
    }
    return 3.0;
}

static double rendementScore(double rendement)
{
    if (rendement >= 18.0) {
        return 30.0;
    }
    if (rendement >= 15.0) {
        return 24.0;
    }
    if (rendement >= 12.0) {
        return 16.0;
    }
    if (rendement >= 9.0) {
        return 10.0;
    }
    return 5.0;
}

static int selectedQualityScore(const QString &selected)
{
    const QString low = selected.toLower();
    if (low.contains("excellent") || low.contains("extra")) {
        return 10;
    }
    if (low.contains("bonne") || low.contains("vierge")) {
        return 7;
    }
    if (low.contains("moyenne")) {
        return 4;
    }
    return 2;
}

} // namespace

QualityEvaluationResult QualityAIAgent::evaluate(const QualityEvaluationInput &input)
{
    QualityEvaluationResult out;

    const double aScore = acidityScore(input.acidityPercent);
    const double rScore = rendementScore(input.rendementPercent);
    const double qScore = selectedQualityScore(input.selectedQuality);
    const double conformityBonus = input.userConformity ? 5.0 : 0.0;

    out.score = qBound(0.0, aScore + rScore + qScore + conformityBonus, 100.0);
    out.qualityLabel = qualityFromAcidity(input.acidityPercent);

    if (input.acidityPercent > 2.0 || input.rendementPercent < 10.0) {
        out.riskLevel = "Eleve";
    } else if (input.acidityPercent > 1.0 || input.rendementPercent < 14.0) {
        out.riskLevel = "Moyen";
    } else {
        out.riskLevel = "Faible";
    }

    out.isConforme = (input.acidityPercent <= 2.0) && (input.rendementPercent >= 10.0);

    double confidence = 60.0;
    if (input.olivesKg > 0) confidence += 10.0;
    if (!input.operatorNotes.trimmed().isEmpty()) confidence += 10.0;
    if (input.acidityPercent > 0.0) confidence += 10.0;
    if (input.rendementPercent > 0.0) confidence += 10.0;
    out.confidence = qBound(0.0, confidence, 98.0);

    QString diagnosis;
    if (out.qualityLabel == "Extra Vierge") {
        diagnosis = "Diagnostic: Qualite premium avec acidite faible.";
    } else if (out.qualityLabel == "Vierge") {
        diagnosis = "Diagnostic: Qualite correcte, mais optimisation possible.";
    } else {
        diagnosis = "Diagnostic: Qualite non conforme premium, risque d'oxydation/fermentation.";
    }

    QString causes;
    if (input.acidityPercent > 2.0) {
        causes = "Causes probables: olives trop mures, delai long avant pressage, stockage inadapte.";
    } else if (input.rendementPercent < 12.0) {
        causes = "Causes probables: mauvais reglages de pressage ou humidite elevee de la pate.";
    } else {
        causes = "Causes probables: conditions globalement stables avec marge de progression process.";
    }

    QString actions;
    if (out.riskLevel == "Eleve") {
        actions = "Actions: trier les lots, reduire delai recolte->pressage, verifier malaxage et temperature.";
    } else if (out.riskLevel == "Moyen") {
        actions = "Actions: ajuster temps de malaxage, controler hygiene et recalibrer la centrifugation.";
    } else {
        actions = "Actions: maintenir le protocole, surveiller acidite lot par lot et tracer les parametres.";
    }

    out.smartRemarks = QString("%1\n%2\n%3\n\nScore IA: %4/100 | Confiance: %5% | Risque: %6")
        .arg(diagnosis)
        .arg(causes)
        .arg(actions)
        .arg(QString::number(out.score, 'f', 1))
        .arg(QString::number(out.confidence, 'f', 0))
        .arg(out.riskLevel);

    return out;
}
