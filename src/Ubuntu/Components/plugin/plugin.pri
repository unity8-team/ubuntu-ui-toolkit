unix {
    CONFIG += link_pkgconfig
    PKGCONFIG += gio-2.0 dbus-1 libnih-dbus
}

QT *= core-private qml qml-private quick quick-private gui-private dbus svg UbuntuGestures UbuntuGestures_private UbuntuToolkit

equals(QT_MAJOR_VERSION, 5):lessThan(QT_MINOR_VERSION, 2) {
    QT += v8-private
}

CONFIG += no_keywords c++11

# QOrganizer
QT *= organizer

#comment in the following line to disable traces
#DEFINES += QT_NO_DEBUG_OUTPUT
DEFINES += QT_USE_QSTRINGBUILDER

INCLUDEPATH+=$$PWD

HEADERS += $$PWD/plugin.h \
    $$PWD/uctheme.h \
    $$PWD/i18n.h \
    $$PWD/listener.h \
    $$PWD/ucscalingimageprovider.h \
    $$PWD/ucunits.h \
    $$PWD/ucqquickimageextension.h \
    $$PWD/quickutils.h \
    $$PWD/ucubuntushapetexture.h \
    $$PWD/ucubuntushape.h \
    $$PWD/ucubuntushapeoverlay.h \
    $$PWD/ucproportionalshape.h \
    $$PWD/inversemouseareatype.h \
    $$PWD/qquickclipboard.h \
    $$PWD/qquickmimedata.h \
    $$PWD/qquickclipboard_p.h \
    $$PWD/ucubuntuanimation.h \
    $$PWD/ucfontutils.h \
    $$PWD/ucapplication.h \
    $$PWD/ucarguments.h \
    $$PWD/ucargument.h \
    $$PWD/ucalarm.h \
    $$PWD/ucalarm_p.h \
    $$PWD/alarmmanager_p_p.h \
    $$PWD/alarmmanager_p.h \
    $$PWD/ucalarmmodel.h \
    $$PWD/unitythemeiconprovider.h \
    $$PWD/adapters/alarmsadapter_p.h \
    $$PWD/ucstatesaver.h \
    $$PWD/sortbehavior.h \
    $$PWD/filterbehavior.h \
    $$PWD/sortfiltermodel.h \
    $$PWD/statesaverbackend_p.h \
    $$PWD/ucstatesaver_p.h \
    $$PWD/ucurihandler.h \
    $$PWD/ucinversemouse.h \
    $$PWD/ucmouse.h \
    $$PWD/unixsignalhandler_p.h \
    $$PWD/ucstyleditembase.h \
    $$PWD/ucstyleditembase_p.h \
    $$PWD/ucaction.h \
    $$PWD/ucactioncontext.h \
    $$PWD/ucactionmanager.h \
    $$PWD/adapters/actionsproxy_p.h \
    $$PWD/adapters/dbuspropertywatcher_p.h \
    $$PWD/uclistitem.h \
    $$PWD/uclistitem_p.h \
    $$PWD/uclistitemactions.h \
    $$PWD/uclistitemactions_p.h \
    $$PWD/propertychange_p.h \
    $$PWD/uclistitemstyle.h \
    $$PWD/ucserviceproperties.h \
    $$PWD/ucserviceproperties_p.h \
    $$PWD/privates/listitemdragarea.h \
    $$PWD/privates/listitemdraghandler.h \
    $$PWD/privates/listitemselection.h \
    $$PWD/ucnamespace.h \
    $$PWD/ucdeprecatedtheme.h \
    $$PWD/ucdefaulttheme.h \
    $$PWD/ucstylehints.h \
    $$PWD/ucslotslayout.h \
    $$PWD/ucslotslayout_p.h \
    $$PWD/livetimer.h \
    $$PWD/livetimer_p.h \
    $$PWD/timeutils_p.h \
    $$PWD/ucactionitem.h \
    $$PWD/ucactionitem_p.h \
    $$PWD/uchaptics.h \
    $$PWD/ucabstractbutton.h \
    $$PWD/ucabstractbutton_p.h \
    $$PWD/ucmargins.h \
    $$PWD/ucthemingextension.h \
    $$PWD/ucheader.h \
    $$PWD/uclabel.h \
    $$PWD/uclistitemlayout.h \
    $$PWD/privates/threelabelsslot_p.h \
    $$PWD/ucimportversionchecker_p.h \
    $$PWD/ucbottomedgehint.h \
    $$PWD/ucbottomedgehint_p.h \
    $$PWD/gestures/ubuntugesturesqmlglobal.h \
    $$PWD/ucmathutils.h \
    $$PWD/ucbottomedge.h \
    $$PWD/ucbottomedge_p.h \
    $$PWD/ucbottomedgestyle.h \
    $$PWD/ucbottomedgeregion.h \
    $$PWD/ucpagetreenode.h \
    $$PWD/ucpagetreenode_p.h \
    $$PWD/ucmainviewbase.h \
    $$PWD/ucmainviewbase_p.h \
    $$PWD/ucperformancemonitor.h \
    $$PWD/label_p.h \
    $$PWD/privates/listviewextensions.h \
    $$PWD/privates/ucpagewrapper.h \
    $$PWD/privates/ucpagewrapper_p.h \
    $$PWD/privates/ucpagewrapperincubator_p.h \
    $$PWD/privates/appheaderbase.h \
    $$PWD/button.h \
    $$PWD/textbutton.h \
    $$PWD/privates/shape/utils.h \
    $$PWD/privates/shape/texturefactory.h \
    $$PWD/privates/shape/fill.h \
    $$PWD/privates/shape/frame.h \
    $$PWD/privates/shape/shadow.h

