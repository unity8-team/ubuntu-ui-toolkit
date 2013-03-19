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

#ifndef LAYOUTMANAGER_P_H
#define LAYOUTMANAGER_P_H

#include <QQuickItem>

class Layout;
class LayoutManager;

class LayoutManagerPrivate : public QObject {
public:
    LayoutManagerPrivate(QObject *parent, LayoutManager *layoutManager);

    bool updateAutoLayout();

    // callbacks for the "layouts" QQmlListProperty of LayoutManager
    static void append_layout(QQmlListProperty<Layout>*, Layout*);
    static int count_layouts(QQmlListProperty<Layout>*);
    static Layout *at_layout(QQmlListProperty<Layout>*, int);
    static void clear_layouts(QQmlListProperty<Layout>*);

    QList<Layout *> layouts;
    QHash <QString, QQuickItem*> items;
    QString currentLayout;
    LayoutManager *q;
};

#endif
