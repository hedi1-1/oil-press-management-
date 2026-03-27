#ifndef PRODUCTIONEDITDIALOG_H
#define PRODUCTIONEDITDIALOG_H

#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QTextEdit>
#include <QPushButton>
#include <QFrame>
#include <QScrollArea>
#include <QDateEdit>
#include <QDate>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QShowEvent>

class ProductionEditDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ProductionEditDialog(QWidget *parent = nullptr);

    // Setters for pre-filling the form
    void setProductionId(int id);
    void setOlivesKg(int kg);
    void setHuileL(double l);
    void setTypePressage(const QString &type);
    void setStatut(const QString &statut);
    void setQualite(const QString &qualite);
    void setNotes(const QString &notes);
    void setDate(const QDate &date);
    void setRendement(double r);

    // Getters for reading form values
    int getOlivesKg() const;
    double getHuileL() const;
    QString getTypePressage() const;
    QString getStatut() const;
    QString getQualite() const;
    QString getNotes() const;
    QDate getDate() const;

private slots:
    void onSaveClicked();
    void onCancelClicked();
    void onOlivesOrHuileChanged();

protected:
    void showEvent(QShowEvent *e) override;

private:
    void setupUI();
    void applyStyles();
    void playOpenAnimation();

    int m_productionId = 0;

    // Header
    QLabel *lblTitle;
    QLabel *lblSubtitle;
    QLabel *lblBadgeId;

    // Fields
    QDateEdit   *dateEdit;
    QSpinBox    *spinOlives;
    QDoubleSpinBox *spinHuile;
    QComboBox   *comboType;
    QComboBox   *comboStatut;
    QComboBox   *comboQualite;
    QTextEdit   *txtNotes;

    // Live preview
    QLabel *lblRendementPreview;
    QLabel *lblYieldBar;

    // Buttons
    QPushButton *btnSave;
    QPushButton *btnCancel;
};

#endif // PRODUCTIONEDITDIALOG_H
