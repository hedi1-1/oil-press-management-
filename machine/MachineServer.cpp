#include "MachineServer.h"
#include "connexionmachine.h"

#include <QDate>
#include <QHostAddress>
#include <QHostInfo>
#include <QHttpServerRequest>
#include <QHttpServerResponder>
#include <QHttpServerResponse>
#include <QNetworkInterface>
#include <QSqlError>
#include <QSqlQuery>
#include <QUrl>
#include <QUrlQuery>
#include <algorithm>

MachineServer::MachineServer(QObject *parent)
    : QObject(parent), m_port(8181), m_started(false),
      m_routeRegistered(false) {

    // 1. Initialisation par défaut
    m_hostIp = "127.0.0.1";

    // 2. Détection intelligente de l'IP WiFi
    // On parcourt toutes les cartes réseaux du PC
    const QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();
    for (const QNetworkInterface &iface : interfaces) {
        
        // On ignore les cartes virtuelles (VirtualBox, VMware) et les cartes éteintes
        bool isVirtual = iface.humanReadableName().toLower().contains("virtualbox") || 
                         iface.humanReadableName().toLower().contains("vmware") ||
                         iface.humanReadableName().toLower().contains("pseudo");

        if (!iface.flags().testFlag(QNetworkInterface::IsUp) || 
            iface.flags().testFlag(QNetworkInterface::IsLoopBack) || 
            isVirtual) {
            continue;
        }

        for (const QNetworkAddressEntry &entry : iface.addressEntries()) {
            QHostAddress addr = entry.ip();
            // On ne prend que l'IPv4
            if (addr.protocol() == QAbstractSocket::IPv4Protocol) {
                m_hostIp = addr.toString();
                
                // Si le nom de la carte contient "Wi-Fi", on a trouvé la meilleure cible
                if (iface.humanReadableName().toLower().contains("wi-fi") || 
                    iface.humanReadableName().toLower().contains("wlan")) {
                    return; 
                }
            }
        }
    }
}

bool MachineServer::start(quint16 port) {
    if (m_started) return true;
    m_port = port;

    if (!m_routeRegistered) {
        m_server.route("/machine", [this](const QHttpServerRequest &request) {
            return handleMachineRequest(request);
        });
        m_routeRegistered = true;
    }

    // IMPORTANT : QHostAddress::Any permet à l'iPhone de se connecter via le WiFi
    const bool listening = m_server.listen(QHostAddress::Any, m_port);
    m_started = listening;
    return listening;
}

QString MachineServer::machineUrlForId(const QString &machineId) const {
    // SOLUTION STABLE : On utilise le nom du PC + .local
    // Même si l'IP change, l'iPhone retrouvera le PC sur le réseau
    QString pcName = QHostInfo::localHostName().toLower();
    return QString("http://%1.local:%2/machine?id=%3")
        .arg(pcName)
        .arg(m_port)
        .arg(machineId);
}

void MachineServer::setHostIp(const QString &hostIp) {
    if (!hostIp.trimmed().isEmpty()) m_hostIp = hostIp.trimmed();
}

QString MachineServer::hostIp() const { return m_hostIp; }

QHttpServerResponse MachineServer::handleMachineRequest(const QHttpServerRequest &request) {
    const QUrlQuery urlQuery(request.url());
    const QString machineId = urlQuery.queryItemValue("id").trimmed();

    if (machineId.isEmpty()) {
        return QHttpServerResponse("text/plain; charset=UTF-8", QByteArray("Parametre id manquant."));
    }

    MachineSnapshot snapshot;
    QString error;
    if (!fetchMachineById(machineId, &snapshot, &error)) {
        const QByteArray message = error.isEmpty() ? QByteArray("Machine introuvable.") : error.toUtf8();
        return QHttpServerResponse("text/plain; charset=UTF-8", message);
    }

    emit machineRequested(snapshot.id);
    const QList<MachineTimelineItem> timeline = buildTimeline(snapshot);
    const QString html = buildMachineHtml(snapshot, timeline);
    return QHttpServerResponse("text/html; charset=UTF-8", html.toUtf8());
}

bool MachineServer::fetchMachineById(const QString &machineId, MachineSnapshot *snapshot, QString *error) const {
    if (!snapshot) return false;

    QSqlQuery query(ConnectionMachine::getInstance().getDatabase());
    query.prepare("SELECT ID_MACHINE, NOM_MACHINE, TYPE_MACHINE, ETAT_MARCHE, "
                  "TEMPERATURE_ACTUELLE, NIVEAU_CHARGE, ETAT_FONCTIONNEMENT, TYPE_ALERTE, "
                  "NIVEAU_CRITICITE, DATE_DERNIERE_MAINTENANCE, DATE_INSTALLATION, "
                  "SCORE_SANTE, DATE_MISE_A_JOUR, USER_ID, REFROIDISSEMENT, TAG, PRIORITE "
                  "FROM MACHINE WHERE ID_MACHINE = :id");
    query.bindValue(":id", machineId);

    if (!query.exec()) {
        if (error) *error = query.lastError().text();
        return false;
    }
    if (!query.next()) return false;

    snapshot->id = query.value(0).toString();
    snapshot->nom = query.value(1).toString();
    snapshot->type = query.value(2).toString();
    snapshot->etatMarche = query.value(3).toString();
    snapshot->temperatureActuelle = query.value(4).toDouble();
    snapshot->niveauCharge = query.value(5).toDouble();
    snapshot->etatFonctionnement = query.value(6).toString();
    snapshot->typeAlerte = query.value(7).toString();
    snapshot->niveauCriticite = query.value(8).toString();
    snapshot->dateDerniereMaintenance = query.value(9).toDateTime();
    snapshot->dateInstallation = query.value(10).toDateTime();
    snapshot->scoreSante = query.value(11).toInt();
    snapshot->dateMiseAJour = query.value(12).toDateTime();
    snapshot->userId = query.value(13).toString();
    snapshot->refroidissement = query.value(14).toString();
    snapshot->tag = query.value(15).toString();
    snapshot->priorite = query.value(16).toString();
    return true;
}

