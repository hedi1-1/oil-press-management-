#ifndef MACHINE_H
#define MACHINE_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class machine;
}
QT_END_NAMESPACE

class machine : public QMainWindow
{
    Q_OBJECT

public:
    machine(QWidget *parent = nullptr);
    ~machine();

signals:
    void backToMenu();

private slots:
    void onBackButtonClicked();

private:
    Ui::machine *ui;
};
#endif // MACHINE_H
