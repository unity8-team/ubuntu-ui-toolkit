set(uri "Ubuntu.Components.Themes.Ambiance")
string(REPLACE "." "/" install_path "${uri}")

set(install_path "${QT_INSTALL_QML}/${install_path}")

# Components
file(GLOB QML_FILES *.qml)

# Components/artwork
file(GLOB ARTWORK_FILES
    artwork/*.png
    artwork/*.svg
    artwork/*.sci
)

# qmldir
set(QMLDIR_FILE qmldir )

add_custom_target(Themes_Ambiance_uncompiled_files ALL SOURCES ${QML_FILES} ${QMLDIR_FILE} ${ARTWORK_FILES})

install(FILES ${QMLDIR_FILE} DESTINATION ${install_path})
install(FILES ${QML_FILES} DESTINATION ${install_path})
install(FILES ${ARTWORK_FILES} DESTINATION ${install_path}/artwork)
