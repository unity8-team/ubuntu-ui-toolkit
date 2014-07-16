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

import testscenarios
from autopilot.matchers import Eventually
from testtools.matchers import Equals

import ubuntuuitoolkit
from ubuntuuitoolkit import fixture_setup, ubuntu_scenarios


class BaseSliderTestCase(ubuntuuitoolkit.tests.QMLFileAppTestCase):

    path = os.path.abspath(__file__)
    dir_path = os.path.dirname(path)
    test_qml_file_path = os.path.join(
        dir_path, 'test_slider.SliderTestCase.qml')

    def setUp(self):
        super(BaseSliderTestCase, self).setUp()
        # The test slider has a minimum value = -10, maximum value = 10 and
        # starts with value = 0.
        self.slider = self.main_view.select_single(
            ubuntuuitoolkit.Slider, objectName='testSlider')


class SliderCustomProxyObjectTestCase(BaseSliderTestCase):

    def test_get_slider_must_return_custom_proxy_object(self):
        """Test the type of the returned object after selecting the slider.

        It must be the custom proxy object for the Slider.

        """
        self.assertIsInstance(self.slider, ubuntuuitoolkit.Slider)

    def test_select_value_below_minimum_must_raise_exception(self):
        """Test to select a value that is below the slider minimum.

        It must raise a ToolkitException.

        """
        error = self.assertRaises(
            ubuntuuitoolkit.ToolkitException, self.slider.select_value, -15)
        self.assertEqual(
            'The value is lower than the minimum permitted by the slider.',
            str(error))

    def test_select_value_above_maximum_must_raise_exception(self):
        """Test to select a value that is above the slider maximum.

        It must raise a ToolkitException.

        """
        error = self.assertRaises(
            ubuntuuitoolkit.ToolkitException, self.slider.select_value, 15)
        self.assertEqual(
            'The value is higher than the maximum permitted by the slider.',
            str(error))

    def test_select_current_value_must_do_nothing(self):
        """Test selecting the current value on the slider.

        The value must be kept the same and the slider should not be touched.

        """
        with mock.patch.object(self.slider, 'pointing_device') as mock_device:
            self.slider.select_value(0)

        self.assertFalse(mock_device.called)
        self.assertEqual(self.slider.value, 0)

    def test_select_value_not_selectable_must_raise_exception(self):
        """Test selecting a value that's not selectable with the thumb.

        It must raise a ubuntuuitoolkit.ToolkitException.

        """
        error = self.assertRaises(
            ubuntuuitoolkit.ToolkitException, self.slider.select_value, 2.2222)
        self.assertEqual(
            'The value is not selectable on the slider.', str(error))


class SelectSliderValueTestCase(BaseSliderTestCase):

    value_scenarios = [
        ('positive value', {'value': 5}),
        ('negative value', {'value': -5}),
        ('minimum value', {'value': -10}),
        ('maximum value', {'value': 10}),
        ('positive float value', {'value': 2.152777777777777}),
        ('negative float value', {'value': -1.1805555555555554}),
    ]
    scenarios = testscenarios.multiply_scenarios(
        ubuntu_scenarios.get_device_simulation_scenarios(),
        value_scenarios)

    def setUp(self):
        if self.should_simulate_device():
            # Hide the Unity7 launcher because it takes space that might be
            # needed by the app with the simulated size.
            self.useFixture(fixture_setup.HideUnity7Launcher())
            # This sets the grid units, so it should be called before launching
            # the app.
            self.simulate_device()
        super(SelectSliderValueTestCase, self).setUp()

        if self.should_simulate_device():
            # XXX Currently we have no way to launch the application with a
            # specific size, so we must resize it after it's launched.
            # --elopio - 2014-06-25
            self.resize_window()

    def should_simulate_device(self):
        return (hasattr(self, 'app_width') and hasattr(self, 'app_height') and
                hasattr(self, 'grid_unit_px'))

    def simulate_device(self):
        simulate_device_fixture = self.useFixture(fixture_setup.SimulateDevice(
            self.app_width, self.app_height, self.grid_unit_px))
        self.app_width = simulate_device_fixture.app_width
        self.app_height = simulate_device_fixture.app_height

    def resize_window(self):
        application = self.process_manager.get_running_applications()[0]
        window = application.get_windows()[0]
        window.resize(self.app_width, self.app_height)

        def get_window_size():
            _, _, window_width, window_height = window.geometry
            return window_width, window_height

        self.assertThat(
            get_window_size,
            Eventually(Equals((self.app_width, self.app_height))))

    def test_select_valid_value_must_update_slider_value(self):
        """Test that selecting a valid value on the slider must update it."""
        self.slider.select_value(self.value)
        self.assertEqual(self.slider.value, self.value)
