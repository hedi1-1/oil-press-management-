#include "productioneditdialog.h"
#include <QApplication>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QTimer>

ProductionEditDialog::ProductionEditDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUI();
    applyStyles();
    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setFixedWidth(540);
}

// ─── UI SETUP ───────────────────────────────────────────────────────────────

void ProductionEditDialog::setupUI()
{
    // ── Root container with rounded corners / shadow ──
    QWidget *card = new QWidget(this);
    card->setObjectName("card");

    QVBoxLayout *rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(16, 16, 16, 16);
    rootLayout->addWidget(card);

    QVBoxLayout *cardLayout = new QVBoxLayout(card);
    cardLayout->setContentsMargins(0, 0, 0, 0);
    cardLayout->setSpacing(0);

    // ── HEADER BANNER ────────────────────────────────────────────────────────
    QWidget *header = new QWidget();
    header->setObjectName("header");
    header->setFixedHeight(88);
    QHBoxLayout *hdrLay = new QHBoxLayout(header);
    hdrLay->setContentsMargins(24, 16, 24, 16);

    // Left: icon + titles
    QLabel *iconLbl = new QLabel("✏");
    iconLbl->setObjectName("iconLbl");

    QVBoxLayout *titlesLay = new QVBoxLayout();
    titlesLay->setSpacing(2);
    lblTitle = new QLabel("Modifier la Production");
    lblTitle->setObjectName("lblTitle");
    lblSubtitle = new QLabel("Mettez à jour les informations de production");
    lblSubtitle->setObjectName("lblSubtitle");
    titlesLay->addWidget(lblTitle);
    titlesLay->addWidget(lblSubtitle);

    // Right: badge
    lblBadgeId = new QLabel("ID #—");
    lblBadgeId->setObjectName("lblBadgeId");
    lblBadgeId->setAlignment(Qt::AlignCenter);

    hdrLay->addWidget(iconLbl);
    hdrLay->addSpacing(12);
    hdrLay->addLayout(titlesLay);
    hdrLay->addStretch();
    hdrLay->addWidget(lblBadgeId);

    // ── BODY ─────────────────────────────────────────────────────────────────
    QWidget *body = new QWidget();
    body->setObjectName("body");
    QVBoxLayout *bodyLay = new QVBoxLayout(body);
    bodyLay->setContentsMargins(24, 20, 24, 24);
    bodyLay->setSpacing(14);

    // Helper lambda for section labels
    auto sectionLabel = [](const QString &txt) {
        QLabel *l = new QLabel(txt);
        l->setObjectName("sectionLabel");
        return l;
    };
    auto fieldLabel = [](const QString &txt) {
        QLabel *l = new QLabel(txt);
        l->setObjectName("fieldLabel");
        return l;
    };

    // ── SECTION: Données de base ──────────────────────────────────────────
    bodyLay->addWidget(sectionLabel("📋  Données de base"));

    QGridLayout *grid1 = new QGridLayout();
    grid1->setHorizontalSpacing(16);
    grid1->setVerticalSpacing(10);
    grid1->setColumnStretch(0, 1);
    grid1->setColumnStretch(1, 1);

    // Date
    grid1->addWidget(fieldLabel("Date de production"), 0, 0);
    grid1->addWidget(fieldLabel("Type de pressage"), 0, 1);
    dateEdit = new QDateEdit(QDate::currentDate());
    dateEdit->setObjectName("dateEdit");
    dateEdit->setDisplayFormat("dd/MM/yyyy");
    dateEdit->setCalendarPopup(true);
    comboType = new QComboBox();
    comboType->setObjectName("comboBox");
    comboType->addItems({"Première pression à froid",
                         "Pression standard",
                         "Centrifugation",
                         "Pression à chaud"});
    grid1->addWidget(dateEdit, 1, 0);
    grid1->addWidget(comboType, 1, 1);

    // Statut / Qualité
    grid1->addWidget(fieldLabel("Statut"), 2, 0);
    grid1->addWidget(fieldLabel("Qualité"), 2, 1);
    comboStatut = new QComboBox();
    comboStatut->setObjectName("comboBox");
    comboStatut->addItems({"Planifie", "En cours", "Arrete", "Termine"});
    comboQualite = new QComboBox();
    comboQualite->setObjectName("comboBox");
    comboQualite->addItems({"—", "Extra Vierge", "Vierge", "Standard", "Lampante"});
    grid1->addWidget(comboStatut, 3, 0);
    grid1->addWidget(comboQualite, 3, 1);

    bodyLay->addLayout(grid1);

    // ── SEPARATOR ────────────────────────────────────────────────────────────
    QFrame *sep1 = new QFrame(); sep1->setFrameShape(QFrame::HLine);
    sep1->setObjectName("separator"); bodyLay->addWidget(sep1);

    // ── SECTION: Quantités & Rendement ──────────────────────────────────────
    bodyLay->addWidget(sectionLabel("⚖  Quantités & Rendement"));

    QGridLayout *grid2 = new QGridLayout();
    grid2->setHorizontalSpacing(16);
    grid2->setVerticalSpacing(10);
    grid2->setColumnStretch(0, 1);
    grid2->setColumnStretch(1, 1);

    grid2->addWidget(fieldLabel("Olives (kg)"), 0, 0);
    grid2->addWidget(fieldLabel("Huile produite (L)"), 0, 1);

    spinOlives = new QSpinBox();
    spinOlives->setObjectName("spinBox");
    spinOlives->setRange(1, 999999);
    spinOlives->setValue(500);
    spinOlives->setSuffix(" kg");

    spinHuile = new QDoubleSpinBox();
    spinHuile->setObjectName("spinBox");
    spinHuile->setRange(0.0, 999999.0);
    spinHuile->setDecimals(2);
    spinHuile->setValue(0.0);
    spinHuile->setSuffix(" L");

    grid2->addWidget(spinOlives, 1, 0);
    grid2->addWidget(spinHuile, 1, 1);

    bodyLay->addLayout(grid2);

    // Live rendement card
    QWidget *yieldCard = new QWidget();
    yieldCard->setObjectName("yieldCard");
    QVBoxLayout *yieldLay = new QVBoxLayout(yieldCard);
    yieldLay->setContentsMargins(16, 12, 16, 12);
    yieldLay->setSpacing(6);

    QHBoxLayout *yieldTopRow = new QHBoxLayout();
    QLabel *yieldIcon = new QLabel("📊");
    QLabel *yieldCaption = new QLabel("Rendement calculé en temps réel");
    yieldCaption->setObjectName("yieldCaption");
    lblRendementPreview = new QLabel("0.00 %");
    lblRendementPreview->setObjectName("lblRendementPreview");
    yieldTopRow->addWidget(yieldIcon);
    yieldTopRow->addWidget(yieldCaption);
    yieldTopRow->addStretch();
    yieldTopRow->addWidget(lblRendementPreview);

    lblYieldBar = new QLabel();
    lblYieldBar->setObjectName("lblYieldBar");
    lblYieldBar->setFixedHeight(8);

    yieldLay->addLayout(yieldTopRow);
    yieldLay->addWidget(lblYieldBar);
    bodyLay->addWidget(yieldCard);

    // ── SEPARATOR ────────────────────────────────────────────────────────────
    QFrame *sep2 = new QFrame(); sep2->setFrameShape(QFrame::HLine);
    sep2->setObjectName("separator"); bodyLay->addWidget(sep2);

    // ── SECTION: Notes ───────────────────────────────────────────────────────
    bodyLay->addWidget(sectionLabel("📝  Notes"));
    txtNotes = new QTextEdit();
    txtNotes->setObjectName("txtNotes");
    txtNotes->setPlaceholderText("Observations, remarques sur la production…");
    txtNotes->setFixedHeight(72);
    bodyLay->addWidget(txtNotes);

    // ── FOOTER BUTTONS ───────────────────────────────────────────────────────
    QFrame *sep3 = new QFrame(); sep3->setFrameShape(QFrame::HLine);
    sep3->setObjectName("separator"); bodyLay->addWidget(sep3);

    QHBoxLayout *btnRow = new QHBoxLayout();
    btnRow->setSpacing(12);
    btnCancel = new QPushButton("✕  Annuler");
    btnCancel->setObjectName("btnCancel");
    btnSave   = new QPushButton("✔  Enregistrer");
    btnSave->setObjectName("btnSave");
    btnCancel->setFixedHeight(40);
    btnSave->setFixedHeight(40);
    btnRow->addWidget(btnCancel);
    btnRow->addWidget(btnSave);
    bodyLay->addLayout(btnRow);

    // ── Assemble card ────────────────────────────────────────────────────────
    cardLayout->addWidget(header);
    cardLayout->addWidget(body);

    // ── Signals ──────────────────────────────────────────────────────────────
    connect(btnSave,   &QPushButton::clicked, this, &ProductionEditDialog::onSaveClicked);
    connect(btnCancel, &QPushButton::clicked, this, &ProductionEditDialog::onCancelClicked);
    connect(spinOlives, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &ProductionEditDialog::onOlivesOrHuileChanged);
    connect(spinHuile, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &ProductionEditDialog::onOlivesOrHuileChanged);
}

