include(../test-include.pri)
QT += core-private qml-private quick-private gui-private
SOURCES += tst_subtheming.cpp

OTHER_FILES += \
    TestStyle.qml \
    SimpleItem.qml \
    themes/CustomTheme/TestStyle.qml \
    themes/CustomTheme/Palette.qml \
    themes/CustomTheme/parent_theme \
    themes/TestModule/TestTheme/TestStyle.qml \
    themes/TestModule/TestTheme/qmldir \
    themes/TestModule/TestTheme/parent_theme \
    DynamicAssignment.qml \
    ParentChanges.qml \
    TestMain.qml \
    TestStyleChange.qml \
    DifferentThemes.qml \
    ChangePaletteValueWhenParentChanges.qml \
    ChangeDefaultPaletteInChildren.qml \
    ImplicitPaletteSettings.qml \
    ExplicitPaletteSettings.qml \
    InvalidPaletteSettings1.qml \
    InvalidPaletteSettings2.qml \
    InvalidPaletteSettings3.qml \
    InvalidPaletteSettings4.qml \
    InvalidPaletteSettings5.qml \
    InvalidPaletteSettings6.qml \
    MorePaletteSettingsInTheme.qml \
    SameNamedPaletteSettings.qml \
    WrongPaletteSettingsDeclaration.qml

