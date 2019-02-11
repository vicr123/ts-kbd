#include "layoutemoji.h"
#include "ui_layoutemoji.h"

#include <QFile>
#include <QGridLayout>
#include <QScroller>
#include <QFontDatabase>
#include <QSoundEffect>
#include <QSharedPointer>
#include "keybutton.h"

#include <the-libs_global.h>

#include <X11/keysym.h>

struct Emoji {
    QString emoji;
    QList<QSharedPointer<Emoji>> supplementalEmoji;
    QString qualification;

    QString name;
};

struct EmojiGroup {
    QString name;
    QList<QSharedPointer<Emoji>> emoji;
};

struct LayoutEmojiPrivate {
    QList<EmojiGroup> emojiGroups;
    QList<KeyButton*> currentButtons;

    QFont fnt;
};

LayoutEmoji::LayoutEmoji(QWidget *parent) :
    Layout(parent),
    ui(new Ui::LayoutEmoji)
{
    ui->setupUi(this);

    d = new LayoutEmojiPrivate();

    ui->categoriesList->setFixedWidth(ui->categoriesList->width() * theLibsGlobal::getDPIScaling());

    //Decide on a font
    QString fontFamily;
    QFontDatabase fontDb;
    if (fontDb.hasFamily("Noto Color Emoji")) {
        fontFamily = "Noto Color Emoji";
    }

    if (fontFamily == "") {
        //Warn about emoji support
    } else {
        d->fnt.setFamily(fontFamily);
        d->fnt.setPointSize(20);
    }

    ui->emojiSelectionWidget->setFont(d->fnt);

    QFile emojiOrdering(":/data/emoji-test.txt");
    emojiOrdering.open(QFile::ReadOnly);

    QSharedPointer<struct Emoji> lastBaseEmoji;
    EmojiGroup currentGroup;
    while (!emojiOrdering.atEnd()) {
        QString line = emojiOrdering.readLine().trimmed();
        if (line == "") {
            //Do nothing
        } else if (line.startsWith("#")) {
            //Comment line
            if (line.startsWith("# group: ")) {
                if (currentGroup.name != "") {
                    //Add the completed group
                    d->emojiGroups.append(currentGroup);
                    currentGroup = EmojiGroup();
                }

                currentGroup.name = line.mid(9);
            }
        } else {
            //Emoji line
            QStringList parts = line.split(" ", QString::SkipEmptyParts);

            QSharedPointer<struct Emoji> e(new struct Emoji());
            int state = 0;
            for (QString part : parts) {
                if (state == 0) {
                    //Emoji codepoints
                    if (part == ";") {
                        state++;
                    } else {
                        uint codepoint = part.toUInt(nullptr, 16);
                        if (QChar::requiresSurrogates(codepoint)) {
                            QChar raws[2];
                            raws[0] = QChar::highSurrogate(codepoint);
                            raws[1] = QChar::lowSurrogate(codepoint);
                            e->emoji.append(QString(raws, 2));
                        } else {
                            e->emoji.append(QChar(codepoint));
                        }
                    }
                } else if (state == 1) {
                    //Qualification
                    if (part == "#") {
                        state++;
                    } else {
                        e->qualification = part;
                    }
                } else if (state == 2) {
                    //Ignore the first part of the comment
                    state++;
                } else {
                    e->name.append(part + " ");
                }
            }
            e->name = e->name.trimmed();

            bool add = true;
            if (!lastBaseEmoji.isNull() && e->qualification == "fully-qualified") {
                QRegularExpression exp(QString("%1: (light|medium(-(light|dark))?|dark) skin tone").arg(lastBaseEmoji->name));
                QRegularExpressionMatch match = exp.match(e->name);
                if (match.hasMatch()) {
                    lastBaseEmoji->supplementalEmoji.append(e);
                    add = false;
                }
            }

            if (add) {
                if (e->qualification == "fully-qualified") {
                    lastBaseEmoji = e;
                }
                currentGroup.emoji.append(e);
            }
        }
    }
    d->emojiGroups.append(currentGroup);

    ui->categoriesList->setFont(d->fnt);
    for (int i = 0; i < d->emojiGroups.count(); i++) {
        EmojiGroup g = d->emojiGroups.at(i);
        //if (g.name == "Component") continue;
        QListWidgetItem* category = new QListWidgetItem();
        category->setText(g.emoji.first()->emoji);
        category->setData(Qt::UserRole, i);
        ui->categoriesList->addItem(category);
    }

    QScroller::grabGesture(ui->categoriesList, QScroller::LeftMouseButtonGesture);
    QScroller::grabGesture(ui->emojiSelectionArea, QScroller::TouchGesture);
}

LayoutEmoji::~LayoutEmoji()
{
    delete ui;
}

Layout::Layouts LayoutEmoji::layoutType() {
    return Emoji;
}

void LayoutEmoji::on_categoriesList_currentRowChanged(int currentRow)
{
    while (d->currentButtons.count() != 0) {
        KeyButton* b = d->currentButtons.takeFirst();
        b->deleteLater();
        d->currentButtons.removeOne(b);
    }

    QGridLayout* layout = new QGridLayout();
    delete ui->emojiSelectionWidget->layout();
    ui->emojiSelectionWidget->setLayout(layout);
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);

    int row = 0, col = 0;
    EmojiGroup g = d->emojiGroups.at(currentRow);
    for (QSharedPointer<struct Emoji> e : g.emoji) {
        if (e->qualification != "fully-qualified") continue;

        KeyButton* b = new KeyButton();
        b->setText(e->emoji);
        b->setFlat(true);
        b->setFixedWidth(ui->emojiSelectionArea->width() / 10);
        b->setFixedHeight(50 * theLibsGlobal::getDPIScaling());
        b->setFont(d->fnt);

        QStringList supplementaryCharacters;
        for (QSharedPointer<struct Emoji> supplementary : e->supplementalEmoji) {
            supplementaryCharacters.append(supplementary->emoji);
        }
        b->setSupplementaryCharacters(supplementaryCharacters);

        connect(b, &KeyButton::tapped, b, [=] {
            emit typeLetter(b->text());

            QSoundEffect* keySound = new QSoundEffect();
            keySound->setSource(QUrl("qrc:/sounds/keyclick.wav"));
            keySound->play();
            connect(keySound, SIGNAL(playingChanged()), keySound, SLOT(deleteLater()));
        });

        connect(b, &KeyButton::typeSupplementary, [=](QString supplementary) {
            emit typeLetter(supplementary);

            QSoundEffect* keySound = new QSoundEffect();
            keySound->setSource(QUrl("qrc:/sounds/keyclick.wav"));
            keySound->play();
            connect(keySound, SIGNAL(playingChanged()), keySound, SLOT(deleteLater()));

        });
        layout->addWidget(b, row, col);
        d->currentButtons.append(b);

        col++;
        if (col > 9) {
            col = 0;
            row++;
        }
    }
}

void LayoutEmoji::on_backspace_tapped()
{
    emit typeKey(XK_BackSpace);
}

void LayoutEmoji::on_returnButton_tapped()
{
    emit typeKey(XK_Return);
}
