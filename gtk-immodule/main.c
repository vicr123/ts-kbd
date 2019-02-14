#include <stdio.h>
#include <gtk/gtkimmodule.h>
#include <gio/gio.h>

extern gboolean isKeyboardRunning(void);
extern void showKeyboard(void);
extern void hideKeyboard(void);
extern void setKeyboardType(GString* type);
extern void setPredictive(gboolean predictive);

GType registeredType;
GtkIMContextClass* parent_class = NULL;
guint commitSignal;

static const GtkIMContextInfo imInfo = {
    "ts-kbd",
    "theShell Touch Screen Keyboard",
    "ts-kbd",
    "/usr/share/ts-kbd/gtk-immodule/translations",
    ""
};

static const GtkIMContextInfo *info_list[] = {
    &imInfo
};

typedef struct tKbdImContext {
    GtkIMContext parent;
    GtkIMContext* childCtx;

    //gboolean preedit;
} tKbdImContext;

typedef struct tKbdImContextClass {
    GtkIMContextClass parent;
} tKbdImContextClass;

G_MODULE_EXPORT void im_module_list(const GtkIMContextInfo ***contexts, guint *n_contexts) {
    *contexts = info_list;
    *n_contexts = G_N_ELEMENTS(info_list);
}

G_MODULE_EXPORT void im_module_exit (void) {

}

G_MODULE_EXPORT GtkIMContext* im_module_create (const gchar *context_id) {
    if (g_strcmp0(context_id, "ts-kbd") == 0) {
        tKbdImContext* context;
        context = g_object_new(registeredType, NULL);

        return (GtkIMContext*) context;
    }
    return NULL;
}

void tim_focusIn(GtkIMContext* context) {
    if (!isKeyboardRunning()) return; //Don't bother doing anything if ts-kbd isn't running

    tKbdImContext* ctx = (GtkIMContext*) context;
    GtkInputPurpose purpose;
    GtkInputHints hints;

    g_object_get(GTK_IM_CONTEXT(ctx), "input-purpose", &purpose, "input-hints", &hints, NULL);

    if (hints & GTK_INPUT_HINT_INHIBIT_OSK) return; //Don't do anything

    //Make DBus call to make keyboard appear
    showKeyboard();

    switch (purpose) {
        case GTK_INPUT_PURPOSE_NUMBER:
        case GTK_INPUT_PURPOSE_DIGITS:
        case GTK_INPUT_PURPOSE_PHONE:
            setKeyboardType(g_string_new("numeric"));
            break;
        case GTK_INPUT_PURPOSE_URL:
            setKeyboardType(g_string_new("url"));
            break;
        case GTK_INPUT_PURPOSE_PIN:
            setKeyboardType(g_string_new("numeric"));
            //fall through
        case GTK_INPUT_PURPOSE_PASSWORD:
            setPredictive(FALSE);
            break;
        default:
            break;
    }
}

void tim_focusOut(GtkIMContext* ctx) {
    if (!isKeyboardRunning()) return; //Don't bother doing anything if ts-kbd isn't running

    //Make DBus call to make keyboard disappear
    hideKeyboard();
}

gboolean tim_filterKeypresss(GtkIMContext *context, GdkEventKey *event) {
    //Pass filter keypress event to helper to process
    return gtk_im_context_filter_keypress(((tKbdImContext*) context)->childCtx, event);
}

void tim_emit_commit(GtkIMContext *ctx, gchar *string, tKbdImContext* mainCtx) {
    //Re-emit the commit signal
    g_signal_emit(mainCtx, commitSignal, 0, string);
}

G_MODULE_EXPORT void tim_init(GObject* o) {
    tKbdImContext* ctx = (tKbdImContext*) o;

    ctx->childCtx = gtk_im_context_simple_new();
    g_signal_connect(ctx->childCtx, "commit", G_CALLBACK(tim_emit_commit), ctx);
}

G_MODULE_EXPORT void tim_class_init(tKbdImContextClass* ctx) {
    parent_class = GTK_IM_CONTEXT_CLASS(ctx);
    parent_class->focus_in = tim_focusIn;
    parent_class->focus_out = tim_focusOut;
    parent_class->filter_keypress = tim_filterKeypresss;

    commitSignal = g_signal_lookup("commit", G_TYPE_FROM_CLASS(ctx));
}

G_MODULE_EXPORT void tim_finalize(tKbdImContextClass* ctx) {
    G_OBJECT_CLASS(ctx)->finalize(ctx);
}

G_MODULE_EXPORT void im_module_init (GTypeModule *module) {
    static const GTypeInfo contextInfo = {
        sizeof (tKbdImContextClass),
        (GBaseInitFunc) NULL,
        (GBaseFinalizeFunc) NULL,
        (GClassInitFunc) tim_class_init,
        (GClassFinalizeFunc) tim_finalize,
        NULL,
        sizeof (tKbdImContext),
        0,
        (GInstanceInitFunc) tim_init,
    };

    registeredType = g_type_module_register_type(module, GTK_TYPE_IM_CONTEXT, "tKbdImContext", &contextInfo, 0);
}
