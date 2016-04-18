CXX_MODULE = qml
TARGET = quickplusplugin
TARGETPATH = QuickPlus

INCLUDEPATH = ../lib
LIBS += -L../../lib -lquickplus

SOURCES += \
    plugin.cpp

QT += quick-private qml-private

DESTDIR_AUX = ../../qml/$${TARGETPATH}
QMAKE_POST_LINK += cp qmldir plugins.qmltypes $${DESTDIR_AUX}

load(qml_plugin)
