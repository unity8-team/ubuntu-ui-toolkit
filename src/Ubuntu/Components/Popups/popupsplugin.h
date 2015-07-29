#ifndef POPUPSPLUGIN_H
#define POPUPSPLUGIN_H

#include <QQmlExtensionPlugin>

class PopupsPlugin: public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface")

public:
    void registerTypes(const char *uri);
};

#endif // POPUPSPLUGIN_H
