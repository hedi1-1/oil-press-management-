#include "userstaff.h"
#include "ui_userstaff.h"

#include "connectionjasser.h"

#include <QCoreApplication>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QToolButton>
#include <QFrame>
#include <QDateEdit>
#include <QSysInfo>
#include <QRandomGenerator>
#include <QStandardPaths>
#include <QPrinter>
#include <QPainter>
#include <QFont>
#include <QColor>
#include <QRect>
#include <QFileDialog>
#include <QSqlQuery>
#include <QSqlError>
#include <QCryptographicHash>
#include <QMessageBox>
#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QLabel>
#include <QCheckBox>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <functional>

// ─────────────────────────────────────────────────────────────────────────────
//  Constructor / Destructor
// ─────────────────────────────────────────────────────────────────────────────
userstaff::userstaff(QWidget *parent)
    : userstaff("Admin", "Admin", parent)
{
}

userstaff::userstaff(const QString &loggedInUser, const QString &loggedInRole, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::userstaff)
    , m_currentUser(loggedInUser)
    , m_currentRole(loggedInRole)
{
    ui->setupUi(this);
    setWindowTitle("PressIQ - User & Staff Management");

    // connect DB
    ConnectionJasser::getInstance().createconnect();

    setupIdentityHeader();
    installActionBarButtons();

    // Offline password reset requests button (insert in action bar; UI file is read-only)
    {
        QFrame *actionBar = findChild<QFrame*>("actionBarFrame");
        QHBoxLayout *actionLayout = actionBar ? qobject_cast<QHBoxLayout*>(actionBar->layout()) : nullptr;
        if (actionLayout) {
            btnResetRequests = new QPushButton(QString::fromUtf8("\xf0\x9f\x94\x91 Demandes (0)"), actionBar);
            btnResetRequests->setObjectName("btnResetRequests");
            btnResetRequests->setCursor(Qt::PointingHandCursor);
            btnResetRequests->setFixedHeight(34);

            int ajouterIndex = actionLayout->indexOf(ui->btnAjouter);
            if (ajouterIndex < 0) ajouterIndex = actionLayout->count();
            actionLayout->insertWidget(ajouterIndex, btnResetRequests);

            connect(btnResetRequests, &QPushButton::clicked,
                    this, &userstaff::onShowResetRequests);
        }
    }

    // header buttons
    connect(ui->btnBack,      &QPushButton::clicked, this, &userstaff::onBackClicked);
    connect(ui->btnAjouter,   &QPushButton::clicked, this, &userstaff::onAjouterClicked);
    connect(ui->btnModifier,  &QPushButton::clicked, this, &userstaff::onModifierClicked);
    connect(ui->btnSupprimer, &QPushButton::clicked, this, &userstaff::onSupprimerClicked);
    connect(ui->btnExporter,  &QPushButton::clicked, this, &userstaff::onExporterClicked);

    // table selection
    connect(ui->userTable, &QTableWidget::itemSelectionChanged,
            this, &userstaff::onTableSelectionChanged);

    // search / filter
    connect(ui->searchField, &QLineEdit::textChanged,
            this, &userstaff::onSearchTextChanged);
    connect(ui->roleFilter,  QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, [this](){ onRoleFilterChanged(ui->roleFilter->currentText()); });
    connect(ui->btnShowArchived, &QPushButton::toggled,
            this, &userstaff::onShowArchivedToggled);

    // advanced filters toggle
    connect(ui->btnAdvancedFilters, &QToolButton::toggled,
            this, [this](bool checked){
                ui->advancedFiltersFrame->setVisible(checked);
            });

    // table setup
    ui->userTable->setColumnCount(8);
    ui->userTable->setHorizontalHeaderLabels(
        {"Nom", "Identifiant", "Rôle", "State",
         "Failed Attempts", "Last Login", "Logs", "Actions"});
    ui->userTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->userTable->horizontalHeader()->setSectionResizeMode(7, QHeaderView::Fixed);
    ui->userTable->setColumnWidth(7, 120);
    ui->userTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    loadEmployees();
    updateStats();
    checkPendingResetRequests();
}

userstaff::~userstaff()
{
    delete ui;
}

// ─────────────────────────────────────────────────────────────────────────────
//  Helpers
// ─────────────────────────────────────────────────────────────────────────────
QString userstaff::hashPassword(const QString &password)
{
    return QString(QCryptographicHash::hash(password.toUtf8(),
                   QCryptographicHash::Sha256).toHex());
}

bool userstaff::isAdmin() const
{
    return m_currentRole.trimmed().compare("Admin", Qt::CaseInsensitive) == 0;
}

bool userstaff::requireAdmin(const QString &actionName)
{
    if (isAdmin()) return true;
    QMessageBox::warning(this, "Accès refusé",
                         QString("Action réservée aux administrateurs : %1").arg(actionName));
    return false;
}

QString userstaff::generateTempPassword(int length) const
{
    static const QString chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    QString out;
    out.reserve(length);
    for (int i = 0; i < length; ++i) {
        const int idx = QRandomGenerator::global()->bounded(chars.size());
        out.append(chars.at(idx));
    }
    return out;
}

QString userstaff::getLoggedInState() const
{
    QSqlDatabase db = ConnectionJasser::getInstance().getDatabase();
    if (!db.isOpen() || m_currentUser.trimmed().isEmpty()) return "ACTIVE";

    QSqlQuery q(db);
    q.prepare("SELECT STATE FROM EMPLOYEES WHERE UPPER(USERNAME)=UPPER(:u)");
    q.bindValue(":u", m_currentUser);
    if (q.exec() && q.next()) {
        const QString st = q.value(0).toString();
        return st.isEmpty() ? "ACTIVE" : st;
    }
    return "ACTIVE";
}

bool userstaff::insertAuditEvent(const QString &username,
                                 const QString &eventType,
                                 const QString &details,
                                 const QString &performedBy)
{
    QSqlDatabase db = ConnectionJasser::getInstance().getDatabase();
    if (!db.isOpen()) return false;

    QSqlQuery a(db);
    a.prepare("INSERT INTO AUDIT_LOG (USERNAME, EVENT_TYPE, DETAILS, PERFORMED_BY, HOSTNAME) "
              "VALUES (:u, :t, :d, :p, :h)");
    a.bindValue(":u", username);
    a.bindValue(":t", eventType);
    a.bindValue(":d", details.left(500));
    if (performedBy.trimmed().isEmpty())
        a.bindValue(":p", QVariant());
    else
        a.bindValue(":p", performedBy);
    a.bindValue(":h", QSysInfo::machineHostName());

    return a.exec();
}

void userstaff::setupIdentityHeader()
{
    if (ui->roleLabel) {
        ui->roleLabel->setText(QString::fromUtf8("\xf0\x9f\x91\xa4 ") + m_currentUser + " | " + m_currentRole);
    }

    const QString st = getLoggedInState().toUpper();
    if (ui->roleStatusDot) {
        ui->roleStatusDot->setText(st == "ACTIVE" ? QString::fromUtf8("\xf0\x9f\x9f\xa2") : QString::fromUtf8("\xf0\x9f\x94\xb4"));
    }

    // Reflect admin-only destructive permissions in the UI
    ui->btnSupprimer->setEnabled(false);
}

void userstaff::installActionBarButtons()
{
    QFrame *actionBar = findChild<QFrame*>("actionBarFrame");
    if (!actionBar || !actionBar->layout()) return;

    auto *layout = qobject_cast<QHBoxLayout*>(actionBar->layout());
    if (!layout) return;

    auto makeBtn = [&](const QString &text, const QString &objName, const QString &bg) {
        auto *b = new QPushButton(text, actionBar);
        b->setObjectName(objName);
        b->setCursor(Qt::PointingHandCursor);
        b->setFixedHeight(34);
        b->setStyleSheet(QString(
            "QPushButton { background: %1; color: white; border: none; border-radius: 6px; font-size: 13px; font-weight: 600; padding: 6px 14px; }"
            "QPushButton:hover { opacity: 0.95; }"
            "QPushButton:disabled { background: #9ca3af; color: white; }")
            .arg(bg));
        return b;
    };

    auto *btnAudit = makeBtn(QString::fromUtf8("\xf0\x9f\x93\x8b Journal"), "btnAuditLog", "#0284c7");
    auto *btnAnalytics = makeBtn(QString::fromUtf8("\xf0\x9f\x93\x8a Analyse"), "btnAnalytics", "#7c3aed");
    auto *btnSecReport = makeBtn(QString::fromUtf8("\xf0\x9f\x9b\xa1\xef\xb8\x8f Sécurité"), "btnSecReport", "#b45309");

    // Insert near existing export button row; keep minimal and safe.
    const int insertIndex = qMin(4, layout->count());
    layout->insertWidget(insertIndex, btnAudit);
    layout->insertWidget(insertIndex + 1, btnAnalytics);
    layout->insertWidget(insertIndex + 2, btnSecReport);

    connect(btnAudit, &QPushButton::clicked, this, &userstaff::onShowAuditLog);
    connect(btnAnalytics, &QPushButton::clicked, this, &userstaff::onShowAnalytics);
    connect(btnSecReport, &QPushButton::clicked, this, &userstaff::onExportSecurityReport);

    // Admin-only report is okay for all, but destructive actions inside are guarded.
}

// ─────────────────────────────────────────────────────────────────────────────
//  Offline password reset requests: badge counter
// ─────────────────────────────────────────────────────────────────────────────
void userstaff::checkPendingResetRequests()
{
    m_pendingResets = 0;

    QSqlDatabase db = ConnectionJasser::getInstance().getDatabase();
    if (db.isOpen()) {
        QSqlQuery q(db);
        if (q.exec("SELECT COUNT(*) FROM PASSWORD_RESET_REQUESTS WHERE STATUS = 'PENDING'") && q.next()) {
            m_pendingResets = q.value(0).toInt();
        }
    }

    if (!btnResetRequests) return;

    if (m_pendingResets > 0) {
        btnResetRequests->setText(QString::fromUtf8("\xf0\x9f\x94\x91 Demandes (%1)").arg(m_pendingResets));
        btnResetRequests->setStyleSheet(
            "QPushButton { background: #dc2626; color: white; border-radius: 6px;"
            "              padding: 7px 14px; font-size: 13px; font-weight: bold; }"
            "QPushButton:hover { background: #ef4444; }");
    } else {
        btnResetRequests->setText(QString::fromUtf8("\xf0\x9f\x94\x91 Demandes (0)"));
        btnResetRequests->setStyleSheet(
            "QPushButton { background: #6b7280; color: white; border-radius: 6px;"
            "              padding: 7px 14px; font-size: 13px; }"
            "QPushButton:hover { background: #9ca3af; }");
    }
}

