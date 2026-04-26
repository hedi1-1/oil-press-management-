QT       += core gui sql serialport charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    IAf.cpp \
    calendrier.cpp \
    connexion.cpp \
    fournisseur.cpp \
    historique.cpp \
    main.cpp \
    stock.cpp

HEADERS += \
    IAf.h \
    calendrier.h \
    connexion.h \
    fournisseur.h \
    historique.h \
    stock.h

FORMS += \
    stock.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    stock_resources.qrc
