set(TARGET tst_components_x11)

file(GLOB TESTS "*.qml")

add_executable(
    ${TARGET}
    tst_components.cpp tabsmodel.cpp tabsmodel.h
)

include(../../plugin_dependency.cmake)
include(../../layout_dependency.cmake)
include(../qmltest-setup.cmake)

message(STATUS ${CMAKE_SYSTEM_PROCESSOR})

