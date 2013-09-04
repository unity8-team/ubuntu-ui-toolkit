/*
 * Copyright 2013 Canonical Ltd.
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

#include "statesaver.h"
#include <QtQml/QQmlInfo>
#include <QtQml/QQmlProperty>
#include "private/qqmldata_p.h"
#include "private/qqmlcontext_p.h"
#include <QDebug>

QStringList StateSaverAttached::s_registeredIds = QStringList();
class QQmlComponentAttached;

StateSaverAttached::StateSaverAttached(QObject* attachee) : QObject(attachee),
    m_attachee(attachee),
    m_archive("Canonical", "TestStateSaver"), // FIXME: hardcoded organisation and application name
    m_absoluteId(""),
    m_id(""),
    m_enabled(true)
{
    m_archive.setFallbacksEnabled(false);


    // try to automatically set the id
/*    QString id = computeUniqueId();
    if (!s_registeredIds.contains(id)) {
        m_id = id;
        restore();
    }
    s_registeredIds.append(id);
*/
    /* Save values of registered properties just before m_attachee is destroyed,
       that is when the attached signal Component::destruction is emitted.

       Saving upon destruction of StateSaverAttached would be too late.
       Saving upon emission of the 'destroyed' signal on m_attachee would be too late.
    */
    QString objectId = qmlContext(m_attachee)->parentContext()->nameForObject(m_attachee);
    qDebug() << "CONSTRUCT" << qmlContext(m_attachee) << qmlContext(m_attachee)->parentContext() << objectId;

    QObject* componentAttached = qmlAttachedPropertiesObject<QQmlComponent>(m_attachee);
    QObject::connect(componentAttached, SIGNAL(completed()), this, SLOT(init()));
    QObject::connect(componentAttached, SIGNAL(destruction()), this, SLOT(save()));
}

StateSaverAttached::~StateSaverAttached()
{
    s_registeredIds.removeOne(m_absoluteId);
}

/*
QString StateSaverAttached::id()
{
    return m_id;
}
*/

void StateSaverAttached::setId(QString id)
{
    if (id != m_id) {
        qDebug() << "setId()" << id;
        m_absoluteId = idPath(id);
        s_registeredIds.removeOne(m_absoluteId);
        if (s_registeredIds.contains(m_absoluteId)) {
            qmlInfo(m_attachee) << "StateSaver.id is not a unique identifier. State will not be saved.";
        }
        s_registeredIds.append(m_absoluteId);
        m_id = id;
        Q_EMIT idChanged();
        restore();
    }
}

bool StateSaverAttached::enabled()
{
    return m_enabled;
}

void StateSaverAttached::setEnabled(bool enabled)
{
    QString objectId = qmlContext(m_attachee)->nameForObject(m_attachee);
    qDebug() << "ENABLED" << qmlContext(m_attachee) << qmlContext(m_attachee)->parentContext() << objectId;
    if (enabled != m_enabled) {
        m_enabled = enabled;
        Q_EMIT enabledChanged();
    }
}

QStringList StateSaverAttached::properties()
{
    return m_properties;
}

void StateSaverAttached::setProperties(QStringList properties)
{
    if (properties != m_properties) {
        m_properties = properties;
        Q_EMIT propertiesChanged();
        restore();
    }
}

void StateSaverAttached::save()
{
    if (m_absoluteId.isEmpty() || s_registeredIds.count(m_absoluteId) > 1 || m_properties.isEmpty()) {
        return;
    }

    m_archive.beginGroup(m_absoluteId);

    Q_FOREACH(QString propertyName, m_properties) {
        QQmlProperty qmlProperty(m_attachee, propertyName.toLocal8Bit().constData());
        QVariant value = qmlProperty.read();
        qDebug() << "saving" << m_absoluteId << propertyName << value;
        m_archive.setValue(propertyName, value);
    }

    m_archive.endGroup();
    m_archive.sync();
}

void StateSaverAttached::init() {
    QString objectId = qmlContext(m_attachee)->nameForObject(m_attachee);
    qDebug() << "INIT" << qmlContext(m_attachee) << objectId;
    setId(objectId);
}

void StateSaverAttached::restore()
{
    if (m_absoluteId.isEmpty() || s_registeredIds.count(m_absoluteId) > 1 || m_properties.isEmpty()) {
        return;
    }

    m_archive.beginGroup(m_absoluteId);
    QStringList propertyNames = m_archive.childKeys();
    Q_FOREACH(QString propertyName, propertyNames) {
        QVariant value = m_archive.value(propertyName);
        qDebug() << "restoring" << m_absoluteId << propertyName << value;
        QQmlProperty qmlProperty(m_attachee, propertyName.toLocal8Bit().constData());
        qmlProperty.write(value);
    }
    m_archive.endGroup();
}

QString StateSaverAttached::computeUniqueId()
{
    QString objectId = qmlContext(m_attachee)->nameForObject(m_attachee);
    QString path;
    QString lineNumber;
    QQmlData *ddata = QQmlData::get(m_attachee);
    if (ddata && ddata->outerContext && !ddata->outerContext->urlString.isEmpty()) {
        QQmlContext* context = qmlEngine(m_attachee)->rootContext();
//        qDebug() << qmlEngine(m_attachee) << context->nameForObject(m_attachee);
        path = ddata->outerContext->url.path().replace('/', '_');
        lineNumber = QString::number(ddata->lineNumber);
    }
    // FIXME: are there cases where path & lineNumber are not given?
    // FIXME: objectId not necessary
    return path + ":" + lineNumber + ":" + objectId;
}

QString StateSaverAttached::idPath(QString id)
{
    QString path = id;

    QObject* parent = m_attachee->parent();
    while (parent != NULL) {
        QString parentId = qmlContext(parent)->nameForObject(parent);
        qDebug() << parent << parentId;
        /*QObject* stateSaverAttached = qmlAttachedPropertiesObject<StateSaver>(parent, false);
        if (stateSaverAttached != NULL) {
            qDebug() << parent << parentId << stateSaverAttached << qobject_cast<StateSaverAttached*>(stateSaverAttached)->id();
            path.prepend(qobject_cast<StateSaverAttached*>(stateSaverAttached)->id() + ":");
        } else {
            qDebug() << parent << parentId;
            return QString();
        }*/
        if (!parentId.isEmpty()) {
            path.prepend(parentId + ":");
        } else {
            qmlInfo(m_attachee) << "All the parents need to have an id.";
            return QString();
        }
        parent = parent->parent();
    }

    return path;
}
