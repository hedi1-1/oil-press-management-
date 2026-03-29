#ifndef QUALITYAIAGENT_H
#define QUALITYAIAGENT_H

#include <QString>

struct QualityEvaluationInput
{
    double acidityPercent = 0.0;
    double rendementPercent = 0.0;
    int olivesKg = 0;
    QString selectedQuality;
    bool userConformity = false;
    QString operatorNotes;
};

struct QualityEvaluationResult
{
    QString qualityLabel;
    QString riskLevel;
    QString smartRemarks;
    double score = 0.0;
    double confidence = 0.0;
    bool isConforme = false;
};

class QualityAIAgent
{
public:
    static QualityEvaluationResult evaluate(const QualityEvaluationInput &input);
};

#endif // QUALITYAIAGENT_H
