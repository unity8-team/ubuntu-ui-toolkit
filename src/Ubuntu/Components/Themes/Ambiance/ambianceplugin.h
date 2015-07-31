#ifndef AMBIANCEPLUGIN_H
#define AMBIANCEPLUGIN_H

#include <QQmlExtensionPlugin>

class AmbiancePlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface")

public:
    void registerTypes(const char *uri);
};

#endif // AMBIANCEPLUGIN_H
