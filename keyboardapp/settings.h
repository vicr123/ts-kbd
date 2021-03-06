#ifndef SETTINGS_H
#define SETTINGS_H

#include <QWidget>
#include <QSettings>

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

    void on_selectionList_currentRowChanged(int currentRow);

    void on_autocorrectCheckbox_toggled(bool checked);

    void on_dictionaryComboBox_currentTextChanged(const QString &arg1);

    void on_addLanguageButton_clicked();

    void on_addLanguageCancelButton_clicked();

    void saveLanguages();

    void on_newLanguageAddButton_clicked();

    void on_removeLanguageButton_clicked();

    private:
    Ui::Settings *ui;

    QSettings settings;
};

#endif // SETTINGS_H
