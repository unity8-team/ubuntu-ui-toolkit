set(TARGET tst_units)

qt5_add_resources(UI_RESOURCES tst_units.qrc)

add_executable(
    ${TARGET}
    "tst_units.cpp"
    ${UI_RESOURCES}
)

include(../../plugin_dependency.cmake)
include(../test-setup.cmake)

set_property(TEST ${TARGET} PROPERTY WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
