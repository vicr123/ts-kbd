#include "layoutnumeric.h"
#include "ui_layoutnumeric.h"

#include <QSoundEffect>
#include "keyboardstate.h"

#include <X11/Xlib.h>
#include <X11/keysym.h>

extern KeyboardState* state;

LayoutNumeric::LayoutNumeric(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LayoutNumeric)
{
    ui->setupUi(this);

    buttonIterate(this);
}

LayoutNumeric::~LayoutNumeric()
{
    delete ui;
}

void LayoutNumeric::buttonIterate(QWidget* wid) {
    for (QObject* widget : wid->children()) {
        if (qobject_cast<KeyButton*>(widget) != nullptr) {
            connect(qobject_cast<KeyButton*>(widget), SIGNAL(tapped()), this, SLOT(pressKey()));
        } else if (qobject_cast<QPushButton*>(widget) != nullptr) {
            connect(qobject_cast<QPushButton*>(widget), SIGNAL(clicked(bool)), this, SLOT(pressKey()));
        } else if (qobject_cast<QWidget*>(widget) != nullptr) {
            buttonIterate(qobject_cast<QWidget*>(widget));
        }
    }
}

void LayoutNumeric::pressKey() {
    QPushButton* button = (QPushButton*) sender();

    Window focused;
    int revert_to;

    //XGetInputFocus(QX11Info::display(), &focused, &revert_to);

    KeySym pressedKey = 0;
    KeyCode pressedKeyCode = 0;
    bool useKeySym = true;

    QString text = button->text();
    bool isShift = false;


    if (text == "!" || text == "@" || text == "#" || text == "$" || text == "%" || text == "^" || text == "&"
            || text == "*" || text == "(" || text == ")" || text == "{" || text == "}" || text == "|"
            || text == ":" || text == "\"" || text == "<" || text == ">" || text == "?" || text == "~") {
        isShift = true;
    }

    if (button == ui->backspace) {
        pressedKey = XK_BackSpace;
    } else if (button == ui->returnButton) {
        pressedKey = XK_Return;
    } else {
        //Register keycode with desired character
        //int kc = findEmptyKeycode();
        QString keycode = button->text().toUtf8().toHex();
        if (button->text() == "&&") keycode = QString("&").toUtf8().toHex();

        if (keycode.length() == 1) {
            keycode.prepend("000");
        } else if (keycode.length() == 2) {
            keycode.prepend("00");
        } else if (keycode.length() == 3) {
            keycode.prepend("0");
        }

        keycode.prepend("U");
        pressedKey = XStringToKeysym(keycode.toLocal8Bit().constData());

        emit pushLetter(button->text());

        /*KeySym ksList[] = {
            pressedKey
        };
        qDebug() << XChangeKeyboardMapping(QX11Info::display(), kc, 1, ksList, 1);
        pressedKeyCode = kc;
        useKeySym = true;*/
    }

    emit typeKey(pressedKey);
}
