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

#ifndef UCSTATESAVERCONFIG_H
#define UCSTATESAVERCONFIG_H

#include <QtCore/QObject>
#include <QtQml/qqml.h>

class UCStateSaverConfig : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool permanent READ permanent WRITE setPermanent NOTIFY permanentChanged)
public:
    explicit UCStateSaverConfig(QObject *parent = 0);

    bool permanent() const;
    void setPermanent(bool permanent);

Q_SIGNALS:
    void permanentChanged();
};

#endif // UCSTATESAVERCONFIG_H
