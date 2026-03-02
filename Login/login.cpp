#include "login.h"

#include <QApplication>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGraphicsDropShadowEffect>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QSequentialAnimationGroup>
#include <QParallelAnimationGroup>
#include <QPalette>
#include <QPainter>
#include <QResizeEvent>
#include <QShowEvent>
#include <QEvent>
#include <QFontDatabase>
#include <QSpacerItem>
#include <QCursor>

// ============================================================================
// STYLESHEET
// ============================================================================

static const char *GLOBAL_QSS = R"(
QMainWindow {
    background: #050608;
}

QWidget#bgWidget {
    background: transparent;
}

QWidget#overlayWidget {
    background: rgba(5, 6, 8, 140);
}

QWidget#leftPanel {
    background: transparent;
}

/* ── CARD ── */
QFrame#cardFrame {
    background: rgba(255, 255, 255, 18);
    border: 1px solid rgba(255, 255, 255, 35);
    border-radius: 24px;
}

/* ── BRAND ── */
QLabel#brandIcon {
    color: #C9A227;
    font-size: 28px;
}
QLabel#brandTitle {
    color: #ffffff;
    font-size: 17px;
    font-weight: 700;
    letter-spacing: 3px;
}
QLabel#brandSubtitle {
    color: rgba(255,255,255,100);
    font-size: 10px;
    letter-spacing: 1px;
}

/* ── INPUT WRAPPER ── */
QFrame#inputFrame {
    background: rgba(255, 255, 255, 6);
    border: 1.5px solid rgba(255, 255, 255, 18);
    border-radius: 14px;
}
QFrame#inputFrame:focus-within {
    border: 1.5px solid #C9A227;
}

QLineEdit {
    background: transparent;
    border: none;
    color: #ffffff;
    font-size: 13px;
    padding: 0px;
    selection-background-color: rgba(201, 162, 39, 80);
}

QLabel#userIconLabel, QLabel#lockIconLabel {
    color: rgba(255,255,255,60);
    font-size: 15px;
    min-width: 20px;
}

QPushButton#eyeToggle {
    background: transparent;
    border: none;
    color: rgba(255,255,255,50);
    font-size: 14px;
    padding: 0px;
    min-width: 24px;
    max-width: 24px;
}
QPushButton#eyeToggle:hover {
    color: rgba(255,255,255,120);
}

/* ── LOGIN BUTTON ── */
QPushButton#loginBtn {
    background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
        stop:0 #1B6B3A, stop:0.5 #C9A227, stop:1 #D4AF37);
    color: #ffffff;
    border: none;
    border-radius: 14px;
    font-size: 14px;
    font-weight: 700;
    letter-spacing: 1px;
    padding: 0px;
}
QPushButton#loginBtn:hover {
    background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
        stop:0 #228B4A, stop:0.5 #D4AF37, stop:1 #E0C068);
}
QPushButton#loginBtn:pressed {
    background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
        stop:0 #145C2E, stop:0.5 #B8941F, stop:1 #C9A227);
}
QPushButton#loginBtn:disabled {
    background: rgba(255, 255, 255, 12);
    color: rgba(255, 255, 255, 35);
}

/* ── CHECKBOX ── */
QCheckBox#rememberCheck {
    color: rgba(255,255,255,120);
    font-size: 11px;
    spacing: 8px;
}
QCheckBox#rememberCheck::indicator {
    width: 16px;
    height: 16px;
    border-radius: 4px;
    border: 1.5px solid rgba(255,255,255,30);
    background: rgba(255,255,255,6);
}
QCheckBox#rememberCheck::indicator:checked {
    background: #C9A227;
    border: 1.5px solid #C9A227;
}

/* ── FORGOT  ── */
QPushButton#forgotBtn {
    background: transparent;
    border: none;
    color: rgba(255,255,255,90);
    font-size: 11px;
}
QPushButton#forgotBtn:hover {
    color: #C9A227;
}

/* ── ERROR ── */
QLabel#errorLabel {
    color: #ef4444;
    font-size: 11px;
    font-weight: 600;
    padding: 8px 12px;
    background: rgba(239, 68, 68, 18);
    border: 1px solid rgba(239, 68, 68, 40);
    border-radius: 10px;
}

