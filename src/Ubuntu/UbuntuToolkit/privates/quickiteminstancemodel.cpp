#include "quickiteminstancemodel_p.h"

#include <QtCore/qcoreapplication.h>
#include <QtQml/qqmlcontext.h>
#include <QtQml/qqmlengine.h>
#include <QtQml/qqmlinfo.h>

#include <private/qqmlchangeset_p.h>
#include <private/qqmlglobal_p.h>
#include <private/qobject_p.h>
#include <private/qpodvector_p.h>

#include <QtCore/qhash.h>
#include <QtCore/qlist.h>

QHash<QObject*, QQuickItemInstanceModelAttached*> QQuickItemInstanceModelAttached::attachedProperties;

class QQuickItemInstanceModelPrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(QQuickItemInstanceModel)
public:
    class Item {
    public:
        Item(QObject *i) : item(i), ref(0) {}

        void addRef() { ++ref; }
        bool deref() { return --ref == 0; }

        QObject *item;
        int ref;
    };

    QQuickItemInstanceModelPrivate() : QObjectPrivate() {}

    static void children_append(QQmlListProperty<QObject> *prop, QObject *item) {
        int index = static_cast<QQuickItemInstanceModelPrivate *>(prop->data)->children.count();
        static_cast<QQuickItemInstanceModelPrivate *>(prop->data)->insert(index, item);
    }

    static int children_count(QQmlListProperty<QObject> *prop) {
        return static_cast<QQuickItemInstanceModelPrivate *>(prop->data)->children.count();
    }

    static QObject *children_at(QQmlListProperty<QObject> *prop, int index) {
        return static_cast<QQuickItemInstanceModelPrivate *>(prop->data)->children.at(index).item;
    }

    static void children_clear(QQmlListProperty<QObject> *prop) {
        static_cast<QQuickItemInstanceModelPrivate *>(prop->data)->clear();
    }

    void insert(int index, QObject *item) {
        Q_Q(QQuickItemInstanceModel);
        children.insert(index, Item(item));
        for (int i = index; i < children.count(); ++i) {
            QQuickItemInstanceModelAttached *attached = QQuickItemInstanceModelAttached::properties(children.at(i).item);
            attached->setIndex(i);
        }
        QQmlChangeSet changeSet;
        changeSet.insert(index, 1);
        Q_EMIT q->modelUpdated(changeSet, false);
        Q_EMIT q->countChanged();
        Q_EMIT q->childrenChanged();
    }

    void move(int from, int to, int n) {
        Q_Q(QQuickItemInstanceModel);
        if (from > to) {
            // Only move forwards - flip if backwards moving
            int tfrom = from;
            int tto = to;
            from = tto;
            to = tto+n;
            n = tfrom-tto;
        }

        QPODVector<QQuickItemInstanceModelPrivate::Item, 4> store;
        for (int i = 0; i < to - from; ++i)
            store.append(children[from + n + i]);
        for (int i = 0; i < n; ++i)
            store.append(children[from + i]);

        for (int i = 0; i < store.count(); ++i) {
            children[from + i] = store[i];
            QQuickItemInstanceModelAttached *attached = QQuickItemInstanceModelAttached::properties(children.at(from + i).item);
            attached->setIndex(from + i);
        }

        QQmlChangeSet changeSet;
        changeSet.move(from, to, n, -1);
        Q_EMIT q->modelUpdated(changeSet, false);
        Q_EMIT q->childrenChanged();
    }

    void remove(int index, int n) {
        Q_Q(QQuickItemInstanceModel);
        for (int i = index; i < index + n; ++i) {
            QQuickItemInstanceModelAttached *attached = QQuickItemInstanceModelAttached::properties(children.at(i).item);
            attached->setIndex(-1);
        }
        children.erase(children.begin() + index, children.begin() + index + n);
        for (int i = index; i < children.count(); ++i) {
            QQuickItemInstanceModelAttached *attached = QQuickItemInstanceModelAttached::properties(children.at(i).item);
            attached->setIndex(i);
        }
        QQmlChangeSet changeSet;
        changeSet.remove(index, n);

        Q_EMIT q->modelUpdated(changeSet, false);
        Q_EMIT q->countChanged();
        Q_EMIT q->childrenChanged();
    }

    void clear() {
        Q_Q(QQuickItemInstanceModel);
        Q_FOREACH (const Item &child, children) {
            Q_EMIT q->destroyingItem(child.item);
        }
        remove(0, children.count());
    }

    int indexOf(QObject *item) const {
        for (int i = 0; i < children.count(); ++i)
            if (children.at(i).item == item)
                return i;
        return -1;
    }


    QList<Item> children;
};



