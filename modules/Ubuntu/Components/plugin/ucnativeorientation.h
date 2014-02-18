/*
 * Copyright © 2014 Canonical Ltd.
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License version 3, as published by
 * the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranties of MERCHANTABILITY,
 * SATISFACTORY QUALITY, or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authored by: Gerry Boland <gerry.boland@canonical.com>
 */

#ifndef NATIVEORIENTATION_H
#define NATIVEORIENTATION_H

#include <QObject>

class UCNativeOrientation : public QObject
{
    Q_OBJECT
    Q_PROPERTY(Qt::ScreenOrientation nativeOrientation READ nativeOrientation CONSTANT)

public:
    explicit UCNativeOrientation(QObject* parent = 0);

    // getter
    Qt::ScreenOrientation nativeOrientation();

private:
    Qt::ScreenOrientation m_nativeOrientation;
};

#endif // NATIVEORIENTATION_H
