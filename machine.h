#ifndef MACHINE_H
#define MACHINE_H

#include <QMainWindow>
#include <QTimer>
#include <QWidget>
#include <QHBoxLayout>
#include <QPushButton>
#include <QVector>

QT_BEGIN_NAMESPACE
namespace Ui {
class machine;
}
QT_END_NAMESPACE

// Navigation Bar Class Declaration
class NavigationBar : public QWidget
{
    Q_OBJECT

public:
    explicit NavigationBar(QWidget *parent = nullptr);
    ~NavigationBar();

    void addTab(const QString &icon, const QString &text);
    void setActiveTab(int index);
    int getCurrentTab() const;
    void clear();

signals:
    void tabClicked(int index);

private slots:
    void onTabButtonClicked();

private:
    void updateStyles();
    void setupLayout();

    QHBoxLayout *layout;
    QVector<QPushButton *> buttons;
    int activeTabIndex;
    const QString ACTIVE_COLOR = "#1A3C2F";
};

// Machine Main Window Class
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
    void updateDateTime();
    void onNavigationTabClicked(int index);
    void onAjouterButtonClicked();
    
    // Auto-connected slots (Qt naming convention)
    void on_btnEnregistrerParc_clicked();
    void on_btnAnnulerParc_clicked();

private:
    void setupNavigationBar();
    void afficherFormulaireParc();
    void masquerFormulaireParc();
    void reinitialiserFormulaireParc();

    Ui::machine *ui;
    QTimer *dateTimeTimer;
    NavigationBar *navigationBar;
};
#endif // MACHINE_H
