CXX_MODULE = qml
TARGET  = UbuntuComponents
TARGETPATH = Ubuntu/Components
IMPORT_VERSION = 0.1

include(plugin/plugin.pri)

QML_FILES += 1.1/Haptics.qml

OTHER_FILES+= 1.3/CrossFadeImage.qdoc \
             1.3/UbuntuListView11.qdoc \
             1.3/Page.qdoc \
             1.3/PageHeadConfiguration.qdoc \
             1.3/MainView.qdoc \
             1.3/Icon.qdoc \
             qmldir

RESOURCES += \
    components.qrc

load(ubuntu_qml_plugin)


