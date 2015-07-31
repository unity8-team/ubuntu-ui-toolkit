TARGETPATH = Ubuntu/Components/Themes

CXX_MODULE = qml
TARGET  = UbuntuThemesModule
IMPORT_VERSION = 0.1

QT *= qml

HEADERS += \
    themesplugin.h

SOURCES += \
    themesplugin.cpp

RESOURCES += \
    themesmodule.qrc

OTHER_FILES += qmldir

load(ubuntu_qml_plugin)
