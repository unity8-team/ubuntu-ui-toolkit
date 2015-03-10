/*
 * Copyright 2013-2015 Canonical Ltd.
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
 * Authors: Zsombor Egri <zsombor.egri@canonical.com>
 *          Florian Boucault <florian.boucault@canonical.com>
 */

#include "ucstyleset.h"
#include "listener.h"
#include "quickutils.h"
#include "i18n.h"
#include "ucfontutils.h"
#include "ucstyleditembase.h"

#include <QtQml/qqml.h>
#include <QtQml/qqmlinfo.h>
#include <QtQml/QQmlEngine>
#include <QtQml/QQmlContext>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <QtCore/QLibraryInfo>
#include <QtCore/QStandardPaths>
#include <QtGui/QGuiApplication>
#include <QtGui/QFont>

/*!
    \qmltype StyleSet
    \instantiates UCStyleSet
    \inqmlmodule Ubuntu.Components 1.3
    \since Ubuntu.Components 1.3
    \ingroup theming
    \brief The StyleSet class provides facilities to define the styleset of a
    StyledItem.

    A global instance, which is the default styleset, is exposed as the \b styleSet
    context property.

    The styleset or theme defines the visual aspect of the Ubuntu components. An
    application can use one or more styleset the same time. The StyleSet component
    provides abilities to change thye styleset used by the component and all its
    child components.

    Changing the styleset of the entire application can be achieved by changing
    the name of the root StyledItem's, i.e. MainView's current styleset.

    \qml
    import QtQuick 2.4
    import Ubuntu.Components 1.3

    MainWindow {
        width: units.gu(40)
        height: units.gu(71)

        styleSet.name: "Ubuntu.Components.Themes.Ambiance"
    }
    \endqml

    Example creating a style component:

    \qml
    import QtQuick 2.4
    import Ubuntu.Components 1.3
    StyledItem {
        id: myItem
        style: styleSet.createStyleComponent("MyItemStyle.qml", myItem)
    }
    \endqml

    \sa {StyledItem}
*/

const QString THEME_FOLDER_FORMAT("%1/%2/");
const QString PARENT_THEME_FILE("parent_theme");

QStringList themeSearchPath() {
    QString envPath = QLatin1String(getenv("UBUNTU_UI_TOOLKIT_THEMES_PATH"));
    QStringList pathList = envPath.split(':', QString::SkipEmptyParts);
    if (pathList.isEmpty()) {
        // get the default path list from generic data location, which contains
        // XDG_DATA_DIRS
        QString xdgDirs = QLatin1String(getenv("XDG_DATA_DIRS"));
        if (!xdgDirs.isEmpty()) {
            pathList << xdgDirs.split(':', QString::SkipEmptyParts);
        }
        // ~/.local/share
        pathList << QStandardPaths::standardLocations(QStandardPaths::GenericDataLocation);
    }

    // append QML import path(s); we must explicitly support env override here
    QString qml2ImportPath(getenv("QML2_IMPORT_PATH"));
    if (!qml2ImportPath.isEmpty()) {
        pathList << qml2ImportPath.split(':', QString::SkipEmptyParts);
    }
    pathList << QLibraryInfo::location(QLibraryInfo::Qml2ImportsPath).split(':', QString::SkipEmptyParts);

    // fix folders
    QStringList result;
    Q_FOREACH(const QString &path, pathList) {
        if (QDir(path).exists()) {
            result << path + '/';
        }
    }
    // prepend current folder
    result.prepend(QDir::currentPath());
    return result;
}

QUrl pathFromThemeName(QString themeName)
{
    themeName.replace('.', '/');
    QStringList pathList = themeSearchPath();
    Q_FOREACH(const QString &path, pathList) {
        QString themeFolder = THEME_FOLDER_FORMAT.arg(path, themeName);
        // QUrl needs a trailing slash to understand it's a directory
        QString absoluteThemeFolder = QDir(themeFolder).absolutePath().append('/');
        if (QDir(absoluteThemeFolder).exists()) {
            return QUrl::fromLocalFile(absoluteThemeFolder);
        }
    }
    return QUrl();
}

UCStyleSet::UCStyleSet(QObject *parent)
    : QObject(parent)
    , m_name(UCStyleSet::defaultSet().m_name)
    , m_palette(UCStyleSet::defaultSet().m_palette)
    , m_engine(UCStyleSet::defaultSet().m_engine)
    , m_defaultStyle(false)
{
    init();
}

UCStyleSet::UCStyleSet(bool defaultStyle)
    : QObject(0)
    , m_palette(NULL)
    , m_engine(NULL)
    , m_defaultStyle(defaultStyle)
{
    init();
    // set the default font
    QFont defaultFont;
    defaultFont.setFamily("Ubuntu");
    defaultFont.setPixelSize(UCFontUtils::instance().sizeToPixels("medium"));
    defaultFont.setWeight(QFont::Light);
    QGuiApplication::setFont(defaultFont);
}

void UCStyleSet::init()
{
    m_completed = false;
    m_name = m_themeSettings.themeName();
    QObject::connect(&m_themeSettings, &UCThemeSettings::themeNameChanged,
                     this, &UCStyleSet::onThemeNameChanged);
    updateThemePaths();
}

void UCStyleSet::classBegin()
{
    m_engine = qmlEngine(this);
    updateEnginePaths();
}

void UCStyleSet::updateEnginePaths()
{
    if (!m_engine) {
        return;
    }

    QStringList paths = themeSearchPath();
    Q_FOREACH(const QString &path, paths) {
        if (QDir(path).exists() && !m_engine->importPathList().contains(path)) {
            m_engine->addImportPath(path);
        }
    }
}

