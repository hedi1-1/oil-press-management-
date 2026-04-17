#include "assistant.h"

#include <QAudioDevice>
#include <QAudioFormat>
#include <QAudioSource>
#include <QCoreApplication>
#include <QDir>
#include <QDialog>
#include <QEventLoop>
#include <QFile>
#include <QFileInfo>
#include <QIODevice>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMediaDevices>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QProcess>
#include <QPushButton>
#include <QRegularExpression>
#include <QSettings>
#include <QTextEdit>
#include <QTextToSpeech>
#include <QTimer>
#include <QUrl>
#include <QVBoxLayout>
#include <QtEndian>

namespace {
static constexpr int kSampleRate     = 16000;
static constexpr int kChannels       = 1;
static constexpr int kBitsPerSample  = 16;
static constexpr int kPollIntervalMs = 50;
static constexpr int kMinAudioBytes  = 3200;

QByteArray buildWavFile(const QByteArray &pcmData)
{
    QByteArray wav;
    wav.reserve(44 + pcmData.size());
    const quint32 dataSize      = static_cast<quint32>(pcmData.size());
    const quint32 riffChunkSize = 36 + dataSize;
    auto le16 = [&](quint16 v){ QByteArray b(2,'\0'); qToLittleEndian(v,reinterpret_cast<uchar*>(b.data())); wav.append(b); };
    auto le32 = [&](quint32 v){ QByteArray b(4,'\0'); qToLittleEndian(v,reinterpret_cast<uchar*>(b.data())); wav.append(b); };
    wav.append("RIFF", 4);  le32(riffChunkSize);
    wav.append("WAVEfmt ", 8); le32(16);
    le16(1);
    le16(static_cast<quint16>(kChannels));
    le32(static_cast<quint32>(kSampleRate));
    le32(static_cast<quint32>(kSampleRate * kChannels * kBitsPerSample / 8));
    le16(static_cast<quint16>(kChannels * kBitsPerSample / 8));
    le16(static_cast<quint16>(kBitsPerSample));
    wav.append("data", 4); le32(dataSize);
    wav.append(pcmData);
    return wav;
}

QString cleanWhisperOutput(QString text)
{
    text.replace(QRegularExpression("\\[.*?\\]"), "");
    return text.simplified();
}
} // namespace

// ---- Constructeur ----
Assistant::Assistant(QWidget *parentWindow)
    : QObject(parentWindow), m_parentWindow(parentWindow)
{
    m_audioFormat.setSampleRate(kSampleRate);
    m_audioFormat.setChannelCount(kChannels);
    m_audioFormat.setSampleFormat(QAudioFormat::Int16);

    m_networkManager = new QNetworkAccessManager(this);
    m_tts            = new QTextToSpeech(this);

    // Timer de polling PCM toutes les 50 ms
    m_pollTimer = new QTimer(this);
    m_pollTimer->setInterval(kPollIntervalMs);
    m_pollTimer->setSingleShot(false);
    connect(m_pollTimer, &QTimer::timeout, this, &Assistant::onPollAudio);

    // Timer d'arret automatique
    m_stopTimer = new QTimer(this);
    m_stopTimer->setSingleShot(true);
    connect(m_stopTimer, &QTimer::timeout, this, &Assistant::onAutoStopTimeout);

    setupHistoryUi();
}

Assistant::~Assistant()
{
    if (m_audioSource) m_audioSource->stop();
}

// ---- UI ----
void Assistant::setTalkButton(QPushButton *button)
{
    m_talkButton = button;
    if (m_talkButton)
        connect(m_talkButton, &QPushButton::clicked, this, &Assistant::startVoiceCycle);
    updateButtonVisual();
}

void Assistant::setupHistoryUi()
{
    m_historyDialog = new QDialog(m_parentWindow);
    m_historyDialog->setWindowTitle("Assistant Vocal - Console");
    m_historyDialog->resize(500, 300);
    QVBoxLayout *layout = new QVBoxLayout(m_historyDialog);
    m_historyEdit = new QTextEdit(m_historyDialog);
    m_historyEdit->setReadOnly(true);
    m_historyEdit->setStyleSheet(
        "background-color: #121212; color: #00FF00; font-family: 'Consolas';")
    ;
    layout->addWidget(m_historyEdit);
}

