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
QT += core gui xml network
TEMPLATE = lib
CONFIG += plugin \
    uitools \
    designer \
    debug_and_release \
    qwt
DEFINES += QEPLUGIN_LIBRARY
DESTDIR = designer
TARGET = QEPlugin

#===========================================================
# Project files

INCLUDEPATH += \
    api/include \
    widgets/include

HEADERS += \
    api/include/Generic.h \
    api/include/CaRecord.h \
    api/include/CaRef.h \
    api/include/CaObject.h \
    api/include/CaConnection.h \
    api/include/CaObjectPrivate.h \
    widgets/include/VariableNameManager.h \
    widgets/include/UserMessage.h \
    widgets/include/contextMenu.h \
    widgets/include/QEWidget.h \
    widgets/include/managePixmaps.h \
    widgets/include/QEDragDrop.h \
    widgets/include/styleManager.h \
    widgets/include/standardProperties.h \
    widgets/include/QEStringFormattingMethods.h \
    widgets/include/QEToolTip.h \
    widgets/include/QEDesignerPlugin.h \
    widgets/include/ContainerProfile.h \
    widgets/include/QEPluginLibrary_global.h \
    widgets/include/persistanceManager.h


SOURCES += \
    api/src/Generic.cpp \
    api/src/CaRecord.cpp \
    api/src/CaObject.cpp \
    api/src/CaConnection.cpp \
    api/src/CaRef.cpp \
    widgets/src/VariableNameManager.cpp \
    widgets/src/UserMessage.cpp \
    widgets/src/contextMenu.cpp \
    widgets/src/QEWidget.cpp \
    widgets/src/managePixmaps.cpp \
    widgets/src/QEDragDrop.cpp \
    widgets/src/styleManager.cpp \
    widgets/src/standardProperties.cpp \
    widgets/src/QEStringFormattingMethods.cpp \
    widgets/src/QEToolTip.cpp \
    widgets/src/QEDesignerPlugin.cpp \
    widgets/src/ContainerProfile.cpp \
    widgets/src/persistanceManager.cpp


#===========================================================
# Widget and other sub projects
# Included .pri (project include) files for each widget
#
include (common/common.pri)
include (data/data.pri)
include (archive/QEArchive.pri)
include (widgets/QEAnalogIndicator/QEAnalogIndicator.pri)
include (widgets/QEAnalogProgressBar/QEAnalogProgressBar.pri)
include (widgets/QEBitStatus/QEBitStatus.pri)
include (widgets/QEButton/QEButton.pri)
include (widgets/QEComboBox/QEComboBox.pri)
include (widgets/QEConfiguredLayout/QEConfiguredLayout.pri)
include (widgets/QEFileBrowser/QEFileBrowser.pri)
include (widgets/QEFileImage/QEFileImage.pri)
include (widgets/QEForm/QEForm.pri)
include (widgets/QEFrame/QEFrame.pri)
include (widgets/QEGroupBox/QEGroupBox.pri)
include (widgets/QEImage/QEImage.pri)
include (widgets/QELabel/QELabel.pri)
include (widgets/QELineEdit/QELineEdit.pri)
include (widgets/QELink/QELink.pri)
include (widgets/QELog/QELog.pri)
include (widgets/QELogin/QELogin.pri)
include (widgets/QEPeriodic/QEPeriodic.pri)
include (widgets/QEPlot/QEPlot.pri)
include (widgets/QEPlotter/QEPlotter.pri)
include (widgets/QEPvProperties/QEPvProperties.pri)
include (widgets/QERadioGroup/QERadioGroup.pri)
include (widgets/QERecipe/QERecipe.pri)
include (widgets/QEResizeableFrame/QEResizeableFrame.pri)
include (widgets/QEScript/QEScript.pri)
include (widgets/QEShape/QEShape.pri)
include (widgets/QESimpleShape/QESimpleShape.pri)
include (widgets/QESlider/QESlider.pri)
include (widgets/QESpinBox/QESpinBox.pri)
include (widgets/QEStripChart/QEStripChart.pri)
include (widgets/QESubstitutedLabel/QESubstitutedLabel.pri)
include (widgets/deprecated/deprecated.pri)


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
#LIBS += -LC:\Qwt-6.0.1\lib

# Depending on build, the qwt library below may need to be -lqwt or -lqwt6
LIBS += -lqwt

#
# end
