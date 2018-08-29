#ifndef LAYOUTUS_H
#define LAYOUTUS_H

#include <QWidget>
#include <QDateTime>
#include <QSettings>
#include <QLabel>

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
    void pushLetter(QString letter);

private:
    Ui::LayoutUS *ui;

    void resizeEvent(QResizeEvent* event);
    bool event(QEvent* event);

    QDateTime capsLockPressedTime = QDateTime::currentDateTime();
    QSettings settings;

    QWidget* extraKey;
    QLabel* extraKeyLabel;
};

#endif // LAYOUTUS_H
