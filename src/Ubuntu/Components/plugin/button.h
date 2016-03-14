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

#ifndef BUTTONBASE_H
#define BUTTONBASE_H

#include "ucabstractbutton.h"

namespace UbuntuToolkit {

class Button : public UCAbstractButton
{
    Q_OBJECT

    Q_ENUMS(IconPosition)
    Q_ENUMS(Type)
    Q_ENUMS(Emphasis)

    Q_PROPERTY(Type type MEMBER m_type NOTIFY typeChanged FINAL)
    Q_PROPERTY(Emphasis emphasis MEMBER m_emphasis NOTIFY emphasisChanged FINAL)
    Q_PROPERTY(QFont font MEMBER m_font NOTIFY fontChanged FINAL)
    Q_PROPERTY(IconPosition iconPosition MEMBER m_iconPosition NOTIFY iconPositionChanged FINAL)

    // deprecated
    Q_PROPERTY(QColor color MEMBER m_color WRITE setColor NOTIFY colorChanged FINAL)
    Q_PROPERTY(QColor strokeColor MEMBER m_strokeColor WRITE setStrokeColor NOTIFY strokeColorChanged FINAL)
    Q_PROPERTY(QGradient gradient MEMBER m_gradient WRITE setGradient NOTIFY gradientChanged FINAL)

public:
    explicit Button(QQuickItem* parent=0);

    enum IconPosition { Before, left = Before, After, right = After };
    enum Type { Normal, Outline };
    enum Emphasis { None, Positive, Negative };

    // deprecated
    void setColor(QColor &color);
    void setStrokeColor(QColor &strokeColor);
    void setGradient(QGradient &gradient);

Q_SIGNALS:
    void iconPositionChanged();
    void typeChanged();
    void emphasisChanged();
    void fontChanged();

    // deprecated
    void colorChanged();
    void strokeColorChanged();
    void gradientChanged();

private:
    IconPosition m_iconPosition;
    Type m_type;
    Emphasis m_emphasis;
    QFont m_font;

    // deprecated
    QColor m_color;
    QColor m_strokeColor;
    QGradient m_gradient;
};

}

QML_DECLARE_TYPE(UbuntuToolkit::Button)

#endif  // BUTTONBASE_H
