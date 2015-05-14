# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
#
# Copyright (C) 2014, 2015 Canonical Ltd.
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


from ubuntuuitoolkit import init_system


def is_initctl_env_var_set(variable, global_=False):
    """Check True if an initctl environment variable is set.

    :param variable: The name of the variable to check.
    :param global: if True, the method will operate on the global environment
        table. Default is False.
    :return: True if the variable is set. False otherwise.

    """
    init = init_system.get_init_system_module()
    return init.is_initctl_env_var_set(variable, global_)


def get_initctl_env_var(variable, global_=False):
    """Return the value of an initctl environment variable."""
    init = init_system.get_init_system_module()
    return init.get_initctl_env_var(variable, global_)


def set_initctl_env_var(variable, value, global_=False):
    """Set the value of an initctl environment variable."""
    init = init_system.get_init_system_module()
    init.set_initctl_env_var(variable, value, global_)


def unset_initctl_env_var(variable, global_=False):
    """Remove an initctl environment variable."""
    init = init_system.get_init_system_module()
    init.unset_initctl_env_var(variable, global_)
