QT += core gui widgets sql charts printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17
CONFIG -= windows_rc

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    dbmanager.cpp

HEADERS += \
    mainwindow.h \
    dbmanager.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

# Application metadata
TARGET = PressIQ_Finance
VERSION = 1.0.0
DEFINES += APP_VERSION=\\\"$$VERSION\\\"

# Output directories
DESTDIR = build
OBJECTS_DIR = build/obj
MOC_DIR = build/moc
UI_DIR = build/ui

# Platform-specific configuration
#win32 {
#    #RC_ICONS = icon.ico
#    QMAKE_TARGET_PRODUCT = "PressIQ Finance"
#    QMAKE_TARGET_DESCRIPTION = "Module de Gestion Financière"
#    QMAKE_TARGET_COMPANY = "PressIQ"
#    QMAKE_TARGET_COPYRIGHT = "Copyright 2026"
#}

unix:!macx {
    QMAKE_LFLAGS += -no-pie
}

macx {
    #ICON = icon.icns
    #QMAKE_INFO_PLIST = Info.plist
}

# Additional defines
DEFINES += QT_DEPRECATED_WARNINGS

# Compiler warnings
QMAKE_CXXFLAGS += -Wall -Wextra

message("==================================================")
message("PressIQ Finance - Configuration qmake")
message("Qt version: $$[QT_VERSION]")
message("Architecture: $$QMAKE_HOST.arch")
message("Compiler: $$QMAKE_CXX")
message("==================================================")
