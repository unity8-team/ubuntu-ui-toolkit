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

#include "ucstyleditembase_p.h"
#include "ucbutton.h"
#include "uctheme.h"
#include "quickutils.h"

/*!
 * \qmltype Button
 * \instantiates UCButton
 * \inqmlmodule Ubuntu.Components 1.3
 * \ingroup ubuntu
 * \brief Standard Ubuntu button.
 *
 * \l {http://design.ubuntu.com/apps/building-blocks/buttons}{See also the Design Guidelines on Buttons}.
 *
 * Examples:
 * \qml
 *     Column {
 *         Button {
 *             text: "Send"
 *             onClicked: print("clicked text-only Button")
 *         }
 *         Button {
 *             iconName: "compose"
 *             onClicked: print("clicked icon-only Button")
 *         }
 *     }
 * \endqml
 *
 * An \l Action can be used to specify \b clicked, iconSource and text. Example:
 * \qml
 *     Item {
 *         Action {
 *             id: action1
 *             text: "Click me"
 *             onTriggered: print("action!")
 *             iconName: "compose"
 *         }
 *         Button {
 *             anchors.centerIn: parent
 *             action: action1
 *         }
 *    }
 * \endqml
 */
UCButton::UCButton(QQuickItem* parent)
    : UCAbstractButton(parent)
    , m_iconPosition(Before)
    , m_type(Normal)
    , m_emphasis(None)
    , m_font(QFont())
    , m_color(QColor())
    , m_strokeColor(QColor())
    , m_gradient(QGradient())
{
    /*
     * From ucbottomedgehint.cpp:
     * we cannot use setStyleName as that will trigger style loading
     * and the qmlEngine is not known at this phase of the of the initialization
     * Therefore we simply set the style name default. Style loading will
     * happen during component completion.
     */
    UCStyledItemBasePrivate::get(this)->styleDocument = "ButtonStyle";
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
  \li \b Button.Text - The button shape is invisible, only the text or the icon remains.
  \li \b Button.Outline - The button shape is transparent but a border is present.
  \endlist
*/

/*!
  \qmlproperty enumeration Button::emphasis
  The emphasis desired for the button. The default value is \c Button.None.

  \list
  \li \b Button.None - The action is neutral.
  \li \b Button.Positive - The action is positive.
  \li \b Button.Negative - The action is negative.
  \endlist
*/

/*!
  \qmlproperty string Button::font
  The font used for the button's text.
*/

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

/*!
  \qmlproperty string Button::color
  \deprecated
  The property is deprecated, use `emphasis` or a custom style instead.
*/
void UCButton::setColor(QColor &color)
{
    if (color == m_color) {
        return;
    }
    m_color = color;

    static bool logged = false;
    if (!logged) {
        logged = true;
        if (QuickUtils::showDeprecationWarnings()) {
            qmlInfo(this) << "WARNING: `color` is deprecated. "
                             "Use `emphasis` or `StyleHint` instead.";
        }
    }

    Q_EMIT colorChanged();
}

/*!
  \qmlproperty string Button::strokeColor
  \deprecated
  The property is deprecated, set `type` to "outline" instead.
*/
void UCButton::setStrokeColor(QColor &strokeColor)
{
    if (strokeColor == m_strokeColor) {
        return;
    }
    m_strokeColor = strokeColor;

    static bool logged = false;
    if (!logged) {
        logged = true;
        if (QuickUtils::showDeprecationWarnings()) {
            qmlInfo(this) << "WARNING: `strokeColor` is deprecated. "
                             "Set the `type` property to Button.Type.Normal, "
                             "or use `StyleHint` instead.";
        }
    }

    Q_EMIT strokeColorChanged();
}

/*!
  \qmlproperty string Button::gradient
  \deprecated
  The property is deprecated, use `emphasis` or a custom style instead.
*/
void UCButton::setGradient(QGradient &gradient)
{
    if (gradient == m_gradient) {
        return;
    }
    m_gradient = gradient;

    static bool logged = false;
    if (!logged) {
        logged = true;
        if (QuickUtils::showDeprecationWarnings()) {
            qmlInfo(this) << "WARNING: `gradient` is deprecated. "
                             "Use `type`, `emphasis`, or `StyleHint` instead.";
        }
    }

    Q_EMIT gradientChanged();
}
