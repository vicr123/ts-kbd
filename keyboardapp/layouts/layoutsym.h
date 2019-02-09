#ifndef LAYOUTSYM_H
#define LAYOUTSYM_H

#include <QWidget>
#include <QDateTime>
#include <QSettings>
#include "layout.h"

namespace Ui {
class LayoutSym;
}

class LayoutSym : public Layout
{
    Q_OBJECT

public:
    explicit LayoutSym(QWidget *parent = nullptr);
    ~LayoutSym();

    Layouts layoutType();

public slots:
    void pressKey();
    void buttonIterate(QWidget* wid);

signals:

private:
    Ui::LayoutSym *ui;

    QDateTime capsLockPressedTime = QDateTime::currentDateTime();
    QSettings settings;
};

#endif // LAYOUTSYM_H
