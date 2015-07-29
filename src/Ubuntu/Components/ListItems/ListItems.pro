CXX_MODULE = qml
TARGET  = UbuntuListItems
TARGETPATH = Ubuntu/Components/ListItems
IMPORT_VERSION = 0.1

QT *= qml

HEADERS += \
    listitemplugin.h

SOURCES += \
    listitemplugin.cpp

RESOURCES += \
    listitems.qrc

OTHER_FILES += qmldir

load(ubuntu_qml_plugin)


