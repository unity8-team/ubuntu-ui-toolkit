include(../test-include.pri)

unix {
    CONFIG += link_pkgconfig
    PKGCONFIG += glib-2.0 accountsservice
}

QT += gui
DEFINES += SRCDIR=\\\"$$PWD/\\\"

system(msgfmt po/en_US.po -o locale/en/LC_MESSAGES/localizedApp.mo)

SOURCES += \
    src\/tst_i18n.cpp

OTHER_FILES += \
    src\/LocalizedApp.qml
