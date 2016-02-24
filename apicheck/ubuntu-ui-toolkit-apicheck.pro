TEMPLATE = app
QT += qml quick qml-private
# For setSharedOpenGLContext
QT += core-private gui-private testlib quick-private
CONFIG += no_keywords
SOURCES += apicheck.cpp
ubuntu-ui-toolkit-apicheck.path = $$[QT_INSTALL_PREFIX]/bin
ubuntu-ui-toolkit-apicheck.files = ubuntu-ui-toolkit-apicheck
INSTALLS += $$TARGET
