#ifndef SETTINGS_H
#define SETTINGS_H

#include <QWidget>

namespace Ui {
class Settings;
}

class Settings : public QWidget
{
    Q_OBJECT

public:
    explicit Settings(QWidget *parent = nullptr);
    ~Settings();

private slots:
    void on_splitCheckBox_toggled(bool checked);

private:
    Ui::Settings *ui;
};

#endif // SETTINGS_H
