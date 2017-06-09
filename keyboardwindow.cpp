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

    QFont fnt = this->font();
    fnt.setPointSize(20);
    this->setFont(fnt);

    buttonIterate(this);
    buttonIterate(ui->alphaPage);
    buttonIterate(ui->symbolPage);
    buttonIterate(ui->bottomFrame);

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
    if (button == ui->shift || button == ui->changeButton || button == ui->hideKeyboard) return; //Ignore SHIFT, change and hide key

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
    XTestFakeKeyEvent(QX11Info::display(), XKeysymToKeycode(QX11Info::display(), pressedKey), true, 0);
    XTestFakeKeyEvent(QX11Info::display(), XKeysymToKeycode(QX11Info::display(), pressedKey), false, 0);
    if (isShift || ui->shift->isChecked()) {
        XTestFakeKeyEvent(QX11Info::display(), XKeysymToKeycode(QX11Info::display(), XK_Shift_L), false, 0);
    }
    XFlush(QX11Info::display());

    ui->shift->setChecked(false);

    QSoundEffect* keySound = new QSoundEffect();
    keySound->setSource(QUrl("qrc:/sounds/keyclick.wav"));
    keySound->play();
    connect(keySound, SIGNAL(playingChanged()), keySound, SLOT(deleteLater()));
}


void KeyboardWindow::show() {
    Atom DesktopWindowTypeAtom;
    DesktopWindowTypeAtom = XInternAtom(QX11Info::display(), "_NET_WM_WINDOW_TYPE_DOCK", False);
    int retval = XChangeProperty(QX11Info::display(), this->winId(), XInternAtom(QX11Info::display(), "_NET_WM_WINDOW_TYPE", False),
                     XA_ATOM, 32, PropModeReplace, (unsigned char*) &DesktopWindowTypeAtom, 1); //Change Window Type

    unsigned long desktop = 0xFFFFFFFF;
    retval = XChangeProperty(QX11Info::display(), this->winId(), XInternAtom(QX11Info::display(), "_NET_WM_DESKTOP", False),
                     XA_CARDINAL, 32, PropModeReplace, (unsigned char*) &desktop, 1); //Set visible on all desktops

    QDialog::show();

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
