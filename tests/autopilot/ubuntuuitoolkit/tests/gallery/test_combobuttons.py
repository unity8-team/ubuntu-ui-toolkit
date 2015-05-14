# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
#
# Copyright (C) 2012, 2013, 2014, 2015 Canonical Ltd.
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

from fixtures import EnvironmentVariable

from ubuntuuitoolkit.tests import gallery
from ubuntuuitoolkit import ubuntu_scenarios


class ComboButtonsTestCase(gallery.GalleryTestCase):

    scenarios = testscenarios.multiply_scenarios(ubuntu_scenarios.get_device_simulation_scenarios())

    def setUp(self):
        self.useFixture(EnvironmentVariable('LANGUAGE', 'en'))
        super().setUp()

    def test_collapsed_combo_button_has_text(self):
        self.open_page('buttonsElement')

        combo_button = self.app.select_single('ComboButton', objectName='collapsed')

        self.assertIsNot(combo_button, None)
        self.assertEquals('Press me', combo_button.text)
        self.assertFalse(combo_button.expanded)

    def test_collapsed_combo_button_has_icon(self):
        self.open_page('buttonsElement')

        combo_button = self.app.select_single('ComboButton', objectName='collapsed_icon')

        self.assertIsNot(combo_button, None)
        self.assertTrue(combo_button.iconSource.endswith('call.png'))
        self.assertFalse(combo_button.expanded)

    def test_collapsed_combo_button_has_icon_and_text(self):
        self.open_page('buttonsElement')

        combo_button = self.app.select_single('ComboButton', objectName='collapsed_icon_and_text')

        self.assertIsNot(combo_button, None)
        self.assertTrue(combo_button.iconSource.endswith('call.png'))
        self.assertEquals('Answer', combo_button.text)
        self.assertFalse(combo_button.expanded)

    def test_expanded_combo_button_has_text_and_correct_size(self):
        self.open_page('buttonsElement')

        combo_button = self.app.select_single('ComboButton', objectName='expanded')
        self.assertIsNot(combo_button, None)
        self.assertEquals('Press me', combo_button.text)
        self.assertTrue(combo_button.expanded)
        self.assertEquals(combo_button.expandedHeight, combo_button.height)

        combo_list = self.app.select_single(objectName='expanded_list')
        self.assertIsNot(combo_list, None)
        self.assertEquals(10, combo_list.count)
