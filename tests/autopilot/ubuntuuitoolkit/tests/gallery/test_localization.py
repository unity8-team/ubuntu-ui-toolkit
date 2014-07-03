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
import subprocess
import os
import signal


class LocalizationTestCase(GalleryTestCase):

    example_text = 'Hello world'

    scenarios = [
        ('English', dict(
            lang='en_US.utf8', back='Back', hello='Hello world')),
        ('Spanish', dict(
            lang='es_ES.utf8', back='Atrás', hello='Hola al mundo')),
        ('French', dict(
            lang='fr_FR.utf8', back='Retour', hello='Salut la monde')),
        ('German', dict(
            lang='de_DE.utf8', back='Zurück', hello='Hallo Welt')),
    ]

    def is_locale_installed(self, lang):
        locales = str(subprocess.check_output(['locale', '-a']))
        return lang in locales

    def setUp(self):
        super(LocalizationTestCase, self).setUp()

        if self.is_locale_installed(self.lang):
            os.environ["LANGUAGE"] = self.lang
            # TODO update this once the restart helpers are implemented in
            # autopilot. See http://pad.lv/1302618 --elopio - 2014-04-04
            os.killpg(self.app.pid, signal.SIGTERM)
            self.launch_application()

        item = 'localizationElement'
        self.open_page(item)
        element = self.main_view.select_single(
            'Standard', objectName=item)
        self.checkPageHeader(element.text)

    def test_translated_toolkit_string(self):
        if not self.is_locale_installed(self.lang):
            self.skipTest('Locale {} not installed'.format(self.lang))

        item = self.main_view.select_single(objectName='Different_domain')
        self.assertEqual(item.text, self.back)

    def test_untranslated(self):
        if not self.is_locale_installed(self.lang):
            self.skipTest('Locale {} not installed'.format(self.lang))

        item = self.main_view.select_single(objectName='Untranslated')
        self.assertEqual(item.text, self.example_text)

    def test_translated_hello_world(self):
        if not self.is_locale_installed(self.lang):
            self.skipTest('Locale {} not installed'.format(self.lang))

        item = self.main_view.select_single(objectName='Translated')
        self.assertEqual(item.text, self.hello)
