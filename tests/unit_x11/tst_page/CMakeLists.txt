set(TARGET tst_page)

file(GLOB QML_SOURCE "*.qml")

add_executable(
    ${TARGET}
    "tst_page.cpp"
)

list(APPEND TEST_QT_MODULES Gui)

include(../../plugin_dependency.cmake)
include(../../layout_dependency.cmake)
include(../test-setup.cmake)
