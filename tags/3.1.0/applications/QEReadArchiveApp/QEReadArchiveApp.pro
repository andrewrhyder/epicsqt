# This file is part of the EPICS QT Framework, initially developed at the Australian Synchrotron.
# The EPICS QT Framework is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# The EPICS QT Framework is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU Lesser General Public License for more details.
# You should have received a copy of the GNU Lesser General Public License
# along with the EPICS QT Framework. If not, see <http://www.gnu.org/licenses/>.
# Copyright (c) 2012
# Author:
# Andrew Starritt
# Contact details:
# andrew.starritt@synchrotron.org.au

QT -= gui
QT += xml network

TARGET = qerad
CONFIG += console
CONFIG -= app_bundle
TEMPLATE = app

# Place all intermediate generated files in architecture specific directories
#
MOC_DIR        = O.$$(EPICS_HOST_ARCH)/moc
OBJECTS_DIR    = O.$$(EPICS_HOST_ARCH)/obj
RCC_DIR        = O.$$(EPICS_HOST_ARCH)/rcc

# Determine EPICS_BASE
_QE_EPICS_BASE = $$(QE_EPICS_BASE)
isEmpty( _QE_EPICS_BASE ) {
    _QE_EPICS_BASE = $$(EPICS_BASE)
    message( QE_EPICS_BASE is not defined. Using EPICS_BASE instead - currently $$_QE_EPICS_BASE )
}

# Check EPICS dependancies
isEmpty( _QE_EPICS_BASE ) {
    error( "EPICS_BASE or QE_EPICS_BASE must be defined. Ensure EPICS is installed and EPICS_BASE or QE_EPICS_BASE is set up." )
}
_EPICS_HOST_ARCH = $$(EPICS_HOST_ARCH)
isEmpty( _EPICS_HOST_ARCH ) {
    error( "EPICS_HOST_ARCH must be defined. Ensure EPICS is installed and EPICS_HOST_ARCH is set up." )
}

DESTDIR = bin

# Install the generated QEReadArchive application in QE_TARGET_DIR if defined.
_QE_TARGET_DIR = $$(QE_TARGET_DIR)
isEmpty( _QE_TARGET_DIR ) {
} else {
    QEREADARCHIVEINSTALL.path = $$(QE_TARGET_DIR)/bin/$$(EPICS_HOST_ARCH)
    unix: QEREADARCHIVEINSTALL.files += $$DESTDIR/$$TARGET
    win32: QEREADARCHIVEINSTALL.files += $$DESTDIR/$$TARGET".exe"
    INSTALLS += QEREADARCHIVEINSTALL
}

#===========================================================
# Project files
#
HEADERS += \
   ./rad_control.h

SOURCES += \
   ./rad.cpp \
   ./rad_control.cpp


INCLUDEPATH += . \
   ../../framework/adaptation_parameters \
   ../../framework/archive \
   ../../framework/common \
   ../../framework/data \
   ../../framework/widgets/QEWidget

OTHER_FILES += \
   ./help_usage.txt \
   ./help_general.txt

RESOURCES +=  \
   ./QEReadArchive.qrc

LIBS += -L../../framework/designer -lQEPlugin
LIBS += -L$$_QE_EPICS_BASE/lib/$$(EPICS_HOST_ARCH) -lca -lCom
# end