// ─────────────────────────────────────────────────────────────────────────────
//  Offline password reset requests: admin list dialog
// ─────────────────────────────────────────────────────────────────────────────
void userstaff::onShowResetRequests()
{
    QSqlDatabase db = ConnectionJasser::getInstance().getDatabase();
    if (!db.isOpen()) {
        QMessageBox::warning(this, "DB", "Base de données non connectée.");
        return;
    }

    auto *dlg = new QDialog(this);
    dlg->setWindowTitle(QString::fromUtf8("\xf0\x9f\x94\x91 Demandes de réinitialisation de mot de passe"));
    dlg->setModal(true);
    dlg->resize(780, 520);
    dlg->setStyleSheet("QDialog { background: #f3f6f5; }");

    auto *root = new QVBoxLayout(dlg);
    root->setContentsMargins(12, 12, 12, 12);
    root->setSpacing(10);

    // Header
    auto *hdr = new QFrame(dlg);
    hdr->setFixedHeight(60);
    hdr->setStyleSheet("QFrame { background: #2c3e2e; border-radius: 10px; }");
    auto *hdrLay = new QHBoxLayout(hdr);
    hdrLay->setContentsMargins(14, 12, 14, 12);
    auto *lblTitle = new QLabel(QString::fromUtf8("\xf0\x9f\x94\x91 Demandes de réinitialisation"), hdr);
    lblTitle->setStyleSheet("color: white; font-weight: 800; font-size: 15px;");
    auto *lblPending = new QLabel("[0] demande(s) en attente", hdr);
    lblPending->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    lblPending->setStyleSheet("color: #d4a63a; font-size: 12px; font-weight: 700;");
    hdrLay->addWidget(lblTitle);
    hdrLay->addStretch();
    hdrLay->addWidget(lblPending);
    root->addWidget(hdr);

    // Filter bar
    auto *filterWrap = new QWidget(dlg);
    auto *filterLay = new QHBoxLayout(filterWrap);
    filterLay->setContentsMargins(12, 0, 12, 0);
    filterLay->setSpacing(10);

    auto *cbStatusFilter = new QComboBox(filterWrap);
    cbStatusFilter->addItems({"En attente (PENDING)", "Résolues (RESOLVED)", "Toutes"});
    cbStatusFilter->setCurrentIndex(0);

    auto *btnRefresh = new QPushButton(QString::fromUtf8("\xf0\x9f\x94\x84 Actualiser"), filterWrap);
    btnRefresh->setCursor(Qt::PointingHandCursor);

    const QString filterQss =
        "QComboBox { background: white; border: 1px solid #d1d5db; border-radius: 6px; padding: 6px 10px; font-size: 12px; }"
        "QPushButton { background: #16a34a; color: white; border: none; border-radius: 6px; padding: 7px 12px; font-size: 12px; font-weight: 800; }"
        "QPushButton:hover { background: #22c55e; }";
    cbStatusFilter->setStyleSheet(filterQss);
    btnRefresh->setStyleSheet(filterQss);

    filterLay->addWidget(cbStatusFilter);
    filterLay->addStretch();
    filterLay->addWidget(btnRefresh);
    root->addWidget(filterWrap);

    // Table
    auto *reqTable = new QTableWidget(dlg);
    reqTable->setColumnCount(7);
    reqTable->setHorizontalHeaderLabels({"ID", "Identifiant", "Email", "Message", "Date demande", "Statut", "Actions"});
    reqTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    reqTable->setAlternatingRowColors(true);
    reqTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    reqTable->setSelectionMode(QAbstractItemView::SingleSelection);
    reqTable->horizontalHeader()->setStretchLastSection(false);
    reqTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    reqTable->setColumnWidth(0, 40);
    reqTable->setColumnWidth(1, 110);
    reqTable->setColumnWidth(2, 150);
    reqTable->setColumnWidth(3, 200);
    reqTable->setColumnWidth(4, 130);
    reqTable->setColumnWidth(5, 80);
    reqTable->setColumnWidth(6, 120);
    reqTable->setStyleSheet(
        "QTableWidget { background: white; alternate-background-color: #f9fafb; gridline-color: #e5e7eb; }"
        "QHeaderView::section { background: #2c3e2e; color: white; padding: 6px; font-weight: 800; border: none; }"
        "QTableWidget::item { padding: 4px; }"
    );
    root->addWidget(reqTable, 1);

    // Footer
    auto *footer = new QHBoxLayout();
    auto *footerNote = new QLabel(
        QString::fromUtf8("\xf0\x9f\x92\xa1 Contactez l'utilisateur puis cliquez 'Définir mot de passe'\n"
                          "pour lui attribuer un nouveau mot de passe temporaire."),
        dlg);
    footerNote->setStyleSheet("color: #6b7280; font-size: 11px; font-style: italic;");
    footer->addWidget(footerNote);
    root->addLayout(footer);

    // Load function
    std::function<void()> loadFunc;
    loadFunc = [this, db, cbStatusFilter, reqTable, lblPending]() {
        // pending count badge in header
        int pendingCount = 0;
        {
            QSqlQuery qc(db);
            if (qc.exec("SELECT COUNT(*) FROM PASSWORD_RESET_REQUESTS WHERE STATUS = 'PENDING'") && qc.next())
                pendingCount = qc.value(0).toInt();
        }
        lblPending->setText(QString("[%1] demande(s) en attente").arg(pendingCount));

        reqTable->setRowCount(0);
        reqTable->clearContents();

        const QString statusFilter = (cbStatusFilter->currentIndex() == 0) ? "PENDING" :
                                     (cbStatusFilter->currentIndex() == 1) ? "RESOLVED" : "";

        QSqlQuery q(db);
        q.prepare(
            "SELECT r.REQUEST_ID, r.USERNAME, r.EMAIL, r.MESSAGE, "
            "       TO_CHAR(r.REQUEST_TIME,'DD/MM/YYYY HH24:MI'), "
            "       r.STATUS, r.RESOLVED_BY, r.RESOLVED_TIME, r.NEW_PASSWORD_SET, "
            "       e.ROLE, e.STATE, e.USER_ID "
            "FROM PASSWORD_RESET_REQUESTS r "
            "LEFT JOIN EMPLOYEES e ON r.USER_ID = e.USER_ID "
            "WHERE (:status = '' OR r.STATUS = :status) "
            "ORDER BY r.REQUEST_TIME DESC "
            "FETCH FIRST 100 ROWS ONLY");
        q.bindValue(":status", statusFilter);

        if (!q.exec()) {
            QMessageBox::warning(this, "Erreur", "Chargement échoué :\n" + q.lastError().text());
            return;
        }

        int row = 0;
        while (q.next()) {
            const int requestId = q.value(0).toInt();
            const QString username = q.value(1).toString();
            const QString email = q.value(2).toString();
            QString message = q.value(3).toString();
            const QString dateStr = q.value(4).toString();
            const QString status = q.value(5).toString();
            const int userId = q.value(11).toInt();

            if (message.length() > 40) message = message.left(40) + QString::fromUtf8("\xe2\x80\xa6");

            reqTable->insertRow(row);
            reqTable->setItem(row, 0, new QTableWidgetItem(QString::number(requestId)));
            reqTable->setItem(row, 1, new QTableWidgetItem(username));
            reqTable->setItem(row, 2, new QTableWidgetItem(email));
            reqTable->setItem(row, 3, new QTableWidgetItem(message));
            reqTable->setItem(row, 4, new QTableWidgetItem(dateStr));

            auto *stItem = new QTableWidgetItem(status);
            QFont f = stItem->font();
            if (status.compare("PENDING", Qt::CaseInsensitive) == 0) {
                stItem->setForeground(QColor("#dc2626"));
                f.setBold(true);
            } else {
                stItem->setForeground(QColor("#16a34a"));
            }
            stItem->setFont(f);
            reqTable->setItem(row, 5, stItem);

            if (status.compare("PENDING", Qt::CaseInsensitive) == 0) {
                auto *btnSetPwd = new QPushButton(QString::fromUtf8("\xf0\x9f\x94\x91 Définir mot de passe"), reqTable);
                btnSetPwd->setCursor(Qt::PointingHandCursor);
                btnSetPwd->setStyleSheet(
                    "QPushButton { background: #1B6B3A; color: white; border-radius: 5px; font-size: 11px; padding: 4px 8px; }"
                    "QPushButton:hover { background: #228B4A; }");

                QObject::connect(btnSetPwd, &QPushButton::clicked, reqTable,
                                 [this, requestId, userId, username, &loadFunc]() {
                                     onAdminSetNewPassword(requestId, userId, username);
                                     loadFunc();
                                     checkPendingResetRequests();
                                 });
                reqTable->setCellWidget(row, 6, btnSetPwd);
            } else {
                auto *lblDone = new QLabel(QString::fromUtf8("\xe2\x9c\x85 Résolu"), reqTable);
                lblDone->setAlignment(Qt::AlignCenter);
                lblDone->setStyleSheet("color: #16a34a; font-size: 11px;");
                reqTable->setCellWidget(row, 6, lblDone);
            }

            ++row;
        }
    };

    QObject::connect(btnRefresh, &QPushButton::clicked, dlg, [&]() { loadFunc(); });
    QObject::connect(cbStatusFilter, QOverload<int>::of(&QComboBox::currentIndexChanged),
                     dlg, [&](int) { loadFunc(); });

    loadFunc();
    dlg->exec();
    dlg->deleteLater();
}

// ─────────────────────────────────────────────────────────────────────────────
//  Offline password reset requests: admin sets new password
// ─────────────────────────────────────────────────────────────────────────────
void userstaff::onAdminSetNewPassword(int requestId, int userId, const QString &username)
{
    QSqlDatabase db = ConnectionJasser::getInstance().getDatabase();
    if (!db.isOpen()) {
        QMessageBox::warning(this, "DB", "Base de données non connectée.");
        return;
    }

    auto *dlg = new QDialog(this);
    dlg->setWindowTitle("Définir un nouveau mot de passe — " + username);
    dlg->setModal(true);
    dlg->resize(400, 300);
    dlg->setStyleSheet(
        "QDialog { background: #f3f6f5; }"
        "QLabel { color: #374151; font-size: 13px; }"
        "QLineEdit { background: white; border: 1px solid #d1d5db; border-radius: 6px; padding: 7px 10px; font-size: 13px; }"
        "QLineEdit:focus { border: 2px solid #22c55e; }"
        "QCheckBox { color: #374151; font-size: 12px; }"
    );

    auto *root = new QVBoxLayout(dlg);
    root->setContentsMargins(24, 24, 24, 20);
    root->setSpacing(14);

    auto *info = new QLabel(
        "Définir un nouveau mot de passe temporaire pour :<br><b>" + username + "</b><br>"
        "L'utilisateur devra le changer à sa prochaine connexion.",
        dlg);
    info->setTextFormat(Qt::RichText);
    info->setWordWrap(true);
    root->addWidget(info);

    auto *sep = new QFrame(dlg);
    sep->setFixedHeight(1);
    sep->setStyleSheet("background: #e5e7eb;");
    root->addWidget(sep);

    auto *pwdRow = new QHBoxLayout();
    auto *lblPwd = new QLabel("Nouveau mot de passe :", dlg);
    auto *lePwd = new QLineEdit(dlg);
    lePwd->setPlaceholderText("Minimum 6 caractères");
    lePwd->setEchoMode(QLineEdit::Password);
    auto *btnGen = new QPushButton(QString::fromUtf8("\xe2\x9a\xa1 Générer"), dlg);
    btnGen->setCursor(Qt::PointingHandCursor);
    btnGen->setStyleSheet("QPushButton { background: #0284c7; color: white; border-radius: 5px; padding: 6px 10px; font-weight: 700; } QPushButton:hover { background: #0ea5e9; }");
    pwdRow->addWidget(lblPwd);
    pwdRow->addWidget(lePwd, 1);
    pwdRow->addWidget(btnGen);
    root->addLayout(pwdRow);

    auto *chkShow = new QCheckBox("Afficher le mot de passe", dlg);
    root->addWidget(chkShow);

    auto *lblErr = new QLabel(dlg);
    lblErr->setStyleSheet("color: #ef4444; font-size: 11px;");
    lblErr->setVisible(false);
    root->addWidget(lblErr);

    auto *btnRow = new QHBoxLayout();
    btnRow->addStretch();
    auto *btnCancel = new QPushButton("Annuler", dlg);
    btnCancel->setCursor(Qt::PointingHandCursor);
    btnCancel->setStyleSheet("QPushButton { background: #e5e7eb; color: #374151; border-radius: 6px; padding: 8px 14px; font-weight: 700; }");
    auto *btnConfirm = new QPushButton(QString::fromUtf8("\xe2\x9c\x85 Confirmer et notifier"), dlg);
    btnConfirm->setCursor(Qt::PointingHandCursor);
    btnConfirm->setStyleSheet("QPushButton { background: #16a34a; color: white; border-radius: 6px; padding: 8px 14px; font-weight: 800; } QPushButton:hover { background: #22c55e; }");
    btnRow->addWidget(btnCancel);
    btnRow->addWidget(btnConfirm);
    root->addLayout(btnRow);

    QObject::connect(btnCancel, &QPushButton::clicked, dlg, &QDialog::reject);
    QObject::connect(chkShow, &QCheckBox::toggled, dlg, [lePwd](bool checked) {
        lePwd->setEchoMode(checked ? QLineEdit::Normal : QLineEdit::Password);
    });
    QObject::connect(btnGen, &QPushButton::clicked, dlg, [lePwd]() {
        const QString chars = "ABCDEFGHJKLMNPQRSTUVWXYZabcdefghjkmnpqrstuvwxyz23456789";
        QString pwd;
        pwd.reserve(8);
        QRandomGenerator *rng = QRandomGenerator::global();
        for (int i = 0; i < 8; ++i) pwd += chars.at(rng->bounded(chars.length()));
        lePwd->setText(pwd);
        lePwd->setEchoMode(QLineEdit::Normal);
    });

    bool didUpdate = false;
    QObject::connect(btnConfirm, &QPushButton::clicked, dlg, [&]() {
        lblErr->setVisible(false);

        const QString plain = lePwd->text();
        if (plain.length() < 6) {
            lblErr->setText("Le mot de passe doit contenir au moins 6 caractères.");
            lblErr->setVisible(true);
            return;
        }

        const QString hashed = hashPassword(plain);

        QSqlQuery updEmp(db);
        updEmp.prepare(
            "UPDATE EMPLOYEES "
            "SET PASSWORD_HASH = :hash, "
            "    STATE = CASE WHEN STATE = 'LOCKED' THEN 'ACTIVE' ELSE STATE END, "
            "    FAILED_ATTEMPTS = 0, "
            "    LAST_ACTION = 'MOT_DE_PASSE_REINITIALISE', "
            "    LAST_ACTION_TIME = SYSTIMESTAMP "
            "WHERE USER_ID = :uid");
        updEmp.bindValue(":hash", hashed);
        updEmp.bindValue(":uid", userId);

        if (!updEmp.exec()) {
            lblErr->setText("Erreur SQL (EMPLOYEES): " + updEmp.lastError().text());
            lblErr->setVisible(true);
            return;
        }

        QSqlQuery updReq(db);
        updReq.prepare(
            "UPDATE PASSWORD_RESET_REQUESTS "
            "SET STATUS = 'RESOLVED', "
            "    RESOLVED_BY = :admin, "
            "    RESOLVED_TIME = SYSTIMESTAMP, "
            "    NEW_PASSWORD_SET = 1 "
            "WHERE REQUEST_ID = :rid");
        updReq.bindValue(":admin", m_currentUser);
        updReq.bindValue(":rid", requestId);
        if (!updReq.exec()) {
            lblErr->setText("Erreur SQL (REQUESTS): " + updReq.lastError().text());
            lblErr->setVisible(true);
            return;
        }

        // Optional AUDIT_LOG insert (skip if table does not exist)
        {
            QSqlQuery checkTable(db);
            const bool ok = checkTable.exec("SELECT COUNT(*) FROM AUDIT_LOG WHERE ROWNUM<=1");
            if (ok && !checkTable.lastError().isValid()) {
                QSqlQuery ins(db);
                ins.prepare(
                    "INSERT INTO AUDIT_LOG (USERNAME, EVENT_TYPE, DETAILS, PERFORMED_BY, HOSTNAME) "
                    "VALUES (:username, 'PASSWORD_CHANGED', :details, :admin, :host)");
                ins.bindValue(":username", username);
                ins.bindValue(":details", "Reset via admin panel — password reset request");
                ins.bindValue(":admin", m_currentUser);
                ins.bindValue(":host", QSysInfo::machineHostName());
                ins.exec();
            }
        }

        didUpdate = true;

        QMessageBox::information(dlg, "Mot de passe défini ✓",
                                 "✅ Nouveau mot de passe défini avec succès.\n\n"
                                 "👤 Utilisateur : " + username + "\n"
                                 "🔑 Mot de passe temporaire : " + plain + "\n\n"
                                 "⚠️ Communiquez ce mot de passe à l'utilisateur\n"
                                 "   en personne ou par téléphone.\n\n"
                                 "   L'utilisateur peut maintenant se connecter.");
        dlg->accept();
    });

    dlg->exec();
    dlg->deleteLater();

    if (!didUpdate) return;
    loadEmployees(ui->searchField->text(), ui->roleFilter->currentText(), ui->btnShowArchived->isChecked());
    updateStats();
}

