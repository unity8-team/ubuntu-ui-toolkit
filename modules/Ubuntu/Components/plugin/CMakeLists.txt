set(uri "Ubuntu.Components")
string(REPLACE "." "/" install_path "${uri}")

set(install_path "${QT_INSTALL_QML}/${install_path}")

if(UNIX)
    find_package(PkgConfig)
    pkg_check_modules(UbuntuComponents_DEPS gio-2.0 thumbnailer dbus-1 libnih-dbus REQUIRED)
endif()

set(UbuntuComponents_HEADERS plugin.h
    uctheme.h
    ucthemesettings.h
    i18n.h
    listener.h
    ucscalingimageprovider.h
    ucunits.h
    ucqquickimageextension.h
    quickutils.h
    shapeitemtexture.h
    shapeitem.h
    inversemouseareatype.h
    qquickclipboard.h
    qquickmimedata.h
    qquickclipboard_p.h
    ucubuntuanimation.h
    ucfontutils.h
    ucapplication.h
    ucarguments.h
    ucargument.h
    ucalarm.h
    ucalarm_p.h
    alarmmanager_p_p.h
    alarmmanager_p.h
    ucalarmmodel.h
    unitythemeiconprovider.h
    thumbnailgenerator.h
    alarmrequest_p.h
    alarmrequest_p_p.h
    adapters/alarmsadapter_p.h
    ucstatesaver.h
    sortbehavior.h
    filterbehavior.h
    sortfiltermodel.h
    statesaverbackend_p.h
    ucstatesaver_p.h
    ucurihandler.h
    ucinversemouse.h
    ucmouse.h
    unixsignalhandler_p.h
    ucstyleditembase.h
    ucstyleditembase_p.h)

set(UbuntuComponents_SOURCES plugin.cpp
    uctheme.cpp
    ucthemesettings.cpp
    i18n.cpp
    listener.cpp
    ucscalingimageprovider.cpp
    ucunits.cpp
    ucqquickimageextension.cpp
    quickutils.cpp
    shapeitem.cpp
    inversemouseareatype.cpp
    qquickclipboard.cpp
    qquickmimedata.cpp
    ucubuntuanimation.cpp
    ucfontutils.cpp
    ucapplication.cpp
    ucarguments.cpp
    ucargument.cpp
    ucalarm.cpp
    alarmmanager_p.cpp
    ucalarmmodel.cpp
    unitythemeiconprovider.cpp
    thumbnailgenerator.cpp
    alarmrequest_p.cpp
    ucstatesaver.cpp
    sortbehavior.cpp
    filterbehavior.cpp
    sortfiltermodel.cpp
    statesaverbackend_p.cpp
    ucurihandler.cpp
    ucmousefilters.cpp
    unixsignalhandler_p.cpp
    ucstyleditembase.cpp)

# adapters
list(APPEND UbuntuComponents_SOURCES adapters/alarmsadapter_organizer.cpp)

add_library(UbuntuComponents SHARED
    ${UbuntuComponents_SOURCES} ${UbuntuComponents_HEADERS}
)
qt5_use_modules(UbuntuComponents Qml Quick DBus Svg Organizer)

set_target_properties(UbuntuComponents PROPERTIES
    COMPILE_DEFINITIONS "QT_NO_KEYWORDS"
    LINK_FLAGS "-Werror"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/../"
)
set_property(TARGET UbuntuComponents PROPERTY INCLUDE_DIRECTORIES
    ${Qt5Core_PRIVATE_INCLUDE_DIRS}
    ${Qt5Qml_PRIVATE_INCLUDE_DIRS}
    ${Qt5Quick_PRIVATE_INCLUDE_DIRS}
    ${Qt5Gui_PRIVATE_INCLUDE_DIRS}
    ${UbuntuComponents_DEPS_INCLUDE_DIRS}
)

set_property(TARGET UbuntuComponents PROPERTY COMPILE_FLAGS
    ${UbuntuComponents_DEPS_CFLAGS_OTHER}
)

target_link_libraries(
  UbuntuComponents
  ${UbuntuComponents_DEPS_LDFLAGS}
)

install(TARGETS UbuntuComponents DESTINATION ${install_path})
