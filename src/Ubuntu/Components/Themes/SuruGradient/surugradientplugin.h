#ifndef SURUGRADIENTPLUGIN_H
#define SURUGRADIENTPLUGIN_H

#include <QQmlExtensionPlugin>

class SuruGradientPlugin: public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface")

public:
    void registerTypes(const char *uri);
};

#endif // SURUGRADIENTPLUGIN_H
