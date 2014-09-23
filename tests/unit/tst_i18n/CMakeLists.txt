set(TARGET tst_i18n)

add_executable(
    ${TARGET}
    "src/tst_i18n.cpp"
)

file(GLOB QML_FILES src/*.qml)
file(COPY diversion localizedApp po src DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

add_test(
    NAME tst_i18n_generated_files
    COMMAND msgfmt po/en_US.po -o localizedApp/share/locale/en/LC_MESSAGES/localizedApp.mo
)

include(../../plugin_dependency.cmake)
include(../test-setup.cmake)

set_property(TEST ${TARGET} APPEND PROPERTY DEPENDS tst_i18n_generated_files )
