#include "keyboardwindow.h"
#include "ui_keyboardwindow.h"

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/keysym.h>
#include <X11/extensions/XTest.h>

KeyboardWindow::KeyboardWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::KeyboardWindow)
{
    ui->setupUi(this);

    this->setAttribute(Qt::WA_AcceptTouchEvents);
    //ui->modifierKeys->setAttribute(Qt::WA_AcceptTouchEvents);

    this->layout()->removeWidget(ui->otherKeyboardsFrame);
    this->setFixedHeight(this->sizeHint().height());

    ui->otherKeyboardsFrame->setParent(this);
    ui->otherKeyboardsFrame->setGeometry(0, this->height(), this->width(), this->height());

    ui->otherKeyboardsFrame->installEventFilter(this);
    ui->shift->installEventFilter(this);

    QFont fnt = this->font();
    ui->otherKeyboardsFrame->setFont(fnt);
    fnt.setPointSize(20);
    this->setFont(fnt);

    buttonIterate(this);
    buttonIterate(ui->alphaPage);
    buttonIterate(ui->symbolPage);
    buttonIterate(ui->bottomFrame);
    buttonIterate(ui->otherKeyboardsFrame);

    ui->shift->setAttribute(Qt::WA_AcceptTouchEvents, false);

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

    /*QPushButton* closeButton = new QPushButton;
    closeButton->setIcon(QIcon::fromTheme("window-close"));
    closeButton->setParent(this);
    closeButton->setIconSize(QSize(24, 24));
    closeButton->setGeometry(this->geometry().right() - 38, 6, 32, 32);
    closeButton->setFlat(true);
    connect(closeButton, SIGNAL(clicked(bool)), this, SLOT(close()));*/

    QSystemTrayIcon* trayIcon = new QSystemTrayIcon();
    trayIcon->setIcon(QIcon::fromTheme("preferences-desktop-keyboard"));
    connect(trayIcon, &QSystemTrayIcon::activated, [=](QSystemTrayIcon::ActivationReason reason) {
        if (this->isVisible()) {
            this->hide();
        } else {
            this->show();
        }
    });
    trayIcon->show();
}

KeyboardWindow::~KeyboardWindow()
{
    delete ui;
}

void KeyboardWindow::buttonIterate(QWidget* wid) {
    //wid->setAttribute(Qt::WA_AcceptTouchEvents);
    for (QObject* widget : wid->children()) {
        if ((QPushButton*) widget != NULL) {
            connect((QPushButton*) widget, SIGNAL(clicked(bool)), this, SLOT(pressKey()));
        } else if ((QWidget*) widget != NULL) {
            buttonIterate((QWidget*) widget);
        }
    }
}

