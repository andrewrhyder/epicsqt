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


# To alanyse code performance using the GNU gprof profiling tool:
# - Include the following two lines
# - Clean all
# - Run qmake
# - Rebuild all
# - Run the program
# - analyse the results with the command: gprof <your-program-name>
#QMAKE_CXXFLAGS_DEBUG += -pg
#QMAKE_LFLAGS_DEBUG += -pg

QT += core gui xml network
TEMPLATE = lib
CONFIG += plugin \
    uitools \
    designer \
    debug_and_release
DEFINES += QCAPLUGIN_LIBRARY
TARGET = QCaPlugin
OTHER_FILES += \
    src/record_field_list.txt \
    src/cameraROI.png \
    src/cameraROIreset.png \
    src/flipRotate.png \
    src/pause.png \
    src/play.png \
    src/save.png \
    src/select.png \
    src/zoom.png


HEADERS += \
    ../api/include/Generic.h \
    ../api/include/CaRecord.h \
    ../api/include/CaRef.h \
    ../api/include/CaObject.h \
    ../api/include/CaConnection.h \
    ../api/include/CaObjectPrivate.h \
    ../data/include/QCaDataPoint.h \
    ../data/include/QCaVariableNamePropertyManager.h \
    ../data/include/QCaStringFormatting.h \
    ../data/include/QCaString.h \
    ../data/include/QCaStateMachine.h \
    ../data/include/QCaObject.h \
    ../data/include/QCaIntegerFormatting.h \
    ../data/include/QCaInteger.h \
    ../data/include/QCaFloatingFormatting.h \
    ../data/include/QCaFloating.h \
    ../data/include/QCaByteArray.h \
    ../data/include/QCaEventUpdate.h \
    ../data/include/QCaEventFilter.h \
    ../data/include/QCaDateTime.h \
    ../data/include/QCaAlarmInfo.h \
    ../data/include/QCaConnectionInfo.h \
    ../data/include/qepicspv.h \
    ../widgets/include/VariableNameManager.h \
    ../widgets/include/UserMessage.h \
    ../widgets/include/contextMenu.h \
    ../widgets/include/QCaWidget.h \
    ../widgets/include/managePixmaps.h \
    ../widgets/include/QCaDragDrop.h \
    ../widgets/include/styleManager.h \
    ../widgets/include/standardProperties.h \
    ../widgets/include/QCaStringFormattingMethods.h \
    ../widgets/include/QCaToolTip.h \
    ../widgets/include/QCaDesignerPlugin.h \
    ../widgets/include/ContainerProfile.h \
    ../widgets/include/QCaPluginLibrary_global.h \


SOURCES += \
    ../api/src/Generic.cpp \
    ../api/src/CaRecord.cpp \
    ../api/src/CaObject.cpp \
    ../api/src/CaConnection.cpp \
    ../data/src/QCaVariableNamePropertyManager.cpp \
    ../data/src/QCaStringFormatting.cpp \
    ../data/src/QCaString.cpp \
    ../data/src/QCaStateMachine.cpp \
    ../data/src/QCaObject.cpp \
    ../data/src/QCaIntegerFormatting.cpp \
    ../data/src/QCaInteger.cpp \
    ../data/src/QCaFloatingFormatting.cpp \
    ../data/src/QCaFloating.cpp \
    ../data/src/QCaByteArray.cpp \
    ../data/src/QCaEventUpdate.cpp \
    ../data/src/QCaEventFilter.cpp \
    ../data/src/QCaDateTime.cpp \
    ../data/src/QCaAlarmInfo.cpp \
    ../data/src/QCaConnectionInfo.cpp \
    ../data/src/qepicspv.cpp \
    ../widgets/src/VariableNameManager.cpp \
    ../widgets/src/UserMessage.cpp \
    ../widgets/src/contextMenu.cpp \
    ../widgets/src/QCaWidget.cpp \
    ../widgets/src/managePixmaps.cpp \
    ../widgets/src/QCaDragDrop.cpp \
    ../widgets/src/styleManager.cpp \
    ../widgets/src/standardProperties.cpp \
    ../widgets/src/QCaStringFormattingMethods.cpp \
    ../widgets/src/QCaToolTip.cpp \
    ../widgets/src/QCaDesignerPlugin.cpp \
    ../widgets/src/ContainerProfile.cpp \


