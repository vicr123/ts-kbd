#include "layout.h"

#include <QX11Info>
#include <X11/Xlib.h>

Layout::Layout(QWidget *parent) : QWidget(parent)
{

}

void Layout::typeString(QString str) {
    KeySym pressedKey = 0;

    //Check if keycode works
    QString keycode = str.toUtf8().toHex();
    if (str == "&&") keycode = QString("&").toUtf8().toHex();

    if (keycode.length() == 1) {
        keycode.prepend("000");
    } else if (keycode.length() == 2) {
        keycode.prepend("00");
    } else if (keycode.length() == 3) {
        keycode.prepend("0");
    }

    keycode.prepend("U");
    pressedKey = XStringToKeysym(keycode.toLocal8Bit().constData());

    emit pushLetter(str);
    if (XKeysymToKeycode(QX11Info::display(), pressedKey) == 0) {
        //No keycode exists on this keyboard for this character
        //Use copy+paste hack
        emit typeLetter(str);
    } else {
        emit typeKey(pressedKey);
    }
}
