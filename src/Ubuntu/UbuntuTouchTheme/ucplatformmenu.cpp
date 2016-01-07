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

#include "ucplatformmenu.h"

#include <QtGui/QGuiApplication>
#include <QtQuick/QQuickView>
#include <QtQuick/QQuickItem>

Q_LOGGING_CATEGORY(qpaUbuntuTheme, "qpa.UbuntuTouchTheme", QtMsgType::QtWarningMsg)

#define MENU_TRACE(params)   qCDebug(qpaUbuntuTheme) << "[QPA MENU]" << params

UCPlatformMenu *UCPlatformMenu::m_currentMenu = Q_NULLPTR;

UCPlatformMenu::UCPlatformMenu()
    : QPlatformMenu()
    , m_targetItem(Q_NULLPTR)
    , m_popup(Q_NULLPTR)
    , m_tag(0)
    , m_menuType(DefaultMenu)
    , m_effectiveMenuType(DefaultMenu)
    , m_enabled(true)
    , m_visible(true)
{
    MENU_TRACE("CREATE MENU");
}

UCPlatformMenu::~UCPlatformMenu()
{
    MENU_TRACE("DESTROY MENU");
    dismiss();
}

void UCPlatformMenu::insertMenuItem(QPlatformMenuItem *menuItem, QPlatformMenuItem *before)
{
    if (!before) {
        MENU_TRACE("APPEND ITEM");
        m_menuItems.append(static_cast<UCPlatformMenuItem*>(menuItem));
    } else {
        int index = m_menuItems.indexOf(static_cast<UCPlatformMenuItem *>(before)) + 1;
        MENU_TRACE("INSERT ITEM AT" << index);
        m_menuItems.insert(index, static_cast<UCPlatformMenuItem *>(menuItem));
    }
    if (m_currentMenu == this) {
        syncMenuItem(menuItem);
    }
}

void UCPlatformMenu::removeMenuItem(QPlatformMenuItem *menuItem)
{
    MENU_TRACE("REMOVE ITEM");
    m_menuItems.removeOne(static_cast<UCPlatformMenuItem*>(menuItem));
    if (m_currentMenu == this) {
        syncMenuItem(menuItem);
    }
}

void UCPlatformMenu::syncMenuItem(QPlatformMenuItem *menuItem)
{
    Q_UNUSED(menuItem);
    if (m_currentMenu != this) {
        return;
    }
    MENU_TRACE("SYNC ITEM");
}

void UCPlatformMenu::syncSeparatorsCollapsible(bool enable)
{
    Q_UNUSED(enable);
    MENU_TRACE("syncSeparator null");
}

void UCPlatformMenu::setTag(quintptr tag)
{
    m_tag = tag;
    MENU_TRACE("SET TAG" << tag);
}

quintptr UCPlatformMenu::tag()const
{
    return m_tag;
}


void UCPlatformMenu::setText(const QString &text)
{
    m_text = text;
    MENU_TRACE("SET TEXT" << text);
}

void UCPlatformMenu::setIcon(const QIcon &icon)
{
    m_icon = icon;
    MENU_TRACE("SET ICON" << icon.name());
}

void UCPlatformMenu::setEnabled(bool enabled)
{
    m_enabled = enabled;
    MENU_TRACE("SET ENABLED" << enabled);
}

void UCPlatformMenu::setVisible(bool visible)
{
    m_visible = visible;
    MENU_TRACE("SET VISIBLE" << visible);
}


void UCPlatformMenu::setMenuType(MenuType type)
{
    m_menuType = type;
    MENU_TRACE("SET TYPE" << type);
}


void UCPlatformMenu::showPopup(const QWindow *parentWindow, const QRect &targetRect, const QPlatformMenuItem *item)
{
    MENU_TRACE("Prepare to show");
    if (!m_visible || !m_enabled || !parentWindow) {
        return;
    }
    Q_UNUSED(parentWindow);
    Q_UNUSED(targetRect);
    Q_UNUSED(item);

    Q_EMIT aboutToShow();

    m_parentWindow = const_cast<QWindow*>(parentWindow);
    m_targetRect = targetRect;
    m_targetItem = static_cast<const UCPlatformMenuItem*>(item);

    if (m_currentMenu == this) {
        MENU_TRACE(" ->Position to" << m_targetRect.topLeft());
        m_popup->setPosition(m_targetRect.topLeft());
        m_popup->setVisible(true);
        return;
    }

    if (!m_parentWindow->isActive()) {
        m_parentWindow->requestActivate();
    }
    if (m_currentMenu && m_currentMenu != this) {
        m_currentMenu->dismiss();
    }
    m_currentMenu = this;
    m_effectiveMenuType = m_menuType;
    connect(qGuiApp, &QGuiApplication::focusObjectChanged, this, &UCPlatformMenu::dismiss);

    QQuickView *view = qobject_cast<QQuickView*>(m_parentWindow);
    if (view) {
        if (!m_popup) {
            QQmlComponent component(view->engine(), QUrl("qrc:/EditMenu.qml"));
            m_popup = qobject_cast<QQuickItem*>(component.create());
            Q_ASSERT(m_popup);
            // TEMP
            QStringList list;
            Q_FOREACH(UCPlatformMenuItem *item, m_menuItems) {
                list << item->m_text;
            }
            m_popup->setProperty("model", QVariant::fromValue(list));

            // Popup must have closing signal to drive dismiss
            connect(m_popup, SIGNAL(aboutToClose()), this, SLOT(dismiss()));
            m_popup->setParent(view->rootObject());
            m_popup->setParentItem(view->rootObject());
            m_popup->setVisible(false);
        }

        MENU_TRACE(" ->Position to" << m_targetRect.topLeft());
        m_popup->setPosition(m_targetRect.topLeft());
        m_popup->setVisible(true);
    } else {
        MENU_TRACE("TODO: must create an own QML Engine");
    }
}


void UCPlatformMenu::dismiss()
{
    MENU_TRACE("DISMISS");
    if (!m_popup) {
        return;
    }

    Q_EMIT aboutToHide();
    disconnect(qGuiApp, &QGuiApplication::focusObjectChanged, this, &UCPlatformMenu::dismiss);
    m_popup->setVisible(false);
}

QPlatformMenuItem *UCPlatformMenu::menuItemAt(int position) const
{
    if (position < 0 && position > m_menuItems.size()) {
        return 0;
    }
    MENU_TRACE("ITEM.AT" << position);
    return m_menuItems.at(position);
}

QPlatformMenuItem *UCPlatformMenu::menuItemForTag(quintptr tag) const
{
    for (int i = 0; i < m_menuItems.size(); i++) {
        QPlatformMenuItem *item = m_menuItems.at(i);
        if (item->tag() == tag) {
            MENU_TRACE("ITEM FOR TAG" << tag);
            return item;
        }
    }
    return 0;
}
