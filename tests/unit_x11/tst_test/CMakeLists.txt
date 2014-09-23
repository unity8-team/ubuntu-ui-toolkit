set(TARGET tst_test)

file(GLOB TESTS "*.qml")

add_executable(
    ${TARGET}
    "tst_test.cpp"
)

include(../../plugin_dependency.cmake)
include(../../layout_dependency.cmake)
include(../qmltest-setup.cmake)
