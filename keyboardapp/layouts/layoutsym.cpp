#include "layoutsym.h"
#include "ui_layoutsym.h"

#include <QSoundEffect>
#include <QX11Info>
#include "keyboardstate.h"

#include <X11/Xlib.h>
#include <X11/keysym.h>

extern KeyboardState* state;

LayoutSym::LayoutSym(QWidget *parent) :
    Layout(parent),
    ui(new Ui::LayoutSym)
{
    ui->setupUi(this);

    buttonIterate(this);
}

LayoutSym::~LayoutSym()
{
    delete ui;
}

Layout::Layouts LayoutSym::layoutType() {
    return Symbol;
}

void LayoutSym::buttonIterate(QWidget* wid) {
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

void LayoutSym::pressKey() {
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
    } else {
        typeString(button->text());
        return;
    }

    emit typeKey(pressedKey);
}
