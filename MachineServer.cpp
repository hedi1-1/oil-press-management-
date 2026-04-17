#include "MachineServer.h"

#include "connexionmachine.h"

#include <QDate>
#include <QHostAddress>
#include <QHttpServerRequest>
#include <QHttpServerResponder>
#include <QHttpServerResponse>
#include <QSqlError>
#include <QSqlQuery>
#include <QUrl>
#include <QUrlQuery>
#include <algorithm>

MachineServer::MachineServer(QObject *parent)
  : QObject(parent), m_hostIp("127.0.0.1"), m_port(8181), m_started(false),
      m_routeRegistered(false) {}

bool MachineServer::start(quint16 port) {
  if (m_started) {
    return true;
  }

  m_port = port;

  if (!m_routeRegistered) {
    m_server.route("/machine", [this](const QHttpServerRequest &request) {
      return handleMachineRequest(request);
    });
    m_routeRegistered = true;
  }

  const bool listening = m_server.listen(QHostAddress::Any, m_port);
  m_started = listening;
  return listening;
}

void MachineServer::setHostIp(const QString &hostIp) {
  if (!hostIp.trimmed().isEmpty()) {
    m_hostIp = hostIp.trimmed();
  }
}

QString MachineServer::hostIp() const { return m_hostIp; }

QString MachineServer::machineUrlForId(const QString &machineId) const {
  return QString("http://%1:%2/machine?id=%3")
      .arg(m_hostIp)
      .arg(m_port)
      .arg(machineId);
}

QHttpServerResponse
MachineServer::handleMachineRequest(const QHttpServerRequest &request) {
  const QUrlQuery urlQuery(request.url());
  const QString machineId = urlQuery.queryItemValue("id").trimmed();

  if (machineId.isEmpty()) {
    return QHttpServerResponse("text/plain; charset=UTF-8",
                               QByteArray("Parametre id manquant."));
  }

  MachineSnapshot snapshot;
  QString error;
  if (!fetchMachineById(machineId, &snapshot, &error)) {
    const QByteArray message = error.isEmpty()
                                   ? QByteArray("Machine introuvable.")
                                   : error.toUtf8();
    return QHttpServerResponse("text/plain; charset=UTF-8",
                   message);
  }

  emit machineRequested(snapshot.id);

  const QList<MachineTimelineItem> timeline = buildTimeline(snapshot);
  const QString html = buildMachineHtml(snapshot, timeline);
  return QHttpServerResponse("text/html; charset=UTF-8", html.toUtf8());
}

