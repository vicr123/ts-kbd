#ifndef KEYBUTTON_H
#define KEYBUTTON_H

#include <QPushButton>
#include <QTouchEvent>

struct KeyButtonPrivate;

class KeyButton : public QPushButton
{
    Q_OBJECT
    public:
        explicit KeyButton(QWidget* parent = nullptr);
        ~KeyButton();

        Q_PROPERTY(QString shiftedOutput READ shiftedOutput WRITE setShiftedOutput)

        QString shiftedOutput();

    signals:
        void tapped();
        void typeSupplementary(QString supplementary);
        void held(QPoint heldPoint);
        void letGo(QPoint letGoPoint);
        void touchMoved(QPoint pos);

    public slots:
        void setSupplementaryCharacters(QStringList characters);
        void setShiftedOutput(QString shifted);
        void setText(QString text);

    private:
        KeyButtonPrivate* d;

        bool event(QEvent* event);
};

#endif // KEYBUTTON_H