void userstaff::logUserCreated(const QString &username)
{
    insertAuditEvent(username, "USER_CREATED", "User created", m_currentUser);
}

void userstaff::logUserArchived(const QString &username)
{
    insertAuditEvent(username, "USER_ARCHIVED", "User archived", m_currentUser);
}

void userstaff::logUserDeleted(const QString &username)
{
    insertAuditEvent(username, "USER_DELETED", "User deleted", m_currentUser);
}

int userstaff::getSelectedUserId()
{
    int row = ui->userTable->currentRow();
    if (row < 0) return -1;
    // USER_ID is stored in UserRole of col 0
    QTableWidgetItem *item = ui->userTable->item(row, 0);
    if (!item) return -1;
    return item->data(Qt::UserRole).toInt();
}

// ─────────────────────────────────────────────────────────────────────────────
//  Build dialog (shared for add & edit)
// ─────────────────────────────────────────────────────────────────────────────
QDialog* userstaff::buildEmployeeDialog(const QString &title,
                                        const QString &username,
                                        const QString &email,
                                        const QString &role,
                                        const QString &state)
{
    QDialog *dlg = new QDialog(this);
    dlg->setWindowTitle(title);
    dlg->setMinimumWidth(420);
    dlg->setStyleSheet(
        "QDialog { background-color: #f3f6f5; }"
        "QLabel  { color: #111827; font-size: 13px; font-weight: 600; }"
        "QLineEdit, QComboBox {"
        "   background: white; border: 1px solid #d1d5db;"
        "   border-radius: 6px; padding: 7px 10px; font-size: 13px; color: #111827; }"
        "QLineEdit:focus { border: 2px solid #22c55e; }"
        "QPushButton { border-radius: 6px; padding: 8px 20px; font-size: 13px; font-weight: 600; }"
        "QPushButton[text='Enregistrer'] { background-color: #16a34a; color: white; border: none; }"
        "QPushButton[text='Enregistrer']:hover { background-color: #22c55e; }"
        "QPushButton[text='Annuler'] { background-color: #e5e7eb; color: #374151; border: none; }"
    );

    auto *mainLayout = new QVBoxLayout(dlg);
    mainLayout->setSpacing(14);
    mainLayout->setContentsMargins(24, 24, 24, 20);

    // Title label
    auto *titleLbl = new QLabel(title, dlg);
    titleLbl->setStyleSheet("font-size: 16px; font-weight: bold; color: #2c3e2e; margin-bottom: 6px;");
    mainLayout->addWidget(titleLbl);

    // Form
    auto *form = new QFormLayout();
    form->setSpacing(12);
    form->setLabelAlignment(Qt::AlignRight | Qt::AlignVCenter);

    auto *leUsername = new QLineEdit(username, dlg);
    leUsername->setObjectName("leUsername");
    leUsername->setPlaceholderText("ex: jean.dupont");

    auto *leEmail = new QLineEdit(email, dlg);
    leEmail->setObjectName("leEmail");
    leEmail->setPlaceholderText("ex: jean@pressiq.com");

    auto *lePassword = new QLineEdit(dlg);
    lePassword->setObjectName("lePassword");
    lePassword->setEchoMode(QLineEdit::Password);
    lePassword->setPlaceholderText(username.isEmpty() ? "Mot de passe requis" : "Laisser vide pour conserver");

    auto *cbRole = new QComboBox(dlg);
    cbRole->setObjectName("cbRole");
    cbRole->addItems({"Admin", "Manager", "Opérateur"});
    cbRole->setCurrentText(role);

    auto *cbState = new QComboBox(dlg);
    cbState->setObjectName("cbState");
    cbState->addItems({"ACTIVE", "INACTIVE", "LOCKED"});
    cbState->setCurrentText(state);

    form->addRow("Nom d'utilisateur :", leUsername);
    form->addRow("Email :",             leEmail);
    form->addRow("Mot de passe :",      lePassword);
    form->addRow("Rôle :",              cbRole);
    form->addRow("État :",              cbState);

    mainLayout->addLayout(form);

    // Buttons
    auto *btnBox = new QHBoxLayout();
    btnBox->addStretch();
    auto *btnCancel = new QPushButton("Annuler", dlg);
    auto *btnSave   = new QPushButton("Enregistrer", dlg);
    btnBox->addWidget(btnCancel);
    btnBox->addWidget(btnSave);
    mainLayout->addLayout(btnBox);

    QObject::connect(btnCancel, &QPushButton::clicked, dlg, &QDialog::reject);
    QObject::connect(btnSave,   &QPushButton::clicked, dlg, &QDialog::accept);

    return dlg;
}

// ─────────────────────────────────────────────────────────────────────────────
//  Load employees into table
// ─────────────────────────────────────────────────────────────────────────────
void userstaff::loadEmployees(const QString &searchText, const QString &roleFilter, bool showArchived)
{
    QSqlDatabase db = ConnectionJasser::getInstance().getDatabase();
    if (!db.isOpen()) return;

    QString sql = "SELECT USER_ID, USERNAME, EMAIL, ROLE, STATE, "
                  "FAILED_ATTEMPTS, LAST_LOGIN, LAST_ACTION "
                  "FROM EMPLOYEES WHERE 1=1 ";

    if (!showArchived)
        sql += " AND NVL(STATE,'ACTIVE') != 'ARCHIVED' ";

    if (!searchText.isEmpty())
        sql += " AND (UPPER(USERNAME) LIKE UPPER(:search) OR UPPER(EMAIL) LIKE UPPER(:search2)) ";

    if (!roleFilter.isEmpty() && roleFilter != "Tous les rôles")
        sql += " AND UPPER(ROLE) = UPPER(:role) ";

    sql += " ORDER BY USERNAME";

    QSqlQuery q(db);
    q.prepare(sql);

    if (!searchText.isEmpty()) {
        q.bindValue(":search",  "%" + searchText + "%");
        q.bindValue(":search2", "%" + searchText + "%");
    }
    if (!roleFilter.isEmpty() && roleFilter != "Tous les rôles")
        q.bindValue(":role", roleFilter);

    ui->userTable->setRowCount(0);
    if (!q.exec()) {
        qDebug() << "[userstaff] loadEmployees error:" << q.lastError().text();
        return;
    }

    int row = 0;
    while (q.next()) {
        int    userId        = q.value(0).toInt();
        QString username     = q.value(1).toString();
        QString email        = q.value(2).toString();
        QString role         = q.value(3).toString();
        QString stateVal     = q.value(4).toString();
        int    failedAttempts= q.value(5).toInt();
        QString lastLogin    = q.value(6).isNull() ? "—" : q.value(6).toDateTime().toString("yyyy-MM-dd HH:mm");
        QString lastAction   = q.value(7).isNull() ? "—" : q.value(7).toString();

        ui->userTable->insertRow(row);

        // col 0 – Nom, stores USER_ID in UserRole
        auto *itemNom = new QTableWidgetItem(username);
        itemNom->setData(Qt::UserRole, userId);
        ui->userTable->setItem(row, 0, itemNom);

        ui->userTable->setItem(row, 1, new QTableWidgetItem(email));
        ui->userTable->setItem(row, 2, new QTableWidgetItem(role));

        // State badge colouring
        auto *itemState = new QTableWidgetItem(stateVal);
        if (stateVal == "ACTIVE")
            itemState->setForeground(QColor("#16a34a"));
        else if (stateVal == "LOCKED" || failedAttempts >= 3)
            itemState->setForeground(QColor("#dc2626"));
        else
            itemState->setForeground(QColor("#6b7280"));
        ui->userTable->setItem(row, 3, itemState);

        ui->userTable->setItem(row, 4, new QTableWidgetItem(QString::number(failedAttempts)));
        ui->userTable->setItem(row, 5, new QTableWidgetItem(lastLogin));
        ui->userTable->setItem(row, 6, new QTableWidgetItem(lastAction));

        // col 7 – per-row actions (unlock for locked accounts)
        ui->userTable->setCellWidget(row, 7, nullptr);
        if (stateVal == "LOCKED") {
            auto *btn = new QPushButton(QString::fromUtf8("\xf0\x9f\x94\x93 Unlock"));
            btn->setCursor(Qt::PointingHandCursor);
            btn->setFixedHeight(28);
            btn->setStyleSheet(
                "QPushButton { background: #16a34a; color: white; border: none; border-radius: 6px; font-size: 12px; font-weight: 600; padding: 4px 10px; }"
                "QPushButton:hover { background: #22c55e; }"
                "QPushButton:disabled { background: #9ca3af; color: white; }");
            btn->setEnabled(isAdmin());
            ui->userTable->setCellWidget(row, 7, btn);

            const int lockedUserId = userId;
            const QString lockedUsername = username;
            connect(btn, &QPushButton::clicked, this, [this, lockedUserId, lockedUsername]() {
                onUnlockAccount(lockedUserId, lockedUsername);
            });
        } else {
            auto *itemActions = new QTableWidgetItem("—");
            itemActions->setTextAlignment(Qt::AlignCenter);
            ui->userTable->setItem(row, 7, itemActions);
        }

        ++row;
    }
}

// ─────────────────────────────────────────────────────────────────────────────
//  Update stat cards
// ─────────────────────────────────────────────────────────────────────────────
void userstaff::updateStats()
{
    QSqlDatabase db = ConnectionJasser::getInstance().getDatabase();
    if (!db.isOpen()) return;

    auto runCount = [&](const QString &sql) -> int {
        QSqlQuery q(db);
        if (q.exec(sql) && q.next()) return q.value(0).toInt();
        return 0;
    };

    int total    = runCount("SELECT COUNT(*) FROM EMPLOYEES");
    int active   = runCount("SELECT COUNT(*) FROM EMPLOYEES "
                            "WHERE LAST_LOGIN >= SYSDATE - 1");
    int archived = runCount("SELECT COUNT(*) FROM EMPLOYEES "
                            "WHERE UPPER(NVL(STATE,'ACTIVE')) = 'ARCHIVED'");
    int locked   = runCount("SELECT COUNT(*) FROM EMPLOYEES "
                            "WHERE UPPER(NVL(STATE,'ACTIVE')) = 'LOCKED'");

    ui->cardTotalEmployeesValue->setText(QString::number(total));
    ui->cardActiveTodayValue->setText(QString::number(active));
    ui->cardArchivedUsersValue->setText(QString::number(archived));
    ui->cardAccountLockedValue->setText(QString::number(locked));
}

