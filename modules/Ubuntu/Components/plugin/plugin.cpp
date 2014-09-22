/*
 * Copyright 2012-2014 Canonical Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Author: Juhapekka Piiroinen <juhapekka.piiroinen@canonical.com>
 */

#include <QtQuick/private/qquickimagebase_p.h>
#include <QDBusConnection>
#include <QtGui/QGuiApplication>
#include <QtGui/QScreen>

#include "plugin.h"
#include "uctheme.h"

#include <QtQml/QQmlContext>
#include "i18n.h"
#include "listener.h"
#include "ucunits.h"
#include "ucscalingimageprovider.h"
#include "ucqquickimageextension.h"
#include "quickutils.h"
#include "shapeitem.h"
#include "inversemouseareatype.h"
#include "qquickclipboard.h"
#include "qquickmimedata.h"
#include "thumbnailgenerator.h"
#include "ucubuntuanimation.h"
#include "ucfontutils.h"
#include "ucarguments.h"
#include "ucargument.h"
#include "ucapplication.h"
#include "ucalarm.h"
#include "ucalarmmodel.h"
#include "unitythemeiconprovider.h"
#include "ucstatesaver.h"
#include "ucurihandler.h"
#include "ucmouse.h"
#include "ucinversemouse.h"
#include "sortfiltermodel.h"
#include "ucstyleditembase.h"
#include "ucaction.h"
#include "ucactioncontext.h"
#include "ucactionmanager.h"

#include <QtQml/private/qjsvalue_p.h>

#include <sys/types.h>
#include <unistd.h>
#include <stdexcept>

static const QmlType qmlTypes[] = {
    {"ActionItem", NULL, false, 0, 1}, {"ActionItem", NULL, false, 1, 0},
    {"ActionList", NULL, false, 0, 1}, {"ActionList", NULL, false, 1, 0},
    {"ToolbarItems", NULL, false, 0, 1}, {"ToolbarItems", NULL, false, 1, 0},
    {"ToolbarButton", NULL, false, 0, 1}, {"ToolbarButton", NULL, false, 1, 0},
    {"MainView", NULL, false, 0, 1}, {"MainView", NULL, false, 1, 0},
    {"Button", "10/Button.qml", false, 0, 1}, {"Button", "10/Button.qml", false, 1, 0},
    {"Panel", NULL, false, 0, 1}, {"Panel", NULL, false, 1, 0},
    {"Tab", NULL, false, 0, 1}, {"Tab", NULL, false, 1, 0},
    {"TabBar", NULL, false, 0, 1}, {"TabBar", NULL, false, 1, 0},
    {"Tabs", NULL, false, 0, 1}, {"Tabs", NULL, false, 1, 0},
    {"Label", NULL, false, 0, 1}, {"Label", NULL, false, 1, 0},
    {"AbstractButton", NULL, false, 0, 1}, {"AbstractButton", NULL, false, 1, 0},
    {"ActivityIndicator", NULL, false, 0, 1}, {"ActivityIndicator", NULL, false, 1, 0},
    {"ProgressBar", "ProgressBar10.qml", false, 0, 1}, {"ProgressBar", "ProgressBar10.qml", false, 1, 0},
    {"TextField", NULL, false, 0, 1}, {"TextField", NULL, false, 1, 0},
    {"TextArea", NULL, false, 0, 1}, {"TextArea", NULL, false, 1, 0},
    {"Switch", NULL, false, 0, 1}, {"Switch", NULL, false, 1, 0},
    {"CheckBox", NULL, false, 0, 1}, {"CheckBox", NULL, false, 1, 0},
    {"Slider", NULL, false, 0, 1}, {"Slider", NULL, false, 1, 0},
    {"Scrollbar", NULL, false, 0, 1}, {"Scrollbar", NULL, false, 1, 0},
    {"Object", NULL, false, 0, 1}, {"Object", NULL, false, 1, 0},
    {"OptionSelector", NULL, false, 0, 1}, {"OptionSelector", NULL, false, 1, 0},
    {"OptionSelectorDelegate", NULL, false, 0, 1}, {"OptionSelectorDelegate", NULL, false, 1, 0},
    {"Page", NULL, false, 0, 1}, {"Page", NULL, false, 1, 0},
    {"PageStack", NULL, false, 0, 1}, {"PageStack", NULL, false, 1, 0},
    {"Header", NULL, false, 0, 1}, {"Header", NULL, false, 1, 0},
    {"CrossFadeImage", "CrossFadeImage10.qml", false, 0, 1}, {"CrossFadeImage", "CrossFadeImage10.qml", false, 1, 0},
    {"Icon", "Icon10.qml", false, 0, 1}, {"Icon", "Icon10.qml", false, 1, 0},
    {"OrientationHelper", NULL, false, 0, 1}, {"OrientationHelper", NULL, false, 1, 0},
    {"UbuntuNumberAnimation", NULL, false, 0, 1}, {"UbuntuNumberAnimation", NULL, false, 1, 0},
    {"UbuntuListView", NULL, false, 0, 1}, {"UbuntuListView", NULL, false, 1, 0},

    // 1.1
    {"ComboButton", NULL, false, 1, 1},
    {"Button", "11/Button.qml", false, 1, 1},
    {"Icon", "Icon11.qml", false, 1, 1},
    {"ProgressBar", "ProgressBar11.qml", false, 1, 1},
    {"CrossFadeImage", "CrossFadeImage11.qml", false, 1, 1},
    {"PullToRefresh", NULL, false, 1, 1},
    {"UbuntuListView", "UbuntuListView11.qml", false, 1, 1},
    {"Page", "Page11.qml", false, 1, 1},
    {"PageHeadConfiguration", NULL, false, 1, 1},
    {"PageHeadSections", NULL, false, 1, 1},
    {"PageHeadState", NULL, false, 1, 1},
    {"StyledItem", NULL, false, 1, 1},

    {"UbuntuColors", "10/UbuntuColors10.qml", true, 0, 1},
    {"UbuntuColors", "10/UbuntuColors10.qml", true, 1, 0},
    {"UbuntuColors", "11/UbuntuColors.qml", true, 1, 1},
};

