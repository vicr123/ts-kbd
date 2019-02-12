#ifndef LAYOUT_H
#define LAYOUT_H

#include <QWidget>

class Layout : public QWidget
{
        Q_OBJECT
    public:
        explicit Layout(QWidget *parent = nullptr);

        enum Layouts {
            enUS = 0,
            viVN,
            urPK,

            LastSelection,
            Symbol,
            Numeric,
            Emoji
        };
        Q_ENUM(Layouts)

        virtual Layouts layoutType() = 0;

    signals:
        void typeKey(unsigned long key);
        void typeLetter(QString letter);
        void pushLetter(QString letter);

    public slots:

    protected:
        void typeString(QString str);
};

#endif // LAYOUT_H
