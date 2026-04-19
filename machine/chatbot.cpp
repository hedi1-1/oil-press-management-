#include "chatbot.h"
#include "config.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QScrollArea>
#include <QScrollBar>
#include <QTimer>
#include <QPropertyAnimation>
#include <QGraphicsDropShadowEffect>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QNetworkRequest>
#include <QUrl>
#include <QFrame>

// ============================================================
//  Configuration GROQ API (100% gratuit)
//  Obtenir votre cle sur : console.groq.com
// ============================================================
static const QString API_KEY = GROQ_KEY;
static const QString API_URL = "https://api.groq.com/openai/v1/chat/completions";

static const QString SYSTEM_PROMPT =
    "Tu es l'assistant technique de l'application 'Gestion des Machines'.\n"
    "CONSIGNE STRICTE : Tu ne reponds QU AUX questions sur la maintenance industrielle et l application.\n"
    "1. Si la question est hors-sujet (meteo, sport, etc.), reponds EXACTEMENT : "
    "Desole, je ne peux repondre qu aux questions concernant la gestion technique des machines.\n"
    "2. Si la question est valide, fais une reponse TRES COURTE (maximum 2 phrases).\n"
    "3. REPONSES TYPES :\n"
    "- Bonjour -> Bonjour, comment puis-je vous aider ?\n"
    "- Qui es-tu ? -> Je suis votre assistant technique specialise dans la gestion de vos machines.\n"
    "- Merci -> De rien ! Je reste a votre disposition pour toute autre question.\n"
    "- Comment verifier l etat d une machine ? -> Consultez l indicateur colore dans le Parc Machines :"
    " Vert pour normal, Orange pour alerte, et Rouge pour une panne.\n"
    "- Que signifie le statut VEILLE ? -> La machine est sous tension mais ne produit pas.\n"
    "- Comment exporter les donnees ? -> Utilisez le bouton Exporter en haut a droite de la liste.";

// ============================================================
//  CONSTRUCTEUR
// ============================================================
Chatbot::Chatbot(QWidget *parent)
    : QWidget(parent),
      m_networkManager(new QNetworkAccessManager(this)),
    m_dotCount(0),
    m_isVisible(false)
{
    hide();
    setFixedWidth(400);
    setAttribute(Qt::WA_StyledBackground, true);
    setObjectName("ChatbotWidget");

    setStyleSheet(
        "QWidget#ChatbotWidget {"
        "   background-color: #050F0A;"
        "   border-left: 1px solid #00FF88;"
        "}"
    );

    // Ombre externe neon verte
    auto *shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(40);
    shadow->setOffset(-8, 0);
    shadow->setColor(QColor(0, 255, 136, 120));
    setGraphicsEffect(shadow);

    // Timer pour animation des points de chargement
    m_dotsTimer = new QTimer(this);
    connect(m_dotsTimer, &QTimer::timeout, this, &Chatbot::animateDots);

    setupUi();

    m_animation = new QPropertyAnimation(this, "geometry");
    m_animation->setDuration(400);
    m_animation->setEasingCurve(QEasingCurve::OutCubic);
}

// ============================================================
//  DESTRUCTEUR
// ============================================================
Chatbot::~Chatbot() {}

