# This file is part of the EPICS QT Framework, initially developed at the Australian Synchrotron.
# The EPICS QT Framework is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# The EPICS QT Framework is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# You should have received a copy of the GNU General Public License
# along with the EPICS QT Framework.  If not, see <http://www.gnu.org/licenses/>.
# Copyright (c) 2009, 2010
# Author:
# Anthony Owen
# Contact details:
# anthony.owen@gmail.com


#===========================================================
# To alanyse code performance using the GNU gprof profiling tool:
# - Include the following two lines
# - Clean all
# - Run qmake
# - Rebuild all
# - Run the program
# - analyse the results with the command: gprof <your-program-name>
#QMAKE_CXXFLAGS_DEBUG += -pg
#QMAKE_LFLAGS_DEBUG += -pg

# To look at optimised performance (with or without the profileing above, include the following:
#QMAKE_CXXFLAGS_DEBUG += -O2

#===========================================================
# Include the following gdbmacros line for debugging only
#SOURCES += <YOUR-QTSDK-DIRECTORY>/share/qtcreator/gdbmacros/gdbmacros.cpp

#===========================================================
# Since Qt 4.7.4, enable-auto-import is required to avoid a crash on windows when the qwt dll is loaded
# Depending on compiler the '-W1' may or may not be required. If an error relating to nable-auto-import occurs (missing leading -e) then use -W1 option
#win32:QMAKE_LFLAGS += -enable-auto-import
win32QMAKE_LFLAGS += -Wl,-enable-auto-import

#===========================================================
# Project configuration

# Qt 4 configuration
equals( QT_MAJOR_VERSION, 4 ) {
    CONFIG += plugin debug_and_release uitools designer qwt
    QT += core gui xml network
}

# Qt 5 configuration
equals( QT_MAJOR_VERSION, 5 ) {
    CONFIG += plugin debug_and_release qwt
    QT += core gui xml network uitools designer
}

TEMPLATE = lib

DEFINES += QEPLUGIN_LIBRARY
DESTDIR = designer
TARGET = QEPlugin

DEFINES += QWT_DLL=TRUE

#===========================================================
# Include MPEG streaming into QEImage widget
# If mpeg streaming is required, define environment variable QE_FFMPEG (on windows, this must point to the FFMPEG directory)

_QE_FFMPEG = $$(QE_FFMPEG)
isEmpty( _QE_FFMPEG ) {
    warning( "MPEG viewer will NOT be included in QEImage. If you want MPEG support install FFmpeg and define environment variable QE_FFMPEG. This can be defined as anything on linux, but must point to the FFmpeg directory on windows" )
} else {
    warning( "MPEG viewer will be included in QEImage. FFmpeg libraries will be expected. On windows the environment variable QE_FFMPEG must point to the FFmpeg directory. Remove environment variable QE_FFMPEG if you don't want this" )
    DEFINES += QE_USE_MPEG
}

#===========================================================
# Project files
#
# Widget and other sub projects
# Included .pri (project include) files for each widget
#
include (adaptation_parameters/adaptation_parameters.pri)
include (common/common.pri)
include (api/api.pri)
include (data/data.pri)
include (widgets/widgets.pri)
include (archive/archive.pri)
include (widgets/QEAnalogIndicator/QEAnalogIndicator.pri)
include (widgets/QEAnalogProgressBar/QEAnalogProgressBar.pri)
include (widgets/QEBitStatus/QEBitStatus.pri)
include (widgets/QEButton/QEButton.pri)
include (widgets/QEComboBox/QEComboBox.pri)
include (widgets/QEConfiguredLayout/QEConfiguredLayout.pri)
include (widgets/QEFileBrowser/QEFileBrowser.pri)
include (widgets/QEFileImage/QEFileImage.pri)
include (widgets/QEForm/QEForm.pri)
include (widgets/QEFormGrid/QEFormGrid.pri)
include (widgets/QEFrame/QEFrame.pri)
include (widgets/QEGeneralEdit/QEGeneralEdit.pri)
include (widgets/QEGroupBox/QEGroupBox.pri)
include (widgets/QEHistogram/QEHistogram.pri)
include (widgets/QEImage/QEImage.pri)
include (widgets/QELabel/QELabel.pri)
include (widgets/QELineEdit/QELineEdit.pri)
include (widgets/QELink/QELink.pri)
include (widgets/QELog/QELog.pri)
include (widgets/QELogin/QELogin.pri)
include (widgets/QENumericEdit/QENumericEdit.pri)
include (widgets/QEPeriodic/QEPeriodic.pri)
include (widgets/QEPlot/QEPlot.pri)
include (widgets/QEPlotter/QEPlotter.pri)
include (widgets/QEPvLoadSave/QEPvLoadSave.pri)
include (widgets/QEPvProperties/QEPvProperties.pri)
include (widgets/QERadioGroup/QERadioGroup.pri)
include (widgets/QERecipe/QERecipe.pri)
include (widgets/QEResizeableFrame/QEResizeableFrame.pri)
include (widgets/QEScratchPad/QEScratchPad.pri)
include (widgets/QEScript/QEScript.pri)
include (widgets/QEShape/QEShape.pri)
include (widgets/QESimpleShape/QESimpleShape.pri)
include (widgets/QESlider/QESlider.pri)
include (widgets/QESpinBox/QESpinBox.pri)
include (widgets/QEStripChart/QEStripChart.pri)
include (widgets/QESubstitutedLabel/QESubstitutedLabel.pri)