// ─────────────────────────────────────────────────────────────────────────────
//  SLOT – Table selection changed → enable/disable Modifier & Supprimer
// ─────────────────────────────────────────────────────────────────────────────
void userstaff::onTableSelectionChanged()
{
    bool hasSelection = ui->userTable->currentRow() >= 0;
    ui->btnModifier->setEnabled(hasSelection);
    ui->btnSupprimer->setEnabled(hasSelection && isAdmin());
}

// ─────────────────────────────────────────────────────────────────────────────
//  SLOT – Search
// ─────────────────────────────────────────────────────────────────────────────
void userstaff::onSearchTextChanged(const QString &text)
{
    QString role    = ui->roleFilter->currentText();
    bool archived   = ui->btnShowArchived->isChecked();
    loadEmployees(text, role, archived);
}

// ─────────────────────────────────────────────────────────────────────────────
//  SLOT – Role filter
// ─────────────────────────────────────────────────────────────────────────────
void userstaff::onRoleFilterChanged(const QString &role)
{
    QString search  = ui->searchField->text();
    bool archived   = ui->btnShowArchived->isChecked();
    loadEmployees(search, role, archived);
}

// ─────────────────────────────────────────────────────────────────────────────
//  SLOT – Show archived toggle
// ─────────────────────────────────────────────────────────────────────────────
void userstaff::onShowArchivedToggled(bool checked)
{
    QString search = ui->searchField->text();
    QString role   = ui->roleFilter->currentText();
    loadEmployees(search, role, checked);
}

// ─────────────────────────────────────────────────────────────────────────────
//  SLOT – Ajouter
// ─────────────────────────────────────────────────────────────────────────────
void userstaff::onAjouterClicked()
{
    QDialog *dlg = buildEmployeeDialog("Ajouter un employé");
    if (dlg->exec() != QDialog::Accepted) { delete dlg; return; }

    QString username = dlg->findChild<QLineEdit*>("leUsername")->text().trimmed();
    QString email    = dlg->findChild<QLineEdit*>("leEmail")->text().trimmed();
    QString password = dlg->findChild<QLineEdit*>("lePassword")->text();
    QString role     = dlg->findChild<QComboBox*>("cbRole")->currentText();
    QString state    = dlg->findChild<QComboBox*>("cbState")->currentText();
    delete dlg;

    if (username.isEmpty() || email.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Champs requis",
                             "Veuillez remplir tous les champs obligatoires.");
        return;
    }

    QSqlDatabase db = ConnectionJasser::getInstance().getDatabase();
    QSqlQuery q(db);
    q.prepare("INSERT INTO EMPLOYEES (USERNAME, EMAIL, PASSWORD_HASH, ROLE, STATE) "
              "VALUES (:u, :e, :p, :r, :s)");
    q.bindValue(":u", username);
    q.bindValue(":e", email);
    q.bindValue(":p", hashPassword(password));
    q.bindValue(":r", role);
    q.bindValue(":s", state);

    if (!q.exec()) {
        QMessageBox::critical(this, "Erreur",
                              "Impossible d'ajouter l'employé :\n" + q.lastError().text());
        return;
    }

    logUserCreated(username);

    QMessageBox::information(this, "Succès", "Employé ajouté avec succès.");
    loadEmployees();
    updateStats();
}

// ─────────────────────────────────────────────────────────────────────────────
//  SLOT – Modifier
// ─────────────────────────────────────────────────────────────────────────────
void userstaff::onModifierClicked()
{
    int userId = getSelectedUserId();
    if (userId < 0) return;

    // Fetch current values
    QSqlDatabase db = ConnectionJasser::getInstance().getDatabase();
    QSqlQuery q(db);
    q.prepare("SELECT USERNAME, EMAIL, ROLE, STATE FROM EMPLOYEES WHERE USER_ID = :id");
    q.bindValue(":id", userId);
    if (!q.exec() || !q.next()) {
        QMessageBox::warning(this, "Erreur", "Employé introuvable.");
        return;
    }
    QString curUsername = q.value(0).toString();
    QString curEmail    = q.value(1).toString();
    QString curRole     = q.value(2).toString();
    QString curState    = q.value(3).toString();

    QDialog *dlg = buildEmployeeDialog("Modifier un employé",
                                       curUsername, curEmail, curRole, curState);
    if (dlg->exec() != QDialog::Accepted) { delete dlg; return; }

    QString username = dlg->findChild<QLineEdit*>("leUsername")->text().trimmed();
    QString email    = dlg->findChild<QLineEdit*>("leEmail")->text().trimmed();
    QString password = dlg->findChild<QLineEdit*>("lePassword")->text();
    QString role     = dlg->findChild<QComboBox*>("cbRole")->currentText();
    QString state    = dlg->findChild<QComboBox*>("cbState")->currentText();
    delete dlg;

    if (username.isEmpty() || email.isEmpty()) {
        QMessageBox::warning(this, "Champs requis",
                             "Le nom et l'email sont obligatoires.");
        return;
    }

    QSqlQuery uq(db);
    if (password.isEmpty()) {
        uq.prepare("UPDATE EMPLOYEES SET USERNAME=:u, EMAIL=:e, ROLE=:r, STATE=:s "
                   "WHERE USER_ID=:id");
    } else {
        uq.prepare("UPDATE EMPLOYEES SET USERNAME=:u, EMAIL=:e, ROLE=:r, STATE=:s, "
                   "PASSWORD_HASH=:p WHERE USER_ID=:id");
        uq.bindValue(":p", hashPassword(password));
    }
    uq.bindValue(":u",  username);
    uq.bindValue(":e",  email);
    uq.bindValue(":r",  role);
    uq.bindValue(":s",  state);
    uq.bindValue(":id", userId);

    if (!uq.exec()) {
        QMessageBox::critical(this, "Erreur",
                              "Impossible de modifier l'employé :\n" + uq.lastError().text());
        return;
    }

    QMessageBox::information(this, "Succès", "Employé modifié avec succès.");
    loadEmployees(ui->searchField->text(),
                  ui->roleFilter->currentText(),
                  ui->btnShowArchived->isChecked());
    updateStats();
}

// ─────────────────────────────────────────────────────────────────────────────
//  SLOT – Supprimer
// ─────────────────────────────────────────────────────────────────────────────
void userstaff::onSupprimerClicked()
{
    if (!requireAdmin("Suppression / archivage")) return;

    int userId = getSelectedUserId();
    if (userId < 0) return;

    int row      = ui->userTable->currentRow();
    QString name = ui->userTable->item(row, 0)->text();

    QMessageBox mb(this);
    mb.setWindowTitle("Confirmer la suppression");
    mb.setText(QString("Voulez-vous archiver l'employé <b>%1</b> ?<br>"
                       "<small>L'enregistrement sera marqué comme ARCHIVED (soft-delete).</small>")
               .arg(name));
    mb.setIcon(QMessageBox::Warning);
    QPushButton *btnArchive = mb.addButton("Archiver", QMessageBox::AcceptRole);
    QPushButton *btnDelete  = mb.addButton("Supprimer définitivement", QMessageBox::DestructiveRole);
    mb.addButton("Annuler", QMessageBox::RejectRole);
    mb.exec();

    QSqlDatabase db = ConnectionJasser::getInstance().getDatabase();
    QSqlQuery q(db);

    if (mb.clickedButton() == btnArchive) {
        q.prepare("UPDATE EMPLOYEES SET STATE='ARCHIVED' WHERE USER_ID=:id");
        q.bindValue(":id", userId);
    } else if (mb.clickedButton() == btnDelete) {
        q.prepare("DELETE FROM EMPLOYEES WHERE USER_ID=:id");
        q.bindValue(":id", userId);
    } else {
        return; // Annuler
    }

    if (!q.exec()) {
        QMessageBox::critical(this, "Erreur",
                              "Opération échouée :\n" + q.lastError().text());
        return;
    }

    QMessageBox::information(this, "Succès",
                             (mb.clickedButton() == btnArchive)
                             ? "Employé archivé."
                             : "Employé supprimé.");

    if (mb.clickedButton() == btnArchive) {
        logUserArchived(name);
    } else if (mb.clickedButton() == btnDelete) {
        logUserDeleted(name);
    }

    loadEmployees(ui->searchField->text(),
                  ui->roleFilter->currentText(),
                  ui->btnShowArchived->isChecked());
    updateStats();
}

// ─────────────────────────────────────────────────────────────────────────────
//  SLOT – Exporter (PDF)
// ─────────────────────────────────────────────────────────────────────────────
void userstaff::onExporterClicked()
{
    QString filePath = QFileDialog::getSaveFileName(
        this, "Exporter la liste des employés",
        QDir::homePath() + "/employees.pdf",
        "PDF Files (*.pdf)");
    if (filePath.isEmpty()) return;

    // Fetch all active employees
    QSqlDatabase db = ConnectionJasser::getInstance().getDatabase();
    QSqlQuery q(db);
    q.exec("SELECT USERNAME, EMAIL, ROLE, STATE, FAILED_ATTEMPTS, "
           "TO_CHAR(LAST_LOGIN,'YYYY-MM-DD HH24:MI'), "
           "TO_CHAR(CREATED_AT,'YYYY-MM-DD') "
           "FROM EMPLOYEES ORDER BY USERNAME");

    // Collect rows
    struct Row { QString username, email, role, state; int failed; QString lastLogin, created; };
    QList<Row> rows;
    while (q.next()) {
        Row r;
        r.username  = q.value(0).toString();
        r.email     = q.value(1).toString();
        r.role      = q.value(2).toString();
        r.state     = q.value(3).toString();
        r.failed    = q.value(4).toInt();
        r.lastLogin = q.value(5).isNull() ? "—" : q.value(5).toString();
        r.created   = q.value(6).isNull() ? "—" : q.value(6).toString();
        rows << r;
    }

    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(filePath);
    printer.setPageOrientation(QPageLayout::Landscape);
    printer.setPageSize(QPageSize::A4);
    printer.setPageMargins(QMarginsF(15, 15, 15, 15), QPageLayout::Millimeter);

    QPainter painter;
    if (!painter.begin(&printer)) {
        QMessageBox::critical(this, "Erreur", "Impossible de créer le fichier PDF.");
        return;
    }

    const QRect page = painter.viewport();
    const int W = page.width();

    // Background
    painter.fillRect(page, QColor("#f3f6f5"));

    // Header bar
    painter.fillRect(0, 0, W, 160, QColor("#2c3e2e"));

    // Title
    QFont fontTitle("Segoe UI", 22, QFont::Bold);
    painter.setFont(fontTitle);
    painter.setPen(Qt::white);
    painter.drawText(QRect(40, 20, W - 80, 80), Qt::AlignLeft | Qt::AlignVCenter,
                     "PRESSIQ — User & Staff Management");

    QFont fontSub("Segoe UI", 12);
    painter.setFont(fontSub);
    painter.setPen(QColor("#d4a63a"));
    painter.drawText(QRect(40, 90, W - 80, 50), Qt::AlignLeft | Qt::AlignVCenter,
                     QString("Rapport généré le %1  •  %2 employé(s)")
                     .arg(QDate::currentDate().toString("dd/MM/yyyy"))
                     .arg(rows.size()));

    // Table header
    const int top      = 200;
    const int rowH     = 75;
    const int colCount = 7;
    int colW[7] = {0};
    colW[0] = W * 18 / 100;   // Nom
    colW[1] = W * 24 / 100;   // Email
    colW[2] = W * 12 / 100;   // Rôle
    colW[3] = W * 10 / 100;   // État
    colW[4] = W * 8  / 100;   // Failed
    colW[5] = W * 16 / 100;   // Last Login
    colW[6] = W * 12 / 100;   // Créé le

    const QStringList headers = {"Nom", "Email", "Rôle", "État",
                                  "Tentatives", "Last Login", "Créé le"};

    painter.fillRect(0, top, W, rowH, QColor("#2c3e2e"));
    QFont fontHdr("Segoe UI", 11, QFont::Bold);
    painter.setFont(fontHdr);
    painter.setPen(Qt::white);

    int x = 0;
    for (int c = 0; c < colCount; ++c) {
        painter.drawText(QRect(x + 15, top, colW[c] - 15, rowH),
                         Qt::AlignLeft | Qt::AlignVCenter, headers[c]);
        x += colW[c];
    }

    // Rows
    QFont fontRow("Segoe UI", 10);
    painter.setFont(fontRow);
    int y = top + rowH;
    bool alt = false;

    for (const Row &r : rows) {
        // row background
        QColor bg = alt ? QColor("#f9fafb") : Qt::white;
        painter.fillRect(0, y, W, rowH, bg);

        // bottom line
        painter.setPen(QColor("#e5e7eb"));
        painter.drawLine(0, y + rowH - 1, W, y + rowH - 1);

        painter.setPen(QColor("#111827"));
        QStringList vals = {r.username, r.email, r.role, r.state,
                            QString::number(r.failed), r.lastLogin, r.created};
        int cx = 0;
        for (int c = 0; c < colCount; ++c) {
            // state badge colour
            if (c == 3) {
                if (r.state == "ACTIVE")       painter.setPen(QColor("#16a34a"));
                else if (r.state == "LOCKED"
                         || r.failed >= 3)     painter.setPen(QColor("#dc2626"));
                else                           painter.setPen(QColor("#6b7280"));
            } else {
                painter.setPen(QColor("#111827"));
            }
            painter.drawText(QRect(cx + 15, y, colW[c] - 15, rowH),
                             Qt::AlignLeft | Qt::AlignVCenter,
                             painter.fontMetrics().elidedText(vals[c], Qt::ElideRight, colW[c] - 20));
            cx += colW[c];
        }

        y += rowH;
        alt = !alt;

        // new page if needed
        if (y + rowH > page.height() - 60) {
            printer.newPage();
            y = 40;
            alt = false;
        }
    }

    // Footer
    painter.setPen(QColor("#6b7280"));
    QFont fontFoot("Segoe UI", 9);
    painter.setFont(fontFoot);
    painter.drawText(QRect(0, page.height() - 55, W, 55),
                     Qt::AlignCenter,
                     "PressIQ © Smart Oil Systems — Confidentiel");

    painter.end();
    QMessageBox::information(this, "Export réussi",
                             QString("PDF exporté avec succès :\n%1").arg(filePath));
}

