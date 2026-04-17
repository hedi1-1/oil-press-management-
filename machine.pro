QT       += core gui sql printsupport charts network httpserver multimedia texttospeech serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17
QMAKE_CXXFLAGS += -finput-charset=UTF-8 -fexec-charset=UTF-8

# Ensure qmake uses the full MinGW toolchain (gcc/g++) from Qt Tools.
# Some Qt installs under C:/Qt/<version>/mingw_64 ship Qt binaries but not cc1plus.
win32-g++ {
    QMAKE_CC = C:/Qt/Tools/mingw1120_64/bin/gcc.exe
    QMAKE_CXX = C:/Qt/Tools/mingw1120_64/bin/g++.exe
    QMAKE_LINK = C:/Qt/Tools/mingw1120_64/bin/g++.exe
    QMAKE_LINK_SHLIB = C:/Qt/Tools/mingw1120_64/bin/g++.exe
}

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    MachineServer.cpp \
    assistant.cpp \
    connexionmachine.cpp \
    main.cpp \
    machine.cpp

HEADERS += \
    MachineServer.h \
    assistant.h \
    connexionmachine.h \
    machine.h

FORMS += \
    machine.ui

RESOURCES += \
    resources.qrc

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
QT+= sql
# Optionnel: activez ces lignes si libqrencode est installee sur votre poste.
#DEFINES += MACHINE_USE_QRENCODE
#LIBS += -lqrencode

