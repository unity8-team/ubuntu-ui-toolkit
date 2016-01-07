TEMPLATE=lib
TARGET = UbuntuTouchTheme

PLUGIN_TYPE = platformthemes
PLUGIN_CLASS_NAME = UCPlatformThemePlugin
load(qt_plugin)

#load(qt_build_config)
#load(ubuntu_qt_module)

QT += core-private gui-private platformsupport-private qml quick
QMAKE_CXXFLAGS += -fvisibility=hidden -fvisibility-inlines-hidden -std=c++11 -Werror -Wall
QMAKE_LFLAGS += -std=c++11 -Wl,-no-undefined

CONFIG += c++11 plugin link_pkgconfig no_keywords #qpa/genericunixfontdatabase
#DESTDIR = ./
INCLUDEPATH+=$$PWD

target.path = $$[QT_INSTALL_PLUGINS]/platforms

INSTALLS += target

RESOURCES += \
    UbuntuTouchTheme.qrc

HEADERS += \
    ucplatformthemeplugin.h \
    ucplatformtheme.h \
    ucplatformmenu.h

SOURCES += \
    ucplatformthemeplugin.cpp \
    ucplatformtheme.cpp \
    ucplatformmenuitem.cpp \
    ucplatformmenu.cpp

OTHER_FILES += \
    ubuntutouchtheme.json
