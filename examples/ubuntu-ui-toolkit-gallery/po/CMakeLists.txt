#translation
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

add_custom_target(Translations_gallery ALL DEPENDS ${MO_RESULTS})
