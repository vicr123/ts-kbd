#include "keyboardwindow.h"
#include "kbddbus.h"
#include "keyboardstate.h"
#include <QApplication>
#include <QDBusConnection>
#include <QDBusConnectionInterface>

KeyboardWindow* mainWindow;
KeyboardState* state;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setAttribute(Qt::AA_SynthesizeTouchForUnhandledMouseEvents);

    a.setOrganizationName("theSuite");
    a.setOrganizationDomain("");
    a.setApplicationName("ts-kbd");

    state = new KeyboardState;

    mainWindow = new KeyboardWindow;

    KbdDbus* dbus = new KbdDbus();
    if (!dbus->reg()) return 0;

    mainWindow->initTrayIcon();

    return a.exec();
}

float getDPIScaling() {
    float currentDPI = QApplication::desktop()->logicalDpiX();
    return currentDPI / (float) 96;
}
