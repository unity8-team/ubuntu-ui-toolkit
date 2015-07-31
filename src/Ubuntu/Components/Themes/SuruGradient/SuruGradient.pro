TARGETPATH = Ubuntu/Components/Themes/SuruGradient

CXX_MODULE = qml
TARGET  = UbuntuSuruGradientTheme
IMPORT_VERSION = 0.1

QT *= qml

PARENT_THEME_FILE = parent_theme
DEPRECATED_FILE = deprecated

QML_FILES += \
    $$PARENT_THEME_FILE \
    $$DEPRECATED_FILE

HEADERS += \
    surugradientplugin.h

SOURCES += \
    surugradientplugin.cpp

RESOURCES += \
    surugradient.qrc

OTHER_FILES += qmldir

load(ubuntu_qml_plugin)