// ============================================================
//  SETUP UI — Construction de toute l'interface
// ============================================================
void Chatbot::setupUi()
{
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // ----------------------------------------------------------
    // HEADER
    // ----------------------------------------------------------
    auto *header = new QWidget(this);
    header->setFixedHeight(70);
    header->setObjectName("Header");
    header->setStyleSheet(
        "QWidget#Header {"
        "   background-color: #050F0A;"
        "   border-bottom: 1px solid #00FF88;"
        "}"
    );

    auto *headerGlow = new QGraphicsDropShadowEffect(header);
    headerGlow->setBlurRadius(20);
    headerGlow->setOffset(0, 4);
    headerGlow->setColor(QColor(0, 255, 136, 80));
    header->setGraphicsEffect(headerGlow);

    auto *headerLayout = new QHBoxLayout(header);
    headerLayout->setContentsMargins(18, 0, 12, 0);
    headerLayout->setSpacing(12);

    // Point de statut vert
    auto *statusDot = new QLabel(header);
    statusDot->setFixedSize(10, 10);
    statusDot->setStyleSheet(
        "QLabel {"
        "   background-color: #00FF88;"
        "   border-radius: 5px;"
        "}"
    );
    auto *dotGlow = new QGraphicsDropShadowEffect(statusDot);
    dotGlow->setBlurRadius(12);
    dotGlow->setOffset(0, 0);
    dotGlow->setColor(QColor(0, 255, 136, 200));
    statusDot->setGraphicsEffect(dotGlow);

    // Titre et sous-titre
    auto *titleBlock = new QVBoxLayout();
    titleBlock->setSpacing(2);

    auto *titleLabel = new QLabel("ASSISTANT TECHNIQUE", header);
    titleLabel->setStyleSheet(
        "QLabel {"
        "   color: #00FF88;"
        "   font-weight: bold;"
        "   font-size: 13px;"
        "   letter-spacing: 3px;"
        "   font-family: 'Courier New', monospace;"
        "}"
    );
    auto *titleGlow = new QGraphicsDropShadowEffect(titleLabel);
    titleGlow->setBlurRadius(10);
    titleGlow->setOffset(0, 0);
    titleGlow->setColor(QColor(0, 255, 136, 180));
    titleLabel->setGraphicsEffect(titleGlow);

    auto *subLabel = new QLabel("SYSTEME IA  -  EN LIGNE", header);
    subLabel->setStyleSheet(
        "QLabel {"
        "   color: #7FFFA0;"
        "   font-size: 9px;"
        "   letter-spacing: 2px;"
        "   font-family: 'Courier New', monospace;"
        "}"
    );

    titleBlock->addWidget(titleLabel);
    titleBlock->addWidget(subLabel);

    // Bouton fermer
    auto *closeBtn = new QPushButton("[ X ]", header);
    closeBtn->setFixedSize(50, 30);
    closeBtn->setCursor(Qt::PointingHandCursor);
    closeBtn->setStyleSheet(
        "QPushButton {"
        "   background: transparent;"
        "   color: #00FF88;"
        "   font-size: 11px;"
        "   font-weight: bold;"
        "   border: 1px solid #00FF88;"
        "   border-radius: 4px;"
        "   font-family: 'Courier New', monospace;"
        "}"
        "QPushButton:hover {"
        "   background-color: #00FF88;"
        "   color: #050F0A;"
        "}"
    );
    connect(closeBtn, &QPushButton::clicked, this, &Chatbot::toggleVisibility);

    headerLayout->addWidget(statusDot);
    headerLayout->addLayout(titleBlock, 1);
    headerLayout->addWidget(closeBtn);
    mainLayout->addWidget(header);

    // ----------------------------------------------------------
    // ZONE DE MESSAGES
    // ----------------------------------------------------------
    m_scrollArea = new QScrollArea(this);
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setFrameShape(QFrame::NoFrame);
    m_scrollArea->setStyleSheet(
        "QScrollArea { border: none; background-color: #050F0A; }"
        "QScrollBar:vertical { width: 4px; background: #0A1A10; border-radius: 2px; }"
        "QScrollBar::handle:vertical { background: #00FF88; border-radius: 2px; min-height: 20px; }"
        "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0px; }"
    );

    m_scrollWidget = new QWidget();
    m_scrollWidget->setStyleSheet("background-color: #050F0A;");
    m_messagesLayout = new QVBoxLayout(m_scrollWidget);
    m_messagesLayout->setContentsMargins(14, 14, 14, 14);
    m_messagesLayout->setSpacing(12);
    m_messagesLayout->addStretch();
    m_scrollArea->setWidget(m_scrollWidget);
    mainLayout->addWidget(m_scrollArea, 1);

    // ----------------------------------------------------------
    // INDICATEUR DE FRAPPE
    // ----------------------------------------------------------
    m_typingWidget = new QWidget(this);
    m_typingWidget->setVisible(false);
    m_typingWidget->setFixedHeight(28);
    m_typingWidget->setStyleSheet("background-color: #050F0A;");

    auto *typingLayout = new QHBoxLayout(m_typingWidget);
    typingLayout->setContentsMargins(18, 0, 18, 0);

    m_typingLabel = new QLabel("IA EN TRAITEMENT   ", m_typingWidget);
    m_typingLabel->setStyleSheet(
        "QLabel {"
        "   color: #00FF88;"
        "   font-size: 10px;"
        "   font-family: 'Courier New', monospace;"
        "   letter-spacing: 2px;"
        "}"
    );
    typingLayout->addWidget(m_typingLabel);
    typingLayout->addStretch();
    mainLayout->addWidget(m_typingWidget);

    // ----------------------------------------------------------
    // SEPARATEUR
    // ----------------------------------------------------------
    auto *separator = new QFrame(this);
    separator->setFixedHeight(1);
    separator->setStyleSheet("background-color: #00FF88;");
    mainLayout->addWidget(separator);

    // ----------------------------------------------------------
    // ZONE DE SAISIE
    // ----------------------------------------------------------
    auto *inputContainer = new QWidget(this);
    inputContainer->setFixedHeight(70);
    inputContainer->setStyleSheet("background-color: #050F0A;");

    auto *inputLayout = new QHBoxLayout(inputContainer);
    inputLayout->setContentsMargins(14, 14, 14, 14);
    inputLayout->setSpacing(10);

    m_inputField = new QLineEdit(inputContainer);
    m_inputField->setPlaceholderText("> ENTREZ VOTRE REQUETE...");
    m_inputField->setFixedHeight(42);
    m_inputField->setStyleSheet(
        "QLineEdit {"
        "   background-color: #0A1A10;"
        "   color: #00FF88;"
        "   border: 1px solid #00CC66;"
        "   border-radius: 21px;"
        "   padding: 0px 18px;"
        "   font-size: 12px;"
        "   font-family: 'Courier New', monospace;"
        "}"
        "QLineEdit:focus {"
        "   border: 1px solid #00FF88;"
        "   background-color: #0D2018;"
        "}"
    );
    connect(m_inputField, &QLineEdit::returnPressed, this, &Chatbot::sendMessage);

    m_sendButton = new QPushButton(">>", inputContainer);
    m_sendButton->setFixedSize(42, 42);
    m_sendButton->setCursor(Qt::PointingHandCursor);
    m_sendButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #00FF88;"
        "   color: #050F0A;"
        "   border-radius: 21px;"
        "   font-size: 13px;"
        "   font-weight: bold;"
        "   font-family: 'Courier New', monospace;"
        "   border: none;"
        "}"
        "QPushButton:hover { background-color: #39FF14; }"
        "QPushButton:pressed { background-color: #00CC66; }"
        "QPushButton:disabled {"
        "   background-color: #1A4030;"
        "   color: #3A6A50;"
        "}"
    );

    auto *btnGlow = new QGraphicsDropShadowEffect(m_sendButton);
    btnGlow->setBlurRadius(15);
    btnGlow->setOffset(0, 0);
    btnGlow->setColor(QColor(0, 255, 136, 150));
    m_sendButton->setGraphicsEffect(btnGlow);

    connect(m_sendButton, &QPushButton::clicked, this, &Chatbot::sendMessage);

    inputLayout->addWidget(m_inputField);
    inputLayout->addWidget(m_sendButton);
    mainLayout->addWidget(inputContainer);

    // Message de bienvenue automatique
    QTimer::singleShot(400, [this]() {
        addMessage("Systeme initialise. Bonjour, comment puis-je vous aider ?", false);
    });
}

