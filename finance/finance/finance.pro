QT       += core gui sql printsupport charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

SOURCES += \
    main.cpp \
    finance.cpp \
    connexionfinance.cpp

HEADERS += \
    finance.h \
    connexionfinance.h

FORMS += \
    finance.ui

RESOURCES += \
    ../production/resources.qrc

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target


# Configuration de la base de données terminée.
