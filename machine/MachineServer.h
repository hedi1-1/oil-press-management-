#ifndef MACHINESERVER_H
#define MACHINESERVER_H

#include <QObject>
#include <QDateTime>
#include <QHttpServer>
#include <QHttpServerRequest>
#include <QHttpServerResponse>
#include <QList>
#include <QString>
#include <QNetworkInterface>
#include <QHostInfo> // Nécessaire pour le nom du PC

struct MachineSnapshot {
    QString id;
    QString nom;
    QString type;
    QString etatMarche;
    double temperatureActuelle = 0.0;
    double niveauCharge = 0.0;
    QString etatFonctionnement;
    QString typeAlerte;
    QString niveauCriticite;
    QDateTime dateDerniereMaintenance;
    QDateTime dateInstallation;
    int scoreSante = 0;
    QDateTime dateMiseAJour;
    QString userId;
    QString refroidissement;
    QString tag;
    QString priorite;
};

struct MachineTimelineItem {
    QDateTime when;
    QString title;
    QString detail;
};

class MachineServer : public QObject {
    Q_OBJECT

public:
    explicit MachineServer(QObject *parent = nullptr);
    bool start(quint16 port = 8181);

    void setHostIp(const QString &hostIp);
    QString hostIp() const;
    QString machineUrlForId(const QString &machineId) const;

signals:
    void machineRequested(const QString &machineId);

private:
    QHttpServerResponse handleMachineRequest(const QHttpServerRequest &request);
    bool fetchMachineById(const QString &machineId, MachineSnapshot *snapshot,
                         QString *error) const;
    QList<MachineTimelineItem> buildTimeline(const MachineSnapshot &snapshot) const;
    QString buildMachineHtml(const MachineSnapshot &snapshot,
                            const QList<MachineTimelineItem> &timeline) const;
    QString stateBadgeClass(const QString &state) const;
    static QString h(const QString &value);

    QHttpServer m_server;
    QString m_hostIp;
    quint16 m_port;
    bool m_started;
    bool m_routeRegistered;
};

#endif // MACHINESERVER_H