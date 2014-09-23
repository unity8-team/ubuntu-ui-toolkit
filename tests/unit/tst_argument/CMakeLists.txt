set(TARGET tst_argument)

add_executable(
    ${TARGET}
    "tst_argument.cpp"
)

include(../../plugin_dependency.cmake)
include(../test-setup.cmake)
