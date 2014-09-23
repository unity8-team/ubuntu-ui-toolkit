set(TARGET tst_components_benchmark)
file(GLOB TESTS tst_*.qml)

add_executable(
    ${TARGET}
    "tst_components_benchmark.cpp"
)

include(../../plugin_dependency.cmake)
include(../test-setup.cmake)
