#include "connexionmachine.h"
#include "machine.h"

#include <QApplication>
#include <QDebug>
#include <QMessageBox>
#include <QSqlError>
#include <QSqlQuery>

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);

  bool test = ConnectionMachine::getInstance().createconnect();

  if (test) {
    QMessageBox::information(nullptr, "Connexion",
                             "Connexion à la base de données réussie !");

    // --- Test SELECT sur la table MACHINE ---
    QSqlQuery query(ConnectionMachine::getInstance().getDatabase());
    if (query.exec("SELECT * FROM MACHINE")) {
      int count = 0;
      while (query.next()) {
        count++;
        qDebug() << "Ligne" << count << ":" << query.value(0).toString();
      }
      QMessageBox::information(
          nullptr, "Test SELECT",
          QString("SELECT réussi ! %1 ligne(s) trouvée(s) dans MACHINE.")
              .arg(count));
    } else {
      QMessageBox::warning(nullptr, "Test SELECT",
                           "Erreur SELECT : " + query.lastError().text());
    }

  } else {
    QSqlDatabase db = QSqlDatabase::database();
    QString details = db.lastError().text();
    QString drivers = QSqlDatabase::drivers().join(", ");
    QMessageBox::critical(
        nullptr, "Erreur de connexion",
        "Impossible de se connecter à la base de données.\n\n"
        "Détails : " +
            details +
            "\n\n"
            "Drivers disponibles : " +
            drivers +
            "\n\n"
            "Vérifiez :\n"
            "- Que le service Oracle est démarré\n"
            "- Que les identifiants (amal/qwerty123456789) sont corrects\n"
            "- Que la source ODBC 'Projet2A' est configurée (32 ET 64 bits)\n"
            "- Que le service Oracle est accessible");
  }

  machine w;
  w.show();
  return a.exec();
}
