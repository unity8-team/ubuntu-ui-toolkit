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

#include "ucunits.h"
#include "uctheme.h"
#include "uclistitem.h"
#include "uclistitem_p.h"
#include "uclistitemactions.h"
#include "uclistitemactions_p.h"
#include "ucubuntuanimation.h"
#include "propertychange_p.h"
#include "i18n.h"
#include "quickutils.h"
#include "plugin.h"
#include "ucaction.h"
#include <QtQml/QQmlInfo>
#include <QtQuick/private/qquickitem_p.h>
#include <QtQuick/private/qquickflickable_p.h>
#include <QtQuick/private/qquickpositioners_p.h>
#include <QtQuick/private/qquickmousearea_p.h>
#include <QtGui/QGuiApplication>
#include <QtGui/QStyleHints>

QColor getPaletteColor(const char *profile, const char *color)
{
    QColor result;
    QObject *palette = UCTheme::instance().palette();
    if (palette) {
        QObject *paletteProfile = palette->property(profile).value<QObject*>();
        if (paletteProfile) {
            result = paletteProfile->property(color).value<QColor>();
        }
    }
    return result;
}
/******************************************************************************
 * Divider
 */
UCListItemDivider::UCListItemDivider(QObject *parent)
    : QObject(parent)
    , m_visible(true)
    , m_lastItem(false)
    , m_leftMarginChanged(false)
    , m_rightMarginChanged(false)
    , m_colorFromChanged(false)
    , m_colorToChanged(false)
    , m_thickness(0)
    , m_leftMargin(0)
    , m_rightMargin(0)
    , m_listItem(0)
{
    connect(&UCUnits::instance(), &UCUnits::gridUnitChanged, this, &UCListItemDivider::unitsChanged);
    connect(&UCTheme::instance(), &UCTheme::paletteChanged, this, &UCListItemDivider::paletteChanged);
    unitsChanged();
    paletteChanged();
}
UCListItemDivider::~UCListItemDivider()
{
}

void UCListItemDivider::init(UCListItem *listItem)
{
    QQml_setParent_noEvent(this, listItem);
    m_listItem = UCListItemPrivate::get(listItem);
}

void UCListItemDivider::unitsChanged()
{
    m_thickness = UCUnits::instance().dp(2);
    if (!m_leftMarginChanged) {
        m_leftMargin = UCUnits::instance().gu(2);
    }
    if (!m_rightMarginChanged) {
        m_rightMargin = UCUnits::instance().gu(2);
    }
    if (m_listItem) {
        m_listItem->update();
    }
}

void UCListItemDivider::paletteChanged()
{
    QColor background = getPaletteColor("normal", "background");
    if (!background.isValid()) {
        return;
    }
    // FIXME: we need a palette value for divider colors, till then base on the background
    // luminance
    if (!m_colorFromChanged || !m_colorToChanged) {
        qreal luminance = (background.red()*212 + background.green()*715 + background.blue()*73)/1000.0/255.0;
        bool lightBackground = (luminance > 0.85);
        if (!m_colorFromChanged) {
            m_colorFrom = lightBackground ? QColor("#26000000") : QColor("#26FFFFFF");
        }
        if (!m_colorToChanged) {
            m_colorTo = lightBackground ? QColor("#14FFFFFF") : QColor("#14000000");
        }
        updateGradient();
    }
}

void UCListItemDivider::updateGradient()
{
    m_gradient.clear();
    m_gradient.append(QGradientStop(0.0, m_colorFrom));
    m_gradient.append(QGradientStop(0.49, m_colorFrom));
    m_gradient.append(QGradientStop(0.5, m_colorTo));
    m_gradient.append(QGradientStop(1.0, m_colorTo));
    if (m_listItem) {
        m_listItem->update();
    }
}

QSGNode *UCListItemDivider::paint(const QRectF &rect)
{
    if (m_visible && !m_lastItem && (m_gradient.size() > 0)) {
        // the parent always recreates the node, so no worries for the existing child node
        QSGRectangleNode *rectNode = m_listItem->sceneGraphContext()->createRectangleNode();
        // margins are only applied when the ListItem is in normal state, when pressed,
        // the divider is painted from edge to edge
        qreal left = (m_listItem && m_listItem->canHighlight()) ? 0 : m_leftMargin;
        qreal right = (m_listItem && m_listItem->canHighlight()) ? rect.width() : rect.width() - m_leftMargin - m_rightMargin;
        rectNode->setRect(QRectF(left, rect.height() - m_thickness, right, m_thickness));
        rectNode->setGradientStops(m_gradient);
        rectNode->update();
        return rectNode;
    } else {
        return 0;
    }
}

void UCListItemDivider::setVisible(bool visible)
{
    if (m_visible == visible) {
        return;
    }
    m_visible = visible;
    m_listItem->resize();
    m_listItem->update();
    Q_EMIT visibleChanged();
}

void UCListItemDivider::setLeftMargin(qreal leftMargin)
{
    if (m_leftMargin == leftMargin) {
        return;
    }
    m_leftMargin = leftMargin;
    m_leftMarginChanged = true;
    m_listItem->update();
    Q_EMIT leftMarginChanged();
}

void UCListItemDivider::setRightMargin(qreal rightMargin)
{
    if (m_rightMargin == rightMargin) {
        return;
    }
    m_rightMargin = rightMargin;
    m_rightMarginChanged = true;
    m_listItem->update();
    Q_EMIT rightMarginChanged();
}

void UCListItemDivider::setColorFrom(const QColor &color)
{
    if (m_colorFrom == color) {
        return;
    }
    m_colorFrom = color;
    m_colorFromChanged = true;
    updateGradient();
    Q_EMIT colorFromChanged();
}

void UCListItemDivider::setColorTo(const QColor &color)
{
    if (m_colorTo == color) {
        return;
    }
    m_colorTo = color;
    m_colorToChanged = true;
    updateGradient();
    Q_EMIT colorToChanged();
}

/******************************************************************************
 * FlickableControl
 */
FlickableControl::FlickableControl(QObject *parent)
    : QObject(parent)
    , item(qobject_cast<UCListItem*>(parent))
{
}
FlickableControl::~FlickableControl()
{
    grab(false);
    listenToRebind(false);
}