// ============================================================
//  AJOUTER UN MESSAGE dans la zone de chat
// ============================================================
void Chatbot::addMessage(const QString &text, bool isUser)
{
    auto *row = new QHBoxLayout();
    row->setSpacing(8);

    auto *msgLabel = new QLabel(text, m_scrollWidget);
    msgLabel->setWordWrap(true);
    msgLabel->setMaximumWidth(280);
    msgLabel->setMinimumWidth(60);

    if (isUser) {
        // Bulle utilisateur — droite, vert fonce
        msgLabel->setStyleSheet(
            "QLabel {"
            "   background-color: #003322;"
            "   color: #E0FFE8;"
            "   padding: 12px 16px;"
            "   border-radius: 20px;"
            "   border-bottom-right-radius: 4px;"
            "   border: 1px solid #00CC66;"
            "   font-size: 13px;"
            "   font-family: 'Courier New', monospace;"
            "}"
        );
        auto *bubbleGlow = new QGraphicsDropShadowEffect(msgLabel);
        bubbleGlow->setBlurRadius(12);
        bubbleGlow->setOffset(0, 2);
        bubbleGlow->setColor(QColor(0, 204, 102, 100));
        msgLabel->setGraphicsEffect(bubbleGlow);

        // Label YOU
        auto *youLabel = new QLabel("[YOU]", m_scrollWidget);
        youLabel->setFixedWidth(45);
        youLabel->setAlignment(Qt::AlignTop);
        youLabel->setStyleSheet(
            "QLabel {"
            "   color: #7FFFA0;"
            "   font-size: 9px;"
            "   font-family: 'Courier New', monospace;"
            "   font-weight: bold;"
            "   padding-top: 4px;"
            "}"
        );

        row->addStretch();
        row->addWidget(msgLabel);
        row->addWidget(youLabel);

    } else {
        // Bulle IA — gauche, noir verdatre
        msgLabel->setStyleSheet(
            "QLabel {"
            "   background-color: #0A1A10;"
            "   color: #E0FFE8;"
            "   padding: 12px 16px;"
            "   border-radius: 20px;"
            "   border-bottom-left-radius: 4px;"
            "   border: 1px solid #1A5A35;"
            "   font-size: 13px;"
            "   font-family: 'Courier New', monospace;"
            "}"
        );
        auto *bubbleGlow = new QGraphicsDropShadowEffect(msgLabel);
        bubbleGlow->setBlurRadius(8);
        bubbleGlow->setOffset(2, 2);
        bubbleGlow->setColor(QColor(0, 255, 136, 50));
        msgLabel->setGraphicsEffect(bubbleGlow);

        // Label IA
        auto *iaLabel = new QLabel("[IA]", m_scrollWidget);
        iaLabel->setFixedWidth(35);
        iaLabel->setAlignment(Qt::AlignTop);
        iaLabel->setStyleSheet(
            "QLabel {"
            "   color: #00FF88;"
            "   font-size: 9px;"
            "   font-family: 'Courier New', monospace;"
            "   font-weight: bold;"
            "   padding-top: 4px;"
            "}"
        );

        row->addWidget(iaLabel);
        row->addWidget(msgLabel);
        row->addStretch();
    }

    m_messagesLayout->insertLayout(m_messagesLayout->count() - 1, row);

    // Scroll automatique vers le bas
    QTimer::singleShot(80, [this]() {
        m_scrollArea->verticalScrollBar()->setValue(
            m_scrollArea->verticalScrollBar()->maximum()
        );
    });
}

