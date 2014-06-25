# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
#
# Copyright (C) 2014 Canonical Ltd.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation; version 3.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.

import logging

import autopilot.logging

from ubuntuuitoolkit._custom_proxy_objects import _common


logger = logging.getLogger(__name__)


class Slider(_common.UbuntuUIToolkitCustomProxyObjectBase):
    """Autopilot helper for the Slider."""

    @autopilot.logging.log_action(logger.info)
    def select_value(self, value):
        """Select a value from the slider.

        :param value: The value to select.
        :raises ubuntuuitoolkit.ToolkitException: if the value is below the
           minimum or above the minimum, and if the value is not selectable.

        """
        if value < self.minimumValue:
            raise _common.ToolkitException(
                'The value is lower than the minimum permitted by the slider.')
        elif value > self.maximumValue:
            raise _common.ToolkitException(
                'The value is higher than the maximum permitted by the '
                'slider.')
        else:
            self._set_valid_value(value)

    def _set_valid_value(self, value):
        x, y = self._get_value_point_in_bar(value)
        self.pointing_device.move(round(x), round(y))
        self.pointing_device.click()
        if self.value != value:
            raise _common.ToolkitException(
                'The value is not selectable on the slider.')

    def _get_value_point_in_bar(self, value):
        bar_start, bar_end = self._get_bar_start_and_end()
        # We have two points: (min, bar_start) and (max, bar_end).
        # With that we can use linear interpolation to get the x coordinate to
        # click.
        p_x_from_slider = (
            bar_start +
            (bar_end - bar_start) *
            (value - self.minimumValue) /
            (self.maximumValue - self.minimumValue))
        p_x = p_x_from_slider + self.globalRect.x
        # The point to click on the y coordinate is the center of the slider.
        p_y = self.globalRect.y + self.globalRect.height // 2
        return p_x, p_y

    def _get_bar_start_and_end(self):
        # We subtract from the bar width the width of the thumb because the
        # center of the thumb never reaches the start or the end of the bar,
        # so we have to scale down the range of values that we can select.
        thumb = self.select_single(objectName='sliderThumb')
        half_thumb_width = thumb.globalRect.width / 2
        bar = self.select_single(objectName='sliderBar')
        bar_start = half_thumb_width
        bar_end = bar.globalRect.width - half_thumb_width
        return bar_start, bar_end
