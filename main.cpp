#include "keyboardwindow.h"
#include "kbddbus.h"
#include <QApplication>

KeyboardWindow* mainWindow;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setAttribute(Qt::AA_SynthesizeTouchForUnhandledMouseEvents);

    a.setOrganizationName("theSuite");
    a.setOrganizationDomain("");
    a.setApplicationName("ts-kbd");

    mainWindow = new KeyboardWindow;
    KbdDbus* dbus = new KbdDbus();
    //w.show();

    return a.exec();
}
