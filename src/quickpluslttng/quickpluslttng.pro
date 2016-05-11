TEMPLATE = lib
TARGET = quickplus-lttng
QT += core-private
CONFIG += c++11 no_keywords
DEFINES += QUICK_PLUS_LTTNG_BUILD
INCLUDEPATH += \
    $${OUT_PWD}/../../include \
    ../quickplus # For quickplusglobal_p.h.
    ..           # To be able to include <quickpluslttng/*.h> in public headers.
LIBS += -L$${OUT_PWD}/../../lib -lquickplus
unix {
    CONFIG += link_pkgconfig
    PKGCONFIG += lttng-ust
}

include(../../version.pri)
VERSION = $${QUICK_PLUS_VERSION}

PUBLIC_HEADERS = \
    $$PWD/quickpluslttngglobal.h \
    $$PWD/lttngmetricslogger.h

HEADERS += \
    $${PUBLIC_HEADERS} \
    $$PWD/lttngmetricslogger_p.h \
    $$PWD/tracepoint_p.h

SOURCES += \
    $$PWD/lttngmetricslogger.cpp \
       $$PWD/tracepoint.cpp

headers.files = $${PUBLIC_HEADERS}
headers.path = /usr/include/quickpluslttng
target.path = /usr/lib
INSTALLS += headers target

DESTDIR = ../../lib

DESTDIR_HEADERS = ../../include/quickpluslttng
QMAKE_POST_LINK += mkdir -p $${DESTDIR_HEADERS} && cp $${PUBLIC_HEADERS} $${DESTDIR_HEADERS}

# Another way to do it with extra targets (not sure what is the best way).
# destdir_headers.commands = mkdir -p $${DESTDIR_HEADERS} && cp $${PUBLIC_HEADERS} $${DESTDIR_HEADERS}
# QMAKE_EXTRA_TARGETS += destdir_headers
# POST_TARGETDEPS += destdir_headers

CONFIG += create_pc
QMAKE_PKGCONFIG_FILE = quick+-lttng
QMAKE_PKGCONFIG_NAME = Quick+ LTTng
QMAKE_PKGCONFIG_DESCRIPTION = LTTng utilities for Quick+
QMAKE_PKGCONFIG_PREFIX = $${INSTALLBASE}
QMAKE_PKGCONFIG_LIBDIR = $${target.path}
QMAKE_PKGCONFIG_DESTDIR = pkgconfig
QMAKE_PKGCONFIG_VERSION = $${QUICK_PLUS_VERSION}
QMAKE_PKGCONFIG_REQUIRES = Qt5Core quick+
