# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
#
# Copyright (C) 2014 Canonical Ltd.
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

import ubuntuuitoolkit
from ubuntuuitoolkit import tests
from ubuntuuitoolkit._custom_proxy_objects import _common


class FlickableTestCase(testtools.TestCase):

    def test_get_unity_top_container(self):
        """Test that we can get the top cointainer in Unity."""
        # This tests bug http://pad.lv/1314390
        # On Unity, the top container is not the first child as it is in all
        # the apps that have a MainView. This makes the first implementation of
        # _get_top_container fails. Instead of going from the top looking for
        # a container, we should start from the flickable until we find the
        # top-most container.
        mock_root_instance = type('obj', (object,), {'id': 'root'})
        # We consider a container is an object with a globalRect.
        mock_non_container = type('obj', (object,), {})
        mock_container = type(
            'obj', (object,), {'id': 'container', 'globalRect': 'dummy'})
        mock_container.get_parent = lambda: mock_root_instance

        # The root instance has two children. This exposes the bug.
        mock_root_instance.get_children = lambda: [
            mock_non_container, mock_container]

        dummy_state = {'id': '10'}
        flickable = ubuntuuitoolkit.Flickable(dummy_state, 'dummy', 'dummy')

        flickable.get_root_instance = lambda: mock_root_instance
        # The top container of the flickable is its immediate parent.
        flickable.get_parent = lambda: mock_container

        top_container = flickable._get_top_container()
        self.assertEqual(top_container, mock_container)

    def test_is_flickable_with_flicking_property_must_return_true(self):
        """is_flickable returns True if flickable property exists."""
        dummy_id = (0, 0)
        dummy_flicking = (0, 'dummy')
        state_with_flicking = {'id': dummy_id, 'flicking': dummy_flicking}
        element = _common.UbuntuUIToolkitCustomProxyObjectBase(
            state_with_flicking, 'dummy', 'dummy')
        with element.no_automatic_refreshing():
            self.assertTrue(element.is_flickable())

    def test_is_flickable_without_flicking_property_must_return_false(self):
        """is_flickable returns False if flickable property doesn't exist."""
        dummy_id = (0, 0)
        state_without_flicking = {'id': dummy_id}
        element = _common.UbuntuUIToolkitCustomProxyObjectBase(
            state_without_flicking, 'dummy', 'dummy')
        with element.no_automatic_refreshing():
            self.assertFalse(element.is_flickable())


class IsFlickableTestCase(tests.QMLStringAppTestCase):
    """Functional test to check that is_flickable returns the right value.

    We already have tests for is_flickable with mocks, so here we just check
    with some real elements.

    """

    test_qml = ("""
import QtQuick 2.0
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem

MainView {
    objectName: 'mainView'
    width: units.gu(48)
    height: units.gu(60)

    Flickable {
        objectName: 'flickable'
    }
    ListView {
        objectName: 'listView'
    }
    Label {
        objectName: 'label'
    }
}
""")

    scenarios = [
        ('main view', dict(object_name='mainView', is_flickable=False)),
        ('flickable', dict(object_name='flickable', is_flickable=True)),
        ('list view', dict(object_name='listView', is_flickable=True)),
        ('label', dict(object_name='label', is_flickable=False))
    ]

    def test_is_flickable(self):
        """Test that is_flickable identifies the elements correctly."""
        element = self.app.select_single(objectName=self.object_name)
        self.assertEqual(element.is_flickable(), self.is_flickable)
