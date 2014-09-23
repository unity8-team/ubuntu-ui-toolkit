set(uri "Ubuntu.PerformanceMetrics")
string(REPLACE "." "/" install_path "${uri}")

set(install_path "${QT_INSTALL_QML}/${install_path}")

set(HEADERS
    upmplugin.h
    upmgraphmodel.h
    upmtexturefromimage.h
    upmrenderingtimes.h
    upmcpuusage.h
    rendertimer.h
)

set(SOURCES
    upmplugin.cpp
    upmgraphmodel.cpp
    upmtexturefromimage.cpp
    upmrenderingtimes.cpp
    upmcpuusage.cpp
    rendertimer.cpp
)

#!TODO
#contains(QT_CONFIG, opengles2) {
#    CONFIG += egl
#}

add_library(UbuntuPerformanceMetrics SHARED
    ${SOURCES} ${HEADERS}
)
qt5_use_modules(UbuntuPerformanceMetrics Qml Quick)

set_target_properties(UbuntuPerformanceMetrics PROPERTIES
    COMPILE_DEFINITIONS "QT_NO_KEYWORDS"
    LINK_FLAGS "-Werror"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/../"
)

set_property(TARGET UbuntuPerformanceMetrics PROPERTY INCLUDE_DIRECTORIES
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
)

install(TARGETS UbuntuPerformanceMetrics DESTINATION ${install_path})
