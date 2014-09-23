set(TARGET tst_theme_engine)

file(GLOB QML_SOURCE "*.qml")

add_executable(
    ${TARGET}
    "tst_theme_enginetest.cpp"
)

include(../../plugin_dependency.cmake)
include(../../layout_dependency.cmake)
include(../test-setup.cmake)

set_property(TEST ${TARGET} PROPERTY WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} )
