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

#include "button_p.h"
#include "ucstyleditembase_p.h"
#include "ucnamespace.h"

namespace UbuntuToolkit {

ButtonPrivate::ButtonPrivate()
    : UCAbstractButtonPrivate()
    , type(Button::Normal)
    , font(QFont())
    , iconPosition(Button::Before)
    , color(QColor())
    , strokeColor(QColor())
    , gradient(QGradient())
{
}

/*!
\qmltype Button
\instantiates UbuntuToolkit::Button
\inherits UCAbstractButton
\inqmlmodule Ubuntu.Components 1.3
\ingroup ubuntu
\brief Standard Ubuntu button.

The Button component allows to create a button that follows the Ubuntu visual and behavioral standards.

Several types are available: normal, the default, will be represented by an opaque shape with the text above it, while the outline type will display a transparent button with a border. The positive and negative types can be used to indicate the nature of the action.

\l {http://design.ubuntu.com/apps/building-blocks/buttons}{See also the Design Guidelines on Buttons}.

Example:
\qml
     Column {
         Button {
             text: "Send"
             onClicked: print("clicked text-only Button")
         }
         Button {
             iconName: "compose"
             onClicked: print("clicked icon-only Button")
         }
     }
\endqml

An \l Action can be used to specify \b clicked, iconSource and text:
\qml
      Item {
          Action {
              id: action1
              text: "Click me"
              onTriggered: print("action!")
              iconName: "compose"
          }
          Button {
              anchors.centerIn: parent
              action: action1
          }
     }
\endqml

\sa TextButton
*/
Button::Button(QQuickItem* parent)
    : UCAbstractButton(*(new ButtonPrivate), parent)
{
    /*
     * From ucbottomedgehint.cpp:
     * we cannot use setStyleName as that will trigger style loading
     * and the qmlEngine is not known at this phase of the of the
     * initialization. Therefore we simply set the style name default. Style
     * loading will happen during component completion.
     */
    UCStyledItemBasePrivate::get(this)->styleDocument = QStringLiteral("ButtonStyle");
}

/*!
   \qmlproperty url Button::iconSource
   The source URL of the icon to display inside the button. Leave this value
   blank for a text-only button.
   If \l action is set, the default iconSource is that of the action.
*/

/*!
   \qmlproperty string Button::text
   The text to display in the button. If an icon was defined, the text will be
   shown next to the icon, otherwise it will be centered. Leave blank for an
   icon-only button.
   If \l action is set, the default text is that of the action.
*/

/*!
 \qmlsignal Button::clicked()
 This handler is called when there is a mouse click on the button and the
 button is not disabled. If \l {ActionItem::action}{action} is defined, the
 action will be triggered.
 */

/*!
 *
 * \qmlsignal Button::pressAndHold()
 * This handler is called when there is a long press.
 */

/*!
   \qmlproperty Action Button::action
   An \l Action to specify the \l clicked, \l iconSource and \l text
   properties.
*/

/*!
  \qmlproperty enumeration Button::type
  The type defines the button style. The default value is \c Button.Normal.
  \list
  \li \b Button.Normal - The button shape is visible.
  \li \b Button.Outline - The button shape is transparent but a border is present.
  \li \b Button.Positive - The action is positive.
  \li \b Button.Negative - The action is negative.
  \endlist
*/
Button::Type Button::type()
{
    Q_D(Button);
    return d->type;
}
void Button::setType(Type &type)
{
    Q_D(Button);
    d->type = type;
    Q_EMIT typeChanged();
}

/*!
  \qmlproperty string Button::font
  The font used for the button's text.
*/
QFont Button::font()
{
    Q_D(Button);
    return d->font;
}
void Button::setFont(QFont &font)
{
    Q_D(Button);
    d->font = font;
    Q_EMIT fontChanged();
}

/*!
  \qmlproperty enumeration Button::iconPosition

  Where to position the icon in the button. The default value is \c Button.Before.

  \note If only text or only an icon is defined, this property is ignored and the
  text or icon is centered horizontally and vertically in the button.

  \list
  \li \b Button.Before - The icon is positioned before the text.
  \li \b Button.After - The icon is positioned after the text.
  \endlist
*/
Button::IconPosition Button::iconPosition()
{
    Q_D(Button);
    return d->iconPosition;
}
void Button::setIconPosition(IconPosition &iconPosition)
{
    Q_D(Button);
    d->iconPosition = iconPosition;
    Q_EMIT iconPositionChanged();
}

/*!
  \qmlproperty string Button::color
  \deprecated
  The property is deprecated, use `emphasis` or a custom style instead.
*/
QColor Button::color()
{
    Q_D(Button);
    return d->color;
}
void Button::setColor(QColor &color)
{
    Q_D(Button);

    qDebug() << color;
    qDebug() << d->color;

    if (color == d->color) {
        return;
    }
    d->color = color;

    qDebug() << color;
    qDebug() << d->color;

    UC_QML_DEPRECATION_WARNING("WARNING: `color` is deprecated. Use `emphasis` or `StyleHint` instead.");

    Q_EMIT colorChanged();
}

/*!
  \qmlproperty string Button::strokeColor
  \deprecated
  The property is deprecated, set `type` to "outline" instead.
*/
QColor Button::strokeColor()
{
    Q_D(Button);
    return d->strokeColor;
}
void Button::setStrokeColor(QColor &strokeColor)
{
    Q_D(Button);
    if (strokeColor == d->strokeColor) {
        return;
    }
    d->strokeColor = strokeColor;

    UC_QML_DEPRECATION_WARNING("WARNING: `strokeColor` is deprecated. Set the `type` property to Button.Type.Normal, or use `StyleHint` instead.");

    Q_EMIT strokeColorChanged();
}

/*!
  \qmlproperty string Button::gradient
  \deprecated
  The property is deprecated, use `type` or a custom style instead.
*/
QGradient Button::gradient()
{
    Q_D(Button);
    return d->gradient;
}
void Button::setGradient(QGradient &gradient)
{
    Q_D(Button);
    if (gradient == d->gradient) {
        return;
    }
    d->gradient = gradient;

    UC_QML_DEPRECATION_WARNING("WARNING: `gradient` is deprecated. Use `type`, `type`, or `StyleHint` instead.");

    Q_EMIT gradientChanged();
}

}
