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
from time import sleep
import pdb

from ubuntuuitoolkit._custom_proxy_objects import _common, _flickable

logger = logging.getLogger(__name__)


class QQuickListView(_flickable.QQuickFlickable):

    @autopilot_logging.log_action(logger.info)
    def click_element(self, object_name, direction=None):
        """Click an element from the list.

        It swipes the element into view if it's center is not visible.

        :parameter objectName: The objectName property of the element to click.
        :parameter direction: The direction where the element is, it can be
            either 'above' or 'below'. Default value is None, which means we
            don't know where the object is and we will need to search the full
            list.

        """
        try:
            element = self.select_single(objectName=object_name)
        except dbus.StateNotFoundError:
            # The element might be on a part of the list that hasn't been
            # created yet. We have to search for it scrolling the entire list.
            element = self._find_element(object_name, direction)
        self.swipe_child_into_view(element)
        self.pointing_device.click_object(element)

    @autopilot_logging.log_action(logger.info)
    def _find_element(self, object_name, direction=None):
        if direction is None:
            # We don't know where the object is so we start looking for it from
            # the top.
            self.swipe_to_top()
            direction = 'below'

        if direction == 'below':
            fail_condition = lambda: self.atYEnd
            swipe_method = self.swipe_to_show_more_below
        elif direction == 'above':
            fail_condition = lambda: self.atYBeginning
            swipe_method = self.swipe_to_show_more_above
        else:
            raise _common.ToolkitException(
                'Invalid direction: {}'.format(direction))

        containers = self._get_containers()
        while not fail_condition():
            try:
                return self.select_single(objectName=object_name)
            except dbus.StateNotFoundError:
                swipe_method(containers)
        else:
            # We have reached the start or the end of the list.
            try:
                return self.select_single(objectName=object_name)
            except dbus.StateNotFoundError:
                raise _common.ToolkitException(
                    'List element with objectName "{}" not found.'.format(
                        object_name))

    def _is_element_clickable(self, object_name):
        child = self.select_single(objectName=object_name)
        containers = self._get_containers()
        return self._is_child_visible(child, containers)

    @autopilot_logging.log_action(logger.info)
    def drag_list_item(self, fromIndex, toIndex):
        """Drags the ListItem. The ListView delegates must be ListItems.
           ListItems must have objectName set to "listitem"+index.

           parameters: fromIndex - ListItem index to be dragged
                       toIndex - ListItem index to be dropped
        """
        direction = 1 if fromIndex < toIndex else -1
        # bring fromIndex into visible area and move mouse over the drag handler
        from_item = self._find_element('listitem' + str(fromIndex))
#        pdb.set_trace()
        # we cannot get ListView.view attached property, so teh assumption is that
        # the parent's parent is the ListView
        view = from_item.get_parent().get_parent()
        name = 'draghandler_panel' + str(fromIndex)
        drag_handler = from_item.select_single(objectName=name)
        self.pointing_device.move_to_object(drag_handler)
        mouse_x = self.pointing_device.x
        mouse_y = self.pointing_device.y
        delta = abs(toIndex - fromIndex)
        move_dy = mouse_y + delta * from_item.height
        # we cannot move the mouse further than the edges of the ListView
        hold_at_edge = False
        if move_dy < view.y:
            move_dy = view.y
            hold_at_edge = True
        if move_dy > view.y + view.height:
            move_dy = view.y + view.height
            hold_at_edge = True
        # proceed with drag
        self.pointing_device.press()
        self.pointing_device.move(mouse_x, move_dy)
        if hold_at_edge:
            # hold at edge till we get the targetted item in range
            fcount = 0
            while (1):
                try:
                    dragged_item = self.select_single('listitem' + str(toIndex))
                    break
                except dbus.StateNotFoundError:
                    print(fcount)
                    fcount += 1
                    pass
                sleep(0.5)
        self.pointing_device.release()

