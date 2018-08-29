#include "kbddbus.h"
#include "tskbd_adaptor.h"

#include "keyboardstate.h"
extern KeyboardWindow* mainWindow;
extern KeyboardState* state;

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

void KbdDbus::setPredictive(bool predictive) {
    mainWindow->setPredictive(predictive);
}

void KbdDbus::setHidden(bool hidden) {

}

void KbdDbus::setSensitive(bool sensitive) {

}

void KbdDbus::setAutoUppercase(bool autoUpperCase) {

}

void KbdDbus::setEnterKeyType(QString enterKeyType) {
    if (enterKeyType == "return") {
        state->setEnterKeyType(Qt::EnterKeyReturn);
    } else if (enterKeyType == "done") {
        state->setEnterKeyType(Qt::EnterKeyDone);
    } else if (enterKeyType == "go") {
        state->setEnterKeyType(Qt::EnterKeyGo);
    } else if (enterKeyType == "next") {
        state->setEnterKeyType(Qt::EnterKeyNext);
    } else if (enterKeyType == "previous") {
        state->setEnterKeyType(Qt::EnterKeyPrevious);
    } else if (enterKeyType == "search") {
        state->setEnterKeyType(Qt::EnterKeySearch);
    } else if (enterKeyType == "send") {
        state->setEnterKeyType(Qt::EnterKeySend);
    }
}