// ─────────────────────────────────────────────────────────────────────────────
//  FEATURE 2 – Unlock account with audit trail
// ─────────────────────────────────────────────────────────────────────────────
void userstaff::onUnlockAccount(int userId, const QString &username)
{
    if (!requireAdmin("Déverrouillage de compte")) return;

    auto *dlg = new QDialog(this);
    dlg->setWindowTitle("Déverrouiller le compte");
    dlg->setModal(true);
    dlg->setMinimumWidth(520);
    dlg->setStyleSheet(
        "QDialog { background-color: #f3f6f5; }"
        "QLabel  { color: #111827; font-size: 13px; font-weight: 600; }"
        "QLineEdit { background: white; border: 1px solid #d1d5db; border-radius: 6px; padding: 7px 10px; font-size: 13px; color: #111827; }"
        "QLineEdit:focus { border: 2px solid #22c55e; }"
        "QCheckBox { color: #111827; font-size: 12px; }"
        "QPushButton { border-radius: 6px; padding: 8px 18px; font-size: 13px; font-weight: 600; }"
        "QPushButton#btnUnlock { background-color: #16a34a; color: white; border: none; }"
        "QPushButton#btnUnlock:hover { background-color: #22c55e; }"
        "QPushButton#btnCancel { background-color: #e5e7eb; color: #374151; border: none; }");

    auto *main = new QVBoxLayout(dlg);
    main->setContentsMargins(20, 18, 20, 18);
    main->setSpacing(12);

    auto *header = new QFrame(dlg);
    header->setStyleSheet("QFrame { background: #2c3e2e; border-radius: 10px; }");
    auto *headerLay = new QVBoxLayout(header);
    headerLay->setContentsMargins(14, 12, 14, 12);
    auto *title = new QLabel("Déverrouiller le compte", header);
    title->setStyleSheet("color: white; font-size: 16px; font-weight: 800;");
    auto *sub = new QLabel("Compte: " + username + " (verrouillé)", header);
    sub->setStyleSheet("color: rgba(255,255,255,210); font-size: 12px; font-weight: 600;");
    headerLay->addWidget(title);
    headerLay->addWidget(sub);
    main->addWidget(header);

    auto *leReason = new QLineEdit(dlg);
    leReason->setPlaceholderText("Raison du déverrouillage (obligatoire)");
    main->addWidget(leReason);

    auto *chkForce = new QCheckBox("Forcer la réinitialisation du mot de passe", dlg);
    main->addWidget(chkForce);

    auto *btnRow = new QHBoxLayout();
    btnRow->addStretch();
    auto *btnCancel = new QPushButton("Annuler", dlg);
    btnCancel->setObjectName("btnCancel");
    auto *btnUnlock = new QPushButton("Déverrouiller", dlg);
    btnUnlock->setObjectName("btnUnlock");
    btnRow->addWidget(btnCancel);
    btnRow->addWidget(btnUnlock);
    main->addLayout(btnRow);

    connect(btnCancel, &QPushButton::clicked, dlg, &QDialog::reject);
    connect(btnUnlock, &QPushButton::clicked, this, [=]() {
        if (leReason->text().trimmed().isEmpty()) {
            QMessageBox::warning(dlg, "Raison requise", "Veuillez saisir une raison.");
            return;
        }
        dlg->accept();
    });

    if (dlg->exec() != QDialog::Accepted) {
        dlg->deleteLater();
        return;
    }

    const QString reason = leReason->text().trimmed();
    const bool forceReset = chkForce->isChecked();
    dlg->deleteLater();

    QSqlDatabase db = ConnectionJasser::getInstance().getDatabase();
    if (!db.isOpen()) return;

    QSqlQuery upd(db);
    upd.prepare(
        "UPDATE EMPLOYEES SET STATE='ACTIVE', FAILED_ATTEMPTS=0, "
        "LAST_ACTION='DEBLOQUE_PAR_' || :admin, "
        "LAST_ACTION_TIME=SYSTIMESTAMP "
        "WHERE USER_ID=:id");
    upd.bindValue(":admin", m_currentUser);
    upd.bindValue(":id", userId);
    if (!upd.exec()) {
        QMessageBox::critical(this, "Erreur", "Déverrouillage échoué :\n" + upd.lastError().text());
        return;
    }

    insertAuditEvent(username, "ACCOUNT_UNLOCKED", reason, m_currentUser);

    if (forceReset) {
        const QString tempPwd = generateTempPassword(8);
        QSqlQuery pw(db);
        pw.prepare("UPDATE EMPLOYEES SET PASSWORD_HASH=:p WHERE USER_ID=:id");
        pw.bindValue(":p", hashPassword(tempPwd));
        pw.bindValue(":id", userId);
        if (!pw.exec()) {
            QMessageBox::warning(this, "Mot de passe", "Compte déverrouillé, mais reset échoué :\n" + pw.lastError().text());
        } else {
            insertAuditEvent(username, "PASSWORD_CHANGED", "Forced reset by admin", m_currentUser);
            QMessageBox::information(this, "Déverrouillage",
                                     "Compte déverrouillé. Mot de passe temporaire: " + tempPwd);
        }
    } else {
        QMessageBox::information(this, "Déverrouillage", "Compte déverrouillé avec succès.");
    }

    loadEmployees(ui->searchField->text(), ui->roleFilter->currentText(), ui->btnShowArchived->isChecked());
    updateStats();
}

