#ifndef ASSISTANT_H
#define ASSISTANT_H

#include <QObject>
#include <QAudioFormat>
#include <QByteArray>
#include <QJsonObject>
#include <QPointer>

class QAudioSource;
class QIODevice;
class QTimer;
class QDialog;
class QNetworkAccessManager;
class QPushButton;
class QTextEdit;
class QTextToSpeech;
class QWidget;

class Assistant : public QObject {
    Q_OBJECT

public:
    explicit Assistant(QWidget *parentWindow = nullptr);
    ~Assistant() override;

    void setTalkButton(QPushButton *button);

signals:
    void requestNavigation(int index);

public slots:
    void startVoiceCycle();

private slots:
    void onAutoStopTimeout();   // arret apres 5 s
    void onPollAudio();         // polling PCM toutes les 50 ms

private:
    void setupHistoryUi();
    void appendHistoryLine(const QString &line, const QString &colorHex = "#D7E6E0");
    void updateButtonVisual();

    bool    startRecording();
    QString stopRecordingAndProcess();
    QString writeTempWavFile(const QByteArray &pcmData) const;
    QString transcribeWithWhisper(const QString &wavPath);

    QString askGroqAndExecuteIntent(const QString &userText);
    bool    executeQtIntent(const QJsonObject &intent, QString *resultMessage);

    void    speakText(const QString &text);

    QString resolveWhisperCliPath();
    QString resolveWhisperModelPath();
    QString resolveGroqApiKey();

    // UI
    QWidget               *m_parentWindow;
    QPointer<QPushButton>  m_talkButton;
    QPointer<QDialog>      m_historyDialog;
    QPointer<QTextEdit>    m_historyEdit;

    // Audio
    QAudioFormat  m_audioFormat;
    QAudioSource *m_audioSource = nullptr;
    QIODevice    *m_audioDevice = nullptr;   // retourne par QAudioSource::start()
    QByteArray    m_pcmBuffer;               // accumulateur PCM

    // Timers
    QTimer *m_pollTimer = nullptr;   // lit le PCM toutes les 50 ms
    QTimer *m_stopTimer = nullptr;   // arret auto apres recordDurationMs

    // Services
    QNetworkAccessManager *m_networkManager = nullptr;
    QTextToSpeech         *m_tts            = nullptr;

    // Etat
    bool m_isRecording      = false;
    int  m_recordDurationMs = 5000;
};

#endif // ASSISTANT_H