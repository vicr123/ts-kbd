#include "layoutvi.h"
#include "ui_layoutus.h"

#include <QSoundEffect>
#include <QDebug>
#include <tvariantanimation.h>
#include "keyboardstate.h"

#include <X11/Xlib.h>
#include <X11/keysym.h>

extern KeyboardState* state;
extern float getDPIScaling();

LayoutVI::LayoutVI(QWidget *parent) :
    Layout(parent),
    ui(new Ui::LayoutUS)
{
    ui->setupUi(this);

    this->setAttribute(Qt::WA_AcceptTouchEvents);

    connect(ui->shift, &QPushButton::toggled, [=](bool checked) {
        if (checked) {
            ui->qButton->setText("Q");
            ui->wButton->setText("W");
            ui->eButton->setText("E");
            ui->rButton->setText("R");
            ui->tButton->setText("T");
            ui->yButton->setText("Y");
            ui->uButton->setText("U");
            ui->iButton->setText("I");
            ui->oButton->setText("O");
            ui->pButton->setText("P");

            ui->aButton->setText("A");
            ui->sButton->setText("S");
            ui->dButton->setText("D");
            ui->fButton->setText("F");
            ui->gButton->setText("G");
            ui->hButton->setText("H");
            ui->jButton->setText("J");
            ui->kButton->setText("K");
            ui->lButton->setText("L");

            ui->zButton->setText("Z");
            ui->xButton->setText("X");
            ui->cButton->setText("C");
            ui->vButton->setText("V");
            ui->bButton->setText("B");
            ui->nButton->setText("N");
            ui->mButton->setText("M");
        } else {
            ui->qButton->setText("q");
            ui->wButton->setText("w");
            ui->eButton->setText("e");
            ui->rButton->setText("r");
            ui->tButton->setText("t");
            ui->yButton->setText("y");
            ui->uButton->setText("u");
            ui->iButton->setText("i");
            ui->oButton->setText("o");
            ui->pButton->setText("p");

            ui->aButton->setText("a");
            ui->sButton->setText("s");
            ui->dButton->setText("d");
            ui->fButton->setText("f");
            ui->gButton->setText("g");
            ui->hButton->setText("h");
            ui->jButton->setText("j");
            ui->kButton->setText("k");
            ui->lButton->setText("l");

            ui->zButton->setText("z");
            ui->xButton->setText("x");
            ui->cButton->setText("c");
            ui->vButton->setText("v");
            ui->bButton->setText("b");
            ui->nButton->setText("n");
            ui->mButton->setText("m");
        }

        updateSupplementaryCharacters();

        QSoundEffect* keySound = new QSoundEffect();
        keySound->setSource(QUrl("qrc:/sounds/keyclickErase.wav"));
        keySound->play();
        connect(keySound, SIGNAL(playingChanged()), keySound, SLOT(deleteLater()));
    });

    if (!state->split()) {
        ui->splitWidget1->setVisible(false);
        ui->splitWidget2->setVisible(false);
        ui->splitWidget3->setVisible(false);
    }

    connect(state, &KeyboardState::shiftChanged, [=] {
        ui->shift->setChecked(state->capsLock() || state->shift());
    });
    connect(state, &KeyboardState::capsLockChanged, [=] {
        ui->shift->setChecked(state->capsLock() || state->shift());
    });
    connect(state, &KeyboardState::splitChanged, [=](bool split) {
        if (split) {
            ui->splitWidget1->setVisible(true);
            ui->splitWidget2->setVisible(true);
            ui->splitWidget3->setVisible(true);
        } else {
            ui->splitWidget1->setVisible(false);
            ui->splitWidget2->setVisible(false);
            ui->splitWidget3->setVisible(false);
        }
    });

    buttonIterate(this);
    ui->returnButton->setFixedWidth(ui->returnButton->sizeHint().width() + 40 * getDPIScaling());
    ui->leftSpacer->changeSize(40 * getDPIScaling(), 0, QSizePolicy::Fixed);

    QBoxLayout* layout = new QBoxLayout(QBoxLayout::LeftToRight);
    layout->addSpacing(50 * getDPIScaling());
    //layout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding));

    extraKeyLabel = new QLabel();
    extraKeyLabel->setMargin(9 * getDPIScaling());
    layout->addWidget(extraKeyLabel);

    extraKey = new QWidget();
    extraKey->setParent(this);
    extraKey->raise();
    extraKey->setAutoFillBackground(true);
    extraKey->setLayout(layout);
    extraKey->setVisible(false);

    QPalette pal = this->palette();
    pal.setColor(QPalette::Window, pal.color(QPalette::Highlight));
    pal.setColor(QPalette::WindowText, pal.color(QPalette::HighlightedText));
    extraKey->setPalette(pal);
}

