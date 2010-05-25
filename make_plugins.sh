#!/bin/bash

#
#    This file is part of the EPICS QT Framework.
#
#    The EPICS QT Framework is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    The EPICS QT Framework is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with the EPICS QT Framework.  If not, see <http://www.gnu.org/licenses/>.
#
#    Copyright (c) 2009
#
#    Contact details:
#    anthony.owen@gmail.com
#

#
# This is an example build script for the CA QT Designer plugins.
#
# Dependencies: EPICS Base, QT4, QT Designer or QT Creator.

# Points to your current epics build and architecture
export EPICS_BASE=/epics/base
export EPICS_HOST_ARCH=linux-x86_64

# Points to your local QT setup
export QTCREATOR=/usr/share/qtcreator
# $QTDIR is where the plugin library is delivered
export QTDIR=`pwd`
export QCAFRAMEWORK=~/epicsqt/ca_framework

cd ca_framework/plugins
make clean
qmake
make

#For runtime
export LD_LIBRARY_PATH=/epics/base/lib/linux-x86_64

echo
echo
echo "****"
echo "Now copy libQCaPluginDebug.so to your plugins directory e.g /usr/lib/qt4/plugins/designer,"
echo "run designer and click Help -> About Plugins to verify"
echo "****"

#End
