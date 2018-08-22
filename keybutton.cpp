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
        emit held();

        return true;
    } else if (event->type() == QEvent::TouchEnd) {
        QTouchEvent* e = (QTouchEvent*) event;
        this->setDown(false);

        if (this->geometry().translated(-this->geometry().topLeft()).contains(e->touchPoints().first().pos().toPoint())) {
            emit tapped();
        }
        emit letGo(e->touchPoints().first().pos().toPoint());

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