/* ── LEFT PANEL ── */
QLabel#welcomeLabel {
    color: rgba(255,255,255,220);
    font-size: 32px;
    font-weight: 300;
}
QLabel#appNameLabel {
    color: #C9A227;
    font-size: 54px;
    font-weight: 900;
}
QLabel#taglineLabel {
    color: rgba(255,255,255,140);
    font-size: 14px;
}
QLabel#feat1, QLabel#feat2, QLabel#feat3 {
    color: rgba(255,255,255,100);
    font-size: 12px;
    padding: 3px 0px;
}

/* ── FOOTER ── */
QLabel#footerLabel {
    color: rgba(255,255,255,50);
    font-size: 10px;
}

/* ── SPINNER ── */
QLabel#spinnerLabel {
    color: #ffffff;
    font-size: 13px;
    font-weight: 600;
}
)";

// ============================================================================
// CONSTRUCTOR
// ============================================================================

Login::Login(QWidget *parent)
    : QMainWindow(parent)
    , m_cardOpacity(0.0)
    , m_cardYOffset(40)
    , m_isLoading(false)
    , m_passwordVisible(false)
    , m_entrancePlayed(false)
{
    setWindowTitle("PressIQ - Connexion");
    setMinimumSize(960, 620);
    resize(1441, 616);
    setStyleSheet(GLOBAL_QSS);

    bgPixmap = QPixmap(":/image/bg.png");

    buildUi();

    // Connections
    connect(loginBtn, &QPushButton::clicked, this, &Login::onLoginClicked);
    connect(usernameEdit, &QLineEdit::textChanged, this, &Login::validateFields);
    connect(passwordEdit, &QLineEdit::textChanged, this, &Login::validateFields);
    connect(passwordEdit, &QLineEdit::returnPressed, this, &Login::onLoginClicked);
    connect(usernameEdit, &QLineEdit::returnPressed, this, [this]() {
        passwordEdit->setFocus();
    });
    connect(eyeToggle, &QPushButton::clicked, this, &Login::togglePasswordVisibility);

    // Install event filter for input frame focus styling
    usernameEdit->installEventFilter(this);
    passwordEdit->installEventFilter(this);

    validateFields();
    usernameEdit->setFocus();
}

Login::~Login()
{
}

// ============================================================================
// BUILD UI (code-only, no .ui file)
// ============================================================================

