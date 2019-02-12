#ifndef LAYOUTUR_H
#define LAYOUTUR_H

#include <QWidget>
#include <QDateTime>
#include <QSettings>
#include <QLabel>
#include "layout.h"

namespace Ui {
class LayoutUR;
}

class LayoutUR : public Layout
{
    Q_OBJECT

public:
    explicit LayoutUR(QWidget *parent = nullptr);
    ~LayoutUR();

    Layouts layoutType();

public slots:
    void pressKey();
    void buttonIterate(QWidget* wid);

private:
    Ui::LayoutUR *ui;

    void resizeEvent(QResizeEvent* event);
    bool event(QEvent* event);

    void updateSupplementaryCharacters();

    QDateTime capsLockPressedTime = QDateTime::currentDateTime();
    QSettings settings;

    int extraKeyTop;

    QWidget* extraKey;
    QLabel* extraKeyLabel;
};

#endif // LayoutUR_H
