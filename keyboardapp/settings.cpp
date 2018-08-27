#include "settings.h"
#include "ui_settings.h"

#include <QDir>

#include "keyboardstate.h"
extern KeyboardState* state;

extern float getDPIScaling();

Settings::Settings(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Settings)
{
    ui->setupUi(this);

    ui->selectionList->setFixedWidth(250 * getDPIScaling());
    this->resize(this->size() * getDPIScaling());

    ui->splitCheckBox->setChecked(state->split());
    ui->autocorrectCheckbox->setChecked(settings.value("autocorrect/on", true).toBool());

    ui->dictionaryComboBox->blockSignals(true);
    QDir dir("/usr/share/hunspell");
    for (QFileInfo file : dir.entryInfoList(QDir::Files)) {
        if (file.suffix() == "dic") {
            ui->dictionaryComboBox->addItem(file.baseName());
            if (settings.value("autocorrect/dictionary").toString() == file.baseName()) ui->dictionaryComboBox->setCurrentIndex(ui->dictionaryComboBox->count() - 1);
        }
    }
    ui->dictionaryComboBox->blockSignals(false);
}

Settings::~Settings()
{
    delete ui;
}

void Settings::on_splitCheckBox_toggled(bool checked)
{
    state->setSplit(checked);
}

void Settings::on_selectionList_currentRowChanged(int currentRow)
{
    ui->stack->setCurrentIndex(currentRow);
}

void Settings::on_autocorrectCheckbox_toggled(bool checked)
{
    settings.setValue("autocorrect/on", checked);
}

void Settings::on_dictionaryComboBox_currentTextChanged(const QString &arg1)
{
    settings.setValue("autocorrect/dictionary", arg1);
    emit state->dictionariesChanged();
}
