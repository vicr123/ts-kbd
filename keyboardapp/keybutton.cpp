#include "keybutton.h"

#include <QDebug>
#include <QTimer>
#include "supplementarykeypopup.h"
#include "keyboardstate.h"

extern KeyboardState* state;

struct KeyButtonPrivate {
    QStringList supplementaryCharacters;
    SupplementaryKeyPopup* keyPopup = nullptr;
    QTimer* supplementaryPopupTimer;

    QPoint oldTopLeft;

    QString shiftedOutput;
    QString normalOutput;
};

KeyButton::KeyButton(QWidget* parent) : QPushButton(parent)
{
    d = new KeyButtonPrivate;
    this->setAttribute(Qt::WA_AcceptTouchEvents);

    d->supplementaryPopupTimer = new QTimer();
    d->supplementaryPopupTimer->setInterval(300);
    d->supplementaryPopupTimer->setSingleShot(true);
    connect(d->supplementaryPopupTimer, &QTimer::timeout, this, [=] {
        //Show the supplementary popup dialog if needed
        if (!d->supplementaryCharacters.isEmpty() && d->oldTopLeft == this->mapToGlobal(QPoint(0, 0))) {
            d->keyPopup->setFont(this->parentWidget()->font());
            d->keyPopup->setButtonSize(this->size());
            d->keyPopup->setPrimaryKeyTopLeft(this->mapToGlobal(QPoint(0, 0)));

            QStringList buttons;
            buttons.append(this->text());
            buttons.append(d->supplementaryCharacters);
            d->keyPopup->setButtons(buttons);

            d->keyPopup->show();
        }
    });

    connect(state, &KeyboardState::shiftChanged, this, [=] {
        if (d->shiftedOutput != "") {
            if (state->capsLock() || state->shift()) {
                QPushButton::setText(d->shiftedOutput);
            } else {
                QPushButton::setText(d->normalOutput);
            }
        }
    });
    connect(state, &KeyboardState::capsLockChanged, this, [=] {
        if (d->shiftedOutput != "") {
            if (state->capsLock() || state->shift()) {
                QPushButton::setText(d->shiftedOutput);
            } else {
                QPushButton::setText(d->normalOutput);
            }
        }
    });
}

KeyButton::~KeyButton() {
    delete d;
}

bool KeyButton::event(QEvent* event) {
    if (event->type() == QEvent::TouchBegin) {
        QTouchEvent* e = (QTouchEvent*) event;
        e->accept();

        d->oldTopLeft = this->mapToGlobal(QPoint(0, 0));

        this->setDown(true);
        emit held(e->touchPoints().first().pos().toPoint());

        d->keyPopup = new SupplementaryKeyPopup();
        d->supplementaryPopupTimer->start();

        return true;
    } else if (event->type() == QEvent::TouchEnd) {
        QTouchEvent* e = (QTouchEvent*) event;
        this->setDown(false);

        emit letGo(e->touchPoints().first().pos().toPoint());

        if (d->keyPopup->isVisible()) {
            QString key = d->keyPopup->buttonForScreenPoint(this->mapToGlobal(e->touchPoints().first().pos().toPoint())).remove("◌");
            if (key != "") {
                emit typeSupplementary(key);
            }
        } else if (this->geometry().translated(-this->geometry().topLeft()).contains(e->touchPoints().first().pos().toPoint())) {
            emit tapped();
        }

        d->supplementaryPopupTimer->stop();
        d->keyPopup->deleteLater();
        d->keyPopup = nullptr;

        e->accept();
        return true;
    } else if (event->type() == QEvent::TouchUpdate) {
        QTouchEvent* e = (QTouchEvent*) event;

        QPoint pos = e->touchPoints().first().pos().toPoint();
        emit touchMoved(pos);
    } else if (event->type() == QEvent::TouchCancel) {
        QTouchEvent* e = (QTouchEvent*) event;
        this->setDown(false);
        emit letGo(e->touchPoints().first().pos().toPoint());

        return true;
    } else if (event->type() == QEvent::Resize) {
        this->setMinimumSize(QSize(this->height(), this->height()));
    }
    return QPushButton::event(event);
}

void KeyButton::setSupplementaryCharacters(QStringList characters) {
    d->supplementaryCharacters = characters;
}

void KeyButton::setShiftedOutput(QString shifted) {
    d->shiftedOutput = shifted;
}

QString KeyButton::shiftedOutput() {
    return d->shiftedOutput;
}

void KeyButton::setText(QString text) {
    QPushButton::setText(text);
    d->normalOutput = text;
}
