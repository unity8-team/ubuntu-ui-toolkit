CXX_MODULE = qml
TARGET = UbuntuLayouts
TARGETPATH = Ubuntu/Layouts
IMPORT_VERSION = 0.1

include(plugin/plugin.pri)

OTHER_FILES += \
    $$PWD/qmldir

load(ubuntu_qml_plugin)
