#ifndef LOGIN_H
#define LOGIN_H

#include <QMainWindow>
#include <QPixmap>
#include <QPropertyAnimation>
#include <QSequentialAnimationGroup>
#include <QParallelAnimationGroup>
#include <QGraphicsOpacityEffect>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QCheckBox>
#include <QFrame>
#include <QTimer>
#include <QWidget>
#include <QString>

class Login : public QMainWindow
{
    Q_OBJECT
    Q_PROPERTY(double cardOpacity READ cardOpacity WRITE setCardOpacity)
    Q_PROPERTY(int cardYOffset READ cardYOffset WRITE setCardYOffset)

public:
    Login(QWidget *parent = nullptr);
    ~Login();

    double cardOpacity() const { return m_cardOpacity; }
    void setCardOpacity(double v);
    int cardYOffset() const { return m_cardYOffset; }
    void setCardYOffset(int v);

signals:
    void goToMenu(const QString &username, const QString &role);

protected:
    void resizeEvent(QResizeEvent *event) override;
    void showEvent(QShowEvent *event) override;
    bool eventFilter(QObject *watched, QEvent *event) override;

private slots:
    void onLoginClicked();
    void validateFields();
    void togglePasswordVisibility();
    void onForgotPasswordClicked();

private:
    static QString hashPassword(const QString &pw);

    void buildUi();
    void applyBackground();
    void playEntranceAnimation();
    void playShakeAnimation();
    void playSuccessAnimation();
    void showError(const QString &msg);
    void hideError();
    void setLoadingState(bool loading);

    int m_failedAttempts = 0;
    QString m_loggedInUsername;
    QString m_loggedInRole;

    // Background
    QWidget *bgWidget;
    QWidget *overlayWidget;
    QPixmap bgPixmap;

    // Main card
    QFrame *cardFrame;
    QGraphicsOpacityEffect *cardEffect;

    // Brand
    QLabel *brandIcon;
    QLabel *brandTitle;
    QLabel *brandSubtitle;

    // Inputs
    QLineEdit *usernameEdit;
    QLineEdit *passwordEdit;
    QLabel *userIconLabel;
    QLabel *lockIconLabel;
    QPushButton *eyeToggle;

    // Button
    QPushButton *loginBtn;
    QLabel *spinnerLabel;

    // Extras
    QCheckBox *rememberCheck;
    QPushButton *forgotBtn;
    QLabel *errorLabel;
    QGraphicsOpacityEffect *errorEffect;

    // Footer
    QLabel *footerLabel;

    // Left panel
    QWidget *leftPanel;
    QLabel *welcomeLabel;
    QLabel *appNameLabel;
    QLabel *taglineLabel;
    QLabel *feat1;
    QLabel *feat2;
    QLabel *feat3;
    QGraphicsOpacityEffect *leftEffect;

    // Animation state
    double m_cardOpacity;
    int m_cardYOffset;
    bool m_isLoading;
    bool m_passwordVisible;
    bool m_entrancePlayed;
};

#endif // LOGIN_H
