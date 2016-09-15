QT = core-private gui-private quick-private qml-private
equals(QT_MAJOR_VERSION, 5):lessThan(QT_MINOR_VERSION, 2) {
    QT *= v8-private
}

SOURCES += \
    $$PWD/propertychanges.cpp \
    $$PWD/ulconditionallayout.cpp \
    $$PWD/ulitemlayout.cpp \
    $$PWD/ullayouts.cpp \
    $$PWD/ullayoutsattached.cpp \
    $$PWD/ulplugin.cpp

HEADERS += \
    $$PWD/propertychanges_p.h \
    $$PWD/ulconditionallayout.h \
    $$PWD/ulconditionallayout_p.h \
    $$PWD/ulitemlayout.h \
    $$PWD/ullayouts.h \
    $$PWD/ullayouts_p.h \
    $$PWD/ulplugin.h