// collect Flickables from ascendants and listens for moves so list item can be rebound
void FlickableControl::listenToRebind(bool listen)
{
    Q_ASSERT(item);
    if (listen) {
        // we should not have any flickables connected
        Q_ASSERT(!list.count());
        // collect flickables and connect their movement
        QQuickItem *parent = QQuickItemPrivate::get(item)->parentItem;
        while (parent) {
            QQuickFlickable *flickable = qobject_cast<QQuickFlickable*>(parent);
            if (flickable) {
                QObject::connect(flickable, SIGNAL(movementStarted()), this, SLOT(rebind()), Qt::DirectConnection);
                // add flickable to the list
                Record record;
                record.flickable = flickable;
                record.interactive = new PropertyChange(flickable, "interactive");
                list.append(record);
            }
            parent = QQuickItemPrivate::get(parent)->parentItem;
        }
    } else {
        Q_FOREACH(const Record &record, list) {
            if (record.flickable) {
                QObject::disconnect(record.flickable.data(), SIGNAL(movementStarted()), this, SLOT(rebind()));
            }
            delete record.interactive;
        }
        // clear the list
        list.clear();
    }
}

void FlickableControl::grab(bool grab)
{
    // go thru the list and block/unblock interactive
    Q_FOREACH(const Record &record, list) {
        if (grab) {
            PropertyChange::setValue(record.interactive, false);
            qDebug() << "GRAB" << record.flickable << record.flickable->isInteractive();
        } else {
            PropertyChange::restore(record.interactive);
            qDebug() << "UNGRAB" << record.flickable << record.flickable->isInteractive();
        }
    }
}

// reports if any ascendant Flickable is moving
bool FlickableControl::isMoving()
{
    QQuickItem *parent = QQuickItemPrivate::get(item)->parentItem;
    while (parent) {
        QQuickFlickable *flickable = qobject_cast<QQuickFlickable*>(parent);
        if (flickable && flickable->isMoving()) {
            return true;
        }
        parent = QQuickItemPrivate::get(parent)->parentItem;
    }
    return false;
}

// slot to rebind the ListItem
void FlickableControl::rebind()
{
    if (item->contentItem()->x() != 0.0) {
        // content is not in origin, rebind
        UCListItemPrivate::get(item)->_q_rebound();
    } else {
        // we simply do cleanup
        UCListItemPrivate::get(item)->cleanup();
    }
}

/******************************************************************************
 * ListItemPrivate
 */
UCListItemPrivate::UCListItemPrivate()
    : UCStyledItemBasePrivate()
    , pressed(false)
    , highlightColorChanged(false)
    , moved(false)
    , ready(false)
    , selectable(false)
    , selected(false)
    , index(-1)
    , xAxisMoveThresholdGU(1.5)
    , color(Qt::transparent)
    , highlightColor(Qt::transparent)
    , flickableControl(0)
    , flickable(0)
    , reboundAnimation(0)
    , contentItem(new QQuickItem)
    , disabledOpacity(0)
    , divider(new UCListItemDivider)
    , leadingActions(0)
    , trailingActions(0)
    , selectionPanel(0)
    , action(0)
    , expansion(new UCListItemExpansion)
{
}
UCListItemPrivate::~UCListItemPrivate()
{
    delete disabledOpacity;
}

void UCListItemPrivate::init()
{
    Q_Q(UCListItem);
    contentItem->setObjectName("ListItemHolder");
    QQml_setParent_noEvent(contentItem, q);
    contentItem->setParentItem(q);
    divider->init(q);
    expansion->init(q);
    // content will be redirected to the contentItem, therefore we must report
    // children changes as it would come from the main component
    QObject::connect(contentItem, &QQuickItem::childrenChanged,
                     q, &UCListItem::childrenChanged);
    q->setFlag(QQuickItem::ItemHasContents);
    // turn activeFocusOnPress on
//    activeFocusOnPress = true;
//    setFocusable();
    q->setActiveFocusOnPress(true);

    // create flickable controller
    flickableControl = new FlickableControl(q);

    // catch theme palette changes
    QObject::connect(&UCTheme::instance(), SIGNAL(paletteChanged()), q, SLOT(_q_updateColors()));
    _q_updateColors();

    // watch size change and set implicit size;
    QObject::connect(&UCUnits::instance(), SIGNAL(gridUnitChanged()), q, SLOT(_q_updateSize()));
    _q_updateSize();

    // watch enabledChanged()
    QObject::connect(q, SIGNAL(enabledChanged()), q, SLOT(_q_dimmDisabled()), Qt::DirectConnection);

    // create rebound animation
    UCUbuntuAnimation animationCodes;
    reboundAnimation = new QQuickPropertyAnimation(q);
    reboundAnimation->setEasing(animationCodes.StandardEasing());
    reboundAnimation->setDuration(animationCodes.SnapDuration());
    reboundAnimation->setTargetObject(contentItem);
    reboundAnimation->setProperty("x");
    reboundAnimation->setAlwaysRunToEnd(true);
}

//void UCListItemPrivate::setFocusable()
//{
//    // always accept mouse events
//    Q_Q(UCListItem);
//    q->setAcceptedMouseButtons(Qt::LeftButton | Qt::MiddleButton | Qt::RightButton);
//    q->setFiltersChildMouseEvents(true);
//}

// inspired from IS_SIGNAL_CONNECTED(q, UCListItem, pressAndHold, ())
// the macro cannot be used due to Arguments cannot be an empty ()
bool UCListItemPrivate::isPressAndHoldConnected()
{
    Q_Q(UCListItem);
    void (UCListItem::*signal)() = &UCListItem::pressAndHold;
    static QMetaMethod method = QMetaMethod::fromSignal(signal);
    static int signalIdx = QMetaObjectPrivate::signalIndex(method);
    return QObjectPrivate::get(q)->isSignalConnected(signalIdx);
}

void UCListItemPrivate::_q_updateColors()
{
    Q_Q(UCListItem);
    highlightColor = getPaletteColor("selected", "background");
    q->update();
}

