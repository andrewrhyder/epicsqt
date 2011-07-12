################################################################
# This file is part of the EPICS QT Framework, initially developed at
# the Australian Synchrotron.
#
# The EPICS QT Framework is free software: you can redistribute it
# and/or modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation, either version 3 of
# the License, or (at your option) any later version.
#
# The EPICS QT Framework is distributed in the hope that it will be
# useful, but WITHOUT ANY WARRANTY; without even the implied warranty
# of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with the EPICS QT Framework.  If not, see
# <http://www.gnu.org/licenses/>.
#
#  Copyright (c) 2011
#
# Author:
#   Andrew Rhyder
# Contact details:
#   andrew.rhyder@synchrotron.org.au
#
################################################################

#include( qwtconfig.pri )

TEMPLATE = subdirs
CONFIG   += ordered

SUBDIRS = \
    ca_framework/qwt-6.0.0 \
    ca_framework/plugins \
    applications/ASguiApp \
    applications/QCaMonitorApp

#contains(QWT_CONFIG, QwtDesigner ) {
#    SUBDIRS += designer 
#}
#
#contains(QWT_CONFIG, QwtExamples ) {
#    SUBDIRS += examples 
#}

#qwtspec.files  = qwtconfig.pri qwt.prf
#qwtspec.path  = $${QWT_INSTALL_FEATURES}

#INSTALLS += qwtspec

