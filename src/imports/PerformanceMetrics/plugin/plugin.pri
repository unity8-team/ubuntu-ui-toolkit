QT = qml quick

SOURCES += \
    $$PWD/rendertimer.cpp \
    $$PWD/upmcpuusage.cpp \
    $$PWD/upmgraphmodel.cpp \
    $$PWD/upmplugin.cpp \
    $$PWD/upmrenderingtimes.cpp \
    $$PWD/upmtexturefromimage.cpp

HEADERS += \
    $$PWD/rendertimer.h \
    $$PWD/upmcpuusage.h \
    $$PWD/upmgraphmodel.h \
    $$PWD/upmplugin.h \
    $$PWD/upmrenderingtimes.h \
    $$PWD/upmtexturefromimage.h

contains(QT_CONFIG, opengles2) {
    CONFIG += egl
}
