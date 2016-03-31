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

#include "textbutton_p.h"
#include "ucstyleditembase_p.h"
#include "ucnamespace.h"

namespace UbuntuToolkit {

TextButtonPrivate::TextButtonPrivate()
    : UCAbstractButtonPrivate()
    , strong(false)
{
}

/*!
\qmltype TextButton
\instantiates UbuntuToolkit::TextButton
\inherits UCAbstractButton
\inqmlmodule Ubuntu.Components 1.3
\ingroup ubuntu
\brief Ubuntu text button.

The TextButton component allows to create a text-only button that follows the Ubuntu visual and behavioral standards.

The button can be strong, which will be represented by a bold font.

\l {http://design.ubuntu.com/apps/building-blocks/buttons}{See also the Design Guidelines on Buttons}.

Example:

\qml
TextButton {
    text: "Send"
    onClicked: print("clicked text-only Button")
}
\endqml

An \l Action can be used to specify \b clicked and \b text:

\qml
Item {
    Action {
       id: myAction
       text: "Click me"
       onTriggered: print("action!")
    }
    TextButton {
       action: myAction
    }
}
\endqml

\sa Button
*/

TextButton::TextButton(QQuickItem* parent)
    : UCAbstractButton(*(new TextButtonPrivate), parent)
{
    /*
     * From ucbottomedgehint.cpp:
     * we cannot use setStyleName as that will trigger style loading
     * and the qmlEngine is not known at this phase of the of the
     * initialization. Therefore we simply set the style name default. Style
     * loading will happen during component completion.
     */
    UCStyledItemBasePrivate::get(this)->styleDocument = QStringLiteral("TextButtonStyle");
}

/*!
   \qmlproperty url TextButton::iconSource
   The source URL of the icon to display inside the button. Leave this value
   blank for a text-only button.
   If \l action is set, the default iconSource is that of the action.
*/

/*!
    \qmlproperty string TextButton::text
    The text to display in the button. If an icon was defined, the text will be
    shown next to the icon, otherwise it will be centered. Leave blank for an
    icon-only button.
    If \l action is set, the default text is that of the action.
*/

/*!
    \qmlsignal TextButton::clicked()
    This handler is called when there is a mouse click on the button and the
    button is not disabled. If \l {ActionItem::action}{action} is defined, the
    action will be triggered.
*/

/*!
    \qmlsignal TextButton::pressAndHold()
    This handler is called when there is a long press.
*/

/*!
    \qmlproperty Action TextButton::action
    An \l Action to specify the clicked and \l text properties.
*/

/*!
    \qmlproperty bool TextButton::strong
    Set to `true` to give importance to the button. The button font will appear
    in bold.
*/
bool TextButton::strong()
{
    Q_D(TextButton);
    return d->strong;
}
void TextButton::setStrong(bool &strong)
{
    Q_D(TextButton);
    d->strong = strong;
    Q_EMIT strongChanged();
}

}
