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
 * Author: Pierre Bertet <pierre.bertet@canonical.com>
 */

#ifndef TEXTBUTTON_H
#define TEXTBUTTON_H

#include "ucabstractbutton.h"

namespace UbuntuToolkit {

class TextButtonPrivate;
class TextButton : public UCAbstractButton
{
    Q_OBJECT

    Q_PROPERTY(bool strong READ strong WRITE setStrong NOTIFY strongChanged)

public:
    explicit TextButton(QQuickItem* parent=0);

    bool strong();
    void setStrong(bool &strong);

Q_SIGNALS:
    void strongChanged();

private:
    Q_DECLARE_PRIVATE(TextButton)
    Q_DISABLE_COPY(TextButton)
};

}

QML_DECLARE_TYPE(UbuntuToolkit::TextButton)

#endif  // TEXTBUTTON_H
