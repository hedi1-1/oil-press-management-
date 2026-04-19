QT       += core gui sql printsupport charts serialport network

qtHaveModule(httpserver) {
    QT += httpserver
    DEFINES += MACHINE_HAS_HTTPSERVER
}


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    menu.cpp \
    ../Login/login.cpp \
    ../Login/connectionlogin.cpp \
    ../production/production.cpp \
    ../production/connection.cpp \
    ../production/productionmodel.cpp \
    ../production/productioneditdialog.cpp \
    ../production/qualityaiagent.cpp \
    ../userstaff/userstaff.cpp \
    ../userstaff/connectionjasser.cpp \
    ../gestionclients/gestionclients.cpp \
    ../gestionclients/gestionclientswidget.cpp \
    ../gestionclients/metierswidget.cpp \
    ../gestionclients/client.cpp \
    ../gestionclients/clientconnection.cpp \
    ../stock/IAf.cpp \
    ../stock/calendrier.cpp \
    ../stock/fournisseur.cpp \
    ../stock/historique.cpp \
    ../stock/stock.cpp \
    ../stock/connexion.cpp \
    ../machine/machine.cpp \
    ../machine/connexionmachine.cpp \
    ../machine/chatbot.cpp \
    ../finance/finance/finance.cpp \
    ../finance/finance/connexionfinance.cpp \
    ../finance/finance/advancedtab.cpp \
    ../finance/finance/historiqueaudit.cpp

qtHaveModule(httpserver) {
    SOURCES += \
        ../machine/MachineServer.cpp
}

HEADERS += \
    menu.h \
    ../Login/login.h \
    ../Login/connectionlogin.h \
    ../production/production.h \
    ../production/connection.h \
    ../production/productionmodel.h \
    ../production/productioneditdialog.h \
    ../production/qualityaiagent.h \
    ../userstaff/userstaff.h \
    ../userstaff/connectionjasser.h \
    ../gestionclients/gestionclients.h \
    ../gestionclients/gestionclientswidget.h \
    ../gestionclients/metierswidget.h \
    ../gestionclients/client.h \
    ../gestionclients/clientconnection.h \
    ../stock/IAf.h \
    ../stock/calendrier.h \
    ../stock/historique.h \
    ../stock/stock.h \
    ../stock/connexion.h \
    ../machine/machine.h \
    ../machine/connexionmachine.h \
    ../machine/chatbot.h \
    ../finance/finance/finance.h \
    ../finance/finance/connexionfinance.h \
    ../finance/finance/advancedtab.h \
    ../finance/finance/historiqueaudit.h

qtHaveModule(httpserver) {
    HEADERS += \
        ../machine/MachineServer.h
}

FORMS += \
    menu.ui \
    ../production/production.ui \
    ../userstaff/userstaff.ui \
    ../gestionclients/gestionclients.ui \
    ../gestionclients/gestionclientswidget.ui \
    ../gestionclients/metierswidget.ui \
    ../stock/stock.ui \
    ../machine/machine.ui \
    ../finance/finance/finance.ui

RESOURCES += \
    menu_resources.qrc \
    ../Login/login_resources.qrc \
    ../machine/resources.qrc \
    ../stock/stock_resources.qrc

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

