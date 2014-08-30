#
#    This file is part of the EPICS QT Framework, initially developed at the Australian Synchrotron.
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
#    Copyright (c) 2009, 2010
#
#    Author:
#      Andrew Rhyder
#    Contact details:
#      andrew.rhyder@synchrotron.org.au
#

QT -= gui
TARGET = qemonitor
CONFIG += console
CONFIG -= app_bundle
TEMPLATE = app

# Place all intermediate generated files in architecture specific directories
#
MOC_DIR        = O.$$(EPICS_HOST_ARCH)/moc
OBJECTS_DIR    = O.$$(EPICS_HOST_ARCH)/obj
UI_HEADERS_DIR = O.$$(EPICS_HOST_ARCH)/ui_headers

SOURCES += \
    ./src/main.cpp \
    ./src/monitor.cpp

# Include the following gdbmacros line for debugging only
#SOURCES += <YOUR-QTSDK-DIRECTORY>/share/qtcreator/gdbmacros/gdbmacros.cpp

HEADERS += \
    include/monitor.h

INCLUDEPATH += \
    ./include \
    ../../framework/widgets/QEWidget \
    ../../framework/data \
    ../../framework/api

LIBS += -L../../framework/designer -lQEPlugin

# end
