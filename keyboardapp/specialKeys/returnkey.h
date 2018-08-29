#ifndef RETURNKEY_H
#define RETURNKEY_H

#include <QWidget>
#include "keybutton.h"

class ReturnKey : public KeyButton
{
        Q_OBJECT
    public:
        explicit ReturnKey(QWidget *parent = nullptr);

    signals:

    public slots:
};

#endif // RETURNKEY_H
