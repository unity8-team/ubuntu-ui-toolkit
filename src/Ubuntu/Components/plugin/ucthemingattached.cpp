/*
 * Copyright 2015 Canonical Ltd.
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
 * Author: Zsombor Egri <zsombor.egri@canonical.com>
 */

#include "ucthemingattached.h"
#include <QtQuick/QQuickItem>
#include "uctheme.h"

/*
 * UCThemingAttached is an attached object to all themable components. It is used
 * internally by those components which need theme access thru the theme property.
 * Components which want to use this should do the following:
 * - attach the object in classBegin() function
 * - expose a theme property
 * - getter should use the getTheme() attached function
 * - setter/reset should use the setTheme() attached function
 * - themeChanged() signal must be connected to the property change signal
 *
 * The theme change is notified through listener callbacks. Registering listener
 * is not mandatory.
 */

UCThemingAttached *itemTheming(QQuickItem *item)
{
    return qobject_cast<UCThemingAttached*>(
                qmlAttachedPropertiesObject<UCThemingAttached>(item, false));
}

UCThemingAttached::UCThemingAttached(QObject *parent)
    : QObject(parent)
    , m_theme(Q_NULLPTR)
    , m_listener(Q_NULLPTR)
    , m_ownerItem(static_cast<QQuickItem*>(parent))
{
}

UCThemingAttached *UCThemingAttached::qmlAttachedProperties(QObject *owner)
{
    UCThemingAttached *theming = new UCThemingAttached(owner);
    // the theme is the default one at this stage, so connect all the necessary signals
    // owner must have theme signal
    theming->connectThemeSignals(theming->getTheme(), true);
    return theming;
}

void UCThemingAttached::connectThemeSignals(UCTheme *theme, bool connect)
{
    if (connect) {
        QObject::connect(theme, SIGNAL(nameChanged()), this, SIGNAL(themeChanged()), Qt::DirectConnection);
        QObject::connect(theme, SIGNAL(versionChanged()), this, SIGNAL(themeChanged()), Qt::DirectConnection);
    } else {
        QObject::disconnect(theme, SIGNAL(nameChanged()), this, SIGNAL(themeChanged()));
        QObject::disconnect(theme, SIGNAL(versionChanged()), this, SIGNAL(themeChanged()));
    }
}

// link/unlink all ascendant items until we reach a StyledItem, returns true if the
// theme change signal emission is justified
bool UCThemingAttached::connectParents(QQuickItem *fromItem)
{
    QQuickItem *item = fromItem ? fromItem : m_ownerItem->parentItem();
    while (item) {
        // push the item onto the stack
        m_parentStack.push(QPointer<QQuickItem>(item));
        UCThemingAttached *styling = itemTheming(item);
        if (styling) {
            // this is the closest StyledItem, connect its themeChanged() signal
            QObject::connect(styling, SIGNAL(themeChanged()),
                             this, SLOT(parentStyleChanged()), Qt::DirectConnection);
            // set the current style set to the one in the parent's one if differs
            return setParentStyled(styling);
        } else {
            // connect to the item's parentChanged() signal so we can detect when the parent changes
            QObject::connect(item, SIGNAL(parentChanged(QQuickItem*)),
                             this, SLOT(ascendantChanged(QQuickItem*)), Qt::DirectConnection);
        }
        item = item->parentItem();
    }
    return false;
}

// disconnect parent stack till item is reached; all the stack if item == 0
void UCThemingAttached::disconnectTillItem(QQuickItem *item)
{
    while (!m_parentStack.isEmpty() && item != m_parentStack.top()) {
        QPointer<QQuickItem> stackItem = m_parentStack.pop();
        // the topmost item can be the only one which is a StyledItem
        UCThemingAttached *styling = itemTheming(stackItem.data());
        if (styling) {
            QObject::disconnect(styling, SIGNAL(themeChanged()),
                                this, SLOT(parentStyleChanged()));
            // clear parent styling as well
            if (styling == m_parentTheming) {
                m_parentTheming.clear();
            }
        } else if (!stackItem.isNull()) {
            QObject::disconnect(stackItem.data(), SIGNAL(parentChanged(QQuickItem*)),
                                this, SLOT(ascendantChanged(QQuickItem*)));
        }
    }
}