// ─────────────────────────────────────────────────────────────────────────────
//  FEATURE 3 – Audit log viewer
// ─────────────────────────────────────────────────────────────────────────────
void userstaff::onShowAuditLog()
{
    QSqlDatabase db = ConnectionJasser::getInstance().getDatabase();
    if (!db.isOpen()) {
        QMessageBox::warning(this, "DB", "Base de données non connectée.");
        return;
    }

    auto *dlg = new QDialog(this);
    dlg->setWindowTitle(QString::fromUtf8("\xf0\x9f\x93\x8b Journal d'Audit de Sécurité — PressIQ"));
    dlg->setModal(true);
    dlg->resize(800, 550);
    dlg->setStyleSheet("QDialog { background: #f3f6f5; }");

    auto *root = new QVBoxLayout(dlg);
    root->setContentsMargins(14, 14, 14, 14);
    root->setSpacing(10);

    auto *hdr = new QFrame(dlg);
    hdr->setStyleSheet("QFrame { background: #2c3e2e; border-radius: 10px; }");
    auto *hdrLay = new QHBoxLayout(hdr);
    hdrLay->setContentsMargins(14, 12, 14, 12);
    auto *lbl = new QLabel(QString::fromUtf8("\xf0\x9f\x93\x8b  Journal d'Audit de Sécurité — PressIQ"), hdr);
    lbl->setStyleSheet("color: white; font-size: 15px; font-weight: 800;");
    hdrLay->addWidget(lbl);
    hdrLay->addStretch();
    root->addWidget(hdr);

    // Filter bar
    auto *bar = new QHBoxLayout();
    bar->setSpacing(8);
    auto *cbEvent = new QComboBox(dlg);
    cbEvent->addItems({"Tous", "LOGIN_SUCCESS", "LOGIN_FAIL", "ACCOUNT_LOCKED", "ACCOUNT_UNLOCKED",
                       "PASSWORD_CHANGED", "USER_CREATED", "USER_ARCHIVED", "USER_DELETED"});
    auto *leUser = new QLineEdit(dlg);
    leUser->setPlaceholderText("Filtrer par utilisateur...");
    auto *dateFrom = new QDateEdit(QDate::currentDate().addDays(-30), dlg);
    dateFrom->setCalendarPopup(true);
    auto *dateTo = new QDateEdit(QDate::currentDate(), dlg);
    dateTo->setCalendarPopup(true);
    auto *btnRefresh = new QPushButton(QString::fromUtf8("\xf0\x9f\x94\x84 Actualiser"), dlg);
    auto *btnExport = new QPushButton(QString::fromUtf8("\xf0\x9f\x93\xa5 Exporter PDF"), dlg);

    const QString filterQss =
        "QComboBox, QLineEdit, QDateEdit { background: white; border: 1px solid #d1d5db; border-radius: 6px; padding: 6px 10px; font-size: 12px; }";
    cbEvent->setStyleSheet(filterQss);
    leUser->setStyleSheet(filterQss);
    dateFrom->setStyleSheet(filterQss);
    dateTo->setStyleSheet(filterQss);
    btnRefresh->setStyleSheet("QPushButton { background: #16a34a; color: white; border: none; border-radius: 6px; padding: 7px 12px; font-size: 12px; font-weight: 700; } QPushButton:hover { background: #22c55e; }");
    btnExport->setStyleSheet("QPushButton { background: #0284c7; color: white; border: none; border-radius: 6px; padding: 7px 12px; font-size: 12px; font-weight: 700; } QPushButton:hover { background: #0ea5e9; }");

    bar->addWidget(cbEvent);
    bar->addWidget(leUser, 1);
    bar->addWidget(dateFrom);
    bar->addWidget(dateTo);
    bar->addWidget(btnRefresh);
    bar->addWidget(btnExport);
    root->addLayout(bar);

    auto *table = new QTableWidget(dlg);
    table->setColumnCount(6);
    table->setHorizontalHeaderLabels({"Date/Heure", "Utilisateur", "Type d'événement", "Détails", "Effectué par", "Machine"});
    table->horizontalHeader()->setStretchLastSection(true);
    table->setColumnWidth(0, 150);
    table->setColumnWidth(1, 120);
    table->setColumnWidth(2, 130);
    table->setColumnWidth(3, 200);
    table->setColumnWidth(4, 120);
    table->setColumnWidth(5, 100);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setAlternatingRowColors(true);
    root->addWidget(table, 1);

    auto colorForEvent = [&](const QString &eventType) -> QColor {
        if (eventType == "LOGIN_SUCCESS") return QColor("#16a34a");
        if (eventType == "LOGIN_FAIL") return QColor("#ea580c");
        if (eventType == "ACCOUNT_LOCKED") return QColor("#dc2626");
        if (eventType == "ACCOUNT_UNLOCKED") return QColor("#2563eb");
        if (eventType == "PASSWORD_CHANGED") return QColor("#7c3aed");
        return QColor("#6b7280");
    };

    auto refresh = [&]() {
        const QString event = cbEvent->currentText();
        const QString user = leUser->text().trimmed();
        const QDateTime fromDT(dateFrom->date(), QTime(0, 0, 0));
        const QDateTime toDT(dateTo->date(), QTime(23, 59, 59));

        QString sql =
            "SELECT LOG_TIME, USERNAME, EVENT_TYPE, DETAILS, PERFORMED_BY, HOSTNAME "
            "FROM AUDIT_LOG WHERE LOG_TIME BETWEEN :from AND :to ";
        if (event != "Tous") sql += " AND EVENT_TYPE = :event ";
        if (!user.isEmpty()) sql += " AND UPPER(USERNAME) LIKE UPPER(:user) ";
        sql += " ORDER BY LOG_TIME DESC FETCH FIRST 500 ROWS ONLY";

        QSqlQuery q(db);
        q.prepare(sql);
        q.bindValue(":from", fromDT);
        q.bindValue(":to", toDT);
        if (event != "Tous") q.bindValue(":event", event);
        if (!user.isEmpty()) q.bindValue(":user", "%" + user + "%");

        table->setRowCount(0);
        if (!q.exec()) {
            QMessageBox::warning(dlg, "Audit", "Erreur: " + q.lastError().text());
            return;
        }

        int r = 0;
        while (q.next()) {
            const QDateTime t = q.value(0).toDateTime();
            const QString u = q.value(1).toString();
            const QString type = q.value(2).toString();
            const QString det = q.value(3).toString();
            const QString by = q.value(4).toString();
            const QString host = q.value(5).toString();

            table->insertRow(r);
            auto mk = [&](int c, const QString &text) {
                auto *it = new QTableWidgetItem(text);
                it->setForeground(colorForEvent(type));
                table->setItem(r, c, it);
            };

            mk(0, t.isValid() ? t.toString("yyyy-MM-dd HH:mm") : "—");
            mk(1, u);
            mk(2, type);
            mk(3, det);
            mk(4, by.isEmpty() ? "—" : by);
            mk(5, host);

            if (type == "ACCOUNT_LOCKED") {
                for (int c = 0; c < 6; ++c) {
                    if (auto *it = table->item(r, c)) it->setFont(QFont(it->font().family(), it->font().pointSize(), QFont::Bold));
                }
            }
            ++r;
        }
    };

    auto exportPdf = [&]() {
        const QString savePath = QFileDialog::getSaveFileName(dlg, "Exporter le journal d'audit",
                                                              QDir::homePath() + "/audit_log.pdf",
                                                              "PDF Files (*.pdf)");
        if (savePath.isEmpty()) return;

        QPrinter printer(QPrinter::HighResolution);
        printer.setOutputFormat(QPrinter::PdfFormat);
        printer.setOutputFileName(savePath);
        printer.setPageOrientation(QPageLayout::Portrait);
        printer.setPageSize(QPageSize::A4);
        printer.setPageMargins(QMarginsF(15, 15, 15, 15), QPageLayout::Millimeter);

        QPainter p;
        if (!p.begin(&printer)) {
            QMessageBox::warning(dlg, "PDF", "Impossible de créer le PDF.");
            return;
        }

        const QRect page = p.viewport();
        const int W = page.width();
        const int H = page.height();

        auto drawHeader = [&]() {
            p.fillRect(0, 0, W, 110, QColor("#2c3e2e"));
            p.setPen(Qt::white);
            p.setFont(QFont("Segoe UI", 16, QFont::Bold));
            p.drawText(QRect(30, 20, W - 60, 40), Qt::AlignLeft | Qt::AlignVCenter,
                       "PressIQ — Journal d'Audit de Sécurité");
            p.setFont(QFont("Segoe UI", 10));
            p.setPen(QColor("#d4a63a"));
            p.drawText(QRect(30, 60, W - 60, 40), Qt::AlignLeft | Qt::AlignVCenter,
                       QString("Généré le %1 par %2")
                           .arg(QDateTime::currentDateTime().toString("dd/MM/yyyy HH:mm"))
                           .arg(m_currentUser));
        };

        drawHeader();

        const int top = 130;
        const int rowH = 24;
        int y = top;

        QStringList headers = {"Date/Heure", "Utilisateur", "Type", "Détails", "Effectué par", "Machine"};
        QVector<int> colW = {150, 110, 110, 210, 110, W - (150 + 110 + 110 + 210 + 110) - 20};
        if (colW.last() < 80) colW.last() = 80;

        auto drawTableHeader = [&]() {
            p.fillRect(0, y, W, rowH, QColor("#2c3e2e"));
            p.setPen(Qt::white);
            p.setFont(QFont("Segoe UI", 10, QFont::Bold));
            int x = 10;
            for (int c = 0; c < headers.size(); ++c) {
                p.drawText(QRect(x + 6, y, colW[c] - 8, rowH), Qt::AlignLeft | Qt::AlignVCenter, headers[c]);
                x += colW[c];
            }
            y += rowH;
        };

        drawTableHeader();
        p.setFont(QFont("Segoe UI", 9));

        auto nextPage = [&]() {
            printer.newPage();
            p.fillRect(page, QColor("#f3f6f5"));
            drawHeader();
            y = top;
            drawTableHeader();
        };

        for (int r = 0; r < table->rowCount(); ++r) {
            if (y + rowH > H - 50) nextPage();
            const bool alt = (r % 2) == 1;
            p.fillRect(0, y, W, rowH, alt ? QColor("#f9fafb") : Qt::white);

            const QString type = table->item(r, 2) ? table->item(r, 2)->text() : QString();
            p.setPen(colorForEvent(type));

            int x = 10;
            for (int c = 0; c < 6; ++c) {
                const QString txt = table->item(r, c) ? table->item(r, c)->text() : QString();
                p.drawText(QRect(x + 6, y, colW[c] - 10, rowH), Qt::AlignLeft | Qt::AlignVCenter,
                           p.fontMetrics().elidedText(txt, Qt::ElideRight, colW[c] - 12));
                x += colW[c];
            }
            y += rowH;
        }

        p.end();
        QMessageBox::information(dlg, "Export", "PDF exporté :\n" + savePath);
    };

    connect(btnRefresh, &QPushButton::clicked, dlg, refresh);
    connect(btnExport, &QPushButton::clicked, dlg, exportPdf);
    connect(cbEvent, &QComboBox::currentTextChanged, dlg, [&](const QString &) { refresh(); });
    connect(leUser, &QLineEdit::textChanged, dlg, [&](const QString &) { /* manual refresh */ });

    refresh();
    dlg->exec();
    dlg->deleteLater();
}

// ─────────────────────────────────────────────────────────────────────────────
//  FEATURE 4 – Analytics dashboard
// ─────────────────────────────────────────────────────────────────────────────
void userstaff::onShowAnalytics()
{
    QSqlDatabase db = ConnectionJasser::getInstance().getDatabase();
    if (!db.isOpen()) {
        QMessageBox::warning(this, "DB", "Base de données non connectée.");
        return;
    }

    auto *dlg = new QDialog(this);
    dlg->setWindowTitle(QString::fromUtf8("\xf0\x9f\x93\x8a Tableau de Bord Analytique — Sécurité des Comptes"));
    dlg->setModal(true);
    dlg->resize(900, 600);
    dlg->setStyleSheet("QDialog { background: #f3f6f5; }");

    auto *layout = new QVBoxLayout(dlg);
    layout->setContentsMargins(12, 12, 12, 12);
    layout->setSpacing(10);

    auto *hdr = new QFrame(dlg);
    hdr->setStyleSheet("QFrame { background: #2c3e2e; border-radius: 10px; }");
    auto *hl = new QHBoxLayout(hdr);
    hl->setContentsMargins(14, 12, 14, 12);
    auto *t = new QLabel(QString::fromUtf8("\xf0\x9f\x93\x8a  Tableau de Bord Analytique — Sécurité des Comptes"), hdr);
    t->setStyleSheet("color: white; font-size: 15px; font-weight: 800;");
    hl->addWidget(t);
    hl->addStretch();
    layout->addWidget(hdr);

    auto *widget = new AnalyticsWidget(db, dlg);
    widget->refreshData();
    widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    layout->addWidget(widget, 1);

    auto *btnClose = new QPushButton("Fermer", dlg);
    btnClose->setStyleSheet("QPushButton { background: #e5e7eb; color: #374151; border: none; border-radius: 6px; padding: 8px 18px; font-weight: 700; }"
                            "QPushButton:hover { background: #d1d5db; }");
    connect(btnClose, &QPushButton::clicked, dlg, &QDialog::accept);
    layout->addWidget(btnClose, 0, Qt::AlignRight);

    dlg->exec();
    dlg->deleteLater();
}

