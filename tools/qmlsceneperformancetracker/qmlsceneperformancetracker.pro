TEMPLATE = app
TARGET = qmlscene-performance-tracker
QT += qml core-private gui-private quick-private
qtHaveModule(widgets): QT += widgets
CONFIG += c++11

SOURCES += qmlsceneperformancetracker.cpp
DEFINES += QML_RUNTIME_TESTING QT_QML_DEBUG_NO_WARNING

INCLUDEPATH = $${OUT_PWD}/../../include
LIBS += -L$${OUT_PWD}/../../lib -lquickplus

load(qt_tool)
