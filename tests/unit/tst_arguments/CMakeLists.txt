set(TARGET tst_arguments)

add_executable(
    ${TARGET}
    "tst_arguments.cpp"
)

include(../../plugin_dependency.cmake)
include(../test-setup.cmake)