OTHER_FILES += analogindicator.json

#===========================================================
# Extra targets
#
# Install header files into the include/ directory
#
# Possible enhancements:
# a/ be more selective - create an INSTALL_HEADERS variable.
# b/ be smarter re only installing modified headers.
#
install_headers.target   = install_headers
install_headers.commands = $(MKDIR) include && \
                           rm -f include/*  &&  \
                           $(INSTALL_FILE) $$HEADERS  include/

QMAKE_EXTRA_TARGETS += install_headers


#===========================================================
# EPICS
#
# Check EPICS appears to be present
_EPICS_BASE = $$(EPICS_BASE)
isEmpty( _EPICS_BASE ) {
    error( "EPICS_BASE must be defined. Ensure EPICS is installed and EPICS_BASE is set up" )
}
_EPICS_HOST_ARCH = $$(EPICS_HOST_ARCH)
isEmpty( _EPICS_HOST_ARCH ) {
    error( "EPICS_HOST_ARCH must be defined. Ensure EPICS is installed and EPICS_HOST_ARCH is set up" )
}

# Set up EPICS
unix:INCLUDEPATH += $$(EPICS_BASE)/include/os/Linux
win32:INCLUDEPATH += $$(EPICS_BASE)/include/os/WIN32
INCLUDEPATH += $$(EPICS_BASE)/include

LIBS += -L$$(EPICS_BASE)/lib/$$(EPICS_HOST_ARCH) \
    -lca \
    -lCom

#===========================================================
# QWT
#
# Check QWT is accessable. Check there is a chance QMAKEFEATURES includes a path to
# the qwt features directory, or that QWT_INCLUDE_PATH is defined.
# Note, qwt install may set up QMAKEFEATURES to point to the product features file, rather than
# the directory. Not sure if this is wrong, but changing it to the directory works (C:\Qwt-6.0.1\features\qwt.prf  to  C:\Qwt-6.0.1\features)
_QWT_INCLUDE_PATH = $$(QWT_INCLUDE_PATH)
isEmpty( _QWT_INCLUDE_PATH ) {
    _QMAKEFEATURES = $$(QMAKEFEATURES)
    _QWT_FEATURE = $$find( _QMAKEFEATURES, [Q|q][W|w][T|t] )
    isEmpty( _QWT_FEATURE ) {
        error( "Qwt does not appear to be available. I've checked if 'qwt' is in QMAKEFEATURES or if QWT_INCLUDE_PATH is defined" )
    }
}

# The following QWT include path and library path are only required if
# qwt was not installed fully, with qwt available as a Qt 'feature'.
# When installed as a Qt 'feature' all that is needed is CONFIG += qwt (above)
INCLUDEPATH += $$(QWT_INCLUDE_PATH)
#win32:LIBS += -LC:/qwt-6.0.1/lib
win32:LIBS += -LC:/qwt-6.1.0/lib

# Depending on build, the qwt library below may need to be -lqwt or -lqwt6
win32 {
    debug {
#        warning( "debug build. using lib qwtd (not qwt) ============================" )
        LIBS += -lqwtd
    }
    release {
#        warning( "release build. using lib qwt (not qwtd) ============================" )
        LIBS += -lqwt
    }
}

unix {
    LIBS += -lqwt
}

# ffmpeg stuff
isEmpty( _QE_FFMPEG ) {
} else {
    unix:INCLUDEPATH += /usr/local/include
    unix:LIBS += -L/usr/local/lib/

    win32:INCLUDEPATH += $$_QE_FFMPEG/include
    win32:LIBS += -L$$_QE_FFMPEG/lib

    LIBS += -lavutil -lavcodec -lavformat
    DEFINES += __STDC_CONSTANT_MACROS
}

#
# end
