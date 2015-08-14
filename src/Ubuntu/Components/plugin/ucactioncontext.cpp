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
 * will be removed and destroyed. However, action contexts may contain actions
 * from other documents (so called shared actions), which in case of deletion
 * will be moved into the \l ActionManager::sharedContext actions context.
 */
UCActionContext::UCActionContext(QObject *parent)
    : QObject(parent)
    , m_active(false)
    , m_overlay(false)
{
}
UCActionContext::~UCActionContext()
{
    // remove all actions from the context
    clear();
    ActionProxy::removeContext(this);
}

void UCActionContext::componentComplete()
{
    // add the context to the management
    ActionProxy::addContext(this);
}

void UCActionContext::clear()
{
    if (ActionProxy::instance().globalContext == this) {
        // cleaning global context, only set the context to NULL
        Q_FOREACH(UCAction *action, m_actions) {
            // remove action from context
            action->m_context = Q_NULLPTR;
        }
        m_actions.clear();
    } else {
        // move actions to global context
        QSetIterator<UCAction*> i(m_actions);
        while (i.hasNext()) {
            UCAction *action = i.next();
            qDebug() << "MOVE" << action->m_text << "INTO SHARED";
            action->m_context = ActionProxy::instance().sharedContext;
            action->setGlobal(false);
        }
        m_actions.clear();
    }
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
    return QQmlListProperty<UCAction>(this, 0, UCActionContext::append, UCActionContext::count, 0, UCActionContext::clear);
}

void UCActionContext::append(QQmlListProperty<UCAction> *list, UCAction *action)
{
    UCActionContext *context = qobject_cast<UCActionContext*>(list->object);
    if (context) {
        context->addAction(action);
    }
}

void UCActionContext::clear(QQmlListProperty<UCAction> *list)
{
    UCActionContext *context = qobject_cast<UCActionContext*>(list->object);
    if (context) {
        context->clear();
    }
}

int UCActionContext::count(QQmlListProperty<UCAction> *list)
{
    UCActionContext *context = qobject_cast<UCActionContext*>(list->object);
    if (context) {
        return context->m_actions.count();
    }
    return 0;
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
void UCActionContext::setActive(bool active)
{
    if (m_active == active) {
        return;
    }
    // skip deactivation for global context
    if (!active && (ActionProxy::instance().globalContext == this || ActionProxy::instance().sharedContext == this)) {
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
    // global context canot be overlay
    if (ActionProxy::instance().globalContext == this) {
        return;
    }
    m_overlay = overlay;
    Q_EMIT overlayChanged(overlay);
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
    qDebug() << "INSERT ACTION" << action->m_text << "INTO" << this;
    m_actions.insert(action);
    action->m_context = this;
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
    qDebug() << "REMOVE ACTION" << action->m_text << "FROM" << this;
    m_actions.remove(action);
    action->m_context = (this == ActionProxy::instance().sharedContext) ? Q_NULLPTR : ActionProxy::instance().sharedContext;
    action->setGlobal(false);
}
