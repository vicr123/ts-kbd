#ifndef INPUTCONTEXT_H
#define INPUTCONTEXT_H

#include <qpa/qplatforminputcontext.h>
#include <tvirtualkeyboard.h>
#include <QSettings>
#include <QEvent>
#include <QMouseEvent>

class InputContext : public QPlatformInputContext
{
    Q_OBJECT
public:
    explicit InputContext();

    bool isValid() const;

    void showInputPanel();
    void hideInputPanel();
    bool isInputPanelVisible() const;
    void setFocusObject(QObject *object);

signals:

public slots:

private:
    QSettings* tsSettings;
    bool lastEventWasTouch = false;
    QObject* focusObject = nullptr;

    bool eventFilter(QObject* watched, QEvent* event);
};

#endif // INPUTCONTEXT_H