# Include the following gdbmacros line for debugging only
#SOURCES += $$(QCAQTDIR)/share/qtcreator/gdbmacros/gdbmacros.cpp

_EPICS_BASE = $$(EPICS_BASE)
isEmpty( _EPICS_BASE ) {
    error( "EPICS_BASE must be defined. Ensure EPICS is installed and EPICS_BASE is set up" )
}

_EPICS_HOST_ARCH = $$(EPICS_HOST_ARCH)
isEmpty( _EPICS_HOST_ARCH ) {
    error( "EPICS_HOST_ARCH must be defined. Ensure EPICS is installed and EPICS_HOST_ARCH is set up" )
}

INCLUDEPATH += \
    ../api/include \
    ../data/include \
    ../widgets/include \
    $$(EPICS_BASE)/include

unix:INCLUDEPATH += $$(EPICS_BASE)/include/os/Linux
win32:INCLUDEPATH += $$(EPICS_BASE)/include/os/WIN32
INCLUDEPATH += $$(EPICS_BASE)/include
# Depending on build, the qwt library below may need to be -lqwt or -lqwt6
LIBS += -L$$(EPICS_BASE)/lib/$$(EPICS_HOST_ARCH) \
    -lca \
    -lCom

#===========================================================
# Included .pri (project include) files for each widget
#
include (../archive/QEArchive.pri)
include (../widgets/QEAnalogIndicator/QEAnalogIndicator.pri)
include (../widgets/QEAnalogProgressBar/QEAnalogProgressBar.pri)
include (../widgets/QEBitStatus/QEBitStatus.pri)
include (../widgets/QEConfiguredLayout/QEConfiguredLayout.pri)
include (../widgets/QEFrame/QEFrame.pri)
include (../widgets/QEGroupBox/QEGroupBox.pri)
include (../widgets/QEFileBrowser/QEFileBrowser.pri)
include (../widgets/QEImage/QEImage.pri)
include (../widgets/QELabel/QELabel.pri)
include (../widgets/QELineEdit/QELineEdit.pri)
include (../widgets/QELog/QELog.pri)
include (../widgets/QELogin/QELogin.pri)
include (../widgets/QEPvProperties/QEPvProperties.pri)
include (../widgets/QERecipe/QERecipe.pri)
include (../widgets/QEScript/QEScript.pri)
include (../widgets/QEStripChart/QEStripChart.pri)
include (../widgets/QEForm/QEForm.pri)
include (../widgets/QEPeriodic/QEPeriodic.pri)
include (../widgets/QEComboBox/QEComboBox.pri)
include (../widgets/QEButton/QEButton.pri)
include (../widgets/QEShape/QEShape.pri)
include (../widgets/QESlider/QESlider.pri)
include (../widgets/QESpinBox/QESpinBox.pri)
include (../widgets/QESubstitutedLabel/QESubstitutedLabel.pri)
include (../widgets/QELink/QELink.pri)
include (../widgets/QEPlot/QEPlot.pri)
include (../widgets/deprecated/deprecated.pri)

#===========================================================
#
_QWT_INCLUDE_PATH = $$(QWT_INCLUDE_PATH)
isEmpty( _QWT_INCLUDE_PATH ) {
    error( "QWT_INCLUDE_PATH must be defined. Ensure Qwt is installed for development and QWT_INCLUDE_PATH is set up" )
}

INCLUDEPATH += \
    $$(QWT_INCLUDE_PATH)

# Depending on build, the qwt library below may need to be -lqwt or -lqwt6
LIBS += -lqwt

#
# end
