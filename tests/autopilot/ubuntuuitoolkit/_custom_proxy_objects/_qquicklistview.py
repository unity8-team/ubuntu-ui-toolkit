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

from autopilot import (
    input,
    logging as autopilot_logging
)
from autopilot.introspection import dbus
from time import sleep
import pdb

from ubuntuuitoolkit._custom_proxy_objects import _common, _flickable, _uclistitem

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
    def find_element(self, object_name, direction=None):
        """Looks for an element in the ListView identifed with a given
           object_name. On success returns the element.

        :parameter object_name: The objectName property of the lookup element

        """
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

#    @autopilot_logging.log_action(logger.info)
#    def drag_item(self, fromIndex, toIndex):
#        """Drags the ListItem. The ListView delegates must be ListItems.
#           ListItems must have objectName set to "listitem"+index.

#           parameters: fromIndex - ListItem index to be dragged
#                       toIndex - ListItem index to be dropped
#        """
#        # bail out if the ListView delegate is not  using ListItems
#        items = self.get_children_by_type('QQuickItem')[0].get_children()
#        if not(isinstance(items[0], _uclistitem.UCListItem)):
#            raise _common.ToolkitException(
#                'ListView must have ListItem as delegate to drag')
#        # also bail out if the two indexes are same or out of count
#        if fromIndex == toIndex:
#            raise _common.ToolkitException(
#                'fromIndex and toIndex are the same.')
#        if fromIndex < 0 or fromIndex > self.count:
#            raise _common.ToolkitException(
#                'fromIndex out of range.')
#        if toIndex < 0 or toIndex > self.count:
#            raise _common.ToolkitException(
#                'toIndex out of range.')
#        direction = 1 if fromIndex < toIndex else -1
#        # bring fromIndex into visible area and move mouse over the drag handler
#        from_item = self.find_element('listitem{}'.format(fromIndex))
#        from_item.swipe_into_view()
#        # bail out if the drag mode is not set
#        if not(from_item.draggable):
#            raise _common.ToolkitException(
#                'ListView must be in drag mode.')
#        drag_panel_name = 'draghandler_panel' + str(fromIndex)
#        drag_handler = from_item.select_single(objectName=drag_panel_name)
#        self.pointing_device.move_to_object(drag_handler)
#        mouse_x = self.pointing_device.x
#        mouse_y = self.pointing_device.y
#        delta = abs(toIndex - fromIndex)
#        move_dy = mouse_y + direction * delta * from_item.height
#        # we cannot move the mouse further than the edges of the ListView
#        hold_at_edge = False
#        if move_dy < self.globalRect.y + self.topMargin:
#            move_dy = self.globalRect.y + self.topMargin
#            hold_at_edge = True
#        if move_dy > self.globalRect.y + self.height:
#            move_dy = self.globalRect.y + self.height
#            hold_at_edge = True
#        # proceed with drag
#        self.pointing_device.press()
#        self.pointing_device.move(mouse_x, move_dy)
#        if hold_at_edge:
#            # hold at edge till we get the targetted item in range
#            fcount = 0
#            while (1):
#                try:
#                    to_name = 'listitem{}'.format(toIndex)
#                    dragged_item = self.select_many('UCListItem', objectName=to_name)
#                    if self.is_child_visible(dragged_item[0]):
#                        break
#                except:
#                    print(fcount)
#                    pass
#                fcount += 1
#                sleep(0.1)
#        self.pointing_device.release()

    def drag_item(self, from_index, to_index):
        both_items_visible = (
            self._is_drag_handler_visible(from_index) and
            self._is_drag_handler_visible(to_index))
        if both_items_visible:
            from_drag_handler = self._get_drag_handler(from_index)
            to_drag_handler = self._get_drag_handler(to_index)
            start_x, start_y = input.get_center_point(from_drag_handler)
            stop_x, stop_y = input.get_center_point(to_drag_handler)
            self.pointing_device.drag(start_x, start_y, stop_x, stop_y)
        else:
            self._drag_item_with_pagination(from_index, to_index)

    def _drag_item_with_pagination(self, from_index, to_index):
        from_drag_handler = self._get_drag_handler(from_index)
        if from_index < to_index:
            containers = self._get_containers()
            visible_bottom = _flickable._get_visible_container_bottom(
                containers)
            start_x, start_y = input.get_center_point(from_drag_handler)
            stop_x = start_x
            stop_y = visible_bottom
            self.pointing_device.drag(start_x, start_y, stop_x, stop_y)

    @autopilot_logging.log_action(logger.debug)
    def get_first_item(self):
        """Returns the first item from the ListView.
        """
        items = self.get_children_by_type('QQuickItem')[0].get_children()
        items = sorted(items, key=lambda item: item.globalRect.y)
        return items[0]

    def _is_drag_handler_visible(self, index):
        try:
            drag_handler = self._get_drag_handler(index)
        except:
            return False
        else:
            return self.is_child_visible(drag_handler)

    def _get_drag_handler(self, index):
        return self.select_single(
            'QQuickItem', objectName='draghandler_panel{}'.format(index))

