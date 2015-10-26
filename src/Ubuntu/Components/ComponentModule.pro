CXX_MODULE = qml
TARGET  = UbuntuComponents
TARGETPATH = Ubuntu/Components
IMPORT_VERSION = 0.1

include(plugin/plugin.pri)

OTHER_FILES+= qmldir \
             1.3/CrossFadeImage.qdoc \
             1.3/UbuntuListView11.qdoc \
             1.3/PageHeadConfiguration.qdoc \
             1.3/MainView.qdoc \
             1.3/Icon.qdoc

RESOURCES += \
    components.qrc

load(ubuntu_qml_plugin)
