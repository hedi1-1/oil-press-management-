#include "assistant.h"

#include <QAudioDevice>
#include <QAudioFormat>
#include <QAudioSource>
#include <QBuffer>
#include <QCoreApplication>
#include <QDateTime>
#include <QDialog>
#include <QDir>
#include <QEventLoop>
#include <QFile>
#include <QHBoxLayout>
#include <QIODevice>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QMessageBox>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QProcess>
#include <QPushButton>
#include <QSettings>
#include <QTabWidget>
#include <QTableView>
#include <QTextEdit>
#include <QTextToSpeech>
#include <QTimer>
#include <QUrl>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QtEndian>
#include <QMediaDevices>
#include <QFileInfo>
#include <QRegularExpression>
#include <QStandardPaths>
#include <vector>
#include <QInputDialog>
#include <QMetaObject>
#include <QAbstractItemModel>

#ifdef MACHINE_ENABLE_WHISPER
#include <whisper.h>
#endif

namespace {
static constexpr int kSampleRate = 16000;
static constexpr int kChannels = 1;
static constexpr int kBitsPerSample = 16;

QByteArray buildWavFile(const QByteArray &pcmData) {
  QByteArray wav;
  wav.reserve(44 + pcmData.size());

  const quint32 dataSize = static_cast<quint32>(pcmData.size());
  const quint32 riffChunkSize = 36 + dataSize;
  const quint16 audioFormatPcm = 1;
  const quint16 channels = kChannels;
  const quint32 sampleRate = kSampleRate;
  const quint16 bitsPerSample = kBitsPerSample;
  const quint16 blockAlign = static_cast<quint16>(channels * bitsPerSample / 8);
  const quint32 byteRate = sampleRate * blockAlign;

  wav.append("RIFF", 4);
  {
    QByteArray chunk(4, '\0');
    qToLittleEndian(riffChunkSize, reinterpret_cast<uchar *>(chunk.data()));
    wav.append(chunk);
  }
  wav.append("WAVE", 4);

  wav.append("fmt ", 4);
  {
    QByteArray fmtChunkSize(4, '\0');
    qToLittleEndian(static_cast<quint32>(16), reinterpret_cast<uchar *>(fmtChunkSize.data()));
    wav.append(fmtChunkSize);
  }
  {
    QByteArray af(2, '\0');
    qToLittleEndian(audioFormatPcm, reinterpret_cast<uchar *>(af.data()));
    wav.append(af);
  }
  {
    QByteArray ch(2, '\0');
    qToLittleEndian(channels, reinterpret_cast<uchar *>(ch.data()));
    wav.append(ch);
  }
  {
    QByteArray sr(4, '\0');
    qToLittleEndian(sampleRate, reinterpret_cast<uchar *>(sr.data()));
    wav.append(sr);
  }
  {
    QByteArray br(4, '\0');
    qToLittleEndian(byteRate, reinterpret_cast<uchar *>(br.data()));
    wav.append(br);
  }
  {
    QByteArray ba(2, '\0');
    qToLittleEndian(blockAlign, reinterpret_cast<uchar *>(ba.data()));
    wav.append(ba);
  }
  {
    QByteArray bps(2, '\0');
    qToLittleEndian(bitsPerSample, reinterpret_cast<uchar *>(bps.data()));
    wav.append(bps);
  }

  wav.append("data", 4);
  {
    QByteArray ds(4, '\0');
    qToLittleEndian(dataSize, reinterpret_cast<uchar *>(ds.data()));
    wav.append(ds);
  }

  wav.append(pcmData);
  return wav;
}

QString extractJsonObjectText(const QString &rawText) {
  const QString trimmed = rawText.trimmed();
  if (trimmed.startsWith('{') && trimmed.endsWith('}')) {
    return trimmed;
  }

  QRegularExpression re("\\{[\\s\\S]*\\}");
  QRegularExpressionMatch match = re.match(trimmed);
  if (match.hasMatch()) {
    return match.captured(0).trimmed();
  }
  return QString();
}

QString extractOpenAICompatError(const QJsonObject &root) {
  const QJsonObject errorObj = root.value("error").toObject();
  if (!errorObj.isEmpty()) {
    const QString type = errorObj.value("type").toString().trimmed();
    const QString message = errorObj.value("message").toString().trimmed();
    if (!message.isEmpty()) {
      return type.isEmpty() ? message : QString("%1: %2").arg(type, message);
    }
  }
  const QString message = root.value("message").toString().trimmed();
  if (!message.isEmpty()) {
    return message;
  }
  return QString();
}

QString cleanWhisperOutput(QString text) {
  text.replace("\r", "\n");
  text.replace(QRegularExpression("\\n+"), "\n");
  text = text.trimmed();
  if (text.isEmpty()) {
    return text;
  }

  QStringList lines = text.split('\n', Qt::SkipEmptyParts);
  for (QString &line : lines) {
    line.replace(QRegularExpression("^\\s*\\[[^\\]]+\\]\\s*"), "");
    line = line.trimmed();
  }
  lines.removeAll(QString());
  return lines.join(' ').simplified();
}
} // namespace