static const QmlType internalTypes [] = {
    {"DraggingArea", NULL, false, 0, 1},
    {"PageTreeNode", NULL, false, 0, 1},
    {"Toolbar", NULL, false, 0, 1},
    {"AppHeader", NULL, false, 0, 1},
    {"AnimatedItem", NULL, false, 0, 1},
    {"PageWrapper", NULL, false, 0, 1},
    {"TextCursor", NULL, false, 0, 1},
    {"TextInputPopover", NULL, false, 0, 1},
    {"InputHandler", NULL, false, 0, 1},
    {"PageBase", "Page10.qml", false, 0, 1},
};

QUrl UbuntuComponentsPlugin::m_baseUrl = QUrl();

/*
 * Type registration functions.
 */

static QObject *registerClipboard(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)

    QQuickClipboard *clipboard = new QQuickClipboard;
    return clipboard;
}

static QObject *registerUCUbuntuAnimation(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)

    UCUbuntuAnimation *animation = new UCUbuntuAnimation();
    return animation;
}

static QObject *registerUriHandler(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)

    UCUriHandler *uriHandler = new UCUriHandler();
    return uriHandler;
}

void UbuntuComponentsPlugin::registerWindowContextProperty()
{
    setWindowContextProperty(QGuiApplication::focusWindow());

    // listen to QGuiApplication::focusWindowChanged
    /* Ensure that setWindowContextProperty is called in the same thread (the
       main thread) otherwise it segfaults. Reference:
       https://bugs.launchpad.net/ubuntu-ui-toolkit/+bug/1205556
    */
    QGuiApplication* application = static_cast<QGuiApplication*>(QCoreApplication::instance());
    QObject::connect(application, SIGNAL(focusWindowChanged(QWindow*)),
                     this, SLOT(setWindowContextProperty(QWindow*)),
                     Qt::ConnectionType(Qt::DirectConnection | Qt::UniqueConnection));

}

void UbuntuComponentsPlugin::setWindowContextProperty(QWindow* focusWindow)
{
    QQuickView* view = qobject_cast<QQuickView*>(focusWindow);

    if (view != NULL) {
        view->rootContext()->setContextProperty("window", view);
    }
}

