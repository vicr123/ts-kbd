#ifndef KEYBOARDWINDOW_H
#define KEYBOARDWINDOW_H

#include <QDialog>
#include <QDesktopWidget>
#include <QX11Info>
#include <QDebug>
#include <QSoundEffect>
#include <QTimer>
#include <QString>
#include <QByteArray>
#include <QPushButton>
#include <QStackedWidget>
#include <QSystemTrayIcon>
#include <QEvent>
#include <QGestureEvent>
#include <tpropertyanimation.h>
#include <QLayout>
#include <QSettings>

namespace Ui {
class KeyboardWindow;
}

class KeyboardWindow : public QDialog
{
    Q_OBJECT

public:
    explicit KeyboardWindow(QWidget *parent = nullptr);
    ~KeyboardWindow();

    void show();
    void hide();
    void buttonIterate(QWidget* widget);
    void initTrayIcon();

private slots:
    void pressKey();

    void on_changeButton_clicked();

    void on_hideKeyboard_clicked();

    void on_ctrlKey_clicked();

    void on_altKey_clicked();

    void on_tabButton_clicked();

    void on_shift_clicked(bool checked);

    void on_returnButton_held(QPoint holdPoint);

    void on_returnButton_letGo(QPoint letGoPoint);

    void on_splitButton_clicked();

    void screenResolutionResized();

    void on_superKey_clicked();

    int findEmptyKeycode();

    void on_spaceButton_held(const QPoint &);

    void on_spaceButton_letGo(const QPoint &);

    void on_spaceButton_touchMoved(const QPoint &);

signals:
    void keyboardVisibleChanged(bool isVisible);

private:
    Ui::KeyboardWindow *ui;

    bool capsLock = false;
    QDateTime capsLockPressedTime = QDateTime::currentDateTime();

    bool event(QEvent *event);
    bool eventFilter(QObject *obj, QEvent *event);
    void resizeEvent(QResizeEvent* event);

    bool ignoreSpaceBar = false;
    QPoint spaceBarInitialPoint;
    int spaceBarLastMovePoint;

    QSettings settings;
};

#endif // KEYBOARDWINDOW_H
