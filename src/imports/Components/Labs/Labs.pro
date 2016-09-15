CXX_MODULE = qml
TARGET = UbuntuComponentsLabs
TARGETPATH = Ubuntu/Components/Labs
IMPORT_VERSION = 1.0

include(plugin/plugin.pri)

OTHER_FILES += \
    $$PWD/qmldir

load(ubuntu_qml_plugin)
