#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTabWidget>

class TransactionTab;
class SearchTab;
class StatsTab;
class JournalTab;
class BudgetTab;
class AdvancedTab;
class ExportTab;

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void initializeUI();
    void applyStyles();

    Ui::MainWindow *ui;
    QTabWidget *mainTabWidget;

    // Tabs
    TransactionTab *transactionTab;
    SearchTab *searchTab;
    StatsTab *statsTab;
    JournalTab *journalTab;
    BudgetTab *budgetTab;
    AdvancedTab *advancedTab;
    ExportTab *exportTab;
};
#endif // MAINWINDOW_H
