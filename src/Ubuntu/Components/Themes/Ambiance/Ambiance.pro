TARGETPATH = Ubuntu/Components/Themes/Ambiance

CXX_MODULE = qml
TARGET  = UbuntuAmbianceTheme
IMPORT_VERSION = 0.1

QT *= qml

HEADERS += \
    ambianceplugin.h

SOURCES += \
    ambianceplugin.cpp

OTHER_FILES += qmldir

RESOURCES += \
    ambiance.qrc

load(ubuntu_qml_plugin)
