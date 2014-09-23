set(EXAMPLE "ubuntu-ui-toolkit-gallery")

file(GLOB SOURCES *.qml *.png *.svg *.js *.qmltheme *.jpg *.qmlproject *.desktop *.wav gallery)
file(GLOB SOURCES_COMPONENTS components/*.qml
    components/*.png components/*.svg
    components/*.js components/*.qmltheme
    components/*.jpg components/*.qmlproject
    components/*.desktop components/*.wav)

add_custom_target(${EXAMPLE}_uncompiled_files ALL SOURCES ${SOURCES} ${SOURCES_COMPONENTS})

install(FILES ${EXAMPLE}.desktop DESTINATION /usr/share/applications)
install(FILES ${SOURCES} DESTINATION /usr/lib/ubuntu-ui-toolkit/examples/${EXAMPLE})
install(FILES ${SOURCES_COMPONENTS} DESTINATION /usr/lib/ubuntu-ui-toolkit/examples/${EXAMPLE}/components)

add_subdirectory(po)
