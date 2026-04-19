QT       += core gui sql printsupport charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# Fix IntelliSense et compilateur
INCLUDEPATH += $$QT_INSTALL_HEADERS
INCLUDEPATH += $$QT_INSTALL_HEADERS/QtCore
INCLUDEPATH += $$QT_INSTALL_HEADERS/QtGui
INCLUDEPATH += $$QT_INSTALL_HEADERS/QtWidgets
INCLUDEPATH += $$QT_INSTALL_HEADERS/QtSql
INCLUDEPATH += $$QT_INSTALL_HEADERS/QtPrintSupport
INCLUDEPATH += $$QT_INSTALL_HEADERS/QtCharts

SOURCES += \
    main.cpp \
    finance.cpp \
    connexionfinance.cpp \
    historiqueaudit.cpp \
    advancedtab.cpp

HEADERS += \
    finance.h \
    connexionfinance.h \
    historiqueaudit.h \
    advancedtab.h

FORMS += \
    finance.ui

RESOURCES += \
    ../production/resources.qrc

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target


# Configuration de la base de données terminée.
