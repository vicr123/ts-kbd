#ifndef LAYOUTEMOJI_H
#define LAYOUTEMOJI_H

#include <QWidget>
#include "layout.h"

namespace Ui {
    class LayoutEmoji;
}

struct LayoutEmojiPrivate;
class LayoutEmoji : public Layout
{
        Q_OBJECT

    public:
        explicit LayoutEmoji(QWidget *parent = nullptr);
        ~LayoutEmoji();

        Layouts layoutType();

    private slots:
        void on_categoriesList_currentRowChanged(int currentRow);

    private:
        Ui::LayoutEmoji *ui;

        LayoutEmojiPrivate* d;
};

#endif // LAYOUTEMOJI_H
