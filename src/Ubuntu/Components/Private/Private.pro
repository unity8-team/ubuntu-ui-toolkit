CXX_MODULE = qml
TARGET  = UbuntuComponentsPrivate
TARGETPATH = Ubuntu/Components/Private
IMPORT_VERSION = 1.3

include(plugin/plugin.pri)

QML_FILES += 1.3/Menu.qml \
             1.3/MenuItem.qml \
             1.3/MenuSeparator.qml \
             1.3/MenuNavigator.qml

OTHER_FILES+= qmldir

load(ubuntu_qml_plugin)
