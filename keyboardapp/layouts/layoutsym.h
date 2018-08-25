#ifndef LAYOUTSYM_H
#define LAYOUTSYM_H

#include <QWidget>
#include <QDateTime>
#include <QSettings>

namespace Ui {
class LayoutSym;
}

class LayoutSym : public QWidget
{
    Q_OBJECT

public:
    explicit LayoutSym(QWidget *parent = nullptr);
    ~LayoutSym();

public slots:
    void pressKey();
    void buttonIterate(QWidget* wid);

signals:
    void typeKey(unsigned long key);

private:
    Ui::LayoutSym *ui;

    QDateTime capsLockPressedTime = QDateTime::currentDateTime();
    QSettings settings;
};

#endif // LAYOUTSYM_H