Assistant::Assistant(QWidget *parentWindow)
    : QObject(parentWindow), m_parentWindow(parentWindow), m_audioSource(nullptr),
      m_captureBuffer(nullptr), m_networkManager(new QNetworkAccessManager(this)),
      m_tts(new QTextToSpeech(this)), m_isRecording(false), m_recordDurationMs(5000) {
  m_audioFormat.setSampleRate(kSampleRate);
  m_audioFormat.setChannelCount(kChannels);
  m_audioFormat.setSampleFormat(QAudioFormat::Int16);

  setupHistoryUi();
  updateButtonVisual();
}

Assistant::~Assistant() {
  if (m_audioSource) {
    m_audioSource->stop();
  }
  if (m_captureBuffer) {
    m_captureBuffer->close();
  }
}

void Assistant::setTalkButton(QPushButton *button) {
  m_talkButton = button;
  updateButtonVisual();
}

void Assistant::setupHistoryUi() {
  QDialog *dialog = new QDialog(m_parentWindow);
  dialog->setWindowTitle("Assistant vocal IA");
  dialog->resize(620, 420);

  QVBoxLayout *layout = new QVBoxLayout(dialog);
  QLabel *title = new QLabel("Historique conversation (Vous / Assistant)", dialog);
  title->setStyleSheet("font-weight: 700; color: #1A3C2F;");

  QTextEdit *history = new QTextEdit(dialog);
  history->setReadOnly(true);
  history->setStyleSheet(
      "QTextEdit { background-color: #0F1B20; color: #D7E6E0; border: 1px solid #30554A; "
      "border-radius: 8px; padding: 8px; font-size: 12px; }");

  layout->addWidget(title);
  layout->addWidget(history);

  m_historyDialog = dialog;
  m_historyEdit = history;
}

void Assistant::appendHistoryLine(const QString &line, const QString &colorHex) {
  if (!m_historyEdit) {
    return;
  }

  const QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss");
  const QString escapedLine = line.toHtmlEscaped();
  const QString html = QString("<p style='margin:4px 0;color:%1;'><b>[%2]</b> %3</p>")
                           .arg(colorHex, timestamp, escapedLine);
  m_historyEdit->append(html);
}

void Assistant::updateButtonVisual() {
  if (!m_talkButton) {
    return;
  }

  if (m_isRecording) {
    m_talkButton->setText("■ Stop");
    m_talkButton->setToolTip("Arrêter l'enregistrement vocal");
    m_talkButton->setStyleSheet(
        "QPushButton { background-color: #A32121; color: #FDECEC; border: 1px solid #DB6A6A; "
        "border-radius: 8px; font-size: 14px; font-weight: 700; }");
  } else {
    m_talkButton->setText("🗣 Parler");
    m_talkButton->setToolTip("Assistant vocal IA");
    m_talkButton->setStyleSheet(
        "QPushButton { background-color: rgba(255, 255, 255, 0.12); color: #C9A227; font-weight: 700; "
        "border: 1px solid rgba(255, 255, 255, 0.25); border-radius: 8px; font-size: 14px; } "
        "QPushButton:hover { background-color: rgba(255, 255, 255, 0.20); }");
  }
}

