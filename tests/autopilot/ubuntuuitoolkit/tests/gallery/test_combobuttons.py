# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
#
# Copyright (C) 2012, 2013, 2014 Canonical Ltd.
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

"""Tests for the Ubuntu UI Toolkit Gallery - ComboButton component"""

import testscenarios
from testtools.matchers import Equals

from ubuntuuitoolkit import ubuntu_scenarios
from ubuntuuitoolkit.tests import gallery

import os


class ComboButtonsTestCase(gallery.GalleryTestCase):

    combo_buttons_scenarios = [
        ('standard combo button', dict(
            combo_button_name="combobutton_collapsed", icon=None, text="Press me", expanded=False)),
        ('standard combo button', dict(
            combo_button_name="combobutton_collapsed_icon", icon="call.png", text=None, expanded=False)),
        ('standard combo button', dict(
            combo_button_name="combobutton_collapsed_icon_and_text", icon="call.png", text="Answer", expanded=False)),
        ('standard combo button', dict(
            combo_button_name="combobutton_expanded", icon=None, text="Press me", expanded=True))
    ]

    scenarios = testscenarios.multiply_scenarios(
        ubuntu_scenarios.get_device_simulation_scenarios(),
        combo_buttons_scenarios)

    def setUp(self):
        # Reset the locale to English
        os.environ['LANGUAGE'] = 'en'
        super().setUp()

    def test_combo_buttons(self):
        self.open_page('buttonsElement')

        combo_button = self.app.select_single(objectName=self.combo_button_name)
        self.assertIsNot(combo_button, None)

        if self.icon is not None:
            self.assertTrue(combo_button.iconSource.endswith(self.icon))

        if self.text is not None:
            self.assertEquals(self.text, combo_button.text)

        if self.expanded:
            self.assertTrue(combo_button.expanded)
            self.assertEquals(combo_button.expandedHeight, combo_button.height)
        else:
            self.assertFalse(combo_button.expanded)
