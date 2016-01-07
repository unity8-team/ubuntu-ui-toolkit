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
#include <qpa/qplatformtheme.h>

#define MENUITEM_TRACE(params)    qCDebug(qpaUbuntuTheme).nospace() << "[QPA MENUITEM]" << params

UCPlatformMenuItem::UCPlatformMenuItem()
    : QPlatformMenuItem()
    , m_menu(Q_NULLPTR)
    , m_tag(0)
    , m_role(MenuRole(0))
    , m_enabled(true)
    , m_visible(true)
    , m_separator(false)
{
    MENUITEM_TRACE("CREATE MENUITEM");
}

UCPlatformMenuItem::~UCPlatformMenuItem()
{
    MENUITEM_TRACE("DELETE MENUITEM");
}

void UCPlatformMenuItem::setTag(quintptr tag)
{
    m_tag = tag;
    MENUITEM_TRACE("SET TAG " << tag);
}
quintptr UCPlatformMenuItem::tag()const
{
    return m_tag;
}

void UCPlatformMenuItem::setText(const QString &text)
{
    // FIXME: 5.5 has support toremove mnemonics!
    m_text = text;
    MENUITEM_TRACE("TEXT " << text);
}
void UCPlatformMenuItem::setIcon(const QIcon &icon)
{
    m_icon = icon;
    MENUITEM_TRACE("ICON " << icon.name());
}
void UCPlatformMenuItem::setMenu(QPlatformMenu *menu)
{
    m_menu = static_cast<UCPlatformMenu*>(menu);
    MENUITEM_TRACE("SET MENU " << menu);
}
void UCPlatformMenuItem::setVisible(bool isVisible)
{
    m_visible = isVisible;
    MENUITEM_TRACE("VISIBLE " << m_text << m_visible);
}
void UCPlatformMenuItem::setIsSeparator(bool isSeparator)
{
    m_separator = isSeparator;
    MENUITEM_TRACE("SEPARATOR " << m_separator);
}
void UCPlatformMenuItem::setCheckable(bool checkable)
{
    m_checkable = checkable;
    MENUITEM_TRACE("CHECKABLE " << m_text << m_checkable);
}
void UCPlatformMenuItem::setChecked(bool isChecked)
{
    m_checked = isChecked;
    MENUITEM_TRACE("CHECKED " << m_text << m_checked);
}
void UCPlatformMenuItem::setShortcut(const QKeySequence& shortcut)
{
    m_shortcut = shortcut;
    MENUITEM_TRACE("SHORTCUT " << m_text << m_shortcut.toString());
}
void UCPlatformMenuItem::setEnabled(bool enabled)
{
    m_enabled = enabled;
    MENUITEM_TRACE("ENABLED " << m_text << m_enabled);
}