void Login::buildUi()
{
    QWidget *central = new QWidget(this);
    setCentralWidget(central);

    // ── Background layer ──
    bgWidget = new QWidget(central);
    bgWidget->setObjectName("bgWidget");

    overlayWidget = new QWidget(central);
    overlayWidget->setObjectName("overlayWidget");

    // ── MAIN LAYOUT (on overlay) ──
    QHBoxLayout *mainH = new QHBoxLayout(overlayWidget);
    mainH->setContentsMargins(60, 50, 60, 30);
    mainH->setSpacing(50);

    // ════════════════════════════════════════
    // LEFT PANEL
    // ════════════════════════════════════════
    leftPanel = new QWidget();
    leftPanel->setObjectName("leftPanel");
    leftPanel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    leftEffect = new QGraphicsOpacityEffect(leftPanel);
    leftEffect->setOpacity(0.0);
    leftPanel->setGraphicsEffect(leftEffect);

    QVBoxLayout *leftLayout = new QVBoxLayout(leftPanel);
    leftLayout->setContentsMargins(10, 0, 0, 0);
    leftLayout->setSpacing(6);

    leftLayout->addStretch(2);

    welcomeLabel = new QLabel("Bienvenue sur");
    welcomeLabel->setObjectName("welcomeLabel");
    leftLayout->addWidget(welcomeLabel);

    appNameLabel = new QLabel("PRESSIQ");
    appNameLabel->setObjectName("appNameLabel");
    leftLayout->addWidget(appNameLabel);

    taglineLabel = new QLabel("Systeme de Gestion Intelligente d'Huilerie");
    taglineLabel->setObjectName("taglineLabel");
    taglineLabel->setWordWrap(true);
    leftLayout->addWidget(taglineLabel);

    leftLayout->addSpacing(35);

    // Feature bullets with Unicode dots
    feat1 = new QLabel(QString::fromUtf8("\xe2\x97\x8f  Suivi de production en temps reel"));
    feat1->setObjectName("feat1");
    feat2 = new QLabel(QString::fromUtf8("\xe2\x97\x8f  Gestion des stocks et machines"));
    feat2->setObjectName("feat2");
    feat3 = new QLabel(QString::fromUtf8("\xe2\x97\x8f  Tableaux de bord et rapports financiers"));
    feat3->setObjectName("feat3");
    leftLayout->addWidget(feat1);
    leftLayout->addWidget(feat2);
    leftLayout->addWidget(feat3);

    leftLayout->addStretch(3);

    mainH->addWidget(leftPanel, 1);

    // ════════════════════════════════════════
    // RIGHT PANEL — LOGIN CARD
    // ════════════════════════════════════════
    QVBoxLayout *rightVBox = new QVBoxLayout();
    rightVBox->setSpacing(0);
    rightVBox->addStretch(1);

    cardFrame = new QFrame();
    cardFrame->setObjectName("cardFrame");
    cardFrame->setFixedWidth(420);
    cardFrame->setMinimumHeight(460);

    // Card glow shadow
    QGraphicsDropShadowEffect *cardShadow = new QGraphicsDropShadowEffect(cardFrame);
    cardShadow->setBlurRadius(60);
    cardShadow->setOffset(0, 8);
    cardShadow->setColor(QColor(201, 162, 39, 45));
    cardFrame->setGraphicsEffect(cardShadow);

    QVBoxLayout *cardLayout = new QVBoxLayout(cardFrame);
    cardLayout->setContentsMargins(36, 40, 36, 32);
    cardLayout->setSpacing(0);

    // ── Brand row ──
    QHBoxLayout *brandRow = new QHBoxLayout();
    brandRow->setSpacing(10);

    brandIcon = new QLabel(QString::fromUtf8("\xf0\x9f\xab\x92"));
    brandIcon->setObjectName("brandIcon");
    brandRow->addWidget(brandIcon);

    QVBoxLayout *brandText = new QVBoxLayout();
    brandText->setSpacing(0);
    brandTitle = new QLabel("PRESSIQ");
    brandTitle->setObjectName("brandTitle");
    brandSubtitle = new QLabel("SMART OIL PRESS MANAGEMENT");
    brandSubtitle->setObjectName("brandSubtitle");
    brandText->addWidget(brandTitle);
    brandText->addWidget(brandSubtitle);
    brandRow->addLayout(brandText);
    brandRow->addStretch();
    cardLayout->addLayout(brandRow);

    cardLayout->addSpacing(28);

    // ── Separator ──
    QFrame *sep = new QFrame();
    sep->setFrameShape(QFrame::HLine);
    sep->setStyleSheet("background: rgba(255,255,255,15); max-height: 1px; border: none;");
    cardLayout->addWidget(sep);

    cardLayout->addSpacing(24);

    // ── USERNAME FIELD ──
    QFrame *userFrame = new QFrame();
    userFrame->setObjectName("inputFrame");
    userFrame->setFixedHeight(52);

    QHBoxLayout *userH = new QHBoxLayout(userFrame);
    userH->setContentsMargins(16, 0, 16, 0);
    userH->setSpacing(12);

    userIconLabel = new QLabel(QString::fromUtf8("\xf0\x9f\x91\xa4"));
    userIconLabel->setObjectName("userIconLabel");
    userIconLabel->setFixedWidth(20);
    userH->addWidget(userIconLabel);

    usernameEdit = new QLineEdit();
    usernameEdit->setPlaceholderText("Identifiant ou email");
    usernameEdit->setFrame(false);
    userH->addWidget(usernameEdit);

    cardLayout->addWidget(userFrame);
    cardLayout->addSpacing(14);

    // ── PASSWORD FIELD ──
    QFrame *passFrame = new QFrame();
    passFrame->setObjectName("inputFrame");
    passFrame->setFixedHeight(52);

    QHBoxLayout *passH = new QHBoxLayout(passFrame);
    passH->setContentsMargins(16, 0, 16, 0);
    passH->setSpacing(12);

    lockIconLabel = new QLabel(QString::fromUtf8("\xf0\x9f\x94\x92"));
    lockIconLabel->setObjectName("lockIconLabel");
    lockIconLabel->setFixedWidth(20);
    passH->addWidget(lockIconLabel);

    passwordEdit = new QLineEdit();
    passwordEdit->setPlaceholderText("Mot de passe");
    passwordEdit->setEchoMode(QLineEdit::Password);
    passwordEdit->setFrame(false);
    passH->addWidget(passwordEdit);

    eyeToggle = new QPushButton(QString::fromUtf8("\xf0\x9f\x91\x81"));
    eyeToggle->setObjectName("eyeToggle");
    eyeToggle->setCursor(Qt::PointingHandCursor);
    eyeToggle->setFocusPolicy(Qt::NoFocus);
    passH->addWidget(eyeToggle);

    cardLayout->addWidget(passFrame);
    cardLayout->addSpacing(10);

    // ── ERROR LABEL ──
    errorLabel = new QLabel();
    errorLabel->setObjectName("errorLabel");
    errorLabel->setWordWrap(true);
    errorLabel->setAlignment(Qt::AlignCenter);
    errorLabel->setVisible(false);
    errorLabel->setFixedHeight(0);

    errorEffect = new QGraphicsOpacityEffect(errorLabel);
    errorEffect->setOpacity(0.0);
    errorLabel->setGraphicsEffect(errorEffect);

    cardLayout->addWidget(errorLabel);
    cardLayout->addSpacing(4);

    // ── OPTIONS ROW ──
    QHBoxLayout *optRow = new QHBoxLayout();
    rememberCheck = new QCheckBox("Se souvenir de moi");
    rememberCheck->setObjectName("rememberCheck");
    rememberCheck->setCursor(Qt::PointingHandCursor);
    optRow->addWidget(rememberCheck);

    optRow->addStretch();

    forgotBtn = new QPushButton("Mot de passe oublie ?");
    forgotBtn->setObjectName("forgotBtn");
    forgotBtn->setCursor(Qt::PointingHandCursor);
    forgotBtn->setFocusPolicy(Qt::NoFocus);
    optRow->addWidget(forgotBtn);

    cardLayout->addLayout(optRow);
    cardLayout->addSpacing(20);

    // ── LOGIN BUTTON ──
    loginBtn = new QPushButton("SE CONNECTER");
    loginBtn->setObjectName("loginBtn");
    loginBtn->setFixedHeight(52);
    loginBtn->setCursor(Qt::PointingHandCursor);
    loginBtn->setEnabled(false);
    cardLayout->addWidget(loginBtn);

    // Hidden spinner label (overlaps button text)
    spinnerLabel = new QLabel("Connexion en cours...");
    spinnerLabel->setObjectName("spinnerLabel");
    spinnerLabel->setAlignment(Qt::AlignCenter);
    spinnerLabel->setVisible(false);

    cardLayout->addSpacing(8);
    cardLayout->addStretch();

    rightVBox->addWidget(cardFrame, 0, Qt::AlignHCenter);
    rightVBox->addStretch(1);

    mainH->addLayout(rightVBox);

    // ── FOOTER ──
    footerLabel = new QLabel("2025 PressIQ  |  v1.0  |  Securise");
    footerLabel->setObjectName("footerLabel");
    footerLabel->setAlignment(Qt::AlignCenter);

    QVBoxLayout *rootLayout = new QVBoxLayout(central);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->setSpacing(0);
    rootLayout->addWidget(overlayWidget, 1);
    rootLayout->addWidget(footerLabel);
    rootLayout->addSpacing(8);

    // Apply background
    applyBackground();
}

