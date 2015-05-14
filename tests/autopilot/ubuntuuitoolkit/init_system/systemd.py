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

import logging
import re
import subprocess

from autopilot import logging as autopilot_logging


logger = logging.getLogger(__name__)


def is_initctl_env_var_set(variable, global_=False):
    """Check True if an initctl environment variable is set.

    :param variable: The name of the variable to check.
    :param global: if True, the method will operate on the global environment
        table. Default is False.
    :return: True if the variable is set. False otherwise.

    """
    variables = _get_initctl_env_vars_dictionary(global_)
    return variable in variables


def _get_initctl_env_vars_dictionary(global_):
    command = ['systemctl', 'show-environment']
    if not global_:
        command += ['--user']
    output = subprocess.check_output(
        command, stderr=subprocess.STDOUT, universal_newlines=True).strip()
    lines = output.split('\n')
    dictionary = {}
    for line in lines:
        match = re.match('(?P<name>[^=]+)=(?P<value>.*)', line)
        dictionary[match.group('name')] = match.group('value')
    return dictionary


def get_initctl_env_var(variable, global_=False):
    """Return the value of an initctl environment variable."""
    variables = _get_initctl_env_vars_dictionary(global_)
    return variables[variable]


@autopilot_logging.log_action(logger.info)
def set_initctl_env_var(variable, value, global_=False):
    """Set the value of an initctl environment variable."""
    command = ['systemctl', 'set-environment', '%s=%s' % (variable, value)]
    if not global_:
        command += ['--user']
    subprocess.call(command, stderr=subprocess.STDOUT, universal_newlines=True)


@autopilot_logging.log_action(logger.info)
def unset_initctl_env_var(variable, global_=False):
    """Remove an initctl environment variable."""
    command = ['systemctl', 'unset-environment', variable]
    if not global_:
        command += ['--user']
    subprocess.call(
        command, stderr=subprocess.STDOUT, universal_newlines=True)
