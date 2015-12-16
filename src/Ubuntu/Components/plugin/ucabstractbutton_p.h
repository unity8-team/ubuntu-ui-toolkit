/*
 * Copyright 2015 Canonical Ltd.
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

#ifndef UCABSTRACTBUTTON_P
#define UCABSTRACTBUTTON_P

#include "ucabstractbutton.h"
#include "ucactionitem_p.h"

class UCAbstractButtonPrivate : public UCActionItemPrivate
{
    Q_DECLARE_PUBLIC(UCAbstractButton)
public:
    static UCAbstractButtonPrivate *get(UCAbstractButton *item)
    {
        return item->d_func();
    }

    UCAbstractButtonPrivate();
    void init();

    void completeComponentInitialization() override;

    bool isPressAndHoldConnected();

    // private slots
    void _q_mouseAreaPressed();
    void _q_mouseAreaClicked();
    void _q_mouseAreaPressAndHold();

    QQuickMouseArea *mouseArea;
    bool acceptEvents:1;
    bool pressAndHoldConnected:1;
};

#endif // UCABSTRACTBUTTON_P