bool MachineServer::fetchMachineById(const QString &machineId,
                                     MachineSnapshot *snapshot,
                                     QString *error) const {
  if (!snapshot) {
    if (error) {
      *error = "Snapshot invalide.";
    }
    return false;
  }

  QSqlQuery query(ConnectionMachine::getInstance().getDatabase());
  query.prepare(
      "SELECT ID_MACHINE, NOM_MACHINE, TYPE_MACHINE, ETAT_MARCHE, "
      "TEMPERATURE_ACTUELLE, NIVEAU_CHARGE, ETAT_FONCTIONNEMENT, TYPE_ALERTE, "
      "NIVEAU_CRITICITE, DATE_DERNIERE_MAINTENANCE, DATE_INSTALLATION, "
      "SCORE_SANTE, DATE_MISE_A_JOUR, USER_ID, REFROIDISSEMENT, TAG, PRIORITE "
      "FROM MACHINE WHERE ID_MACHINE = :id");
  query.bindValue(":id", machineId);

  if (!query.exec()) {
    if (error) {
      *error = QString("Erreur Oracle: %1").arg(query.lastError().text());
    }
    return false;
  }

  if (!query.next()) {
    if (error) {
      *error = QString("Aucune machine pour ID=%1").arg(machineId);
    }
    return false;
  }

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

QList<MachineTimelineItem>
MachineServer::buildTimeline(const MachineSnapshot &snapshot) const {
  QList<MachineTimelineItem> timeline;

  const QDateTime now = QDateTime::currentDateTime();
  const QDateTime updateTime = snapshot.dateMiseAJour.isValid() ? snapshot.dateMiseAJour : now;

  if (snapshot.dateDerniereMaintenance.isValid()) {
    MachineTimelineItem item;
    item.when = snapshot.dateDerniereMaintenance;
    item.title = "Maintenance";
    item.detail = QString("Intervention de maintenance enregistree le %1")
                      .arg(snapshot.dateDerniereMaintenance.toString("dd/MM/yyyy HH:mm"));
    timeline.append(item);
  }

  if (!snapshot.typeAlerte.trimmed().isEmpty() && snapshot.typeAlerte.trimmed().toLower() != "aucune") {
    MachineTimelineItem item;
    item.when = updateTime.addSecs(-1200);
    item.title = "Alerte";
    item.detail = QString("Alerte detectee: %1 (criticite: %2)")
                      .arg(snapshot.typeAlerte, snapshot.niveauCriticite);
    timeline.append(item);
  }

  {
    MachineTimelineItem item;
    item.when = updateTime.addSecs(-600);
    item.title = "Etat";
    item.detail = QString("Etat de marche: %1 | Fonctionnement: %2")
                      .arg(snapshot.etatMarche, snapshot.etatFonctionnement);
    timeline.append(item);
  }

  {
    MachineTimelineItem item;
    item.when = updateTime.addSecs(-180);
    item.title = "Score sante";
    item.detail = QString("Mise a jour score sante: %1/100")
                      .arg(snapshot.scoreSante);
    timeline.append(item);
  }

  {
    MachineTimelineItem item;
    item.when = updateTime;
    item.title = "Synchronisation";
    item.detail = QString("Derniere synchronisation de la machine.");
    timeline.append(item);
  }

  std::sort(timeline.begin(), timeline.end(),
            [](const MachineTimelineItem &a, const MachineTimelineItem &b) {
              return a.when > b.when;
            });

  return timeline;
}

QString MachineServer::stateBadgeClass(const QString &state) const {
  const QString normalized = state.trimmed().toUpper();
  if (normalized == "ON") {
    return "state-on";
  }
  if (normalized == "OFF") {
    return "state-off";
  }
  return "state-standby";
}

QString MachineServer::buildMachineHtml(const MachineSnapshot &snapshot,
                                        const QList<MachineTimelineItem> &timeline) const {
  const double santePct = std::clamp(static_cast<double>(snapshot.scoreSante), 0.0, 100.0);

  const QString fonctionnement = snapshot.etatFonctionnement.trimmed();
  const QString fonctionnementLower = fonctionnement.toLower();
  const QString scoreAccent = santePct >= 75.0 ? "#19c79f" : (santePct >= 45.0 ? "#f4aa3c" : "#ff6f6f");

  const QString alertBadgeText = snapshot.typeAlerte.trimmed().isEmpty()
                                     ? QString("Aucune")
                                     : snapshot.typeAlerte;
  const QString alertBadgeClass = alertBadgeText.toLower() == "aucune" ? "badge-ok" : "badge-warn";

  const QString fonctionnementClass =
      fonctionnementLower == "normal"
          ? "value-ok"
          : (fonctionnementLower == "alerte" ? "value-warn" : "value-danger");

  const QString refroidissementText = snapshot.refroidissement.trimmed().isEmpty()
                                          ? QString("Off")
                                          : snapshot.refroidissement;
  const QString refroidissementClass =
      refroidissementText.trimmed().toLower().contains("on") ? "value-ok" : "value-warn";

  const QString prioriteText = snapshot.priorite.trimmed().isEmpty() ? QString("-") : snapshot.priorite;
  const QString criticiteText = snapshot.niveauCriticite.trimmed().isEmpty()
                                    ? QString("-")
                                    : snapshot.niveauCriticite;

  const QString dateMaintenance = snapshot.dateDerniereMaintenance.isValid()
                                      ? snapshot.dateDerniereMaintenance.toString("dd/MM/yyyy")
                                      : QString("-");

  const QString updateText = snapshot.dateMiseAJour.isValid()
                                 ? snapshot.dateMiseAJour.toString("dd/MM/yyyy HH:mm")
                                 : QDateTime::currentDateTime().toString("dd/MM/yyyy HH:mm");

    const QString machineLink = machineUrlForId(snapshot.id);
    const QString encodedMachineLink =
      QString::fromUtf8(QUrl::toPercentEncoding(machineLink));
    const QString qrImageUrl =
      QString("https://api.qrserver.com/v1/create-qr-code/?size=260x260&margin=2&data=%1")
        .arg(encodedMachineLink);

  QString timelineHtml;
  if (timeline.isEmpty()) {
    timelineHtml =
        "<div class=\"row\"><div class=\"cell-date\">-</div><div class=\"cell-title\">Aucun evenement recent</div><div class=\"cell-badge badge-ok\">OK</div></div>";
  } else {
    for (qsizetype i = 0; i < timeline.size(); ++i) {
      const MachineTimelineItem &item = timeline.at(i);
      const QString itemLower = item.title.trimmed().toLower();
      QString badgeClass = "badge-neutral";
      QString badgeText = "Info";
      if (itemLower.contains("alerte")) {
        badgeClass = "badge-warn";
        badgeText = "Avert.";
      } else if (itemLower.contains("maintenance")) {
        badgeClass = "badge-ok";
        badgeText = "Fait";
      } else if (itemLower.contains("etat") || itemLower.contains("score") ||
                 itemLower.contains("synchronisation")) {
        badgeClass = "badge-ok";
        badgeText = "OK";
      }

      timelineHtml += QString(
          "<div class=\"row\">"
          "<div class=\"cell-date\">%1</div>"
          "<div class=\"cell-title\">%2</div>"
          "<div class=\"cell-badge %3\">%4</div>"
          "</div>")
                          .arg(h(item.when.toString("dd/MM HH:mm")), h(item.detail), badgeClass,
                               badgeText);
    }
  }

  return QString(R"HTML(
<!DOCTYPE html>
<html lang="fr">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <meta http-equiv="refresh" content="20">
  <title>Machine %1</title>
  <style>
    :root {
      --bg: #eaf4ee;
      --panel: #f7fcf9;
      --line: #c6ded0;
      --text: #163a2c;
      --muted: #4f7364;
      --ok: #118a66;
      --warn: #ca8b2f;
      --danger: #c95151;
      --shadow: rgba(25, 70, 52, 0.14);
    }
    * { box-sizing: border-box; -webkit-font-smoothing: antialiased; }
    body {
      margin: 0;
      min-height: 100vh;
      font-family: "Segoe UI", Tahoma, Arial, sans-serif;
      color: var(--text);
      background:
        radial-gradient(circle at 20%% 0%%, #d9ecdf 0%%, #edf7f1 46%%),
        linear-gradient(160deg, #e3f0e8 0%%, #f3faf6 100%%);
    }
    .wrap { max-width: 860px; margin: 0 auto; padding: 10px; }
    .grid { display: grid; grid-template-columns: 210px 1fr; gap: 10px; }
    .card {
      background: linear-gradient(180deg, #ffffff 0%%, #f5fbf8 100%%);
      border: 1px solid var(--line);
      border-radius: 12px;
      padding: 12px;
      margin-bottom: 10px;
      box-shadow: 0 8px 22px var(--shadow);
    }
    .qr-title { color: var(--muted); text-align:center; font-weight:600; margin-bottom:8px; font-size: 12px; }
    .qr-box {
      width: 130px; height: 130px; margin: 0 auto 10px auto; border-radius: 8px;
      border: 1px solid #bfd7c8;
      background: linear-gradient(90deg, #eef8f2 0%%, #ffffff 100%%);
      display: flex;
      align-items: center;
      justify-content: center;
      overflow: hidden;
      text-decoration: none;
    }
    .qr-box img {
      width: 100%%;
      height: 100%%;
      display: block;
      object-fit: contain;
      object-position: center;
      background: #ffffff;
    }
    .qr-machine-id { text-align:center; font-size: 18px; font-weight: 800; color: #0f5d43; }
    .qr-machine-type { text-align:center; color: var(--muted); margin-top: 4px; font-size: 13px; }
    .health { margin-top: 6px; }
    .health-title { color:#234d3c; font-size: 18px; font-weight: 800; }
    .health-score { color: var(--ok); font-size: 34px; font-weight: 800; line-height: 1; }
    .health-score span { color:#6a8b7d; font-size:18px; font-weight:600; }
    .progress {
      height: 10px; border-radius: 8px; background:#dcece3; overflow:hidden; margin-top: 10px;
    }
    .progress-fill {
      height: 100%%;
      width: %2%%;
      background: linear-gradient(90deg, #10b58f 0%%, #3ad2ad 100%%);
      border-radius: 8px;
    }
    .health-note { margin-top: 8px; color: var(--ok); font-weight: 600; font-size: 13px; }
    .head {
      display: grid; grid-template-columns: 1fr auto; gap: 10px; align-items: center;
      border-bottom: 1px solid var(--line); padding-bottom: 10px;
    }
    .title { margin: 0; font-size: 27px; font-weight: 800; }
    .meta { color: var(--muted); margin-top: 4px; font-size: 13px; }
    .switch {
      font-weight: 700; border-radius: 999px; padding: 6px 15px;
      font-size: 14px; background: #d6efdd; color: #15563f;
    }
    .kv {
      margin-top: 10px; display: flex; gap: 16px; color: #dadada; font-size: 15px;
    }
    .kv span { color: #9d9d9d; }
    .value-ok { color: var(--ok); font-weight: 700; }
    .value-warn { color: var(--warn); font-weight: 700; }
    .value-danger { color: var(--danger); font-weight: 700; }
    .mini-grid { display: grid; grid-template-columns: repeat(3, 1fr); gap: 12px; }
    .mini h3 { margin:0; color:#3d6756; font-size: 15px; font-weight: 600; }
    .mini p { margin: 6px 0 0 0; font-size: 30px; font-weight: 800; }
    .mini small { color:#6f8f82; font-size: 14px; }
    .block-title { margin:0 0 8px 0; font-size: 21px; font-weight: 700; }
    .line {
      display:flex; align-items:center; justify-content:space-between; padding: 10px 0;
      border-top: 1px solid var(--line); font-size: 15px;
    }
    .badge {
      display:inline-flex; align-items:center; justify-content:center; min-width: 92px;
      font-size: 13px; font-weight: 700; border-radius: 999px; padding: 4px 10px;
    }
    .badge-ok { background:#cfeee0; color:#0e6b4d; }
    .badge-warn { background:#f7e5ce; color:#8f5a1a; }
    .badge-neutral { background:#dbece3; color:#376451; }
    .tags { display:flex; gap:8px; flex-wrap:wrap; margin-top: 12px; }
    .chip {
      border-radius: 999px; padding: 4px 10px; font-size: 12px;
      background:#dfeee5; color:#1f6146; font-weight: 700;
    }
    .table { margin-top: 10px; }
    .row {
      display: grid;
      grid-template-columns: 110px 1fr auto;
      gap: 10px;
      align-items: center;
      padding: 8px 0;
      border-top: 1px solid var(--line);
    }
    .cell-date { color:#658879; font-size: 12px; }
    .cell-title { font-size: 14px; }
    .cell-badge { font-size: 12px; }
    .footer-note { margin-top: 8px; color: #5f8374; font-size: 11px; text-align:right; }
    @media (max-width: 640px) {
      .grid { grid-template-columns: 1fr; }
      .mini-grid { grid-template-columns: 1fr; }
      .row { grid-template-columns: 1fr; }
      .title { font-size: 22px; }
      .block-title { font-size: 18px; }
      .line, .cell-title { font-size: 14px; }
      .cell-date, .badge, .cell-badge, .chip { font-size: 12px; }
    }
  </style>
</head>
<body>
  <div class="wrap">
    <div class="grid">
      <div>
        <section class="card">
          <div class="qr-title">Scanner pour acceder</div>
          <a class="qr-box" href="%26" target="_blank" rel="noopener">
            <img src="%27" alt="QR machine %3">
          </a>
          <div class="qr-machine-id">MACHINE-%3</div>
          <div class="qr-machine-type">%4</div>
        </section>

        <section class="card health">
          <div class="health-title">Score de sante</div>
          <div class="health-score">%5<span> /100</span></div>
          <div class="progress"><div class="progress-fill"></div></div>
          <div class="health-note">Bon etat general</div>
        </section>

        <section class="card">
          <div class="block-title">Priorite & criticite</div>
          <div class="line"><span>Priorite</span><span class="badge badge-ok">%6</span></div>
          <div class="line"><span>Criticite</span><span class="badge badge-ok">%7</span></div>
          <div class="tags">
            <span class="chip">%8</span>
            <span class="chip">%9</span>
          </div>
        </section>
      </div>

      <div>
        <section class="card">
          <div class="head">
            <div>
              <h1 class="title">%10</h1>
              <div class="meta">ID: %3 · Type: %4 · Installee: %11</div>
            </div>
            <div class="switch">%12</div>
          </div>
          <div class="kv">
            <div>Etat fonctionnement: <span class="%13">%14</span></div>
            <div>Refroidissement: <span class="%15">%16</span></div>
          </div>
        </section>

        <section class="mini-grid">
          <div class="card mini">
            <h3>Temperature</h3>
            <p>%17<small> °C</small></p>
          </div>
          <div class="card mini">
            <h3>Niveau de charge</h3>
            <p>%18<small> %%</small></p>
          </div>
          <div class="card mini">
            <h3>Derniere MAJ</h3>
            <p style="font-size:16px;">%19</p>
          </div>
        </section>

        <section class="card">
          <h2 class="block-title">Alertes & maintenance</h2>
          <div class="line"><span>Type d'alerte</span><span class="badge %20">%21</span></div>
          <div class="line"><span>Derniere maintenance</span><strong>%22</strong></div>
          <div class="line"><span>Responsable (user ID)</span><strong>#%23</strong></div>
        </section>

        <section class="card">
          <h2 class="block-title">Historique complet</h2>
          <div class="table">%24</div>
        </section>

        <div class="footer-note">Actualise: %25 (auto-refresh 20 s)</div>
      </div>
    </div>
  </div>
</body>
</html>
)HTML")
      .arg(h(snapshot.id),
           QString::number(santePct, 'f', 0),
           h(snapshot.id),
           h(snapshot.type.trimmed().isEmpty() ? QString("Non defini") : snapshot.type),
           QString::number(snapshot.scoreSante),
           h(prioriteText),
           h(criticiteText),
           h(snapshot.tag.trimmed().isEmpty() ? QString("Atelier A") : snapshot.tag),
           h(snapshot.type.trimmed().isEmpty() ? QString("Ligne 3") : snapshot.type),
           h(snapshot.nom.trimmed().isEmpty() ? QString("Machine") : snapshot.nom),
           h(snapshot.dateInstallation.isValid() ? snapshot.dateInstallation.toString("dd/MM/yyyy") : QString("-")),
           h(snapshot.etatMarche.trimmed().isEmpty() ? QString("OFF") : snapshot.etatMarche),
           fonctionnementClass,
           h(fonctionnement.isEmpty() ? QString("Normal") : fonctionnement),
           refroidissementClass,
           h(refroidissementText),
           QString::number(snapshot.temperatureActuelle, 'f', 1),
           QString::number(snapshot.niveauCharge, 'f', 1),
           h(updateText),
           alertBadgeClass,
           h(alertBadgeText),
           h(dateMaintenance),
           h(snapshot.userId.trimmed().isEmpty() ? QString("-") : snapshot.userId),
           timelineHtml,
           h(updateText),
           h(machineLink),
           h(qrImageUrl));
}

QString MachineServer::h(const QString &value) {
  QString escaped = value;
  escaped.replace('&', "&amp;");
  escaped.replace('<', "&lt;");
  escaped.replace('>', "&gt;");
  escaped.replace('"', "&quot;");
  escaped.replace('\'', "&#39;");
  return escaped;
}