bool Assistant::startRecording() {
  if (m_isRecording) {
    return true;
  }

  const QAudioDevice input = QMediaDevices::defaultAudioInput();
  if (input.isNull()) {
    appendHistoryLine("Erreur: aucun micro disponible.", "#FF9AA2");
    QMessageBox::warning(m_parentWindow, "Assistant vocal",
                         "Aucun micro détecté sur cette machine.");
    return false;
  }

  if (m_audioSource) {
    m_audioSource->stop();
    m_audioSource->deleteLater();
    m_audioSource = nullptr;
  }

  if (m_captureBuffer) {
    m_captureBuffer->close();
    m_captureBuffer->deleteLater();
    m_captureBuffer = nullptr;
  }

  m_pcmBuffer.clear();
  m_captureBuffer = new QBuffer(&m_pcmBuffer, this);
  if (!m_captureBuffer->open(QIODevice::WriteOnly)) {
    appendHistoryLine("Erreur: impossible d'ouvrir le buffer audio.", "#FF9AA2");
    return false;
  }

  m_audioSource = new QAudioSource(input, m_audioFormat, this);
  m_audioSource->start(m_captureBuffer);

  m_isRecording = true;
  updateButtonVisual();

  if (m_historyDialog) {
    m_historyDialog->show();
    m_historyDialog->raise();
    m_historyDialog->activateWindow();
  }

  appendHistoryLine("Micro activé. Parlez maintenant...", "#A9E34B");

  QTimer::singleShot(m_recordDurationMs, this, &Assistant::onAutoStopTimeout);
  return true;
}

void Assistant::onAutoStopTimeout() {
  if (!m_isRecording) {
    return;
  }

  const QString processingError = stopRecordingAndProcess();
  if (!processingError.isEmpty()) {
    appendHistoryLine(processingError, "#FF9AA2");
  }
}

QString Assistant::stopRecordingAndProcess() {
  if (!m_isRecording) {
    return QString();
  }

  if (m_audioSource) {
    m_audioSource->stop();
  }
  if (m_captureBuffer) {
    m_captureBuffer->close();
  }

  m_isRecording = false;
  updateButtonVisual();

  if (m_pcmBuffer.isEmpty()) {
    return "Enregistrement vide. Réessayez en parlant plus près du micro.";
  }

  const QString wavPath = writeTempWavFile(m_pcmBuffer);
  if (wavPath.isEmpty()) {
    return "Impossible de créer le fichier WAV temporaire.";
  }

  appendHistoryLine("Transcription en cours via whisper.cpp...", "#9AD1D4");
  const QString userText = transcribeWithWhisper(wavPath).trimmed();
  if (userText.isEmpty()) {
    const QString detail = m_lastTranscriptionError.trimmed();
    if (!detail.isEmpty()) {
      return QString("La transcription est vide. Détail: %1").arg(detail);
    }
    return "La transcription est vide ou whisper.cpp n'est pas configuré.";
  }

  appendHistoryLine(QString("Vous: %1").arg(userText), "#F8D66D");
  appendHistoryLine("Envoi vers Groq...", "#9AD1D4");

  QString assistantText = askGroqAndExecuteIntent(userText).trimmed();
  if (assistantText.isEmpty()) {
    const QString detail = m_lastApiError.trimmed();
    if (!detail.isEmpty()) {
      appendHistoryLine(QString("Groq indisponible: %1").arg(detail), "#FF9AA2");
    } else {
      appendHistoryLine("Groq indisponible: erreur API.", "#FF9AA2");
    }

    assistantText = buildLocalFallbackReply(userText);
    appendHistoryLine("Bascule en mode local (hors ligne API Groq).", "#9AD1D4");
  }

  appendHistoryLine(QString("Assistant: %1").arg(assistantText), "#7EE0A4");
  speakText(assistantText);
  return QString();
}

QString Assistant::buildLocalFallbackReply(const QString &userText) const {
  const QString t = userText.trimmed();
  const QString lower = t.toLower();

  if (t.isEmpty()) {
    return "Je n'ai pas bien entendu. Pouvez-vous répéter clairement, avec le nom de la machine et son état ?";
  }

  if (lower.contains("panne") || lower.contains("erreur") || lower.contains("alerte")) {
    return QString("Reçu. J'ai détecté un état critique: '%1'. Je recommande: 1) vérifier température et charge, 2) passer en veille si nécessaire, 3) planifier une maintenance.")
        .arg(t);
  }

  if (lower.contains("id") || lower.contains("machine") || lower.contains("presse") ||
      lower.contains("broyeur") || lower.contains("malax") || lower.contains("chauffeur")) {
    return QString("Message machine reçu: '%1'. J'ai bien capté votre description. Vous pouvez préciser l'état (ON/OFF/VEILLE), la température et la criticité pour une recommandation plus précise.")
        .arg(t);
  }

  return QString("J'ai bien compris: '%1'. La transcription fonctionne correctement. L'API Groq est actuellement indisponible, donc je réponds en mode local.")
      .arg(t);
}

