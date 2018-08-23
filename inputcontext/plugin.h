#ifndef PLUGIN_H
#define PLUGIN_H

#include <QGenericPlugin>
#include <qpa/qplatforminputcontextplugin_p.h>
#include "inputcontext.h"

class Plugin : public QPlatformInputContextPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QPlatformInputContextFactoryInterface_iid FILE "inputcontext.json")

public:
    Plugin(QObject *parent = nullptr);

    QPlatformInputContext* create(const QString& key, const QStringList& paramList);
};

#endif // PLUGIN_H
