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

"""Tests for the Ubuntu UI Toolkit Gallery - OptionSelector component."""

from ubuntuuitoolkit import ubuntu_scenarios
from ubuntuuitoolkit.tests.gallery import GalleryTestCase


class ListItemsTestCase(GalleryTestCase):

    scenarios = ubuntu_scenarios.get_device_simulation_scenarios()

    def setUp(self):
        super(ListItemsTestCase, self).setUp()
        self.open_page('listItemsElement')

    def test_delete_listitem(self):
        listItem = self.main_view.select_single(objectName='list_item1')
        listItem.trigger_leading_action('delete_action',
                                        listItem.wait_until_destroyed)
