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

#ifndef BUTTON_H
#define BUTTON_H

#include "ucabstractbutton.h"

namespace UbuntuToolkit {

class ButtonPrivate;
class Button : public UCAbstractButton
{
    Q_OBJECT

    Q_ENUMS(IconPosition)
    Q_ENUMS(Type)

    Q_PROPERTY(Type type READ type WRITE setType NOTIFY typeChanged FINAL)
    Q_PROPERTY(QFont font READ font WRITE setFont NOTIFY fontChanged FINAL)
    Q_PROPERTY(IconPosition iconPosition READ iconPosition WRITE setIconPosition NOTIFY iconPositionChanged FINAL)

    // deprecated properties
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged FINAL)
    Q_PROPERTY(QColor strokeColor READ strokeColor WRITE setStrokeColor NOTIFY strokeColorChanged FINAL)
    Q_PROPERTY(QGradient gradient READ gradient WRITE setGradient NOTIFY gradientChanged FINAL)

public:
    explicit Button(QQuickItem* parent=0);

    enum IconPosition { Before, left = Before, After, right = After };
    enum Type { Normal, Outline, Positive, Negative };

    Type type();
    void setType(Type &type);

    QFont font();
    void setFont(QFont &font);

    IconPosition iconPosition();
    void setIconPosition(IconPosition &iconPosition);

    QColor color();
    void setColor(QColor &color);

    QColor strokeColor();
    void setStrokeColor(QColor &strokeColor);

    QGradient gradient();
    void setGradient(QGradient &gradient);

Q_SIGNALS:
    void typeChanged();
    void fontChanged();
    void iconPositionChanged();
    void colorChanged();
    void strokeColorChanged();
    void gradientChanged();

private:
    Q_DECLARE_PRIVATE(Button)
    Q_DISABLE_COPY(Button)
};

}

QML_DECLARE_TYPE(UbuntuToolkit::Button)

#endif // BUTTON_H
