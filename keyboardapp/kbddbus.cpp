#include "kbddbus.h"
#include "tskbd_adaptor.h"

#include "keyboardstate.h"
extern KeyboardWindow* mainWindow;
extern KeyboardState* state;

struct KbdDbusPrivate {
    QTimer* kbdBuffer;
    bool show = false;
};

KbdDbus::KbdDbus(QObject *parent) : QObject(parent)
{
    d = new KbdDbusPrivate();

    new TskbdAdaptor(this);
    QDBusConnection dbus = QDBusConnection::sessionBus();
    dbus.registerObject("/org/thesuite/tskbd", this);

    connect(mainWindow, SIGNAL(keyboardVisibleChanged(bool)), this, SIGNAL(keyboardVisibleChanged(bool)));

    d->kbdBuffer = new QTimer();
    d->kbdBuffer->setInterval(100);
    d->kbdBuffer->setSingleShot(true);
    connect(d->kbdBuffer, &QTimer::timeout, [=] {
        if (d->show) {
            if (!mainWindow->isVisible()) {
                mainWindow->show();
                mainWindow->raise();
            }
        } else {
            if (mainWindow->isVisible()) {
                mainWindow->hide();
            }
        }
    });
}

KbdDbus::~KbdDbus() {
    delete d->kbdBuffer;
    delete d;
}

bool KbdDbus::reg() {
    return QDBusConnection::sessionBus().registerService("org.thesuite.tskbd");
}

void KbdDbus::showKeyboard() {
    d->show = true;
    if (d->kbdBuffer->isActive()) d->kbdBuffer->stop();
    d->kbdBuffer->start();
}

void KbdDbus::hideKeyboard() {
    d->show = false;
    if (d->kbdBuffer->isActive()) d->kbdBuffer->stop();
    d->kbdBuffer->start();
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
