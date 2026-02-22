#include "speechtotextengine.h"
#include <QDebug>

SpeechToTextEngine::SpeechToTextEngine(QObject *parent)
    : QObject(parent)
    , m_listening(false)
{
}

void SpeechToTextEngine::startListening()
{
    if (m_listening) return;
    m_listening = true;
    qDebug() << "[SpeechToTextEngine] Listening started (simulated)";
    emit listeningStarted();
}

void SpeechToTextEngine::stopListening()
{
    if (!m_listening) return;
    m_listening = false;
    qDebug() << "[SpeechToTextEngine] Listening stopped";
    emit listeningStopped();
}

bool SpeechToTextEngine::isListening() const
{
    return m_listening;
}

void SpeechToTextEngine::feedText(const QString &text)
{
    if (text.trimmed().isEmpty()) {
        emit errorOccurred("Texte vide reçu.");
        return;
    }
    qDebug() << "[SpeechToTextEngine] Text received:" << text;
    emit textRecognized(text.trimmed());
}
