set(uri "Ubuntu.Components.10")
string(REPLACE "." "/" install_path "${uri}")

set(install_path "${QT_INSTALL_QML}/${install_path}")

file(GLOB QML_FILES *.qml)

add_custom_target(Components_10_uncompiled_files ALL SOURCES ${QML_FILES})

install(FILES ${QML_FILES} DESTINATION ${install_path})
