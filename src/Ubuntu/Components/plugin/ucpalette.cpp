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

#include "ucpalette.h"

/*!
 * \qmltype Palette
 * \instantiates UCPalette
 * \inqmlmodule Ubuntu.Components.Themes 1.0
 * \ingroup theme-module
 * \brief Provides the palette of colors from the theme that widgets use to draw themselves.
 *
 * Palette provides access to colors defined by the current theme. The actual
 * color palette to use depends on the state of the widget being drawn.
 *
 * A color palette (\l PaletteValues) has various roles to choose from depending
 * on what part of the widget is being drawn (the base of the widget, the
 * foreground, etc.).
 *
 * For each color there is a corresponding 'text' color that is guaranteed to
 * provide good legibility for text or icons placed on top of a background
 * of the corresponding color.
 *
 * Example of a Text on top of a background Rectangle:
 * \qml
 * Item {
 *     Rectangle {
 *         color: theme.palette.normal.base
 *     }
 *
 *     Text {
 *         color: theme.palette.normal.baseText
 *     }
 * }
 * \endqml
 */

UCPalette::UCPalette(QObject *parent)
    : QObject(parent)
    , m_normal(new UCPaletteValues(this))
    , m_selected(new UCPaletteValues(this))
{
}

/*!
 * \qmlproperty PaletteValues Palette::normal
 *
 * Color palette to use when the widget is not in any particular state.
 */
/*!
 * \qmlproperty PaletteValues Palette::selected
 * Color palette to use when the widget is selected, for example when a tab
 * is the current one.
 */

/*!
 * \qmltype PaletteValues
 * \instantiates UCPaletteValues
 * \inqmlmodule Ubuntu.Components.Themes 1.0
 * \ingroup theme-module
 * \brief Color values used for a given widget state.
 */
UCPaletteValues::UCPaletteValues(QObject *parent)
    : QObject(parent)
{
}

/*!
 * \qmlproperty color background
 *  Color applied to the background of the application.
 */

/*!
 * \qmlproperty color backgroundText
 * Color applied to elements placed on top of the \l background color.
 * Typically used for labels and images.
 */

/*!
 * \qmlproperty color base
 * Color applied to the background of widgets.
 */

/*!
 * \qmlproperty color baseText
 * Color applied to elements placed on top of the \l base color.
 * Typically used for labels and images.
 */

/*!
 * \qmlproperty color foreground
 * Color applied to widgets on top of the base colour.
 */

/*!
 * \qmlproperty color foregroundText
 * Color applied to elements placed on top of the \l foreground color.
 * Typically used for labels and images.
 */

/*!
 * \qmlproperty color overlay
 * Color applied to the background of widgets floating over other widgets.
 * For example: popovers, Toolbar.
 */

/*!
 * \qmlproperty color overlayText
 * Color applied to elements placed on top of the \l overlay color.
 * Typically used for labels and images.
 */

/*!
 * \qmlproperty color field
 * Colour applied to the backgrouhnd of text input fields.
 */

/*!
 * \qmlproperty color fieldText
 * Color applied to elements placed on top of the \l field color.
 * Typically used for labels and images.
 */

/*!
 * \qmlproperty color selection
 * Color applied to selected text in editable components.
 */

