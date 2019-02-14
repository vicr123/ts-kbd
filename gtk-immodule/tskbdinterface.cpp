#include <tvirtualkeyboard.h>
#include <QDebug>

extern "C" {
    #include <gtk/gtk.h>

    gboolean isKeyboardRunning(void) {
        return tVirtualKeyboard::instance()->isKeyboardRunning();
    }

    void showKeyboard(void) {
        tVirtualKeyboard::instance()->showKeyboard();
    }

    void hideKeyboard(void) {
        tVirtualKeyboard::instance()->hideKeyboard();
    }

    void setKeyboardType(GString* type) {
        QString qType = QString::fromLocal8Bit(type->str, type->len);
        tVirtualKeyboard::instance()->setKeyboardType(qType);
        g_string_free(type, TRUE);
    }

    void setPredictive(gboolean predictive) {
        tVirtualKeyboard::instance()->setPredictive(predictive);
    }
}
