#include "inputcontext.h"
#include <QProcess>
#include <QDebug>
#include <QCoreApplication>

InputContext::InputContext() : QPlatformInputContext()
{
    tsSettings = new QSettings("theSuite", "theShell");
    connect(tVirtualKeyboard::instance(), &tVirtualKeyboard::keyboardVisibleChanged, [=](bool visible) {
        emitInputPanelVisibleChanged();
    });

    //Install an event filter on the application to figure out if the keyboard was brought up with touch events or not
    QCoreApplication::instance()->installEventFilter(this);
}

bool InputContext::isValid() const {
    /*if (!tVirtualKeyboard::instance()->isKeyboardRunning()) {
        return false;
    }*/
    return true;
}

void InputContext::showInputPanel() {
    if (!tVirtualKeyboard::instance()->isKeyboardRunning()) return;
    if (tVirtualKeyboard::instance()->keyboardVisible()) return;
    if (!lastEventWasTouch) return;
    tVirtualKeyboard::instance()->showKeyboard();

    if (focusObject != nullptr) {
        QInputMethodQueryEvent event(Qt::ImHints | Qt::ImEnterKeyType);
        QCoreApplication::sendEvent(focusObject, &event);

        Qt::InputMethodHints hints = (Qt::InputMethodHints) event.value(Qt::ImHints).toInt();
        qDebug() << hints;
        if (hints.testFlag(Qt::ImhDigitsOnly) || hints.testFlag(Qt::ImhPreferNumbers) || hints.testFlag(Qt::ImhTime) || hints.testFlag(Qt::ImhDate)) {
            tVirtualKeyboard::instance()->setKeyboardType("numeric");
        }
        if (hints.testFlag(Qt::ImhUrlCharactersOnly)) {
            tVirtualKeyboard::instance()->setKeyboardType("url");
        }
        if (hints.testFlag(Qt::ImhNoPredictiveText)) {
            tVirtualKeyboard::instance()->setPredictive(false);
        }

        Qt::EnterKeyType enter = (Qt::EnterKeyType) event.value(Qt::ImEnterKeyType).toInt();
        switch (enter) {
            case Qt::EnterKeyDefault:
            case Qt::EnterKeyReturn:
                tVirtualKeyboard::instance()->setEnterKeyType("return");
                break;
            case Qt::EnterKeyDone:
                tVirtualKeyboard::instance()->setEnterKeyType("done");
                break;
            case Qt::EnterKeyGo:
                tVirtualKeyboard::instance()->setEnterKeyType("go");
                break;
            case Qt::EnterKeyNext:
                tVirtualKeyboard::instance()->setEnterKeyType("next");
                break;
            case Qt::EnterKeyPrevious:
                tVirtualKeyboard::instance()->setEnterKeyType("previous");
                break;
            case Qt::EnterKeySearch:
                tVirtualKeyboard::instance()->setEnterKeyType("search");
                break;
            case Qt::EnterKeySend:
                tVirtualKeyboard::instance()->setEnterKeyType("send");
                break;
        }
    }
}

void InputContext::hideInputPanel() {
    if (!tVirtualKeyboard::instance()->isKeyboardRunning()) return;
    if (!tVirtualKeyboard::instance()->keyboardVisible()) return;
    tVirtualKeyboard::instance()->hideKeyboard();
}

bool InputContext::isInputPanelVisible() const {
    return tVirtualKeyboard::instance()->keyboardVisible();
}

bool InputContext::eventFilter(QObject* watched, QEvent* event) {
    //if (watched == focusObject) {
        if (event->type() == QEvent::MouseButtonPress || event->type() == QEvent::MouseMove) {
            QMouseEvent* e = (QMouseEvent*) event;
            if (e->source() == Qt::MouseEventSynthesizedByQt) { //Touch event
                lastEventWasTouch = true;
                if (!tVirtualKeyboard::instance()->isKeyboardRunning()) {
                    //Attempt to start the virtual keyboard if required
                    QProcess::startDetached("ts-kbd");
                }
            } else {
                lastEventWasTouch = false;
            }
        } else if (event->type() == QEvent::TouchBegin) {
            lastEventWasTouch = true;
            if (!tVirtualKeyboard::instance()->isKeyboardRunning()) {
                //Attempt to start the virtual keyboard if required
                QProcess::startDetached("ts-kbd");
            }
        }
    //}
    return false;
}

void InputContext::setFocusObject(QObject* focusObject) {
    this->focusObject = focusObject;
}
