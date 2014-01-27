/*
 * Copyright 2013 Canonical Ltd.
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

#include "uctheme.h"
#include "listener.h"
#include "quickutils.h"
#include "i18n.h"

#include <QtQml/qqml.h>
#include <QtQml/qqmlinfo.h>
#include <QtQml/QQmlEngine>
#include <QtQml/QQmlContext>
#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <QtCore/QLibraryInfo>
#include <QtCore/QStandardPaths>
#include <QtQml/private/qqmlproperty_p.h>
#include <QtQml/private/qqmlabstractbinding_p.h>
/*!
    \qmltype Theme
    \instantiates UCTheme
    \inqmlmodule Ubuntu.Components 0.1
    \ingroup theming
    \brief The Theme class provides facilities to interact with the current theme.

    A global instance is exposed as the \b Theme context property.

    The theme defines the visual aspect of the Ubuntu components.

    Example changing the current theme:

    \qml
    import QtQuick 2.0
    import Ubuntu.Components 0.1

    Item {
        Button {
            onClicked: Theme.name = "Ubuntu.Components.Themes.Ambiance"
        }
    }
    \endqml

    Example creating a style component:

    \qml
    import QtQuick 2.0
    import Ubuntu.Components 0.1

    StyledItem {
        id: myItem
        style: Theme.createStyleComponent(Theme.name, "MyItemStyle.qml", myItem)
    }
    \endqml

    \sa {StyledItem}
*/

const QString THEME_FOLDER_FORMAT("%1/%2/");
const QString PARENT_THEME_FILE("parent_theme");
const char *ENV_PATH = "UBUNTU_UI_TOOLKIT_THEMES_PATH";

UCTheme *UCTheme::m_themeEngine = 0;

QStringList themeSearchPath() {
    QString envPath = QLatin1String(getenv("UBUNTU_UI_TOOLKIT_THEMES_PATH"));
    QStringList pathList = envPath.split(':', QString::SkipEmptyParts);
    if (pathList.isEmpty()) {
        // get the default path list from generic data location, which contains
        // ~/.local/share and XDG_DATA_DIRS
        pathList << QStandardPaths::standardLocations(QStandardPaths::GenericDataLocation);
    }
    // fix folders
    QStringList result;
    Q_FOREACH(const QString &path, pathList) {
        if (QDir(path).exists()) {
            result << path + '/';
        }
    }
    // append standard QML2_IMPORT_PATH value
    result << QLibraryInfo::location(QLibraryInfo::Qml2ImportsPath);
    return result;
}

UCTheme::UCTheme(QObject *parent) :
    QObject(0),
    m_palette(NULL),
    m_engine(static_cast<QQmlEngine*>(parent))
{
    m_themeEngine = this;
    m_name = m_themeSettings.themeName();
    QObject::connect(&m_themeSettings, &UCThemeSettings::themeNameChanged,
                     this, &UCTheme::onThemeNameChanged);
    updateThemePaths();
    loadPalette();

    QObject::connect(this, &UCTheme::nameChanged,
                     this, &UCTheme::loadPalette, Qt::UniqueConnection);
}

UCTheme::~UCTheme()
{
    m_themeEngine = 0;
}

void UCTheme::updateEnginePaths()
{
    if (!m_engine) {
        return;
    }

    QStringList paths = themeSearchPath();
    Q_FOREACH(const QString &path, paths) {
        if (QDir(path).exists()) {
            m_engine->addImportPath(path);
        }
    }
}

void UCTheme::onThemeNameChanged()
{
    if (m_themeSettings.themeName() != m_name) {
        m_name = m_themeSettings.themeName();
        updateThemePaths();
        Q_EMIT nameChanged();
    }
}

QUrl UCTheme::pathFromThemeName(QString themeName)
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

void UCTheme::updateThemePaths()
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
    \qmlproperty string Theme::name

    The name of the current theme.
*/
QString UCTheme::name() const
{
    return m_name;
}

void UCTheme::setName(const QString& name)
{
    if (name != m_name) {
        QObject::disconnect(&m_themeSettings, &UCThemeSettings::themeNameChanged,
                            this, &UCTheme::onThemeNameChanged);
        m_name = name;
        updateThemePaths();
        Q_EMIT nameChanged();
    }
}

/*!
    \qmlproperty Palette Theme::palette

    The palette of the current theme.
*/
QObject* UCTheme::palette() const
{
    return m_palette;
}

QUrl UCTheme::styleUrl(const QString& styleName)
{
    Q_FOREACH (const QUrl& themePath, m_themePaths) {
        QUrl styleUrl = themePath.resolved(styleName);
        if (styleUrl.isValid() && QFile::exists(styleUrl.toLocalFile())) {
            return styleUrl;
        }
    }

    return QUrl();
}

QString UCTheme::parentThemeName(const QString& themeName)
{
    QString parentTheme;
    QUrl themePath = pathFromThemeName(themeName);
    if (!themePath.isValid()) {
        qWarning() << UbuntuI18n::instance().tr("Theme not found: ") << themeName;
    } else {
        QFile file(themePath.resolved(PARENT_THEME_FILE).toLocalFile());
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&file);
            parentTheme = in.readLine();
        }
    }
    return parentTheme;
}

/*!
    \qmlmethod Component Theme::createStyleComponent(string styleName, object parent)

    Returns an instance of the style component named \a styleName.
*/
QQmlComponent* UCTheme::createStyleComponent(const QString& styleName, QObject* parent)
{
    qmlInfo(parent) << "Deprecated, use createStyleComponent(themeName, styleName, parent)";
    return createStyleComponent(m_name, styleName, parent);
}

/*!
    \qmlmethod Component Theme::createStyleComponent(string themeName, string styleName, object parent)

    Returns an instance of the style component named \a styleName from \a themeName.
*/
QQmlComponent* UCTheme::createStyleComponent(const QString &themeName, const QString &styleName, QObject *parent)
{
    QQmlComponent *style = 0;
    if (!(themeName.isEmpty() || themeName != m_name) && (parent != NULL)) {
        QQmlEngine *engine = qmlEngine(parent);
        if (!engine || ((engine != m_engine) && !m_engine)) {
            qmlInfo(parent) <<
               UbuntuI18n::instance().tr(QString("ERROR: %1 component to be created in a different QQmlEngine instance.").arg(styleName));
            return NULL;
        }
        QUrl url = styleUrl(styleName);
        if (url.isValid()) {
            style = new QQmlComponent(engine, url, QQmlComponent::PreferSynchronous, parent);
            if (style->isError()) {
                qmlInfo(parent) << style->errorString();
                delete style;
                style = NULL;
            }
        } else {
            qmlInfo(parent) <<
               UbuntuI18n::instance().tr(QString("Warning: Style %1 not found in theme %2").arg(styleName).arg(m_name));
        }
    }
    return style;
}

void UCTheme::loadPalette()
{
    if (!m_engine || m_themePaths.isEmpty()) {
        return;
    }
    if (m_palette != NULL) {
        delete m_palette;
    }
    m_palette = QuickUtils::instance().createQmlObject(styleUrl("Palette.qml"), m_engine);
    Q_EMIT paletteChanged();
}
