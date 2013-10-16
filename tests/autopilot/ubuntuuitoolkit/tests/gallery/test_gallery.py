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

"""Tests for the Ubuntu UI Toolkit Gallery"""

import os
import shutil

from autopilot.matchers import Eventually
from testtools.matchers import Is, Not, Equals

from ubuntuuitoolkit import emulators, tests


class GalleryTestCase(tests.QMLFileAppTestCase):
    """Base class for gallery test cases."""

    local_desktop_file_path = None

    def setUp(self):
        self.app_qml_source_path = os.path.join(
            self._get_path_to_gallery_source(),
            'ubuntu-ui-toolkit-gallery.qml')
        self.test_qml_file_path = self._get_test_qml_file_path()
        self.desktop_file_path = self._get_desktop_file_path()
        super(GalleryTestCase, self).setUp()

    def _get_path_to_gallery_source(self):
        return os.path.join(
            tests.get_path_to_source_root(), 'examples',
            'ubuntu-ui-toolkit-gallery')

    def _application_source_exists(self):
        return os.path.exists(self.app_qml_source_path)

    def _get_test_qml_file_path(self):
        if self._application_source_exists():
            return self.app_qml_source_path
        else:
            return os.path.join(
                self._get_path_to_installed_gallery(),
                'ubuntu-ui-toolkit-gallery.qml')

    def _get_path_to_installed_gallery(self):
        return '/usr/lib/ubuntu-ui-toolkit/examples/ubuntu-ui-toolkit-gallery'

    def _get_desktop_file_path(self):
        if self._application_source_exists():
            local_desktop_file_dir = tests.get_local_desktop_file_directory()
            if not os.path.exists(local_desktop_file_dir):
                os.makedirs(local_desktop_file_dir)
            source_desktop_file_path = os.path.join(
                self._get_path_to_gallery_source(),
                'ubuntu-ui-toolkit-gallery.desktop')
            local_desktop_file_path = os.path.join(
                local_desktop_file_dir, 'ubuntu-ui-toolkit-gallery.desktop')
            shutil.copy(source_desktop_file_path, local_desktop_file_path)
            # We can't delete the desktop file before we close the application,
            # so we save it on an attribute to be deleted on tear down.
            self.local_desktop_file_path = local_desktop_file_path
            return local_desktop_file_path
        else:
            return os.path.join(
                self._get_path_to_installed_gallery(),
                'ubuntu-ui-toolkit-gallery.desktop')

    def tearDown(self):
        super(GalleryTestCase, self).tearDown()
        # We can't delete the desktop file before we close the application,
        # so we save it on an attribute to be deleted on tear down.
        if self.local_desktop_file_path is not None:
            os.remove(self.local_desktop_file_path)


class GenericTests(GalleryTestCase):
    """Generic tests for the Gallery"""

    def test_0_can_select_mainwindow(self):
        """Must be able to select the main window."""

        rootItem = self.main_view
        self.assertThat(rootItem, Not(Is(None)))
        self.assertThat(rootItem.visible, Eventually(Equals(True)))

    def test_navigation(self):
        item = "Navigation"
        self.loadItem(item)
        self.checkPageHeader(item)

    def test_slider(self):
        item = "Slider"
        self.loadItem(item)
        self.checkPageHeader(item)

        item_data = [
            ["slider_standard"],
            ["slider_live"],
            ["slider_range"]
        ]

        for data in item_data:
            objName = data[0]
            self.getObject(objName)
            self.tap(objName)

            # TODO: move slider value

#     def test_textarea(self):
#         item = "Text Field"
#         self.loadItem(item)
#         self.checkPageHeader(item)

#         template_textinputs = self.getObject("textinputs")

#         item_data = [
#         ["textarea_default", True, -1, template_textinputs.longText, None ],
#             [ "textarea_expanding", True, -1, "", None],
#         [ "textarea_richtext", True, -1, template_textinputs.richText, None ]
#         ]

#         for data in item_data:
#             objName = data[0]
#             objEnabled = data[1]
#             objEchoMode = data[2]
#             objText = data[3]
#             objNumbersOnly = data[4]

