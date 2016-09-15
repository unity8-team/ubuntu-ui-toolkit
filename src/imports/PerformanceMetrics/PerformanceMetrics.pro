CXX_MODULE = qml
TARGET = UbuntuPerformanceMetrics
TARGETPATH = Ubuntu/PerformanceMetrics
IMPORT_VERSION = 0.1

include(plugin/plugin.pri)

QML_FILES += \
    $$PWD/BarGraph.qml \
    $$PWD/PerformanceOverlay.qml

load(ubuntu_qml_plugin)