// ─── STYLES ─────────────────────────────────────────────────────────────────

void ProductionEditDialog::applyStyles()
{
    setStyleSheet(R"(
        /* ── Root transparent (shadow effect) ── */
        ProductionEditDialog {
            background: transparent;
        }

        /* ── Card ── */
        QWidget#card {
            background: white;
            border-radius: 16px;
            border: 1px solid #e5e7eb;
        }

        /* ── Header ── */
        QWidget#header {
            background: qlineargradient(x1:0,y1:0,x2:1,y2:0,
                stop:0 #1B4332, stop:1 #234E3E);
            border-top-left-radius: 16px;
            border-top-right-radius: 16px;
        }
        QLabel#iconLbl {
            font-size: 28px;
            color: #D4AC0D;
        }
        QLabel#lblTitle {
            font-size: 18px;
            font-weight: 700;
            color: white;
        }
        QLabel#lblSubtitle {
            font-size: 12px;
            color: #86efac;
        }
        QLabel#lblBadgeId {
            background: #D4AC0D;
            color: #1B4332;
            font-size: 13px;
            font-weight: 700;
            padding: 4px 14px;
            border-radius: 12px;
        }

        /* ── Body ── */
        QWidget#body {
            background: white;
            border-bottom-left-radius: 16px;
            border-bottom-right-radius: 16px;
        }

        /* ── Section labels ── */
        QLabel#sectionLabel {
            font-size: 12px;
            font-weight: 700;
            color: #6b7280;
            text-transform: uppercase;
            letter-spacing: 1px;
        }
        QLabel#fieldLabel {
            font-size: 12px;
            font-weight: 600;
            color: #374151;
        }

        /* ── Inputs ── */
        QSpinBox#spinBox, QDoubleSpinBox#spinBox,
        QComboBox#comboBox, QDateEdit#dateEdit {
            border: 1.5px solid #d1d5db;
            border-radius: 8px;
            padding: 7px 10px;
            font-size: 13px;
            color: #111827;
            background: #f9fafb;
            min-height: 36px;
        }
        QSpinBox#spinBox:focus, QDoubleSpinBox#spinBox:focus,
        QComboBox#comboBox:focus, QDateEdit#dateEdit:focus {
            border-color: #1B4332;
            background: white;
        }
        QComboBox#comboBox::drop-down {
            border: none;
            width: 28px;
        }
        QComboBox#comboBox::down-arrow {
            width: 12px; height: 12px;
        }

        QTextEdit#txtNotes {
            border: 1.5px solid #d1d5db;
            border-radius: 8px;
            padding: 8px;
            font-size: 13px;
            color: #111827;
            background: #f9fafb;
        }
        QTextEdit#txtNotes:focus { border-color: #1B4332; background: white; }

        /* ── Yield Card ── */
        QWidget#yieldCard {
            background: #f0fdf4;
            border: 1.5px solid #bbf7d0;
            border-radius: 10px;
        }
        QLabel#yieldCaption {
            font-size: 12px;
            color: #166534;
        }
        QLabel#lblRendementPreview {
            font-size: 22px;
            font-weight: 700;
            color: #166534;
        }

        /* ── Separators ── */
        QFrame#separator {
            color: #f3f4f6;
            background: #f3f4f6;
            max-height: 1px;
        }

        /* ── Buttons ── */
        QPushButton#btnSave {
            background: #1B4332;
            color: white;
            font-size: 14px;
            font-weight: 700;
            border-radius: 10px;
            border: none;
            padding: 0 20px;
        }
        QPushButton#btnSave:hover { background: #234E3E; }
        QPushButton#btnSave:pressed { background: #14532d; }

        QPushButton#btnCancel {
            background: white;
            color: #6b7280;
            font-size: 14px;
            font-weight: 600;
            border-radius: 10px;
            border: 1.5px solid #d1d5db;
            padding: 0 20px;
        }
        QPushButton#btnCancel:hover {
            background: #fef2f2;
            color: #dc2626;
            border-color: #fca5a5;
        }
    )");
}

