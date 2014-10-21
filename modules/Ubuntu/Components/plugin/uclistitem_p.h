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

#ifndef UCVIEWITEM_P_H
#define UCVIEWITEM_P_H

#include "uclistitem.h"
#include "ucstyleditembase_p.h"
#include <QtCore/QPointer>
#include <QtCore/QBasicTimer>
#include <QtQuick/private/qquickrectangle_p.h>

#define MIN(x, y)           ((x < y) ? x : y)
#define MAX(x, y)           ((x > y) ? x : y)
#define CLAMP(v, min, max)  (min <= max) ? MAX(min, MIN(v, max)) : MAX(max, MIN(v, min))

class QQuickFlickable;
class QQuickPropertyAnimation;
class UCListItemContent;
class UCListItemDivider;
class UCListItemActions;
class PropertyChange;
class FlickableControl;
class UCListItemExpansion;
class UCListItemPrivate : public UCStyledItemBasePrivate
{
    Q_DECLARE_PUBLIC(UCListItem)
public:
    UCListItemPrivate();
    virtual ~UCListItemPrivate();
    void init();

    static inline UCListItemPrivate *get(UCListItem *that)
    {
        Q_ASSERT(that);
        return that->d_func();
    }

    bool isPressAndHoldConnected();

    void _q_updateColors();
    void _q_dimmDisabled();
    void _q_rebound();
    void _q_updateSize();
    void _q_completeRebinding();
    void _q_updateIndex(QObject *ownerItem = 0);
    void _q_updateSelected();
    void cleanup();
    void reboundTo(qreal x);
    void setPressed(bool pressed);
    void setMoved(bool moved);
    bool grabPanel(UCListItemActions *optionList, bool isMoved);
    void resize();
    void update();
    void clampX(qreal &x, qreal dx);
    QQuickItem *createSelectionPanel();
    void toggleSelectionMode();
    bool canHighlight();

    UCListItemAttachedPrivate *parentAttached();

    bool pressed:1;
    bool highlightColorChanged:1;
    bool moved:1;
    bool suppressClick:1;
    bool ready:1;
    bool selectable:1;
    bool selected:1;
    int index;
    qreal xAxisMoveThresholdGU;
    QBasicTimer pressAndHoldTimer;
    QPointF lastPos;
    QPointF pressedPos;
    QColor color;
    QColor highlightColor;
    FlickableControl *flickableControl;
    QPointer<QQuickFlickable> flickable;
    QQuickPropertyAnimation *reboundAnimation;
    QQuickItem *contentItem;
    PropertyChange *disabledOpacity;
    UCListItemDivider *divider;
    UCListItemActions *leadingActions;
    UCListItemActions *trailingActions;
    QQuickItem *selectionPanel;
    UCAction *action;
    UCListItemExpansion *expansion;
};

// controls all ascendant Flickables
class FlickableControl : QObject {
    Q_OBJECT
public:
    FlickableControl(QObject *parent = 0);
    ~FlickableControl();

    void listenToRebind(bool listen);
    void grab(bool grab);
    bool isMoving();

private Q_SLOTS:
    void rebind();

protected:
    struct Record {
        QPointer<QQuickFlickable> flickable;
        PropertyChange *interactive;
    };

    UCListItem *item;
    QList<Record> list;
};

class UCListItemDivider : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool visible MEMBER m_visible WRITE setVisible NOTIFY visibleChanged)
    Q_PROPERTY(qreal leftMargin MEMBER m_leftMargin WRITE setLeftMargin NOTIFY leftMarginChanged)
    Q_PROPERTY(qreal rightMargin MEMBER m_rightMargin WRITE setRightMargin NOTIFY rightMarginChanged)
    Q_PROPERTY(QColor colorFrom MEMBER m_colorFrom WRITE setColorFrom NOTIFY colorFromChanged)
    Q_PROPERTY(QColor colorTo MEMBER m_colorTo WRITE setColorTo NOTIFY colorToChanged)
