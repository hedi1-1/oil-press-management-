#ifndef USERSTAFF_H
#define USERSTAFF_H

#include <QMainWindow>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QLabel>
#include <QPushButton>
#include <QCheckBox>
#include <QPrinter>
#include <QPainter>
#include <QFileDialog>
#include <QDateTime>
#include <QCryptographicHash>
#include <QTableWidgetItem>

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
    void onAjouterClicked();
    void onModifierClicked();
    void onSupprimerClicked();
    void onExporterClicked();
    void onTableSelectionChanged();
    void onSearchTextChanged(const QString &text);
    void onRoleFilterChanged(const QString &role);
    void onShowArchivedToggled(bool checked);

private:
    Ui::userstaff *ui;

    void loadEmployees(const QString &searchText = "", const QString &roleFilter = "", bool showArchived = false);
    void updateStats();
    int  getSelectedUserId();
    QString hashPassword(const QString &password);
    QDialog* buildEmployeeDialog(const QString &title,
                                 const QString &username = "",
                                 const QString &email    = "",
                                 const QString &role     = "Admin",
                                 const QString &state    = "ACTIVE");
};
#endif // USERSTAFF_H
