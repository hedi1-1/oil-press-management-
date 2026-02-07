QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    menu.cpp \
    ../production/production.cpp \
    ../machine/machine.cpp \
    ../userstaff/userstaff.cpp \
    ../gestionclients/gestionclients.cpp \
    ../stock/stock.cpp

HEADERS += \
    menu.h \
    ../production/production.h \
    ../machine/machine.h \
    ../userstaff/userstaff.h \
    ../gestionclients/gestionclients.h \
    ../stock/stock.h

FORMS += \
    menu.ui \
    ../production/production.ui \
    ../machine/machine.ui \
    ../userstaff/userstaff.ui \
    ../gestionclients/gestionclients.ui \
    ../stock/stock.ui

RESOURCES += \
    ../production/resources.qrc

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
