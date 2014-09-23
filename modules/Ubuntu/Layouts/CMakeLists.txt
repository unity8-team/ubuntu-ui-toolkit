add_subdirectory(plugin)

set(uri "Ubuntu.Layouts")
string(REPLACE "." "/" install_path "${uri}")
set(install_path "${QT_INSTALL_QML}/${install_path}")

set(QMLDIR_FILE "qmldir")

#make sure the qmldir file is available in the import path
file(COPY ${QMLDIR_FILE} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

add_custom_target(UbuntuLayouts_uncompiled_files ALL SOURCES ${QMLDIR_FILE})


# Silence spam on stderr due to fonts
# https://bugs.launchpad.net/ubuntu-ui-toolkit/+bug/1256999
# https://bugreports.qt-project.org/browse/QTBUG-36243
add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/plugins.qmltypes
                   COMMAND env ALARM_BACKEND=memory ${QT_INSTALL_BINS}/qmlplugindump -notrelocatable Ubuntu.Layouts 0.1 2>/dev/null > ${CMAKE_CURRENT_BINARY_DIR}/plugins.qmltypes
                   DEPENDS UbuntuLayouts
                   WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/modules")

#make sure the qmltypes file is generated
add_custom_target(UbuntuLayouts_generated_files ALL SOURCES ${CMAKE_CURRENT_BINARY_DIR}/plugins.qmltypes)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/plugins.qmltypes
    ${QMLDIR_FILE}
    DESTINATION ${install_path}
)