void KeyboardWindow::pressKey() {
    QPushButton* button = (QPushButton*) sender();
    if (button == ui->shift || button == ui->ctrlKey || button == ui->altKey || button == ui->changeButton || button == ui->hideKeyboard) return; //Ignore SHIFT, change and hide key

    Window focused;
    int revert_to;

    XGetInputFocus(QX11Info::display(), &focused, &revert_to);

    KeySym pressedKey;
    QString text = button->text();
    bool isShift = false;


    if (text == "!" || text == "@" || text == "#" || text == "$" || text == "%" || text == "^" || text == "&"
            || text == "*" || text == "(" || text == ")" || text == "{" || text == "}" || text == "|"
            || text == ":" || text == "\"" || text == "<" || text == ">" || text == "?" || text == "~") {
        isShift = true;
    }

    if (button == ui->backspace || button == ui->backspace2Button) {
        pressedKey = XK_BackSpace;
    } else if (button == ui->returnButton) {
        pressedKey = XK_Return;
    } else if (button == ui->spaceButton) {
        pressedKey = XK_space;
    } else if (button == ui->ampButton) {
        pressedKey = XK_7;
    } else if (button == ui->tabButton) {
        pressedKey = XK_Tab;
    } else if (button == ui->upButton) {
        pressedKey = XK_Up;
    } else if (button == ui->downButton) {
        pressedKey = XK_Down;
    } else if (button == ui->leftButton) {
        pressedKey = XK_Left;
    } else if (button == ui->rightButton) {
        pressedKey = XK_Right;
    } else if (button == ui->insertKey) {
        pressedKey = XK_Insert;
    } else if (button == ui->homeKey) {
        pressedKey = XK_Home;
    } else if (button == ui->endKey) {
        pressedKey = XK_End;
    } else if (button == ui->deleteKey) {
        pressedKey = XK_Delete;
    } else if (button == ui->pgUpKey) {
        pressedKey = XK_Page_Up;
    } else if (button == ui->pgDownKey) {
        pressedKey = XK_Page_Down;
    } else if (button == ui->numpadEnter) {
        pressedKey = XK_KP_Enter;
    } else if (button == ui->escKey) {
        pressedKey = XK_Escape;
    } else if (button == ui->f1Key) {
        pressedKey = XK_F1;
    } else if (button == ui->f2Key) {
        pressedKey = XK_F2;
    } else if (button == ui->f3Key) {
        pressedKey = XK_F3;
    } else if (button == ui->f4Key) {
        pressedKey = XK_F4;
    } else if (button == ui->f5Key) {
        pressedKey = XK_F5;
    } else if (button == ui->f6Key) {
        pressedKey = XK_F6;
    } else if (button == ui->f7Key) {
        pressedKey = XK_F7;
    } else if (button == ui->f8Key) {
        pressedKey = XK_F8;
    } else if (button == ui->f9Key) {
        pressedKey = XK_F9;
    } else if (button == ui->f10Key) {
        pressedKey = XK_F10;
    } else if (button == ui->f11Key) {
        pressedKey = XK_F11;
    } else if (button == ui->f12Key) {
        pressedKey = XK_F12;
    } else {
        QString keycode = button->text().toUtf8().toHex();

        if (keycode.length() == 1) {
            keycode.prepend("000");
        } else if (keycode.length() == 2) {
            keycode.prepend("00");
        } else if (keycode.length() == 3) {
            keycode.prepend("0");
        }

        keycode.prepend("U");
        pressedKey = XStringToKeysym(keycode.toUtf8());

    }


    /*XKeyEvent event;

    event.display = QX11Info::display();
    event.window = focused;
    event.root = DefaultRootWindow(QX11Info::display());
    event.subwindow = None;
    event.time = CurrentTime;
    event.x = 1;
    event.y = 1;
    event.x_root = 1;
    event.y_root = 1;
    event.same_screen = True;
    event.keycode = XKeysymToKeycode(QX11Info::display(), pressedKey);

    if (ui->shift->isChecked() || isShift) {
        event.state = ShiftMask;
    } else {
        event.state = None;
    }

    event.type = KeyPress;
    XSendEvent(QX11Info::display(), focused, True, KeyPressMask, (XEvent *) &event);

    event.type = KeyRelease;
    XSendEvent(QX11Info::display(), focused, True, KeyReleaseMask, (XEvent *) &event);*/
    if (isShift || ui->shift->isChecked()) {
        XTestFakeKeyEvent(QX11Info::display(), XKeysymToKeycode(QX11Info::display(), XK_Shift_L), true, 0);
    }
    if (ui->ctrlKey->isChecked()) {
        XTestFakeKeyEvent(QX11Info::display(), XKeysymToKeycode(QX11Info::display(), XK_Control_L), true, 0);
    }
    if (ui->altKey->isChecked()) {
        XTestFakeKeyEvent(QX11Info::display(), XKeysymToKeycode(QX11Info::display(), XK_Alt_L), true, 0);
    }
    XTestFakeKeyEvent(QX11Info::display(), XKeysymToKeycode(QX11Info::display(), pressedKey), true, 0);
    XTestFakeKeyEvent(QX11Info::display(), XKeysymToKeycode(QX11Info::display(), pressedKey), false, 0);
    if (ui->altKey->isChecked()) {
        XTestFakeKeyEvent(QX11Info::display(), XKeysymToKeycode(QX11Info::display(), XK_Alt_L), false, 0);
    }
    if (ui->ctrlKey->isChecked()) {
        XTestFakeKeyEvent(QX11Info::display(), XKeysymToKeycode(QX11Info::display(), XK_Control_L), false, 0);
    }
    if (isShift || ui->shift->isChecked()) {
        XTestFakeKeyEvent(QX11Info::display(), XKeysymToKeycode(QX11Info::display(), XK_Shift_L), false, 0);
    }
    XFlush(QX11Info::display());

    ui->altKey->setChecked(false);
    ui->ctrlKey->setChecked(false);

    if (!capsLock) {
        ui->shift->setChecked(false);
    }

    QSoundEffect* keySound = new QSoundEffect();
    keySound->setSource(QUrl("qrc:/sounds/keyclick.wav"));
    keySound->play();
    connect(keySound, SIGNAL(playingChanged()), keySound, SLOT(deleteLater()));
}


