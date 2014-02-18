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

#include "ucnativeorientation.h"

/*!
 * \qmltype NativeOrientation
 * \instantiates UCNativeOrientation
 * \inqmlmodule Ubuntu.Components 0.1
 * \ingroup ubuntu
 * \brief UCNativeOrientation determines the native orientation of the device.
 *
 * UCNativeOrientation determines the native orientation of the device.
 */
UCNativeOrientation::UCNativeOrientation(QObject *parent)
 :  QObject(parent)
 ,  m_nativeOrientation(Qt::PrimaryOrientation)
{
    bool landscape = qgetenv("NATIVE_ORIENTATION") == "landscape";
    if (landscape) {
        m_nativeOrientation = Qt::InvertedLandscapeOrientation;
    }
}

/*!
 * \qmlproperty Qt::ScreenOrientation UCNativeOrientation::nativeOrientation
 *
 * This property holds the native orientation of the device
 */
Qt::ScreenOrientation UCNativeOrientation::nativeOrientation()
{
    return m_nativeOrientation;
}
