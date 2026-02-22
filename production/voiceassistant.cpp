#include "voiceassistant.h"
#include <QDebug>

// ============================================================================
// Constructor / Destructor
// ============================================================================
VoiceAssistant::VoiceAssistant(ProductionService *service,
                               SmartProductionAssistant *assistant,
                               QObject *parent)
    : QObject(parent)
    , m_stt(new SpeechToTextEngine(this))
    , m_intentAnalyzer(new IntentAnalyzer(this))
    , m_service(service)
    , m_assistant(assistant)
    , m_tts(new QTextToSpeech(this))
    , m_voiceEnabled(true)
{
    connect(m_stt, &SpeechToTextEngine::textRecognized,
            this, &VoiceAssistant::onTextRecognized);

    connect(m_stt, &SpeechToTextEngine::listeningStarted, this, [this]() {
        emit listeningStateChanged(true);
    });
    connect(m_stt, &SpeechToTextEngine::listeningStopped, this, [this]() {
        emit listeningStateChanged(false);
    });
}

VoiceAssistant::~VoiceAssistant()
{
}

// ============================================================================
// Listening
// ============================================================================
void VoiceAssistant::startListening()
{
    m_stt->startListening();
}

void VoiceAssistant::stopListening()
{
    m_stt->stopListening();
}

bool VoiceAssistant::isListening() const
{
    return m_stt->isListening();
}

// ============================================================================
// Manual text command
// ============================================================================
void VoiceAssistant::processTextCommand(const QString &text)
{
    onTextRecognized(text);
}

// ============================================================================
// TTS
// ============================================================================
void VoiceAssistant::speak(const QString &text)
{
    if (m_voiceEnabled && m_tts) {
        m_tts->say(text);
        emit speakingStateChanged(true);
    }
}

void VoiceAssistant::stopSpeaking()
{
    if (m_tts) {
        m_tts->stop();
        emit speakingStateChanged(false);
    }
}

bool VoiceAssistant::isSpeaking() const
{
    return m_tts && m_tts->state() == QTextToSpeech::Speaking;
}

void VoiceAssistant::setVoiceEnabled(bool enabled)
{
    m_voiceEnabled = enabled;
}

bool VoiceAssistant::isVoiceEnabled() const
{
    return m_voiceEnabled;
}

// ============================================================================
// Core — Text recognised → intent → action → response
// ============================================================================
void VoiceAssistant::onTextRecognized(const QString &text)
{
    qDebug() << "[VoiceAssistant] Processing:" << text;

    IntentResult intent = m_intentAnalyzer->analyzeText(text);
    emit intentDetected(IntentAnalyzer::intentToString(intent.intent), intent.confidence);

    QString response = handleIntent(intent);
    emit responseReady(response);

    if (m_voiceEnabled) {
        speak(response);
    }
}

// ============================================================================
// Intent Dispatcher
// ============================================================================
QString VoiceAssistant::handleIntent(const IntentResult &intent)
{
    switch (intent.intent) {
    case IntentType::CREATE_PRODUCTION:  return handleCreateProduction(intent);
    case IntentType::START_PRODUCTION:   return handleStartProduction(intent);
    case IntentType::STOP_PRODUCTION:    return handleStopProduction(intent);
    case IntentType::GET_RENDEMENT:      return handleGetRendement(intent);
    case IntentType::GET_STATUS:         return handleGetStatus(intent);
    case IntentType::VALIDATE_QUALITY:   return handleValidateQuality(intent);
    case IntentType::GENERATE_REPORT:    return handleGenerateReport(intent);
    case IntentType::DETECT_ANOMALY:     return handleDetectAnomaly(intent);
    case IntentType::LIST_PRODUCTIONS:   return handleListProductions();
    case IntentType::ANALYZE_PRODUCTION: return handleAnalyzeProduction(intent);
    case IntentType::OPTIMIZE:           return handleOptimize(intent);
    case IntentType::HELP:               return handleHelp();
    default:
        return "Désolé, je n'ai pas compris votre demande. "
               "Dites 'aide' pour voir les commandes disponibles.";
    }
}

