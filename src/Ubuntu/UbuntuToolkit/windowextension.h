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

#ifndef WINDOWEXTENSION_H
#define WINDOWEXTENSION_H

#include <QtCore/QObject>
#include <QtQml/QQmlListProperty>
#include "ubuntutoolkitglobal.h"

class QQuickWindow;

namespace UbuntuToolkit {

class UBUNTUTOOLKIT_EXPORT WindowExtension : public QObject
{
    Q_OBJECT
//    Q_PROPERTY(QQmlListProperty<QObject> data READ data DESIGNABLE false)
    Q_PROPERTY(int width READ width WRITE setWidth NOTIFY widthChanged2)
public:
    explicit WindowExtension(QObject *parent = 0);
    QQmlListProperty<QObject> data();
    int width() const;
    void setWidth(int width);

Q_SIGNALS:
    void widthChanged2();

private:
    QQuickWindow *window;
};

} // namespace

#endif // WINDOWEXTENSION_H
