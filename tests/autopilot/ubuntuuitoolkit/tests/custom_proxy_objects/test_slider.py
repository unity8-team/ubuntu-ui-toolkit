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

import os

try:
    from unittest import mock
except ImportError:
    import mock

import ubuntuuitoolkit


class SliderTestCase(ubuntuuitoolkit.tests.QMLFileAppTestCase):

    path = os.path.abspath(__file__)
    dir_path = os.path.dirname(path)
    test_qml_file_path = os.path.join(
        dir_path, 'test_slider.SliderTestCase.qml')

    def setUp(self):
        super(SliderTestCase, self).setUp()
        # The test slider has a minimum value = -10, maximum value = 10 and
        # starts with value = 0.

        self.slider = self.main_view.select_single(
            ubuntuuitoolkit.Slider, objectName='testSlider')

    def test_get_slider_must_return_custom_proxy_object(self):
        """Test the type of the returned object after selecting the slider.

        It must be the custom proxy object for the Slider.

        """
        self.assertIsInstance(self.slider, ubuntuuitoolkit.Slider)

    def test_set_value_below_minimum_must_raise_exception(self):
        """Test to set a value that is below the slider minimum.

        It must raise a ToolkitException.

        """
        error = self.assertRaises(
            ubuntuuitoolkit.ToolkitException, self.slider.set_value, -15)
        self.assertEqual(
            'The value is lower than the minimum permitted by the slider.',
            str(error))

    def test_set_value_above_maximum_must_raise_exception(self):
        """Test to set a value that is above the slider maximum.

        It must raise a ToolkitException.

        """
        error = self.assertRaises(
            ubuntuuitoolkit.ToolkitException, self.slider.set_value, 15)
        self.assertEqual(
            'The value is higher than the maximum permitted by the slider.',
            str(error))

    def test_set_current_value_must_do_nothing(self):
        """Test setting the current value on the slider.

        The value must be kept the same and the slider should not be touched.

        """
        with mock.patch.object(self.slider, 'pointing_device') as mock_device:
            self.slider.set_value(0)

        self.assertFalse(mock_device.called)
        self.assertEqual(self.slider.value, 0)

    def test_set_valid_value_must_update_slider_value(self):
        """Test that setting a valid value on the slider must update it."""
        self.slider.set_value(5)
        self.assertEqual(self.slider.value, 5)

    # TODO test set minimum.
    # TODO test set maximum.
    # TODO file a bug because the docs doesn't say if min and max are exclusive or inclusive.
    # TODO test set a value that's not selectable with the mouse
    # TODO send Julia the test double post.
