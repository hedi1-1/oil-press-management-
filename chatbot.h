#ifndef CHATBOT_H
#define CHATBOT_H

#include <QWidget>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QLineEdit>
#include <QPushButton>
#include <QPropertyAnimation>
#include <QLabel>
#include <QTimer>

class Chatbot : public QWidget {
    Q_OBJECT

public:
    explicit Chatbot(QWidget *parent = nullptr);
    ~Chatbot();

    void toggleVisibility();

private slots:
    void sendMessage();
    void onReplyFinished(QNetworkReply *reply);
    void animateDots();

private:
    void setupUi();
    void addMessage(const QString &text, bool isUser);
    void animateShow();
    void animateHide();
    void showTypingIndicator();
    void hideTypingIndicator();

    // Widgets
    QVBoxLayout    *m_messagesLayout;
    QScrollArea    *m_scrollArea;
    QWidget        *m_scrollWidget;
    QLineEdit      *m_inputField;
    QPushButton    *m_sendButton;
    QWidget        *m_typingWidget;
    QLabel         *m_typingLabel;

    // Animation
    QNetworkAccessManager *m_networkManager;
    QPropertyAnimation    *m_animation;
    QTimer                *m_dotsTimer;
    int                    m_dotCount;

    bool m_isVisible;
};

#endif // CHATBOT_H