void KeyboardWindow::show() {
    this->setWindowFlags(Qt::Dialog | Qt::WindowStaysOnTopHint);

    Atom DesktopWindowTypeAtom;
    DesktopWindowTypeAtom = XInternAtom(QX11Info::display(), "_NET_WM_WINDOW_TYPE_DOCK", False);
    int retval = XChangeProperty(QX11Info::display(), this->winId(), XInternAtom(QX11Info::display(), "_NET_WM_WINDOW_TYPE", False),
                     XA_ATOM, 32, PropModeReplace, (unsigned char*) &DesktopWindowTypeAtom, 1); //Change Window Type

    unsigned long desktop = 0xFFFFFFFF;
    retval = XChangeProperty(QX11Info::display(), this->winId(), XInternAtom(QX11Info::display(), "_NET_WM_DESKTOP", False),
                     XA_CARDINAL, 32, PropModeReplace, (unsigned char*) &desktop, 1); //Set visible on all desktops

    QDialog::show();

    XEvent event;

    event.xclient.type = ClientMessage;
    event.xclient.serial = 0;
    event.xclient.send_event = True;
    event.xclient.message_type = XInternAtom(QX11Info::display(), "_NET_WM_STATE", False);
    event.xclient.window = this->winId();
    event.xclient.format = 32;
    event.xclient.data.l[0] = 1;
    event.xclient.data.l[1] = XInternAtom(QX11Info::display(), "_NET_WM_STATE_SKIP_TASKBAR", False);
    event.xclient.data.l[2] = XInternAtom(QX11Info::display(), "_NET_WM_STATE_SKIP_PAGER", False);;
    event.xclient.data.l[3] = 1;
    event.xclient.data.l[4] = 0;

    XSendEvent(QX11Info::display(), DefaultRootWindow(QX11Info::display()), False, SubstructureRedirectMask | SubstructureNotifyMask, &event);


    QRect screenGeometry = QApplication::desktop()->screenGeometry();
    long* struts = (long*) malloc(sizeof(long) * 12);
    struts[0] = 0;
    struts[1] = 0;
    struts[2] = 0;
    struts[3] = this->height();
    struts[4] = 0;
    struts[5] = 0;
    struts[6] = 0;
    struts[7] = 0;
    struts[8] = 0;
    struts[9] = 0;
    struts[10] = screenGeometry.left();
    struts[11] = screenGeometry.right();
    retval = XChangeProperty(QX11Info::display(), this->winId(), XInternAtom(QX11Info::display(), "_NET_WM_STRUT_PARTIAL", False),
                     XA_CARDINAL, 32, PropModeReplace, (unsigned char*) struts, 12);

    free(struts);

    this->setGeometry(screenGeometry.left(), screenGeometry.bottom() - this->height() + 1, screenGeometry.width(), this->height());
}

void KeyboardWindow::on_changeButton_clicked() {
    if (ui->stackedWidget->currentIndex() == 0) {
        ui->stackedWidget->setCurrentIndex(1);
    } else {
        ui->stackedWidget->setCurrentIndex(0);
    }

    QSoundEffect* keySound = new QSoundEffect();
    keySound->setSource(QUrl("qrc:/sounds/keyclickErase.wav"));
    keySound->play();
    connect(keySound, SIGNAL(playingChanged()), keySound, SLOT(deleteLater()));
}

void KeyboardWindow::on_hideKeyboard_clicked()
{
    this->hide();

    QSoundEffect* keySound = new QSoundEffect();
    keySound->setSource(QUrl("qrc:/sounds/keyclickErase.wav"));
    keySound->play();
    connect(keySound, SIGNAL(playingChanged()), keySound, SLOT(deleteLater()));
}

