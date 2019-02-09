#include "keyboardwindow.h"
#include "ui_keyboardwindow.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QPainter>
#include "keyboardstate.h"

#include "layouts/layoutus.h"
#include "layouts/layoutsym.h"
#include "layouts/layoutnumeric.h"

#include "settings.h"

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/keysym.h>
#include <X11/extensions/XTest.h>
#include <X11/XKBlib.h>
#include <unistd.h>

#undef Bool
#undef CursorShape
#undef Unsorted

extern float getDPIScaling();
extern KeyboardState* state;

KeyboardWindow::KeyboardWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::KeyboardWindow)
{
    ui->setupUi(this);

    this->setAttribute(Qt::WA_AcceptTouchEvents);

    this->layout()->removeWidget(ui->otherKeyboardsFrame);
    this->setFixedHeight(this->sizeHint().height());
    ui->keyboardsStack->setCurrentAnimation(tStackedWidget::Fade);

    ui->otherKeyboardsFrame->setParent(this);
    ui->otherKeyboardsFrame->setGeometry(0, this->height(), this->width(), this->height());

    this->layout()->removeWidget(ui->enterOptionsWidget);
    ui->enterOptionsWidget->setVisible(false);

    this->layout()->removeWidget(ui->splash);
    ui->splash->setFixedSize(this->size());
    ui->splash->move(0, 0);
    ui->splash->installEventFilter(this);
    ui->splash->setVisible(false);

    ui->otherKeyboardsFrame->installEventFilter(this);

    QFont fnt = this->font();
    ui->otherKeyboardsFrame->setFont(fnt);
    fnt.setPointSize(20);
    this->setFont(fnt);

    buttonIterate(this);

    LayoutUS* usLayout = new LayoutUS();
    usLayout->setFont(fnt);
    ui->keyboardsStack->addWidget(usLayout);
    connect(usLayout, SIGNAL(typeKey(unsigned long)), this, SLOT(pressKeySym(unsigned long)));
    connect(usLayout, SIGNAL(pushLetter(QString)), ui->suggestionBar, SLOT(pushLetter(QString)));
    layouts.insert(enUS, usLayout);

    LayoutSym* symLayout = new LayoutSym();
    symLayout->setFont(fnt);
    ui->keyboardsStack->addWidget(symLayout);
    connect(symLayout, SIGNAL(typeKey(unsigned long)), this, SLOT(pressKeySym(unsigned long)));
    connect(symLayout, SIGNAL(pushLetter(QString)), ui->suggestionBar, SLOT(pushLetter(QString)));
    layouts.insert(Symbol, symLayout);

    LayoutNumeric* numLayout = new LayoutNumeric();
    numLayout->setFont(fnt);
    ui->keyboardsStack->addWidget(numLayout);
    connect(numLayout, SIGNAL(typeKey(unsigned long)), this, SLOT(pressKeySym(unsigned long)));
    connect(numLayout, SIGNAL(pushLetter(QString)), ui->suggestionBar, SLOT(pushLetter(QString)));
    layouts.insert(Numeric, numLayout);

    if (state->split()) {
        ui->spaceButton->sizePolicy().setHorizontalStretch(10);
    }
    connect(state, &KeyboardState::splitChanged, [=](bool split) {
        if (split) {
            ui->spaceButton->sizePolicy().setHorizontalStretch(15);
        } else {
            ui->spaceButton->sizePolicy().setHorizontalStretch(10);
        }
    });

    connect(QApplication::desktop(), SIGNAL(resized(int)), this, SLOT(screenResolutionResized()));

    if (geteuid() == 0) {
        //Don't want the user to open settings in theDM
        ui->settingsButton->setEnabled(false);
    }
}

KeyboardWindow::~KeyboardWindow()
{
    delete ui;
}

void KeyboardWindow::buttonIterate(QWidget* wid) {
    for (QObject* widget : wid->children()) {
        if (qobject_cast<KeyButton*>(widget) != nullptr) {
            qDebug() << "Connecting " << qobject_cast<KeyButton*>(widget)->text();
            connect(qobject_cast<KeyButton*>(widget), SIGNAL(tapped()), this, SLOT(pressKey()));
        } else if (qobject_cast<QPushButton*>(widget) != nullptr) {
            connect(qobject_cast<QPushButton*>(widget), SIGNAL(clicked(bool)), this, SLOT(pressKey()));
        } else if (qobject_cast<QWidget*>(widget) != nullptr) {
            buttonIterate(qobject_cast<QWidget*>(widget));
        }
    }
}

