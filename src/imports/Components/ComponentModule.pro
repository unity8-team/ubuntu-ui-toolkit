CXX_MODULE = qml
TARGET = UbuntuComponents
TARGETPATH = Ubuntu/Components
IMPORT_VERSION = 0.1

include(plugin/plugin.pri)

ARTWORK_FILES = $$system(find artwork -type f)

QML_FILES += $$ARTWORK_FILES

# 1.0
QML_FILES += \
    $$PWD/1.0/Button.qml \
    $$PWD/1.0/Icon.qml \
    $$PWD/1.0/ProgressBar.qml \
    $$PWD/1.0/UbuntuColors.qml

# 1.1
QML_FILES += \
    $$PWD/1.1/Button.qml \
    $$PWD/1.1/Haptics.qml \
    $$PWD/1.1/Icon.qml \
    $$PWD/1.1/ProgressBar.qml \
    $$PWD/1.1/UbuntuColors.qml

# 1.2
QML_FILES += \
    $$PWD/1.2/AbstractButton.qml \
    $$PWD/1.2/ActionList.qml \
    $$PWD/1.2/ActivityIndicator.qml \
    $$PWD/1.2/AnimatedItem.qml \
    $$PWD/1.2/AppHeader.qml \
    $$PWD/1.2/Captions.qml \
    $$PWD/1.2/CheckBox.qml \
    $$PWD/1.2/ComboButton.qml \
    $$PWD/1.2/CrossFadeImage10.qml \
    $$PWD/1.2/CrossFadeImage11.qml \
    $$PWD/1.2/DraggingArea.qml \
    $$PWD/1.2/Header.qml \
    $$PWD/1.2/InputHandler.qml \
    $$PWD/1.2/Label.qml \
    $$PWD/1.2/MainView.qml \
    $$PWD/1.2/MainView12.qml \
    $$PWD/1.2/MainViewBase.qml \
    $$PWD/1.2/Object.qml \
    $$PWD/1.2/OptionSelector.qml \
    $$PWD/1.2/OptionSelectorDelegate.qml \
    $$PWD/1.2/OrientationHelper.qml \
    $$PWD/1.2/Page10.qml \
    $$PWD/1.2/Page11.qml \
    $$PWD/1.2/PageHeadConfiguration.qml \
    $$PWD/1.2/PageHeadSections.qml \
    $$PWD/1.2/PageHeadState.qml \
    $$PWD/1.2/PageStack.qml \
    $$PWD/1.2/PageTreeNode.qml \
    $$PWD/1.2/PageWrapper.qml \
    $$PWD/1.2/PageWrapperUtils.js \
    $$PWD/1.2/Panel.qml \
    $$PWD/1.2/PullToRefresh.qml \
    $$PWD/1.2/Scrollbar.qml \
    $$PWD/1.2/Slider.qml \
    $$PWD/1.2/Switch.qml \
    $$PWD/1.2/Tab.qml \
    $$PWD/1.2/TabBar.qml \
    $$PWD/1.2/Tabs.qml \
    $$PWD/1.2/TextArea.qml \
    $$PWD/1.2/TextCursor.qml \
    $$PWD/1.2/TextField.qml \
    $$PWD/1.2/TextInputPopover.qml \
    $$PWD/1.2/Toolbar.qml \
    $$PWD/1.2/ToolbarButton.qml \
    $$PWD/1.2/ToolbarItems.qml \
    $$PWD/1.2/UbuntuListView.qml \
    $$PWD/1.2/UbuntuListView11.qml \
    $$PWD/1.2/UbuntuNumberAnimation.qml \
    $$PWD/1.2/dateUtils.js \
    $$PWD/1.2/sliderUtils.js \
    $$PWD/1.2/stack.js

# 1.3
QML_FILES += \
    $$PWD/1.3/ActionBar.qml \
    $$PWD/1.3/ActivityIndicator.qml \
    $$PWD/1.3/AdaptivePageLayout.qml \
    $$PWD/1.3/AnimatedItem.qml \
    $$PWD/1.3/AppHeader.qml \
    $$PWD/1.3/Button.qml \
    $$PWD/1.3/Captions.qml \
    $$PWD/1.3/CheckBox.qml \
    $$PWD/1.3/ComboButton.qml \
    $$PWD/1.3/CrossFadeImage.qml \
    $$PWD/1.3/DraggingArea.qml \
    $$PWD/1.3/Icon.qml \
    $$PWD/1.3/InputHandler.qml \
    $$PWD/1.3/ListItemPopover.qml \
    $$PWD/1.3/MainView.qml \
    $$PWD/1.3/OptionSelector.qml \
    $$PWD/1.3/OptionSelectorDelegate.qml \
    $$PWD/1.3/OrientationHelper.qml \
    $$PWD/1.3/Page.qml \
    $$PWD/1.3/PageColumn.qml \
    $$PWD/1.3/PageColumnsLayout.qml \
    $$PWD/1.3/PageHeadConfiguration.qml \
    $$PWD/1.3/PageHeadSections.qml \
    $$PWD/1.3/PageHeadState.qml \
    $$PWD/1.3/PageHeader.qml \
    $$PWD/1.3/PageStack.qml \
    $$PWD/1.3/Panel.qml \
    $$PWD/1.3/ProgressBar.qml \
    $$PWD/1.3/ProgressionSlot.qml \
    $$PWD/1.3/PullToRefresh.qml \
    $$PWD/1.3/ScrollView.qml \
    $$PWD/1.3/Scrollbar.qml \
    $$PWD/1.3/Sections.qml \
    $$PWD/1.3/Slider.qml \
    $$PWD/1.3/Switch.qml \
    $$PWD/1.3/Tab.qml \
    $$PWD/1.3/TabBar.qml \
    $$PWD/1.3/Tabs.qml \
    $$PWD/1.3/TextArea.qml \
    $$PWD/1.3/TextCursor.qml \
    $$PWD/1.3/TextField.qml \
    $$PWD/1.3/TextInputPopover.qml \
    $$PWD/1.3/Toolbar.qml \
    $$PWD/1.3/ToolbarButton.qml \
    $$PWD/1.3/ToolbarItems.qml \
    $$PWD/1.3/UbuntuColors.qml \
    $$PWD/1.3/UbuntuListView.qml \
    $$PWD/1.3/UbuntuNumberAnimation.qml \
    $$PWD/1.3/dateUtils.js \
    $$PWD/1.3/pageUtils.js

OTHER_FILES+= \
    $$PWD/1.3/CrossFadeImage.qdoc \
    $$PWD/1.3/Icon.qdoc \
    $$PWD/1.3/MainView.qdoc \
    $$PWD/1.3/PageHeadConfiguration.qdoc \
    $$PWD/qmldir

load(ubuntu_qml_plugin)