void Assistant::appendHistoryLine(const QString &line, const QString &colorHex)
{
    if (!m_historyEdit) return;
    m_historyEdit->append(
        QString("<span style='color:%1;'>%2</span>").arg(colorHex, line));
}

void Assistant::updateButtonVisual()
{
    if (!m_talkButton) return;
    if (m_isRecording) {
        m_talkButton->setText("ECOUTE...");
        m_talkButton->setStyleSheet(
            "background-color: #A32121; color: white; border-radius: 8px; padding: 5px;");
    } else {
        m_talkButton->setText("PARLER");
        m_talkButton->setStyleSheet(
            "background-color: #2D5A27; color: #C9A227;"
            "border-radius: 8px; padding: 5px; font-weight: bold;");
    }
}

void Assistant::startVoiceCycle()
{
    if (m_isRecording)
        stopRecordingAndProcess();
    else
        startRecording();
}

// ─────────────────────────────────────────────────────────
// startRecording  -- strategie POLLING
//
// Pourquoi le buffer etait vide :
//  - IdleState -> ActiveState -> StoppedState en quelques ms :
//    le driver WASAPI Realtek envoie un seul chunk au demarrage
//    puis passe en IdleState si readyRead() n'est pas consomme
//    assez vite OU si le buffer interne deborde.
//
// Solution : on ne depend plus du signal readyRead() (unreliable
// sous WASAPI). On utilise un QTimer a 50 ms qui lit activement
// bytesReady() octets depuis le QIODevice. Cette approche "pull"
// vide le buffer interne en permanence et empeche le driver de
// couper la session audio.
// ─────────────────────────────────────────────────────────
bool Assistant::startRecording()
{
    if (m_historyDialog) { m_historyDialog->show(); m_historyDialog->raise(); }

    const QAudioDevice inputDevice = QMediaDevices::defaultAudioInput();
    if (inputDevice.isNull()) {
        appendHistoryLine("IA : Erreur - Aucun microphone trouve.", "#FF4040");
        return false;
    }

    // Verifier le format, mais continuer meme si non supporte nativement
    if (!inputDevice.isFormatSupported(m_audioFormat))
        appendHistoryLine("IA : Avertissement format 16kHz - Qt va adapter.", "#FFA500");

    appendHistoryLine(
        QString("IA : Micro selectionne : %1").arg(inputDevice.description()), "#AABBCC");

    // Nettoyage session precedente
    m_pollTimer->stop();
    m_stopTimer->stop();
    if (m_audioSource) {
        m_audioSource->stop();
        m_audioSource->deleteLater();
        m_audioSource = nullptr;
    }
    m_audioDevice = nullptr;
    m_pcmBuffer.clear();

    // Creation QAudioSource
    m_audioSource = new QAudioSource(inputDevice, m_audioFormat, this);

    // Buffer grand = le driver ne deborde pas entre deux polls a 50 ms
    // 16000 * 2 * 0.5s = 16000 octets (500 ms de tampon)
    m_audioSource->setBufferSize(16000);

    // Log des changements d'etat
    connect(m_audioSource, &QAudioSource::stateChanged,
            this, [this](QAudio::State state) {
        const QStringList names = {"ActiveState","SuspendedState","StoppedState","IdleState"};
        int idx = static_cast<int>(state);
        appendHistoryLine(
            QString("IA : [Audio] Etat -> %1").arg(
                (idx >= 0 && idx < names.size()) ? names[idx] : "Unknown"),
            "#555555");

        // WASAPI peut passer en IdleState si on ne lit pas assez vite.
        // Dans ce cas on ne fait rien : le pollTimer continue de lire.
        if (state == QAudio::StoppedState && m_audioSource) {
            auto err = m_audioSource->error();
            if (err != QAudio::NoError)
                appendHistoryLine(
                    QString("IA : Erreur audio (code %1)").arg(static_cast<int>(err)),
                    "#FF4040");
        }
    });

    // start() sans argument -> QIODevice* interne gere par Qt/WASAPI
    m_audioDevice = m_audioSource->start();
    if (!m_audioDevice) {
        appendHistoryLine("IA : Erreur - start() a retourne nullptr.", "#FF4040");
        return false;
    }

    // Workaround critique : WASAPI sous Windows peut s'arrêter s'il n'y a pas de signal readyRead.
    // Lier readyRead au read de Qt force le driver à rester actif, même si l'on poll avec le timer.
    connect(m_audioDevice, &QIODevice::readyRead, this, [this]() {
        if (!m_audioDevice || !m_audioSource) return;
        qint64 available = m_audioSource->bytesAvailable();
        if (available > 0) {
            QByteArray chunk = m_audioDevice->read(available);
            if (!chunk.isEmpty()) m_pcmBuffer.append(chunk);
        }
    });

    m_isRecording = true;
    updateButtonVisual();

    // Demarrer le polling PCM comme filet de sécurité
    m_pollTimer->start();
    // Demarrer le timer d'arret
    m_stopTimer->start(m_recordDurationMs);

    const QString promptMsg = "Je vous ecoute.";
    appendHistoryLine("IA : " + promptMsg, "#7EE0A4");
    speakText(promptMsg);
    return true;
}

