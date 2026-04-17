QT       += core gui sql printsupport charts network httpserver multimedia texttospeech

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17
QMAKE_CXXFLAGS += -finput-charset=UTF-8 -fexec-charset=UTF-8

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    MachineServer.cpp \
    assistant.cpp \
    connexionmachine.cpp \
    main.cpp \
    machine.cpp \
    chatbot.cpp

HEADERS += \
    MachineServer.h \
    assistant.h \
    connexionmachine.h \
    machine.h \
    chatbot.h

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

