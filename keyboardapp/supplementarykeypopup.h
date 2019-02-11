#ifndef SUPPLEMENTARYKEYPOPUP_H
#define SUPPLEMENTARYKEYPOPUP_H

#include <QDialog>

namespace Ui {
    class SupplementaryKeyPopup;
}

struct SupplementaryKeyPopupPrivate;
class SupplementaryKeyPopup : public QDialog
{
        Q_OBJECT

    public:
        explicit SupplementaryKeyPopup(QWidget *parent = nullptr);
        ~SupplementaryKeyPopup();

        void setButtonSize(QSize size);
        void setButtons(QStringList buttons);
        void setPrimaryKeyTopLeft(QPoint topLeft);
        QString buttonForScreenPoint(QPoint screenPoint);

    public slots:
        void show();

    private:
        Ui::SupplementaryKeyPopup *ui;
        SupplementaryKeyPopupPrivate* d;
};

#endif // SUPPLEMENTARYKEYPOPUP_H
