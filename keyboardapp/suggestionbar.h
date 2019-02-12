#ifndef SUGGESTIONBAR_H
#define SUGGESTIONBAR_H

#include <QWidget>
#include <QSettings>
#include <hunspell/hunspell.hxx>

#include <QPushButton>
#include <keybutton.h>

namespace Ui {
class SuggestionBar;
}

class SuggestionBar : public QWidget
{
    Q_OBJECT

public:
    explicit SuggestionBar(QWidget *parent = nullptr);
    ~SuggestionBar();

    bool hasAutocorrection();
    QPushButton* clearButton();

public slots:
    void pushLetter(QString letter);
    void backspace();
    void reset();
    void acceptAutocorrection();

    void updateSuggestions();
    void changeDictionary(QString dictionaryName);

signals:
    void wordSelected(QString word, int charactersToBackspace);

    private slots:
    void on_clearSuggestionBarButton_clicked();

    private:
    Ui::SuggestionBar *ui;

    QList<KeyButton*> suggestions;

    QString currentWord;
    QString tentativeCorrection;
    Hunspell* dictionary;

    QSettings settings;
};

#endif // SUGGESTIONBAR_H
