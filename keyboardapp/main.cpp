#include "keyboardwindow.h"
#include "kbddbus.h"
#include "keyboardstate.h"
#include <QApplication>
#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDir>

KeyboardWindow* mainWindow;
KeyboardState* state;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setAttribute(Qt::AA_SynthesizeTouchForUnhandledMouseEvents);
    a.setQuitOnLastWindowClosed(false);

    a.setOrganizationName("theSuite");
    a.setOrganizationDomain("");
    a.setApplicationName("ts-kbd");

    QSettings settings;
    int count = settings.beginReadArray("layouts/languages");
    settings.endArray();
    if (count < 1) {
        settings.beginWriteArray("layouts/languages");
        settings.setArrayIndex(0);

        QStringList dictionaries;
        QDir dir("/usr/share/hunspell");
        for (QFileInfo file : dir.entryInfoList(QDir::Files)) {
            if (file.suffix() == "dic") {
                dictionaries.append(file.baseName());
            }
        }

        if (dictionaries.contains("en")) {
            settings.setValue("name", "English");
            settings.setValue("dictionary", "en");
        } else if (dictionaries.contains("en_US")) {
            settings.setValue("dictionary", "en_US");
            settings.setValue("name", "English (US)");
        } else if (dictionaries.contains("en_GB")) {
            settings.setValue("dictionary", "en_GB");
            settings.setValue("name", "English (GB)");
        } else if (dictionaries.contains("en_AU")) {
            settings.setValue("dictionary", "en_AU");
            settings.setValue("name", "English (AU)");
        } else if (dictionaries.contains("en_NZ")) {
            settings.setValue("dictionary", "en_NZ");
            settings.setValue("name", "English (NZ)");
        } else if (dictionaries.contains("en_AU")) {
            settings.setValue("dictionary", "en_CA");
            settings.setValue("name", "English (CA)");
        } else if (dictionaries.count() > 0) {
            settings.setValue("dictionary", dictionaries.first());
            settings.setValue("name", "Keyboard");
        }
        settings.setValue("layout", "enUS");

        settings.endArray();
    }

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
