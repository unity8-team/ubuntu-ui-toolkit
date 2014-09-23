add_subdirectory(10)
add_subdirectory(11)
add_subdirectory(plugin)
add_subdirectory(Themes)

set(uri "Ubuntu.Components")
string(REPLACE "." "/" install_path "${uri}")

set(install_path "${QT_INSTALL_QML}/${install_path}")

# Components
file(GLOB QML_FILES *.qml)
file(GLOB JS_FILES *.js)

# Components/artwork
file(GLOB ARTWORK_FILES artwork/*)

# Components/Listitems
file(GLOB_RECURSE LISTITEMS_FILES ListItems/*)
list(REMOVE_ITEM LISTITEMS_FILES "${CMAKE_CURRENT_SOURCE_DIR}/ListItems/artwork")
file(GLOB LISTITEMS_ARTWORK_FILES ListItems/artwork/*)

# Components/Popups
file(GLOB POPUPS_FILES Popups/*)

#Components/Pickers
file(GLOB PICKER_FILES Pickers/* )

#Style API
file(GLOB STYLES_FILES Styles/* )

# qmldir
set(QMLDIR_FILE qmldir )

#make sure the qmldir file is available in the import path for the tests
file(COPY ${QMLDIR_FILE} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

add_custom_target(Components_uncompiled_files ALL SOURCES ${QML_FILES} ${JS_FILES}
                        ${ARTWORK_FILES} ${LISTITEMS_FILES}
                        ${LISTITEMS_ARTWORK_FILES} ${POPUPS_FILES}
                        ${PICKER_FILES} ${STYLES_FILES} ${QMLDIR_FILE})

# define deployment for found deployables
install(FILES ${QMLDIR_FILE} DESTINATION ${install_path})
install(FILES ${QML_FILES} DESTINATION ${install_path})
install(FILES ${JS_FILES} DESTINATION ${install_path})
install(FILES ${ARTWORK_FILES} DESTINATION ${install_path}/artwork)
install(FILES ${LISTITEMS_FILES} DESTINATION ${install_path}/ListItems)
install(FILES ${LISTITEMS_ARTWORK_FILES} DESTINATION ${install_path}/ListItems/artwork)
install(FILES ${POPUPS_FILES} DESTINATION ${install_path}/Popups)
install(FILES ${PICKER_FILES} DESTINATION ${install_path}/Pickers)
install(FILES ${STYLES_FILES} DESTINATION ${install_path}/Styles)

# Silence spam on stderr due to fonts
# https://bugs.launchpad.net/ubuntu-ui-toolkit/+bug/1256999
# https://bugreports.qt-project.org/browse/QTBUG-36243
add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/plugins.qmltypes
                   COMMAND env ALARM_BACKEND=memory ${QT_INSTALL_BINS}/qmlplugindump -notrelocatable Ubuntu.Components 0.1 2>/tmp/out > ${CMAKE_CURRENT_BINARY_DIR}/plugins.qmltypes
                   DEPENDS UbuntuComponents
                   WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

#make sure the qmltypes file is generated
add_custom_target(Components_generated_files ALL SOURCES ${CMAKE_CURRENT_BINARY_DIR}/plugins.qmltypes)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/plugins.qmltypes DESTINATION ${install_path})
