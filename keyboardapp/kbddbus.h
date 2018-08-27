#ifndef KBDDBUS_H
#define KBDDBUS_H

#include <QObject>
#include "keyboardwindow.h"

class KbdDbus : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.thesuite.tskbd")
public:
    explicit KbdDbus(QObject *parent = nullptr);
    bool reg();

Q_SIGNALS:
    Q_SCRIPTABLE void keyboardVisibleChanged(bool visible);

public Q_SLOTS:
    Q_SCRIPTABLE void showKeyboard();
    Q_SCRIPTABLE void hideKeyboard();
    Q_SCRIPTABLE int height();
    Q_SCRIPTABLE bool keyboardVisible();
    Q_SCRIPTABLE void setKeyboardType(QString type);

private:

};

#endif // KBDDBUS_H