void KeyboardWindow::pressKey() {
    QPushButton* button = (QPushButton*) sender();

    if (button == ui->ctrlKey || button == ui->altKey || button == ui->superKey || button == ui->changeButton || button == ui->hideKeyboard || button == ui->suggestionBar->clearButton()) return;

    Window focused;
    int revert_to;

    XGetInputFocus(QX11Info::display(), &focused, &revert_to);

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

    if (button == ui->spaceButton) {
        if (ignoreSpaceBar) {
            ignoreSpaceBar = false;
            return;
        }

        if (spacePressed.msecsTo(QDateTime::currentDateTime()) < 1000 && !spaceReplacementDone) {
            pressKeySym(XK_BackSpace);
            pressKeySym(XK_period);
            spaceReplacementDone = true;
        }

        spacePressed = QDateTime::currentDateTime();
        pressedKey = XK_space;
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
        //Register keycode with desired character
        int kc = findEmptyKeycode();
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

        KeySym ksList[] = {
            pressedKey
        };
        qDebug() << XChangeKeyboardMapping(QX11Info::display(), kc, 1, ksList, 1);
        pressedKeyCode = kc;
        useKeySym = true;
    }

    if (pressedKey != XK_space && pressedKey != XK_BackSpace) {
        spaceReplacementDone = false;
    }

    pressKeySym(pressedKey);
}

void KeyboardWindow::pressKeySym(unsigned long ks) {
    if (ks != XK_space && ks != XK_BackSpace) {
        spaceReplacementDone = false;
        spacePressed = QDateTime::fromMSecsSinceEpoch(0);
    }

    if (ks == XK_BackSpace) {
        ui->suggestionBar->backspace();
    } else if (ks == XK_space || ks == XK_Return) {
        //Complete word
        ui->suggestionBar->acceptAutocorrection();
    }

    int keysymNumber = -1;
    int keysymsPerKeycode;
    KeySym* keysyms = XGetKeyboardMapping(QX11Info::display(), XKeysymToKeycode(QX11Info::display(), ks), 1, &keysymsPerKeycode);

    if (keysyms == nullptr) return; //Unknown key
    for (int i = 0; i < keysymsPerKeycode; i++) {
        if (keysyms[i] == ks) {
            keysymNumber = i;
            break;
        }
    }


    if (state->shift() || state->capsLock() || keysymNumber == 1) {
        XTestFakeKeyEvent(QX11Info::display(), XKeysymToKeycode(QX11Info::display(), XK_Shift_L), true, 0);
    }
    if (ui->ctrlKey->isChecked()) {
        XTestFakeKeyEvent(QX11Info::display(), XKeysymToKeycode(QX11Info::display(), XK_Control_L), true, 0);
    }
    if (ui->altKey->isChecked()) {
        XTestFakeKeyEvent(QX11Info::display(), XKeysymToKeycode(QX11Info::display(), XK_Alt_L), true, 0);
    }
    if (ui->superKey->isChecked()) {
        XTestFakeKeyEvent(QX11Info::display(), XKeysymToKeycode(QX11Info::display(), XK_Super_L), true, 0);
    }

    XTestFakeKeyEvent(QX11Info::display(), XKeysymToKeycode(QX11Info::display(), ks), true, 0);
    XTestFakeKeyEvent(QX11Info::display(), XKeysymToKeycode(QX11Info::display(), ks), false, 0);

    if (ui->superKey->isChecked()) {
        XTestFakeKeyEvent(QX11Info::display(), XKeysymToKeycode(QX11Info::display(), XK_Super_L), false, 0);
    }
    if (ui->altKey->isChecked()) {
        XTestFakeKeyEvent(QX11Info::display(), XKeysymToKeycode(QX11Info::display(), XK_Alt_L), false, 0);
    }
    if (ui->ctrlKey->isChecked()) {
        XTestFakeKeyEvent(QX11Info::display(), XKeysymToKeycode(QX11Info::display(), XK_Control_L), false, 0);
    }
    if (state->shift() || state->capsLock() || keysymNumber == 1) {
        XTestFakeKeyEvent(QX11Info::display(), XKeysymToKeycode(QX11Info::display(), XK_Shift_L), false, 0);
    }
    XFlush(QX11Info::display());

    ui->superKey->setChecked(false);
    ui->altKey->setChecked(false);
    ui->ctrlKey->setChecked(false);

    if (state->shift()) state->setShift(false);

    QSoundEffect* keySound = new QSoundEffect();
    keySound->setSource(QUrl("qrc:/sounds/keyclick.wav"));
    keySound->play();
    connect(keySound, SIGNAL(playingChanged()), keySound, SLOT(deleteLater()));
}


