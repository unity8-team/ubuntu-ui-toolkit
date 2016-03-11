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
 *
 * Author: Zsombor Egri <zsombor.egri@canonical.com>
 */

#include "windowextension.h"
#include <QtQuick/private/qquickwindow_p.h>
#include <QtQml/QQmlListProperty>

namespace UbuntuToolkit {

WindowExtension::WindowExtension(QObject *parent)
    : QObject(parent)
    , window(qobject_cast<QQuickWindow*>(parent))
{
    qDebug() << "EXTENDED";
    connect(window, &QQuickWindow::widthChanged, this, &WindowExtension::widthChanged2);
}

QQmlListProperty<QObject> WindowExtension::data()
{
    return QQuickWindowPrivate::get(window)->data();
}

int WindowExtension::width() const
{
    return window->width();
}
void WindowExtension::setWidth(int width)
{
    window->setWidth(width);
}


//#include "moc_windowextension.cpp"

} // namespace
