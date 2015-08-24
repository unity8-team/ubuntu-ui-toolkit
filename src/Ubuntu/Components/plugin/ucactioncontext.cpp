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

#include "ucactioncontext.h"
#include "ucaction.h"
#include "adapters/actionsproxy_p.h"
#include <QtQuick/QQuickItem>
#include <QtQuick/private/qquickitem_p.h>

/*!
 * \qmltype ActionContext
 * \instantiates UCActionContext
 * \inqmlmodule Ubuntu.Components 1.1
 * \ingroup ubuntu
 * \brief ActionContext groups actions together and by providing multiple contexts
 * the developer is able to control the visibility of the actions. The \l ActionManager
 * then exposes the actions from these different contexts.
 *
 * Actions declared in Dialog or Page, including the ones declared for the header,
 * are stored implicitly in the ActionContext associated to the component. When
 * the Page is activated, the Actions in the context will be activated as well.
 * Depending on the context \l overlay type, there can be only one or more action
 * contexts active at a time. When the ActionContext is destroyed, it is removed
 * from the action management, and all its actions declared in the same document
 * will be removed and destroyed, exception being the shared actions.
 *
 * There can be two types of action contexts present in an application: local
 * and global context. \b Local contexts contains those actions which are active
 * and/or are available on a given Page or Dialog or other context. This can contain
 * \e shared actions. Shared actions are typical reusable actions across application
 * views. They are "dormant" until used in local contexts. \b Global actions are
 * those type of actions which are also available while the application is in
 * background.
 */
UCActionContext::UCActionContext(QQuickItem *parent)
    : QQuickItem(parent)
    , m_active(false)
    , m_overlay(false)
{
    setFlag(ItemIsFocusScope);
}
UCActionContext::~UCActionContext()
{
    // remove all actions from the context
    clear();
    ActionProxy::removeContext(this);
}

// Returns an ancestor ActionContext declared either as actionContext or __actionContext (1.2)
// property. If none found, returns the sharedContext. Never returns NULL.
UCActionContext *UCActionContext::findAncestorContext(QObject *parent)
{
    UCActionContext *context = Q_NULLPTR;
    while (parent && !context) {
        context = qobject_cast<UCActionContext*>(parent);
        // if the parent is an Item, we go that way forward
        QQuickItem *parentItem = qobject_cast<QQuickItem*>(parent);
        parent = parentItem ? parentItem->parentItem() : parent->parent();
    }
    return context;
}

bool UCActionContext::registerActionToAncestorContext(QObject *parent, UCAction *action)
{
    if (!parent || !action) {
        return false;
    }
    UCActionContext *context = findAncestorContext(parent);
    if (!context) {
        return false;
    }
    context->addAction(action);
    return true;
}

void UCActionContext::componentComplete()
{
    // add the context to the management
    ActionProxy::addContext(this);
}

void UCActionContext::clear()
{
    // remove context from actions
    Q_FOREACH(UCAction *action, m_actions) {
        action->m_contexts.remove(this);
        // FIXME: shouldn't we emit action->activeChanged?
        // actions can be active until they are in an active context,
        // maybe we want to notify this
    }
    m_actions.clear();
}

/*
 * The function marks all context actions being (un)published.
 */
void UCActionContext::markActionsPublished(bool mark)
{
    Q_FOREACH(UCAction *action, m_actions) {
        action->m_published = mark;
    }
}

/*!
 * \qmlproperty list<Action> ActionContext::actions
 * \default
 * List of Actions in this ActionContext.
 */
QQmlListProperty<UCAction> UCActionContext::actions()
{
    return QQmlListProperty<UCAction>(this, 0, UCActionContext::append, UCActionContext::count, UCActionContext::at, UCActionContext::clear);
}

void UCActionContext::append(QQmlListProperty<UCAction> *list, UCAction *action)
{
    UCActionContext *context = qobject_cast<UCActionContext*>(list->object);
    context->addAction(action);
}

void UCActionContext::clear(QQmlListProperty<UCAction> *list)
{
    UCActionContext *context = qobject_cast<UCActionContext*>(list->object);
    context->clear();
}

UCAction *UCActionContext::at(QQmlListProperty<UCAction> *list, int index)
{
    UCActionContext *context = qobject_cast<UCActionContext*>(list->object);
    return context->m_actions.toList().at(index);
}