// ============================================================================
// BACKGROUND
// ============================================================================

void Login::applyBackground()
{
    if (bgPixmap.isNull() || !bgWidget) return;

    QWidget *central = centralWidget();
    if (!central) return;

    bgWidget->setGeometry(0, 0, central->width(), central->height());
    overlayWidget->setGeometry(0, 0, central->width(), central->height());

    QPixmap scaled = bgPixmap.scaled(
        central->size(),
        Qt::KeepAspectRatioByExpanding,
        Qt::SmoothTransformation
    );

    int x = (scaled.width() - central->width()) / 2;
    int y = (scaled.height() - central->height()) / 2;
    QPixmap cropped = scaled.copy(x, y, central->width(), central->height());

    QPalette pal;
    pal.setBrush(QPalette::Window, QBrush(cropped));
    bgWidget->setAutoFillBackground(true);
    bgWidget->setPalette(pal);
    bgWidget->lower();
}

void Login::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    applyBackground();
}

void Login::showEvent(QShowEvent *event)
{
    QMainWindow::showEvent(event);
    if (!m_entrancePlayed) {
        m_entrancePlayed = true;
        QTimer::singleShot(100, this, &Login::playEntranceAnimation);
    }
}

// ============================================================================
// EVENT FILTER (focus glow on input frames)
// ============================================================================