// ─── ANIMATION ──────────────────────────────────────────────────────────────

void ProductionEditDialog::playOpenAnimation()
{
    QGraphicsOpacityEffect *effect = new QGraphicsOpacityEffect(this);
    this->setGraphicsEffect(effect);
    QPropertyAnimation *anim = new QPropertyAnimation(effect, "opacity", this);
    anim->setDuration(250);
    anim->setStartValue(0.0);
    anim->setEndValue(1.0);
    anim->start(QAbstractAnimation::DeleteWhenStopped);
}

// ─── SETTERS ────────────────────────────────────────────────────────────────

void ProductionEditDialog::setProductionId(int id)
{
    m_productionId = id;
    lblBadgeId->setText("ID  #" + QString::number(id));
}
void ProductionEditDialog::setOlivesKg(int kg)    { spinOlives->setValue(kg); }
void ProductionEditDialog::setHuileL(double l)    { spinHuile->setValue(l); }
void ProductionEditDialog::setTypePressage(const QString &t)
{
    int idx = comboType->findText(t, Qt::MatchContains);
    if (idx >= 0) comboType->setCurrentIndex(idx);
}
void ProductionEditDialog::setStatut(const QString &s)
{
    int idx = comboStatut->findText(s, Qt::MatchFixedString);
    if (idx >= 0) comboStatut->setCurrentIndex(idx);
}
void ProductionEditDialog::setQualite(const QString &q)
{
    int idx = comboQualite->findText(q, Qt::MatchFixedString);
    if (idx >= 0) comboQualite->setCurrentIndex(idx);
    else comboQualite->setCurrentIndex(0);
}
void ProductionEditDialog::setNotes(const QString &n)   { txtNotes->setPlainText(n); }
void ProductionEditDialog::setDate(const QDate &d)      { dateEdit->setDate(d); }
void ProductionEditDialog::setRendement(double r)
{
    lblRendementPreview->setText(QString::number(r, 'f', 2) + " %");
}