void KeyboardWindow::show() {
    this->setWindowFlags(Qt::Dialog | Qt::WindowStaysOnTopHint | Qt::SubWindow);
    //this->setFixedHeight(this->sizeHint().height());
    this->setFixedHeight(300 * getDPIScaling());
    setKeyboardType("normal");
    setPredictive(true);

    Atom DesktopWindowTypeAtom;
    DesktopWindowTypeAtom = XInternAtom(QX11Info::display(), "_NET_WM_WINDOW_TYPE_DOCK", False);
    int retval = XChangeProperty(QX11Info::display(), this->winId(), XInternAtom(QX11Info::display(), "_NET_WM_WINDOW_TYPE", False),
                     XA_ATOM, 32, PropModeReplace, (unsigned char*) &DesktopWindowTypeAtom, 1); //Change Window Type

    unsigned long desktop = 0xFFFFFFFF;
    retval = XChangeProperty(QX11Info::display(), this->winId(), XInternAtom(QX11Info::display(), "_NET_WM_DESKTOP", False),
                     XA_CARDINAL, 32, PropModeReplace, (unsigned char*) &desktop, 1); //Set visible on all desktops

    QDialog::show();

    changeGeometry();

    emit keyboardVisibleChanged(true);
}

void KeyboardWindow::on_changeButton_clicked() {
    if (ui->keyboardsStack->currentWidget() != layouts.value(Symbol)) {
        ui->keyboardsStack->setCurrentWidget(layouts.value(Symbol));
    } else {
        ui->keyboardsStack->setCurrentWidget(layouts.value(currentKeyboardLayout));
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
                if (point.pos().y() > this->height() - 30) { //Cancel it
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
    } else if (obj == ui->splash) {
        if (event->type() == QEvent::Paint) {
            QPainter painter(ui->splash);
            painter.setBrush(ui->splash->palette().color(QPalette::WindowText));

            int width = 50 * getDPIScaling();
            int height = 30 * getDPIScaling();
            painter.drawRect(this->width() / 2 - width / 2, this->height() - height, width, height);

            QPolygon polygon;
            polygon.append(QPoint(this->width() / 2 - width / 2 - 20 * getDPIScaling(), this->height() - height));
            polygon.append(QPoint(this->width() / 2, this->height() - height - 50 * getDPIScaling()));
            polygon.append(QPoint(this->width() / 2 + width / 2 + 20 * getDPIScaling(), this->height() - height));

            painter.drawPolygon(polygon);
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

void KeyboardWindow::resizeEvent(QResizeEvent* event) {
    ui->enterOptionsWidget->setFixedSize(ui->enterOptionsWidget->sizeHint());
    ui->splash->setFixedSize(this->size());
}

void KeyboardWindow::on_splitButton_clicked()
{
    if (settings.value("keyboard/split", false).toBool()) {
        ui->spaceButton->sizePolicy().setHorizontalStretch(15);
        settings.setValue("keyboard/split", false);
    } else {
        ui->spaceButton->sizePolicy().setHorizontalStretch(10);
        settings.setValue("keyboard/split", true);
    }
}

void KeyboardWindow::screenResolutionResized() {
    if (this->isVisible()) {
        this->show();
    }
}

void KeyboardWindow::hide() {
    QDialog::hide();
    emit keyboardVisibleChanged(false);

    ui->suggestionBar->reset();
}

void KeyboardWindow::initTrayIcon() {
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

void KeyboardWindow::on_superKey_clicked()
{
    tPropertyAnimation* anim = new tPropertyAnimation(ui->otherKeyboardsFrame, "geometry");
    anim->setStartValue(ui->otherKeyboardsFrame->geometry());
    anim->setEndValue(QRect(0, this->height(), this->width(), this->height()));
    anim->setDuration(250);
    anim->setEasingCurve(QEasingCurve::OutCubic);
    connect(anim, SIGNAL(finished()), anim, SLOT(deleteLater()));
    anim->start();
}

int KeyboardWindow::findEmptyKeycode() {
    KeySym *keysyms = nullptr;
    int ksPerKc = 0;
    int kcLow, kcHigh;
    XDisplayKeycodes(QX11Info::display(), &kcLow, &kcHigh);
    //Get all available keysyms
    keysyms = XGetKeyboardMapping(QX11Info::display(), kcLow, kcHigh - kcLow, &ksPerKc);

    //Find an unused keycode
    for (int i = kcLow; i <= kcHigh; i++) {
        bool isEmpty = true;
        for (int j = 0; j < ksPerKc; j++) {
            int symindex = (i - kcLow) * ksPerKc + j;

            if (keysyms[symindex] != 0) {
                isEmpty = false;
                break;
            }
        }

        if (isEmpty) {
            XFree(keysyms);
            return i;
        }
    }
    XFree(keysyms);
    return -1;
}

void KeyboardWindow::on_spaceButton_held(const QPoint &point)
{
    spaceBarInitialPoint = point;
    spaceBarLastMovePoint = point.x();
}

void KeyboardWindow::on_spaceButton_letGo(const QPoint &point)
{
    if (ui->spaceButton->geometry().translated(-ui->spaceButton->geometry().topLeft()).contains(point) && abs(point.x() - spaceBarInitialPoint.x()) > 5) {
        ignoreSpaceBar = true;
    }
}

void KeyboardWindow::on_spaceButton_touchMoved(const QPoint &point)
{
    int distance = point.x() - spaceBarLastMovePoint;
    if (distance > 5 * getDPIScaling()) {
        ui->rightButton->click();
        spaceBarLastMovePoint = point.x();
    } else if (distance < -5 * getDPIScaling()) {
        ui->leftButton->click();
        spaceBarLastMovePoint = point.x();
    }
}

void KeyboardWindow::on_settingsButton_clicked()
{
    Settings* settings = new Settings();
    settings->show();
}

void KeyboardWindow::on_suggestionBar_wordSelected(const QString &word, int charactersToBackspace)
{
    for (int i = 0; i < charactersToBackspace; i++) {
        pressKeySym(XK_BackSpace);
    }

    for (QChar c : word) {
        QString str(c);

        QString keycode = str.toUtf8().toHex();

        if (keycode.length() == 1) {
            keycode.prepend("000");
        } else if (keycode.length() == 2) {
            keycode.prepend("00");
        } else if (keycode.length() == 3) {
            keycode.prepend("0");
        }

        keycode.prepend("U");

        bool oldShiftState = state->shift();
        if (c.isUpper()) {
            state->setShift(true);
        }
        pressKeySym(XStringToKeysym(keycode.toLocal8Bit().constData()));
        if (c.isUpper()) {
            state->setShift(oldShiftState);
        }
    }
}

void KeyboardWindow::changeGeometry() {
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
    XChangeProperty(QX11Info::display(), this->winId(), XInternAtom(QX11Info::display(), "_NET_WM_STRUT_PARTIAL", False),
                     XA_CARDINAL, 32, PropModeReplace, (unsigned char*) struts, 12);

    free(struts);

    this->setGeometry(screenGeometry.left(), screenGeometry.bottom() - this->height() + 1, screenGeometry.width(), this->height());
    ui->otherKeyboardsFrame->setGeometry(0, this->height(), this->width(), this->height());
}

void KeyboardWindow::setKeyboardType(QString type) {
    qDebug() << "Set type to " + type;

    ui->urlSlashButton->setVisible(false);
    if (type == "normal") {
        changeKeyboard(enUS);
        ui->suggestionBar->setVisible(true);
    } else if (type == "numeric") {
        changeKeyboard(Numeric);
        ui->suggestionBar->setVisible(false);
    } else if (type == "url") {
        changeKeyboard(enUS);
        ui->suggestionBar->setVisible(false);
        ui->urlSlashButton->setVisible(true);
    }
    //this->setFixedHeight(this->sizeHint().height());
    //changeGeometry();
}

void KeyboardWindow::changeKeyboard(Layouts layout) {
    currentKeyboardLayout = layout;
    ui->keyboardsStack->setCurrentWidget(layouts.value(layout));

    switch (layout) {
        case enUS:
            ui->spaceButton->setText("ENGLISH (US)");
            break;
        case Symbol:
            ui->spaceButton->setText("SYMBOLS");
            break;
        case Numeric:
            ui->spaceButton->setText("NUMERIC");
            break;
    }
}

void KeyboardWindow::setPredictive(bool predictive) {
    ui->suggestionBar->setVisible(predictive);
}
