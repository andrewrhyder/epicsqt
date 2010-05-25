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
#    Copyright (c) 2009, 2010
#
#    Author:
#      Andrew Rhyder
#    Contact details:
#      andrew@rhyder.org
#

TARGET = ASgui
TEMPLATE = app
CONFIG += uitools \
    debug_and_release
SOURCES += ./src/main.cpp \
    ./src/MainWindow.cpp \
    ./src/MainContext.cpp \
    ./src/StartupParams.cpp \
    $$(QTCREATOR)/gdbmacros/gdbmacros.cpp
include($$(QCAFRAMEWORK)/qtsingleapplication/src/qtsingleapplication.pri)
HEADERS += include/MainWindow.h \
    include/MainContext.h \
    include/StartupParams.h
INCLUDEPATH += . \
    ./include \
    $$(QCAFRAMEWORK)/data/include \
    $$(QCAFRAMEWORK)/widgets/include \
    $$(QCAFRAMEWORK)/plugins/include
win32:INCLUDEPATH += $$(CYGWIN)/usr/include
LIBS += -L$$(QTDIR)/plugins/designer -lQCaPluginDebug
LIBS += -L/opt/fontconfig/lib
FORMS += ./src/MainWindow.ui
