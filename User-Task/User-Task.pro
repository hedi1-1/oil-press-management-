QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# Add your resource file
RESOURCES += resources.qrc

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

# Add your source files here
SOURCES += \
    main.cpp \
    mainwindow.cpp \
    login.cpp

HEADERS += \
    mainwindow.h \
    login.h

FORMS += \
    mainwindow.ui \
    login.ui
