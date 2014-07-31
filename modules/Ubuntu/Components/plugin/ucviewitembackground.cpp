/*
 * Copyright 2014 Canonical Ltd.
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
 */

#include "ucviewitembackground.h"
#include "ucviewitem.h"
#include "ucviewitem_p.h"
#include "uctheme.h"
#include <QtQml/QQmlInfo>
#include <QtQuick/private/qquickitem_p.h>

UCViewItemBackground::UCViewItemBackground(QQuickItem *parent)
    : QQuickItem(parent)
    , m_item(0)
    , m_color(Qt::transparent)
    , m_pressedColor(Qt::yellow)
{
    setFlag(QQuickItem::ItemHasContents, true);
    // set the z-order to be above the main item
    setZ(1);
}

UCViewItemBackground::~UCViewItemBackground()
{
}

void UCViewItemBackground::itemChange(ItemChange change, const ItemChangeData &data)
{
    if (change == ItemParentHasChanged) {
        m_item = qobject_cast<UCViewItemBase*>(data.item);
    }
    QQuickItem::itemChange(change, data);
}

QSGNode *UCViewItemBackground::updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *data)
{
    Q_UNUSED(data);

    UCViewItemBasePrivate *dd = UCViewItemBasePrivate::get(m_item);
    bool pressed = (dd && dd->pressed);
    QColor color = pressed ? m_pressedColor : m_color;

    if (width() <= 0 || height() <= 0 || (color.alpha() == 0)) {
        delete oldNode;
        return 0;
    }
    QSGRectangleNode *rectNode = static_cast<QSGRectangleNode *>(oldNode);
    if (!rectNode) {
        rectNode = QQuickItemPrivate::get(this)->sceneGraphContext()->createRectangleNode();
    }
    rectNode->setColor(color);
    rectNode->setRect(boundingRect());
    rectNode->update();
    return rectNode;
}

QColor UCViewItemBackground::color() const
{
    return m_color;
}
void UCViewItemBackground::setColor(const QColor &color)
{
    if (m_color != color) {
        m_color = color;
        update();
        Q_EMIT colorChanged();
    }
}

QColor UCViewItemBackground::pressedColor() const
{
    return m_pressedColor;
}
void UCViewItemBackground::setPressedColor(const QColor &pressedColor)
{
    if (m_pressedColor != pressedColor) {
        m_pressedColor = pressedColor;
        update();
        Q_EMIT pressedColorChanged();
    }
}