SOURCES += $$PWD/plugin.cpp \
    $$PWD/uctheme.cpp \
    $$PWD/i18n.cpp \
    $$PWD/listener.cpp \
    $$PWD/ucscalingimageprovider.cpp \
    $$PWD/ucunits.cpp \
    $$PWD/ucqquickimageextension.cpp \
    $$PWD/quickutils.cpp \
    $$PWD/ucubuntushape.cpp \
    $$PWD/ucubuntushapeoverlay.cpp \
    $$PWD/ucproportionalshape.cpp \
    $$PWD/inversemouseareatype.cpp \
    $$PWD/qquickclipboard.cpp \
    $$PWD/qquickmimedata.cpp \
    $$PWD/ucubuntuanimation.cpp \
    $$PWD/ucfontutils.cpp \
    $$PWD/ucapplication.cpp \
    $$PWD/ucarguments.cpp \
    $$PWD/ucargument.cpp \
    $$PWD/ucalarm.cpp \
    $$PWD/alarmmanager_p.cpp \
    $$PWD/ucalarmmodel.cpp \
    $$PWD/unitythemeiconprovider.cpp \
    $$PWD/ucstatesaver.cpp \
    $$PWD/sortbehavior.cpp \
    $$PWD/filterbehavior.cpp \
    $$PWD/sortfiltermodel.cpp \
    $$PWD/statesaverbackend_p.cpp \
    $$PWD/ucurihandler.cpp \
    $$PWD/ucmousefilters.cpp \
    $$PWD/unixsignalhandler_p.cpp \
    $$PWD/ucstyleditembase.cpp \
    $$PWD/ucaction.cpp \
    $$PWD/ucactioncontext.cpp \
    $$PWD/ucactionmanager.cpp \
    $$PWD/adapters/actionsproxy_p.cpp \
    $$PWD/adapters/dbuspropertywatcher_p.cpp \
    $$PWD/uclistitem.cpp \
    $$PWD/uclistitemactions.cpp \
    $$PWD/propertychange_p.cpp \
    $$PWD/uclistitemstyle.cpp \
    $$PWD/ucviewitemsattached.cpp \
    $$PWD/ucserviceproperties.cpp \
    $$PWD/privates/listitemdragarea.cpp \
    $$PWD/privates/listitemdraghandler.cpp \
    $$PWD/privates/listitemexpansion.cpp \
    $$PWD/privates/listitemselection.cpp \
    $$PWD/ucnamespace.cpp \
    $$PWD/ucdeprecatedtheme.cpp \
    $$PWD/ucdefaulttheme.cpp \
    $$PWD/ucstylehints.cpp \
    $$PWD/ucslotslayout.cpp \
    $$PWD/livetimer.cpp \
    $$PWD/livetimer_p.cpp \
    $$PWD/ucactionitem.cpp \
    $$PWD/uchaptics.cpp \
    $$PWD/ucabstractbutton.cpp \
    $$PWD/ucthemingextension.cpp \
    $$PWD/ucheader.cpp \
    $$PWD/uclabel.cpp \
    $$PWD/uclistitemlayout.cpp \
    $$PWD/privates/threelabelsslot_p.cpp \
    $$PWD/ucimportversionchecker_p.cpp \
    $$PWD/ucbottomedgehint.cpp \
    $$PWD/ucmathutils.cpp \
    $$PWD/ucbottomedge.cpp \
    $$PWD/ucbottomedgestyle.cpp \
    $$PWD/ucbottomedgeregion.cpp \
    $$PWD/ucpagetreenode.cpp \
    $$PWD/ucmainviewbase.cpp \
    $$PWD/ucperformancemonitor.cpp \
    $$PWD/privates/listviewextensions.cpp \
    $$PWD/privates/ucpagewrapper.cpp \
    $$PWD/privates/ucpagewrapperincubator.cpp \
    $$PWD/privates/appheaderbase.cpp \
    $$PWD/button.cpp \
    $$PWD/textbutton.cpp \
    $$PWD/privates/shape/utils.cpp \
    $$PWD/privates/shape/texturefactory.cpp \
    $$PWD/privates/shape/fill.cpp \
    $$PWD/privates/shape/frame.cpp \
    $$PWD/privates/shape/shadow.cpp

# adapters
SOURCES += $$PWD/adapters/alarmsadapter_organizer.cpp

RESOURCES += \
    $$PWD/plugin.qrc

OTHER_FILES += \
    $$PWD/shaders/shape.vert \
    $$PWD/shaders/shape.frag \
    $$PWD/shaders/shape_no_dfdy.frag \
    $$PWD/shaders/shapeoverlay.vert \
    $$PWD/shaders/shapeoverlay.frag \
    $$PWD/shaders/shapeoverlay_no_dfdy.frag \
    $$PWD/privates/shape/shaders/color.vert \
    $$PWD/privates/shape/shaders/opaquecolor.frag \
    $$PWD/privates/shape/shaders/color.frag \
    $$PWD/privates/shape/shaders/texture.vert \
    $$PWD/privates/shape/shaders/luminance.frag \
    $$PWD/privates/shape/shaders/shadow.frag \
    $$PWD/privates/shape/shaders/frame.vert \
    $$PWD/privates/shape/shaders/frame.frag