LayoutVI::~LayoutVI()
{
    delete ui;
}

Layout::Layouts LayoutVI::layoutType() {
    return viVN;
}

void LayoutVI::updateSupplementaryCharacters() {
    if (ui->shift->isChecked()) {
        ui->aButton->setSupplementaryCharacters({"Á", "À", "Ả", "Ã", "Ạ", "Â", "Ấ", "Ầ", "Ẩ", "Ẫ", "Ậ", "Ă", "Ắ", "Ằ", "Ẳ", "Ẵ", "Ặ"});
        ui->eButton->setSupplementaryCharacters({"É", "È", "Ẻ", "Ẽ", "Ẹ", "Ê", "Ế", "Ề", "Ể", "Ễ", "Ệ"});
        ui->iButton->setSupplementaryCharacters({"Í", "Ì", "Ỉ", "Ĩ", "Ị"});
        ui->oButton->setSupplementaryCharacters({"Ó", "Ò", "Ỏ", "Õ", "Ọ", "Ô", "Ố", "Ồ", "Ổ", "Ỗ", "Ộ", "Ơ", "Ớ", "Ờ", "Ở", "Ỡ", "Ợ"});
        ui->uButton->setSupplementaryCharacters({"Ú", "Ù", "Ủ", "Ũ", "Ụ", "Ư", "Ứ", "Ừ", "Ử", "Ữ", "Ự"});
        ui->dButton->setSupplementaryCharacters({"Đ"});
    } else {
        ui->aButton->setSupplementaryCharacters({"á", "à", "ả", "ã", "ạ", "â", "ấ", "ầ", "ẩ", "ẫ", "ậ", "ă", "ắ", "ằ", "ẳ", "ẵ", "ặ"});
        ui->eButton->setSupplementaryCharacters({"é", "è", "ẻ", "ẽ", "ẹ", "ê", "ế", "ề", "ể", "ễ", "ệ"});
        ui->iButton->setSupplementaryCharacters({"í", "ì", "ỉ", "ĩ", "ị"});
        ui->oButton->setSupplementaryCharacters({"ó", "ò", "ỏ", "õ", "ọ", "ô", "ố", "ồ", "ổ", "ỗ", "ộ", "ơ", "ớ", "ờ", "ở", "ỡ", "ợ"});
        ui->uButton->setSupplementaryCharacters({"ú", "ù", "ủ", "ũ", "ụ", "ư", "ứ", "ừ", "ử", "ữ", "ự"});
        ui->dButton->setSupplementaryCharacters({"đ"});
    }
}

void LayoutVI::buttonIterate(QWidget* wid) {
    for (QObject* widget : wid->children()) {
        if (qobject_cast<KeyButton*>(widget) != nullptr) {
            connect(qobject_cast<KeyButton*>(widget), SIGNAL(tapped()), this, SLOT(pressKey()));
            connect(qobject_cast<KeyButton*>(widget), &KeyButton::typeSupplementary, this, &LayoutVI::typeString);
        } else if (qobject_cast<QPushButton*>(widget) != nullptr) {
            connect(qobject_cast<QPushButton*>(widget), SIGNAL(clicked(bool)), this, SLOT(pressKey()));
        } else if (qobject_cast<QWidget*>(widget) != nullptr) {
            buttonIterate(qobject_cast<QWidget*>(widget));
        }
    }
}

