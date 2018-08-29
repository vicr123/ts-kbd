#ifndef KEYBOARDSTATE_H
#define KEYBOARDSTATE_H

#include <QObject>
#include <QSettings>

class KeyboardState : public QObject
{
    Q_OBJECT

    public:
        explicit KeyboardState(QObject *parent = nullptr);

        bool capsLock();
        void setCapsLock(bool isOn);

        bool shift();
        void setShift(bool isOn);

        bool split();
        void setSplit(bool split);

        void setEnterKeyType(Qt::EnterKeyType type);
        Qt::EnterKeyType enterKeyType();
    signals:
        void capsLockChanged(bool isOn);
        void shiftChanged(bool isOn);
        void splitChanged(bool split);
        void dictionariesChanged();
        void enterKeyTypeChanged(Qt::EnterKeyType type);

    public slots:

    private:
        bool caps = false;
        bool sh = false;
        bool sp;

        QSettings settings;
        Qt::EnterKeyType ret;
};

#endif // KEYBOARDSTATE_H
