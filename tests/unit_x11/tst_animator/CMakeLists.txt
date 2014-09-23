set(TARGET tst_animator)

file(GLOB QML_SOURCE "*.qml")

add_executable(
    ${TARGET}
    "tst_animator.cpp"
)

include(../../plugin_dependency.cmake)
include(../test-setup.cmake)

message(STATUS ${CMAKE_SYSTEM_PROCESSOR})