int UCActionContext::count(QQmlListProperty<UCAction> *list)
{
    UCActionContext *context = qobject_cast<UCActionContext*>(list->object);
    return context->m_actions.count();
}

/*!
 * \qmlproperty bool ActionContext::active
 * If true the context is active. If false the context is inactive. Defaults to
 * false.
 *
 * When context has been added to the \l ActionManager setting this value controls
 * whether or not the actions in a context are available to external components.
 *
 * The \l ActionManager monitors the active property of each of the local contexts
 * that has been added to it. There can be many local contexts active at the same
 * time.
 *
 * The global context is the only context which cannot be deactivated.
 */
bool UCActionContext::isActive()
{
    // TODO: lookup for an overlay parent if there is overlay contexts set
    if (ActionProxy::instance().activeOverlays.size() > 0) {
        if (ActionProxy::instance().activeOverlays.last() == this) {
            return m_active;
        }
        bool active = true;
        UCActionContext *parentContext = findAncestorContext(this);
        while (parentContext) {
            if (parentContext->isOverlay()) {
                active = parentContext->
            }
        }
    }
    return ((ActionProxy::instance().activeOverlays.size() > 0) ? (ActionProxy::instance().activeOverlays.last() == this) : m_active) &&
            isEnabled();
}
void UCActionContext::setActive(bool active)
{
    if (m_active == active) {
        return;
    }
    // skip deactivation for global context or activation of the shared context
    if (!active && ActionProxy::instance().globalContext == this) {
        return;
    }
    m_active = active;
    Q_EMIT activeChanged(active);
}

/*!
 * \qmlproperty bool ActionContext::overlay
 * \since Ubuntu.Components 1.3
 * The property specifies whether the action context is overlay or not. An active
 * overlay ActionContext deactivates all previous active contexts, so only the
 * active overlay context stays active. There can be only one active overlay
 * ActionContext at a time in an application. When all overlay ActionContext
 * elements are deactivated, the last state of local and global contexts will
 * be restored.
 *
 * Defaults to false.
 */
void UCActionContext::setOverlay(bool overlay)
{
    if (m_overlay == overlay) {
        return;
    }
    // global or shared context canot be overlay
    if (ActionProxy::instance().globalContext == this) {
        return;
    }
    m_overlay = overlay;
    Q_EMIT overlayChanged(overlay);
}

// override data property's append and clear function to add/remove orphan shared actions
// to/from context
QQmlListProperty<QObject> UCActionContext::data()
{
    return QQmlListProperty<QObject>(this, 0, UCActionContext::data_append,
                                             QQuickItemPrivate::data_count,
                                             QQuickItemPrivate::data_at,
                                             UCActionContext::data_clear);
}
void UCActionContext::data_append(QQmlListProperty<QObject> *list, QObject *obj)
{
    QQuickItemPrivate::data_append(list, obj);
    // if obj is Action, add the action to the context.
    UCAction *action = qobject_cast<UCAction*>(obj);
    if (action) {
        static_cast<UCActionContext*>(list->object)->addAction(action);
    }
}
void UCActionContext::data_clear(QQmlListProperty<QObject> *list)
{
    // actions are registered as resources, so we must do cleanup here
    UCActionContext *context = static_cast<UCActionContext*>(list->object);
    context->clear();
    QQuickItemPrivate::data_clear(list);
}

/*!
 * \qmlmethod void ActionContext::addAction(Action action)
 * Adds an Action to the context programatically.
 */
void UCActionContext::addAction(UCAction *action)
{
    if (m_actions.contains(action)) {
        return;
    }
    m_actions.insert(action);
    action->m_contexts.insert(this);
    action->setGlobal(this == ActionProxy::instance().globalContext);
}

/*!
 * \qmlmethod void ActionContext::removeAction(Action action)
 * Removes an action from the context programatically.
 */
void UCActionContext::removeAction(UCAction *action)
{
    if (!action) {
        return;
    }
    m_actions.remove(action);
    action->m_contexts.remove(this);
    // reset global behavior once the action is removed from the global list
    if (!action->m_contexts.contains(ActionProxy::instance().globalContext)) {
        action->setGlobal(false);
    }
}