bool Assistant::executeQtIntent(const QJsonObject &intent, QString *resultMessage) {
  const QString action = intent.value("action").toString().trimmed().toLower();
  const QString param = intent.value("param").toString().trimmed();
  const QString machineId = intent.value("machine_id").toString().trimmed();

  if (action.isEmpty() || action == "none") {
    if (resultMessage) {
      *resultMessage = "Aucune action Qt déclenchée.";
    }
    return true;
  }

  if (action == "goto_tab") {
    QTabWidget *tabs = m_parentWindow ? m_parentWindow->findChild<QTabWidget *>("tabWidgetMachineManagement") : nullptr;
    if (!tabs) {
      if (resultMessage) {
        *resultMessage = "QTabWidget introuvable.";
      }
      return false;
    }

    const QString wanted = param.toLower();
    int index = -1;
    for (int i = 0; i < tabs->count(); ++i) {
      const QString tabText = tabs->tabText(i).toLower();
      if (tabText.contains(wanted) || wanted.contains(tabText)) {
        index = i;
        break;
      }
    }

    if (index < 0) {
      if (wanted.contains("parc")) index = 0;
      else if (wanted.contains("action")) index = 1;
      else if (wanted.contains("stat")) index = 2;
      else if (wanted.contains("carnet") || wanted.contains("ia")) index = 3;
    }

    if (index >= 0 && index < tabs->count()) {
      tabs->setCurrentIndex(index);
      if (resultMessage) {
        *resultMessage = QString("Navigation vers l'onglet: %1").arg(tabs->tabText(index));
      }
      return true;
    }

    if (resultMessage) {
      *resultMessage = QString("Onglet introuvable pour: %1").arg(param);
    }
    return false;
  }

  if (action == "show_machine_card") {
    QString id = machineId.isEmpty() ? param : machineId;
    if (id.isEmpty()) {
      QRegularExpression reId("(?:id\\s*[:#-]?\\s*)([A-Za-z0-9_-]+)",
                              QRegularExpression::CaseInsensitiveOption);
      const QRegularExpressionMatch m = reId.match(param);
      if (m.hasMatch()) {
        id = m.captured(1).trimmed();
      }
    }
    if (id.isEmpty()) {
      if (resultMessage) {
        *resultMessage = "ID machine manquant pour afficher la fiche carte.";
      }
      return false;
    }

    bool invoked = false;
    if (m_parentWindow) {
      invoked = QMetaObject::invokeMethod(
          m_parentWindow,
          "onMachineRequestedFromHttp",
          Qt::DirectConnection,
          Q_ARG(QString, id));
    }

    QPushButton *cardBtn = m_parentWindow ? m_parentWindow->findChild<QPushButton *>("btnCarteMachine") : nullptr;
    if (cardBtn) {
      cardBtn->click();
    }

    if (resultMessage) {
      *resultMessage = invoked
                           ? QString("Fiche carte machine affichée pour ID %1.").arg(id)
                           : QString("Action partielle: carte demandée, mais sélection machine incertaine (ID %1).")
                                 .arg(id);
    }
    return true;
  }

  if (resultMessage) {
    *resultMessage = QString("Action non supportée: %1").arg(action);
  }
  return false;
}

QString Assistant::writeTempWavFile(const QByteArray &pcmData) const {
  const QString tempPath = QDir(QDir::tempPath()).filePath(
      QString("machine_assistant_%1.wav")
          .arg(QDateTime::currentMSecsSinceEpoch()));

  QFile wavFile(tempPath);
  if (!wavFile.open(QIODevice::WriteOnly)) {
    return QString();
  }

  const QByteArray wav = buildWavFile(pcmData);
  const qint64 written = wavFile.write(wav);
  wavFile.close();

  if (written != wav.size()) {
    QFile::remove(tempPath);
    return QString();
  }

  return tempPath;
}

