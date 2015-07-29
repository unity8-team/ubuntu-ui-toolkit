CXX_MODULE = qml
TARGET  = UbuntuPickers
TARGETPATH = Ubuntu/Components/Pickers
IMPORT_VERSION = 0.1

QT *= qml

HEADERS += \
    pickersplugin.h

SOURCES += \
    pickersplugin.cpp

RESOURCES += \
    pickers.qrc

OTHER_FILES += qmldir


load(ubuntu_qml_plugin)
