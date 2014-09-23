set(TARGET tst_performance)

file(GLOB QML_SOURCE "*.qml")

add_executable(
    ${TARGET}
    "tst_performance.cpp"
)

include(../../plugin_dependency.cmake)
include(../test-setup.cmake)

