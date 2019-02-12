#include "settings.h"
#include "ui_settings.h"

#include <QDir>
#include <QMetaEnum>

#include "layouts/layout.h"

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

    QMetaEnum layouts = QMetaEnum::fromType<Layout::Layouts>();
    for (int i = 0; i < Layout::LastSelection; i++) {
        ui->layoutComboBox->addItem(layouts.valueToKey(i));
    }

    QDir dir("/usr/share/hunspell");
    for (QFileInfo file : dir.entryInfoList(QDir::Files)) {
        if (file.suffix() == "dic") {
            ui->dictionaryComboBox->addItem(file.baseName());
            if (settings.value("autocorrect/dictionary").toString() == file.baseName()) ui->dictionaryComboBox->setCurrentIndex(ui->dictionaryComboBox->count() - 1);
        }
    }

    int count = settings.beginReadArray("layouts/languages");
    for (int i = 0; i < count; i++) {
        settings.setArrayIndex(i);
        QListWidgetItem* lang = new QListWidgetItem();
        lang->setText(settings.value("name").toString());
        lang->setData(Qt::UserRole, settings.value("layout").toString());
        lang->setData(Qt::UserRole + 1, settings.value("dictionary").toString());
        ui->setLanguagesList->addItem(lang);
    }
    settings.endArray();
}

Settings::~Settings()
{
    delete ui;
}

void Settings::saveLanguages() {
    settings.beginWriteArray("layouts/languages");
    for (int i = 0; i < ui->setLanguagesList->count(); i++) {
        QListWidgetItem* item = ui->setLanguagesList->item(i);
        settings.setArrayIndex(i);
        settings.setValue("name", item->text());
        settings.setValue("layout", item->data(Qt::UserRole));
        settings.setValue("dictionary", item->data(Qt::UserRole + 1));
    }
    settings.endArray();
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
    //settings.setValue("autocorrect/dictionary", arg1);
}

void Settings::on_addLanguageButton_clicked()
{
    ui->languagesStack->setCurrentIndex(1);
}

void Settings::on_addLanguageCancelButton_clicked()
{
    ui->languagesStack->setCurrentIndex(0);
}

void Settings::on_newLanguageAddButton_clicked()
{
    QListWidgetItem* lang = new QListWidgetItem();
    lang->setText(ui->newLanguageName->text());
    lang->setData(Qt::UserRole, ui->layoutComboBox->currentText());
    lang->setData(Qt::UserRole + 1, ui->dictionaryComboBox->currentText());
    ui->setLanguagesList->addItem(lang);
    ui->languagesStack->setCurrentIndex(0);

    saveLanguages();
}

void Settings::on_removeLanguageButton_clicked()
{
    if (ui->setLanguagesList->currentRow() != -1) {
        delete ui->setLanguagesList->takeItem(ui->setLanguagesList->currentRow());
    }

    saveLanguages();
}
