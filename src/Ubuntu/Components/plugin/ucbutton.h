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
 *
 * Author: Pierre Bertet <pierre.bertet@canonical.com>
 */

#ifndef UCBUTTONBASE_H
#define UCBUTTONBASE_H

#include "ucabstractbutton.h"

class UCButton : public UCAbstractButton
{
    Q_OBJECT

    Q_ENUMS(IconInsertion)
    Q_ENUMS(Type)
    Q_ENUMS(Emphasis)

    Q_PROPERTY(
        Type type
        MEMBER m_type
        NOTIFY typeChanged
    )
    Q_PROPERTY(
        Emphasis emphasis
        MEMBER m_emphasis
        NOTIFY emphasisChanged
    )
    Q_PROPERTY(
        QFont font
        MEMBER m_font
        NOTIFY fontChanged
    )
    Q_PROPERTY(
        IconInsertion iconInsertion
        MEMBER m_iconInsertion
        NOTIFY iconInsertionChanged
    )

    // deprecated
    Q_PROPERTY(
        QString iconPosition
        READ iconPosition
        WRITE setIconPosition
        NOTIFY iconPositionChanged
    )
    Q_PROPERTY(
        QColor color
        MEMBER m_color
        WRITE setColor
        NOTIFY colorChanged
    )
    Q_PROPERTY(
        QColor strokeColor
        MEMBER m_strokeColor
        WRITE setStrokeColor
        NOTIFY strokeColorChanged
    )
    Q_PROPERTY(
        QGradient gradient
        MEMBER m_gradient
        WRITE setGradient
        NOTIFY gradientChanged
    )

public:
    UCButton(QQuickItem* parent=0);

    enum IconInsertion { Before, After };
    enum Type { Normal, Text, Outline };
    enum Emphasis { None, Positive, Negative };

    // deprecated
    void setIconPosition(QString &newIconPosition);
    void setIconInsertion(IconInsertion &iconInsertion);
    void setColor(QColor &color);
    void setStrokeColor(QColor &strokeColor);
    void setGradient(QGradient &gradient);

Q_SIGNALS:
    void typeChanged();
    void emphasisChanged();
    void fontChanged();
    void iconInsertionChanged();

    // deprecated
    void iconPositionChanged();
    void colorChanged();
    void strokeColorChanged();
    void gradientChanged();

private:
    Type m_type;
    Emphasis m_emphasis;
    QFont m_font;
    IconInsertion m_iconInsertion;

    // deprecated
    QString iconPosition();
    QColor m_color;
    QColor m_strokeColor;
    QGradient m_gradient;
};

QML_DECLARE_TYPE(UCButton)

#endif  // UCBUTTONBASE_H
