#!/bin/bash -i
#
# Copyright (C) 2015 Canonical Ltd.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation; version 3.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

# This script spawns a new Bash shell with all the environment variables needed
# to run apps based on the local source tree uninstalled. Note that qmake must
# be run before in order to have the build_paths.inc file generated:
#
#  $ qmake
#  $ ./uitk_shell.sh
#  $ make
#  $ qmlscene uitk_based_file.qml

NAME='uitk'

source `dirname $0`/export_modules_dir.sh

export UITK_SHELL=1

TEMP_FILE=`mktemp -t bashrc.XXXXXXXX`
echo source $HOME/.bashrc >> $TEMP_FILE
echo PS1=\'[$NAME] $PS1\' >> $TEMP_FILE
SHELL_OPTIONS="--init-file $TEMP_FILE"

echo Entering $NAME shell.
$SHELL $SHELL_OPTIONS
echo Leaving $NAME shell. Have a nice day!

if test ! -z "$TEMP_FILE"
then
    rm $TEMP_FILE
fi
