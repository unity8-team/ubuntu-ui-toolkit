set(TARGET tst_alarms)

add_executable(
    ${TARGET}
    "tst_alarms.cpp"
)

list(APPEND TEST_QT_MODULES Organizer)

include(../../plugin_dependency.cmake)
include(../test-setup.cmake)

