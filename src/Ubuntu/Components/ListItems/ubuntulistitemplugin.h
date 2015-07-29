#ifndef UBUNTULISTITEMPLUGIN_H
#define UBUNTULISTITEMPLUGIN_H

#include <QQmlExtensionPlugin>

class UbuntuListItemPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface")

public:
    void registerTypes(const char *uri);
};

#endif // UBUNTULISTITEMPLUGIN_H
