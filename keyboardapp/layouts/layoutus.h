#ifndef LAYOUTUS_H
#define LAYOUTUS_H

#include <QWidget>
#include <QDateTime>
#include <QSettings>

namespace Ui {
class LayoutUS;
}

class LayoutUS : public QWidget
{
    Q_OBJECT

public:
    explicit LayoutUS(QWidget *parent = nullptr);
    ~LayoutUS();

public slots:
    void pressKey();
    void buttonIterate(QWidget* wid);

signals:
    void typeKey(unsigned long key);

private:
    Ui::LayoutUS *ui;

    QDateTime capsLockPressedTime = QDateTime::currentDateTime();
    QSettings settings;
};

#endif // LAYOUTUS_H
