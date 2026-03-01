#include "userstaff.h"
#include "ui_userstaff.h"
#include "connectionjasser.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
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
#include <QTableWidgetItem>

// ─────────────────────────────────────────────────────────────────────────────
//  Constructor / Destructor
// ─────────────────────────────────────────────────────────────────────────────
userstaff::userstaff(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::userstaff)
{
    ui->setupUi(this);
    {
        QPixmap logo(":/logo.png");
        if (!logo.isNull())
            ui->lblLogo->setPixmap(logo.scaled(40, 40, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
    setWindowTitle("PressIQ - User & Staff Management");

    // connect DB
    ConnectionJasser::getInstance().createconnect();

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

        // col 7 – placeholder (real action buttons are header-level)
        auto *itemActions = new QTableWidgetItem("—");
        itemActions->setTextAlignment(Qt::AlignCenter);
        ui->userTable->setItem(row, 7, itemActions);

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
                            "WHERE FAILED_ATTEMPTS >= 3");

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
    ui->btnSupprimer->setEnabled(hasSelection);
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
//  SLOT – Back
// ─────────────────────────────────────────────────────────────────────────────
void userstaff::onBackClicked()
{
    emit backToMenu();
    this->hide();
}

