#include "keyboardstate.h"

KeyboardState::KeyboardState(QObject *parent) : QObject(parent)
{
    sp = settings.value("keyboard/split", false).toBool();
}

bool KeyboardState::capsLock() {
    return caps;
}

void KeyboardState::setCapsLock(bool isOn) {
    caps = isOn;
    emit capsLockChanged(isOn);

    if (isOn && sh) sh = false;
}

bool KeyboardState::shift() {
    return sh;
}

void KeyboardState::setShift(bool isOn) {
    sh = isOn;
    emit shiftChanged(isOn);

    if (isOn && caps) caps = false;
}

bool KeyboardState::split() {
    return sp;
}

void KeyboardState::setSplit(bool split) {
    sp = split;
    settings.setValue("keyboard/split", split);
    emit splitChanged(split);
}

void KeyboardState::setEnterKeyType(Qt::EnterKeyType type) {
    ret = type;
    emit enterKeyTypeChanged(type);
}

Qt::EnterKeyType KeyboardState::enterKeyType() {
    return ret;
}
