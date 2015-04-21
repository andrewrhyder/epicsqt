# QEWidgetDisplayApp.pro
#
# This file is part of the EPICS QT Framework, initially developed at the Australian Synchrotron.
#
# The EPICS QT Framework is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# The EPICS QT Framework is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public License
#  along with the EPICS QT Framework.  If not, see <http://www.gnu.org/licenses/>.
#
#  Copyright (c) 2014 Australian Synchrotron
#
#  Author:
#    Andrew Rhyder
#  Contact details:
#    andrew.rhyder@synchrotron.org.au
#

#
# Simple GUI application to demonstrate programatically adding QE widgets to a user interface
#

QT       += core gui xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QEWidgetDisplay
TEMPLATE = app

# Place all intermediate generated files in architecture specific directories
#
MOC_DIR        = O.$$(EPICS_HOST_ARCH)/moc
OBJECTS_DIR    = O.$$(EPICS_HOST_ARCH)/obj
UI_DIR         = O.$$(EPICS_HOST_ARCH)/ui
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

# Install the generated QEWidgetDisplay application in QE_TARGET_DIR if defined.
_QE_TARGET_DIR = $$(QE_TARGET_DIR)
isEmpty( _QE_TARGET_DIR ) {
} else {
    QEWIDGETDISPLAYINSTALL.path = $$(QE_TARGET_DIR)/bin/$$(EPICS_HOST_ARCH)
    unix: QEWIDGETDISPLAYINSTALL.files += $$DESTDIR/$$TARGET
    win32: QEWIDGETDISPLAYINSTALL.files += $$DESTDIR/$$TARGET".exe"
    INSTALLS += QEWIDGETDISPLAYINSTALL
}

SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui \
    QEWidgetDisplayDialog.ui

INCLUDEPATH += . \
    ./include \
    ../../framework/widgets/QEWidget \
    ../../framework/widgets/QELabel \
    ../../framework/widgets/QELineEdit \
    ../../framework/widgets/QEComboBox \
    ../../framework/widgets/QEButton \
    ../../framework/widgets/QEForm \
    ../../framework/widgets/QEAbstractWidget \
    ../../framework/data \
    ../../framework/api \
    ../../framework/common

LIBS += -L$$_QE_EPICS_BASE/lib/$$(EPICS_HOST_ARCH) -lca -lCom

LIBS += -L../../framework/designer -lQEPlugin

RESOURCES += \
    resources.qrc

