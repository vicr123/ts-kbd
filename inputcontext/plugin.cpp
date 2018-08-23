#include "plugin.h"
#include <QDebug>

Plugin::Plugin(QObject *parent) : QPlatformInputContextPlugin(parent)
{
}

QPlatformInputContext* Plugin::create(const QString& key, const QStringList& paramList) {
    qDebug() << key;
    if (key == "ts-kbd") {
        qDebug() << "Loading ts-kbd";
        return new InputContext();
    }
    return nullptr;
}
