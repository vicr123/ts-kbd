#include "suggestionbar.h"
#include "ui_suggestionbar.h"

#include <QDebug>
#include <QScroller>

#include "keyboardstate.h"
extern KeyboardState* state;

SuggestionBar::SuggestionBar(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SuggestionBar)
{
    ui->setupUi(this);

    connect(state, SIGNAL(dictionariesChanged()), this, SLOT(setupDictionary()));
    setupDictionary();
}

SuggestionBar::~SuggestionBar()
{
    delete ui;
}

void SuggestionBar::setupDictionary() {
    QString dictionaryBase = "/usr/share/hunspell/" + settings.value("autocorrect/dictionary").toString();
    dictionary = new Hunspell((dictionaryBase + ".aff").toLocal8Bit().data(), (dictionaryBase + ".dic").toLocal8Bit().data());
    QScroller::grabGesture(ui->suggestedArea->viewport(), QScroller::LeftMouseButtonGesture);

    //Add custom words into dictionary
    dictionary->add("theShell");
    dictionary->add("theSlate");
    dictionary->add("theBeat");
    dictionary->add("theCaluclator");
    dictionary->add("theTerminal");
    dictionary->add("AstralMod");
    dictionary->add("Qt");
    dictionary->add("KDE");
    dictionary->add("Llanfairpwllgwyngyllgogerychwyrndrobwllllantysiliogogogoch");
}

void SuggestionBar::pushLetter(QString letter) {
    currentWord += letter;
    updateSuggestions();
}

void SuggestionBar::reset() {
    currentWord = "";
    updateSuggestions();
}

void SuggestionBar::updateSuggestions() {
    for (KeyButton* button : suggestions) {
        button->deleteLater();
    }
    suggestions.clear();
    tentativeCorrection = "";
    //ui->currentWord->setText(currentWord);

    QFont font = this->font();
    font.setPointSize(20);

    QStringList suggestions;
    QStringList autocorrections;
    QStringList shownWords;

    //Hardcoded autocorrections
    if (currentWord == "i") autocorrections.append("I");
    if (currentWord == "im") autocorrections.append("I'm");
    if (currentWord == "ill") autocorrections.append("I'll");
    if (currentWord == "ive") autocorrections.append("I've");
    if (currentWord == "dont") autocorrections.append("don't");
    if (currentWord == "Dont") autocorrections.append("Don't");
    if (currentWord == "couldnt") autocorrections.append("couldn't");
    if (currentWord == "Couldnt") autocorrections.append("Couldn't");
    if (currentWord == "wouldnt") autocorrections.append("Wouldn't");
    if (currentWord == "Wouldnt") autocorrections.append("Wouldn't");
    if (currentWord == "didnt") autocorrections.append("didn't");
    if (currentWord == "Didnt") autocorrections.append("Didn't");
    if (currentWord == "hes") autocorrections.append("he's");
    if (currentWord == "Hes") autocorrections.append("He's");
    if (currentWord == "shes") autocorrections.append("she's");
    if (currentWord == "Shes") autocorrections.append("She's");

    if (currentWord == "its") shownWords.append("it's");
    if (currentWord == "Its") shownWords.append("It's");

    QVector<std::string> strings = QVector<std::string>::fromStdVector(dictionary->suggest(currentWord.toStdString()));
    for (std::string string : strings) {
        QString str = QString::fromStdString(string);

        int difference = abs(str.length() - currentWord.length());
        for (int i = 0; i < qMin(str.length(), currentWord.length()); i++) {
            if (str.at(i).toLower() != currentWord.at(i).toLower()) difference++;
        }

        if (difference == 1) {
            //Only one character difference between suggestion and word
            autocorrections.append(str);
        } else if (str.toLower() != str && str.toLower() == currentWord.toLower()) {
            //Capitalisation difference
            autocorrections.append(str);
        } else if (str == currentWord) {
            //Do nothing; ignore this suggestion
        } else {
            suggestions.append(str);
        }
    }


    bool pendingAutocorrect = true;
    bool firstWord = true;
    if (!settings.value("autocorrect/on", true).toBool()) pendingAutocorrect = false; //Disable Autocorrect if user turns setting off
    if (dictionary->spell(currentWord.toStdString()) && currentWord != "i") pendingAutocorrect = false; //Disable Autocorrect if word exists in dictionary
    if (!this->isVisible()) pendingAutocorrect = false; //Disable Autocorrect if suggestion bar is invisible

    if (currentWord != "") shownWords.append(currentWord); //Allow user to override autocorrect
    shownWords.append(autocorrections);
    shownWords.append(suggestions);
    for (QString suggestion : shownWords) {
        KeyButton* button = new KeyButton();
        button->setFont(font);
        button->setText(suggestion);
        connect(button, &KeyButton::tapped, [=] {
            emit wordSelected(suggestion + " ", currentWord.length());
            reset();
        });

        if (pendingAutocorrect && autocorrections.length() > 0 && !firstWord) {
            //Mark for autocorrection
            QFont f = font;
            f.setBold(true);
            button->setFont(f);
            pendingAutocorrect = false;
            tentativeCorrection = suggestion;
        } else {
            button->setFlat(true);
        }

        firstWord = false;
        ((QBoxLayout*) ui->suggestedWords->layout())->insertWidget(this->suggestions.count(), button);
        this->suggestions.append(button);
    }
}

void SuggestionBar::backspace() {
    currentWord.chop(1);
    updateSuggestions();
}

bool SuggestionBar::hasAutocorrection() {
    return tentativeCorrection != "";
}

void SuggestionBar::acceptAutocorrection() {
    if (tentativeCorrection != "") {
        emit wordSelected(tentativeCorrection, currentWord.length());
    }
    reset();
}

void SuggestionBar::on_clearSuggestionBarButton_clicked()
{
    this->reset();
}

QPushButton* SuggestionBar::clearButton() {
    return ui->clearSuggestionBarButton;
}
