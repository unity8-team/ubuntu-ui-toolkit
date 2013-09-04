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

#ifndef UBUNTU_COMPONENTS_STATE_SAVER_H
#define UBUNTU_COMPONENTS_STATE_SAVER_H

#include <QtCore/QObject>
#include <QtQml>

class StateSaverAttached : public QObject
{
    Q_OBJECT

//    Q_PROPERTY(QString id READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)
    Q_PROPERTY(QStringList properties READ properties WRITE setProperties NOTIFY propertiesChanged)

public:
    StateSaverAttached(QObject* attachee);
    ~StateSaverAttached();

//    QString id();
    bool enabled();
    QStringList properties();

    void setId(QString id);
    void setEnabled(bool enabled);
    void setProperties(QStringList properties);

Q_SIGNALS:
    void propertiesChanged();
    void idChanged();
    void enabledChanged();

protected Q_SLOTS:
    void init();
    void save();
    void restore();
    QString computeUniqueId();
    QString idPath(QString id);

private:
    QObject* m_attachee;
    QSettings m_archive;
    QString m_absoluteId;
    QString m_id;
    bool m_enabled;
    QStringList m_properties;
    static QStringList s_registeredIds;
};

class StateSaver : public QObject
{
    Q_OBJECT
public:
    static StateSaverAttached* qmlAttachedProperties(QObject* attachee)
    {
        return new StateSaverAttached(attachee);
    }
};

QML_DECLARE_TYPEINFO(StateSaver, QML_HAS_ATTACHED_PROPERTIES)

#endif // UBUNTU_COMPONENTS_STATE_SAVER_H