// ============================================================================
// Intent Handlers
// ============================================================================
QString VoiceAssistant::handleCreateProduction(const IntentResult &intent)
{
    if (intent.quantity <= 0) {
        return "Pour créer une production, précisez la quantité d'olives. "
               "Par exemple : 'Créer une production de 500 kg en pression à froid'.";
    }

    ProductionModel prod;
    prod.setQuantiteOlivesKg(intent.quantity);
    prod.setTypePressage(intent.typePressage.isEmpty() ? "Pression à froid" : intent.typePressage);
    prod.setDateProduction(QDate::currentDate());
    prod.setStatut("Planifiée");

    bool ok = m_service->addProduction(prod);
    if (ok) {
        int duree = m_service->estimerDuree(intent.quantity, prod.getTypePressage());
        return QString("Production créée avec succès ! "
                       "%1 kg d'olives en %2. "
                       "Durée estimée : %3 minutes.")
            .arg(intent.quantity).arg(prod.getTypePressage()).arg(duree);
    }
    return "Erreur lors de la création de la production. Vérifiez la connexion à la base de données.";
}

QString VoiceAssistant::handleStartProduction(const IntentResult &intent)
{
    if (intent.idProduction <= 0) {
        return "Précisez le numéro de la production à démarrer. "
               "Par exemple : 'Démarrer la production numéro 5'.";
    }

    bool ok = m_service->demarrerProduction(intent.idProduction);
    if (ok)
        return QString("Production numéro %1 démarrée avec succès !").arg(intent.idProduction);
    return QString("Impossible de démarrer la production %1. Vérifiez qu'elle est bien planifiée.")
        .arg(intent.idProduction);
}

QString VoiceAssistant::handleStopProduction(const IntentResult &intent)
{
    if (intent.idProduction <= 0) {
        return "Précisez le numéro de la production à arrêter. "
               "Par exemple : 'Arrêter la production numéro 3'.";
    }

    bool ok = m_service->arreterProduction(intent.idProduction);
    if (ok)
        return QString("Production numéro %1 arrêtée.").arg(intent.idProduction);
    return QString("Impossible d'arrêter la production %1.").arg(intent.idProduction);
}

QString VoiceAssistant::handleGetRendement(const IntentResult &intent)
{
    if (intent.idProduction > 0) {
        double rend = m_service->calculerRendement(intent.idProduction);
        if (rend >= 0) {
            QString eval;
            if (rend >= 20) eval = "Excellent";
            else if (rend >= 15) eval = "Bon";
            else if (rend >= 12) eval = "Moyen";
            else eval = "Faible";

            return QString("Le rendement de la production %1 est de %2 pour cent. "
                           "Évaluation : %3.")
                .arg(intent.idProduction).arg(rend, 0, 'f', 1).arg(eval);
        }
        return QString("Impossible de calculer le rendement pour la production %1.")
            .arg(intent.idProduction);
    }

    // Global average
    double avg = m_service->getRendementMoyen();
    return QString("Le rendement moyen global est de %.1f pour cent.").arg(avg);
}

QString VoiceAssistant::handleGetStatus(const IntentResult &intent)
{
    if (intent.idProduction > 0) {
        ProductionModel prod = m_service->getProductionById(intent.idProduction);
        if (prod.getIdProduction() > 0) {
            return QString("Production %1 : statut %2, %3 kg d'olives, "
                           "%4 litres d'huile produite, rendement %5 pour cent.")
                .arg(prod.getIdProduction())
                .arg(prod.getStatut())
                .arg(prod.getQuantiteOlivesKg())
                .arg(prod.getHuileProduiteL(), 0, 'f', 1)
                .arg(prod.getRendement(), 0, 'f', 1);
        }
        return QString("Production %1 introuvable.").arg(intent.idProduction);
    }

    int total = m_service->getProductionCount();
    double totalHuile = m_service->getTotalHuileProduced();
    return QString("Il y a %1 productions au total, avec %2 litres d'huile produite.")
        .arg(total).arg(totalHuile, 0, 'f', 1);
}

QString VoiceAssistant::handleValidateQuality(const IntentResult &intent)
{
    if (intent.idProduction <= 0) {
        return "Précisez le numéro de la production à valider. "
               "Par exemple : 'Valider la qualité de la production 7'.";
    }

    ProductionModel prod = m_service->getProductionById(intent.idProduction);
    if (prod.getIdProduction() <= 0)
        return QString("Production %1 introuvable.").arg(intent.idProduction);

    bool conforme = m_service->isConforme(prod.getRendement(), prod.getQualite());
    if (conforme) {
        return QString("La production %1 est CONFORME aux normes de qualité. "
                       "Rendement suffisant et qualité excellente.").arg(intent.idProduction);
    }
    return QString("La production %1 n'est PAS conforme aux normes. "
                   "Vérification manuelle recommandée.").arg(intent.idProduction);
}

