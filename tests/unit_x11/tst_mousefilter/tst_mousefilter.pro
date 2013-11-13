include(../test-include.pri)
QT += quick-private
SOURCES += tst_mousefiltertest.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"

OTHER_FILES += \
    Defaults.qml \
    Filter.qml
