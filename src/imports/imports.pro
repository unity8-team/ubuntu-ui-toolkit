CXX_MODULE = qml
TARGET = quickplusplugin
TARGETPATH = QuickPlus

INCLUDEPATH = ../quickplus
LIBS += -L../../lib -lquickplus

SOURCES += \
    plugin.cpp

QT += quick-private qml-private

QMAKE_POST_LINK += cp $${PWD}/qmldir $${PWD}/plugins.qmltypes $${OUT_PWD}/../../qml/$${TARGETPATH}

load(qml_plugin)
