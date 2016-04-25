#!/bin/bash -i

# Copyright © 2016 Canonical Ltd.
# Author: Loïc Molinari <loic.molinari@canonical.com>
#
# This file is part of Quick+.
#
# Quick+ is free software: you can redistribute it and/or modify it under the
# terms of the GNU Lesser General Public License as published by the Free
# Software Foundation; version 3.
#
# Quick+ is distributed in the hope that it will be useful, but WITHOUT ANY
# WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
# A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
# details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with Quick+. If not, see <http://www.gnu.org/licenses/>.

# This script spawns a new Bash shell with all the environment variables needed
# to run apps based on the local source tree uninstalled. Note that qmake must
# be run before in order to have the build_paths.inc file generated:

#  $ qmake
#  $ ./shell.sh
#  [Quick+] $ make
#  [Quick+] $ quick-plus-scene <file>

NAME='Quick+'

BUILD_DIR_FILENAME=`dirname ${BASH_SOURCE[0]}`/../.build_dir.inc
if ! [ -f $BUILD_DIR_FILENAME ]
then
    echo "Generate makefiles with 'qmake' first!"
    exit 1
fi
. $BUILD_DIR_FILENAME || exit 1

export QML2_IMPORT_PATH=$BUILD_DIR/qml
export LD_LIBRARY_PATH=$BUILD_DIR/lib${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}
export PKG_CONFIG_PATH=$BUILD_DIR/lib/pkgconfig${PKG_CONFIG_PATH:+:$PKG_CONFIG_PATH}
export PATH=$BUILD_DIR/bin${PATH:+:$PATH}

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
