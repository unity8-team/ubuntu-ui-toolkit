set(TARGET tst_mainview)

add_executable(
    ${TARGET}
    "tst_mainview.cpp"
)

include(../../plugin_dependency.cmake)
include(../test-setup.cmake)

set_property(TEST ${TARGET} PROPERTY WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} )
