# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
#
# Copyright (C) 2015 Canonical Ltd.
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

import subprocess


_init_system = None


def get_init_system_module():
    if get_init_system() == 'systemd':
        from ubuntuuitoolkit.init_system import systemd
        return systemd
    else:
        from ubuntuuitoolkit.init_system import upstart
        return upstart


def get_init_system():
    global _init_system
    if _init_system is None:
        # Based on http://ur1.ca/lw3d0
        p1_name = subprocess.check_output(
            'ps -p1 -o comm=', shell=True, universal_newlines=True).strip()
        if p1_name == 'systemd':
            _init_system = p1_name
        else:
            _init_system = 'upstart'
    return _init_system
