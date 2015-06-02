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

"""Tests for the Ubuntu UI Toolkit Gallery"""

import os
import shutil

from autopilot.matchers import Eventually
from testtools.matchers import Equals

import ubuntuuitoolkit
from ubuntuuitoolkit import (
    base,
    fixture_setup,
    tests
)


class GalleryTestCase(tests.QMLFileAppTestCase):

    """Base class for gallery test cases."""

    local_desktop_file_path = None

    def setUp(self):
        if self.should_simulate_device():
            # Hide the Unity7 launcher because it takes space that might be
            # needed by the app with the simulated size.
            self.useFixture(fixture_setup.HideUnity7Launcher())
            # This sets the grid units, so it should be called before launching
            # the app.
            self.simulate_device()

        super().setUp()

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

    def launch_application(self):
        if self._application_source_exists():
            self._launch_application_from_source()
        else:
            self._launch_installed_application()
        self.assertThat(
            self.main_view.visible, Eventually(Equals(True)))

    def _application_source_exists(self):
        return 'UBUNTU_UI_TOOLKIT_AUTOPILOT_FROM_SOURCE' in os.environ

    def _launch_installed_application(self):
        self.app = self.launch_upstart_application(
            'ubuntu-ui-toolkit-gallery',
            emulator_base=ubuntuuitoolkit.UbuntuUIToolkitCustomProxyObjectBase)

    def _launch_application_from_source(self):
        test_source_path = self._get_test_source_path()
        desktop_file_path = self._get_desktop_file_path(test_source_path)
        command_line = [
            base.get_toolkit_launcher_command(),
            "-I", tests._get_module_include_path(),
            self._get_test_qml_file_path(test_source_path),
            '--desktop_file_hint={0}'.format(desktop_file_path)
            ]
        self.app = self.launch_test_application(
            *self.get_command_line(command_line),
            emulator_base=ubuntuuitoolkit.UbuntuUIToolkitCustomProxyObjectBase,
            app_type='qt')

    def _get_test_source_path(self):
        test_source_path = self._get_path_to_gallery_source()
        assert os.path.exists(test_source_path)
        return test_source_path

    def _get_path_to_gallery_source(self):
        return os.path.join(
            tests.get_path_to_source_root(), 'examples',
            'ubuntu-ui-toolkit-gallery')

    def _get_test_qml_file_path(self, test_source_path):
        return os.path.join(test_source_path, 'ubuntu-ui-toolkit-gallery.qml')

    def _get_desktop_file_path(self, test_source_path):
        desktop_file_path = os.path.join(
            test_source_path, 'ubuntu-ui-toolkit-gallery.desktop')

        local_desktop_file_dir = (
            tests.get_local_desktop_file_directory())
        if not os.path.exists(local_desktop_file_dir):
            os.makedirs(local_desktop_file_dir)
        local_desktop_file_path = os.path.join(
            local_desktop_file_dir, 'ubuntu-ui-toolkit-gallery.desktop')
        shutil.copy(desktop_file_path, local_desktop_file_path)
        # We can't delete the desktop file before we close the application,
        # so we save it on an attribute to be deleted on tear down.
        self.local_desktop_file_path = local_desktop_file_path
        return local_desktop_file_path

    def open_page(self, page):
        """Open a page of the widget gallery.

        :param page: The objectName of the element in the index list that opens
            the page.

        """
        list_view = self.main_view.select_single(
            ubuntuuitoolkit.QQuickListView, objectName="widgetList")
        list_view.click_element(page)
        element = self.main_view.select_single('Standard', objectName=page)
        element.selected.wait_for(True)

    def tearDown(self):
        super().tearDown()
        # We can't delete the desktop file before we close the application,
        # so we save it on an attribute to be deleted on tear down.
        if self.local_desktop_file_path is not None:
            os.remove(self.local_desktop_file_path)