QString Assistant::transcribeWithWhisper(const QString &wavPath) {
  m_lastTranscriptionError.clear();

  auto transcribeWithCli = [this, &wavPath]() -> QString {
    const QString cliPath = resolveWhisperCliPath();
    const QString modelPath = resolveWhisperModelPath();
    if (cliPath.isEmpty()) {
      m_lastTranscriptionError =
          "Binaire whisper introuvable (WHISPER_CLI_PATH, whisper-cli.exe ou main.exe).";
      return QString();
    }
    if (modelPath.isEmpty()) {
      m_lastTranscriptionError =
          "Modèle whisper introuvable (WHISPER_MODEL_PATH ou dossier models/*.bin).";
      return QString();
    }

    const QString outputBase = QDir(QDir::tempPath()).filePath(
        QString("machine_whisper_%1").arg(QDateTime::currentMSecsSinceEpoch()));

    const QList<QStringList> argVariants = {
        QStringList{"-m", modelPath, "-f", wavPath, "-otxt", "-of", outputBase,
                    "-l", "fr", "-nt"},
        QStringList{"-m", modelPath, "-f", wavPath, "-otxt", "-of", outputBase,
                    "-l", "fr"},
        QStringList{"-m", modelPath, "-f", wavPath, "-l", "fr", "-nt"},
        QStringList{"-m", modelPath, "-f", wavPath, "-l", "fr"}};

    QString lastStderr;
    for (const QStringList &args : argVariants) {
      QProcess whisperProc;
      whisperProc.start(cliPath, args);
      if (!whisperProc.waitForFinished(120000)) {
        lastStderr = "Timeout whisper-cli (>120s).";
        continue;
      }

      const QString stdOut = QString::fromUtf8(whisperProc.readAllStandardOutput());
      const QString stdErr = QString::fromUtf8(whisperProc.readAllStandardError());
      if (!stdErr.trimmed().isEmpty()) {
        lastStderr = stdErr.trimmed();
      }

      if (whisperProc.exitStatus() != QProcess::NormalExit || whisperProc.exitCode() != 0) {
        continue;
      }

      QFile txtFile(outputBase + ".txt");
      if (txtFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        const QString fileText = cleanWhisperOutput(QString::fromUtf8(txtFile.readAll()));
        txtFile.close();
        if (!fileText.isEmpty()) {
          return fileText;
        }
      }

      const QString stdoutText = cleanWhisperOutput(stdOut);
      if (!stdoutText.isEmpty()) {
        return stdoutText;
      }
    }

    m_lastTranscriptionError =
        QString("Échec whisper-cli. Exécutable: %1 | Modèle: %2 | Détail: %3")
            .arg(cliPath, modelPath, lastStderr.isEmpty() ? "aucun message" : lastStderr);
    return QString();
  };

#ifdef MACHINE_ENABLE_WHISPER
  const QString modelPath = resolveWhisperModelPath();
  if (modelPath.trimmed().isEmpty()) {
    return transcribeWithCli();
  }

  QFile wavFile(wavPath);
  if (!wavFile.open(QIODevice::ReadOnly)) {
    return QString();
  }
  const QByteArray wavData = wavFile.readAll();
  wavFile.close();

  if (wavData.size() <= 44) {
    return QString();
  }

  const QByteArray pcmData = wavData.mid(44);
  const qint16 *samplesI16 = reinterpret_cast<const qint16 *>(pcmData.constData());
  const int sampleCount = pcmData.size() / static_cast<int>(sizeof(qint16));
  if (sampleCount <= 0) {
    return QString();
  }

  std::vector<float> samples;
  samples.reserve(sampleCount);
  for (int i = 0; i < sampleCount; ++i) {
    samples.push_back(static_cast<float>(samplesI16[i]) / 32768.0f);
  }

  whisper_context_params ctxParams = whisper_context_default_params();
  whisper_context *ctx = whisper_init_from_file_with_params(modelPath.toUtf8().constData(), ctxParams);
  if (!ctx) {
    m_lastTranscriptionError = "whisper_init_from_file_with_params a échoué, fallback CLI.";
    return transcribeWithCli();
  }

  whisper_full_params params = whisper_full_default_params(WHISPER_SAMPLING_GREEDY);
  params.print_realtime = false;
  params.print_progress = false;
  params.print_timestamps = false;
  params.print_special = false;
  params.translate = false;
  params.language = "fr";

  const int rc = whisper_full(ctx, params, samples.data(), static_cast<int>(samples.size()));
  if (rc != 0) {
    whisper_free(ctx);
    m_lastTranscriptionError = QString("whisper_full a échoué (code %1), fallback CLI.").arg(rc);
    return transcribeWithCli();
  }

  QString fullText;
  const int nSegments = whisper_full_n_segments(ctx);
  for (int i = 0; i < nSegments; ++i) {
    const char *seg = whisper_full_get_segment_text(ctx, i);
    if (seg) {
      fullText += QString::fromUtf8(seg);
    }
  }

  whisper_free(ctx);
  const QString trimmed = fullText.trimmed();
  return trimmed.isEmpty() ? transcribeWithCli() : trimmed;
#else
  return transcribeWithCli();
#endif
}

