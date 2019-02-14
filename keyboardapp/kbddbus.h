#ifndef KBDDBUS_H
#define KBDDBUS_H

#include <QObject>
#include "keyboardwindow.h"

struct KbdDbusPrivate;
class KbdDbus : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.thesuite.tskbd")
    public:
        explicit KbdDbus(QObject *parent = nullptr);
        ~KbdDbus();
        bool reg();

    Q_SIGNALS:
        Q_SCRIPTABLE void keyboardVisibleChanged(bool visible);

    public Q_SLOTS:
        Q_SCRIPTABLE void showKeyboard();
        Q_SCRIPTABLE void hideKeyboard();
        Q_SCRIPTABLE int height();
        Q_SCRIPTABLE bool keyboardVisible();
        Q_SCRIPTABLE void setKeyboardType(QString type);
        Q_SCRIPTABLE void setPredictive(bool predictive);
        Q_SCRIPTABLE void setHidden(bool hidden);
        Q_SCRIPTABLE void setSensitive(bool sensitive);
        Q_SCRIPTABLE void setAutoUppercase(bool autoUpperCase);
        Q_SCRIPTABLE void setEnterKeyType(QString enterKeyType);

    private:
        KbdDbusPrivate* d;
};

#endif // KBDDBUS_H