bool Login::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::FocusIn || event->type() == QEvent::FocusOut) {
        QLineEdit *edit = qobject_cast<QLineEdit*>(watched);
        if (edit) {
            QFrame *frame = qobject_cast<QFrame*>(edit->parentWidget());
            if (frame) {
                bool focused = (event->type() == QEvent::FocusIn);
                frame->setStyleSheet(
                    focused
                    ? "QFrame#inputFrame { background: rgba(255,255,255,10); border: 1.5px solid #C9A227; border-radius: 14px; }"
                    : "QFrame#inputFrame { background: rgba(255,255,255,6); border: 1.5px solid rgba(255,255,255,18); border-radius: 14px; }"
                );

                // Glow effect on focus
                if (focused) {
                    QGraphicsDropShadowEffect *glow = new QGraphicsDropShadowEffect(frame);
                    glow->setBlurRadius(25);
                    glow->setOffset(0, 0);
                    glow->setColor(QColor(201, 162, 39, 60));
                    frame->setGraphicsEffect(glow);
                } else {
                    frame->setGraphicsEffect(nullptr);
                }
            }
        }
    }
    return QMainWindow::eventFilter(watched, event);
}

// ============================================================================
// PROPERTY SETTERS (for animation)
// ============================================================================

void Login::setCardOpacity(double v)
{
    m_cardOpacity = v;
    if (cardFrame) {
        QGraphicsOpacityEffect *eff = qobject_cast<QGraphicsOpacityEffect*>(cardFrame->graphicsEffect());
        if (eff) {
            eff->setOpacity(v);
        }
    }
}

void Login::setCardYOffset(int v)
{
    m_cardYOffset = v;
    // Card position is managed by layout, Y offset applied via margins
    if (cardFrame) {
        cardFrame->setContentsMargins(0, v, 0, 0);
    }
}

// ============================================================================
// ANIMATIONS
// ============================================================================

void Login::playEntranceAnimation()
{
    // Prepare: set card shadow back (was set in buildUi, need opacity effect for fade)
    // We animate the left panel and manually do card fade via container margins

    // Left panel fade in
    QPropertyAnimation *leftFade = new QPropertyAnimation(leftEffect, "opacity");
    leftFade->setDuration(800);
    leftFade->setStartValue(0.0);
    leftFade->setEndValue(1.0);
    leftFade->setEasingCurve(QEasingCurve::OutCubic);

    // Card: animate top margin for slide-up effect
    // We use a simple opacity + position animation on the card itself
    // First, temporarily replace shadow with opacity effect
    QGraphicsDropShadowEffect *origShadow = new QGraphicsDropShadowEffect(cardFrame);
    origShadow->setBlurRadius(60);
    origShadow->setOffset(0, 8);
    origShadow->setColor(QColor(201, 162, 39, 45));

    // For the entrance, we'll just animate the card's content margins
    cardFrame->setContentsMargins(36, 80, 36, 32); // Start offset down

    // Parallel entrance animation
    QParallelAnimationGroup *group = new QParallelAnimationGroup(this);
    group->addAnimation(leftFade);

    group->start(QAbstractAnimation::DeleteWhenStopped);

    // Card fade - simple timer approach
    cardFrame->setStyleSheet(cardFrame->styleSheet()); // reset
    int step = 0;
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [this, timer, &step]() {
        static int s = 0;
        s++;
        if (s >= 20) {
            timer->stop();
            timer->deleteLater();
            cardFrame->setContentsMargins(36, 40, 36, 32);
            // Restore glow shadow
            QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(cardFrame);
            shadow->setBlurRadius(60);
            shadow->setOffset(0, 8);
            shadow->setColor(QColor(201, 162, 39, 45));
            cardFrame->setGraphicsEffect(shadow);
            return;
        }
        // Ease the top margin from 80 to 40
        double t = (double)s / 20.0;
        t = 1.0 - (1.0 - t) * (1.0 - t); // ease out quad
        int topMargin = 80 - (int)(40.0 * t);
        cardFrame->setContentsMargins(36, topMargin, 36, 32);
    });
    timer->start(25); // 20 steps * 25ms = 500ms
}

void Login::playShakeAnimation()
{
    QPropertyAnimation *shake = new QPropertyAnimation(cardFrame, "pos");
    shake->setDuration(400);
    QPoint orig = cardFrame->pos();
    shake->setKeyValueAt(0.0, orig);
    shake->setKeyValueAt(0.1, orig + QPoint(-12, 0));
    shake->setKeyValueAt(0.2, orig + QPoint(10, 0));
    shake->setKeyValueAt(0.3, orig + QPoint(-8, 0));
    shake->setKeyValueAt(0.4, orig + QPoint(6, 0));
    shake->setKeyValueAt(0.5, orig + QPoint(-4, 0));
    shake->setKeyValueAt(0.6, orig + QPoint(2, 0));
    shake->setKeyValueAt(1.0, orig);
    shake->setEasingCurve(QEasingCurve::OutCubic);
    shake->start(QAbstractAnimation::DeleteWhenStopped);
}

