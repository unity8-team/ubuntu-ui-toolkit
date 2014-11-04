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

class QQuickFlickable;
class QQuickPropertyAnimation;
class UCListItemContent;
class UCListItemDivider;
class UCListItemActions;
class PropertyChange;
class FlickableControl;
class UCListItemSnapTransition;
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
    void _q_updateIndex();
    void _q_updateSelected();
    int index();
    bool isMoving() const;
    void setContentMoved(bool move);
    void promptRebound();
    bool canHighlight(QMouseEvent *event);
    bool autoHighlightable();
    void setPressed(bool pressed);
    void setTugged(bool tugged);
    bool grabPanel(UCListItemActions *optionList, bool isTugged);
    void listenToRebind(bool listen);
    void resize();
    void update();
    void clampX(qreal &x, qreal dx);
    QQuickItem *createSelectionPanel();
    void toggleSelectionMode();    
    QQuickPropertyAnimation *snapAnimation() const;
    void setSnapAnimation(QQuickPropertyAnimation *transition);

    bool pressed:1;
    bool highlightColorChanged:1;
    bool tugged:1;
    bool suppressClick:1;
    bool ready:1;
    bool contentMoving:1;
    bool selectable:1;
    bool selected:1;
    UCListItem::HighlightPolicy highlight;
    qreal xAxisMoveThresholdGU;
    QBasicTimer pressAndHoldTimer;
    qreal overshootGU;
    QPointF lastPos;
    QPointF pressedPos;
    QColor color;
    QColor highlightColor;
    FlickableControl *flickableControl;
    QPointer<UCListItemAttached> attachedObject;
    QPointer<QQuickItem> countOwner;
    QPointer<QQuickFlickable> flickable;
    QQuickPropertyAnimation *snap;
    UCListItemSnapTransition *snapManager;
    QQuickItem *contentItem;
    PropertyChange *disabledOpacity;
    UCListItemDivider *divider;
    UCListItemActions *leadingActions;
    UCListItemActions *trailingActions;
    QQuickItem *selectionPanel;
    UCAction *action;

    // getter/setter
    UCListItem::HighlightPolicy highlightPolicy() const;
    void setHighlightPolicy(UCListItem::HighlightPolicy policy);
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
    QSGNode *paint(QSGNode *node, const QRectF &rect);

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

QColor getPaletteColor(const char *profile, const char *color);

QML_DECLARE_TYPE(UCListItemDivider)

class UCListItemSnapTransition : public QObject
{
    Q_OBJECT
public:
    UCListItemSnapTransition(UCListItem *item);
    ~UCListItemSnapTransition();

    bool snap(qreal to);

public Q_SLOTS:
    void snapOut();
    void snapIn();

    QQuickPropertyAnimation *getDefaultAnimation();

private:
    bool active;
    UCListItem *item;
    QQuickPropertyAnimation *defaultAnimation;
};

#endif // UCVIEWITEM_P_H
