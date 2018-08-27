#ifndef LAYOUTNUMERIC_H
#define LAYOUTNUMERIC_H

#include <QWidget>
#include <QDateTime>
#include <QSettings>

namespace Ui {
class LayoutNumeric;
}

class LayoutNumeric : public QWidget
{
    Q_OBJECT

public:
    explicit LayoutNumeric(QWidget *parent = nullptr);
    ~LayoutNumeric();

public slots:
    void pressKey();
    void buttonIterate(QWidget* wid);

signals:
    void typeKey(unsigned long key);
    void pushLetter(QString letter);

private:
    Ui::LayoutNumeric *ui;

    QDateTime capsLockPressedTime = QDateTime::currentDateTime();
    QSettings settings;
};

#endif // LAYOUTNUMERIC_H