void UCListItemPrivate::_q_dimmDisabled()
{
    Q_Q(UCListItem);
    if (q->isEnabled()) {
        PropertyChange::restore(disabledOpacity);
    } else if (opacity() != 0.5) {
        // this is the first time we need to create the property change
        if (!disabledOpacity) {
            disabledOpacity = new PropertyChange(q, "opacity");
        }
        PropertyChange::setValue(disabledOpacity, 0.5);
    }
}

void UCListItemPrivate::_q_rebound()
{
    setPressed(false);
    // initiate rebinding only if there were actions tugged
    Q_Q(UCListItem);
    if (!UCListItemActionsPrivate::isConnectedTo(leadingActions, q) && !UCListItemActionsPrivate::isConnectedTo(trailingActions, q)) {
        return;
    }
    setMoved(false);
    //connect rebound completion so we can disconnect
    QObject::connect(reboundAnimation, SIGNAL(stopped()), q, SLOT(_q_completeRebinding()));
    // then rebound to zero
    reboundTo(0);
}
void UCListItemPrivate::_q_completeRebinding()
{
    Q_Q(UCListItem);
    // disconnect animation, otherwise snapping will disconnect the panel
    QObject::disconnect(reboundAnimation, SIGNAL(stopped()), q, SLOT(_q_completeRebinding()));
    // restore flickable's interactive and cleanup
    flickableControl->grab(false);
    // no need to listen flickables any longer
    flickableControl->listenToRebind(false);
    // disconnect actions
    grabPanel(leadingActions, false);
    grabPanel(trailingActions, false);
}

void UCListItemPrivate::_q_updateIndex(QObject *ownerItem)
{
    Q_Q(UCListItem);
    if (!ownerItem) {
        ownerItem = q->sender();
    }
    Q_ASSERT(ownerItem);
    // update the index as well
    QQmlContext *context = qmlContext(q);
    if (context) {
        QVariant indexProperty = context->contextProperty("index");
        index = indexProperty.isValid() ? indexProperty.toInt() : -1;
    }
    divider->m_lastItem = ready && index == (ownerItem->property("count").toInt() - 1);
}

void UCListItemPrivate::_q_updateSelected()
{
    Q_Q(UCListItem);
    bool checked = selectionPanel->property("checked").toBool();
    q->setSelected(checked);
    update();
}

// the function performs a cleanup on mouse release without any rebound animation
void UCListItemPrivate::cleanup()
{
    setPressed(false);
    setMoved(false);
    _q_completeRebinding();
}

void UCListItemPrivate::reboundTo(qreal x)
{
    reboundAnimation->setFrom(contentItem->x());
    reboundAnimation->setTo(x);
    reboundAnimation->restart();
}

// set pressed flag and update background
// called when units size changes
void UCListItemPrivate::_q_updateSize()
{
    Q_Q(UCListItem);
    QQuickItem *owner = flickable ? flickable : parentItem;
    q->setImplicitWidth(owner ? owner->width() : UCUnits::instance().gu(40));
    q->setImplicitHeight(UCUnits::instance().gu(7));
}

// set pressed flag and update contentItem
void UCListItemPrivate::setPressed(bool pressed)
{
    if (this->pressed != pressed) {
        this->pressed = pressed;
        suppressClick = false;
        Q_Q(UCListItem);
        q->update();
        Q_EMIT q->pressedChanged();
    }
}
// toggles the moved flag and installs/removes event filter
void UCListItemPrivate::setMoved(bool moved)
{
    suppressClick = moved;
    if (this->moved == moved) {
        return;
    }
    this->moved = moved;
    Q_Q(UCListItem);
    QQuickWindow *window = q->window();
    if (moved) {
        window->installEventFilter(q);
    } else {
        window->removeEventFilter(q);
    }
}

// sets the moved flag but also grabs the panels from the leading/trailing actions
bool UCListItemPrivate::grabPanel(UCListItemActions *actionsList, bool isMoved)
{
    Q_Q(UCListItem);
    if (isMoved) {
        bool grab = UCListItemActionsPrivate::connectToListItem(actionsList, q, (actionsList == leadingActions));
        if (actionsList) {
            flickableControl->grab(grab);
        }
        return grab;
    } else {
        UCListItemActionsPrivate::disconnectFromListItem(actionsList);
        return false;
    }
}

void UCListItemPrivate::resize()
{
    Q_Q(UCListItem);
    QRectF rect(q->boundingRect());
    if (divider && divider->m_visible) {
        rect.setHeight(rect.height() - divider->m_thickness);
    }
    if (expansion->isExpanded() && !expansion->hasFlag(UCListItem::ExpandContentItem)) {
        contentItem->setSize(QSizeF(rect.width(), expansion->collapsedHeight()));
    } else {
        contentItem->setSize(rect.size());
    }
}

void UCListItemPrivate::update()
{
    if (!ready) {
        return;
    }
    Q_Q(UCListItem);
    q->update();
}

void UCListItemPrivate::clampX(qreal &x, qreal dx)
{
    UCListItemActionsPrivate *leading = UCListItemActionsPrivate::get(leadingActions);
    UCListItemActionsPrivate *trailing = UCListItemActionsPrivate::get(trailingActions);
    x += dx;
    // min cannot be less than the trailing's panel width
    qreal min = (trailing && trailing->panelItem) ? -trailing->panelItem->width() : 0;
    // max cannot be bigger than 0 or the leading's width in case we have leading panel
    qreal max = (leading && leading->panelItem) ? leading->panelItem->width() : 0;
    x = CLAMP(x, min, max);
}

