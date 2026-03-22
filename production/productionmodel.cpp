#include "productionmodel.h"
#include <QRegularExpression>

// ============================================================================
// CONSTRUCTORS
// ============================================================================

ProductionModel::ProductionModel()
    : m_idProduction(0)
    , m_idClient(0)
    , m_idUser(0)
    , m_idMachine(0)
    , m_idStock(0)
    , m_dateProduction(QDate::currentDate())
    , m_quantiteOlivesKg(0)
    , m_typePressage("")
    , m_notesPlanification("")
    , m_heureDemarrage(QTime::currentTime())
    , m_dureeEstimee(0)
    , m_tempsEcoule(0)
    , m_statut("En attente")
    , m_huileProduiteL(0.0)
    , m_rendement(0.0)
    , m_qualite("")
    , m_conformeNormes(false)
    , m_remarquesQualite("")
    , m_dateGenerationRapport(QDate::currentDate())
{
}

ProductionModel::ProductionModel(int idProduction, int idClient, int idUser, int idMachine, int idStock,
                                 QDate dateProduction, int quantiteOlivesKg, QString typePressage,
                                 QString notesPlanification, QTime heureDemarrage, int dureeEstimee,
                                 int tempsEcoule, QString statut, double huileProduiteL, double rendement,
                                 QString qualite, bool conformeNormes, QString remarquesQualite,
                                 QDate dateGenerationRapport)
    : m_idProduction(idProduction)
    , m_idClient(idClient)
    , m_idUser(idUser)
    , m_idMachine(idMachine)
    , m_idStock(idStock)
    , m_dateProduction(dateProduction)
    , m_quantiteOlivesKg(quantiteOlivesKg)
    , m_typePressage(typePressage)
    , m_notesPlanification(notesPlanification)
    , m_heureDemarrage(heureDemarrage)
    , m_dureeEstimee(dureeEstimee)
    , m_tempsEcoule(tempsEcoule)
    , m_statut(statut)
    , m_huileProduiteL(huileProduiteL)
    , m_rendement(rendement)
    , m_qualite(qualite)
    , m_conformeNormes(conformeNormes)
    , m_remarquesQualite(remarquesQualite)
    , m_dateGenerationRapport(dateGenerationRapport)
{
}

// ============================================================================
// GETTERS
// ============================================================================

int ProductionModel::getIdProduction() const { return m_idProduction; }
int ProductionModel::getIdClient() const { return m_idClient; }
int ProductionModel::getIdUser() const { return m_idUser; }
int ProductionModel::getIdMachine() const { return m_idMachine; }
int ProductionModel::getIdStock() const { return m_idStock; }
QDate ProductionModel::getDateProduction() const { return m_dateProduction; }
int ProductionModel::getQuantiteOlivesKg() const { return m_quantiteOlivesKg; }
QString ProductionModel::getTypePressage() const { return m_typePressage; }
QString ProductionModel::getNotesPlanification() const { return m_notesPlanification; }
QTime ProductionModel::getHeureDemarrage() const { return m_heureDemarrage; }
int ProductionModel::getDureeEstimee() const { return m_dureeEstimee; }
int ProductionModel::getTempsEcoule() const { return m_tempsEcoule; }
QString ProductionModel::getStatut() const { return m_statut; }
double ProductionModel::getHuileProduiteL() const { return m_huileProduiteL; }
double ProductionModel::getRendement() const { return m_rendement; }
QString ProductionModel::getQualite() const { return m_qualite; }
bool ProductionModel::getConformeNormes() const { return m_conformeNormes; }
QString ProductionModel::getRemarquesQualite() const { return m_remarquesQualite; }
QDate ProductionModel::getDateGenerationRapport() const { return m_dateGenerationRapport; }

// ============================================================================
// SETTERS
// ============================================================================

