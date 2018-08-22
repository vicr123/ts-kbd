#include "keybutton.h"

#include <QDebug>

KeyButton::KeyButton(QWidget* parent) : QPushButton(parent)
{
    this->setAttribute(Qt::WA_AcceptTouchEvents);
}

bool KeyButton::event(QEvent* event) {
    if (event->type() == QEvent::TouchBegin) {
        QTouchEvent* e = (QTouchEvent*) event;
        e->accept();

        this->setDown(true);

        return true;
    } else if (event->type() == QEvent::TouchEnd) {
        QTouchEvent* e = (QTouchEvent*) event;
        this->setDown(false);
        e->accept();

        emit tapped();
        return true;
    } else if (event->type() == QEvent::TouchCancel) {
        QTouchEvent* e = (QTouchEvent*) event;
        this->setDown(false);

        return true;
    }
    return QPushButton::event(event);
}
