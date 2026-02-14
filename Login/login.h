#ifndef LOGIN_H
#define LOGIN_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class Login;
}
QT_END_NAMESPACE

class Login : public QMainWindow
{
    Q_OBJECT

public:
    Login(QWidget *parent = nullptr);
    ~Login();

signals:
    void goToMenu();

private slots:
    void onSuivantClicked();

private:
    Ui::Login *ui;
};
#endif // LOGIN_H
