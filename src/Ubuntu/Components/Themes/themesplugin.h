#ifndef THEMESPLUGIN_H
#define THEMESPLUGIN_H

#include <QQmlExtensionPlugin>

class ThemesPlugin: public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface")

public:
    void registerTypes(const char *uri);
};

#endif // THEMESPLUGIN_H