void LayoutVI::pressKey() {
    QPushButton* button = (QPushButton*) sender();
    if (button == ui->shift) {
        if (state->capsLock()) {
            state->setCapsLock(false);
        } else if (capsLockPressedTime.msecsTo(QDateTime::currentDateTime()) < QApplication::doubleClickInterval()) {
            state->setCapsLock(true);
        } else if (state->shift()) {
            state->setShift(false);
        } else {
            capsLockPressedTime = QDateTime::currentDateTime();
            state->setShift(true);
        }
        return;
    }

    Window focused;
    int revert_to;

    //XGetInputFocus(QX11Info::display(), &focused, &revert_to);

    KeySym pressedKey = 0;
    KeyCode pressedKeyCode = 0;
    bool useKeySym = true;

    QString text = button->text();
    bool isShift = false;


    if (text == "!" || text == "@" || text == "#" || text == "$" || text == "%" || text == "^" || text == "&"
            || text == "*" || text == "(" || text == ")" || text == "{" || text == "}" || text == "|"
            || text == ":" || text == "\"" || text == "<" || text == ">" || text == "?" || text == "~") {
        isShift = true;
    }

    if (button == ui->backspace) {
        pressedKey = XK_BackSpace;
    } else if (button == ui->returnButton) {
        pressedKey = XK_Return;
    } else {
        this->typeString(button->text());
    }

    emit typeKey(pressedKey);
}

void LayoutVI::resizeEvent(QResizeEvent* event) {

}

bool LayoutVI::event(QEvent* event) {
    if (event->type() == QEvent::TouchBegin) {
        QTouchEvent* e = (QTouchEvent*) event;
        QPoint touchPoint = e->touchPoints().first().pos().toPoint();
        if (touchPoint.x() < 5 && ui->middleLayout->geometry().contains(touchPoint)) {
            //Tab key
            extraKeyLabel->setText("⇄ TAB");
            extraKeyTop = ui->middleLayout->geometry().top();
            extraKey->move(extraKeyTop, -extraKey->sizeHint().width());
            extraKey->setFixedWidth(extraKey->sizeHint().width());
            extraKey->setFixedHeight(ui->middleLayout->geometry().height());
            extraKey->setVisible(true);
            e->accept();
            return true;
        } else if (touchPoint.x() < 5 && ui->topLayout->geometry().contains(touchPoint)) {
            //Escape key
            extraKeyLabel->setText("⎋ ESC");
            extraKeyTop = ui->topLayout->geometry().top();
            extraKey->move(extraKeyTop, -extraKey->sizeHint().width());
            extraKey->setFixedWidth(extraKey->sizeHint().width());
            extraKey->setFixedHeight(ui->topLayout->geometry().height());
            extraKey->setVisible(true);
            e->accept();
            return true;
        }
    } else if (event->type() == QEvent::TouchUpdate) {
        QTouchEvent* e = (QTouchEvent*) event;
        QPoint touchPoint = e->touchPoints().first().pos().toPoint();
        int left = -extraKey->width() + touchPoint.x();
        if (left > 0) left = 0;
        extraKey->move(left, extraKeyTop);
        return true;
    } else if (event->type() == QEvent::TouchEnd) {
        if (extraKey->geometry().left() == 0) {
            if (extraKeyTop == ui->middleLayout->geometry().top()) {
                //Press tab key
                emit typeKey(XK_Tab);
            } else if (extraKeyTop == ui->topLayout->geometry().top()) {
                //Press escape key
                emit typeKey(XK_Escape);
            }
        }

        tVariantAnimation* anim = new tVariantAnimation();
        anim->setStartValue(extraKey->geometry().left());
        anim->setEndValue(-extraKey->geometry().width());
        anim->setDuration(500);
        anim->setEasingCurve(QEasingCurve::OutCubic);
        connect(anim, &tVariantAnimation::valueChanged, [=](QVariant value) {
            extraKey->move(value.toInt(), extraKeyTop);
        });
        connect(anim, SIGNAL(finished()), anim, SLOT(deleteLater()));
        anim->start();
        return true;
    }
    return false;
}
