TARGETPATH = Ubuntu/Components/Themes/SuruDark

CXX_MODULE = qml
TARGET  = UbuntuSuruDarkTheme
IMPORT_VERSION = 0.1

QT *= qml

PARENT_THEME_FILE = parent_theme
QML_FILES += $$PARENT_THEME_FILE

HEADERS += \
    surudarkplugin.h

SOURCES += \
    surudarkplugin.cpp

RESOURCES += \
    surudark.qrc

OTHER_FILES += qmldir

load(ubuntu_qml_plugin)