// ============================================================
//  INDICATEUR DE CHARGEMENT — Afficher / Cacher
// ============================================================
void Chatbot::showTypingIndicator()
{
    m_dotCount = 0;
    m_typingWidget->setVisible(true);
    m_dotsTimer->start(400);
}

void Chatbot::hideTypingIndicator()
{
    m_dotsTimer->stop();
    m_typingWidget->setVisible(false);
}

// ============================================================
//  ANIMATION DES POINTS "IA EN TRAITEMENT..."
// ============================================================
void Chatbot::animateDots()
{
    m_dotCount = (m_dotCount + 1) % 4;
    QString dots   = QString(".").repeated(m_dotCount);
    QString spaces = QString(" ").repeated(3 - m_dotCount);
    m_typingLabel->setText("IA EN TRAITEMENT " + dots + spaces);
}

// ============================================================
//  ENVOYER UN MESSAGE — Appel API Groq
// ============================================================
void Chatbot::sendMessage()
{
    QString txt = m_inputField->text().trimmed();
    if (txt.isEmpty()) return;

    // Afficher le message utilisateur et bloquer la saisie
    m_inputField->clear();
    m_inputField->setEnabled(false);
    m_sendButton->setEnabled(false);
    addMessage(txt, true);
    showTypingIndicator();

    // Construction de la requete HTTP
    QNetworkRequest request((QUrl(API_URL)));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", QString("Bearer %1").arg(API_KEY).toUtf8());

    // Corps JSON — format OpenAI compatible
    QJsonArray messages;

    QJsonObject systemMsg;
    systemMsg["role"]    = "system";
    systemMsg["content"] = SYSTEM_PROMPT;
    messages.append(systemMsg);

    QJsonObject userMsg;
    userMsg["role"]    = "user";
    userMsg["content"] = txt;
    messages.append(userMsg);

    QJsonObject jsonBody;
    jsonBody["model"]       = "llama-3.1-8b-instant";
    jsonBody["messages"]    = messages;
    jsonBody["max_tokens"]  = 256;
    jsonBody["temperature"] = 0.7;
    jsonBody["stream"]      = false;

    QNetworkReply *reply = m_networkManager->post(
        request,
        QJsonDocument(jsonBody).toJson()
    );

    // Callback quand la reponse arrive
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        hideTypingIndicator();
        m_inputField->setEnabled(true);
        m_sendButton->setEnabled(true);
        m_inputField->setFocus();
        onReplyFinished(reply);
    });
}

