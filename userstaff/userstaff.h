#ifndef USERSTAFF_H
#define USERSTAFF_H

#include <QMainWindow>
#include <QWidget>
#include <QMap>
#include <QDate>
#include <QtSql/QSqlDatabase>
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
#include <QtPrintSupport/QPrinter>
#include <QPainter>
#include <QFileDialog>
#include <QDateTime>
#include <QCryptographicHash>
#include <QTableWidgetItem>
#include <QEvent>

class AnalyticsWidget;

QT_BEGIN_NAMESPACE
namespace Ui {
class userstaff;
}
QT_END_NAMESPACE

class userstaff : public QMainWindow
{
    Q_OBJECT

public:
    explicit userstaff(QWidget *parent);
    explicit userstaff(const QString &loggedInUser = "Admin",
                       const QString &loggedInRole = "Admin",
                       QWidget *parent = nullptr);
    ~userstaff();

signals:
    void backToMenu();

private slots:
    void onBackClicked();

    void onTableSelectionChanged();
    void onSearchTextChanged(const QString &text);
    void onRoleFilterChanged(const QString &role);
    void onShowArchivedToggled(bool checked);

    void onAjouterClicked();
    void onModifierClicked();
    void onSupprimerClicked();
    void onExporterClicked();

    void onUnlockAccount(int userId, const QString &username);
    void onShowAuditLog();
    void onShowAnalytics();
    void onExportSecurityReport();

    void onShowResetRequests();

protected:
    void changeEvent(QEvent *event) override;

private:
    Ui::userstaff *ui;

    QString m_currentUser;
    QString m_currentRole;

    QPushButton *btnResetRequests = nullptr;
    QPushButton *m_btnFullscreenToggle = nullptr;
    QPushButton *m_btnModifier = nullptr;
    QPushButton *m_btnSupprimer = nullptr;
    int m_pendingResets = 0;
    QWidget *m_mainGraphWidget = nullptr;
    QWidget *m_leftMiniGraphWidget = nullptr;
    QWidget *m_rightMiniGraphWidget = nullptr;

    void loadEmployees(const QString &searchText = "", const QString &roleFilter = "", bool showArchived = false);
    void updateStats();
    int  getSelectedUserId();
    QString hashPassword(const QString &password);

    void checkPendingResetRequests();
    void onAdminSetNewPassword(int requestId, int userId, const QString &username);

    bool isAdmin() const;
    bool requireAdmin(const QString &actionName);
    QString generateTempPassword(int length = 8) const;
    QString getLoggedInState() const;
    bool insertAuditEvent(const QString &username,
                          const QString &eventType,
                          const QString &details,
                          const QString &performedBy);
    bool tableExists(const QString &tableName) const;
    bool ensureUserstaffSchema();
    void setupIdentityHeader();
    void updateFullscreenButtonText();
    void installActionBarButtons();
    void setupGraphTab();
    void refreshGraphTab();

    void setupActivityLogTab();
    void refreshActivityLogTab();

    void logUserCreated(const QString &username);
    void logUserArchived(const QString &username);
    void logUserDeleted(const QString &username);

    QDialog* buildEmployeeDialog(const QString &title,
                                 const QString &username = "",
                                 const QString &email    = "",
                                 const QString &role     = "Admin",
                                 const QString &state    = "ACTIVE");
};

// ─────────────────────────────────────────────────────────────────────────────
// Analytics dashboard widget (QPainter charts)
// ─────────────────────────────────────────────────────────────────────────────
class AnalyticsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AnalyticsWidget(const QSqlDatabase &db, QWidget *parent = nullptr);
    void refreshData();

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QSqlDatabase m_db;

    struct UserFailRow { QString username; int failed = 0; };
    QList<UserFailRow> m_topFails;
    QMap<QString, int> m_stateCounts;
    int m_inactiveCount = 0;

    struct DayEventCounts { QDate day; int success = 0; int fail = 0; int locked = 0; };
    QList<DayEventCounts> m_last7;
};

#endif // USERSTAFF_H
