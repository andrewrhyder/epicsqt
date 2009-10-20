#!/bin/bash

#
# This is an example build script for the CAQT designer plugins.
#
# Dependencies: EPICS Base, QT4, QT Creator, QT Designer
#

# Points to your current epics build and architecture
export EPICS_BASE=/epics/base
export EPICS_HOST_ARCH=linux-x86_64

# Points to your local QT setup
export QTCREATOR=/usr/share/qtcreator
# $QTDIR/plugins/designer is where the plugin library is delivered
export QTDIR=/usr/lib/qt4
export QCAFRAMEWORK=~/epicsqt/ca_framework

cd ca_framework/plugins
make clean
qmake
make

#For runtime
export LD_LIBRARY_PATH=/epics/base/lib/linux-x86_64

#End
