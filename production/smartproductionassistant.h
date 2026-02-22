#ifndef SMARTPRODUCTIONASSISTANT_H
#define SMARTPRODUCTIONASSISTANT_H

#include <QObject>
#include <QString>
#include <QStringList>
#include "productionservice.h"

// ============================================================================
// SmartProductionAssistant — Rule-Based AI Advisor
// Analyzes production data and provides intelligent suggestions.
// ============================================================================
class SmartProductionAssistant : public QObject
{
    Q_OBJECT

public:
    explicit SmartProductionAssistant(ProductionService *service, QObject *parent = nullptr);

    // ── Analysis ────────────────────────────────────────────────────────
    struct AnalysisResult {
        QString  summary;
        QString  niveau;          // "excellent" | "bon" | "moyen" | "faible" | "critique"
        QStringList suggestions;
        QStringList warnings;
        double   score;           // 0-100
    };

    AnalysisResult analyserProduction(int idProduction);
    AnalysisResult analyserProduction(const ProductionModel &prod);

    // ── Suggestions ─────────────────────────────────────────────────────
    QStringList suggererAmelioration(int idProduction);
    QStringList suggererAmelioration(const ProductionModel &prod);

    // ── Explanations ────────────────────────────────────────────────────
    QString expliquerRendementFaible(const ProductionModel &prod);

    // ── Optimization ────────────────────────────────────────────────────
    struct OptimisationPlan {
        QString  typePressageRecommande;
        int      dureeOptimale;
        double   rendementCible;
        QStringList actions;
    };

    OptimisationPlan proposerOptimisation(int idProduction);
    OptimisationPlan proposerOptimisation(const ProductionModel &prod);

    // ── Quick responses for voice assistant ─────────────────────────────
    QString getQuickAnalysis(int idProduction);
    QString getStatusBrief();

signals:
    void analysisComplete(int idProduction, const AnalysisResult &result);
    void suggestionReady(int idProduction, const QStringList &suggestions);

private:
    ProductionService *m_service;

    QString evaluerNiveau(double rendement);
    double  calculerScore(const ProductionModel &prod);
};

#endif // SMARTPRODUCTIONASSISTANT_H
