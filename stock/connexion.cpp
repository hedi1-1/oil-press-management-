#include "connexion.h"

// Initialisation de l'instance statique à nullptr
Connexion *Connexion::instance = nullptr;

// Constructeur privé
Connexion::Connexion() {
  // Le constructeur ne fait rien de spécial
  // La connexion sera établie via createConnection()
}

// Destructeur
Connexion::~Connexion() { closeConnection(); }

// Méthode getInstance pour récupérer l'instance unique (Singleton)
Connexion *Connexion::getInstance() {
  if (instance == nullptr) {
    instance = new Connexion();
  }
  return instance;
}

// Méthode pour créer la connexion à la base de données Oracle
bool Connexion::createConnection() {
  // Vérifier si la connexion existe déjà
  if (db.isOpen()) {
    qDebug() << "La connexion est déjà établie.";
    return true;
  }

  // Créer la connexion avec le driver ODBC via DSN Projet2A
  db = QSqlDatabase::addDatabase("QODBC", "stock_conn");
  db.setDatabaseName("projet");
  db.setUserName("zitouna");
  db.setPassword("zitouna");

  // Tenter d'ouvrir la connexion
  if (db.open()) {
    qDebug() << "========================================";
    qDebug() << "✓ Connexion à la base de données réussie !";
    qDebug() << "✓ DSN : Projet2A";
    qDebug() << "✓ User : amal";
    qDebug() << "✓ Driver : " << db.driverName();
    qDebug() << "========================================";

    // Message de succès optionnel (commentez si vous ne voulez pas de popup)
    // QMessageBox::information(nullptr, "Connexion", "Connexion à la base de
    // données établie avec succès !");

    return true;
  } else {
    // Erreur de connexion
    QString errorMsg = db.lastError().text();
    qDebug() << "========================================";
    qDebug() << "✗ ERREUR : Échec de la connexion à la base de données !";
    qDebug() << "✗ Détails de l'erreur : " << errorMsg;
    qDebug() << "========================================";

    // Afficher un message d'erreur à l'utilisateur
    QMessageBox::critical(
        nullptr, "Erreur de connexion",
        "Impossible de se connecter à la base de données.\n\n"
        "Détails : " +
            errorMsg +
            "\n\n"
            "Vérifiez :\n"
            "- Le service Oracle est démarré\n"
            "- Les identifiants (amal/qwerty123456789) sont corrects\n"
            "- La source ODBC 'Projet2A' est configurée\n"
            "- Le service Oracle est accessible");

    return false;
  }
}

// Méthode pour fermer la connexion
void Connexion::closeConnection() {
  if (db.isOpen()) {
    QString dbName = db.databaseName();
    db.close();
    qDebug() << "========================================";
    qDebug() << "✓ Connexion fermée : " << dbName;
    qDebug() << "========================================";
  }
}

// Getter pour récupérer l'objet QSqlDatabase
QSqlDatabase Connexion::getDatabase() { return db; }

// Vérifier si la connexion est ouverte
bool Connexion::isOpen() const { return db.isOpen(); }
