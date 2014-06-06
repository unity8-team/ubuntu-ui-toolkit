# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
#
# Copyright (C) 2013, 2014 Canonical Ltd.
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

import ubuntuuitoolkit
from ubuntuuitoolkit import tests

import os
import signal
from autopilot.matchers import Eventually
from testtools.matchers import Equals


defaultGroup = ("""
import QtQuick 2.0
import Ubuntu.Components 1.1

MainView {
    width: units.gu(48)
    height: units.gu(60)
    applicationName: 'once.upon.a.time'

    Settings {
        objectName: 'settings'
        Option {
            id: optionVibrate
            name: "vibrate"
            defaultValue: false
        }
        Option {
            id: optionYear
            name: "year"
            defaultValue: 1960
        }
        Option {
            id: optionHomepage
            name: "homepage"
            defaultValue: "http://www.canonical.com"
        }
        Option {
            id: optionHair
            name: "hairColor"
            defaultValue: 0
        }
    }

    Column {
        Switch {
            action: optionVibrate
            objectName: "vibrateSwitch"
            property bool val: action.value
        }
        TextField {
            action: optionHomepage
            objectName: "homepageEntry"
            property string val: action.value
        }
        TextField {
            action: optionYear
            objectName: "yearEntry"
            property int val: action.value
        }
        OptionSelector {
            action: optionHair
            objectName: "hairColorSelector"
            model: [ "Black", "Ginger", "Peroxided", "White" ]
            property int val: action.value
        }
    }
}
""")

multipleGroups = ("""
import QtQuick 2.0
import Ubuntu.Components 1.1

MainView {
    width: units.gu(48)
    height: units.gu(60)
    applicationName: 'once.upon.a.time'

    Settings {
        objectName: 'settings'
        group: "general"
        Option {
            id: optionVibrate
            name: "vibrate"
            defaultValue: false
        }
        Option {
            id: optionYear
            name: "year"
            defaultValue: 1960
        }
    }
    Settings {
        group: "MiscellaneousOptions"
        Option {
            id: optionHomepage
            name: "homepage"
            defaultValue: "http://www.canonical.com"
        }
    }
    Settings {
        group: "userProfile"
        Option {
            id: optionHair
            name: "hairColor"
            defaultValue: 0
        }
    }

    Column {
        Switch {
            action: optionVibrate
            objectName: "vibrateSwitch"
            property bool val: action.value
        }
        TextField {
            action: optionHomepage
            objectName: "homepageEntry"
            property string val: action.value
        }
        TextField {
            action: optionYear
            objectName: "yearEntry"
            property int val: action.value
        }
        OptionSelector {
            action: optionHair
            objectName: "hairColorSelector"
            model: [ "Black", "Ginger", "Peroxided", "White" ]
            property int val: action.value
        }
    }
}
""")


class SettingsTestCase(tests.QMLStringAppTestCase):

    scenarios = [
        ('defaultGroup', dict(test_qml=defaultGroup)),
        ('multipleGroups', dict(test_qml=multipleGroups)),
    ]

    def setUp(self):
        # Make sure we start with the default settings.
        ubuntuuitoolkit.Settings.clear('once.upon.a.time')
        super(SettingsTestCase, self).setUp()

    def get_settings(self):
        return self.main_view.select_single(
            'Settings', objectName='settings')

    def get_switch(self):
        return self.main_view.select_single(objectName='vibrateSwitch')

    def get_entry(self):
        return self.main_view.select_single(objectName='homepageEntry')

    def get_year_entry(self):
        return self.main_view.select_single(objectName='yearEntry')

    def get_selector(self):
        return self.main_view.select_single('OptionSelector',
                                            objectName='hairColorSelector')

    def test_select_settings_must_return_custom_proxy_object(self):
        self.assertIsInstance(self.get_settings(), ubuntuuitoolkit.Settings)

    def test_application_must_start_with_default_values(self):
        # FIXME: bug #1273956
        #self.assertEqual(self.settings.get_option('vibrate').value, False)
        switch = self.get_switch()

        self.assertThat(switch.checked, Eventually(Equals(False)))
        self.assertThat(switch.val, Equals(switch.checked))

    def test_check_switch_must_update_option_value(self):
        switch = self.get_switch()
        switch.check()

        self.assertThat(switch.checked, Eventually(Equals(True)))
        self.assertThat(switch.val, Equals(switch.checked))

    def test_updated_values_must_be_kept_after_app_restart(self):
        switch = self.get_switch()
        switch.check()
        self.assertThat(switch.checked, Eventually(Equals(True)))
        self.assertThat(switch.val, Equals(switch.checked))

        ubuntuBirth = '2004'
        year = self.get_year_entry()
        year.clear()
        year.write(ubuntuBirth)
        # Leave it as-is for now

        domain = 'http://www.ubuntu.com'
        entry = self.get_entry()
        entry.clear()
        entry.write(domain)
        self.keyboard.press_and_release('Enter')
        self.assertThat(entry.text, Eventually(Equals(domain)))
        self.assertThat(entry.text, Equals(entry.val))

        # Now check that the focus change made the year save
        self.assertThat(year.text, Eventually(Equals(ubuntuBirth)))
        self.assertThat(year.text, Equals(str(year.val)))

        selector = self.get_selector()
        favouriteHairColor = 'Ginger'
        selector.select_option('Label', text=favouriteHairColor)
        index = selector.get_selected_index()
        self.assertThat(index, Eventually(Equals(selector.val)))
        self.assertThat(selector.get_selected_text(),
                        Eventually(Equals(favouriteHairColor)))

        db_file = ubuntuuitoolkit.Settings._get_database_filename(
            'once.upon.a.time')
        assert(os.path.exists(db_file))

        # TODO update this once the restart helpers are implemented in
        # autopilot. See http://pad.lv/1302618 --elopio - 2014-04-04
        os.killpg(self.app.pid, signal.SIGTERM)
        self.launch_application()

        switch = self.get_switch()
        self.assertThat(switch.checked, Eventually(Equals(True)))
        self.assertThat(switch.val, Equals(switch.checked))
        year = self.get_year_entry()
        self.assertThat(year.text, Eventually(Equals(ubuntuBirth)))
        self.assertThat(year.text, Equals(str(year.val)))
        entry = self.get_entry()
        self.assertThat(entry.text, Eventually(Equals(domain)))
        self.assertThat(entry.text, Equals(entry.val))
        selector = self.get_selector()
        index = selector.get_selected_index()
        self.assertThat(index, Eventually(Equals(selector.val)))
        self.assertThat(selector.get_selected_text(),
                        Eventually(Equals(favouriteHairColor)))
