#ifndef VOICEASSISTANT_H
#define VOICEASSISTANT_H

#include <QObject>
#include <QString>
#include <QTextToSpeech>
#include "speechtotextengine.h"
#include "intentanalyzer.h"
#include "productionservice.h"
#include "smartproductionassistant.h"

// ============================================================================
// VoiceAssistant — Full voice-driven AI assistant for production
// Combines SpeechToText → IntentAnalyzer → ProductionService → TextToSpeech
// ============================================================================
class VoiceAssistant : public QObject
{
    Q_OBJECT

public:
    explicit VoiceAssistant(ProductionService *service,
                            SmartProductionAssistant *assistant,
                            QObject *parent = nullptr);
    ~VoiceAssistant();

    // Voice control
    void startListening();
    void stopListening();
    bool isListening() const;

    // Manual text input (bypass speech recognition)
    void processTextCommand(const QString &text);

    // TTS control
    void speak(const QString &text);
    void stopSpeaking();
    bool isSpeaking() const;

    // Settings
    void setVoiceEnabled(bool enabled);
    bool isVoiceEnabled() const;

signals:
    void responseReady(const QString &response);
    void intentDetected(const QString &intentName, double confidence);
    void listeningStateChanged(bool listening);
    void speakingStateChanged(bool speaking);
    void errorOccurred(const QString &error);

private slots:
    void onTextRecognized(const QString &text);

private:
    SpeechToTextEngine       *m_stt;
    IntentAnalyzer           *m_intentAnalyzer;
    ProductionService        *m_service;
    SmartProductionAssistant *m_assistant;
    QTextToSpeech            *m_tts;
    bool                      m_voiceEnabled;

    QString handleIntent(const IntentResult &intent);
    QString handleCreateProduction(const IntentResult &intent);
    QString handleStartProduction(const IntentResult &intent);
    QString handleStopProduction(const IntentResult &intent);
    QString handleGetRendement(const IntentResult &intent);
    QString handleGetStatus(const IntentResult &intent);
    QString handleValidateQuality(const IntentResult &intent);
    QString handleGenerateReport(const IntentResult &intent);
    QString handleDetectAnomaly(const IntentResult &intent);
    QString handleListProductions();
    QString handleAnalyzeProduction(const IntentResult &intent);
    QString handleOptimize(const IntentResult &intent);
    QString handleHelp();
};

#endif // VOICEASSISTANT_H
