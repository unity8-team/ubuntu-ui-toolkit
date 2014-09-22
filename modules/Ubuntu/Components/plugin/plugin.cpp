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

typedef struct tagQmlType QmlType;
struct tagQmlType {
    const char *type;
    const char *file;
    int major, minor;
};

static const QmlType qmlTypes[] = {
    {"ActionItem", NULL, 0, 1}, {"ActionItem", NULL, 1, 0},
    {"ActionList", NULL, 0, 1}, {"ActionList", NULL, 1, 0},
    {"ToolbarItems", NULL, 0, 1}, {"ToolbarItems", NULL, 1, 0},
    {"ToolbarButton", NULL, 0, 1}, {"ToolbarButton", NULL, 1, 0},
    {"MainView", NULL, 0, 1}, {"MainView", NULL, 1, 0},
    {"Button", "10/Button.qml", 0, 1}, {"Button", "10/Button.qml", 1, 0},
    {"Panel", NULL, 0, 1}, {"Panel", NULL, 1, 0},
    {"Tab", NULL, 0, 1}, {"Tab", NULL, 1, 0},
    {"TabBar", NULL, 0, 1}, {"TabBar", NULL, 1, 0},
    {"Tabs", NULL, 0, 1}, {"Tabs", NULL, 1, 0},
    {"Label", NULL, 0, 1}, {"Label", NULL, 1, 0},
    {"AbstractButton", NULL, 0, 1}, {"AbstractButton", NULL, 1, 0},
    {"ActivityIndicator", NULL, 0, 1}, {"ActivityIndicator", NULL, 1, 0},
    {"ProgressBar", "ProgressBar10.qml", 0, 1}, {"ProgressBar", "ProgressBar10.qml", 1, 0},
    {"TextField", NULL, 0, 1}, {"TextField", NULL, 1, 0},
    {"TextArea", NULL, 0, 1}, {"TextArea", NULL, 1, 0},
    {"Switch", NULL, 0, 1}, {"Switch", NULL, 1, 0},
    {"CheckBox", NULL, 0, 1}, {"CheckBox", NULL, 1, 0},
    {"Slider", NULL, 0, 1}, {"Slider", NULL, 1, 0},
    {"Scrollbar", NULL, 0, 1}, {"Scrollbar", NULL, 1, 0},
    {"Object", NULL, 0, 1}, {"Object", NULL, 1, 0},
    {"OptionSelector", NULL, 0, 1}, {"OptionSelector", NULL, 1, 0},
    {"OptionSelectorDelegate", NULL, 0, 1}, {"OptionSelectorDelegate", NULL, 1, 0},
    {"Page", NULL, 0, 1}, {"Page", NULL, 1, 0},
    {"PageStack", NULL, 0, 1}, {"PageStack", NULL, 1, 0},
    {"Header", NULL, 0, 1}, {"Header", NULL, 1, 0},
    {"CrossFadeImage", NULL, 0, 1}, {"CrossFadeImage", NULL, 1, 0},
    {"Icon", "Icon10.qml", 0, 1}, {"Icon", "Icon10.qml", 1, 0},
    {"OrientationHelper", NULL, 0, 1}, {"OrientationHelper", NULL, 1, 0},
    {"UbuntuNumberAnimation", NULL, 0, 1}, {"UbuntuNumberAnimation", NULL, 1, 0},
    {"UbuntuListView", NULL, 0, 1}, {"UbuntuListView", NULL, 1, 0},

    // 1.1
    {"ComboButton", NULL, 1, 1},
    {"Button", "11/Button.qml", 1, 1},
    {"Icon", "Icon11.qml", 1, 1},
    {"ProgressBar", "ProgressBar11.qml", 1, 1},
    {"CrossFadeImage", "CrossFadeImage11.qml", 1, 1},
    {"PullToRefresh", NULL, 1, 1},
    {"UbuntuListView", "UbuntuListView11.qml", 1, 1},
    {"Page", "Page11.qml", 1, 1},
    {"PageHeadConfiguration", NULL, 1, 1},
    {"PageHeadSections", NULL, 1, 1},
    {"PageHeadState", NULL, 1, 1},
    {"StyledItem", NULL, 1, 1},
};

// JS files
static const QmlType jsTypes [] = {
    {"MathUtils", "mathUtils.js", 0, 1}, //{"MathUtils", "mathUtils.js", 1, 0},
    {"SliderUtils", "sliderUtils.js", 0, 1}, //{"SliderUtils", "sliderUtils.js", 1, 0},
    {"ScrollbarUtils", "scrollbarUtils.js", 0, 1}, //{"ScrollbarUtils", "scrollbarUtils.js", 1, 0},
    {"ColorUtils", "colorUtils.js", 0, 1}, //{"ColorUtils", "colorUtils.js", 1, 0},
    {"DateUtils", "dateUtils.js", 0, 1}, //{"DateUtils", "dateUtils.js", 1, 0},
};

static const QmlType singletonQmlTypes[] = {
    {"UbuntuColors", "10/UbuntuColors10.qml", 0, 1},
    {"UbuntuColors", "10/UbuntuColors10.qml", 1, 0},
    {"UbuntuColors", "11/UbuntuColors.qml", 1, 1},
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

    // register QML file types
    QUrl filePath = fileLocation();
    for (uint i = 0; i < (sizeof(qmlTypes) / sizeof(qmlTypes[0])); i++) {
        int result;
        if (qmlTypes[i].file) {
            result = qmlRegisterType(filePath.resolved(QString(qmlTypes[i].file)), uri, qmlTypes[i].major, qmlTypes[i].minor, qmlTypes[i].type);
        } else {
            result = qmlRegisterType(filePath.resolved(QString(qmlTypes[i].type) + ".qml"), uri, qmlTypes[i].major, qmlTypes[i].minor, qmlTypes[i].type);
        }
        if (!result) {
            qDebug() << QString("Type '%1' registration failed!").arg(qmlTypes[i].type);
        }
    }
    // register singletons
    for (uint i = 0; i < (sizeof(singletonQmlTypes) / sizeof(singletonQmlTypes[0])); i++) {
        int result;
        if (singletonQmlTypes[i].file) {
            result = qmlRegisterSingletonType(filePath.resolved(QString(singletonQmlTypes[i].file)), uri, singletonQmlTypes[i].major, singletonQmlTypes[i].minor, singletonQmlTypes[i].type);
        } else {
            result = qmlRegisterSingletonType(filePath.resolved(QString(singletonQmlTypes[i].type) + ".qml"), uri, singletonQmlTypes[i].major, singletonQmlTypes[i].minor, singletonQmlTypes[i].type);
        }
        if (!result) {
            qDebug() << QString("Singleton '%1' registration failed!").arg(singletonQmlTypes[i].type);
        }
    }
}

void UbuntuComponentsPlugin::initializeEngine(QQmlEngine *engine, const char *uri)
{
    // initialize baseURL
    if (m_baseUrl.isEmpty()) {
        m_baseUrl = QUrl(baseUrl().toString() + '/');
    }

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
