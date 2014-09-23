unix {
    CONFIG += link_pkgconfig
    PKGCONFIG += gio-2.0 thumbnailer dbus-1 libnih-dbus
}

TEMPLATE = lib
TARGET = ../UbuntuComponents
QT += core-private qml qml-private quick quick-private gui-private dbus svg

equals(QT_MAJOR_VERSION, 5):lessThan(QT_MINOR_VERSION, 2) {
    QT += v8-private
}

CONFIG += qt plugin no_keywords

QMAKE_CXXFLAGS += -Werror

# QOrganizer
QT += organizer

#comment in the following line to disable traces
#DEFINES += QT_NO_DEBUG_OUTPUT

TARGET = $$qtLibraryTarget($$TARGET)
uri = Ubuntu.Components

HEADERS += plugin.h \
    uctheme.h \
    ucthemesettings.h \
    i18n.h \
    listener.h \
    ucscalingimageprovider.h \
    ucunits.h \
    ucqquickimageextension.h \
    quickutils.h \
    shapeitemtexture.h \
    shapeitem.h \
    inversemouseareatype.h \
    qquickclipboard.h \
    qquickmimedata.h \
    qquickclipboard_p.h \
    ucubuntuanimation.h \
    ucfontutils.h \
    ucapplication.h \
    ucarguments.h \
    ucargument.h \
    ucalarm.h \
    ucalarm_p.h \
    alarmmanager_p_p.h \
    alarmmanager_p.h \
    ucalarmmodel.h \
    unitythemeiconprovider.h \
    thumbnailgenerator.h \
    alarmrequest_p.h \
    alarmrequest_p_p.h \
    adapters/alarmsadapter_p.h \
    ucstatesaver.h \
    sortbehavior.h \
    filterbehavior.h \
    sortfiltermodel.h \
    statesaverbackend_p.h \
    ucstatesaver_p.h \
    ucurihandler.h \
    ucinversemouse.h \
    ucmouse.h \
    unixsignalhandler_p.h \
    ucstyleditembase.h \
    ucstyleditembase_p.h \
    ucaction.h \
    ucactioncontext.h \
    ucactionmanager.h \
    adapters/actionsproxy_p.h

SOURCES += plugin.cpp \
    uctheme.cpp \
    ucthemesettings.cpp \
    i18n.cpp \
    listener.cpp \
    ucscalingimageprovider.cpp \
    ucunits.cpp \
    ucqquickimageextension.cpp \
    quickutils.cpp \
    shapeitem.cpp \
    inversemouseareatype.cpp \
    qquickclipboard.cpp \
    qquickmimedata.cpp \
    ucubuntuanimation.cpp \
    ucfontutils.cpp \
    ucapplication.cpp \
    ucarguments.cpp \
    ucargument.cpp \
    ucalarm.cpp \
    alarmmanager_p.cpp \
    ucalarmmodel.cpp \
    unitythemeiconprovider.cpp \
    thumbnailgenerator.cpp \
    alarmrequest_p.cpp \
    ucstatesaver.cpp \
    sortbehavior.cpp \
    filterbehavior.cpp \
    sortfiltermodel.cpp \
    statesaverbackend_p.cpp \
    ucurihandler.cpp \
    ucmousefilters.cpp \
    unixsignalhandler_p.cpp \
    ucstyleditembase.cpp \
    ucaction.cpp \
    ucactioncontext.cpp \
    ucactionmanager.cpp \
    adapters/actionsproxy_p.cpp

# adapters
SOURCES += adapters/alarmsadapter_organizer.cpp

#resources
RESOURCE_FILES = $$system(ls ../10/*.qml) \
                $$system(ls ../10/*.js) \
                $$system(ls ../11/*.qml) \
                $$system(ls ../11/*.js) \
                $$system(ls ../Internals/*.qml) \
                $$system(ls ../Internals/*.js) \
                $$system(ls ../*.js) \
                $$system(find ../artwork -type f)

GENERATED_RESOURCE_FILE = $$OUT_PWD/resources.qrc

RESOURCE_CONTENT = \
    "<RCC>" \
    "<qresource prefix=\"/Ubuntu/Components\">"

for (resourcefile, RESOURCE_FILES) {
    resourcefileabsolutepath = $$absolute_path($$resourcefile)
    relativepath_in = $$relative_path($$resourcefileabsolutepath, $$_PRO_FILE_PWD_)
    relativepath_out = $$relative_path($$resourcefileabsolutepath, $$OUT_PWD)
    RESOURCE_CONTENT += "<file alias=\"$$relativepath_in\">$$relativepath_out</file>"
}

RESOURCE_CONTENT += \
    "</qresource>" \
    "</RCC>"

write_file($$GENERATED_RESOURCE_FILE, RESOURCE_CONTENT)|error("Aborting.")

RESOURCES +=$$GENERATED_RESOURCE_FILE

# deployment rules for the plugin
installPath = $$[QT_INSTALL_QML]/$$replace(uri, \\., /)
target.path = $$installPath
INSTALLS += target

!equals(_PRO_FILE_PWD_, $$OUT_PWD) {
    main_qmldir.target = .qmldir
    main_qmldir.commands = cp \"$$_PRO_FILE_PWD_/../qmldir\" \"$$$$OUT_PWD/../qmldir\"
    QMAKE_EXTRA_TARGETS += main_qmldir
    PRE_TARGETDEPS += $$main_qmldir.target
}
