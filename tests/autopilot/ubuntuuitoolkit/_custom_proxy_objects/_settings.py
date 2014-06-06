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

import logging
import os

from autopilot import logging as autopilot_logging
from ubuntuuitoolkit._custom_proxy_objects import _common


logger = logging.getLogger(__name__)


class Settings(_common.UbuntuUIToolkitCustomProxyObjectBase):
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
        main_view = root.select_single('MainView')
        popover = main_view.get_action_selection_popover('text_input_popover')
        popover.click_button_by_text('Select All')
