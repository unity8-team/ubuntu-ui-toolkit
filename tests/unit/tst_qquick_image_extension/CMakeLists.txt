set(TARGET tst_qquick_image_extension)

add_executable(
    ${TARGET}
    "tst_qquick_image_extension.cpp"
)


list(APPEND TEST_INCLUDE_DIRS
    ${Qt5Quick_PRIVATE_INCLUDE_DIRS}
)
include(../../plugin_dependency.cmake)
include(../test-setup.cmake)

set_property(TEST ${TARGET} PROPERTY WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
