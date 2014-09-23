
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
)
