#ifndef USERSTAFF_H
#define USERSTAFF_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class userstaff;
}
QT_END_NAMESPACE

class userstaff : public QMainWindow
{
    Q_OBJECT

public:
    userstaff(QWidget *parent = nullptr);
    ~userstaff();

signals:
    void backToMenu();

private slots:
    void onBackClicked();

private:
    Ui::userstaff *ui;
};
#endif // USERSTAFF_H
