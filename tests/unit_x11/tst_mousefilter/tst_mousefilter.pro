include(../test-include.pri)
QT += quick-private
SOURCES += tst_mousefiltertest.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"

OTHER_FILES += \
    Defaults.qml \
    Filter.qml \
    Capture.qml \
    CaptureDoubleClick.qml \
    CaptureClicked.qml \
    PressOverOSK.qml \
    AcceptedButtonsChanged.qml \
    PressAndHoldDelayChanged.qml \
    FilterBiggerDelay.qml \
    CaptureMouseMove.qml
