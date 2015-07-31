#ifndef SURUDARKPLUGIN_H
#define SURUDARKPLUGIN_H

#include <QQmlExtensionPlugin>

class SuruDarkPlugin: public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface")

public:
    void registerTypes(const char *uri);
};

#endif // SURUDARKPLUGIN_H
