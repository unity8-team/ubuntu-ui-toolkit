set(TARGET tst_inversemousearea)

file(GLOB QML_SOURCE "*.qml")

add_executable(
    ${TARGET}
    "tst_inversemouseareatest.cpp"
)

list(APPEND TEST_INCLUDE_DIRS
    ${Qt5Quick_PRIVATE_INCLUDE_DIRS}
    ${Qt5Gui_PRIVATE_INCLUDE_DIRS}
)

include(../../plugin_dependency.cmake)
include(../../layout_dependency.cmake)
include(../test-setup.cmake)

set_property(TEST ${TARGET} PROPERTY WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} )