void UCStyleSet::onThemeNameChanged()
{
    if (m_themeSettings.themeName() != m_name) {
        m_name = m_themeSettings.themeName();
        updateThemePaths();
        Q_EMIT nameChanged();
    }
}

void UCStyleSet::updateThemePaths()
{
    m_themePaths.clear();

    QString themeName = m_name;
    while (!themeName.isEmpty()) {
        QUrl themePath = pathFromThemeName(themeName);
        if (themePath.isValid()) {
            m_themePaths.append(themePath);
        }
        themeName = parentThemeName(themeName);
    }
}

/*!
    \qmlproperty string StyleSet::name

    The name of the current theme. The name can be set only at creation time, runtime
    changes will be omitted.

    \qml
    import QtQuick 2.4
    import Ubuntu.Componenst 1.3

    StyledItem {
        style: StyleSet {
            // this is right
            name: "Ubuntu.Components.Themes.Ambiance"
        }
        // this is not allowed, and will be omitted
        Components.onCompleted: styleSet.name = "Ubuntu.Components.Themes.SuruDark"
    }
    \endqml
*/
QString UCStyleSet::name() const
{
    return m_name;
}
void UCStyleSet::setName(const QString& name)
{
    if (name == m_name) {
        return;
    }
    if (name.isEmpty()) {
        init();
    } else {
        QObject::disconnect(&m_themeSettings, &UCThemeSettings::themeNameChanged,
                            this, &UCStyleSet::onThemeNameChanged);
        m_name = name;
        updateThemePaths();
    }
    updateEnginePaths();
    Q_EMIT nameChanged();
    loadPalette();
}
void UCStyleSet::resetName()
{
    setName(QString());
}

/*!
    \qmlproperty Palette StyleSet::palette

    The palette of the current theme.
*/
QObject* UCStyleSet::palette()
{
    if (!m_palette) {
        loadPalette(false);
    }
    return m_palette;
}
void UCStyleSet::setPalette(QObject *palette)
{
    if (m_palette == palette) {
        return;
    }
    m_palette = palette;
    Q_EMIT paletteChanged();
}

/*!
 * \qmlproperty list<PaletteChanges> StyleSet::paletteChanges
 * \default
 * List of PaletteChanges to be applied on each styleset.
 */
QQmlListProperty<UCPaletteChanges> UCStyleSet::paletteChanges()
{
    return QQmlListProperty<UCPaletteChanges>(this, m_paletteChanges);
}

QUrl UCStyleSet::styleUrl(const QString& styleName)
{
    Q_FOREACH (const QUrl& themePath, m_themePaths) {
        QUrl styleUrl = themePath.resolved(styleName);
        if (styleUrl.isValid() && QFile::exists(styleUrl.toLocalFile())) {
            return styleUrl;
        }
    }

    return QUrl();
}

QString UCStyleSet::parentThemeName(const QString& themeName)
{
    QString parentTheme;
    QUrl themePath = pathFromThemeName(themeName);
    if (!themePath.isValid()) {
        qWarning() << qPrintable(UbuntuI18n::instance().tr("Theme not found: \"%1\"").arg(themeName));
    } else {
        QFile file(themePath.resolved(PARENT_THEME_FILE).toLocalFile());
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&file);
            parentTheme = in.readLine();
        }
    }
    return parentTheme;
}

// registers the default styleSet property to the root context
void UCStyleSet::registerToContext(QQmlContext* context)
{
    UCStyleSet *defaultSet = &UCStyleSet::defaultSet();
    defaultSet->m_engine = context->engine();
    defaultSet->updateEnginePaths();

    context->setContextProperty("styleSet", defaultSet);
    ContextPropertyChangeListener *listener =
        new ContextPropertyChangeListener(context, "styleSet");
    QObject::connect(defaultSet, &UCStyleSet::nameChanged,
                     listener, &ContextPropertyChangeListener::updateContextProperty);
}

/*!
    \qmlmethod Component StyleSet::createStyleComponent(string styleName, object parent)

    Returns an instance of the style component named \a styleName.
*/
QQmlComponent* UCStyleSet::createStyleComponent(const QString& styleName, QObject* parent)
{
    QQmlComponent *component = NULL;

    if (parent != NULL) {
        QQmlEngine* engine = qmlEngine(parent);
        if (engine != m_engine && !m_engine) {
            m_engine = engine;
            updateEnginePaths();
        }
        // make sure we have the paths
        if (engine != NULL) {
            QUrl url = styleUrl(styleName);
            if (url.isValid()) {
                component = new QQmlComponent(engine, url, QQmlComponent::PreferSynchronous, parent);
                if (component->isError()) {
                    qmlInfo(parent) << component->errorString();
                    delete component;
                    component = NULL;
                }
            } else {
                qmlInfo(parent) <<
                   UbuntuI18n::instance().tr(QString("Warning: Style %1 not found in theme %2").arg(styleName).arg(m_name));
            }
        }
    }

    return component;
}

void UCStyleSet::loadPalette(bool notify)
{
    if (!m_engine) {
        return;
    }
    if (!m_palette.isNull()) {
        delete m_palette;
    }
    // theme may not have palette defined
    QUrl paletteUrl = styleUrl("Palette.qml");
    if (paletteUrl.isValid()) {
        m_palette = QuickUtils::instance().createQmlObject(paletteUrl, m_engine);
        if (notify) {
            Q_EMIT paletteChanged();
        }
    } else {
        // use the default palette if none defined
        m_palette = defaultSet().m_palette;
    }
}