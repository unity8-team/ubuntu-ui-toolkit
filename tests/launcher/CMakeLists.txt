set(HEADERS
    MouseTouchAdaptor.h
)

set(SOURCES
    launcher.cpp
    MouseTouchAdaptor.cpp
)

add_executable(
    launcher
    ${SOURCES} ${HEADERS}
)

qt5_use_modules(launcher Qml Quick Test)

set_target_properties(launcher PROPERTIES
    COMPILE_DEFINITIONS "QT_NO_KEYWORDS"
    LINK_FLAGS "-Werror"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/../"
)

set_property(TARGET launcher PROPERTY INCLUDE_DIRECTORIES
    ${Qt5Core_PRIVATE_INCLUDE_DIRS}
    ${Qt5Quick_PRIVATE_INCLUDE_DIRS}
    ${Qt5Gui_PRIVATE_INCLUDE_DIRS}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
)

install(TARGETS launcher DESTINATION ${QT_INSTALL_LIBS}/ubuntu-ui-toolkit)
