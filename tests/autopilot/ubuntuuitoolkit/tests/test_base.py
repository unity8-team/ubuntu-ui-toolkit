# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
#
# Copyright (C) 2013 Canonical Ltd.
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

import testtools
from autopilot import input, platform

from ubuntuuitoolkit import base, tests


class EmptyAppTestCase(base.UbuntuUIToolkitAppTestCase):
    """Empty test case to be used by other tests."""

    def _runTest(self):
        pass


class TestUbuntuUIToolkitAppTestCase(testtools.TestCase):

    @testtools.skipIf(platform.model() != 'Desktop', 'Desktop only')
    def test_desktop_input_device_class(self):
        test = EmptyAppTestCase('_runTest')
        test.setUp()
        self.assertIs(test.input_device_class, input.Mouse)

    @testtools.skipIf(platform.model() == 'Desktop', 'Phablet only')
    def test_phablet_input_device_class(self):
        test = EmptyAppTestCase('_runTest')
        test.setUp()
        self.assertIs(test.input_device_class, input.Touch)


class EmptyQMLStringAppTestCase(tests.QMLStringAppTestCase):
    """Empty test case to be used by other tests."""

    def _runTest(self):
        pass


class TestQMLStringAppTestCase(testtools.TestCase):

    def test_launch_application_not_visible_fails(self):
        test = EmptyQMLStringAppTestCase('_runTest')
        test.launch_application()
        app1 = test.app
        # Launch another application that will be in front of the first one.
        test.launch_application()
        test.app = app1
        self.assertRaises(AssertionError, test._wait_for_application)
