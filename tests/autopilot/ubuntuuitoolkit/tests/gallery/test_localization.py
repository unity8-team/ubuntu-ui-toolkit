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

"""Tests for the Ubuntu UI Toolkit Gallery - TextInput components"""

from ubuntuuitoolkit.tests.gallery import GalleryTestCase
# import locale


class LocalizationTestCase(GalleryTestCase):

    scenarios = [
        ('English', dict(
            lang='en_US.utf8', back='Back')),
        ('Spanish', dict(
            lang='es_ES.utf8', back='Atrás')),
    ]


def setUp(self):
    super(LocalizationTestCase, self).setUp()
    # Apply the user locale from the environment
    # The UITK does the same, so the test must be localized
    # locale.setlocale(locale.LC_ALL, "")
    item = 'Localization'
    self.loadItem(item)
    self.checkPageHeader(item)


def test_translated_toolkit_string(self):
    if False:
        self.skipTest('System locale not available for testing')

    self.main_view.select_single('Subtitled', text=self.back)
