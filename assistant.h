#ifndef ASSISTANT_H
#define ASSISTANT_H

#include <QObject>

#include <QAudioFormat>
#include <QByteArray>
#include <QJsonObject>
#include <QPointer>

class QAudioSource;
class QBuffer;
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

public slots:
  void startVoiceCycle();

private slots:
  void onAutoStopTimeout();

private:
  void setupHistoryUi();
  void appendHistoryLine(const QString &line, const QString &colorHex = "#D7E6E0");
  void updateButtonVisual();

  bool startRecording();
  QString stopRecordingAndProcess();
  QString writeTempWavFile(const QByteArray &pcmData) const;
  QString transcribeWithWhisper(const QString &wavPath);
  QString askGroqAndExecuteIntent(const QString &userText);
  QString buildLocalFallbackReply(const QString &userText) const;
  void speakText(const QString &text);
  bool executeQtIntent(const QJsonObject &intent, QString *resultMessage);
  QString resolveWhisperCliPath();
  QString resolveWhisperModelPath();
  QString resolveGroqApiKey();

  QWidget *m_parentWindow;
  QPointer<QPushButton> m_talkButton;
  QPointer<QDialog> m_historyDialog;
  QPointer<QTextEdit> m_historyEdit;

  QAudioFormat m_audioFormat;
  QAudioSource *m_audioSource;
  QBuffer *m_captureBuffer;
  QByteArray m_pcmBuffer;

  QNetworkAccessManager *m_networkManager;
  QTextToSpeech *m_tts;
  mutable QString m_lastTranscriptionError;
  QString m_lastApiError;

  bool m_isRecording;
  int m_recordDurationMs;
};

#endif // ASSISTANT_H
