set(TARGET tst_mousefilters)

file(GLOB QML_SOURCE "*.qml")

add_executable(
    ${TARGET}
    "tst_mousefilterstest.cpp"
)

include(../../plugin_dependency.cmake)
include(../../qtprivate_dependency.cmake)
include(../../layout_dependency.cmake)
include(../test-setup.cmake)

set_property(TEST ${TARGET} PROPERTY WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} )
