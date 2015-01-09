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

# Qt 4 configuration
equals( QT_MAJOR_VERSION, 4 ) {
    CONFIG += debug_and_release uitools designer
    QT += core gui network
}

# Qt 5 configuration
equals( QT_MAJOR_VERSION, 5 ) {
    CONFIG += debug_and_release
    QT += core gui network uitools designer
}

## Added by Anton Mezger (check why and include if required)
#unix {
#    QMAKE_CXXFLAGS += "-g"
#    QMAKE_CFLAGS_RELEASE += "-g"
#}


TARGET = qegui
TEMPLATE = app

# Place all intermediate generated files in architecture specific directories
#
MOC_DIR        = O.$$(EPICS_HOST_ARCH)/moc
OBJECTS_DIR    = O.$$(EPICS_HOST_ARCH)/obj
UI_HEADERS_DIR = O.$$(EPICS_HOST_ARCH)/ui_headers

# We don't get this include path for free - need to be explicit.
#
INCLUDEPATH += O.$$(EPICS_HOST_ARCH)/ui_headers


#===========================================================
# Integration of PSI's caQtDM in QEGui.
# If integration is required, define environment variable QE_CAQTDM

_QE_CAQTDM = $$(QE_CAQTDM)
isEmpty( _QE_CAQTDM ) {
    warning( "Integration with PSI's caQtDM will NOT be included in QEGui. If you want caQtDM integrated, download and build it and define the environment variable QE_CAQTDM to point to the caQtDM_Project directory." )
} else {
    warning( "Integration with PSI's caQtDM will be included in QEGui. caQtDM libraries and include files will be expected and be located using the QE_CAQTDM environment variable (which will should point to the to point to the caQtDM_Project directory). Undefine environment variable QE_CAQTDM if you do not want caQtDM integration." )
    DEFINES += QE_USE_CAQTDM
}
#===========================================================


SOURCES += ./src/main.cpp \
    ./src/MainWindow.cpp \
    ./src/StartupParams.cpp \
    ./src/InstanceManager.cpp \
    ./src/saveRestoreManager.cpp \
    ./src/saveDialog.cpp \
    ./src/QEGui.cpp \
    ./src/restoreDialog.cpp \
    ./src/manageConfigDialog.cpp \
    ./src/loginDialog.cpp \
    ./src/recentFile.cpp \
    ./src/aboutDialog.cpp

# Include the following gdbmacros line for debugging only
#SOURCES += <YOUR-QTSDK-DIRECTORY>/share/qtcreator/gdbmacros/gdbmacros.cpp

HEADERS += include/MainWindow.h \
    include/StartupParams.h \
    include/InstanceManager.h \
    include/saveRestoreManager.h \
    include/saveDialog.h \
    include/QEGui.h \
    include/restoreDialog.h \
    include/manageConfigDialog.h \
    include/loginDialog.h \
    include/recentFile.h \
    include/aboutDialog.h

INCLUDEPATH += . \
    ./include \
    ../../framework/widgets/QEWidget \
    ../../framework/widgets/QEForm \
    ../../framework/widgets/QELogin \
    ../../framework/data \
    ../../framework/api \
    ../../framework/common

# Explicity add ../../framework/common, and hence QEFrameworkVersion.h, to the dependacy path
# so that changes to the version/release numbers force relavent recompilations.
#
DEPENDPATH += ../../framework/common

LIBS += -L$$(EPICS_BASE)/lib/$$(EPICS_HOST_ARCH) -lca -lCom

LIBS += -L../../framework/designer -lQEPlugin

FORMS += ./src/MainWindow.ui \
         ./src/MessageLog.ui \
         ./src/saveDialog.ui \
         ./src/restoreDialog.ui \
         ./src/manageConfigDialog.ui \
         ./src/aboutDialog.ui

OTHER_FILES += \
         ./src/General_PV_Edit.ui \
         ./src/Plotter.ui \
         ./src/PVProperties.ui \
         ./src/StripChart.ui \
         ./src/ScratchPad.ui \
         ./src/ArchiveStatus.ui \
         ./src/ArchiveNameSearch.ui \
         ./src/QEGuiCustomisationDefault.xml \
         ./src/help_usage.txt \
         ./src/help_general.txt


RESOURCES += ./src/QEGui.qrc

#===========================================================
# PSI's caQtDM integration
isEmpty( _QE_CAQTDM ) {
} else {
    INCLUDEPATH += $$(QE_CAQTDM)/caQtDM_Lib/src \
                   $$(QE_CAQTDM)/caQtDM_QtControls/src \
                   $(QWTINCLUDE)
    LIBS += -L$$(QE_CAQTDM)/caQtDM_Lib -lcaQtDM_Lib
}
#===========================================================



# end