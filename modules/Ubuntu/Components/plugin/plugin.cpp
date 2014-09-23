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

static void initResources()
{
    Q_INIT_RESOURCE(resources);
}

static const QmlType qmlTypes[] = {
    {"ActionItem", "10/ActionItem.qml", false, 0, 1}, {"ActionItem", "10/ActionItem.qml", false, 1, 0},
    {"ActionList", "10/ActionList.qml", false, 0, 1}, {"ActionList", "10/ActionList.qml", false, 1, 0},
    {"ToolbarItems", "10/ToolbarItems.qml", false, 0, 1}, {"ToolbarItems", "10/ToolbarItems.qml", false, 1, 0},
    {"ToolbarButton", "10/ToolbarButton.qml", false, 0, 1}, {"ToolbarButton", "10/ToolbarButton.qml", false, 1, 0},
    {"MainView", "10/MainView.qml", false, 0, 1}, {"MainView", "10/MainView.qml", false, 1, 0},
    {"Button", "10/Button.qml", false, 0, 1}, {"Button", "10/Button.qml", false, 1, 0},
    {"Panel", "10/Panel.qml", false, 0, 1}, {"Panel", "10/Panel.qml", false, 1, 0},
    {"Tab", "10/Tab.qml", false, 0, 1}, {"Tab", "10/Tab.qml", false, 1, 0},
    {"TabBar", "10/TabBar.qml", false, 0, 1}, {"TabBar", "10/TabBar.qml", false, 1, 0},
    {"Tabs", "10/Tabs.qml", false, 0, 1}, {"Tabs", "10/Tabs.qml", false, 1, 0},
    {"Label", "10/Label.qml", false, 0, 1}, {"Label", "10/Label.qml", false, 1, 0},
    {"AbstractButton", "10/AbstractButton.qml", false, 0, 1}, {"AbstractButton", "10/AbstractButton.qml", false, 1, 0},
    {"ActivityIndicator", "10/ActivityIndicator.qml", false, 0, 1}, {"ActivityIndicator", "10/ActivityIndicator.qml", false, 1, 0},
    {"ProgressBar", "10/ProgressBar.qml", false, 0, 1}, {"ProgressBar", "10/ProgressBar.qml", false, 1, 0},
    {"TextField", "10/TextField.qml", false, 0, 1}, {"TextField", "10/TextField.qml", false, 1, 0},
    {"TextArea", "10/TextArea.qml", false, 0, 1}, {"TextArea", "10/TextArea.qml", false, 1, 0},
    {"Switch", "10/Switch.qml", false, 0, 1}, {"Switch", "10/Switch.qml", false, 1, 0},
    {"CheckBox", "10/CheckBox.qml", false, 0, 1}, {"CheckBox", "10/CheckBox.qml", false, 1, 0},
    {"Slider", "10/Slider.qml", false, 0, 1}, {"Slider", "10/Slider.qml", false, 1, 0},
    {"Scrollbar", "10/Scrollbar.qml", false, 0, 1}, {"Scrollbar", "10/Scrollbar.qml", false, 1, 0},
    {"Object", "10/Object.qml", false, 0, 1}, {"Object", "10/Object.qml", false, 1, 0},
    {"OptionSelector", "10/OptionSelector.qml", false, 0, 1}, {"OptionSelector", "10/OptionSelector.qml", false, 1, 0},
    {"OptionSelectorDelegate", "10/OptionSelectorDelegate.qml", false, 0, 1}, {"OptionSelectorDelegate", "10/OptionSelectorDelegate.qml", false, 1, 0},
    {"Page", "10/Page.qml", false, 0, 1}, {"Page", "10/Page.qml", false, 1, 0},
    {"PageStack", "10/PageStack.qml", false, 0, 1}, {"PageStack", "10/PageStack.qml", false, 1, 0},
    {"Header", "10/Header.qml", false, 0, 1}, {"Header", "10/Header.qml", false, 1, 0},
    {"CrossFadeImage", "10/CrossFadeImage.qml", false, 0, 1}, {"CrossFadeImage", "10/CrossFadeImage.qml", false, 1, 0},
    {"Icon", "10/Icon.qml", false, 0, 1}, {"Icon", "10/Icon.qml", false, 1, 0},
    {"OrientationHelper", "10/OrientationHelper.qml", false, 0, 1}, {"OrientationHelper", "10/OrientationHelper.qml", false, 1, 0},
    {"UbuntuNumberAnimation", "10/UbuntuNumberAnimation.qml", false, 0, 1}, {"UbuntuNumberAnimation", "10/UbuntuNumberAnimation.qml", false, 1, 0},
    {"UbuntuListView", "10/UbuntuListView.qml", false, 0, 1}, {"UbuntuListView", "10/UbuntuListView.qml", false, 1, 0},

    // 1.1
    {"ComboButton", "11/ComboButton.qml", false, 1, 1},
    {"Button", "11/Button.qml", false, 1, 1},
    {"Icon", "11/Icon.qml", false, 1, 1},
    {"ProgressBar", "11/ProgressBar.qml", false, 1, 1},
    {"CrossFadeImage", "11/CrossFadeImage.qml", false, 1, 1},
    {"PullToRefresh", "11/PullToRefresh.qml", false, 1, 1},
    {"UbuntuListView", "11/UbuntuListView.qml", false, 1, 1},
    {"Page", "11/Page.qml", false, 1, 1},
    {"PageHeadConfiguration", "10/PageHeadConfiguration.qml", false, 1, 1},
    {"PageHeadSections", "10/PageHeadSections.qml", false, 1, 1},
    {"PageHeadState", "10/PageHeadState.qml", false, 1, 1},
    {"StyledItem", "10/StyledItem.qml", false, 1, 1},

    {"UbuntuColors", "10/UbuntuColors.qml", true, 0, 1},
    {"UbuntuColors", "10/UbuntuColors.qml", true, 1, 0},
    {"UbuntuColors", "11/UbuntuColors.qml", true, 1, 1},

    {"PageBase", "10/Page.qml", false, 1, 1},

};