#             obj = self.getObject(objName)
#             self.tap(objName)

#             self.assertThat(obj.enabled, Equals(objEnabled))
# #            self.assertThat(obj.focus, Equals(obj.enabled))
#             self.assertThat(obj.highlighted, Equals(obj.focus))
#             #self.assertThat(obj.hasClearButton, Equals(True))
#             self.assertThat(obj.text, Equals(objText))

            #TODO: There is no clear button or method to clear a textarea?
            #self.tap_clearButton(objName)

            # self.assertThat(obj.text,Equals(""))

            # self.type_string("Hello World!")

            # self.assertThat(obj.text,Equals("Hello World!"))

    def test_progress_and_activity(self):
        item = "Progress and activity"
        self.loadItem(item)
        self.checkPageHeader(item)

        item_data = [
            ["progressbar_standard"],
            ["progressbar_indeterminate"],
            ["activityindicator_standard"]
        ]

        for data in item_data:
            objName = data[0]
            self.getObject(objName)
            self.tap(objName)

            # TODO: check for properties


class TextFieldTestCase(GalleryTestCase):

    scenarios = [
        ('test textfield standard',
         dict(object_name='textfield_standard',
              disabled=False,
              existing_text=None,
              negative_test_text=False,
              new_text=u'Hello World')),
        ('test textfield password',
         dict(object_name='textfield_password',
              disabled=False,
              existing_text='password',
              negative_test_text=False,
              new_text=u'abcdefgh123')),
        ('test textfield numbers',
         dict(object_name='textfield_numbers',
              disabled=False,
              existing_text='123',
              negative_test_text=False,
              new_text=u'0987654321')),
        ('negative test textfield numbers',
         dict(object_name='textfield_numbers',
              disabled=False,
              existing_text='123',
              new_text=None,
              negative_test_text="4a3b2c1",
              negative_expecting_text="4321")),
        ('disabled textfield',
         dict(object_name='textfield_disabled',
              disabled=True)),
    ]

    def test_textfield(self):
        item = "Text Field"
        self.loadItem(item)
        self.checkPageHeader(item)
        test_textfield = self.getObject(self.object_name)
        if self.disabled:
            self.assertFalse(test_textfield.enabled)
            #try tapping a disabled field and verify that focus is false.
            self.pointing_device.click_object(test_textfield)
            self.assertFalse(test_textfield.focus)
        else:
            self.pointing_device.click_object(test_textfield)
            self.assertThat(test_textfield.focus, Eventually(Equals(True)))
            if self.existing_text:
                self.assertThat(test_textfield.text,
                                Equals(self.existing_text))
                self.tap_clearButton(self.object_name)

            if self.new_text:
                self.type_string(self.new_text)
                self.assertThat(test_textfield.text,
                                Eventually(Equals(self.new_text)))

            if self.negative_test_text:
                self.type_string(self.negative_test_text)
                self.assertThat(test_textfield.text,
                                Eventually(
                                    Equals(self.negative_expecting_text)))


class UbuntuShapeTestCase(GalleryTestCase):

    scenarios = [
        ("ubuntushape_color_hex",
         dict(object_name="ubuntushape_color_hex",
              prop="color",
              value=[221, 72, 20, 255])),
        ("ubuntushape_color_lightblue",
         dict(object_name="ubuntushape_color_lightblue",
              prop="color",
              value=[119, 33, 111, 255])),
        ("ubuntushape_color_darkgray",
         dict(object_name="ubuntushape_color_darkgray",
              prop="color",
              value=[174, 167, 159, 255])),
        ("ubuntushape_image",
         dict(object_name="ubuntushape_image",
              prop="image",
              value="map_icon.png")),
        ("ubuntushape_radius_small",
         dict(object_name="ubuntushape_radius_small",
              prop="radius",
              value="small")),
        ("ubuntushape_radius_medium",
         dict(object_name="ubuntushape_radius_medium",
              prop="radius",
              value="medium")),
        ("ubuntushape_sizes_15_6",
         dict(object_name="ubuntushape_sizes_15_6",
              prop=None,
              value=None)),
        ("ubuntushape_sizes_10_14",
         dict(object_name="ubuntushape_sizes_10_14",
              prop=None,
              value=None)),
        ]

    def test_ubuntu_shape(self):
        item = "Ubuntu Shape"
        self.loadItem(item)
        self.checkPageHeader(item)
        self.shape = self.main_view.select_single(
            emulators.UbuntuShape, objectName=self.object_name)

        if self.prop is "color":
            self.assertEqual(self.value, self.shape.color)
        elif self.prop is "radius":
            self.assertEqual(self.value, self.shape.radius)
        elif self.prop is "image":
            image = self.shape.get_image().split('/')
            self.assertEqual(self.value, image[-1])


