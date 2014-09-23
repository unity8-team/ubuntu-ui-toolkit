set(uri "Ubuntu.Components.Themes.SuruDark")
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
set(QMLDIR_FILE "qmldir" )

set(PARENT_THEME_FILE "parent_theme")

add_custom_target(Themes_SuruDark_uncompiled_files ALL SOURCES ${QML_FILES} ${QMLDIR_FILE} ${ARTWORK_FILES} ${PARENT_THEME_FILE})

install(FILES ${QMLDIR_FILE} ${PARENT_THEME_FILE} DESTINATION ${install_path})
install(FILES ${QML_FILES} DESTINATION ${install_path})
install(FILES ${ARTWORK_FILES} DESTINATION ${install_path}/artwork)
