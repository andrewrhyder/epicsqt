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
    CONFIG += uitools designer
    QT += core gui network
}

# Qt 5 configuration
equals( QT_MAJOR_VERSION, 5 ) {
    QT += core gui network uitools designer
}

## Added by Anton Mezger (check why and include if required)
#unix {
#    QMAKE_CXXFLAGS += "-g"
#    QMAKE_CFLAGS_RELEASE += "-g"
#}


TARGET = qegui
TEMPLATE = app

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

# Install the generated QEGui application in QE_TARGET_DIR if defined.
_QE_TARGET_DIR = $$(QE_TARGET_DIR)
isEmpty( _QE_TARGET_DIR ) {
} else {
    QEGUIINSTALL.path = $$(QE_TARGET_DIR)/bin/$$(EPICS_HOST_ARCH)
    unix: QEGUIINSTALL.files += $$DESTDIR/$$TARGET
    win32: QEGUIINSTALL.files += $$DESTDIR/$$TARGET".exe"
    INSTALLS += QEGUIINSTALL
}

# Place all intermediate generated files in architecture specific directories
#
MOC_DIR        = O.$$(EPICS_HOST_ARCH)/moc
OBJECTS_DIR    = O.$$(EPICS_HOST_ARCH)/obj
UI_DIR         = O.$$(EPICS_HOST_ARCH)/ui
RCC_DIR        = O.$$(EPICS_HOST_ARCH)/rcc

# We don't get this include path for free - need to be explicit.
#
INCLUDEPATH += O.$$(EPICS_HOST_ARCH)/ui


#===========================================================
# Integration of PSI's caQtDM in QEGui.
# If integration is required, define environment variable QE_CAQTDM

_QE_CAQTDM = $$(QE_CAQTDM)
isEmpty( _QE_CAQTDM ) {
    message( "Integration with PSI's caQtDM will NOT be included in QEGui. If you want caQtDM integrated, download and build it and define the environment variable QE_CAQTDM to point to the caQtDM_Project directory." )
} else {
    message( "Integration with PSI's caQtDM will be included in QEGui. caQtDM libraries and include files will be expected and be located using the QE_CAQTDM environment variable (which will should point to the to point to the caQtDM_Project directory). Undefine environment variable QE_CAQTDM if you do not want caQtDM integration." )
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
    ../../framework/widgets/QEAbstractWidget \
    ../../framework/data \
    ../../framework/api \
    ../../framework/common

# Explicity add ../../framework/common, and hence QEFrameworkVersion.h, to the dependacy path
# so that changes to the version/release numbers force relavent recompilations.
#
DEPENDPATH += ../../framework/common

LIBS += -L$$_QE_EPICS_BASE/lib/$$(EPICS_HOST_ARCH) -lca -lCom

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

    # Note,the following checks for QWT are repeated in framework.pro - keep in sync
    # Check QWT is accessable. Check there is a chance QMAKEFEATURES includes a path to
    # the qwt features directory, or that QWT_INCLUDE_PATH is defined.
    # Note, qwt install may set up QMAKEFEATURES to point to the product features file, rather than
    # the directory. Not sure if this is wrong, but changing it to the directory works (C:\Qwt-6.0.1\features\qwt.prf  to  C:\Qwt-6.0.1\features)
    _QWT_INCLUDE_PATH = $$(QWT_INCLUDE_PATH)
    isEmpty( _QWT_INCLUDE_PATH ) {
        _QMAKEFEATURES = $$(QMAKEFEATURES)
        _QWT_FEATURE = $$find( _QMAKEFEATURES, [Q|q][W|w][T|t] )
        isEmpty( _QWT_FEATURE ) {
            error( "Qwt does not appear to be available. It is required when building QEGui with caQtDM integration. I've checked if 'qwt' is in QMAKEFEATURES or if QWT_INCLUDE_PATH is defined" )
        }
    }

    # The following QWT include path and library path are only required if
    # qwt was not installed fully, with qwt available as a Qt 'feature'.
    # When installed as a Qt 'feature' all that is needed is CONFIG += qwt (above)
    INCLUDEPATH += $$(QWT_INCLUDE_PATH)
    #win32:LIBS += -LC:/qwt-6.0.1/lib
    win32:LIBS += -LC:/qwt-6.1.0/lib
    #win32:LIBS += -LC:/qwt-6.1.1/lib

    # Depending on build, the qwt library below may need to be -lqwt or -lqwt6
    # The 'scope' labels Debug and Release need to have first letter capitalised for it to work in win32.
    win32 {
        Debug {
            message( "Using qwtd (not qwt) for this debug build" )
            LIBS += -lqwtd
        }
        Release {
            message( "Using qwt (not qwtd) for this release build" )
            LIBS += -lqwt
        }
    }

    unix {
        _QWT_ROOT = $$(QWT_ROOT)
        isEmpty( _QWT_ROOT ) {
            message( "QWT_ROOT is not defined, so using default location of QWT library" )
            LIBS += -lqwt
        } else {
            message( "Using QWT_ROOT environment variable to locate QWT library" )
            LIBS += -L$$_QWT_ROOT/lib -lqwt
        }
    }

    # Add caQtDM source locations
    INCLUDEPATH += $(QE_CAQTDM)/caQtDM_Lib/src \
                   $(QE_CAQTDM)/caQtDM_QtControls/src \
                   $(QWT_INCLUDE_PATH)

    # Include QtPrintSupport for Qt version 5
    equals( QT_MAJOR_VERSION, 5 ) {
       INCLUDEPATH += $$(QTINC)/QtPrintSupport
    }

    # Reference caQtDM library. Look in installed location if supplied, otherwise, caQtDM project area.
    _QE_CAQTDM_LIB = $$(QE_CAQTDM_LIB)
    isEmpty( _QE_CAQTDM_LIB ) {
        message( "QE_CAQTDM_LIB is not defined so looking for caQtDM library in $QE_CAQTDM/caQtDM_Lib" )
        LIBS += -L$(QE_CAQTDM)/caQtDM_Lib -lcaQtDM_Lib
    } else {
        LIBS += -L$(QE_CAQTDM_LIB) -lcaQtDM_Lib
    }

}
#===========================================================



# end
