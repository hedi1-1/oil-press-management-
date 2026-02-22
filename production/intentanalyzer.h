#ifndef INTENTANALYZER_H
#define INTENTANALYZER_H

#include <QObject>
#include <QString>
#include <QMap>

// ============================================================================
// IntentType — Recognised command intents
// ============================================================================
enum class IntentType {
    UNKNOWN,
    CREATE_PRODUCTION,
    START_PRODUCTION,
    STOP_PRODUCTION,
    GET_RENDEMENT,
    GET_STATUS,
    VALIDATE_QUALITY,
    GENERATE_REPORT,
    DETECT_ANOMALY,
    LIST_PRODUCTIONS,
    ANALYZE_PRODUCTION,
    OPTIMIZE,
    HELP
};

// ============================================================================
// IntentResult — Parsed intent with extracted parameters
// ============================================================================
struct IntentResult {
    IntentType  intent          = IntentType::UNKNOWN;
    int         idProduction    = -1;
    float       quantity        = -1;
    QString     typePressage;
    QString     rawText;
    double      confidence      = 0.0;   // 0.0 – 1.0
};

// ============================================================================
// IntentAnalyzer — Rule-based NLU for production commands
// ============================================================================
class IntentAnalyzer : public QObject
{
    Q_OBJECT

public:
    explicit IntentAnalyzer(QObject *parent = nullptr);
    ~IntentAnalyzer() = default;

    IntentResult analyzeText(const QString &text);

    static QString intentToString(IntentType intent);

private:
    IntentType detectIntent(const QString &text, double &confidence);
    int        extractProductionId(const QString &text);
    float      extractQuantity(const QString &text);
    QString    extractTypePressage(const QString &text);
};

#endif // INTENTANALYZER_H