void ProductionModel::setIdProduction(int value) { m_idProduction = value; }
void ProductionModel::setIdClient(int value) { m_idClient = value; }
void ProductionModel::setIdUser(int value) { m_idUser = value; }
void ProductionModel::setIdMachine(int value) { m_idMachine = value; }
void ProductionModel::setIdStock(int value) { m_idStock = value; }
void ProductionModel::setDateProduction(QDate value) { m_dateProduction = value; }
void ProductionModel::setQuantiteOlivesKg(int value) { m_quantiteOlivesKg = value; }
void ProductionModel::setTypePressage(QString value) { m_typePressage = value; }
void ProductionModel::setNotesPlanification(QString value) { m_notesPlanification = value; }
void ProductionModel::setHeureDemarrage(QTime value) { m_heureDemarrage = value; }
void ProductionModel::setDureeEstimee(int value) { m_dureeEstimee = value; }
void ProductionModel::setTempsEcoule(int value) { m_tempsEcoule = value; }
void ProductionModel::setStatut(QString value) { m_statut = value; }
void ProductionModel::setHuileProduiteL(double value) { m_huileProduiteL = value; }
void ProductionModel::setRendement(double value) { m_rendement = value; }
void ProductionModel::setQualite(QString value) { m_qualite = value; }
void ProductionModel::setConformeNormes(bool value) { m_conformeNormes = value; }
void ProductionModel::setRemarquesQualite(QString value) { m_remarquesQualite = value; }
void ProductionModel::setDateGenerationRapport(QDate value) { m_dateGenerationRapport = value; }

// ============================================================================
// UTILITY - AUTO CALCULATE RENDEMENT
// ============================================================================

void ProductionModel::calculateRendement()
{
    if (m_quantiteOlivesKg > 0) {
        m_rendement = (m_huileProduiteL / static_cast<double>(m_quantiteOlivesKg)) * 100.0;
    } else {
        m_rendement = 0.0;
        qDebug() << "Warning: Cannot calculate rendement - quantiteOlivesKg is zero or negative.";
    }
}

// ============================================================================
// CRUD - ADD PRODUCTION
// ============================================================================

bool ProductionModel::addProduction()
{
    // Auto-calculate rendement before insert
    calculateRendement();
    
    // Clean typePressage - remove emojis for Oracle compatibility
    QString cleanTypePressage = m_typePressage;
    cleanTypePressage.remove(QRegularExpression("[^a-zA-Z0-9\\s]")); // Keep only ASCII letters, numbers, spaces
    cleanTypePressage = cleanTypePressage.trimmed();
    if (cleanTypePressage.isEmpty()) cleanTypePressage = "Standard";
    
    QSqlQuery query(QSqlDatabase::database("production_conn"));
    // Simplified INSERT for ODBC compatibility
    query.prepare("INSERT INTO PRODUCTION ("
                  "DATEPRODUCTION, QUANTITEOLIVESKG, TYPEPRESSAGE, "
                  "DUREEESTIMEE, TEMPSECOULE, STATUT, "
                  "HUILEPRODUITEL, RENDEMENT, CONFORMENORMES"
                  ") VALUES ("
                  "SYSDATE, :quantiteOlivesKg, :typePressage, "
                  ":dureeEstimee, :tempsEcoule, :statut, "
                  ":huileProduiteL, :rendement, :conformeNormes"
                  ")");
    
    query.bindValue(":quantiteOlivesKg", m_quantiteOlivesKg);
    query.bindValue(":typePressage", cleanTypePressage);
    query.bindValue(":dureeEstimee", m_dureeEstimee);
    query.bindValue(":tempsEcoule", m_tempsEcoule);
    query.bindValue(":statut", m_statut);
    query.bindValue(":huileProduiteL", m_huileProduiteL);
    query.bindValue(":rendement", m_rendement);
    query.bindValue(":conformeNormes", m_conformeNormes ? 1 : 0);
    
    qDebug() << "Executing INSERT query...";
    qDebug() << "QuantiteOlives:" << m_quantiteOlivesKg << "TypePressage:" << cleanTypePressage << "Statut:" << m_statut;
    
    if (query.exec()) {
        // Get the generated ID
        QSqlQuery idQuery(QSqlDatabase::database("production_conn"));
        idQuery.exec("SELECT SEQ_PRODUCTION.CURRVAL FROM DUAL");
        if (idQuery.next()) {
            m_idProduction = idQuery.value(0).toInt();
        }
        qDebug() << "Production added successfully! ID:" << m_idProduction;
        return true;
    } else {
        qDebug() << "=== SQL ERROR ===";
        qDebug() << "Error:" << query.lastError().text();
        qDebug() << "Driver:" << query.lastError().driverText();
        qDebug() << "Database:" << query.lastError().databaseText();
        qDebug() << "Native code:" << query.lastError().nativeErrorCode();
        return false;
    }
}

