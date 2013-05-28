# This file is part of the EPICS QT Framework, initially developed at the Australian Synchrotron.
# The EPICS QT Framework is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# The EPICS QT Framework is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
# You should have received a copy of the GNU General Public License
# along with the EPICS QT Framework. If not, see <http://www.gnu.org/licenses/>.
# Copyright (c) 2009, 2010
# Author:
# Andrew Rhyder
# Contact details:
# andrew.rhyder@synchrotron.org.au
# To analyse code performance using the GNU gprof profiling tool:
# - Include the following two lines
# - Clean the project
# - Run qmake
# - Build the code
# - Run the program from a terminal
# - analyse the results with the command: gprof <your-program-name>
#QMAKE_CXXFLAGS_DEBUG += -pg
#QMAKE_LFLAGS_DEBUG += -pg
QT += network
TARGET = QEGui
TEMPLATE = app
CONFIG += uitools \
    debug_and_release
SOURCES += ./src/main.cpp \
    ./src/MainWindow.cpp \
    ./src/StartupParams.cpp \
    ./src/InstanceManager.cpp

# Include the following gdbmacros line for debugging only
#SOURCES += <YOUR-QTSDK-DIRECTORY>/share/qtcreator/gdbmacros/gdbmacros.cpp

HEADERS += include/MainWindow.h \
    include/StartupParams.h \
    include/InstanceManager.h

INCLUDEPATH += . \
    ./include \
    ../../framework/common \
    ../../framework/api/include \
    ../../framework/data/include \
    ../../framework/widgets/include \
    ../../framework/widgets/QEForm \
    ../../framework/plugins/include

# Explicity add ../../framework/common, and hence QEFrameworkVersion.h, to the dependacy path
# So that changes to the version/release numbers forece revent recompilations.
#
DEPENDPATH += ../../framework/common

LIBS += -L../../framework/designer -lQEPlugin
FORMS += ./src/MainWindow.ui \
         ./src/PVProperties.ui \
         ./src/StripChart.ui \
         ./src/UserLevel.ui \
         ./src/MessageLog.ui

RESOURCES += ./src/QEGui.qrc