QString Assistant::resolveWhisperCliPath() {
  QSettings settings("MachineProject", "MachineApp");

  const QString envPath = qEnvironmentVariable("WHISPER_CLI_PATH").trimmed();
  if (!envPath.isEmpty() && QFileInfo::exists(envPath)) {
    settings.setValue("assistant/whisperCliPath", envPath);
    return envPath;
  }

  const QString settingsPath = settings.value("assistant/whisperCliPath").toString().trimmed();
  if (!settingsPath.isEmpty() && QFileInfo::exists(settingsPath)) {
    return settingsPath;
  }

  const QString inPathCli = QStandardPaths::findExecutable("whisper-cli");
  if (!inPathCli.isEmpty()) {
    return inPathCli;
  }
  const QString inPathMain = QStandardPaths::findExecutable("main");
  if (!inPathMain.isEmpty()) {
    return inPathMain;
  }

  const QString appDir = QCoreApplication::applicationDirPath();
  const QString cwd = QDir::currentPath();
  const QStringList candidates = {
      QDir(appDir).filePath("whisper-cli.exe"),
      QDir(appDir).filePath("main.exe"),
      QDir(cwd).filePath("whisper-cli.exe"),
      QDir(cwd).filePath("main.exe"),
      QDir(cwd).filePath("whisper.cpp/build/bin/Release/whisper-cli.exe"),
      QDir(cwd).filePath("whisper.cpp/build/bin/whisper-cli.exe")};

  for (const QString &path : candidates) {
    if (QFileInfo::exists(path)) {
      settings.setValue("assistant/whisperCliPath", path);
      return path;
    }
  }

  const QString chosen = QFileDialog::getOpenFileName(
      m_parentWindow,
      "Sélectionner whisper-cli.exe (ou main.exe)",
      QDir::homePath(),
      "Executables (*.exe);;Tous les fichiers (*.*)");
  if (!chosen.trimmed().isEmpty() && QFileInfo::exists(chosen)) {
    settings.setValue("assistant/whisperCliPath", chosen);
    appendHistoryLine(QString("Whisper CLI configuré: %1").arg(chosen), "#9AD1D4");
    return chosen;
  }

  return QString();
}