// ============================================================================
// CRUD - UPDATE PRODUCTION
// ============================================================================

bool ProductionModel::updateProduction()
{
    // Auto-calculate rendement before update
    calculateRendement();
    
    // Clean typePressage
    QString cleanTypePressage = m_typePressage;
    cleanTypePressage.remove(QRegularExpression("[^a-zA-Z0-9\\s]"));
    cleanTypePressage = cleanTypePressage.trimmed();
    if (cleanTypePressage.isEmpty()) cleanTypePressage = "Standard";
    
    QSqlQuery query(QSqlDatabase::database("production_conn"));
    query.prepare("UPDATE PRODUCTION SET "
                  "QUANTITEOLIVESKG = :quantiteOlivesKg, "
                  "TYPEPRESSAGE = :typePressage, "
                  "DUREEESTIMEE = :dureeEstimee, "
                  "TEMPSECOULE = :tempsEcoule, "
                  "STATUT = :statut, "
                  "HUILEPRODUITEL = :huileProduiteL, "
                  "RENDEMENT = :rendement, "
                  "QUALITE = :qualite, "
                  "CONFORMENORMES = :conformeNormes "
                  "WHERE IDPRODUCTION = :idProduction");
    
    query.bindValue(":idProduction", m_idProduction);
    query.bindValue(":quantiteOlivesKg", m_quantiteOlivesKg);
    query.bindValue(":typePressage", cleanTypePressage);
    query.bindValue(":dureeEstimee", m_dureeEstimee);
    query.bindValue(":tempsEcoule", m_tempsEcoule);
    query.bindValue(":statut", m_statut);
    query.bindValue(":huileProduiteL", m_huileProduiteL);
    query.bindValue(":rendement", m_rendement);
    query.bindValue(":qualite", m_qualite.isEmpty() ? "Standard" : m_qualite);
    query.bindValue(":conformeNormes", m_conformeNormes ? 1 : 0);
    
    if (query.exec()) {
        qDebug() << "Production updated successfully! ID:" << m_idProduction;
        return true;
    } else {
        qDebug() << "Error updating production:" << query.lastError().text();
        return false;
    }
}

// ============================================================================
// CRUD - DELETE PRODUCTION (SAFE DELETE)
// ============================================================================

bool ProductionModel::deleteProduction(int idProduction)
{
    QSqlQuery query(QSqlDatabase::database("production_conn"));
    query.prepare("DELETE FROM PRODUCTION WHERE IDPRODUCTION = :idProduction");
    query.bindValue(":idProduction", idProduction);
    
    if (query.exec()) {
        if (query.numRowsAffected() > 0) {
            qDebug() << "Production deleted successfully! ID:" << idProduction;
            return true;
        } else {
            qDebug() << "No production found with ID:" << idProduction;
            return false;
        }
    } else {
        qDebug() << "Error deleting production:" << query.lastError().text();
        if (query.lastError().text().contains("foreign key", Qt::CaseInsensitive) ||
            query.lastError().text().contains("constraint", Qt::CaseInsensitive) ||
            query.lastError().text().contains("integrity", Qt::CaseInsensitive)) {
            qDebug() << "FK Constraint: This production is referenced by other records. Cannot delete.";
        }
        return false;
    }
}

// ============================================================================
// CRUD - GET ALL PRODUCTIONS
// ============================================================================

QSqlQueryModel* ProductionModel::getAllProductions()
{
    QSqlQueryModel* model = new QSqlQueryModel();
    
    model->setQuery("SELECT "
                    "IDPRODUCTION AS \"ID\", "
                    "ID_CLIENT AS \"Client\", "
                    "ID_USER AS \"Utilisateur\", "
                    "ID_MACHINE AS \"Machine\", "
                    "ID_STOCK AS \"Stock\", "
                    "DATEPRODUCTION AS \"Date Production\", "
                    "QUANTITEOLIVESKG AS \"Quantité (kg)\", "
                    "TYPEPRESSAGE AS \"Type Pressage\", "
                    "STATUT AS \"Statut\", "
                    "HUILEPRODUITEL AS \"Huile (L)\", "
                    "RENDEMENT AS \"Rendement (%)\", "
                    "QUALITE AS \"Qualité\", "
                    "CONFORMENORMES AS \"Conforme\" "
                    "FROM PRODUCTION ORDER BY IDPRODUCTION DESC",
                    QSqlDatabase::database("production_conn"));
    
    if (model->lastError().isValid()) {
        qDebug() << "Error fetching all productions:" << model->lastError().text();
    }
    
    return model;
}

