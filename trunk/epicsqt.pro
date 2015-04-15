################################################################
# This file is part of the EPICS QT Framework, initially developed at
# the Australian Synchrotron.
#
# The EPICS QT Framework is free software: you can redistribute it
# and/or modify it under the terms of the GNU Lesser General Public License
# as published by the Free Software Foundation, either version 3 of
# the License, or (at your option) any later version.
#
# The EPICS QT Framework is distributed in the hope that it will be
# useful, but WITHOUT ANY WARRANTY; without even the implied warranty
# of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with the EPICS QT Framework.  If not, see
# <http://www.gnu.org/licenses/>.
#
#  Copyright (c) 2011, 2013
#
# Author:
#   Andrew Rhyder
# Contact details:
#   andrew.rhyder@synchrotron.org.au
#
################################################################


TEMPLATE = subdirs
CONFIG   += ordered

SUBDIRS = \
    framework \
    applications/QEGuiApp \
    applications/QEReadArchiveApp \
    applications/QEMonitorApp \
    applications/QEWidgetDisplayApp

# Install the documentaion in QE_TARGET_DIR if defined.
_QE_TARGET_DIR = $$(QE_TARGET_DIR)
isEmpty( _QE_TARGET_DIR ) {
    message( "QE_TARGET_DIR is not defined. No installation step (make install) will be available" )
} else {
    qeguiDoco.path = $$(QE_TARGET_DIR)/documentation
    qeguiDoco.files += ./documentation/*.pdf
    INSTALLS += qeguiDoco
}


# end