// ─────────────────────────────────────────────────────────
// onPollAudio -- coeur du mecanisme PULL
// Appele toutes les 50 ms par m_pollTimer.
// Lit TOUT ce qui est disponible dans le buffer interne de WASAPI.
// ─────────────────────────────────────────────────────────
void Assistant::onPollAudio()
{
    if (!m_audioDevice || !m_audioSource) return;

    // bytesReady() donne le nombre d'octets disponibles dans le buffer driver
    qint64 available = m_audioSource->bytesAvailable();
    if (available <= 0) return;

    QByteArray chunk = m_audioDevice->read(available);
    if (!chunk.isEmpty())
        m_pcmBuffer.append(chunk);
}

void Assistant::onAutoStopTimeout()
{
    if (m_isRecording) stopRecordingAndProcess();
}

// ─────────────────────────────────────────────────────────
// stopRecordingAndProcess
// ─────────────────────────────────────────────────────────
QString Assistant::stopRecordingAndProcess()
{
    if (!m_isRecording || !m_audioSource) return "";

    // Stopper les timers en premier
    m_pollTimer->stop();
    m_stopTimer->stop();

    // Dernier poll avant fermeture
    if (m_audioDevice) {
        qint64 remaining = m_audioSource->bytesAvailable();
        if (remaining > 0) {
            QByteArray last = m_audioDevice->read(remaining);
            if (!last.isEmpty()) m_pcmBuffer.append(last);
        }
    }

    m_audioSource->stop();
    m_audioDevice = nullptr;
    m_isRecording = false;
    updateButtonVisual();

    appendHistoryLine(
        QString("IA : Capture terminee (%1 octets PCM)").arg(m_pcmBuffer.size()),
        "#888888");

    if (m_pcmBuffer.size() < kMinAudioBytes) {
        appendHistoryLine(
            QString("IA : Erreur - Son insuffisant (%1 octets < %2 min).")
                .arg(m_pcmBuffer.size()).arg(kMinAudioBytes),
            "#FF4040");
        return "Audio vide";
    }

    const QString wavPath = writeTempWavFile(m_pcmBuffer);
    if (wavPath.isEmpty()) {
        appendHistoryLine("IA : Erreur ecriture WAV.", "#FF4040");
        return "Erreur WAV";
    }

    appendHistoryLine("IA : Analyse vocale...", "#FFA500");
    QCoreApplication::processEvents();

    const QString userText = transcribeWithWhisper(wavPath);
    QFile::remove(wavPath);

    if (userText.isEmpty() || userText.startsWith("Erreur")) {
        const QString errMsg = "Je n'ai pas compris.";
        speakText(errMsg);
        appendHistoryLine("IA : " + errMsg, "#FF4040");
        return "Erreur transcription";
    }

    appendHistoryLine("Vous : " + userText, "#F8D66D");

    // Navigation locale (mots-cles rapides)
    const QString lower = userText.toLower();
    int tab = -1;
    if      (lower.contains("parc"))   tab = 0;
    else if (lower.contains("action")) tab = 1;
    else if (lower.contains("stat"))   tab = 2;
    else if (lower.contains("carnet")) tab = 3;

    if (tab != -1) {
        emit requestNavigation(tab);
        const QString reply = "D'accord, j'affiche l'onglet demande.";
        appendHistoryLine("IA : " + reply, "#7EE0A4");
        speakText(reply);
        return "";
    }

    const QString groqReply = askGroqAndExecuteIntent(userText);
    if (!groqReply.isEmpty()) {
        appendHistoryLine("IA : " + groqReply, "#7EE0A4");
        speakText(groqReply);
    }
    return "";
}