bool KeyboardWindow::event(QEvent *event) {
    if (event->type() == QEvent::TouchBegin) {
        QTouchEvent* e = (QTouchEvent*) event;
        if (e->touchPoints().count() == 1) {
            if (e->touchPoints().first().startPos().y() > this->height() - 10) {
                ui->otherKeyboardsFrame->setFixedWidth(this->width());
                e->accept();
            } else {
                e->ignore();
            }
        } else {
            e->ignore();
        }
        return true;
    } else if (event->type() == QEvent::TouchUpdate) {
        QTouchEvent* e = (QTouchEvent*) event;
        if (e->touchPoints().count() == 1) {
            QTouchEvent::TouchPoint point = e->touchPoints().first();
            if (point.startPos().y() > this->height() - 10) {
                int top = point.pos().y();
                if (top < 0) {
                    top = 0;
                }
                ui->otherKeyboardsFrame->move(0, top);
            } else {
                e->ignore();
            }
        } else {
            e->ignore();
        }
        return true;
    } else if (event->type() == QEvent::TouchEnd) {
        QTouchEvent* e = (QTouchEvent*) event;
        if (e->touchPoints().count() == 1) {
            QTouchEvent::TouchPoint point = e->touchPoints().first();
            if (point.startPos().y() > this->height() - 10) {
                tPropertyAnimation* anim = new tPropertyAnimation(ui->otherKeyboardsFrame, "geometry");
                anim->setStartValue(ui->otherKeyboardsFrame->geometry());
                if (point.pos().y() > this->height() / 2) { //Cancel it
                    anim->setEndValue(QRect(0, this->height(), this->width(), this->height()));
                } else {
                    anim->setEndValue(QRect(0, 0, this->width(), this->height()));
                }
                anim->setDuration(250);
                anim->setEasingCurve(QEasingCurve::OutCubic);
                connect(anim, SIGNAL(finished()), anim, SLOT(deleteLater()));
                anim->start();
            } else {
                e->ignore();
            }
        } else {
            e->ignore();
        }
        return true;
    } else {
        return QDialog::event(event);
    }
}

bool KeyboardWindow::eventFilter(QObject *obj, QEvent *event) {
    if (obj == ui->otherKeyboardsFrame) {
        if (event->type() == QEvent::MouseButtonPress) {
            QMouseEvent* e = (QMouseEvent*) event;
            obj->setProperty("startDragPos", e->pos());
            return true;
        } else if (event->type() == QEvent::MouseMove) {
            QMouseEvent* e = (QMouseEvent*) event;
            int top = ui->otherKeyboardsFrame->mapTo(this, e->pos()).y() - obj->property("startDragPos").toPoint().y();
            if (top < 0) {
                top = 0;
            }
            ui->otherKeyboardsFrame->move(0, top);
            return true;
        } else if (event->type() == QEvent::MouseButtonRelease) {
            QMouseEvent* e = (QMouseEvent*) event;
            tPropertyAnimation* anim = new tPropertyAnimation(ui->otherKeyboardsFrame, "geometry");
            anim->setStartValue(ui->otherKeyboardsFrame->geometry());
            if (ui->otherKeyboardsFrame->y() + ui->otherKeyboardsFrame->height() > this->height() / 2) { //Cancel it
                anim->setEndValue(QRect(0, this->height(), this->width(), this->height()));
            } else {
                anim->setEndValue(QRect(0, 0, this->width(), this->height()));
            }
            anim->setDuration(250);
            anim->setEasingCurve(QEasingCurve::OutCubic);
            connect(anim, SIGNAL(finished()), anim, SLOT(deleteLater()));
            anim->start();
            return true;
        } else {
            return false;
        }
    } else if (obj == ui->shift) {
        if (event->type() == QEvent::MouseButtonDblClick) {
            capsLock = true;
            ui->shift->setChecked(true);
            return true;
        }
    }
    return false;
}

void KeyboardWindow::on_ctrlKey_clicked()
{
    tPropertyAnimation* anim = new tPropertyAnimation(ui->otherKeyboardsFrame, "geometry");
    anim->setStartValue(ui->otherKeyboardsFrame->geometry());
    anim->setEndValue(QRect(0, this->height(), this->width(), this->height()));
    anim->setDuration(250);
    anim->setEasingCurve(QEasingCurve::OutCubic);
    connect(anim, SIGNAL(finished()), anim, SLOT(deleteLater()));
    anim->start();
}

void KeyboardWindow::on_altKey_clicked()
{
    tPropertyAnimation* anim = new tPropertyAnimation(ui->otherKeyboardsFrame, "geometry");
    anim->setStartValue(ui->otherKeyboardsFrame->geometry());
    anim->setEndValue(QRect(0, this->height(), this->width(), this->height()));
    anim->setDuration(250);
    anim->setEasingCurve(QEasingCurve::OutCubic);
    connect(anim, SIGNAL(finished()), anim, SLOT(deleteLater()));
    anim->start();
}

void KeyboardWindow::on_tabButton_clicked()
{
    tPropertyAnimation* anim = new tPropertyAnimation(ui->otherKeyboardsFrame, "geometry");
    anim->setStartValue(ui->otherKeyboardsFrame->geometry());
    anim->setEndValue(QRect(0, this->height(), this->width(), this->height()));
    anim->setDuration(250);
    anim->setEasingCurve(QEasingCurve::OutCubic);
    connect(anim, SIGNAL(finished()), anim, SLOT(deleteLater()));
    anim->start();
}

void KeyboardWindow::on_shift_clicked(bool checked)
{
    capsLock = false;
}
