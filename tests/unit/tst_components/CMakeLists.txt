set(TARGET tst_components)
file(GLOB TESTS tst_*.qml)

add_executable(
    ${TARGET}
    "tst_components.cpp"
)

include(../../plugin_dependency.cmake)
include(../qmltest-setup.cmake)
