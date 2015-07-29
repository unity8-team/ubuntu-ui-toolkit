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

#ifndef UCTHEMINGATTACHED_H
#define UCTHEMINGATTACHED_H

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <QtQml>

class UCTheme;
class QQuickItem;

class UCThemeChangeListener {
public:
    virtual void preThemeChanged() = 0;
    virtual void postThemeChanged() = 0;
};

class UCThemingAttached : public QObject
{
    Q_OBJECT
public:
    explicit UCThemingAttached(QObject *parent = 0);

    static UCThemingAttached *qmlAttachedProperties(QObject *owner);

    void setListener(UCThemeChangeListener *listener)
    {
        m_listener = listener;
    }
    UCTheme *getTheme();
    bool setTheme(UCTheme *theme);
    void itemParentChanged();

Q_SIGNALS:
    void themeChanged();

public Q_SLOTS:

    void ascendantChanged(QQuickItem *ascendant);
    void parentStyleChanged();

protected:
    friend class UCTheme;

    QStack< QPointer<QQuickItem> > m_parentStack;
    QPointer<UCThemingAttached> m_parentStyling;
    UCTheme *m_theme;
    UCThemeChangeListener *m_listener;
    QQuickItem *m_ownerItem;

    void connectThemeSignals(UCTheme *theme, bool connect);

    bool connectParents(QQuickItem *fromItem);
    void disconnectTillItem(QQuickItem *item);
    bool setParentStyled(UCThemingAttached *newStyling);

};
QML_DECLARE_TYPEINFO(UCThemingAttached, QML_HAS_ATTACHED_PROPERTIES)

UCThemingAttached *itemStyling(QQuickItem *item);


#endif // UCTHEMINGATTACHED_H
