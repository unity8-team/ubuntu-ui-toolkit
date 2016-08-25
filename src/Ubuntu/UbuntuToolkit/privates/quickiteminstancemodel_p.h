/*
 * Copyright 2016 Canonical Ltd.
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

#ifndef QQUICKITEMINSTANCEMODEL_P_H
#define QQUICKITEMINSTANCEMODEL_P_H

#include <private/qqmlobjectmodel_p.h>

class QQuickItemInstanceModelPrivate;
class QQuickItemInstanceModelAttached;

class QQuickItemInstanceModel : public QQmlInstanceModel
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QQuickItemInstanceModel)

    Q_PROPERTY(QQmlListProperty<QObject> children READ children NOTIFY childrenChanged DESIGNABLE false)
    Q_CLASSINFO("DefaultProperty", "children")

public:
    QQuickItemInstanceModel(QObject *parent=0);
    virtual ~QQuickItemInstanceModel() {}

    int count() const Q_DECL_OVERRIDE;
    bool isValid() const Q_DECL_OVERRIDE;
    QObject *object(int index, bool asynchronous=false) Q_DECL_OVERRIDE;
    ReleaseFlags release(QObject *object) Q_DECL_OVERRIDE;
    QString stringValue(int index, const QString &role) Q_DECL_OVERRIDE;
    void setWatchedRoles(QList<QByteArray>) Q_DECL_OVERRIDE {}

    int indexOf(QObject *object, QObject *objectContext) const Q_DECL_OVERRIDE;

    QQmlListProperty<QObject> children();

    static QQuickItemInstanceModelAttached *qmlAttachedProperties(QObject *obj);

    Q_INVOKABLE QObject *get(int index) const;
    Q_INVOKABLE void append(QObject *object);
    Q_INVOKABLE void insert(int index, QObject *object);
    Q_INVOKABLE void move(int from, int to, int n = 1);
    Q_INVOKABLE void remove(int index, int n = 1);

public Q_SLOTS:
    void clear();

Q_SIGNALS:
    void childrenChanged();

private:
    Q_DISABLE_COPY(QQuickItemInstanceModel)
};

class QQuickItemInstanceModelAttached : public QObject
{
    Q_OBJECT

public:
    QQuickItemInstanceModelAttached(QObject *parent)
        : QObject(parent), m_index(0) {}
    ~QQuickItemInstanceModelAttached() {
        attachedProperties.remove(parent());
    }

    Q_PROPERTY(int index READ index NOTIFY indexChanged)
    int index() const { return m_index; }
    void setIndex(int idx) {
        if (m_index != idx) {
            m_index = idx;
            Q_EMIT indexChanged();
        }
    }

    static QQuickItemInstanceModelAttached *properties(QObject *obj) {
        QQuickItemInstanceModelAttached *rv = attachedProperties.value(obj);
        if (!rv) {
            rv = new QQuickItemInstanceModelAttached(obj);
            attachedProperties.insert(obj, rv);
        }
        return rv;
    }

Q_SIGNALS:
    void indexChanged();

public:
    int m_index;

    static QHash<QObject*, QQuickItemInstanceModelAttached*> attachedProperties;
};

#endif // QQUICKITEMINSTANCEMODEL_P_H
