#include "suggestionbar.h"
#include "ui_suggestionbar.h"

#include <QDebug>
#include <QScroller>

#include "keyboardstate.h"
extern KeyboardState* state;

struct SuggestionBarPrivate {
    QList<KeyButton*> suggestions;

    QString currentWord;
    QString tentativeCorrection;
    Hunspell* dictionary;

    QStringList offensive;

    QSettings settings;
};

SuggestionBar::SuggestionBar(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SuggestionBar)
{
    ui->setupUi(this);

    d = new SuggestionBarPrivate();

    QFile offensiveFile(":/data/offensive-words.txt");
    offensiveFile.open(QFile::ReadOnly);

    while (!offensiveFile.atEnd()) {
        QString line = offensiveFile.readLine().trimmed();
        if (line.startsWith("#") || line == "") continue;
        d->offensive.append(line);
    }
    d->offensive.sort();
}

SuggestionBar::~SuggestionBar()
{
    delete d;
    delete ui;
}

void SuggestionBar::changeDictionary(QString dictionaryName) {
    QString dictionaryBase = "/usr/share/hunspell/" + dictionaryName;
    d->dictionary = new Hunspell((dictionaryBase + ".aff").toLocal8Bit().data(), (dictionaryBase + ".dic").toLocal8Bit().data());
    QScroller::grabGesture(ui->suggestedArea->viewport(), QScroller::LeftMouseButtonGesture);

    //Add custom words into dictionary
    d->dictionary->add("theShell");
    d->dictionary->add("theSlate");
    d->dictionary->add("theBeat");
    d->dictionary->add("theCaluclator");
    d->dictionary->add("theTerminal");
    d->dictionary->add("AstralMod");
    d->dictionary->add("Qt");
    d->dictionary->add("KDE");
    d->dictionary->add("Llanfairpwllgwyngyllgogerychwyrndrobwllllantysiliogogogoch");
}

void SuggestionBar::pushLetter(QString letter) {
    d->currentWord += letter;
    updateSuggestions();
}

void SuggestionBar::reset() {
    d->currentWord = "";
    updateSuggestions();
}

void SuggestionBar::updateSuggestions() {
    for (KeyButton* button : d->suggestions) {
        button->deleteLater();
    }
    d->suggestions.clear();
    d->tentativeCorrection = "";
    //ui->currentWord->setText(currentWord);

    QFont font = this->font();
    font.setPointSize(20);

    QStringList suggestions;
    QStringList autocorrections;
    QStringList shownWords;

    //Hardcoded autocorrections
    if (d->currentWord == "i") autocorrections.append("I");
    if (d->currentWord == "im") autocorrections.append("I'm");
    if (d->currentWord == "ill") autocorrections.append("I'll");
    if (d->currentWord == "ive") autocorrections.append("I've");
    if (d->currentWord == "dont") autocorrections.append("don't");
    if (d->currentWord == "Dont") autocorrections.append("Don't");
    if (d->currentWord == "couldnt") autocorrections.append("couldn't");
    if (d->currentWord == "Couldnt") autocorrections.append("Couldn't");
    if (d->currentWord == "wouldnt") autocorrections.append("Wouldn't");
    if (d->currentWord == "Wouldnt") autocorrections.append("Wouldn't");
    if (d->currentWord == "didnt") autocorrections.append("didn't");
    if (d->currentWord == "Didnt") autocorrections.append("Didn't");
    if (d->currentWord == "hes") autocorrections.append("he's");
    if (d->currentWord == "Hes") autocorrections.append("He's");
    if (d->currentWord == "shes") autocorrections.append("she's");
    if (d->currentWord == "Shes") autocorrections.append("She's");

    if (d->currentWord == "its") shownWords.append("it's");
    if (d->currentWord == "Its") shownWords.append("It's");

    QVector<std::string> strings = QVector<std::string>::fromStdVector(d->dictionary->suggest(d->currentWord.toStdString()));
    for (std::string string : strings) {
        QString str = QString::fromStdString(string);

        if (std::binary_search(d->offensive.begin(), d->offensive.end(), str)) {
            continue;
        }

        int difference = abs(str.length() - d->currentWord.length());
        for (int i = 0; i < qMin(str.length(), d->currentWord.length()); i++) {
            if (str.at(i).toLower() != d->currentWord.at(i).toLower()) difference++;
        }

        if (difference == 1) {
            //Only one character difference between suggestion and word
            autocorrections.append(str);
        } else if (str.toLower() != str && str.toLower() == d->currentWord.toLower()) {
            //Capitalisation difference
            autocorrections.append(str);
        } else if (str == d->currentWord) {
            //Do nothing; ignore this suggestion
        } else {
            suggestions.append(str);
        }
    }


    bool pendingAutocorrect = true;
    bool firstWord = true;
    if (!d->settings.value("autocorrect/on", true).toBool()) pendingAutocorrect = false; //Disable Autocorrect if user turns setting off
    if (d->dictionary->spell(d->currentWord.toStdString()) && d->currentWord != "i") pendingAutocorrect = false; //Disable Autocorrect if word exists in dictionary
    if (!this->isVisible()) pendingAutocorrect = false; //Disable Autocorrect if suggestion bar is invisible

    if (d->currentWord != "") shownWords.append(d->currentWord); //Allow user to override autocorrect
    shownWords.append(autocorrections);
    shownWords.append(suggestions);
    for (QString suggestion : shownWords) {
        KeyButton* button = new KeyButton();
        button->setFont(font);
        button->setText(suggestion);
        connect(button, &KeyButton::tapped, [=] {
            emit wordSelected(suggestion + " ", d->currentWord.length());
            reset();
        });

        if (pendingAutocorrect && autocorrections.length() > 0 && !firstWord) {
            //Mark for autocorrection
            QFont f = font;
            f.setBold(true);
            button->setFont(f);
            pendingAutocorrect = false;
            d->tentativeCorrection = suggestion;
        } else {
            button->setFlat(true);
        }

        firstWord = false;
        ((QBoxLayout*) ui->suggestedWords->layout())->insertWidget(d->suggestions.count(), button);
        d->suggestions.append(button);
    }
}

void SuggestionBar::backspace() {
    d->currentWord.chop(1);
    updateSuggestions();
}

bool SuggestionBar::hasAutocorrection() {
    return d->tentativeCorrection != "";
}

void SuggestionBar::acceptAutocorrection() {
    if (d->tentativeCorrection != "") {
        emit wordSelected(d->tentativeCorrection, d->currentWord.length());
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
