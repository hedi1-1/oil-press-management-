#include "intentanalyzer.h"
#include <QRegularExpression>
#include <QDebug>

// ============================================================================
// Constructor
// ============================================================================
IntentAnalyzer::IntentAnalyzer(QObject *parent)
    : QObject(parent)
{
}

// ============================================================================
// Public — analyzeText
// ============================================================================
IntentResult IntentAnalyzer::analyzeText(const QString &text)
{
    IntentResult result;
    result.rawText = text;

    double confidence = 0.0;
    result.intent        = detectIntent(text, confidence);
    result.confidence    = confidence;
    result.idProduction  = extractProductionId(text);
    result.quantity      = extractQuantity(text);
    result.typePressage  = extractTypePressage(text);

    qDebug() << "[IntentAnalyzer] Intent:" << intentToString(result.intent)
             << " conf:" << result.confidence
             << " id:" << result.idProduction
             << " qty:" << result.quantity
             << " type:" << result.typePressage;

    return result;
}

// ============================================================================
// Intent detection — keyword / pattern matching
// ============================================================================
IntentType IntentAnalyzer::detectIntent(const QString &text, double &confidence)
{
    QString t = text.toLower().trimmed();

    // ── CREATE ──────────────────────────────────────────────────────────
    if (t.contains(QRegularExpression("cr[ée]er|ajouter|nouvelle|planifier|nouveau"))) {
        confidence = 0.85;
        return IntentType::CREATE_PRODUCTION;
    }

    // ── START ───────────────────────────────────────────────────────────
    if (t.contains(QRegularExpression("d[ée]marrer|lancer|commencer|start"))) {
        confidence = 0.90;
        return IntentType::START_PRODUCTION;
    }

    // ── STOP ────────────────────────────────────────────────────────────
    if (t.contains(QRegularExpression("arr[êe]ter|stopper|stop|pause"))) {
        confidence = 0.90;
        return IntentType::STOP_PRODUCTION;
    }

    // ── RENDEMENT ───────────────────────────────────────────────────────
    if (t.contains(QRegularExpression("rendement|yield|pourcentage|taux"))) {
        confidence = 0.85;
        return IntentType::GET_RENDEMENT;
    }

    // ── STATUS ──────────────────────────────────────────────────────────
    if (t.contains(QRegularExpression("statut|status|[ée]tat|situation"))) {
        confidence = 0.80;
        return IntentType::GET_STATUS;
    }

    // ── QUALITY ─────────────────────────────────────────────────────────
    if (t.contains(QRegularExpression("qualit[ée]|valider|conforme|norme"))) {
        confidence = 0.85;
        return IntentType::VALIDATE_QUALITY;
    }

    // ── REPORT ──────────────────────────────────────────────────────────
    if (t.contains(QRegularExpression("rapport|report|r[ée]sum[ée]|bilan"))) {
        confidence = 0.85;
        return IntentType::GENERATE_REPORT;
    }

    // ── ANOMALY ─────────────────────────────────────────────────────────
    if (t.contains(QRegularExpression("anomalie|probl[èe]me|erreur|alerte|d[ée]faut"))) {
        confidence = 0.85;
        return IntentType::DETECT_ANOMALY;
    }

    // ── LIST ────────────────────────────────────────────────────────────
    if (t.contains(QRegularExpression("liste|lister|afficher|montrer|voir|toutes"))) {
        confidence = 0.80;
        return IntentType::LIST_PRODUCTIONS;
    }

    // ── ANALYZE ─────────────────────────────────────────────────────────
    if (t.contains(QRegularExpression("analyser|analyse|diagnostic|[ée]valuer"))) {
        confidence = 0.85;
        return IntentType::ANALYZE_PRODUCTION;
    }

    // ── OPTIMIZE ────────────────────────────────────────────────────────
    if (t.contains(QRegularExpression("optimi[sz]er|am[ée]liorer|suggestion"))) {
        confidence = 0.80;
        return IntentType::OPTIMIZE;
    }

    // ── HELP ────────────────────────────────────────────────────────────
    if (t.contains(QRegularExpression("aide|help|comment|quoi|que faire"))) {
        confidence = 0.90;
        return IntentType::HELP;
    }

    confidence = 0.0;
    return IntentType::UNKNOWN;
}

// ============================================================================
// Parameter Extraction
// ============================================================================
int IntentAnalyzer::extractProductionId(const QString &text)
{
    // Match patterns like "production 42", "#42", "id 42", "numéro 42"
    QRegularExpression re("(?:production|id|num[ée]ro|#)\\s*(\\d+)",
                          QRegularExpression::CaseInsensitiveOption);
    auto match = re.match(text);
    if (match.hasMatch()) {
        return match.captured(1).toInt();
    }
    return -1;
}

float IntentAnalyzer::extractQuantity(const QString &text)
{
    // Match patterns like "500 kg", "500kg", "quantité 500"
    QRegularExpression re("(\\d+(?:\\.\\d+)?)\\s*(?:kg|kilo|litres?|l\\b)",
                          QRegularExpression::CaseInsensitiveOption);
    auto match = re.match(text);
    if (match.hasMatch()) {
        return match.captured(1).toFloat();
    }

    // Try "quantité X"
    QRegularExpression re2("quantit[ée]\\s+(\\d+(?:\\.\\d+)?)",
                           QRegularExpression::CaseInsensitiveOption);
    auto match2 = re2.match(text);
    if (match2.hasMatch()) {
        return match2.captured(1).toFloat();
    }

    return -1;
}

QString IntentAnalyzer::extractTypePressage(const QString &text)
{
    QString t = text.toLower();

    if (t.contains("froid") || t.contains("pression"))
        return "Pression à froid";
    if (t.contains("centrifug"))
        return "Centrifugation 2 phases";
    if (t.contains("tradition"))
        return "Traditionnelle";

    return QString();
}

// ============================================================================
// Utility
// ============================================================================
QString IntentAnalyzer::intentToString(IntentType intent)
{
    switch (intent) {
    case IntentType::CREATE_PRODUCTION:  return "CREATE_PRODUCTION";
    case IntentType::START_PRODUCTION:   return "START_PRODUCTION";
    case IntentType::STOP_PRODUCTION:    return "STOP_PRODUCTION";
    case IntentType::GET_RENDEMENT:      return "GET_RENDEMENT";
    case IntentType::GET_STATUS:         return "GET_STATUS";
    case IntentType::VALIDATE_QUALITY:   return "VALIDATE_QUALITY";
    case IntentType::GENERATE_REPORT:    return "GENERATE_REPORT";
    case IntentType::DETECT_ANOMALY:     return "DETECT_ANOMALY";
    case IntentType::LIST_PRODUCTIONS:   return "LIST_PRODUCTIONS";
    case IntentType::ANALYZE_PRODUCTION: return "ANALYZE_PRODUCTION";
    case IntentType::OPTIMIZE:           return "OPTIMIZE";
    case IntentType::HELP:               return "HELP";
    default:                             return "UNKNOWN";
    }
}