public:
    explicit UCListItemDivider(QObject *parent = 0);
    ~UCListItemDivider();
    void init(UCListItem *listItem);

Q_SIGNALS:
    void visibleChanged();
    void leftMarginChanged();
    void rightMarginChanged();
    void colorFromChanged();
    void colorToChanged();

protected:
    QSGNode *paint(const QRectF &rect);

private Q_SLOTS:
    void unitsChanged();
    void paletteChanged();

private:
    void updateGradient();
    void setVisible(bool visible);
    void setLeftMargin(qreal leftMargin);
    void setRightMargin(qreal rightMargin);
    void setColorFrom(const QColor &color);
    void setColorTo(const QColor &color);

    bool m_visible:1;
    bool m_lastItem:1;
    bool m_leftMarginChanged:1;
    bool m_rightMarginChanged:1;
    bool m_colorFromChanged:1;
    bool m_colorToChanged:1;
    qreal m_thickness;
    qreal m_leftMargin;
    qreal m_rightMargin;
    QColor m_colorFrom;
    QColor m_colorTo;
    QGradientStops m_gradient;
    UCListItemPrivate *m_listItem;
    friend class UCListItem;
    friend class UCListItemPrivate;
};

class UCListItemExpansion : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool expanded READ isExpanded WRITE setExpanded NOTIFY expandedChanged)
    Q_PROPERTY(UCListItem::ExpansionFlags flags MEMBER flags WRITE setFlags NOTIFY flagsChanged)
    Q_PROPERTY(qreal height MEMBER height WRITE setHeight NOTIFY heightChanged)
    Q_PROPERTY(QQmlComponent *content MEMBER content WRITE setContent NOTIFY contentChanged)
public:

    explicit UCListItemExpansion(QObject *parent = 0);
    ~UCListItemExpansion();

    void init(UCListItem *item);

    bool isExpanded() const;
    void setExpanded(bool expanded);
    void setFlags(UCListItem::ExpansionFlags flags);
    void setHeight(qreal height);
    void setContent(QQmlComponent *content);

    qreal collapsedHeight();
    qreal effectiveHeight()
    {
        return effectiveExpansionHeight;
    }

    bool hasFlag(UCListItem::ExpansionFlag flag);

Q_SIGNALS:
    void expandedChanged();
    void flagsChanged();
    void heightChanged();
    void contentChanged();

private Q_SLOTS:
    void completeExpand();
    void completeCollapse();

protected:
    UCListItem *listItem;
    QQmlComponent *content;
    QQuickItem *contentItem;
    PropertyChange *heightChange;
    QQuickPropertyAnimation *heightAnimation;
    bool expanded:1;
    bool collapsed:1; // set when height animation completes on height restore
    UCListItem::ExpansionFlags flags;
    qreal height;
    qreal effectiveExpansionHeight;

    bool createAnimation();
    bool createContent();
    void updateExpanded();
};

class UCListItemAttachedPrivate
{
    Q_DECLARE_PUBLIC(UCListItemAttached)
public:
    UCListItemAttachedPrivate(UCListItemAttached *qq);
    static UCListItemAttachedPrivate *get(UCListItemAttached *object)
    {
        return object->d_func();
    }

    int expandedIndex() const;
    UCListItem *item() const;

    void setExpandedItem(UCListItem *listItem);
    void expansionCompleted();
    void collapseExpanded(bool signalChange = true);

    //private slots
    void _q_updateExpandedIndex(int index);
    void _q_adjustFlickableContentY();
private:
    UCListItemAttached *q_ptr;
    UCListItem *m_attacheeItem;
    QPointer<UCListItem> m_expandedItem;
    int m_expandedIndex;
};

QColor getPaletteColor(const char *profile, const char *color);

QML_DECLARE_TYPE(UCListItemDivider)

#endif // UCVIEWITEM_P_H