// ============================================================
//  TRAITEMENT DE LA REPONSE API
// ============================================================
void Chatbot::onReplyFinished(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError) {
        // Succes — extraire le texte de la reponse
        QByteArray rawData    = reply->readAll();
        QJsonDocument doc     = QJsonDocument::fromJson(rawData);
        QJsonObject root      = doc.object();

        // Format Groq/OpenAI : choices[0].message.content
        QString res = root["choices"]
                          .toArray()[0]
                          .toObject()["message"]
                          .toObject()["content"]
                          .toString()
                          .trimmed();

        if (res.isEmpty()) {
            addMessage("ERREUR : Reponse vide. Verifiez votre cle API.", false);
        } else {
            addMessage(res, false);
        }

    } else {
        // Echec — afficher message d'erreur selon le code HTTP
        int httpStatus = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        QString errMsg;

        switch (httpStatus) {
        case 401:
            errMsg = "ERREUR 401 : Cle API invalide.";
            break;
        case 429:
            errMsg = "ERREUR 429 : Limite atteinte. Reessayez dans quelques secondes.";
            break;
        case 0:
            errMsg = "ERREUR : Pas de connexion reseau. Verifiez votre internet.";
            break;
        default:
            errMsg = QString("ERREUR %1 : Service momentanement indisponible.").arg(httpStatus);
            break;
        }

        addMessage(errMsg, false);
    }

    reply->deleteLater();
}

// ============================================================
//  AFFICHER / CACHER LE CHATBOT
// ============================================================
void Chatbot::toggleVisibility()
{
    if (!parentWidget()) return;
    m_isVisible ? animateHide() : animateShow();
}

// ============================================================
//  ANIMATION D'OUVERTURE — glisse depuis la droite
// ============================================================
void Chatbot::animateShow()
{
    show();
    raise();
    int pw = parentWidget()->width();
    int ph = parentWidget()->height();
    m_animation->setStartValue(QRect(pw, 0, width(), ph));
    m_animation->setEndValue(QRect(pw - width(), 0, width(), ph));
    m_animation->start();
    m_isVisible = true;
}

// ============================================================
//  ANIMATION DE FERMETURE — glisse vers la droite
// ============================================================
void Chatbot::animateHide()
{
    int pw = parentWidget()->width();
    int ph = parentWidget()->height();
    m_animation->setStartValue(QRect(pw - width(), 0, width(), ph));
    m_animation->setEndValue(QRect(pw, 0, width(), ph));
    connect(m_animation, &QPropertyAnimation::finished, this, [this]() {
        if (!m_isVisible) hide();
    });
    m_animation->start();
    m_isVisible = false;
}