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

#include "actionsproxy_p.h"
#include "ucactioncontext.h"

#include <QDebug>

ActionProxy::ActionProxy()
    : QObject(0)
    , globalContext(new UCActionContext)
    , sharedContext(new UCActionContext)
{
    // for testing purposes
    globalContext->setObjectName("GlobalActionContext");
    sharedContext->setObjectName("SharedActionContext");
}
ActionProxy::~ActionProxy()
{
    // clear context explicitly, as global context is not connected to
    clearContextActions(globalContext);
    clearContextActions(sharedContext);
    delete globalContext;
    delete sharedContext;
}

const QSet<UCActionContext*> &ActionProxy::localContexts()
{
    return instance().m_localContexts;
}

// function called by the ActionManager when completed to publish global ActionContext
// actions.
void ActionProxy::publishGlobalContext()
{
    if (instance().globalContext) {
        instance().publishContextActions(instance().globalContext);
        instance().globalContext->markActionsPublished(true);
    }
}

// add a local context
void ActionProxy::addContext(UCActionContext *context)
{
    if (!context) {
        return;
    }
    if (instance().m_localContexts.contains(context)) {
        return;
    }
    instance().m_localContexts.insert(context);
    // watch context activation changes
    instance().watchContextActivation(context, true);
}
// Remove a local context. If the context was active, removes the actions from the system.
void ActionProxy::removeContext(UCActionContext *context)
{
    if (!context) {
        return;
    }
    // make sure the context is deactivated
    context->setActive(false);
    instance().watchContextActivation(context, false);
    instance().m_localContexts.remove(context);
}

// toggles context activation watching for a given context
void ActionProxy::watchContextActivation(UCActionContext *context, bool watch)
{
    if (!context) {
        return;
    }
    if (watch) {
        // connect to action proxy
        QObject::connect(context, &UCActionContext::activeChanged,
                         this, &ActionProxy::handleContextActivation,
                         Qt::DirectConnection);
        QObject::connect(context, &UCActionContext::overlayChanged,
                         this, &ActionProxy::handleContextOverlay,
                         Qt::DirectConnection);
    } else {
        // disconnect
        QObject::disconnect(context, &UCActionContext::activeChanged,
                            this, &ActionProxy::handleContextActivation);
        QObject::disconnect(context, &UCActionContext::overlayChanged,
                            this, &ActionProxy::handleContextOverlay);
    }
}
// handles the local context activation
void ActionProxy::handleContextActivation(bool active)
{
    // sender is the context changing activation
    UCActionContext *context = qobject_cast<UCActionContext*>(sender());
    if (!context || (active && m_activeContexts.contains(context))) {
        return;
    }
    if (!active && m_activeContexts.contains(context)) {
        // perform system cleanup and remove the context
        clearContextActions(context);
        context->markActionsPublished(false);
        m_activeContexts.remove(context);
    } else {
        // publish the context's actions to the system
        publishContextActions(context);
        context->markActionsPublished(true);
        // and finally add as active
        m_activeContexts.insert(context);
    }
}
// handles overlay state change of a context
void ActionProxy::handleContextOverlay(bool overlay)
{
    Q_UNUSED(overlay);
}

// empty functions for context activation/deactivation, connect to HUD
void ActionProxy::clearContextActions(UCActionContext *context)
{
    Q_UNUSED(context);
}
/*
 * Publish actions of a context to the system. Implementations should make sure
 * only unpublished actions are exported.
 */
void ActionProxy::publishContextActions(UCActionContext *context)
{
    Q_UNUSED(context);
}
