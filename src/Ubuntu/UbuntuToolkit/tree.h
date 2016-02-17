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

#ifndef TREE_H
#define TREE_H

#include <QObject>
#include "ubuntutoolkitglobal.h"

namespace UbuntuToolkit {

class TreePrivate;
class UBUNTUTOOLKIT_EXPORT Tree : public QObject
{
    Q_OBJECT
public:
    explicit Tree(QObject *parent = 0);

    Q_INVOKABLE int index(QObject *node) const;
    Q_INVOKABLE bool add(const int stem, QObject *parentNode,QObject *newNode);
    Q_INVOKABLE QList<QObject *> prune(const int stem);
    Q_INVOKABLE QList<QObject *> chop(QObject *node, bool inclusive);
    Q_INVOKABLE QObject *top(const int stem = 0, const bool exactMatch = false, const int n = 0) const;
    Q_INVOKABLE QObject *parent(QObject *node) const;

private:
    Q_DECLARE_PRIVATE(Tree)
};

}

#endif // TREE_H