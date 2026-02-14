#ifndef PRODUCTIONMODEL_H
#define PRODUCTIONMODEL_H

#include <QString>
#include <QDate>
#include <QTime>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QSqlTableModel>
#include <QSqlError>
#include <QDebug>

class ProductionModel
{
public:
    // Constructors
    ProductionModel();
    ProductionModel(int idProduction, int idClient, int idUser, int idMachine, int idStock,
                    QDate dateProduction, int quantiteOlivesKg, QString typePressage,
                    QString notesPlanification, QTime heureDemarrage, int dureeEstimee,
                    int tempsEcoule, QString statut, double huileProduiteL, double rendement,
                    QString qualite, bool conformeNormes, QString remarquesQualite,
                    QDate dateGenerationRapport);

    // Getters
    int getIdProduction() const;
    int getIdClient() const;
    int getIdUser() const;
    int getIdMachine() const;
    int getIdStock() const;
    QDate getDateProduction() const;
    int getQuantiteOlivesKg() const;
    QString getTypePressage() const;
    QString getNotesPlanification() const;
    QTime getHeureDemarrage() const;
    int getDureeEstimee() const;
    int getTempsEcoule() const;
    QString getStatut() const;
    double getHuileProduiteL() const;
    double getRendement() const;
    QString getQualite() const;
    bool getConformeNormes() const;
    QString getRemarquesQualite() const;
    QDate getDateGenerationRapport() const;

    // Setters
    void setIdProduction(int value);
    void setIdClient(int value);
    void setIdUser(int value);
    void setIdMachine(int value);
    void setIdStock(int value);
    void setDateProduction(QDate value);
    void setQuantiteOlivesKg(int value);
    void setTypePressage(QString value);
    void setNotesPlanification(QString value);
    void setHeureDemarrage(QTime value);
    void setDureeEstimee(int value);
    void setTempsEcoule(int value);
    void setStatut(QString value);
    void setHuileProduiteL(double value);
    void setRendement(double value);
    void setQualite(QString value);
    void setConformeNormes(bool value);
    void setRemarquesQualite(QString value);
    void setDateGenerationRapport(QDate value);

    // CRUD Operations
    bool addProduction();
    bool updateProduction();
    static bool deleteProduction(int idProduction);
    static QSqlQueryModel* getAllProductions();
    static QSqlQueryModel* searchProductions(const QString& keyword);
    static QSqlTableModel* getTableModel(QObject* parent = nullptr);

    // Utility
    void calculateRendement();

private:
    int m_idProduction;
    int m_idClient;
    int m_idUser;
    int m_idMachine;
    int m_idStock;
    QDate m_dateProduction;
    int m_quantiteOlivesKg;
    QString m_typePressage;
    QString m_notesPlanification;
    QTime m_heureDemarrage;
    int m_dureeEstimee;
    int m_tempsEcoule;
    QString m_statut;
    double m_huileProduiteL;
    double m_rendement;
    QString m_qualite;
    bool m_conformeNormes;
    QString m_remarquesQualite;
    QDate m_dateGenerationRapport;
};

#endif // PRODUCTIONMODEL_H
