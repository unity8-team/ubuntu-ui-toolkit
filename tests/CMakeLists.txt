
install(
    DIRECTORY "autopilot/ubuntuuitoolkit"
    DESTINATION "/usr/lib/python3/dist-packages"
)

install(
    DIRECTORY "autopilot/ubuntuuitoolkit"
    DESTINATION "/usr/lib/python2.7/dist-packages"
)


add_subdirectory(unit)
add_subdirectory(unit_x11)
add_subdirectory(launcher)
add_subdirectory(autopilot)

add_custom_target(test-xvfb
    COMMAND ${CMAKE_SOURCE_DIR}/tests/xvfb.sh ctest -V
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    DEPENDS UbuntuComponents UbuntuLayouts UbuntuTest UbuntuPerformanceMetrics
)

add_custom_target(test-performance
    COMMAND ctest -L tst_performance
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    DEPENDS UbuntuComponents UbuntuLayouts UbuntuTest UbuntuPerformanceMetrics
)

add_custom_target(test-api
    COMMAND ctest -L tst_components
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    DEPENDS UbuntuComponents UbuntuLayouts UbuntuTest UbuntuPerformanceMetrics
)

add_custom_target(qmluitests
    COMMAND ctest -L unit_x11_tst_components
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    DEPENDS UbuntuComponents UbuntuLayouts UbuntuTest UbuntuPerformanceMetrics
)

add_custom_target(license ALL
    COMMAND ./tests/license/checklicense.sh
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