QString VoiceAssistant::handleGenerateReport(const IntentResult &intent)
{
    Q_UNUSED(intent);
    ProductionService::RapportProduction rapport = m_service->genererRapportProduction();
    return QString("Rapport généré. Productions totales : %1, "
                   "Huile totale : %2 L, Rendement moyen : %3%%.")
        .arg(rapport.totalProductions)
        .arg(rapport.totalHuileL, 0, 'f', 1)
        .arg(rapport.rendementMoyen, 0, 'f', 1);
}

QString VoiceAssistant::handleDetectAnomaly(const IntentResult &intent)
{
    if (intent.idProduction > 0) {
        ProductionService::AnomalieInfo anomalie = m_service->detecterAnomalie(intent.idProduction);
        if (!anomalie.hasAnomalie) {
            return QString("Aucune anomalie détectée pour la production %1. Tout semble normal.")
                .arg(intent.idProduction);
        }

        return QString("Anomalie détectée pour la production %1 : %2 (niveau %3).")
            .arg(intent.idProduction)
            .arg(anomalie.description, anomalie.severity);
    }

    return "Précisez le numéro de la production à vérifier. "
           "Par exemple : 'Détecter les anomalies de la production 3'.";
}

QString VoiceAssistant::handleListProductions()
{
    int count = m_service->getProductionCount();
    if (count == 0)
        return "Aucune production enregistrée dans le système.";

    return QString("Il y a actuellement %1 production(s) enregistrée(s). "
                   "Rendement moyen : %2 pour cent. "
                   "Huile totale produite : %3 litres.")
        .arg(count)
        .arg(m_service->getRendementMoyen(), 0, 'f', 1)
        .arg(m_service->getTotalHuileProduced(), 0, 'f', 1);
}

QString VoiceAssistant::handleAnalyzeProduction(const IntentResult &intent)
{
    if (intent.idProduction <= 0) {
        return "Précisez le numéro de la production à analyser. "
               "Par exemple : 'Analyser la production numéro 2'.";
    }

    ProductionModel prod = m_service->getProductionById(intent.idProduction);
    if (prod.getIdProduction() <= 0) {
        return QString("Production %1 introuvable.").arg(intent.idProduction);
    }

    SmartProductionAssistant::AnalysisResult analysis = m_assistant->analyserProduction(prod);
    return QString("Analyse de la production %1 : %2. Score : %3 sur 100. %4")
        .arg(intent.idProduction)
        .arg(analysis.summary)
        .arg(analysis.score)
        .arg(analysis.suggestions.isEmpty() ? "" :
                 "Suggestion : " + analysis.suggestions.first());
}

QString VoiceAssistant::handleOptimize(const IntentResult &intent)
{
    if (intent.idProduction > 0) {
        ProductionModel prod = m_service->getProductionById(intent.idProduction);
        if (prod.getIdProduction() <= 0)
            return QString("Production %1 introuvable.").arg(intent.idProduction);

        SmartProductionAssistant::OptimisationPlan plan = m_assistant->proposerOptimisation(prod);
        return QString("Plan d'optimisation pour production %1 : "
                       "Type recommandé : %2. Durée optimale : %3 minutes. "
                       "Rendement cible : %4 pour cent. "
                       "%5")
            .arg(intent.idProduction)
            .arg(plan.typePressageRecommande)
            .arg(plan.dureeOptimale)
            .arg(plan.rendementCible, 0, 'f', 1)
            .arg(plan.actions.isEmpty() ? "" : "Première action : " + plan.actions.first());
    }

    double avg = m_service->getRendementMoyen();
    if (avg < 15)
        return QString("Le rendement moyen est de %1 pour cent, ce qui est en-dessous de l'objectif. "
                       "Je recommande de passer en centrifugation 2 phases et de vérifier la qualité des olives.")
            .arg(avg, 0, 'f', 1);

    return QString("Le rendement moyen est de %1 pour cent. "
                   "Les performances sont satisfaisantes. Continuez ainsi !").arg(avg, 0, 'f', 1);
}

QString VoiceAssistant::handleHelp()
{
    return "Voici les commandes disponibles : "
           "Créer une production, Démarrer ou Arrêter une production, "
           "Obtenir le rendement, Voir le statut, Valider la qualité, "
           "Générer un rapport, Détecter les anomalies, "
           "Lister les productions, Analyser une production, "
           "et Optimiser. Précisez toujours le numéro de la production quand c'est nécessaire.";
}