QQuickItem *UCListItemPrivate::createSelectionPanel()
{
    Q_Q(UCListItem);
    if (!selectionPanel) {
        QUrl panelDocument = UbuntuComponentsPlugin::pluginUrl().
                resolved(QUrl::fromLocalFile("ListItemSelectablePanel.qml"));
        QQmlComponent component(qmlEngine(q), panelDocument);
        if (!component.isError()) {
            selectionPanel = qobject_cast<QQuickItem*>(component.beginCreate(qmlContext(q)));
            if (selectionPanel) {
                QQml_setParent_noEvent(selectionPanel, q);
                selectionPanel->setParentItem(q);
                selectionPanel->setVisible(false);
                selectionPanel->setProperty("checked", selected);
                // complete component creation
                component.completeCreate();
            }
        } else {
            qmlInfo(q) << component.errorString();
        }
    }
    return selectionPanel;
}
void UCListItemPrivate::toggleSelectionMode()
{
    if (!createSelectionPanel()) {
        return;
    }
    Q_Q(UCListItem);
    if (selectable) {
        // move and dimm content item
        selectionPanel->setVisible(true);
        reboundTo(selectionPanel->width());
        QObject::connect(selectionPanel, SIGNAL(checkedChanged()), q, SLOT(_q_updateSelected()));
    } else {
        // remove content item dimming and destroy selection panel as well
        reboundTo(0.0);
        selectionPanel->setVisible(false);
        QObject::disconnect(selectionPanel, SIGNAL(checkedChanged()), q, SLOT(_q_updateSelected()));
    }
    _q_updateSelected();
}

bool UCListItemPrivate::canHighlight()
{
    // do we have a child which has an active (enabled) MouseArea?
    Q_Q(UCListItem);
    QQuickMouseArea *mouseArea = q->findChild<QQuickMouseArea*>();
    bool hasActiveMouseArea = mouseArea && mouseArea->isEnabled() && mouseArea->isVisible();
    return ((pressed && (action || leadingActions || trailingActions || hasActiveMouseArea)) || (selectable && selected));
}

/*!
 * \qmltype ListItem
 * \instantiates UCListItem
 * \inqmlmodule Ubuntu.Components 1.2
 * \ingroup unstable-ubuntu-listitems
 * \since Ubuntu.Components 1.2
 * \brief The ListItem element provides Ubuntu design standards for list or grid
 * views.
 *
 * The component is dedicated to be used in designs with static or dynamic lists
 * (i.e. list views where each item's layout differs or in lists where the content
 * is determined by a given model, thus each element has the same layout). The
 * element does not define any specific layout, components can be placed in any
 * ways on it. However, when used in list views, the content must be carefully
 * chosen to in order to keep the kinetic behavior and the highest FPS possible.
 *
 * The component provides two color properties which configures the item's background
 * when normal or pressed. This can be configures through \l color and \l highlightColor
 * properties.
 *
 * \c contentItem holds all components and resources declared as child to ListItem.
 * Being an Item, all other properties can be accessed or altered, with the exception
 * of some:
 * \list A
 * \li do not alter \c x, \c y, \c width or \c height properties as those are
 *      controlled by the ListItem itself when leading or trailing actions are
 *      revealed and thus will destroy your logic
 * \li never anchor left or right anchor lines as it will block revealing the actions.
 * \endlist
 *
 * Each ListItem has a thin divider shown on the bottom of the component. This
 * divider can be configured through the \l divider grouped property, which can
 * configure its margins from the edges of the ListItem as well as its visibility.
 * When used in \c ListView or \l UbuntuListView, the last list item will not
 * show the divider no matter of the visible property value set.
 *
 * ListItem can handle actions that can get tugged from front to back of the item.
 * These actions are Action elements visualized in panels attached to the front
 * or to the back of the item, and are revealed by swiping the item horizontally.
 * The tug is started only after the mouse/touch move had passed a given threshold.
 * These actions are configured through the \l leadingActions as well as \l
 * trailingActions properties.
 * \qml
 * ListItem {
 *     id: listItem
 *     leadingActions: ListItemActions {
 *         actions: [
 *             Action {
 *                 iconName: "delete"
 *                 onTriggered: listItem.destroy()
 *             }
 *         ]
 *     }
 *     trailingActions: ListItemActions {
 *         actions: [
 *             Action {
 *                 iconName: "search"
 *                 onTriggered: {
 *                     // do some search
 *                 }
 *             }
 *         ]
 *     }
 * }
 * \endqml
 * \note When a list item is tugged, it automatically connects both leading and
 * trailing actions to the list item. This implies that a ListItem cannot use
 * the same ListItemActions instance for both leading and trailing actions. If
 * it is desired to have the same action present in both leading and trailing
 * actions, one of the ListItemActions actions list can use the other's list. In
 * the following example the list item can be deleted through both leading and
 * trailing actions:
 * \qml
 * ListItem {
 *     id: listItem
 *     leadingActions: ListItemActions {
 *         actions: [
 *             Action {
 *                 iconName: "delete"
 *                 onTriggered: listItem.destroy()
 *             }
 *         ]
 *     }
 *     trailingActions: ListItemActions {
 *         actions: leadingActions.actions
 *     }
 * }
 * \endqml
 * \sa ListItemActions
 *
 * \section3 Expansion
 * One additional and useful feature of the ListItem is the ability to expand. This
 * feature is driven by the \l expansion group. The component can expand if at least
 * \l expansion.height or \l expansion.content is specified. \l expansion.flags can
 * drive how the \l expansion.content will be placed when expanded, either below the
 * \l contentItem or inside the \l contentItem. In the first case the \l contentItem
 * height will be preserved to the collapsed height and the ListItem will expand to
 * the height specified in \l expansion.height. This implies that the heigth given
 * in \l expansion.height must be greater than the collapsed height of the ListItem.
 *
 * If only height is specified, the ListItem will simply expand to the height
 * given. If only content is specified, the item will expand considering the height
 * set by the content. In case both heigth and content is set, heigth acts as
 * maximum allowed expansion limit. This means that expansion can be less than the
 * maximum heigth specified, and the content of the \l expansion.content will be
 * cut to this maximum value if exceeds.
 * \qml
 * import QtQuick 2.3
 * import Ubuntu.Components 1.2
 *
 * ListView {
 *     width: units.gu(40)
 *     height: units.gu(50)
 *     model: 50
 *     delegate: ListItem {
 *         Label {
 *             anchors.fill: parent
 *             text: "Item #" + index
 *         }
 *         expansion {
 *             flags: ListItem.CollapseOnClick
 *             height: units.gu(10)
 *             content: Rectangle {
 *                 height: units.gu(6)
 *                 width: parent.width
 *                 color: "blue"
 *             }
 *         }
 *         onPressAndHold: expansion.expanded = true
 *     }
 * }
 * \endqml
 */

