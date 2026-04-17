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
    // Génération du QR Code pour l'affichage
    const QString machineLink = machineUrlForId(snapshot.id);
    const QString encodedLink = QString::fromUtf8(QUrl::toPercentEncoding(machineLink));
    const QString qrImageUrl = QString("https://api.qrserver.com/v1/create-qr-code/?size=260x260&data=%1").arg(encodedLink);

    // Construction simplifiée de l'historique pour l'exemple
    QString timelineHtml;
    for (const auto &item : timeline) {
        timelineHtml += QString("<p><b>%1</b>: %2</p>").arg(item.when.toString("dd/MM HH:mm"), item.detail);
    }

    return QString(R"HTML(
        <!DOCTYPE html>
        <html>
        <head>
            <meta charset="UTF-8">
            <meta name="viewport" content="width=device-width, initial-scale=1.0">
            <title>Machine %1</title>
            <style>
                body { font-family: sans-serif; background: #f0f4f2; padding: 20px; text-align: center; }
                .card { background: white; border-radius: 15px; padding: 20px; box-shadow: 0 4px 10px rgba(0,0,0,0.1); max-width: 400px; margin: auto; }
                .qr img { width: 200px; }
                .status { font-weight: bold; color: green; }
            </style>
        </head>
        <body>
            <div class="card">
                <h1>%1</h1>
                <div class="qr"><img src="%2"></div>
                <p>Etat: <span class="status">%3</span></p>
                <p>Température: %4°C</p>
                <hr>
                <h3>Historique</h3>
                %5
            </div>
        </body>
        </html>
    )HTML")
    .arg(snapshot.nom, qrImageUrl, snapshot.etatMarche, QString::number(snapshot.temperatureActuelle, 'f', 1), timelineHtml);
}

QString MachineServer::h(const QString &value) {
    QString escaped = value;
    escaped.replace('&', "&amp;").replace('<', "&lt;").replace('>', "&gt;").replace('"', "&quot;").replace('\'', "&#39;");
    return escaped;
}