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

from autopilot import logging as autopilot_logging
from autopilot.introspection import dbus

from ubuntuuitoolkit._custom_proxy_objects import _common


logger = logging.getLogger(__name__)


class ActionSelectionPopover(_common.UbuntuUIToolkitCustomProxyObjectBase):
    """ActionSelectionPopover Autopilot emulator."""


    def click_button(self, object_name):
        """Click a button on the popover.

        :parameter object_name: The QML objectName property of the button.
        :raise ToolkitException: If the object is not found.

        """
        if not self.visible:
            raise _common.ToolkitException('The popover is not open.')
        button = self._get_button(object_name)
        self._click_button(button)


    def click_button_by_text(self, text):
        """Click a button on the popover.

        :parameter text: The text of the button.
        :raise ToolkitException: If the popover is not open.

        """
        if not self.visible:
            raise _common.ToolkitException('The popover is not open.')
        button = self._get_button_by_text(text)
        self._click_button(button)

    def _get_button_by_text(self, text):
        buttons = self.select_many('Empty')
        for obj in buttons:
            if obj.text == text:
                button = obj
                break
        if button is None:
            raise _common.ToolkitException(
                'Button with text "{0}" not found.'.format(text))
        return button

    def _get_button(self, object_name):
        button = self.select_single('Standard', objectName=object_name)
        if button is None:
            raise _common.ToolkitException(
                'Button with text "{0}" not found.'.format(text))
        return button

    def _click_button(self, button):
        self.pointing_device.click_object(button)
        if self.autoClose:
            try:
                #currently this fails due to
                #https://bugs.launchpad.net/ubuntu-ui-toolkit/+bug/1336945
                self.visible.wait_for(False)
            except dbus.StateNotFoundError:
                # The popover was removed from the tree.
                pass

class ComposerSheet(_common.UbuntuUIToolkitCustomProxyObjectBase):
    """ComposerSheet Autopilot emulator."""

    def __init__(self, *args):
        super(ComposerSheet, self).__init__(*args)

    @autopilot_logging.log_action(logger.info)
    def confirm(self):
        """Confirm the composer sheet."""
        button = self.select_single('Button', objectName='confirmButton')
        self.pointing_device.click_object(button)
        self.wait_until_destroyed()

    @autopilot_logging.log_action(logger.info)
    def cancel(self):
        """Cancel the composer sheet."""
        button = self.select_single('Button', objectName='cancelButton')
        self.pointing_device.click_object(button)
        self.wait_until_destroyed()
