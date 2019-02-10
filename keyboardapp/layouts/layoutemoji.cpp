#include "layoutemoji.h"
#include "ui_layoutemoji.h"

#include <QFile>
#include <QGridLayout>
#include <QScroller>
#include <QFontDatabase>
#include "keybutton.h"

#include <the-libs_global.h>

#include <X11/keysym.h>

struct Emoji {
    QString emoji;
    QList<Emoji> supplementalEmoji;
    QString qualification;

    QString name;
};

struct EmojiGroup {
    QString name;
    QList<Emoji> emoji;
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

    QFile emojiOrdering(":/data/emoji-test.txt");
    emojiOrdering.open(QFile::ReadOnly);

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

            struct Emoji e;
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
                            e.emoji.append(QString(raws, 2));
                        } else {
                            e.emoji.append(QChar(codepoint));
                        }
                    }
                } else if (state == 1) {
                    //Qualification
                    if (part == "#") {
                        state++;
                    } else {
                        e.qualification = part;
                    }
                } else {
                    e.name.append(part + " ");
                }
            }

            currentGroup.emoji.append(e);
        }
    }
    d->emojiGroups.append(currentGroup);

    ui->categoriesList->setFont(d->fnt);
    for (int i = 0; i < d->emojiGroups.count(); i++) {
        EmojiGroup g = d->emojiGroups.at(i);
        //if (g.name == "Component") continue;
        QListWidgetItem* category = new QListWidgetItem();
        category->setText(g.emoji.first().emoji);
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
    for (struct Emoji e : g.emoji) {
        if (e.qualification != "fully-qualified") continue;

        KeyButton* b = new KeyButton();
        b->setText(e.emoji);
        b->setFlat(true);
        b->setFixedWidth(ui->emojiSelectionArea->width() / 10);
        b->setFixedHeight(50 * theLibsGlobal::getDPIScaling());
        b->setFont(d->fnt);
        connect(b, &KeyButton::tapped, b, [=] {
            emit typeLetter(b->text());
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