// ─── GETTERS ────────────────────────────────────────────────────────────────

int     ProductionEditDialog::getOlivesKg()     const { return spinOlives->value(); }
double  ProductionEditDialog::getHuileL()       const { return spinHuile->value(); }
QString ProductionEditDialog::getTypePressage() const { return comboType->currentText(); }
QString ProductionEditDialog::getStatut()       const { return comboStatut->currentText(); }
QString ProductionEditDialog::getQualite()      const
{
    QString q = comboQualite->currentText();
    return (q == "—") ? QString() : q;
}
QString ProductionEditDialog::getNotes()        const { return txtNotes->toPlainText(); }
QDate   ProductionEditDialog::getDate()         const { return dateEdit->date(); }

// ─── LIVE RENDEMENT UPDATE ──────────────────────────────────────────────────

void ProductionEditDialog::onOlivesOrHuileChanged()
{
    double olives = spinOlives->value();
    double huile  = spinHuile->value();

    if (olives <= 0) {
        lblRendementPreview->setText("—");
        lblYieldBar->setStyleSheet("background: #d1d5db; border-radius: 4px;");
        return;
    }

    double r = (huile / olives) * 100.0;
    lblRendementPreview->setText(QString::number(r, 'f', 2) + " %");

    // Color + width of yield bar (max meaningful ~25%)
    int pct = qMin(static_cast<int>(r / 25.0 * 100), 100);

    QString color;
    if (r >= 18)      color = "#16a34a";  // green
    else if (r >= 14) color = "#d97706";  // amber
    else              color = "#dc2626";  // red

    lblYieldBar->setStyleSheet(
        QString("background: qlineargradient(x1:0,y1:0,x2:1,y2:0,"
                " stop:0 %1, stop:%2 %1, stop:%3 #e5e7eb, stop:1 #e5e7eb);"
                " border-radius: 4px;")
            .arg(color)
            .arg(pct / 100.0, 0, 'f', 3)
            .arg(qMin(pct / 100.0 + 0.001, 1.0), 0, 'f', 3)
    );
}

// ─── ACTIONS ────────────────────────────────────────────────────────────────

void ProductionEditDialog::onSaveClicked()
{
    accept();
}

void ProductionEditDialog::onCancelClicked()
{
    reject();
}

// Override showEvent to play animation on open
void ProductionEditDialog::showEvent(QShowEvent *e)
{
    QDialog::showEvent(e);
    playOpenAnimation();
}
