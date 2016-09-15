CXX_MODULE = qml
TARGET = UbuntuMetrics
TARGETPATH = Ubuntu/Metrics
IMPORT_VERSION = 1.0

include(plugin/plugin.pri)

OTHER_FILES += \
    $$PWD/qmldir

load(ubuntu_qml_plugin)
