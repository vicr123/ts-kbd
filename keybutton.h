#ifndef KEYBUTTON_H
#define KEYBUTTON_H

#include <QPushButton>
#include <QTouchEvent>

class KeyButton : public QPushButton
{
    Q_OBJECT
public:
    explicit KeyButton(QWidget* parent = nullptr);

signals:
    void tapped();

public slots:

private:
    bool event(QEvent* event);
};

#endif // KEYBUTTON_H
