#ifndef LAYOUTNUMERIC_H
#define LAYOUTNUMERIC_H

#include <QWidget>
#include <QDateTime>
#include <QSettings>
#include "layout.h"

namespace Ui {
class LayoutNumeric;
}

class LayoutNumeric : public Layout
{
    Q_OBJECT

public:
    explicit LayoutNumeric(QWidget *parent = nullptr);
    ~LayoutNumeric();

    Layouts layoutType();

public slots:
    void pressKey();
    void buttonIterate(QWidget* wid);

private:
    Ui::LayoutNumeric *ui;

    QDateTime capsLockPressedTime = QDateTime::currentDateTime();
    QSettings settings;
};

#endif // LAYOUTNUMERIC_H