QList<MachineTimelineItem> MachineServer::buildTimeline(const MachineSnapshot &snapshot) const {
    QList<MachineTimelineItem> timeline;
    const QDateTime now = QDateTime::currentDateTime();
    const QDateTime updateTime = snapshot.dateMiseAJour.isValid() ? snapshot.dateMiseAJour : now;

    if (snapshot.dateDerniereMaintenance.isValid()) {
        timeline.append({snapshot.dateDerniereMaintenance, "Maintenance", "Intervention enregistree."});
    }
    if (!snapshot.typeAlerte.isEmpty() && snapshot.typeAlerte.toLower() != "aucune") {
        timeline.append({updateTime.addSecs(-1200), "Alerte", snapshot.typeAlerte});
    }
    timeline.append({updateTime, "Synchronisation", "Donnees a jour."});

    std::sort(timeline.begin(), timeline.end(), [](const MachineTimelineItem &a, const MachineTimelineItem &b) {
        return a.when > b.when;
    });
    return timeline;
}

QString MachineServer::buildMachineHtml(const MachineSnapshot &snapshot, const QList<MachineTimelineItem> &timeline) const {
    QString historyHtml;
    for (const auto &item : timeline) {
        historyHtml += QString(
            "<div class='event'>"
            "  <div class='event-dot'></div>"
            "  <div class='event-content'>"
            "    <span class='time'>%1</span>"
            "    <p><b>%2</b> : %3</p>"
            "  </div>"
            "</div>")
            .arg(item.when.toString("HH:mm"), item.title, item.detail);
    }

    return QString(R"HTML(
<!DOCTYPE html>
<html lang="fr">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no">
    <title>Dashboard Machine</title>
    <style>
        :root { --bg: #04160f; --card: rgba(10, 77, 54, 0.4); --green: #10b981; --text: #e2e8f0; }
        body { font-family: -apple-system, sans-serif; background: var(--bg); color: var(--text); margin: 0; padding: 15px; }
        .container { max-width: 500px; margin: auto; }
        .header { border-left: 4px solid var(--green); padding-left: 15px; margin: 20px 0; }
        .card { background: var(--card); backdrop-filter: blur(10px); border: 1px solid #064e3b; border-radius: 20px; padding: 20px; box-shadow: 0 10px 30px rgba(0,0,0,0.5); }
        .status-badge { background: rgba(16, 185, 129, 0.2); color: var(--green); padding: 5px 12px; border-radius: 10px; font-size: 11px; font-weight: bold; border: 1px solid var(--green); }
        .health-bar { background: #000; height: 10px; border-radius: 5px; margin: 15px 0; overflow: hidden; }
        .health-fill { height: 100%; width: %4%%; background: var(--green); box-shadow: 0 0 10px var(--green); }
        .grid { display: grid; grid-template-columns: 1fr 1fr; gap: 10px; margin-top: 15px; }
        .item { background: rgba(0,0,0,0.3); padding: 10px; border-radius: 15px; text-align: center; }
        .item label { display: block; font-size: 10px; color: var(--green); }
        .item span { font-size: 18px; font-weight: bold; }
        .history { margin-top: 20px; }
        .event { display: flex; margin-bottom: 15px; }
        .event-dot { width: 8px; height: 8px; background: var(--green); border-radius: 50%; margin-top: 5px; }
        .event-content { margin-left: 15px; font-size: 14px; }
        .time { font-size: 11px; color: var(--green); font-weight: bold; }
    </style>
</head>
<body>
    <div class="container">
        <div class="header"><h1>SENSORS HUB</h1><p>Monitoring Industriel</p></div>
        <div class="card">
            <span class="status-badge">%2</span>
            <h2 style="margin: 10px 0 0;">%3</h2>
            <p style="font-size: 12px; opacity: 0.6;">Modèle: %5</p>
            <div class="health-bar"><div class="health-fill"></div></div>
            <div class="grid">
                <div class="item"><label>TEMPERATURE</label><span>%6°C</span></div>
                <div class="item"><label>CHARGE</label><span>%7%</span></div>
            </div>
        </div>
        <div class="history">
            <h3 style="color: var(--green); font-size: 16px;">LOGS RÉCENTS</h3>
            %8
        </div>
        <p style="text-align:center; font-size: 10px; opacity: 0.3; margin-top: 20px;">ID: %1 | %9</p>
    </div>
</body>
</html>
    )HTML")
    .arg(h(snapshot.id), h(snapshot.etatMarche), h(snapshot.nom)) // %1, %2, %3
    .arg(snapshot.scoreSante)                                     // %4
    .arg(h(snapshot.type))                                        // %5
    .arg(snapshot.temperatureActuelle, 0, 'f', 1)                 // %6
    .arg(snapshot.niveauCharge, 0, 'f', 1)                        // %7
    .arg(historyHtml)                                             // %8
    .arg(snapshot.dateMiseAJour.toString("dd/MM/yyyy HH:mm:ss")); // %9
}

QString MachineServer::h(const QString &value) {
    QString escaped = value;
    escaped.replace('&', "&amp;").replace('<', "&lt;").replace('>', "&gt;").replace('"', "&quot;").replace('\'', "&#39;");
    return escaped;
}