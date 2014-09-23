set(TARGET tst_scaling_image_provider)

qt5_add_resources(UI_RESOURCES tst_scaling_image_provider.qrc)

add_executable(
    ${TARGET}
    "tst_scaling_image_provider.cpp"
    ${UI_RESOURCES}
)

include(../../plugin_dependency.cmake)
include(../test-setup.cmake)

set_property(TEST ${TARGET} PROPERTY WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
