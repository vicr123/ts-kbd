#include "settings.h"
#include "ui_settings.h"

#include "keyboardstate.h"
extern KeyboardState* state;

Settings::Settings(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Settings)
{
    ui->setupUi(this);

    ui->splitCheckBox->setChecked(state->split());
}

Settings::~Settings()
{
    delete ui;
}

void Settings::on_splitCheckBox_toggled(bool checked)
{
    state->setSplit(checked);
}