QUrl UbuntuComponentsPlugin::fileLocation()
{
    if (m_baseUrl.isEmpty()) {
        m_baseUrl = QUrl(baseUrl().toString() + '/');
    }
    return m_baseUrl;
}

void UbuntuComponentsPlugin::registerQmlTypes(const char *uri, const QmlType *types, uint count)
{
    // register QML file types
    QUrl filePath = fileLocation();
    for (uint i = 0; i < count; i++) {
        int result;
        if (types[i].singleton) {
            if (types[i].file) {
                result = qmlRegisterSingletonType(filePath.resolved(QString(types[i].file)), uri, types[i].major, types[i].minor, types[i].type);
            } else {
                result = qmlRegisterSingletonType(filePath.resolved(QString(types[i].type) + ".qml"), uri, types[i].major, types[i].minor, types[i].type);
            }
        } else {
            if (types[i].file) {
                result = qmlRegisterType(filePath.resolved(QString(types[i].file)), uri, types[i].major, types[i].minor, types[i].type);
            } else {
                result = qmlRegisterType(filePath.resolved(QString(types[i].type) + ".qml"), uri, types[i].major, types[i].minor, types[i].type);
            }
        }
        if (!result) {
            qDebug() << QString("Type '%1' registration failed!").arg(types[i].type);
        }
    }
}

void UbuntuComponentsPlugin::registerTypesToVersion(const char *uri, int major, int minor)
{
    qmlRegisterType<UCAction>(uri, major, minor, "Action");
    qmlRegisterType<UCActionContext>(uri, major, minor, "ActionContext");
    qmlRegisterType<UCActionManager>(uri, major, minor, "ActionManager");
    qmlRegisterType<UCStyledItemBase>(uri, major, minor, "StyledItemBase");
    qmlRegisterUncreatableType<UbuntuI18n>(uri, major, minor, "i18n", "Singleton object");
    qmlRegisterExtendedType<QQuickImageBase, UCQQuickImageExtension>(uri, major, minor, "QQuickImageBase");
    qmlRegisterUncreatableType<UCUnits>(uri, major, minor, "UCUnits", "Not instantiable");
    qmlRegisterType<ShapeItem>(uri, major, minor, "UbuntuShape");
    // FIXME/DEPRECATED: Shape is exported for backwards compatibity only
    qmlRegisterType<ShapeItem>(uri, major, minor, "Shape");
    qmlRegisterType<InverseMouseAreaType>(uri, major, minor, "InverseMouseArea");
    qmlRegisterType<QQuickMimeData>(uri, major, minor, "MimeData");
    qmlRegisterSingletonType<QQuickClipboard>(uri, major, minor, "Clipboard", registerClipboard);
    qmlRegisterSingletonType<UCUbuntuAnimation>(uri, major, minor, "UbuntuAnimation", registerUCUbuntuAnimation);
    qmlRegisterType<UCArguments>(uri, major, minor, "Arguments");
    qmlRegisterType<UCArgument>(uri, major, minor, "Argument");
    qmlRegisterType<QQmlPropertyMap>();
    qmlRegisterType<UCAlarm>(uri, major, minor, "Alarm");
    qmlRegisterType<UCAlarmModel>(uri, major, minor, "AlarmModel");
    qmlRegisterType<UCStateSaver>(uri, major, minor, "StateSaver");
    qmlRegisterType<UCStateSaverAttached>();
    qmlRegisterSingletonType<UCUriHandler>(uri, major, minor, "UriHandler", registerUriHandler);
    qmlRegisterType<UCMouse>(uri, major, minor, "Mouse");
    qmlRegisterType<UCInverseMouse>(uri, major, minor, "InverseMouse");
}

