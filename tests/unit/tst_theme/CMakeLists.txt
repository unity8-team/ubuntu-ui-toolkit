set(TARGET tst_theme)

file(GLOB TESTS "*.qml")

add_executable(
    ${TARGET}
    "tst_theme.cpp"
)

include(../../plugin_dependency.cmake)
include(../qmltest-setup.cmake)