/*!
 * \qmlsignal ListItem::clicked()
 * The signal is emitted when the component gets released while the \l pressed property
 * is set. The signal is not emitted if the ListItem content is tugged or when used in
 * Flickable (or ListView, GridView) and the Flickable gets moved.
 *
 * If the ListItem contains a component which contains a MouseArea, the clicked
 * signal will be supressed.
 */

/*!
 * \qmlsignal ListItem::pressAndHold()
 * The signal is emitted when the list item is long pressed. When a slot is connected,
 * no \l clicked signal will be emitted, similarly to MouseArea's pressAndHold.
 *
 * If the ListItem contains a component which contains a MouseArea, the pressAndHold
 * signal will be supressed.
 */

UCListItem::UCListItem(QQuickItem *parent)
    : UCStyledItemBase(*(new UCListItemPrivate), parent)
{
    Q_D(UCListItem);
    d->init();
}

UCListItem::~UCListItem()
{
}

UCListItemAttached * UCListItem::qmlAttachedProperties(QObject *owner)
{
    return new UCListItemAttached(owner);
}

void UCListItem::componentComplete()
{
    UCStyledItemBase::componentComplete();
    Q_D(UCListItem);
    d->ready = true;
    /* We only deal with ListView, as for other cases we would need to check the children
     * changes, which would have an enormous impact on performance in case of huge amount
     * of items. However, if the parent item, or Flickable declares a "count" property,
     * the ListItem will take use of it!
     */
    QQuickItem *countOwner = (d->flickable && d->flickable->property("count").isValid()) ?
                d->flickable :
                (d->parentItem && d->parentItem->property("count").isValid()) ? d->parentItem : 0;
    if (countOwner) {
        QObject::connect(countOwner, SIGNAL(countChanged()),
                         this, SLOT(_q_updateIndex()), Qt::DirectConnection);
        d->_q_updateIndex(countOwner);
    }
}

void UCListItem::itemChange(ItemChange change, const ItemChangeData &data)
{
    UCStyledItemBase::itemChange(change, data);
    if (change == ItemParentHasChanged) {
        Q_D(UCListItem);

        // make sure we are not connected to any previous Flickable
        d->flickableControl->listenToRebind(false);
        // check if we are in a positioner, and if that positioner is in a Flickable
        QQuickBasePositioner *positioner = qobject_cast<QQuickBasePositioner*>(data.item);
        if (positioner) {
            // create the attached property for the positioner;
            d->attached = static_cast<UCListItemAttached*>(
                            qmlAttachedPropertiesObject<UCListItem>(positioner));
        }
        if (positioner && positioner->parentItem()) {
            d->flickable = qobject_cast<QQuickFlickable*>(positioner->parentItem()->parentItem());
        } else if (data.item && data.item->parentItem()){
            // check if we are in a Flickable then
            d->flickable = qobject_cast<QQuickFlickable*>(data.item->parentItem());
            if (d->flickable) {
                d->attached = static_cast<UCListItemAttached*>(
                                qmlAttachedPropertiesObject<UCListItem>(d->flickable));
            }
        }

        if (d->flickable) {
            // connect to flickable to get width changes
            QObject::connect(d->flickable, SIGNAL(widthChanged()), this, SLOT(_q_updateSize()));
        } else if (data.item) {
            QObject::connect(data.item, SIGNAL(widthChanged()), this, SLOT(_q_updateSize()));
        } else {
            // mar as not ready, so no action should be performed which depends on readyness
            d->ready = false;
        }

        // update size
        d->_q_updateSize();
    }
}

void UCListItem::geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    UCStyledItemBase::geometryChanged(newGeometry, oldGeometry);
    // resize contentItem item
    Q_D(UCListItem);
    d->resize();
}

QSGNode *UCListItem::updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *data)
{
    Q_UNUSED(data);

    Q_D(UCListItem);
    // do highlight only if at least one of action, leadingActions or trailingAction
    // properties is set
    QColor color = d->canHighlight() ? d->highlightColor : d->color;

    delete oldNode;
    if (width() <= 0 || height() <= 0) {
        return 0;
    }

    QSGRectangleNode *rectNode = 0;
    if (color.alpha() > 0) {
        rectNode = QQuickItemPrivate::get(this)->sceneGraphContext()->createRectangleNode();
        rectNode->setColor(color);
        // cover only the area of the contentItem
        rectNode->setRect(d->contentItem->boundingRect());
        rectNode->update();
    }
    oldNode = rectNode;
    if (d->divider && d->divider->m_visible) {
        QSGNode * dividerNode = d->divider->paint(boundingRect());
        if (dividerNode && oldNode) {
            oldNode->appendChildNode(dividerNode);
        } else if (dividerNode) {
            oldNode = dividerNode;
        }
    }
    return oldNode;
}

void UCListItem::mousePressEvent(QMouseEvent *event)
{
    UCStyledItemBase::mousePressEvent(event);
    Q_D(UCListItem);
    if (d->selectable || d->flickableControl->isMoving()) {
        // while moving, we cannot select or tug any items
        return;
    }
    if (!d->pressed && event->button() == Qt::LeftButton && d->contentItem->contains(event->localPos())) {
        d->setPressed(true);
        d->lastPos = d->pressedPos = event->localPos();
        // connect the Flickable to know when to rebound
        d->flickableControl->listenToRebind(true);
        // start pressandhold timer
        d->pressAndHoldTimer.start(QGuiApplication::styleHints()->mousePressAndHoldInterval(), this);
    }
    // accept the event so we get the rest of the events as well
    event->accept();
}

