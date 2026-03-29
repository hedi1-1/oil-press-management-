/**
 * @file main.cpp
 * @brief Point d'entrée de l'application Smart Oil Press Finance
 * 
 * Initialise Qt, configure l'application, et lance la fenêtre principale
 * avec la base de données SQLite.
 */

#include "mainwindow.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <QDir>

/**
 * Point d'entrée de l'application
 * - Initialise QApplication
 * - Configure locale et traductions
 * - Prépare le dossier et le chemin de la base de données
 * - Affiche la fenêtre principale
 * @param argc Nombre d'arguments en ligne de commande
 * @param argv Tableau des arguments en ligne de commande
 * @return Code de sortie (exec retourne 0 si succès)
 */

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Configuration de l'application
    // Nom, version et organisation pour les fichiers de configuration
    QApplication::setApplicationName("PressIQ Finance");
    QApplication::setApplicationVersion("1.0");
    QApplication::setOrganizationName("PressIQ");

    // Support multilingue (optionnel)
    // Charge automatiquement les traductions disponibles si présentes
    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "PressIQ_Finance_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }

    // Préparer le répertoire utilisateur pour les données
    // ~/.smart_oil_finance/ sera créé s'il n'existe pas
    QDir d(QDir::homePath() + "/.smart_oil_finance");
    if (!d.exists()) d.mkpath(".");
    QString dbPath = d.filePath("finance.db");

    // Affichage de la fenêtre principale en passant le chemin de la BDD
    MainWindow w(dbPath);
    w.show();

    return a.exec();
}