QQuickItemInstanceModel::QQuickItemInstanceModel(QObject *parent)
    : QQmlInstanceModel(*(new QQuickItemInstanceModelPrivate), parent)
{
}

QQmlListProperty<QObject> QQuickItemInstanceModel::children()
{
    Q_D(QQuickItemInstanceModel);
    return QQmlListProperty<QObject>(this,
                                        d,
                                        d->children_append,
                                        d->children_count,
                                        d->children_at,
                                        d->children_clear);
}

int QQuickItemInstanceModel::count() const
{
    Q_D(const QQuickItemInstanceModel);
    return d->children.count();
}

bool QQuickItemInstanceModel::isValid() const
{
    return true;
}

QObject *QQuickItemInstanceModel::object(int index, bool)
{
    Q_D(QQuickItemInstanceModel);
    QQuickItemInstanceModelPrivate::Item &item = d->children[index];
    item.addRef();
    if (item.ref == 1) {
        Q_EMIT initItem(index, item.item);
        Q_EMIT createdItem(index, item.item);
    }
    return item.item;
}

QQmlInstanceModel::ReleaseFlags QQuickItemInstanceModel::release(QObject *item)
{
    Q_D(QQuickItemInstanceModel);
    int idx = d->indexOf(item);
    if (idx >= 0) {
        if (!d->children[idx].deref())
            return QQmlInstanceModel::Referenced;
    }
    return 0;
}

QString QQuickItemInstanceModel::stringValue(int index, const QString &name)
{
    Q_D(QQuickItemInstanceModel);
    if (index < 0 || index >= d->children.count())
        return QString();
    return QQmlEngine::contextForObject(d->children.at(index).item)->contextProperty(name).toString();
}

int QQuickItemInstanceModel::indexOf(QObject *item, QObject *) const
{
    Q_D(const QQuickItemInstanceModel);
    return d->indexOf(item);
}

QQuickItemInstanceModelAttached *QQuickItemInstanceModel::qmlAttachedProperties(QObject *obj)
{
    return QQuickItemInstanceModelAttached::properties(obj);
}

QObject *QQuickItemInstanceModel::get(int index) const
{
    Q_D(const QQuickItemInstanceModel);
    if (index < 0 || index >= d->children.count())
        return 0;
    return d->children.at(index).item;
}

void QQuickItemInstanceModel::append(QObject *object)
{
    Q_D(QQuickItemInstanceModel);
    d->insert(count(), object);
}

void QQuickItemInstanceModel::insert(int index, QObject *object)
{
    Q_D(QQuickItemInstanceModel);
    if (index < 0 || index > count()) {
        qmlInfo(this) << tr("insert: index %1 out of range").arg(index);
        return;
    }
    d->insert(index, object);
}

void QQuickItemInstanceModel::move(int from, int to, int n)
{
    Q_D(QQuickItemInstanceModel);
    if (n <= 0 || from == to)
        return;
    if (from < 0 || to < 0 || from + n > count() || to + n > count()) {
        qmlInfo(this) << tr("move: out of range");
        return;
    }
    d->move(from, to, n);
}

void QQuickItemInstanceModel::remove(int index, int n)
{
    Q_D(QQuickItemInstanceModel);
    if (index < 0 || n <= 0 || index + n > count()) {
        qmlInfo(this) << tr("remove: indices [%1 - %2] out of range [0 - %3]").arg(index).arg(index+n).arg(count());
        return;
    }
    d->remove(index, n);
}

void QQuickItemInstanceModel::clear()
{
    Q_D(QQuickItemInstanceModel);
    d->clear();
}
