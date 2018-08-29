#include "returnkey.h"

#include "keyboardstate.h"
extern KeyboardState* state;

ReturnKey::ReturnKey(QWidget *parent) : KeyButton(parent)
{
    connect(state, &KeyboardState::enterKeyTypeChanged, [=](Qt::EnterKeyType newType) {
        switch (newType) {
            case Qt::EnterKeyReturn:
                this->setText("⏎");
                break;
            case Qt::EnterKeyGo:
                this->setText("GO");
                break;
            case Qt::EnterKeySearch:
                this->setText("SEARCH");
                break;
        }
    });
}
