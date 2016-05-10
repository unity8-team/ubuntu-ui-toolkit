TEMPLATE = lib
TARGET = quickplus
QT += core-private gui-private quick-private
CONFIG += c++11 no_keywords
DEFINES += QUICK_PLUS_BUILD
equals(DISABLE_LTTNG, "1"): DEFINES += DISABLE_LTTNG
QMAKE_CXXFLAGS_RELEASE += -Wno-unused-result

include(../../version.pri)
VERSION = $${QUICK_PLUS_VERSION}

PUBLIC_HEADERS = \
    $$PWD/quickplusglobal.h \
    $$PWD/metrics.h \
    $$PWD/metricslogger.h \
    $$PWD/performancemetrics.h

HEADERS += \
    $${PUBLIC_HEADERS} \
    $$PWD/quickplusglobal_p.h \
    $$PWD/quickplus_p.h \
    $$PWD/dropshadow_p.h \
    $$PWD/dropshadowtexture_p.h \
    $$PWD/bitmaptextfont_p.h \
    $$PWD/metricslogger_p.h \
    $$PWD/performancemetrics_p.h

SOURCES += \
    $$PWD/quickplus.cpp \
    $$PWD/dropshadow.cpp \
    $$PWD/metricslogger.cpp \
    $$PWD/performancemetrics.cpp

OTHER_FILES += \
    $$PWD/shaders/dropshadow.vert \
    $$PWD/shaders/dropshadow.frag \
    $$PWD/shaders/dropshadow_core.vert \
    $$PWD/shaders/dropshadow_core.frag

RESOURCES = \
    $$PWD/plus.qrc

headers.files = $${PUBLIC_HEADERS}
headers.path = /usr/include/quickplus
target.path = /usr/lib
INSTALLS += headers target

DESTDIR = ../../lib

DESTDIR_HEADERS = ../../include/quickplus
QMAKE_POST_LINK += mkdir -p $${DESTDIR_HEADERS} && cp $${PUBLIC_HEADERS} $${DESTDIR_HEADERS}

# Another way to do it with extra targets (not sure what is the best way).
# destdir_headers.commands = mkdir -p $${DESTDIR_HEADERS} && cp $${PUBLIC_HEADERS} $${DESTDIR_HEADERS}
# QMAKE_EXTRA_TARGETS += destdir_headers
# POST_TARGETDEPS += destdir_headers

CONFIG += create_pc
QMAKE_PKGCONFIG_FILE = quick+
QMAKE_PKGCONFIG_NAME = Quick+
QMAKE_PKGCONFIG_DESCRIPTION = Additional scenegraph nodes and tools for QtQuick
QMAKE_PKGCONFIG_PREFIX = $${INSTALLBASE}
QMAKE_PKGCONFIG_LIBDIR = $${target.path}
QMAKE_PKGCONFIG_DESTDIR = pkgconfig
QMAKE_PKGCONFIG_VERSION = $${QUICK_PLUS_VERSION}
QMAKE_PKGCONFIG_REQUIRES = Qt5Core Qt5Gui Qt5Quick
