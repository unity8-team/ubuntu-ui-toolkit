set(DOC_SRC .)
set(DOC_PATH ${CMAKE_CURRENT_SOURCE_DIR})

if (Qt5Core_VERSION VERSION_LESS 5.2.0)
    set(DOC_SRC ${CMAKE_SOURCE_DIR}/documentation)
endif()

add_custom_target(docs ALL
    COMMAND env SRC=${DOC_SRC} ${CMAKE_SOURCE_DIR}/documentation/docs.sh ${DOC_PATH};
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/html DESTINATION /usr/share/ubuntu-ui-toolkit/doc)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/ubuntuuserinterfacetoolkit.qch DESTINATION /usr/share/qt5/doc/qch)
