#ifndef STOCK_H
#define STOCK_H

#include <QMainWindow>
#include <QTimer>

QT_BEGIN_NAMESPACE
namespace Ui {
class Stock;
}
QT_END_NAMESPACE

class Stock : public QMainWindow
{
    Q_OBJECT

public:
    Stock(QWidget *parent = nullptr);
    ~Stock();

signals:
    void backToMenu();

private slots:
    void onBackClicked();
    void updateDateTime();

private:
    Ui::Stock *ui;
    QTimer *dateTimeTimer;
};

#endif // STOCK_H
