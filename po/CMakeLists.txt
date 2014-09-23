set(PROJECTNAME "ubuntu-ui-toolkit")
file(GLOB_RECURSE SOURCECODE "../*.qml")

message(STATUS "")
message(STATUS " Project Name: ${PROJECTNAME} ")
#message(STATUS " Source Code: ${SOURCECODE} ")
message(STATUS "")
message(STATUS " run 'make pot' to generate the pot file from source code. ")
message(STATUS " run 'cmake; make install' to install the mo files. ")
message(STATUS "")

## generate pot file 'make pot'
add_custom_target(
    pot
    COMMAND bash update-pot.sh ubuntu-ui-toolkit ${CMAKE_CURRENT_SOURCE_DIR}/../modules ${CMAKE_CURRENT_SOURCE_DIR}
    ## generate pot file for gallery
    COMMAND bash update-pot.sh ubuntu-ui-toolkit-gallery ${CMAKE_CURRENT_SOURCE_DIR}/../examples/ubuntu-ui-toolkit-gallery ${CMAKE_CURRENT_SOURCE_DIR}/../examples/ubuntu-ui-toolkit-gallery/po
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

## Installation steps for mo files. 'make install'
file(GLOB PO_FILES "*.po")

foreach(PO_FILE ${PO_FILES})
    get_filename_component(MO_NAME ${PO_FILE} NAME_WE)
    set(OUT_FILE  "${CMAKE_CURRENT_BINARY_DIR}/${MO_NAME}.mo")
    set(IN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/${MO_NAME}.po")

    add_custom_command(
        OUTPUT  ${OUT_FILE}
        DEPENDS ${IN_FILE}
        COMMAND msgfmt ${IN_FILE} -o ${OUT_FILE}
    )

    list(APPEND MO_RESULTS ${OUT_FILE})
    install(FILES ${OUT_FILE} DESTINATION "/usr/share/locale/${MO_NAME}/LC_MESSAGES")
endforeach()

add_custom_target(Translations ALL DEPENDS ${MO_RESULTS})