// ============================================================================
// CRUD - SEARCH PRODUCTIONS
// ============================================================================

QSqlQueryModel* ProductionModel::searchProductions(const QString& keyword)
{
    QSqlQueryModel* model = new QSqlQueryModel();
    
    QSqlQuery query(QSqlDatabase::database("production_conn"));
    query.prepare("SELECT "
                  "IDPRODUCTION AS \"ID\", "
                  "ID_CLIENT AS \"Client\", "
                  "ID_USER AS \"Utilisateur\", "
                  "ID_MACHINE AS \"Machine\", "
                  "DATEPRODUCTION AS \"Date Production\", "
                  "QUANTITEOLIVESKG AS \"Quantité (kg)\", "
                  "TYPEPRESSAGE AS \"Type Pressage\", "
                  "STATUT AS \"Statut\", "
                  "HUILEPRODUITEL AS \"Huile (L)\", "
                  "RENDEMENT AS \"Rendement (%)\", "
                  "QUALITE AS \"Qualité\" "
                  "FROM PRODUCTION "
                  "WHERE UPPER(TYPEPRESSAGE) LIKE UPPER(:keyword) "
                  "OR UPPER(STATUT) LIKE UPPER(:keyword2) "
                  "OR UPPER(QUALITE) LIKE UPPER(:keyword3) "
                  "OR UPPER(NOTESPLANIFICATION) LIKE UPPER(:keyword4) "
                  "OR TO_CHAR(IDPRODUCTION) LIKE :keyword5 "
                  "ORDER BY IDPRODUCTION DESC");
    
    QString searchPattern = "%" + keyword + "%";
    query.bindValue(":keyword", searchPattern);
    query.bindValue(":keyword2", searchPattern);
    query.bindValue(":keyword3", searchPattern);
    query.bindValue(":keyword4", searchPattern);
    query.bindValue(":keyword5", searchPattern);
    
    if (query.exec()) {
        model->setQuery(std::move(query));
    } else {
        qDebug() << "Error searching productions:" << query.lastError().text();
    }
    
    return model;
}

// ============================================================================
// GET TABLE MODEL FOR QTableView
// ============================================================================

QSqlTableModel* ProductionModel::getTableModel(QObject* parent)
{
    QSqlTableModel* model = new QSqlTableModel(parent, QSqlDatabase::database("production_conn"));
    model->setTable("PRODUCTION");
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    
    // Set column headers
    model->setHeaderData(0, Qt::Horizontal, "ID Production");
    model->setHeaderData(1, Qt::Horizontal, "ID Client");
    model->setHeaderData(2, Qt::Horizontal, "ID Utilisateur");
    model->setHeaderData(3, Qt::Horizontal, "ID Machine");
    model->setHeaderData(4, Qt::Horizontal, "ID Stock");
    model->setHeaderData(5, Qt::Horizontal, "Date Production");
    model->setHeaderData(6, Qt::Horizontal, "Quantité Olives (kg)");
    model->setHeaderData(7, Qt::Horizontal, "Type Pressage");
    model->setHeaderData(8, Qt::Horizontal, "Notes Planification");
    model->setHeaderData(9, Qt::Horizontal, "Heure Démarrage");
    model->setHeaderData(10, Qt::Horizontal, "Durée Estimée");
    model->setHeaderData(11, Qt::Horizontal, "Temps Écoulé");
    model->setHeaderData(12, Qt::Horizontal, "Statut");
    model->setHeaderData(13, Qt::Horizontal, "Huile Produite (L)");
    model->setHeaderData(14, Qt::Horizontal, "Rendement (%)");
    model->setHeaderData(15, Qt::Horizontal, "Qualité");
    model->setHeaderData(16, Qt::Horizontal, "Conforme");
    model->setHeaderData(17, Qt::Horizontal, "Remarques Qualité");
    model->setHeaderData(18, Qt::Horizontal, "Date Rapport");
    
    model->select();
    
    if (model->lastError().isValid()) {
        qDebug() << "Error loading table model:" << model->lastError().text();
    }
    
    return model;
}