QString Assistant::resolveWhisperModelPath() {
  QSettings settings("MachineProject", "MachineApp");

  const QString envPath = qEnvironmentVariable("WHISPER_MODEL_PATH").trimmed();
  if (!envPath.isEmpty() && QFileInfo::exists(envPath)) {
    settings.setValue("assistant/whisperModelPath", envPath);
    return envPath;
  }

  const QString settingsPath = settings.value("assistant/whisperModelPath").toString().trimmed();
  if (!settingsPath.isEmpty() && QFileInfo::exists(settingsPath)) {
    return settingsPath;
  }

  const QString appDir = QCoreApplication::applicationDirPath();
  const QString cwd = QDir::currentPath();
  const QStringList directCandidates = {
      QDir(appDir).filePath("models/ggml-base.bin"),
      QDir(appDir).filePath("models/ggml-small.bin"),
      QDir(cwd).filePath("models/ggml-base.bin"),
      QDir(cwd).filePath("models/ggml-small.bin"),
      QDir(cwd).filePath("whisper.cpp/models/ggml-base.bin"),
      QDir(cwd).filePath("whisper.cpp/models/ggml-small.bin")};
  for (const QString &path : directCandidates) {
    if (QFileInfo::exists(path)) {
      settings.setValue("assistant/whisperModelPath", path);
      return path;
    }
  }

  const QStringList modelDirs = {
      QDir(appDir).filePath("models"),
      QDir(cwd).filePath("models"),
      QDir(cwd).filePath("whisper.cpp/models")};
  for (const QString &dirPath : modelDirs) {
    QDir modelDir(dirPath);
    if (!modelDir.exists()) {
      continue;
    }
    const QStringList bins = modelDir.entryList(QStringList() << "*.bin", QDir::Files, QDir::Name);
    if (!bins.isEmpty()) {
      const QString found = modelDir.filePath(bins.first());
      settings.setValue("assistant/whisperModelPath", found);
      return found;
    }
  }

  const QString chosen = QFileDialog::getOpenFileName(
      m_parentWindow,
      "Sélectionner le modèle Whisper (*.bin)",
      QDir::homePath(),
      "Whisper model (*.bin);;Tous les fichiers (*.*)");
  if (!chosen.trimmed().isEmpty() && QFileInfo::exists(chosen)) {
    settings.setValue("assistant/whisperModelPath", chosen);
    appendHistoryLine(QString("Modèle Whisper configuré: %1").arg(chosen), "#9AD1D4");
    return chosen;
  }

  return QString();
}

QString Assistant::resolveGroqApiKey() {
  QSettings settings("MachineProject", "MachineApp");

  const QString envKey = qEnvironmentVariable("GROQ_API_KEY").trimmed();
  if (!envKey.isEmpty()) {
    settings.setValue("assistant/groqApiKey", envKey);
    return envKey;
  }

  const QString settingsKey = settings.value("assistant/groqApiKey").toString().trimmed();
  if (!settingsKey.isEmpty()) {
    return settingsKey;
  }

  bool ok = false;
  const QString input = QInputDialog::getText(
      m_parentWindow,
      "Configuration Groq",
      "Collez votre clé API Groq (commence par gsk_...):",
      QLineEdit::Normal,
      QString(),
      &ok);
  if (!ok) {
    return QString();
  }

  const QString key = input.trimmed();
  if (!key.isEmpty()) {
    settings.setValue("assistant/groqApiKey", key);
    appendHistoryLine("Clé Groq configurée.", "#9AD1D4");
    return key;
  }

  return QString();
}

