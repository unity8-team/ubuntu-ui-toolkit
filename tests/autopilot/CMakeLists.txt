file(GLOB_RECURSE AUTOPILOT_SRC *.py)

add_custom_target(autopilot_tests_uncompiled_files ALL SOURCES ${AUTOPILOT_SRC})

add_custom_target(test-components
    COMMAND python3 -m autopilot.run run ubuntuuitoolkit
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    DEPENDS UbuntuComponents UbuntuLayouts UbuntuTest UbuntuPerformanceMetrics
)

add_custom_target(test-components2
    COMMAND python2 -m autopilot.run run ubuntuuitoolkit
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    DEPENDS UbuntuComponents UbuntuLayouts UbuntuTest UbuntuPerformanceMetrics
)
