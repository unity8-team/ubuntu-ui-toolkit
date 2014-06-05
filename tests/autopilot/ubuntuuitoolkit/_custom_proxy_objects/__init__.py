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

import os

class Settings(UbuntuUIToolkitEmulatorBase):
    """Settings Autopilot emulator."""

    @classmethod
    @autopilot_logging.log_action(logger.info)
    def clear(cls, application_name):
        database_file = cls._get_database_filename(application_name)
        if os.path.exists(database_file):
            os.remove(database_file)

    @classmethod
    def _get_database_filename(self, application_name):
        data_home = os.environ.get(
            'XDG_DATA_HOME',
            os.path.join(os.environ['HOME'], '.local', 'share'))
        return os.path.join(data_home, application_name, 'settings.db')

    def get_option(self, optionName):
        return self.select_single('Option', name=optionName)


    def _select_all(self):
        self.pointing_device.click_object(self, press_duration=1)
        root = self.get_root_instance()
        main_view = root.select_single(MainView)
        popover = main_view.get_action_selection_popover('text_input_popover')
        popover.click_button_by_text('Select All')

"""Ubuntu UI Toolkit Autopilot custom proxy objects."""


__all__ = [
    'check_autopilot_version',
    'CheckBox',
    'get_keyboard',
    'get_pointing_device',
    'Header',
    'listitems',
    'MainView',
    'OptionSelector',
    'pickers',
    'popups',
    'QQuickFlickable',
    'QQuickListView',
    'TabBar',
    'Tabs',
    'TextField',
    'Toolbar',
    'ToolkitException',
    'UbuntuUIToolkitCustomProxyObjectBase',
]

from ubuntuuitoolkit._custom_proxy_objects._checkbox import CheckBox
from ubuntuuitoolkit._custom_proxy_objects._common import (
    check_autopilot_version,
    get_keyboard,
    get_pointing_device,
    ToolkitException,
    UbuntuUIToolkitCustomProxyObjectBase,
)
from ubuntuuitoolkit._custom_proxy_objects._flickable import QQuickFlickable
from ubuntuuitoolkit._custom_proxy_objects._header import Header
from ubuntuuitoolkit._custom_proxy_objects import listitems
from ubuntuuitoolkit._custom_proxy_objects._mainview import MainView
from ubuntuuitoolkit._custom_proxy_objects._optionselector import (
    OptionSelector
)
from ubuntuuitoolkit._custom_proxy_objects import pickers
from ubuntuuitoolkit._custom_proxy_objects import popups
from ubuntuuitoolkit._custom_proxy_objects._qquicklistview import (
    QQuickListView
)
from ubuntuuitoolkit._custom_proxy_objects._tabbar import TabBar
from ubuntuuitoolkit._custom_proxy_objects._tabs import Tabs
from ubuntuuitoolkit._custom_proxy_objects._textfield import TextField
from ubuntuuitoolkit._custom_proxy_objects._toolbar import Toolbar
