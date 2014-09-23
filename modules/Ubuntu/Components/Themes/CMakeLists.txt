add_subdirectory(Ambiance)
add_subdirectory(SuruDark)
add_subdirectory(SuruGradient)

set(uri "Ubuntu.Components.Themes")
string(REPLACE "." "/" install_path "${uri}")

set(install_path "${QT_INSTALL_QML}/${install_path}")

# Components
file(GLOB QML_FILES *.qml)

# qmldir
set(QMLDIR_FILE qmldir )

add_custom_target(Themes_uncompiled_files ALL SOURCES ${QML_FILES} ${QMLDIR_FILE})

install(FILES ${QMLDIR_FILE} DESTINATION ${install_path})
install(FILES ${QML_FILES} DESTINATION ${install_path})
