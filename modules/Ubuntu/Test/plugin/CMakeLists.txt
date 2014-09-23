set(uri "Ubuntu.Test")
string(REPLACE "." "/" install_path "${uri}")

set(install_path "${QT_INSTALL_QML}/${install_path}")

set(HEADERS
    uctestcase.h
    testplugin.h
    uctestextras.h
)

set(SOURCES
    uctestcase.cpp
    testplugin.cpp
    uctestextras.cpp

)

add_library(UbuntuTest SHARED
    ${SOURCES} ${HEADERS}
)

qt5_use_modules(UbuntuTest Qml Quick Test)

#equals(QT_MAJOR_VERSION, 5):lessThan(QT_MINOR_VERSION, 2) {
#    QT += v8-private
#}

set_target_properties(UbuntuTest PROPERTIES
    LINK_FLAGS "-Werror"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/../"
)

set_property(TARGET UbuntuTest PROPERTY COMPILE_DEFINITIONS
    "QT_NO_KEYWORDS"
    UBUNTU_QML_IMPORT_PATH=\"${TESTROOT}${QT_INSTALL_QML}\"
)

set_property(TARGET UbuntuTest PROPERTY INCLUDE_DIRECTORIES
    ${Qt5Core_PRIVATE_INCLUDE_DIRS}
    ${Qt5Qml_PRIVATE_INCLUDE_DIRS}
    ${Qt5Quick_PRIVATE_INCLUDE_DIRS}
    ${Qt5Gui_PRIVATE_INCLUDE_DIRS}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
)

install(TARGETS UbuntuTest DESTINATION ${install_path})