// set the used parent styled item's style; returns true if the parent styled got changed
bool UCThemingAttached::setParentStyled(UCThemingAttached *newStyling)
{
    if (m_parentTheming == newStyling) {
        return false;
    }
    m_parentTheming = newStyling;
    if (m_theme) {
        Q_EMIT themeChanged();
    }
    return (m_theme == NULL);
}

UCTheme *UCThemingAttached::getTheme()
{
    if (m_theme) {
        return m_theme;
    } else if (!m_parentTheming.isNull()) {
        return m_parentTheming->getTheme();
    }
    return &UCTheme::defaultTheme();
}

bool UCThemingAttached::setTheme(UCTheme *newTheme)
{
    if (m_theme == newTheme) {
        return false;
    }

    // preform pre-theme change tasks
    if (m_listener) {
        m_listener->preThemeChanged();
    }

    // disconnect from the previous set
    UCTheme *connectedSet = m_theme ?
                                m_theme :
                                (!m_parentTheming ? &UCTheme::defaultTheme() : NULL);
    if (connectedSet) {
        connectThemeSignals(connectedSet, false);
    }

    UCTheme *prevSet = m_theme;

    // resolve new theme
    if (m_theme && newTheme) {
        // no need to redo the parentStack, simply set the theme and leave
        m_theme = newTheme;
    } else {
        m_theme = newTheme;
        if (!newTheme) {
            // redo the parent chanin
            disconnectTillItem(0);
            connectParents(0);
        }
    }

    // connect to the new set
    connectedSet = m_theme ?
                    m_theme :
                    (!m_parentTheming ? &UCTheme::defaultTheme() : NULL);
    if (connectedSet) {
        connectThemeSignals(connectedSet, true);
    }
    // detach previous set and attach the new one
    if (prevSet) {
        Q_EMIT prevSet->parentThemeChanged();
    }
    if (m_theme) {
        // re-parent theme to make sure we have it
        // for the entire lifetime of the styled item
        m_theme->setParent(parent());
        Q_EMIT m_theme->parentThemeChanged();
    }

    // perform post-theme changes, update internal styling
    if (m_listener) {
        m_listener->postThemeChanged();
    }

    Q_EMIT themeChanged();
    return true;
}

// lookup for the parent chain theme
void UCThemingAttached::itemParentChanged()
{
    // clean stack
    disconnectTillItem(0);
    // make sure we reset parent StyledItem
    m_parentTheming.clear();
    // build the stack - if possible
    connectParents(0);
    Q_EMIT themeChanged();
}

// captures ascendant change signal, the sender is the one which counts!
void UCThemingAttached::ascendantChanged(QQuickItem *ascendant)
{
    QQuickItem *sender = static_cast<QQuickItem*>(this->sender());
    if (!sender) {
        // cannot detect the sender, leave!
        return;
    }
    if (ascendant) {
        // disconnect from the previous ones
        disconnectTillItem(sender);
        m_parentTheming.clear();
        // traverse ascendants till we reach a StyledItem or root and push them into the stack
        if (connectParents(ascendant)) {
            Q_EMIT themeChanged();
        }
    }
}

// syncs the ascendant styled item's styles
void UCThemingAttached::parentStyleChanged()
{
    // do not trigger themeChanged() on this item if we have a
    // custom one, but resolve its eventual parent change!
    if (m_theme) {
        Q_EMIT m_theme->parentThemeChanged();
        return;
    }

    UCThemingAttached *styling = static_cast<UCThemingAttached*>(sender());
    if (!styling) {
        return;
    }
    setParentStyled(styling);
    Q_EMIT themeChanged();
}
