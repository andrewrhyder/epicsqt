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


TEMPLATE = subdirs
CONFIG   += ordered

unix:message( Applications may need to load the framework library directly. To ensure this can happen one option is to set up LD_LIBRARY_PATH to include the directory <your-epicsqt-project-location>/framework/designer. LD_LIBRARY_PATH is currently $(LD_LIBRARY_PATH) )
message( Applications may need to load the framework library as a Qt plugin. To ensure this can happen one option is to set up QT_PLUGIN_PATH to include the directory <your-epicsqt-project-location>/framework. QT_PLUGIN_PATH is currently $(QT_PLUGIN_PATH) )

SUBDIRS = \
    framework \
    applications/QEGuiApp \
    applications/QEMonitorApp

