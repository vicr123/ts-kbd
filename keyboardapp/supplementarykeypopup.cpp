#include "supplementarykeypopup.h"
#include "ui_supplementarykeypopup.h"

#include <QScreen>
#include <keybutton.h>

#include <QX11Info>
#include <X11/Xlib.h>
#include <X11/Xatom.h>

struct SupplementaryKeyPopupPrivate {
    QSize buttonSize;
    QList<KeyButton*> buttons;
    QPoint topLeft;
};

SupplementaryKeyPopup::SupplementaryKeyPopup(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SupplementaryKeyPopup)
{
    ui->setupUi(this);

    d = new SupplementaryKeyPopupPrivate();
    this->setWindowFlags(Qt::Dialog | Qt::WindowDoesNotAcceptFocus | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    this->setFocusPolicy(Qt::NoFocus);
}

SupplementaryKeyPopup::~SupplementaryKeyPopup()
{
    qDeleteAll(d->buttons);
    delete d;
    delete ui;
}

void SupplementaryKeyPopup::setButtonSize(QSize size) {
    d->buttonSize = size;
}

void SupplementaryKeyPopup::setButtons(QStringList buttons) {
    int col = 0, row = 0;
    for (QString b : buttons) {
        KeyButton* k = new KeyButton();
        k->setText(b);
        k->setFlat(true);
        k->setFixedSize(d->buttonSize);
        k->setFont(this->font());
        ui->keysLayout->addWidget(k, row, col);
        d->buttons.append(k);

        col++;
        if (col > 5) {
            col = 0;
            row++;
        }
    }
}

void SupplementaryKeyPopup::setPrimaryKeyTopLeft(QPoint topLeft) {
    d->topLeft = topLeft;
}

void SupplementaryKeyPopup::show() {
    this->setFixedSize(this->sizeHint());

    QRect newGeometry(d->topLeft - QPoint(10, 10), this->size());

    QRect screenGeometry = QApplication::screenAt(d->topLeft)->geometry();
    Qt::Corner finalCorner = Qt::TopLeftCorner;
    if (newGeometry.right() > screenGeometry.right()) {
        //Reverse everything
        finalCorner = Qt::TopRightCorner;
        newGeometry.moveTopRight(d->topLeft + QPoint(10 + d->buttonSize.width(), -10));
    }

    if (newGeometry.bottom() > screenGeometry.bottom()) {
        //Reverse everything
        if (finalCorner == Qt::TopLeftCorner) {
            finalCorner = Qt::BottomLeftCorner;
        } else {
            finalCorner = Qt::BottomRightCorner;
        }

        newGeometry.moveTop(newGeometry.top() - ((d->buttons.count() - 1) / 6) * d->buttonSize.height());
    }
    ui->keysLayout->setOriginCorner(finalCorner);

    QWidget::show();

    Atom DesktopWindowTypeAtom;
    DesktopWindowTypeAtom = XInternAtom(QX11Info::display(), "_NET_WM_WINDOW_TYPE_DOCK", False);
    int retval = XChangeProperty(QX11Info::display(), this->winId(), XInternAtom(QX11Info::display(), "_NET_WM_WINDOW_TYPE", False),
                     XA_ATOM, 32, PropModeReplace, (unsigned char*) &DesktopWindowTypeAtom, 1); //Change Window Type

    this->setGeometry(newGeometry);
    this->raise();
}

QString SupplementaryKeyPopup::buttonForScreenPoint(QPoint point) {
    for (KeyButton* k : d->buttons) {
        QRect geometry = k->geometry();
        geometry.moveTopLeft(k->mapToGlobal(QPoint(0, 0)));
        if (geometry.contains(point)) {
            return k->text();
        }
    }
    return "";
}