static const QmlType internalTypes [] = {
    {"DraggingArea", "Internals/DraggingArea.qml", false, 0, 1},
    {"PageTreeNode", "Internals/PageTreeNode.qml", false, 0, 1},
    {"Toolbar", "Internals/Toolbar.qml", false, 0, 1},
    {"AppHeader", "Internals/AppHeader.qml", false, 0, 1},
    {"AnimatedItem", "Internals/AnimatedItem.qml", false, 0, 1},
    {"PageWrapper", "Internals/PageWrapper.qml", false, 0, 1},
    {"TextCursor", "Internals/TextCursor.qml", false, 0, 1},
    {"TextInputPopover", "Internals/TextInputPopover.qml", false, 0, 1},
    {"InputHandler", "Internals/InputHandler.qml", false, 0, 1},
    {"PageBase", "10/Page.qml", false, 0, 1},
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
    return loadedFromResources() ? QUrl("qrc:/Ubuntu/Components/") : m_baseUrl;
}

bool UbuntuComponentsPlugin::loadedFromResources()
{
    QString fname = m_baseUrl.resolved(QString("10/MainView.qml")).toString(QUrl::RemoveScheme);
    QFile mainView(fname);
    qDebug() << "FROMFS?" << baseUrl() << fname << mainView.exists();
    return !mainView.exists();
}


void UbuntuComponentsPlugin::registerQmlTypes(const char *uri, const QmlType *types, uint count)
{
    // register QML file types
    QUrl filePath = fileLocation();
    for (uint i = 0; i < count; i++) {
        int result;
        QUrl file = (types[i].file) ? filePath.resolved(QString(types[i].file)) : filePath.resolved(QString(types[i].type) + ".qml");
        if (types[i].singleton) {
            result = qmlRegisterSingletonType(file, uri, types[i].major, types[i].minor, types[i].type);
        } else {
            result = qmlRegisterType(file, uri, types[i].major, types[i].minor, types[i].type);
        }
        if (!result) {
            qCritical() << QString("Type '%1' registration failed!").arg(types[i].type);
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
    // initialize baseURL
    m_baseUrl = QUrl(baseUrl().toString() + '/');

    Q_ASSERT(uri == QLatin1String("Ubuntu.Components"));

    initResources();

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
    // register internal types
//    registerQmlTypes("Ubuntu.Components.Internals", internalTypes, sizeof(internalTypes) / sizeof(internalTypes[0]));

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

    if (loadedFromResources()) {
        engine->addImportPath(QStringLiteral("qrc:/"));
    }
}
