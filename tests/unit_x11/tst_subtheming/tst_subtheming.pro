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
    WrongPaletteChangesDeclaration.qml \
    MorePaletteChangesInTheme.qml \
    InvalidPaletteChanges1.qml \
    InvalidPaletteChanges2.qml \
    InvalidPaletteChanges3.qml \
    InvalidPaletteChanges4.qml \
    InvalidPaletteChanges5.qml \
    InvalidPaletteChanges6.qml \
    SameNamedPaletteChanges.qml \
    ChangePaletteValueWhenParentChanges.qml \
    ChangeDefaultPaletteInChildren.qml \
    ExplicitPaletteChanges.qml \
    ImplicitPaletteChanges.qml