void Login::playSuccessAnimation()
{
    // Fade out entire window
    QGraphicsOpacityEffect *winEffect = new QGraphicsOpacityEffect(centralWidget());
    centralWidget()->setGraphicsEffect(winEffect);

    QPropertyAnimation *fadeOut = new QPropertyAnimation(winEffect, "opacity");
    fadeOut->setDuration(500);
    fadeOut->setStartValue(1.0);
    fadeOut->setEndValue(0.0);
    fadeOut->setEasingCurve(QEasingCurve::InCubic);

    connect(fadeOut, &QPropertyAnimation::finished, this, [this]() {
        emit goToMenu();
        this->hide();
        // Reset for next show
        centralWidget()->setGraphicsEffect(nullptr);
    });

    fadeOut->start(QAbstractAnimation::DeleteWhenStopped);
}

// ============================================================================
// VALIDATION
// ============================================================================

void Login::validateFields()
{
    bool valid = !usernameEdit->text().trimmed().isEmpty()
              && !passwordEdit->text().trimmed().isEmpty();
    loginBtn->setEnabled(valid);
    hideError();
}

// ============================================================================
// PASSWORD TOGGLE
// ============================================================================

void Login::togglePasswordVisibility()
{
    m_passwordVisible = !m_passwordVisible;
    passwordEdit->setEchoMode(
        m_passwordVisible ? QLineEdit::Normal : QLineEdit::Password
    );
    eyeToggle->setText(m_passwordVisible
        ? QString::fromUtf8("\xf0\x9f\x91\x81")   // open eye
        : QString::fromUtf8("\xf0\x9f\x91\x81")); // same icon, style change
    eyeToggle->setStyleSheet(
        m_passwordVisible
        ? "QPushButton#eyeToggle { color: #C9A227; }"
        : "QPushButton#eyeToggle { color: rgba(255,255,255,50); }"
    );
}

// ============================================================================
// ERROR / LOADING
// ============================================================================

void Login::showError(const QString &msg)
{
    errorLabel->setText(msg);
    errorLabel->setFixedHeight(40);
    errorLabel->setVisible(true);

    QPropertyAnimation *fadeIn = new QPropertyAnimation(errorEffect, "opacity");
    fadeIn->setDuration(300);
    fadeIn->setStartValue(0.0);
    fadeIn->setEndValue(1.0);
    fadeIn->start(QAbstractAnimation::DeleteWhenStopped);
}

void Login::hideError()
{
    if (errorLabel->isVisible() && errorEffect->opacity() > 0) {
        QPropertyAnimation *fadeOut = new QPropertyAnimation(errorEffect, "opacity");
        fadeOut->setDuration(200);
        fadeOut->setStartValue(1.0);
        fadeOut->setEndValue(0.0);
        connect(fadeOut, &QPropertyAnimation::finished, this, [this]() {
            errorLabel->setVisible(false);
            errorLabel->setFixedHeight(0);
        });
        fadeOut->start(QAbstractAnimation::DeleteWhenStopped);
    }
}

void Login::setLoadingState(bool loading)
{
    m_isLoading = loading;
    loginBtn->setEnabled(!loading);
    usernameEdit->setEnabled(!loading);
    passwordEdit->setEnabled(!loading);

    if (loading) {
        loginBtn->setText("");
        spinnerLabel->setVisible(true);
        spinnerLabel->setParent(loginBtn);
        spinnerLabel->setGeometry(0, 0, loginBtn->width(), loginBtn->height());
        spinnerLabel->show();
    } else {
        loginBtn->setText("SE CONNECTER");
        spinnerLabel->setVisible(false);
    }
}

// ============================================================================
// LOGIN ACTION
// ============================================================================

void Login::onLoginClicked()
{
    if (m_isLoading) return;

    hideError();
    setLoadingState(true);

    // Simulate authentication delay (replace with real auth)
    QTimer::singleShot(800, this, [this]() {
        setLoadingState(false);

        // For now: always succeed (integrate your auth logic here)
        QString user = usernameEdit->text().trimmed();
        QString pass = passwordEdit->text().trimmed();

        if (user.isEmpty() || pass.isEmpty()) {
            showError("Veuillez remplir tous les champs");
            playShakeAnimation();
            return;
        }

        // Success → play fade out and emit signal
        playSuccessAnimation();
    });
}