void UbuntuComponentsPlugin::registerTypes(const char *uri)
{
    qDebug() << "REGISTER";
    Q_ASSERT(uri == QLatin1String("Ubuntu.Components"));

    // register 0.1 for backward compatibility
    registerTypesToVersion(uri, 0, 1);
    registerTypesToVersion(uri, 1, 0);

    // register custom event
    ForwardedEvent::registerForwardedEvent();

    // register parent type so that properties can get/ set it
    qmlRegisterUncreatableType<QAbstractItemModel>(uri, 1, 1, "QAbstractItemModel", "Not instantiable");

    // register 1.1 only API
    qmlRegisterType<UCStyledItemBase, 1>(uri, 1, 1, "StyledItemBase");
    qmlRegisterType<QSortFilterProxyModelQML>(uri, 1, 1, "SortFilterModel");
    qmlRegisterUncreatableType<FilterBehavior>(uri, 1, 1, "FilterBehavior", "Not instantiable");
    qmlRegisterUncreatableType<SortBehavior>(uri, 1, 1, "SortBehavior", "Not instantiable");

    registerQmlTypes(uri, qmlTypes, (sizeof(qmlTypes) / sizeof(qmlTypes[0])));
}

void UbuntuComponentsPlugin::initializeEngine(QQmlEngine *engine, const char *uri)
{
    qDebug() << "INIT";
    // initialize baseURL
    if (m_baseUrl.isEmpty()) {
        m_baseUrl = QUrl(baseUrl().toString() + '/');
    }

    // register internal types
    registerQmlTypes("Ubuntu.Components.Internals", internalTypes, sizeof(internalTypes) / sizeof(internalTypes[0]));

    QQmlExtensionPlugin::initializeEngine(engine, uri);
    QQmlContext* context = engine->rootContext();

    // register root object watcher that sets a global property with the root object
    // that can be accessed from any object
    context->setContextProperty("QuickUtils", &QuickUtils::instance());

    UCTheme::instance().registerToContext(context);

    context->setContextProperty("i18n", &UbuntuI18n::instance());
    ContextPropertyChangeListener *i18nChangeListener =
        new ContextPropertyChangeListener(context, "i18n");
    QObject::connect(&UbuntuI18n::instance(), SIGNAL(domainChanged()),
                     i18nChangeListener, SLOT(updateContextProperty()));
    QObject::connect(&UbuntuI18n::instance(), SIGNAL(languageChanged()),
                     i18nChangeListener, SLOT(updateContextProperty()));

    // We can't use 'Application' because it exists (undocumented)
    context->setContextProperty("UbuntuApplication", &UCApplication::instance());
    ContextPropertyChangeListener *applicationChangeListener =
        new ContextPropertyChangeListener(context, "UbuntuApplication");
    QObject::connect(&UCApplication::instance(), SIGNAL(applicationNameChanged()),
                     applicationChangeListener, SLOT(updateContextProperty()));
    // Give the application object access to the engine
    UCApplication::instance().setContext(context);

    context->setContextProperty("units", &UCUnits::instance());
    ContextPropertyChangeListener *unitsChangeListener =
        new ContextPropertyChangeListener(context, "units");
    QObject::connect(&UCUnits::instance(), SIGNAL(gridUnitChanged()),
                     unitsChangeListener, SLOT(updateContextProperty()));

    // register FontUtils
    context->setContextProperty("FontUtils", &UCFontUtils::instance());
    ContextPropertyChangeListener *fontUtilsListener =
        new ContextPropertyChangeListener(context, "FontUtils");
    QObject::connect(&UCUnits::instance(), SIGNAL(gridUnitChanged()),
                     fontUtilsListener, SLOT(updateContextProperty()));

    engine->addImageProvider(QLatin1String("scaling"), new UCScalingImageProvider);

    // register icon provider
    engine->addImageProvider(QLatin1String("theme"), new UnityThemeIconProvider);

    try {
        engine->addImageProvider(QLatin1String("thumbnailer"), new ThumbnailGenerator);
    } catch(std::runtime_error &e) {
        qDebug() << "Could not create thumbnailer: " << e.what();
    }

    // Necessary for Screen.orientation (from import QtQuick.Window 2.0) to work
    QGuiApplication::primaryScreen()->setOrientationUpdateMask(
            Qt::PortraitOrientation |
            Qt::LandscapeOrientation |
            Qt::InvertedPortraitOrientation |
            Qt::InvertedLandscapeOrientation);

    registerWindowContextProperty();
}
