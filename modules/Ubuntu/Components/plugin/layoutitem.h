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
 *
 * Author: Gerry Boland <gerry.boland@canonical.com>
 */

#ifndef LAYOUTITEM_H
#define LAYOUTITEM_H

#include <QQuickItem>

class LayoutItem : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QString itemName READ itemName WRITE setItemName NOTIFY itemNameChanged FINAL)
    Q_CLASSINFO("DefaultProperty", "")

public:
    explicit LayoutItem(QQuickItem *parent = 0);
    
    QString itemName() const;
    void setItemName(const QString&);

Q_SIGNALS:
    void itemNameChanged();

private:
    QString m_itemName;
};

#endif // LAYOUTITEM_H
