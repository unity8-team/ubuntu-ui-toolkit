#ifndef LISTITEMPLUGIN_H
#define LISTITEMPLUGIN_H

#include <QQmlExtensionPlugin>

class ListItemPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface")

public:
    void registerTypes(const char *uri);
};

#endif // LISTITEMPLUGIN_H
