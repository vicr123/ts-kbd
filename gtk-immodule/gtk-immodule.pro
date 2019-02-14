TEMPLATE = lib
CONFIG += plugin
CONFIG -= app_bundle
CONFIG += qt

QT += core dbus

TARGET = ts-kbd

CONFIG += link_pkgconfig no_keywords
PKGCONFIG = gtk+-3.0

LIBS += -lthe-libs
INCLUDEPATH += /usr/include/the-libs/

SOURCES += \
    main.c \
    tskbdinterface.cpp

target.path = /usr/lib/gtk-3.0/3.0.0/immodules/
INSTALLS += target

