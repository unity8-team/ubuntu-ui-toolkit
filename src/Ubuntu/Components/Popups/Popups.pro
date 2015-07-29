CXX_MODULE = qml
TARGET  = UbuntuPopups
TARGETPATH = Ubuntu/Components/Popups
IMPORT_VERSION = 0.1

QT *= qml

HEADERS += \
    popupsplugin.h

SOURCES += \
    popupsplugin.cpp

RESOURCES += \
    popups.qrc

OTHER_FILES += qmldir

load(ubuntu_qml_plugin)