// ─────────────────────────────────────────────────────────────────────────────
//  FEATURE 5 – Security PDF report
// ─────────────────────────────────────────────────────────────────────────────
void userstaff::onExportSecurityReport()
{
    QSqlDatabase db = ConnectionJasser::getInstance().getDatabase();
    if (!db.isOpen()) {
        QMessageBox::warning(this, "DB", "Base de données non connectée.");
        return;
    }

    const QString filePath = QFileDialog::getSaveFileName(
        this,
        "Exporter le rapport de sécurité",
        QDir::homePath() + "/security_report.pdf",
        "PDF Files (*.pdf)");
    if (filePath.isEmpty()) return;

    struct DS1Row { QString u, e, r; int failed = 0; QString lockedAt; };
    struct DS2Row { QString u, e, r; QString lastLogin; QString created; };
    struct DS4Row { QString u, e, r; int failed = 0; };
    struct DS5Row { QString u, e, r, st, created; };

    QList<DS1Row> ds1;
    QList<DS2Row> ds2;
    QList<QPair<QString,int>> ds3;
    QList<DS4Row> ds4;
    QList<DS5Row> ds5;

    {
        QSqlQuery q(db);
        q.exec("SELECT USERNAME, EMAIL, ROLE, FAILED_ATTEMPTS, TO_CHAR(LAST_ACTION_TIME,'YYYY-MM-DD HH24:MI') "
               "FROM EMPLOYEES WHERE STATE='LOCKED' ORDER BY LAST_ACTION_TIME DESC");
        while (q.next()) {
            DS1Row r; r.u=q.value(0).toString(); r.e=q.value(1).toString(); r.r=q.value(2).toString();
            r.failed=q.value(3).toInt(); r.lockedAt=q.value(4).toString();
            if (r.lockedAt.isEmpty()) r.lockedAt = "—";
            ds1 << r;
        }
    }
    {
        QSqlQuery q(db);
        q.exec("SELECT USERNAME, EMAIL, ROLE, TO_CHAR(LAST_LOGIN,'YYYY-MM-DD HH24:MI'), TO_CHAR(CREATED_AT,'YYYY-MM-DD') "
               "FROM EMPLOYEES WHERE (LAST_LOGIN IS NULL OR LAST_LOGIN < SYSDATE - 30) "
               "AND NVL(STATE,'ACTIVE') NOT IN ('ARCHIVED','LOCKED') "
               "ORDER BY LAST_LOGIN NULLS FIRST");
        while (q.next()) {
            DS2Row r; r.u=q.value(0).toString(); r.e=q.value(1).toString(); r.r=q.value(2).toString();
            r.lastLogin=q.value(3).toString(); r.created=q.value(4).toString();
            if (r.lastLogin.isEmpty()) r.lastLogin = "—";
            if (r.created.isEmpty()) r.created = "—";
            ds2 << r;
        }
    }
    {
        QSqlQuery q(db);
        q.exec("SELECT EVENT_TYPE, COUNT(*) AS CNT FROM AUDIT_LOG "
               "WHERE LOG_TIME >= SYSDATE - 30 GROUP BY EVENT_TYPE ORDER BY CNT DESC");
        while (q.next()) {
            ds3 << qMakePair(q.value(0).toString(), q.value(1).toInt());
        }
    }
    {
        QSqlQuery q(db);
        q.exec("SELECT USERNAME, EMAIL, ROLE, FAILED_ATTEMPTS "
               "FROM EMPLOYEES WHERE FAILED_ATTEMPTS >= 2 AND NVL(STATE,'ACTIVE') != 'LOCKED' "
               "ORDER BY FAILED_ATTEMPTS DESC");
        while (q.next()) {
            DS4Row r; r.u=q.value(0).toString(); r.e=q.value(1).toString(); r.r=q.value(2).toString(); r.failed=q.value(3).toInt();
            ds4 << r;
        }
    }
    {
        QSqlQuery q(db);
        q.exec("SELECT USERNAME, EMAIL, ROLE, STATE, TO_CHAR(CREATED_AT,'YYYY-MM-DD') "
               "FROM EMPLOYEES WHERE CREATED_AT >= SYSDATE - 7 ORDER BY CREATED_AT DESC");
        while (q.next()) {
            DS5Row r; r.u=q.value(0).toString(); r.e=q.value(1).toString(); r.r=q.value(2).toString(); r.st=q.value(3).toString(); r.created=q.value(4).toString();
            if (r.st.isEmpty()) r.st = "ACTIVE";
            if (r.created.isEmpty()) r.created = "—";
            ds5 << r;
        }
    }

    const int lockedCount = ds1.size();
    const int inactiveCount = ds2.size();
    const int riskCount = lockedCount + ds4.size();

    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(filePath);
    printer.setPageOrientation(QPageLayout::Portrait);
    printer.setPageSize(QPageSize::A4);
    printer.setPageMargins(QMarginsF(15, 15, 15, 15), QPageLayout::Millimeter);

    // Layout constants (device units)
    const int headerH = 120;
    const int footerH = 42;
    const int sectionBarH = 26;
    const int rowH = 22;

    auto simulatePages = [&]() -> int {
        // Simulate pages with the same pagination rules used in the draw pass.
        // We keep it simple but consistent: page1 has header+summary boxes, then DS1/DS2 tables;
        // next page starts DS3 chart + DS4 + DS5.
        const QRect page = printer.pageLayout().paintRectPixels(printer.resolution());

        const int H = page.height();
        const int topStart = headerH + 15;
        const int maxY = H - footerH;

        int pages = 1;
        int y = topStart + 85; // reserve for summary boxes on first page

        auto paginateTable = [&](int rowsNeeded, int headerExtra) {
            int rowsLeft = rowsNeeded;
            int yy = y + headerExtra;
            while (rowsLeft > 0) {
                const int space = maxY - yy;
                const int canFit = qMax(0, space / rowH);
                if (canFit <= 0) {
                    ++pages;
                    yy = topStart + headerExtra;
                    continue;
                }
                const int take = qMin(canFit, rowsLeft);
                rowsLeft -= take;
                yy += take * rowH;
                if (rowsLeft > 0) {
                    ++pages;
                    yy = topStart + headerExtra;
                }
            }
            y = qMin(yy, maxY);
        };

        // DS1
        paginateTable(ds1.size(), sectionBarH + rowH);
        y += 14;
        // DS2
        paginateTable(ds2.size(), sectionBarH + rowH);

        // New page for analytics sections
        ++pages;
        y = topStart;

        // DS3 chart fixed height
        y += sectionBarH + 220 + 18;

        // DS4
        paginateTable(ds4.size(), sectionBarH + rowH);
        y += 14;
        // DS5
        paginateTable(ds5.size(), sectionBarH + rowH);

        return pages;
    };

    const int totalPages = simulatePages();

    QPainter painter;
    if (!painter.begin(&printer)) {
        QMessageBox::critical(this, "Erreur", "Impossible de créer le fichier PDF.");
        return;
    }

    const QRect page = painter.viewport();
    const int W = page.width();
    const int H = page.height();

    int pageIndex = 1;
    int y = 0;

    auto drawFooter = [&]() {
        painter.setPen(QColor("#6b7280"));
        painter.setFont(QFont("Segoe UI", 9));
        painter.drawText(QRect(0, H - footerH, W, footerH), Qt::AlignCenter,
                         QString("PressIQ © Smart Oil Systems — CONFIDENTIEL — %1/%2")
                             .arg(pageIndex).arg(totalPages));
    };

    auto drawHeader = [&]() {
        painter.fillRect(0, 0, W, headerH, QColor("#2c3e2e"));
        painter.setPen(Qt::white);
        painter.setFont(QFont("Segoe UI", 18, QFont::Bold));
        painter.drawText(QRect(30, 20, W - 60, 40), Qt::AlignLeft | Qt::AlignVCenter,
                         "PressIQ — RAPPORT DE SÉCURITÉ");
        painter.setFont(QFont("Segoe UI", 10));
        painter.setPen(QColor("#d4a63a"));
        painter.drawText(QRect(30, 58, W - 60, 26), Qt::AlignLeft | Qt::AlignVCenter,
                         QString("Généré le %1 par %2")
                             .arg(QDateTime::currentDateTime().toString("dd/MM/yyyy HH:mm"))
                             .arg(m_currentUser));
        painter.setFont(QFont("Segoe UI", 12, QFont::Bold));
        painter.setPen(QColor("#d4a63a"));
        painter.drawText(QRect(0, 20, W - 30, 40), Qt::AlignRight | Qt::AlignVCenter,
                         QString::fromUtf8("\xf0\x9f\x9b\xa1\xef\xb8\x8f CONFIDENTIEL"));
    };

    auto newPage = [&]() {
        drawFooter();
        printer.newPage();
        ++pageIndex;
        painter.fillRect(page, QColor("#f3f6f5"));
        drawHeader();
        y = headerH + 15;
    };

    auto drawSectionBar = [&](const QString &title, const QColor &color) {
        painter.fillRect(0, y, W, sectionBarH, color);
        painter.setPen(Qt::white);
        painter.setFont(QFont("Segoe UI", 11, QFont::Bold));
        painter.drawText(QRect(20, y, W - 40, sectionBarH), Qt::AlignLeft | Qt::AlignVCenter, title);
        y += sectionBarH;
    };

    auto drawTableHeader = [&](const QStringList &headers, const QVector<int> &colW) {
        painter.fillRect(0, y, W, rowH, QColor("#111827"));
        painter.setPen(Qt::white);
        painter.setFont(QFont("Segoe UI", 9, QFont::Bold));
        int x = 10;
        for (int i = 0; i < headers.size(); ++i) {
            painter.drawText(QRect(x + 6, y, colW[i] - 10, rowH), Qt::AlignLeft | Qt::AlignVCenter, headers[i]);
            x += colW[i];
        }
        y += rowH;
    };

    auto ensureSpace = [&](int needed) {
        if (y + needed <= H - footerH) return;
        newPage();
    };

    auto drawRowsPaged = [&](const QList<QStringList> &rows,
                             const QVector<int> &colW,
                             const QColor &altBg,
                             const std::function<void()> &repeatHeader) {
        painter.setFont(QFont("Segoe UI", 9));
        bool alt = false;
        for (int i = 0; i < rows.size(); ++i) {
            if (y + rowH > H - footerH) {
                newPage();
                repeatHeader();
                alt = false;
            }
            painter.fillRect(0, y, W, rowH, alt ? altBg : Qt::white);
            painter.setPen(QColor("#111827"));
            int x = 10;
            for (int c = 0; c < rows[i].size(); ++c) {
                const QString txt = rows[i][c];
                painter.drawText(QRect(x + 6, y, colW[c] - 10, rowH), Qt::AlignLeft | Qt::AlignVCenter,
                                 painter.fontMetrics().elidedText(txt, Qt::ElideRight, colW[c] - 12));
                x += colW[c];
            }
            y += rowH;
            alt = !alt;
        }
    };

    painter.fillRect(page, QColor("#f3f6f5"));
    drawHeader();
    y = headerH + 15;

    // Executive summary boxes
    {
        const int boxW = (W - 80) / 3;
        const int boxH = 60;
        int x = 20;
        auto drawBox = [&](const QString &label, const QString &value, const QColor &border) {
            painter.setPen(QPen(border, 2));
            painter.setBrush(Qt::white);
            painter.drawRoundedRect(QRect(x, y, boxW, boxH), 10, 10);
            painter.setPen(QColor("#111827"));
            painter.setFont(QFont("Segoe UI", 10, QFont::Bold));
            painter.drawText(QRect(x + 12, y + 8, boxW - 24, 20), Qt::AlignLeft | Qt::AlignVCenter, label);
            painter.setPen(border);
            painter.setFont(QFont("Segoe UI", 16, QFont::Bold));
            painter.drawText(QRect(x + 12, y + 28, boxW - 24, 28), Qt::AlignLeft | Qt::AlignVCenter, value);
            x += boxW + 20;
        };
        drawBox("Comptes Verrouillés", QString::number(lockedCount), QColor("#dc2626"));
        drawBox("Comptes Inactifs", QString::number(inactiveCount), QColor("#ea580c"));
        drawBox("Risques Détectés", QString::number(riskCount), QColor("#d4a63a"));
        y += boxH + 18;
    }

    // DS1: Locked accounts
    if (!ds1.isEmpty()) {
        ensureSpace(sectionBarH + rowH);
        const QString secTitle = QString::fromUtf8("⚠️ COMPTES VERROUILLÉS");
        const QColor secColor("#dc2626");
        drawSectionBar(secTitle, secColor);
        const QStringList hdrs = {"Utilisateur", "Email", "Rôle", "Tentatives", "Verrouillé le"};
        const QVector<int> colW = {120, 170, 80, 80, W - (120 + 170 + 80 + 80) - 20};
        drawTableHeader(hdrs, colW);

        QList<QStringList> rows;
        for (const auto &r : ds1) rows << (QStringList{r.u, r.e, r.r, QString::number(r.failed), r.lockedAt});
        drawRowsPaged(rows, colW, QColor("#fff5f5"), [&]() {
            drawSectionBar(secTitle, secColor);
            drawTableHeader(hdrs, colW);
        });
        y += 14;
    }

    // DS2: Inactive
    ensureSpace(sectionBarH + rowH);
    drawSectionBar(QString::fromUtf8("😴 COMPTES INACTIFS (>30 jours)"), QColor("#ea580c"));
    {
        const QString secTitle = QString::fromUtf8("😴 COMPTES INACTIFS (>30 jours)");
        const QColor secColor("#ea580c");
        const QStringList hdrs = {"Utilisateur", "Email", "Rôle", "Dernière connexion", "Créé le"};
        const QVector<int> colW = {120, 170, 80, 120, W - (120 + 170 + 80 + 120) - 20};
        drawTableHeader(hdrs, colW);
        QList<QStringList> rows;
        for (const auto &r : ds2) rows << (QStringList{r.u, r.e, r.r, r.lastLogin, r.created});
        drawRowsPaged(rows, colW, QColor("#fff7ed"), [&]() {
            drawSectionBar(secTitle, secColor);
            drawTableHeader(hdrs, colW);
        });
        y += 8;
    }

    // Next page for DS3/DS4/DS5
    newPage();

    // DS3 chart
    ensureSpace(sectionBarH + 240);
    drawSectionBar(QString::fromUtf8("📈 ACTIVITÉ DE SÉCURITÉ (30 derniers jours)"), QColor("#0284c7"));
    {
        const QRect chartRect(30, y + 10, W - 60, 200);
        painter.setPen(QColor("#e5e7eb"));
        painter.setBrush(Qt::white);
        painter.drawRoundedRect(chartRect, 10, 10);

        int maxCnt = 1;
        for (const auto &p : ds3) maxCnt = qMax(maxCnt, p.second);

        const int barAreaX = chartRect.x() + 20;
        const int barAreaY = chartRect.y() + 20;
        const int barAreaW = chartRect.width() - 40;
        const int barAreaH = chartRect.height() - 40;

        const int n = qMax(1, ds3.size());
        const int barW = qMax(12, barAreaW / (n * 2));
        int x = barAreaX;
        painter.setFont(QFont("Segoe UI", 8));

        for (int i = 0; i < ds3.size(); ++i) {
            const QString type = ds3[i].first;
            const int cnt = ds3[i].second;
            const int h = (int)((double)cnt / (double)maxCnt * (barAreaH - 20));
            const QRect bar(x, barAreaY + (barAreaH - h), barW, h);
            painter.fillRect(bar, QColor("#16a34a"));
            painter.setPen(QColor("#111827"));
            painter.drawText(QRect(x - 20, barAreaY + barAreaH + 2, barW + 40, 16), Qt::AlignHCenter | Qt::AlignTop,
                             painter.fontMetrics().elidedText(type, Qt::ElideRight, barW + 40));
            x += barW * 2;
        }
        y += 220;
        y += 18;
    }

    // DS4
    ensureSpace(sectionBarH + rowH);
    drawSectionBar(QString::fromUtf8("🔴 COMPTES À RISQUE (tentatives échouées ≥ 2)"), QColor("#ea580c"));
    {
        const QString secTitle = QString::fromUtf8("🔴 COMPTES À RISQUE (tentatives échouées ≥ 2)");
        const QColor secColor("#ea580c");
        const QStringList hdrs = {"Utilisateur", "Email", "Rôle", "Tentatives échouées"};
        const QVector<int> colW = {130, 200, 90, W - (130 + 200 + 90) - 20};
        drawTableHeader(hdrs, colW);
        QList<QStringList> rows;
        for (const auto &r : ds4) rows << (QStringList{r.u, r.e, r.r, QString::number(r.failed)});
        drawRowsPaged(rows, colW, QColor("#fffbeb"), [&]() {
            drawSectionBar(secTitle, secColor);
            drawTableHeader(hdrs, colW);
        });
        y += 14;
    }

    // DS5
    ensureSpace(sectionBarH + rowH);
    drawSectionBar(QString::fromUtf8("🆕 NOUVEAUX COMPTES (7 derniers jours)"), QColor("#2563eb"));
    {
        const QString secTitle = QString::fromUtf8("🆕 NOUVEAUX COMPTES (7 derniers jours)");
        const QColor secColor("#2563eb");
        const QStringList hdrs = {"Utilisateur", "Email", "Rôle", "État", "Date de création"};
        const QVector<int> colW = {120, 170, 80, 80, W - (120 + 170 + 80 + 80) - 20};
        drawTableHeader(hdrs, colW);
        QList<QStringList> rows;
        for (const auto &r : ds5) rows << (QStringList{r.u, r.e, r.r, r.st, r.created});
        drawRowsPaged(rows, colW, QColor("#eff6ff"), [&]() {
            drawSectionBar(secTitle, secColor);
            drawTableHeader(hdrs, colW);
        });
    }

    drawFooter();
    painter.end();

    QMessageBox::information(this, "Rapport généré",
                             QString("Rapport de sécurité exporté :\n%1").arg(filePath));
}