void UCListItem::mouseReleaseEvent(QMouseEvent *event)
{
    UCStyledItemBase::mouseReleaseEvent(event);
    Q_D(UCListItem);
    if (d->selectable) {
        // no move is allowed while selectable mode is on
        return;
    }
    d->pressAndHoldTimer.stop();
    // set released
    if (d->pressed) {
        // disconnect the flickable
        d->flickableControl->listenToRebind(false);

        if (!d->suppressClick) {
            Q_EMIT clicked();
            if (d->action) {
                Q_EMIT d->action->trigger(d->index);
            }
            d->_q_rebound();
        } else {
            // snap
            qreal snapPosition = 0.0;
            if (d->contentItem->x() < 0) {
                snapPosition = UCListItemActionsPrivate::snap(d->trailingActions);
            } else if (d->contentItem->x() > 0) {
                snapPosition = UCListItemActionsPrivate::snap(d->leadingActions);
            }
            if (d->contentItem->x() == 0.0) {
                // do a cleanup, no need to rebound, the item has been dragged back to 0
                d->cleanup();
            } else if (snapPosition == 0.0){
                d->_q_rebound();
            } else {
                d->reboundTo(snapPosition);
            }
            // unset dragging in panel item
            UCListItemActionsPrivate::setDragging(d->leadingActions, this, false);
            UCListItemActionsPrivate::setDragging(d->trailingActions, this, false);
        }
    }
    d->setPressed(false);
}

void UCListItem::mouseMoveEvent(QMouseEvent *event)
{
    Q_D(UCListItem);
    UCStyledItemBase::mouseMoveEvent(event);

    if (d->selectable) {
        // no move is allowed while selectable mode is on
        return;
    }

    // accept the tugging only if the move is within the threshold
    bool leadingAttached = UCListItemActionsPrivate::isConnectedTo(d->leadingActions, this);
    bool trailingAttached = UCListItemActionsPrivate::isConnectedTo(d->trailingActions, this);
    if (!d->expansion->isExpanded() && d->pressed && !(leadingAttached || trailingAttached)) {
        // check if we can initiate the drag at all
        // only X direction matters, if Y-direction leaves the threshold, but X not, the tug is not valid
        qreal threshold = UCUnits::instance().gu(d->xAxisMoveThresholdGU);
        qreal mouseX = event->localPos().x();
        qreal pressedX = d->pressedPos.x();

        if ((mouseX < (pressedX - threshold)) || (mouseX > (pressedX + threshold))) {
            // the press went out of the threshold area, enable move, if the direction allows it
            d->lastPos = event->localPos();
            // connect both panels
            leadingAttached = d->grabPanel(d->leadingActions, true);
            trailingAttached = d->grabPanel(d->trailingActions, true);
            // stop pressAndHold timer as we started to drag
            d->pressAndHoldTimer.stop();
        }
    }

    if (leadingAttached || trailingAttached) {
        qreal x = d->contentItem->x();
        qreal dx = event->localPos().x() - d->lastPos.x();
        d->lastPos = event->localPos();

        if (dx) {
            // clamp X into allowed dragging area
            d->clampX(x, dx);
            // block flickable
            d->setMoved(true);
            d->contentItem->setX(x);
            // set dragging in panel item
            UCListItemActionsPrivate::setDragging(d->leadingActions, this, true);
            UCListItemActionsPrivate::setDragging(d->trailingActions, this, true);
        }
    }
}

bool UCListItem::childMouseEventFilter(QQuickItem *child, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress) {
        // suppress click event if pressed over an active mouse area
        Q_D(UCListItem);
        QQuickMouseArea *mouseArea = qobject_cast<QQuickMouseArea*>(child);
        if (mouseArea && mouseArea->isEnabled()) {
            d->suppressClick = true;
        } else {
            QQuickFlickable *flickable = qobject_cast<QQuickFlickable*>(child);
            if (flickable && flickable->isInteractive()) {
                d->suppressClick = true;
            }
        }
    }
    return UCStyledItemBase::childMouseEventFilter(child, event);
}

bool UCListItem::eventFilter(QObject *target, QEvent *event)
{
    QPointF myPos;
    // only filter press events, and rebound when pressed outside
    if (event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mouse = static_cast<QMouseEvent*>(event);
        QQuickWindow *window = qobject_cast<QQuickWindow*>(target);
        if (window) {
            myPos = window->contentItem()->mapToItem(this, mouse->localPos());
        }
    } else if (event->type() == QEvent::TouchBegin) {
        QTouchEvent *touch = static_cast<QTouchEvent*>(event);
        QQuickWindow *window = qobject_cast<QQuickWindow*>(target);
        if (window) {
            myPos = window->contentItem()->mapToItem(this, touch->touchPoints()[0].pos());
        }
    }
    if (!myPos.isNull() && !contains(myPos)) {
        Q_D(UCListItem);
        d->_q_rebound();
        // only accept event, but let it be handled by the underlying or surrounding Flickables
        event->accept();
    }
    return UCStyledItemBase::eventFilter(target, event);
}

void UCListItem::timerEvent(QTimerEvent *event)
{
    Q_D(UCListItem);
    if (event->timerId() == d->pressAndHoldTimer.timerId()) {
        d->pressAndHoldTimer.stop();
        if (!d->moved && isEnabled() && d->isPressAndHoldConnected()) {
            d->suppressClick = true;
            Q_EMIT pressAndHold();
        }
    } else {
        QQuickItem::timerEvent(event);
    }
}

/*!
 * \qmlproperty ListItemActions ListItem::leadingActions
 *
 * The property holds the actions and its configuration to be revealed when swiped
 * from left to right.
 *
 * \sa trailingActions
 */
UCListItemActions *UCListItem::leadingActions() const
{
    Q_D(const UCListItem);
    return d->leadingActions;
}
void UCListItem::setLeadingActions(UCListItemActions *actions)
{
    Q_D(UCListItem);
    if (d->leadingActions == actions) {
        return;
    }
    d->leadingActions = actions;
    if (d->leadingActions == d->trailingActions && d->leadingActions) {
        qmlInfo(this) << UbuntuI18n::tr("leadingActions and trailingActions cannot share the same object!");
    }
    Q_EMIT leadingActionsChanged();
}

/*!
 * \qmlproperty ListItemActions ListItem::trailingActions
 *
 * The property holds the actions and its configuration to be revealed when swiped
 * from right to left.
 *
 * \sa leadingActions
 */
