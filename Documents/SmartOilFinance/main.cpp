/**
 * @file main.cpp
 * @brief Point d'entrée de l'application
 * 
 * Smart Oil Finance - Système de Gestion Financière
 * Version 1.0
 * 
 * Cette application gère les transactions financières pour un système
 * de presse d'huile et fournit des analyses et prévisions.
 */

#include "mainwindow.h"
#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <QSplashScreen>
#include <QPixmap>
#include <QThread>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Charger les traductions
    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages)
    {
        const QString baseName = "SmartOilFinance_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName))
        {
            app.installTranslator(&translator);
            break;
        }
    }
    
    // Créer et afficher la fenêtre principale
    MainWindow window;
    window.setWindowTitle("Smart Oil Finance - Gestion Financière v1.0");
    window.show();
    
    return app.exec();
}
