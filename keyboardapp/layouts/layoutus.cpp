#include "layoutus.h"
#include "ui_layoutus.h"

#include <QSoundEffect>
#include "keyboardstate.h"

#include <X11/Xlib.h>
#include <X11/keysym.h>

extern KeyboardState* state;

LayoutUS::LayoutUS(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LayoutUS)
{
    ui->setupUi(this);

    connect(ui->shift, &QPushButton::toggled, [=](bool checked) {
        if (checked) {
            ui->qButton->setText("Q");
            ui->wButton->setText("W");
            ui->eButton->setText("E");
            ui->rButton->setText("R");
            ui->tButton->setText("T");
            ui->yButton->setText("Y");
            ui->uButton->setText("U");
            ui->iButton->setText("I");
            ui->oButton->setText("O");
            ui->pButton->setText("P");

            ui->aButton->setText("A");
            ui->sButton->setText("S");
            ui->dButton->setText("D");
            ui->fButton->setText("F");
            ui->gButton->setText("G");
            ui->hButton->setText("H");
            ui->jButton->setText("J");
            ui->kButton->setText("K");
            ui->lButton->setText("L");

            ui->zButton->setText("Z");
            ui->xButton->setText("X");
            ui->cButton->setText("C");
            ui->vButton->setText("V");
            ui->bButton->setText("B");
            ui->nButton->setText("N");
            ui->mButton->setText("M");
        } else {
            ui->qButton->setText("q");
            ui->wButton->setText("w");
            ui->eButton->setText("e");
            ui->rButton->setText("r");
            ui->tButton->setText("t");
            ui->yButton->setText("y");
            ui->uButton->setText("u");
            ui->iButton->setText("i");
            ui->oButton->setText("o");
            ui->pButton->setText("p");

            ui->aButton->setText("a");
            ui->sButton->setText("s");
            ui->dButton->setText("d");
            ui->fButton->setText("f");
            ui->gButton->setText("g");
            ui->hButton->setText("h");
            ui->jButton->setText("j");
            ui->kButton->setText("k");
            ui->lButton->setText("l");

            ui->zButton->setText("z");
            ui->xButton->setText("x");
            ui->cButton->setText("c");
            ui->vButton->setText("v");
            ui->bButton->setText("b");
            ui->nButton->setText("n");
            ui->mButton->setText("m");
        }

        QSoundEffect* keySound = new QSoundEffect();
        keySound->setSource(QUrl("qrc:/sounds/keyclickErase.wav"));
        keySound->play();
        connect(keySound, SIGNAL(playingChanged()), keySound, SLOT(deleteLater()));
    });

    if (state->split()) {
        ui->splitWidget1->setVisible(false);
        ui->splitWidget2->setVisible(false);
        ui->splitWidget3->setVisible(false);
    }

    connect(state, &KeyboardState::shiftChanged, [=] {
        ui->shift->setChecked(state->capsLock() || state->shift());
    });
    connect(state, &KeyboardState::capsLockChanged, [=] {
        ui->shift->setChecked(state->capsLock() || state->shift());
    });
    connect(state, &KeyboardState::splitChanged, [=](bool split) {
        if (split) {
            ui->splitWidget1->setVisible(true);
            ui->splitWidget2->setVisible(true);
            ui->splitWidget3->setVisible(true);
        } else {
            ui->splitWidget1->setVisible(false);
            ui->splitWidget2->setVisible(false);
            ui->splitWidget3->setVisible(false);
        }
    });

    buttonIterate(this);
}

LayoutUS::~LayoutUS()
{
    delete ui;
}

void LayoutUS::buttonIterate(QWidget* wid) {
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

void LayoutUS::pressKey() {
    QPushButton* button = (QPushButton*) sender();
    if (button == ui->shift) {
        if (state->capsLock()) {
            state->setCapsLock(false);
        } else if (capsLockPressedTime.msecsTo(QDateTime::currentDateTime()) < QApplication::doubleClickInterval()) {
            state->setCapsLock(true);
        } else if (state->shift()) {
            state->setShift(false);
        } else {
            capsLockPressedTime = QDateTime::currentDateTime();
            state->setShift(true);
        }
        return;
    }

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
