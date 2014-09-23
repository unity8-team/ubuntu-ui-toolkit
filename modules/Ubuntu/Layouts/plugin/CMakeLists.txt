set(uri "Ubuntu.Layouts")
string(REPLACE "." "/" install_path "${uri}")

set(install_path "${QT_INSTALL_QML}/${install_path}")

set(HEADERS ullayouts.h
    ullayouts_p.h
    ulconditionallayout.h
    ulconditionallayout_p.h
    ulplugin.h
    ulitemlayout.h
    propertychanges_p.h
)

set(SOURCES ullayouts.cpp
    ulconditionallayout.cpp
    ulplugin.cpp
    ulitemlayout.cpp
    ullayoutsattached.cpp
    propertychanges.cpp)


add_library(UbuntuLayouts SHARED
    ${SOURCES} ${HEADERS}
)
qt5_use_modules(UbuntuLayouts Qml Quick)

set_target_properties(UbuntuLayouts PROPERTIES
    COMPILE_DEFINITIONS "QT_NO_KEYWORDS"
    LINK_FLAGS "-Werror"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/../"
)

#!TODO add cmake equivalent of
#equals(QT_MAJOR_VERSION, 5):lessThan(QT_MINOR_VERSION, 2) {
#    QT += v8-private
#}

set_property(TARGET UbuntuLayouts PROPERTY INCLUDE_DIRECTORIES
    ${Qt5Core_PRIVATE_INCLUDE_DIRS}
    ${Qt5Qml_PRIVATE_INCLUDE_DIRS}
    ${Qt5Quick_PRIVATE_INCLUDE_DIRS}
    ${Qt5Gui_PRIVATE_INCLUDE_DIRS}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
)

install(TARGETS UbuntuLayouts DESTINATION ${install_path})
