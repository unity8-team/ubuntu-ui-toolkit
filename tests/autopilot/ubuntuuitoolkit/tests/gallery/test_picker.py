# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
#
# Copyright (C) 2012, 2013 Canonical Ltd.
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

"""Tests for the Ubuntu UI Toolkit Gallery - OptionSelector component"""

import time
from autopilot.matchers import Eventually
from testtools.matchers import Equals
from ubuntuuitoolkit.tests.gallery import GalleryTestCase
from ubuntuuitoolkit.tests import FlickDirection


class Direction:
    """Enum for Circular Picker direction."""

    BACKWARD, FORWARD = range(0, 2)


class PickerTests(GalleryTestCase):
    """Generic tests for the Gallery"""

    def test_picker_dialer(self):
        item = "Pickers"
        self.loadItem(item)
        self.checkPageHeader(item)

        dialer_clock = self.getObject('dialer_clock')
        #Flick upward to reveal the hidden ui element.
        flickable = self.main_view.select_single('QQuickFlickable')
        self.assertIsNotNone(flickable)
        self.reveal_item_by_flick(dialer_clock, flickable, FlickDirection.UP)
        self.assertThat(flickable.flicking, Eventually(Equals(False)))

    def test_picker_dialerhand(self):
        item = "Pickers"
        self.loadItem(item)
        self.checkPageHeader(item)

        dialer_overlay = self.getObject('dialer_overlay')
        #Flick upward to reveal the hidden ui element.
        flickable = self.main_view.select_single('QQuickFlickable')
        self.assertIsNotNone(flickable)
        self.reveal_item_by_flick(dialer_overlay, flickable, FlickDirection.UP)
        self.assertThat(flickable.flicking, Eventually(Equals(False)))

    def test_picker_linear_picker(self):
        item = "Pickers"
        self.loadItem(item)
        self.checkPageHeader(item)
        linear = self.getObject('picker_linear')
        self.assertThat(linear.circular, Equals(False))
        listview = linear.select_single('QQuickListView')
        self.assertIsNotNone(listview)
        count = listview.count
        for i in range(0, count):
            line = linear.select_single('Label', text='Line' + str(i + 1))
            self.assertIsNotNone(line)
            self.pointing_device.click_object(line)
            linear = self.getObject('picker_linear')
            self.assertThat(linear.selectedIndex, Eventually(Equals(i)))

        try:
            #try finding the next item.
            line = linear.select_single('Label', text='Line' + str(count + 1))
            self.assertIsNone(line)
        except:
            pass

    def test_picker_circular_picker(self):
        item = "Pickers"
        self.loadItem(item)
        self.checkPageHeader(item)
        circular = self.getObject('picker_circular')
        self.assertThat(circular.circular, Equals(True))
        selectedIndex = circular.selectedIndex
        #it seems picker needs some time to move to seletected index
        #this requires uses of wait_select_single method in autopilot
        #At time of writing this test, method is in trunk but not yet released.
        #So I am looping it here to mimick the wait and will update
        #once autopilot wait_select_single method becomes available.
        for i in range(0, 10):
            try:
                line = circular.select_single('Label',
                                              text='Line '
                                              + str(selectedIndex))
                self.assertIsNotNone(line)
            except:
                time.sleep(1)

        self.pointing_device.click_object(line)

        pathview = circular.select_single('QQuickPathView')
        self.assertIsNotNone(pathview)
        count = pathview.count
        direction_to_move = (Direction.BACKWARD
                             if selectedIndex <= count / 2
                             else Direction.FORWARD)

        #Test logic should be independent of test data but it doesn't
        #look possible here, so have to rely on test data values in list
        if direction_to_move == Direction.BACKWARD:
            for i in range(selectedIndex, -2, -1):
                line = circular.select_single('Label',
                                              text='Line ' + str(i % count))
                self.assertIsNotNone(line)
                self.pointing_device.click_object(line)
                self.assertThat(circular.selectedIndex,
                                Eventually(Equals(i % count)))
        else:
            for i in range(selectedIndex, count + 2):
                line = circular.select_single('Label',
                                              text='Line ' + str(i % count))
                self.assertIsNotNone(line)
                self.pointing_device.click_object(line)
                self.assertThat(circular.selectedIndex,
                                Eventually(Equals(i % count)))

    def test_picker_infinite_picker(self):
        item = "Pickers"
        self.loadItem(item)
        self.checkPageHeader(item)

        infinite = self.getObject('picker_infinite')
        self.assertThat(infinite.circular, Equals(False))

        #Flick upward to reveal the hidden ui element.
        flickable = self.main_view.select_single('QQuickFlickable')
        self.assertIsNotNone(flickable)
        self.reveal_item_by_flick(infinite, flickable, FlickDirection.UP)
        self.assertThat(flickable.flicking, Eventually(Equals(False)))

        listview = infinite.select_single('QQuickListView')
        self.assertIsNotNone(listview)
        count = listview.count
        for i in range(0, count + 10):
            line = infinite.select_single('Label', text='Line ' + str(i))
            self.assertIsNotNone(line)
            self.pointing_device.click_object(line)
            infinite = self.getObject('picker_infinite')
            self.assertThat(infinite.selectedIndex, Eventually(Equals(i)))
