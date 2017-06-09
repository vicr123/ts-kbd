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

namespace Ui {
class KeyboardWindow;
}

class KeyboardWindow : public QDialog
{
    Q_OBJECT

public:
    explicit KeyboardWindow(QWidget *parent = 0);
    ~KeyboardWindow();

    void show();
    void buttonIterate(QWidget* widget);

private slots:
    void pressKey();

    void on_changeButton_clicked();

    void on_hideKeyboard_clicked();

private:
    Ui::KeyboardWindow *ui;
};

#endif // KEYBOARDWINDOW_H
