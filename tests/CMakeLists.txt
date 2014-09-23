
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

list(APPEND licence_dirs "modules" "examples" "documentation" "debian" "scripts" "tests" "manual-tests" "po")
foreach(CHECK_DIR ${licence_dirs})
    add_custom_target(license_${CHECK_DIR}
    COMMAND ${CMAKE_SOURCE_DIR}/tests/license/checklicense.sh
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/${CHECK_DIR}
    )
    list(APPEND CHECK_LICENSE_TARGETS license_${CHECK_DIR})
endforeach()

add_custom_target(license
    DEPENDS ${CHECK_LICENSE_TARGETS}
)

add_test(
    NAME "tst_apicheck"
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/qmlapicheck.sh ${CMAKE_CURRENT_BINARY_DIR}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)
set_property(TEST "tst_apicheck" PROPERTY LABELS "tst_apicheck")


