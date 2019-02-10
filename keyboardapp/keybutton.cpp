#include "keybutton.h"

#include <QDebug>
#include <QTimer>
#include "supplementarykeypopup.h"

struct KeyButtonPrivate {
    QList<QString> supplementaryCharacters;
    SupplementaryKeyPopup* keyPopup = nullptr;
    QTimer* supplementaryPopupTimer;
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
        if (!d->supplementaryCharacters.isEmpty()) {
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
}

KeyButton::~KeyButton() {
    delete d;
}

bool KeyButton::event(QEvent* event) {
    if (event->type() == QEvent::TouchBegin) {
        QTouchEvent* e = (QTouchEvent*) event;
        e->accept();

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
            QString key = d->keyPopup->buttonForScreenPoint(e->touchPoints().first().screenPos().toPoint());
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
