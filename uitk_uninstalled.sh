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
#  $ ./uitk_uninstalled.sh
#  $ make
#  $ qmlscene uitk_based_file.qml

NAME='uitk'

. `dirname $0`/build_paths.inc
export QML_IMPORT_PATH=$BUILD_DIR/qml
export QML2_IMPORT_PATH=$BUILD_DIR/qml
export UBUNTU_UI_TOOLKIT_THEMES_PATH=$BUILD_DIR/qml
export LD_LIBRARY_PATH=$BUILD_DIR/lib${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}

TMP=`mktemp -t bashrc.XXXXXXXX`
echo source $HOME/.bashrc >> $TMP
echo PS1=\'[$NAME] $PS1\' >> $TMP
SHELL_OPTIONS="--init-file $TMP"

echo Entering $NAME shell.
$SHELL $SHELL_OPTIONS
echo Leaving $NAME shell. Have a nice day!

if test ! -z "$TMP"
then
    rm $TMP
fi
