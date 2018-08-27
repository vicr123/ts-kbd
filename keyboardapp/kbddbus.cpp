#include "kbddbus.h"
#include "tskbd_adaptor.h"

extern KeyboardWindow* mainWindow;

KbdDbus::KbdDbus(QObject *parent) : QObject(parent)
{
    new TskbdAdaptor(this);
    QDBusConnection dbus = QDBusConnection::sessionBus();
    dbus.registerObject("/org/thesuite/tskbd", this);

    connect(mainWindow, SIGNAL(keyboardVisibleChanged(bool)), this, SIGNAL(keyboardVisibleChanged(bool)));
}

bool KbdDbus::reg() {
    return QDBusConnection::sessionBus().registerService("org.thesuite.tskbd");
}

void KbdDbus::showKeyboard() {
    mainWindow->show();
    mainWindow->raise();
}

void KbdDbus::hideKeyboard() {
    mainWindow->hide();
}

int KbdDbus::height() {
    return mainWindow->height();
}

bool KbdDbus::keyboardVisible() {
    return mainWindow->isVisible();
}

void KbdDbus::setKeyboardType(QString type) {
    mainWindow->setKeyboardType(type);
}
