#include "login.h"

#include <QApplication>
#include <QMessageBox>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Login w;

    QObject::connect(&w, &Login::goToMenu, [&](const QString &username, const QString &role) {
        QMessageBox::information(nullptr, "Connexion réussie",
                                 QString("Connecté en tant que: %1 (%2)").arg(username, role));
        QCoreApplication::quit();
    });

    w.show();
    return a.exec();
}