QString Assistant::askGroqAndExecuteIntent(const QString &userText) {
  m_lastApiError.clear();
  const QString apiKey = resolveGroqApiKey();
  if (apiKey.isEmpty()) {
    m_lastApiError = "Clé API Groq absente.";
    return QString();
  }

  const QString systemPrompt =
      "Tu es un interpreteur d'intentions pour une application Qt de gestion de machines. "
      "Tu dois repondre UNIQUEMENT en JSON sans texte additionnel. "
      "Schema JSON: {\"action\":\"goto_tab|show_machine_card|none\",\"param\":\"...\",\"machine_id\":\"...\",\"speech\":\"...\"}. "
      "Si l'utilisateur demande un onglet, action=goto_tab et param=nom onglet. "
      "Si l'utilisateur demande fiche/carte machine avec ID, action=show_machine_card et machine_id rempli. "
      "Toujours fournir speech en francais, court et clair.";

  const QStringList modelCandidates = {
      "llama3-8b-8192",
      "llama-3.1-8b-instant",
      "llama3-70b-8192",
      "llama-3.3-70b-versatile"};

  QStringList failures;
  for (const QString &modelName : modelCandidates) {
    QNetworkRequest request(QUrl("https://api.groq.com/openai/v1/chat/completions"));
    request.setRawHeader("Authorization", QString("Bearer %1").arg(apiKey).toUtf8());
    request.setRawHeader("Accept", "application/json");
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject root;
    root.insert("model", modelName);
    root.insert("temperature", 0.1);
    QJsonObject responseFormat;
    responseFormat.insert("type", "json_object");
    root.insert("response_format", responseFormat);

    QJsonArray messages;
    {
      QJsonObject m;
      m.insert("role", "system");
      m.insert("content", systemPrompt);
      messages.append(m);
    }
    {
      QJsonObject m;
      m.insert("role", "user");
      m.insert("content", userText);
      messages.append(m);
    }
    root.insert("messages", messages);

    QEventLoop loop;
    QNetworkReply *reply =
        m_networkManager->post(request, QJsonDocument(root).toJson(QJsonDocument::Compact));
    QTimer timeoutTimer;
    timeoutTimer.setSingleShot(true);
    QObject::connect(&timeoutTimer, &QTimer::timeout, &loop, &QEventLoop::quit);
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    timeoutTimer.start(30000);
    loop.exec();

    if (timeoutTimer.isActive()) {
      timeoutTimer.stop();
    } else if (reply->isRunning()) {
      reply->abort();
      reply->deleteLater();
      failures.append(QString("%1: timeout").arg(modelName));
      continue;
    }

    const QByteArray payload = reply->readAll();
    const QNetworkReply::NetworkError netError = reply->error();
    const QString netErrorString = reply->errorString();
    reply->deleteLater();

    QJsonParseError parseError;
    const QJsonDocument doc = QJsonDocument::fromJson(payload, &parseError);
    const QJsonObject rootObj = doc.isObject() ? doc.object() : QJsonObject();

    if (netError != QNetworkReply::NoError) {
      const QString apiMessage = extractOpenAICompatError(rootObj);
      failures.append(QString("%1: %2")
                          .arg(modelName,
                               apiMessage.isEmpty()
                                   ? QString("erreur réseau: %1").arg(netErrorString)
                                   : apiMessage));
      continue;
    }

    if (parseError.error != QJsonParseError::NoError || !doc.isObject()) {
      failures.append(QString("%1: JSON invalide (%2)")
                          .arg(modelName, parseError.errorString()));
      continue;
    }

    const QString apiError = extractOpenAICompatError(rootObj);
    if (!apiError.isEmpty()) {
      failures.append(QString("%1: %2").arg(modelName, apiError));
      continue;
    }

    const QJsonArray choices = rootObj.value("choices").toArray();
    if (choices.isEmpty()) {
      failures.append(QString("%1: aucune choice retournée").arg(modelName));
      continue;
    }

    const QString content = choices.at(0).toObject()
                                .value("message").toObject()
                                .value("content").toString();
    const QString intentJsonText = extractJsonObjectText(content);
    if (intentJsonText.isEmpty()) {
      failures.append(QString("%1: contenu non JSON").arg(modelName));
      continue;
    }

    QJsonParseError intentErr;
    const QJsonDocument intentDoc = QJsonDocument::fromJson(intentJsonText.toUtf8(), &intentErr);
    if (intentErr.error != QJsonParseError::NoError || !intentDoc.isObject()) {
      failures.append(QString("%1: JSON d'intention invalide (%2)")
                          .arg(modelName, intentErr.errorString()));
      continue;
    }

    const QJsonObject intent = intentDoc.object();
    QString actionResult;
    executeQtIntent(intent, &actionResult);

    QString speech = intent.value("speech").toString().trimmed();
    if (speech.isEmpty()) {
      speech = actionResult.isEmpty() ? QString("Commande reçue.") : actionResult;
    } else if (!actionResult.isEmpty()) {
      speech += " " + actionResult;
    }

    appendHistoryLine(QString("Modèle Groq utilisé: %1").arg(modelName), "#9AD1D4");
    appendHistoryLine(QString("Intent JSON: %1").arg(intentJsonText), "#9AD1D4");
    return speech;
  }

  m_lastApiError = failures.isEmpty()
                       ? QString("Aucun modèle Groq compatible n'a répondu.")
                       : QString("Aucun modèle Groq compatible. Détails: %1")
                             .arg(failures.join(" | "));
  return QString();
}

void Assistant::speakText(const QString &text) {
  if (!m_tts) {
    return;
  }

  if (m_tts->state() == QTextToSpeech::Speaking) {
    m_tts->stop();
  }
  m_tts->say(text);
}

void Assistant::startVoiceCycle() {
  if (m_isRecording) {
    const QString processingError = stopRecordingAndProcess();
    if (!processingError.isEmpty()) {
      appendHistoryLine(processingError, "#FF9AA2");
    }
    return;
  }

  startRecording();
}