UCListItemActions *UCListItem::trailingActions() const
{
    Q_D(const UCListItem);
    return d->trailingActions;
}
void UCListItem::setTrailingActions(UCListItemActions *actions)
{
    Q_D(UCListItem);
    if (d->trailingActions == actions) {
        return;
    }
    d->trailingActions = actions;
    if (d->leadingActions == d->trailingActions && d->trailingActions) {
        qmlInfo(this) << UbuntuI18n::tr("leadingActions and trailingActions cannot share the same object!");
    }
    Q_EMIT trailingActionsChanged();
}

/*!
 * \qmlproperty Item ListItem::contentItem
 *
 * contentItem holds the components placed on a ListItem.
 */
QQuickItem* UCListItem::contentItem() const
{
    Q_D(const UCListItem);
    return d->contentItem;
}

/*!
 * \qmlpropertygroup ::ListItem::divider
 * \qmlproperty bool ListItem::divider.visible
 * \qmlproperty real ListItem::divider.leftMargin
 * \qmlproperty real ListItem::divider.rightMargin
 * \qmlproperty real ListItem::divider.colorFrom
 * \qmlproperty real ListItem::divider.colorTo
 *
 * This grouped property configures the thin divider shown in the bottom of the
 * component. Configures the visibility and the margins from the left and right
 * of the ListItem. When tugged (swiped left or right to reveal the actions),
 * it is not moved together with the content. \c colorFrom and \c colorTo configure
 * the starting and ending colors of the divider.
 *
 * When \c visible is true, the ListItem's content size gets thinner with the
 * divider's \c thickness.
 *
 * The default values for the properties are:
 * \list
 * \li \c visible: true
 * \li \c leftMargin: 2 GU
 * \li \c rightMargin: 2 GU
 * \endlist
 */
UCListItemDivider* UCListItem::divider() const
{
    Q_D(const UCListItem);
    return d->divider;
}

/*!
 * \qmlproperty bool ListItem::pressed
 * True when the item is pressed. The items stays pressed when the mouse or touch
 * is moved horizontally. When in Flickable (or ListView), the item gets un-pressed
 * (false) when the mouse or touch is moved towards the vertical direction causing
 * the flickable to move.
 */
bool UCListItem::pressed() const
{
    Q_D(const UCListItem);
    return d->pressed;
}

/*!
 * \qmlproperty color ListItem::color
 * Configures the color of the normal background. The default value is transparent.
 */
QColor UCListItem::color() const
{
    Q_D(const UCListItem);
    return d->color;
}
void UCListItem::setColor(const QColor &color)
{
    Q_D(UCListItem);
    if (d->color == color) {
        return;
    }
    d->color = color;
    update();
    Q_EMIT colorChanged();
}

/*!
 * \qmlproperty color ListItem::highlightColor
 * Configures the color when highlighted. Defaults to the theme palette's background
 * color.
 *
 * An item is highlighted when selected, or when pressed and it has \l leadingActions
 * or \l trailingActions set, when there is a valid default \l action attached
 * to it or if the ListItem has an active component child, such as a \l Button,
 * a \l Switch, or in general, if an active (enabled and visible) \c MouseArea
 * is added as a child component.
 *
 * \note Adding an active component does not mean the component will be activated
 * when the ListItem will be tapped/clicked outside of the component area. If
 * such a behavior is needed, this must be done explicitly.
 * \qml
 * import QtQiock 2.3
 * import Ubuntu.Components 1.2
 *
 * ListItem {
 *     Label {
 *         text: "This is a label"
 *     }
 *     Switch {
 *         id: toggle
 *         anchors.right: parent.right
 *     }
 *     Component.onCompleted: clicked.connect(toggle.clicked)
 * }
 * \endqml
 *
 * \sa action, leadingActions, trailingActions
 */
QColor UCListItem::highlightColor() const
{
    Q_D(const UCListItem);
    return d->highlightColor;
}
void UCListItem::setHighlightColor(const QColor &color)
{
    Q_D(UCListItem);
    if (d->highlightColor == color) {
        return;
    }
    d->highlightColor = color;
    // no more theme change watch
    disconnect(&UCTheme::instance(), SIGNAL(paletteChanged()), this, SLOT(_q_updateColors()));
    update();
    Q_EMIT highlightColorChanged();
}

/*!
 * \qmlproperty bool ListItem::selectable
 * The property drives whether a list item is selectable or not. When set, the item
 * will show a check box on the leading side hanving the content item pushed towards
 * trailing side and dimmed. The checkbox which will reflect and drive the \l selected
 * state.
 * Defaults to false.
 */
bool UCListItem::selectable() const
{
    Q_D(const UCListItem);
    return d->selectable;
}
void UCListItem::setSelectable(bool selectable)
{
    Q_D(UCListItem);
    if (d->selectable == selectable) {
        return;
    }
    d->selectable = selectable;
    d->toggleSelectionMode();
    Q_EMIT selectableChanged();
}

/*!
 * \qmlproperty bool ListItem::selected
 * The property drives whether a list item is selected or not. While selected, the
 * ListItem is dimmed and cannot be tugged. The default value is false.
 */
bool UCListItem::selected() const
{
    Q_D(const UCListItem);
    return d->selected;
}
void UCListItem::setSelected(bool selected)
{
    Q_D(UCListItem);
    if (d->selected == selected) {
        return;
    }
    d->selected = selected;
    // update panel as well
    if (d->selectionPanel) {
        d->selectionPanel->setProperty("checked", d->selected);
    }
    Q_EMIT selectedChanged();
}

/*!
 * \qmlproperty Action ListItem::action
 * The property holds the default action attached to the list item which will be
 * triggered when the ListItem is clicked. ListItem will not visualize the action,
 * that is the responsibility of the components placed inside the list item.
 * However, when set, the ListItem will be highlighted on press.
 *
 * If the action set has no value type set, ListItem will set its type to \c
 * Action.Integer and the \l {Action::triggered}{triggered} signal will be getting
 * the ListItem index as \e value parameter.
 *
 * \note Handling pressAndHold will suppress the action triggering as the clicked
 * signal is also suppressed. If the action triggering is still needed, it must be
 * triggered manually on \l pressed changed.
 * \qml
 * import Ubuntu.Components 1.2
 *
 * ListItem {
 *     property bool emitActionTriggered: false
 *     action: Action {
 *         onTriggered: console.log("action triggered", value)
 *     }
 *     onPresseAndHold: {
 *         console.log("suppresses clicked() signal, also action triggered");
 *         emitActionTriggered = true;
 *     }
 *     onPressedChanged: {
 *         if (!pressed && emitActionTriggered) {
 *             emitActionTriggered = false;
 *             action.trigger(index);
 *         }
 *     }
 * }
 * \endqml
 *
 * Defaults no null.
 */
