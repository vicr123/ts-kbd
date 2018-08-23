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
