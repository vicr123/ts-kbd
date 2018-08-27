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
    if (settings.contains("autocorrect/dictionary")) {
        QStringList dictionaries;
        QDir dir("/usr/share/hunspell");
        for (QFileInfo file : dir.entryInfoList(QDir::Files)) {
            if (file.suffix() == "dic") {
                dictionaries.append(file.baseName());
            }
        }

        if (dictionaries.contains("en")) {
            settings.setValue("autocorrect/dictionary", "en");
        } else if (dictionaries.contains("en_US")) {
            settings.setValue("autocorrect/dictionary", "en_US");
        } else if (dictionaries.contains("en_GB")) {
            settings.setValue("autocorrect/dictionary", "en_GB");
        } else if (dictionaries.contains("en_AU")) {
            settings.setValue("autocorrect/dictionary", "en_AU");
        } else if (dictionaries.contains("en_NZ")) {
            settings.setValue("autocorrect/dictionary", "en_NZ");
        } else if (dictionaries.contains("en_AU")) {
            settings.setValue("autocorrect/dictionary", "en_CA");
        } else if (dictionaries.count() > 0) {
            settings.setValue("autocorrect/dictionary", dictionaries.first());
        }
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