// ---- Transcription Whisper ----
QString Assistant::transcribeWithWhisper(const QString &wavPath)
{
    const QString cli   = resolveWhisperCliPath();
    const QString model = resolveWhisperModelPath();
    if (!QFileInfo::exists(cli))   { appendHistoryLine("IA : whisper-cli.exe introuvable.", "#FF4040"); return ""; }
    if (!QFileInfo::exists(model)) { appendHistoryLine("IA : modele Whisper introuvable.", "#FF4040");  return ""; }

    QProcess proc;
    proc.start(cli, {"-m", model, "-f", wavPath, "-l", "fr", "-nt"});
    if (!proc.waitForFinished(15000)) { appendHistoryLine("IA : Timeout Whisper.", "#FF4040"); return ""; }

    const QString err = proc.readAllStandardError().trimmed();
    if (!err.isEmpty()) appendHistoryLine("Whisper: " + err, "#888888");
    return cleanWhisperOutput(proc.readAllStandardOutput());
}

// ---- WAV ----
QString Assistant::writeTempWavFile(const QByteArray &pcmData) const
{
    const QString path = QDir::temp().filePath("input_ai.wav");
    QFile f(path);
    if (f.open(QIODevice::WriteOnly)) { f.write(buildWavFile(pcmData)); f.close(); return path; }
    return "";
}

// ---- Groq API ----
QString Assistant::askGroqAndExecuteIntent(const QString &userText)
{
    const QString apiKey = resolveGroqApiKey();
    if (apiKey.isEmpty() || apiKey == "VOTRE_CLE_ICI") return "Cle API Groq manquante.";

    const QString systemPrompt =
        "Tu es l'assistant de 'Gestion des Machines'. "
        "Reponds en JSON uniquement: "
        "{\"action\":\"goto_tab|none\","
        " \"param\":\"parc|actions|statistiques|carnet\","
        " \"speech\":\"ta reponse vocale\"}";

    QNetworkRequest request(QUrl("https://api.groq.com/openai/v1/chat/completions"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", "Bearer " + apiKey.toUtf8());

    QJsonObject root;
    root.insert("model", "llama3-8b-8192");
    QJsonArray messages;
    messages.append(QJsonObject{{("role"), ("system")}, {("content"), systemPrompt}});
    messages.append(QJsonObject{{("role"), ("user")},   {("content"), userText}});
    root.insert("messages", messages);
    root.insert("response_format", QJsonObject{{("type"), ("json_object")}});

    QEventLoop loop;
    QNetworkReply *reply = m_networkManager->post(request, QJsonDocument(root).toJson());
    connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    if (reply->error() != QNetworkReply::NoError) {
        appendHistoryLine("IA : Erreur Groq : " + reply->errorString(), "#FF4040");
        reply->deleteLater(); return "";
    }

    const QJsonObject resp = QJsonDocument::fromJson(reply->readAll()).object();
    reply->deleteLater();
    const QString content = resp["choices"].toArray()[0].toObject()["message"].toObject()["content"].toString();
    const QJsonObject intent = QJsonDocument::fromJson(content.toUtf8()).object();
    executeQtIntent(intent, nullptr);
    return intent.value("speech").toString();
}

bool Assistant::executeQtIntent(const QJsonObject &intent, QString *resultMessage)
{
    const QString action = intent.value("action").toString().toLower();
    const QString param  = intent.value("param").toString().toLower();
    if (action == "goto_tab") {
        int index = -1;
        if      (param.contains("parc"))   index = 0;
        else if (param.contains("action")) index = 1;
        else if (param.contains("stat"))   index = 2;
        else if (param.contains("carnet")) index = 3;
        if (index != -1) {
            emit requestNavigation(index);
            if (resultMessage) *resultMessage = "Navigation OK.";
            return true;
        }
    }
    return false;
}

// ---- TTS ----
void Assistant::speakText(const QString &text) { if (m_tts) m_tts->say(text); }

// ---- Chemins ----
QString Assistant::resolveWhisperCliPath()  { return QCoreApplication::applicationDirPath() + "/whisper-cli.exe"; }
QString Assistant::resolveWhisperModelPath(){ return QCoreApplication::applicationDirPath() + "/models/ggml-base.bin"; }
QString Assistant::resolveGroqApiKey()
{
    QSettings s("MachineProject", "MachineApp");
    return s.value("assistant/groqApiKey", "VOTRE_CLE_ICI").toString();
}