UCAction *UCListItem::action() const
{
    Q_D(const UCListItem);
    return d->action;
}
void UCListItem::setAction(UCAction *action)
{
    Q_D(UCListItem);
    if (d->action == action) {
        return;
    }
    d->action = action;
    if (d->action && (d->action->property("parameterType").toInt() == UCAction::None)) {
        d->action->setProperty("parameterType", UCAction::Integer);
    }
    Q_EMIT actionChanged();
}

/*!
 * \qmlpropertygroup ::ListItem::expansion
 * \qmlproperty bool ListItem::expansion.expanded
 * \qmlproperty flags ListItem::expansion.flags
 * \qmlproperty real ListItem::expansion.height
 * \qmlproperty Component ListItem::expansion.content
 *
 * The property group controls the expansion of the ListItem. The \b height drives
 * how much the item should be expanded at maximum and includes the height of the
 * entire ListItem, \b content specifies what should be placed when expanded.
 * \b expanded drives whether the ListItem should be expanded or not and the way
 * it expands and the way the content behaves or is rendered into the ListItem
 * is driven by the \b flags.
 *
 * If \b height is set only, the expansion will proceed but will not place anything
 * in the expanded area. If \b content is set but \b height is not (or set to zero),
 * the expansion will proceed to the size specified in the \b content in the following
 * way:
 * \list
 *  \li - if the \c ExpandContentItem flag is set, the height refers to the entire
 *      height of the ListItem
 *  \li - if the \c ExpandContentItem flag is not set, the height specifies the
 *      additional height to be added to the collapsed height of the ListItem.
 * \endlist
 *
 * The component given in \b content is placed either inside the \l contentItem,
 * or under the \l contentItem, depending whether the \e ExpandContentItem
 * flag is set or not.
 *
 * Expansion will not happen if no \b height or \b content is set, or if \b height
 * is less than the ListItem's collapsed height.
 *
 * The \b flags can take one or a set of the following values:
 * \list
 * \li * \b CollapseOnClick - when set, the expanded ListItem is collapsed when
 *      clicked on it. This requires to click on an area where there is no active
 *      component, meaning that an enabled MouseArea can suppress this behavior,
 *      thus collapse will need to be handled explicitly:
 * \qml
 * import QtQuick 2.3
 * import Ubuntu.Components 1.2
 * ListItem {
 *     width: units.gu(40)
 *     onPressAndHold: expansion.expanded = true
 *     expansion {
 *         flags: ListItem.CollapseOnClick
 *         height: units.gu(6)
 *         content: Rectangle {
 *             id: expandedContext
 *             anchors.fill: parent
 *             color: "blue"
 *             MouseArea {
 *                 width: units.gu(5)
 *                 height: width
 *                 onClicked: console.log("suppress clicked event")
 *                 onPressAndHold: expandedContext.ListItem.collapse()
 *             }
 *         }
 *     }
 * }
 * \endqml
 * \li * \b CollapseOnExternalClick - when set, clicking outside the area of the
 *      expanded ListItem will cause collapsing the ListItem. \e {Set as default.}
 * \li * \b GrabNextInView - when set, expanding the list item will bring in the
 *      next available item in a ListView or Flickable. Setting the flag in any
 *      other cases has no effect.
 * \li * \b ExpandContentItem - when set, the expansion will proceeded on the
 *      \l contentItem and not below it.
 * \li * \b ExclusiveExpand - when set, the expansion of a ListItem implies collapsion
 *      of other expanded ListItems in a ListView or Flickable.
 * \endlist
 *
 * The default values set for the \b flags is \e {CollapseOnExternalClick |
 * GrabNextInView | ExclusiveExpand }.
 */
UCListItemExpansion *UCListItem::expansion() const
{
    Q_D(const UCListItem);
    return d->expansion;
}

/*!
 * \qmlproperty list<Object> ListItem::data
 * \default
 * Overloaded default property containing all the children and resources.
 */
QQmlListProperty<QObject> UCListItem::data()
{
    Q_D(UCListItem);
    return QQuickItemPrivate::get(d->contentItem)->data();
}

/*!
 * \qmlproperty list<Item> ListItem::children
 * Overloaded default property containing all the visible children of the item.
 */
QQmlListProperty<QQuickItem> UCListItem::children()
{
    Q_D(UCListItem);
    return QQuickItemPrivate::get(d->contentItem)->children();
}

/*-----------------------------------------------------------------------------
 * ListItem attached properties
 */

UCListItemAttachedPrivate::UCListItemAttachedPrivate(UCListItemAttached *qq)
    : q_ptr(qq)
    , m_item(0)
    , m_expandedIndex(-1)
{
}

/*!
 * \qmlattachedproperty int ListItem::expandedIndex
 * The property specifies the index of the expanded ListItem in a view (ListView
 * or Flickable). The proeprty is attached to ListView or positioner (Column, Row)
 * and to the expanded content specified in \l expansion.content.
 */
int UCListItemAttachedPrivate::expandedIndex() const
{
    return m_expandedIndex;
}

/*!
 * \qmlattachedproperty ListItem ListItem::item
 * The proeprty holds the instance of the ListItem. The proeprty is valid when
 * attached to the expanded content created from \l expansion.content.
 */
UCListItem *UCListItemAttachedPrivate::item() const
{
    return m_item;
}

UCListItemAttached::UCListItemAttached(QObject *owner)
    : QObject(owner)
{
    new UCListItemAttachedPrivate(this);
}

void UCListItemAttached::setItem(UCListItem *item)
{
    Q_D(UCListItemAttached);
    d->m_item = item;
    Q_EMIT itemChanged();
}

#include "moc_uclistitem.cpp"
