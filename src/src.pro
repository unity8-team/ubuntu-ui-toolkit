CONFIG += no_keywords
CXX_MODULE = qml
TARGET = quickplusplugin
TARGETPATH = QuickPlus

HEADERS += \
    $$PWD/dropshadow.h \
    $$PWD/dropshadowtexture.h
SOURCES += \
    $$PWD/plugin.cpp \
    $$PWD/dropshadow.cpp
RESOURCES = \
    $$PWD/src.qrc
OTHER_FILES += \
    $$PWD/shaders/dropshadow.vert \
    $$PWD/shaders/dropshadow.frag \
    $$PWD/shaders/dropshadow_core.vert \
    $$PWD/shaders/dropshadow_core.frag

QT += core-private gui-private quick-private qml-private

load(qml_plugin)
