set(EXAMPLE "locale")
file(GLOB SOURCES *.qml *.png *.svg *.js *.qmltheme *.jpg *.qmlproject *.desktop *.wav)

add_custom_target(${EXAMPLE}_uncompiled_files ALL SOURCES ${SOURCES})

install(FILES ${EXAMPLE}.desktop DESTINATION /usr/share/applications)
install(FILES ${SOURCES} DESTINATION /usr/lib/ubuntu-ui-toolkit/examples/${EXAMPLE})
