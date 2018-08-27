#-------------------------------------------------
#
# Project created by QtCreator 2017-03-15T20:48:29
#
#-------------------------------------------------

QT       += core gui x11extras multimedia thelib
CONFIG   += c++14

unix {
    CONFIG += link_pkgconfig
    PKGCONFIG += x11 xtst hunspell
}


DBUS_ADAPTORS = org.thesuite.tskbd.xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ts-kbd
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cpp\
    keyboardwindow.cpp \
    kbddbus.cpp \
    keybutton.cpp \
    layouts/layoutus.cpp \
    keyboardstate.cpp \
    layouts/layoutsym.cpp \
    settings.cpp \
    suggestionbar.cpp

HEADERS  += \
    keyboardwindow.h \
    kbddbus.h \
    keybutton.h \
    layouts/layoutus.h \
    keyboardstate.h \
    layouts/layoutsym.h \
    settings.h \
    suggestionbar.h

FORMS    += \
    keyboardwindow.ui \
    layouts/layoutus.ui \
    layouts/layoutsym.ui \
    settings.ui \
    suggestionbar.ui

RESOURCES += \
    resources.qrc

unix {
    target.path = /usr/bin
    INSTALLS += target
}
