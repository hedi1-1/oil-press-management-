#ifndef SPEECHTOTEXTENGINE_H
#define SPEECHTOTEXTENGINE_H

#include <QObject>
#include <QString>

// ============================================================================
// SpeechToTextEngine — Simulated voice-to-text engine
// In a production environment this would connect to a real ASR back-end
// (Whisper, Google Speech, Azure Speech, etc.).  For now it provides a
// placeholder that can be fed text from the UI for demonstration.
// ============================================================================
class SpeechToTextEngine : public QObject
{
    Q_OBJECT

public:
    explicit SpeechToTextEngine(QObject *parent = nullptr);
    ~SpeechToTextEngine() = default;

    // Start / stop listening (simulated)
    void startListening();
    void stopListening();
    bool isListening() const;

    // Feed text manually (for demo / testing)
    void feedText(const QString &text);

signals:
    void textRecognized(const QString &text);
    void listeningStarted();
    void listeningStopped();
    void errorOccurred(const QString &errorMessage);

private:
    bool m_listening;
};

#endif // SPEECHTOTEXTENGINE_H
