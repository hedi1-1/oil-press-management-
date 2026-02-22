QT       += core gui sql texttospeech

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    menu.cpp \
    ../Login/login.cpp \
    ../production/production.cpp \
    ../production/connection.cpp \
    ../production/productionmodel.cpp \
    ../production/productionservice.cpp \
    ../production/smartproductionassistant.cpp \
    ../production/productionbot.cpp \
    ../production/speechtotextengine.cpp \
    ../production/intentanalyzer.cpp \
    ../production/voiceassistant.cpp \
    ../userstaff/userstaff.cpp \
    ../gestionclients/gestionclients.cpp \
    ../stock/stock.cpp \
    ../stock/connexion.cpp \
    ../machine/machine.cpp \
    ../finance/finance.cpp

HEADERS += \
    menu.h \
    ../Login/login.h \
    ../production/production.h \
    ../production/connection.h \
    ../production/productionmodel.h \
    ../production/productionservice.h \
    ../production/smartproductionassistant.h \
    ../production/productionbot.h \
    ../production/speechtotextengine.h \
    ../production/intentanalyzer.h \
    ../production/voiceassistant.h \
    ../userstaff/userstaff.h \
    ../gestionclients/gestionclients.h \
    ../stock/stock.h \
    ../stock/connexion.h \
    ../machine/machine.h \
    ../finance/finance.h

FORMS += \
    menu.ui \
    ../production/production.ui \
    ../userstaff/userstaff.ui \
    ../gestionclients/gestionclients.ui \
    ../stock/stock.ui \
    ../machine/machine.ui \
    ../finance/finance.ui

RESOURCES += \
    ../production/resources.qrc \
    ../Login/login_resources.qrc

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