class TogglesTestCase(GalleryTestCase):

    scenarios = [
        ("checkbox_unchecked", dict(object_name="checkbox_unchecked",
                                    object_checked=False,
                                    object_enabled=True)),
        ("checkbox_checked", dict(object_name="checkbox_checked",
                                  object_checked=True,
                                  object_enabled=True)),
        ("checkbox_disabled_unchecked",
            dict(object_name="checkbox_disabled_unchecked",
                 object_checked=False,
                 object_enabled=False)),
        ("checkbox_disabled_checked",
            dict(object_name="checkbox_disabled_checked",
                 object_checked=True,
                 object_enabled=False)),
        ("switch_unchecked", dict(object_name="switch_unchecked",
                                  object_checked=False,
                                  object_enabled=True)),
        ("switch_checked", dict(object_name="switch_checked",
                                object_checked=True,
                                object_enabled=True)),
        ("switch_disabled_unchecked",
            dict(object_name="switch_disabled_unchecked",
                 object_checked=False,
                 object_enabled=False)),
        ("switch_disabled_checked",
            dict(object_name="switch_disabled_checked",
                 object_checked=True,
                 object_enabled=False)),
    ]

    def test_toggles(self):
        item = "Toggles"
        self.checkListItem(item)
        self.loadItem(item)
        self.checkPageHeader(item)

        obj = self.getObject(self.object_name)
        self.assertThat(obj.checked, Equals(self.object_checked))
        self.assertThat(obj.enabled, Equals(self.object_enabled))

        # try to interact with objects
        self.tap(self.object_name)

        if (obj.enabled):
            self.assertThat(obj.checked, Not(Equals(self.object_checked)))
        else:
            self.assertThat(obj.checked, Equals(self.object_checked))


class ButtonsTestCase(GalleryTestCase):

    scenarios = [
        ('standard button', dict(
            button_name="button_text", is_enabled=True, color=None, icon=None,
            text="Call")),
        ('button with color', dict(
            button_name="button_color", is_enabled=True,
            color=[0, 0, 0, 255], icon=None, text="Call")),
        ('button with icon', dict(
            button_name="button_iconsource", is_enabled=True, color=None,
            icon="call.png", text=None)),
        ('button with icon on the right', dict(
            button_name="button_iconsource_right_text", is_enabled=True,
            color=None, icon="call.png", text="Call")),
        ('button with icon on the left', dict(
            button_name="button_iconsource_left_text", is_enabled=True,
            color=None, icon="call.png", text="Call")),
        ('disabled button', dict(
            button_name="button_text_disabled", is_enabled=False, color=None,
            icon=None, text="Call"))
    ]

    def test_buttons(self):
        item = "Buttons"
        self.loadItem(item)
        self.checkPageHeader(item)

        button = self.app.select_single(objectName=self.button_name)
        self.assertIsNot(button, None)
        self.assertThat(button.enabled, Equals(self.is_enabled))

        if self.color is not None:
            self.assertThat(button.color, Equals(self.color))

        if self.icon is not None:
            self.assertTrue(button.iconSource.endswith(self.icon))

        if self.text is not None:
            self.assertThat(button.text, Equals(self.text))

        # try to interact with objects
        self.pointing_device.move_to_object(button)
        self.pointing_device.press()

        if button.enabled:
            self.assertThat(button.pressed, Eventually(Equals(True)))
        else:
            self.assertFalse(button.pressed)

        self.pointing_device.release()

        self.assertThat(button.pressed, Eventually(Equals(False)))
