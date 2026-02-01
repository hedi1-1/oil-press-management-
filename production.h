#ifndef PRODUCTION_H
#define PRODUCTION_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class Production;
}
QT_END_NAMESPACE

class Production : public QMainWindow
{
    Q_OBJECT

public:
    Production(QWidget *parent = nullptr);
    ~Production();

signals:
    void backToMenu();

private slots:
    void onBackButtonClicked();

private:
    Ui::Production *ui;
};
#endif // PRODUCTION_H
