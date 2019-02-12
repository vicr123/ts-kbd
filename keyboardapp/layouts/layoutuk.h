#ifndef LAYOUTUK_H
#define LAYOUTUK_H

#include <QWidget>
#include <QDateTime>
#include <QSettings>
#include <QLabel>
#include "layout.h"

namespace Ui {
class LayoutUK;
}

class LayoutUK : public Layout
{
    Q_OBJECT

public:
    explicit LayoutUK(QWidget *parent = nullptr);
    ~LayoutUK();

    Layouts layoutType();

public slots:
    void pressKey();
    void buttonIterate(QWidget* wid);

private:
    Ui::LayoutUK *ui;

    void resizeEvent(QResizeEvent* event);
    bool event(QEvent* event);

    void updateSupplementaryCharacters();

    QDateTime capsLockPressedTime = QDateTime::currentDateTime();
    QSettings settings;

    int extraKeyTop;

    QWidget* extraKey;
    QLabel* extraKeyLabel;
};

#endif // LayoutUK_H