// ─────────────────────────────────────────────────────────────────────────────
// AnalyticsWidget implementation
// ─────────────────────────────────────────────────────────────────────────────
AnalyticsWidget::AnalyticsWidget(const QSqlDatabase &db, QWidget *parent)
    : QWidget(parent)
    , m_db(db)
{
    setAutoFillBackground(false);
}

void AnalyticsWidget::refreshData()
{
    m_topFails.clear();
    m_stateCounts.clear();
    m_last7.clear();
    m_inactiveCount = 0;

    if (!m_db.isOpen()) {
        update();
        return;
    }

    // QUERY 1: Top failed attempts
    {
        QSqlQuery q(m_db);
        q.exec("SELECT USERNAME, FAILED_ATTEMPTS FROM EMPLOYEES "
               "WHERE FAILED_ATTEMPTS > 0 ORDER BY FAILED_ATTEMPTS DESC "
               "FETCH FIRST 10 ROWS ONLY");
        while (q.next()) {
            UserFailRow r;
            r.username = q.value(0).toString();
            r.failed = q.value(1).toInt();
            m_topFails << r;
        }
    }

    // QUERY 2: State distribution
    {
        QSqlQuery q(m_db);
        q.exec("SELECT NVL(STATE,'ACTIVE') AS STATE, COUNT(*) FROM EMPLOYEES GROUP BY NVL(STATE,'ACTIVE')");
        while (q.next()) {
            m_stateCounts[q.value(0).toString()] = q.value(1).toInt();
        }
    }

    // QUERY 3: Inactive users
    {
        QSqlQuery q(m_db);
        q.exec("SELECT COUNT(*) FROM EMPLOYEES "
               "WHERE (LAST_LOGIN < SYSDATE - 30 OR LAST_LOGIN IS NULL) "
               "AND NVL(STATE,'ACTIVE') != 'ARCHIVED'");
        if (q.next()) m_inactiveCount = q.value(0).toInt();
    }

    // QUERY 4: Last 7 days login events
    QMap<QDate, DayEventCounts> map;
    {
        const QDate today = QDate::currentDate();
        for (int i = 6; i >= 0; --i) {
            DayEventCounts d; d.day = today.addDays(-i);
            map[d.day] = d;
        }
        QSqlQuery q(m_db);
        q.exec("SELECT TRUNC(LOG_TIME), EVENT_TYPE, COUNT(*) "
               "FROM AUDIT_LOG WHERE LOG_TIME >= SYSDATE - 7 "
               "AND EVENT_TYPE IN ('LOGIN_SUCCESS','LOGIN_FAIL','ACCOUNT_LOCKED') "
               "GROUP BY TRUNC(LOG_TIME), EVENT_TYPE ORDER BY TRUNC(LOG_TIME)");
        while (q.next()) {
            const QDate day = q.value(0).toDate();
            const QString type = q.value(1).toString();
            const int cnt = q.value(2).toInt();
            if (!map.contains(day)) {
                DayEventCounts d; d.day = day; map[day] = d;
            }
            auto d = map[day];
            if (type == "LOGIN_SUCCESS") d.success = cnt;
            else if (type == "LOGIN_FAIL") d.fail = cnt;
            else if (type == "ACCOUNT_LOCKED") d.locked = cnt;
            map[day] = d;
        }
    }
    m_last7 = map.values();

    update();
}

void AnalyticsWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);
    p.fillRect(rect(), QColor("#f3f6f5"));

    const int W = width();
    const int H = height();
    const int pad = 12;

    QRect r1(pad, pad, (int)(W * 0.40) - pad, (int)(H * 0.45) - pad);
    QRect r2((int)(W * 0.40) + pad, pad, W - (int)(W * 0.40) - 2 * pad, (int)(H * 0.45) - pad);
    QRect r3(pad, (int)(H * 0.45) + pad, (int)(W * 0.40) - pad, H - (int)(H * 0.45) - 2 * pad);
    QRect r4((int)(W * 0.40) + pad, (int)(H * 0.45) + pad, W - (int)(W * 0.40) - 2 * pad, H - (int)(H * 0.45) - 2 * pad);

    auto drawPanel = [&](const QRect &r, const QString &title) {
        p.setPen(QColor("#e5e7eb"));
        p.setBrush(Qt::white);
        p.drawRoundedRect(r, 10, 10);
        p.setPen(QColor("#2c3e2e"));
        p.setFont(QFont("Segoe UI", 10, QFont::Bold));
        p.drawText(QRect(r.x() + 12, r.y() + 10, r.width() - 24, 18), Qt::AlignLeft | Qt::AlignVCenter, title);
    };

    drawPanel(r1, "Tentatives échouées par utilisateur");
    drawPanel(r2, "Répartition des états de compte");
    drawPanel(r3, "Activité de connexion (7 derniers jours)");
    drawPanel(r4, "Indicateurs de risque");

    // Section 1: horizontal bars
    {
        QRect area = r1.adjusted(12, 38, -12, -12);
        int maxV = 1;
        for (const auto &u : m_topFails) maxV = qMax(maxV, u.failed);
        const int barH = m_topFails.isEmpty() ? 18 : qMax(12, area.height() / qMax(1, m_topFails.size()));
        int y = area.y();
        p.setFont(QFont("Segoe UI", 8));
        for (const auto &u : m_topFails) {
            const int w = (int)((double)u.failed / (double)maxV * (area.width() - 90));
            QColor col = (u.failed < 2) ? QColor("#16a34a") : (u.failed == 2 ? QColor("#ea580c") : QColor("#dc2626"));
            p.setPen(QColor("#111827"));
            p.drawText(QRect(area.x(), y, 85, barH), Qt::AlignLeft | Qt::AlignVCenter,
                       p.fontMetrics().elidedText(u.username, Qt::ElideRight, 80));
            p.fillRect(QRect(area.x() + 90, y + 4, w, barH - 8), col);
            y += barH;
        }
        if (m_topFails.isEmpty()) {
            p.setPen(QColor("#6b7280"));
            p.drawText(area, Qt::AlignCenter, "Aucune tentative échouée");
        }
    }

    // Section 2: pie/donut
    {
        QRect area = r2.adjusted(12, 38, -12, -12);
        QRect pieRect(area.x(), area.y(), qMin(area.width() - 140, area.height()), qMin(area.width() - 140, area.height()));
        if (pieRect.width() < 80) pieRect.setWidth(80);
        pieRect.setHeight(pieRect.width());

        struct Slice { QString label; int value; QColor color; };
        QList<Slice> slices;
        slices << Slice{"ACTIVE", m_stateCounts.value("ACTIVE"), QColor("#16a34a")};
        slices << Slice{"LOCKED", m_stateCounts.value("LOCKED"), QColor("#dc2626")};
        slices << Slice{"INACTIVE", m_stateCounts.value("INACTIVE"), QColor("#6b7280")};
        slices << Slice{"ARCHIVED", m_stateCounts.value("ARCHIVED"), QColor("#a16207")};

        int total = 0;
        for (const auto &s : slices) total += s.value;
        if (total <= 0) total = 1;

        int startAngle = 90 * 16;
        for (const auto &s : slices) {
            if (s.value <= 0) continue;
            const int span = (int)((double)s.value / (double)total * 360.0 * 16.0);
            p.setBrush(s.color);
            p.setPen(Qt::NoPen);
            p.drawPie(pieRect, startAngle, -span);
            startAngle -= span;
        }

        // donut hole
        p.setBrush(Qt::white);
        p.setPen(Qt::NoPen);
        QRect hole = pieRect.adjusted(pieRect.width() / 4, pieRect.height() / 4,
                                      -pieRect.width() / 4, -pieRect.height() / 4);
        p.drawEllipse(hole);

        // legend
        int lx = pieRect.right() + 16;
        int ly = area.y() + 10;
        p.setFont(QFont("Segoe UI", 9));
        for (const auto &s : slices) {
            p.setBrush(s.color);
            p.setPen(Qt::NoPen);
            p.drawRoundedRect(QRect(lx, ly, 12, 12), 3, 3);
            p.setPen(QColor("#111827"));
            p.drawText(QRect(lx + 18, ly - 2, area.right() - lx - 20, 16), Qt::AlignLeft | Qt::AlignVCenter,
                       QString("%1: %2").arg(s.label).arg(s.value));
            ly += 18;
        }
    }

    // Section 3: grouped bars (7 days)
    {
        QRect area = r3.adjusted(12, 38, -12, -16);
        if (m_last7.isEmpty()) {
            p.setPen(QColor("#6b7280"));
            p.drawText(area, Qt::AlignCenter, "Données indisponibles");
        } else {
            int maxV = 1;
            for (const auto &d : m_last7) maxV = qMax(maxV, qMax(d.success, qMax(d.fail, d.locked)));
            const int n = m_last7.size();
            const int groupW = qMax(18, area.width() / n);
            const int baseY = area.bottom() - 20;
            const int maxH = area.height() - 40;
            p.setFont(QFont("Segoe UI", 8));
            for (int i = 0; i < n; ++i) {
                const auto &d = m_last7[i];
                const int gx = area.x() + i * groupW + 4;
                auto bar = [&](int v, const QColor &c, int offset) {
                    const int h = (int)((double)v / (double)maxV * maxH);
                    p.fillRect(QRect(gx + offset, baseY - h, 6, h), c);
                };
                bar(d.success, QColor("#16a34a"), 0);
                bar(d.fail, QColor("#ea580c"), 8);
                bar(d.locked, QColor("#dc2626"), 16);
                p.setPen(QColor("#111827"));
                p.drawText(QRect(gx - 4, baseY + 4, groupW, 16), Qt::AlignLeft | Qt::AlignTop,
                           d.day.toString("dd/MM"));
            }
        }
    }

    // Section 4: risk boxes
    {
        QRect area = r4.adjusted(12, 38, -12, -12);
        const int boxW = (area.width() - 20) / 3;
        const int boxH = 70;
        int x = area.x();

        auto getCount = [&](const QString &state) {
            return m_stateCounts.value(state, 0);
        };

        int recentSuccess = 0, recentFail = 0;
        for (const auto &d : m_last7) { recentSuccess += d.success; recentFail += d.fail; }
        const int totalRecent = recentSuccess + recentFail;
        const int failRate = totalRecent == 0 ? 0 : (int)((double)recentFail / (double)totalRecent * 100.0);

        auto drawStat = [&](const QString &label, const QString &value, const QColor &accent) {
            p.setPen(QPen(QColor("#e5e7eb"), 1));
            p.setBrush(Qt::white);
            p.drawRoundedRect(QRect(x, area.y(), boxW, boxH), 10, 10);
            p.setPen(QColor("#111827"));
            p.setFont(QFont("Segoe UI", 9, QFont::Bold));
            p.drawText(QRect(x + 12, area.y() + 10, boxW - 24, 16), Qt::AlignLeft | Qt::AlignVCenter, label);
            p.setPen(accent);
            p.setFont(QFont("Segoe UI", 18, QFont::Bold));
            p.drawText(QRect(x + 12, area.y() + 28, boxW - 24, 30), Qt::AlignLeft | Qt::AlignVCenter, value);
            x += boxW + 10;
        };

        drawStat("Comptes verrouillés", QString::number(getCount("LOCKED")), QColor("#dc2626"));
        drawStat("Utilisateurs inactifs (>30j)", QString::number(m_inactiveCount), QColor("#ea580c"));
        drawStat("Taux d'échec récent", QString::number(failRate) + "%", QColor("#7c3aed"));
    }
}

// ─────────────────────────────────────────────────────────────────────────────
//  SLOT – Back
// ─────────────────────────────────────────────────────────────────────────────
void userstaff::onBackClicked()
{
    emit backToMenu();
    this->hide();
}
