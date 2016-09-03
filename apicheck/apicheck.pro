TEMPLATE = app
QT += qml quick qml-private
# For setSharedOpenGLContext
QT += core-private gui-private testlib quick-private
CONFIG += no_keywords
SOURCES += apicheck.cpp
target.path = $$[QT_INSTALL_LIBS]/ubuntu-ui-toolkit 
INSTALLS += target
