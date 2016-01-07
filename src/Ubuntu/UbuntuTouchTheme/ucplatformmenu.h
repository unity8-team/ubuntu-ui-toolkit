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

#ifndef UCPLATFORMMENU_H
#define UCPLATFORMMENU_H

#include <qpa/qplatformmenu.h>
#include <QtCore/QLoggingCategory>

class UCPlatformMenu;
class UCPlatformMenuItem : public QPlatformMenuItem
{
    Q_OBJECT
public:
    UCPlatformMenuItem();
    ~UCPlatformMenuItem();

    void setTag(quintptr tag) Q_DECL_OVERRIDE;
    quintptr tag()const Q_DECL_OVERRIDE;

    void setText(const QString &text) Q_DECL_OVERRIDE;
    void setIcon(const QIcon &icon) Q_DECL_OVERRIDE;
    void setMenu(QPlatformMenu *menu) Q_DECL_OVERRIDE;
    void setVisible(bool isVisible) Q_DECL_OVERRIDE;
    void setIsSeparator(bool isSeparator) Q_DECL_OVERRIDE;
    void setFont(const QFont &font) Q_DECL_OVERRIDE { Q_UNUSED(font); }
    void setRole(MenuRole role) Q_DECL_OVERRIDE { Q_UNUSED(role); }
    void setCheckable(bool checkable) Q_DECL_OVERRIDE;
    void setChecked(bool isChecked) Q_DECL_OVERRIDE;
    void setShortcut(const QKeySequence& shortcut) Q_DECL_OVERRIDE;
    void setEnabled(bool enabled) Q_DECL_OVERRIDE;
    void setIconSize(int size) Q_DECL_OVERRIDE { Q_UNUSED(size); }
    void setNativeContents(WId item) Q_DECL_OVERRIDE { Q_UNUSED(item); }

    // members
    QKeySequence m_shortcut;
    QString m_text;
    QIcon m_icon;
    UCPlatformMenu *m_menu;
    quintptr m_tag;
    MenuRole m_role;
    bool m_enabled:1;
    bool m_visible:1;
    bool m_separator:1;
    bool m_checkable:1;
    bool m_checked:1;
};

typedef QList<UCPlatformMenuItem*> UCPlatformMenuItemList;

class QQuickItem;
class UCPlatformMenu : public QPlatformMenu
{
    Q_OBJECT
public:
    UCPlatformMenu();
    ~UCPlatformMenu();

    void insertMenuItem(QPlatformMenuItem *menuItem, QPlatformMenuItem *before) Q_DECL_OVERRIDE;
    void removeMenuItem(QPlatformMenuItem *menuItem) Q_DECL_OVERRIDE;
    void syncMenuItem(QPlatformMenuItem *menuItem) Q_DECL_OVERRIDE;
    void syncSeparatorsCollapsible(bool enable) Q_DECL_OVERRIDE;

    void setTag(quintptr tag) Q_DECL_OVERRIDE;
    quintptr tag()const Q_DECL_OVERRIDE;

    void setText(const QString &text) Q_DECL_OVERRIDE;
    void setIcon(const QIcon &icon) Q_DECL_OVERRIDE;
    void setEnabled(bool enabled) Q_DECL_OVERRIDE;
    void setVisible(bool visible) Q_DECL_OVERRIDE;

    void setMenuType(MenuType type) Q_DECL_OVERRIDE;

    void showPopup(const QWindow *parentWindow, const QRect &targetRect, const QPlatformMenuItem *item) Q_DECL_OVERRIDE;

    QPlatformMenuItem *menuItemAt(int position) const Q_DECL_OVERRIDE;
    QPlatformMenuItem *menuItemForTag(quintptr tag) const Q_DECL_OVERRIDE;

public Q_SLOTS:
    // turn these into slots
    void dismiss() Q_DECL_OVERRIDE;

private:
    UCPlatformMenuItemList m_menuItems;
    QPointer<QWindow> m_parentWindow;
    QRect m_targetRect;
    QString m_text;
    QIcon m_icon;
    const UCPlatformMenuItem *m_targetItem;
    QQuickItem *m_popup;
    quintptr m_tag;
    MenuType m_menuType;
    MenuType m_effectiveMenuType;
    bool m_enabled:1;
    bool m_visible:1;

    static UCPlatformMenu *m_currentMenu;
};

Q_DECLARE_LOGGING_CATEGORY(qpaUbuntuTheme)

#define UC_QPA_TRACE(param)     qCDebug(qpaUbuntuTheme).nospace() << " [QPA] " << param

#endif // UCPLATFORMMENU_H
