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

#ifndef BUTTON_P
#define BUTTON_P

#include "button.h"
#include "ucabstractbutton_p.h"

namespace UbuntuToolkit {

class ButtonPrivate : public UCAbstractButtonPrivate
{
    Q_DECLARE_PUBLIC(Button)
public:
    explicit ButtonPrivate();

    static ButtonPrivate *get(Button *item)
    {
        return item->d_func();
    }

    Button::Type type;
    Button::Emphasis emphasis;
    QFont font;
    Button::IconPosition iconPosition;
    QColor color;
    QColor strokeColor;
    QGradient gradient;
};

}

#endif // BUTTON_P
