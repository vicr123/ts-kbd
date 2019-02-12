#ifndef LAYOUTVI_H
#define LAYOUTVI_H

#include <QWidget>
#include <QDateTime>
#include <QSettings>
#include <QLabel>
#include "layout.h"

namespace Ui {
class LayoutUS;
}

class LayoutVI : public Layout
{
    Q_OBJECT

public:
    explicit LayoutVI(QWidget *parent = nullptr);
    ~LayoutVI();

    Layouts layoutType();

public slots:
    void pressKey();
    void buttonIterate(QWidget* wid);

private:
    Ui::LayoutUS *ui;

    void resizeEvent(QResizeEvent* event);
    bool event(QEvent* event);

    void updateSupplementaryCharacters();

    QDateTime capsLockPressedTime = QDateTime::currentDateTime();
    QSettings settings;

    int extraKeyTop;

    QWidget* extraKey;
    QLabel* extraKeyLabel;
};

#endif // LayoutVI_H
