CXX_MODULE = qml
TARGET = UbuntuComponentsStyles
TARGETPATH = Ubuntu/Components/Styles
IMPORT_VERSION = 1.1

include(plugin/plugin.pri)

QML_FILES += \
    $$PWD/1.2/ComboButtonStyle.qml \
    $$PWD/1.2/PageHeadStyle.qml \
    $$PWD/1.2/PullToRefreshStyle.qml \
    $$PWD/1.3/ActionBarStyle.qml \
    $$PWD/1.3/ActionItemProperties.qml \
    $$PWD/1.3/PageHeadStyle.qml \
    $$PWD/1.3/PageHeaderStyle.qml \
    $$PWD/1.3/SectionsStyle.qml \
    $$PWD/1.3/ToolbarStyle.qml

OTHER_FILES += \
    $$PWD/qmldir

load(ubuntu_qml_plugin)
