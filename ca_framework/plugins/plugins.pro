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
QT += core gui multimedia xml network
TEMPLATE = lib
CONFIG += plugin \
    uitools \
    designer \
    debug_and_release
DEFINES += QCAPLUGIN_LIBRARY
TARGET = QCaPlugin
OTHER_FILES += src/QCaSpinBox.png \
    src/QCaSlider.png \
    src/QCaShape.png \
    src/QCaPushButton.png \
    src/QCaRadioButton.png \
    src/QCaPlot.png \
    src/QCaPvProperties.png \
    src/QCaStripChart.png \
    src/QCaLineEdit.png \
    src/QELabel.png \
    src/QSubstitutedLabel.png \
    src/QCaComboBox.png \
    src/Link.png \
    src/ASguiForm.png \
    src/QCaPeriodic.png \
    src/QAnalogProgressBar.png \
    src/QCaAnalogProgressBar.png \
    src/QBitStatus.png \
    src/QCaBitStatus.png \
    src/QCaImage.png \
    src/QCaLogin.png \
    src/record_field_list.txt \
    src/QCaLog.png \
    src/QCaMotor.png \
    src/strip_chart_play.png \
    src/strip_chart_pause.png \
    src/strip_chart_page_forward.png \
    src/strip_chart_page_backward.png

HEADERS += \
    include/QCaPluginLibrary_global.h \
    include/QCaSpinBoxPluginManager.h \
    include/QCaSpinBoxPlugin.h \
    include/QCaComboBoxPluginManager.h \
    include/QCaComboBoxPlugin.h \
    include/QCaSliderPluginManager.h \
    include/QCaSliderPlugin.h \
    include/QCaShapePluginManager.h \
    include/QCaShapePlugin.h \
    include/QCaPushButtonPluginManager.h \
    include/QCaPushButtonPlugin.h \
    include/QCaRadioButtonPluginManager.h \
    include/QCaRadioButtonPlugin.h \
    include/QCaLineEditPluginManager.h \
    include/QCaLineEditPlugin.h \
    include/QCaDesignerPlugin.h \
    include/ContainerProfile.h \
    include/ASguiFormPluginManager.h \
    include/ASguiFormPlugin.h \
    include/QSubstitutedLabelPluginManager.h \
    include/QSubstitutedLabelPlugin.h \
    include/LinkPluginManager.h \
    include/LinkPlugin.h \
    include/QCaPlotPluginManager.h \
    include/QCaPlotPlugin.h \
    include/QCaPeriodicPlugin.h \
    include/QCaPeriodicPluginManager.h \
    include/QCaPeriodicPluginTaskMenu.h \
    include/PeriodicSetupDialog.h \
    include/PeriodicElementSetupForm.h \
    include/QAnalogProgressBarPluginManager.h \
    include/QCaAnalogProgressBarPluginManager.h \
    include/QCaPvPropertiesPluginManager.h \
    include/QCaStripChartPluginManager.h \
    include/QBitStatusPluginManager.h \
    include/QCaBitStatusPluginManager.h \
    include/QCaImagePluginManager.h \
    include/QCaImagePlugin.h \
    include/QCaLoginPluginManager.h \
    include/QCaLogPluginManager.h \
    include/QCaMotorPluginManager.h \
    ../api/include/Generic.h \
    ../api/include/CaRecord.h \
    ../api/include/CaObject.h \
    ../api/include/CaConnection.h \
    ../api/include/CaObjectPrivate.h \
    ../archive/include/QCaArchiveInterface.h \
    ../archive/include/QCaArchiveManager.h \
    ../archive/include/maiaFault.h \
    ../archive/include/maiaObject.h \
    ../archive/include/maiaXmlRpcClient.h \
    ../data/include/VariableNameManager.h \
    ../data/include/UserMessage.h \
    ../data/include/QCaDataPoint.h \
    ../data/include/QCaWidget.h \
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
    ../widgets/include/managePixmaps.h \
    ../widgets/include/QCaDragDrop.h \
    ../widgets/include/styleManager.h \
    ../widgets/include/QCaStringFormattingMethods.h \
    ../widgets/include/QCaSpinBox.h \
    ../widgets/include/QCaComboBox.h \
    ../widgets/include/QCaSlider.h \
    ../widgets/include/QCaShape.h \
    ../widgets/include/QCaPushButton.h \
    ../widgets/include/QCaPvProperties.h \
    ../widgets/include/QCaStripChart.h \
    ../widgets/include/QCaStripChartItem.h \
    ../widgets/include/QCaStripChartTimeDialog.h \
    ../widgets/include/QCaStripChartItemDialog.h \
    ../widgets/include/QCaRadioButton.h \
    ../widgets/include/QCaGenericButton.h \
    ../widgets/include/QCaLineEdit.h \
    ../widgets/QELabel/QELabel.h \
    ../widgets/QELabel/QELabelManager.h \
    ../widgets/include/QSubstitutedLabel.h \
    ../widgets/include/Link.h \
    ../widgets/include/ASguiForm.h \
    ../widgets/include/QCaPlot.h \
    ../widgets/include/QCaToolTip.h \
    ../widgets/include/QCaPeriodic.h \
    ../widgets/include/PeriodicDialog.h \
    ../widgets/include/QAnalogProgressBar.h \
    ../widgets/include/QCaAnalogProgressBar.h \
    ../widgets/include/QBitStatus.h \
    ../widgets/include/QCaBitStatus.h \
    ../widgets/include/QCaImage.h \
    ../widgets/include/videowidget.h \
    ../widgets/include/profilePlot.h \
    ../widgets/include/imageMarkup.h \
    ../widgets/include/QCaLogin.h \
    ../widgets/include/QCaLog.h \
    ../widgets/include/QCaMotor.h

#====================================
# common properties
HEADERS += \
    ../widgets/include/singleVariableProperties.inc> \
    ../widgets/include/standardProperties.inc> \
    ../widgets/include/stringProperties.inc>

#====================================
# deprecated
HEADERS += \
    include/QCaLabelPluginManager.h \
    include/QCaLabelPlugin.h

#====================================

SOURCES += src/QCaSpinBoxPluginManager.cpp \
    src/QCaSpinBoxPlugin.cpp \
    src/QCaComboBoxPluginManager.cpp \
    src/QCaComboBoxPlugin.cpp \
    src/QCaSliderPluginManager.cpp \
    src/QCaSliderPlugin.cpp \
    src/QCaShapePluginManager.cpp \
    src/QCaShapePlugin.cpp \
    src/QCaPushButtonPluginManager.cpp \
    src/QCaPushButtonPlugin.cpp \
    src/QCaRadioButtonPluginManager.cpp \
    src/QCaRadioButtonPlugin.cpp \
    src/QCaLineEditPluginManager.cpp \
    src/QCaLineEditPlugin.cpp \
    src/QSubstitutedLabelPluginManager.cpp \
    src/QSubstitutedLabelPlugin.cpp \
    src/LinkPluginManager.cpp \
    src/LinkPlugin.cpp \
    src/QCaDesignerPlugin.cpp \
    src/ContainerProfile.cpp \
    src/ASguiFormPluginManager.cpp \
    src/ASguiFormPlugin.cpp \
    src/QCaPlotPlugin.cpp \
    src/QCaPlotPluginManager.cpp \
    src/QCaPeriodicPlugin.cpp \
    src/QCaPeriodicPluginManager.cpp \
    src/QCaPeriodicPluginTaskMenuExtension.cpp \
    src/PeriodicSetupDialog.cpp \
    src/PeriodicElementSetupForm.cpp \
    src/QAnalogProgressBarPluginManager.cpp \
    src/QCaAnalogProgressBarPluginManager.cpp \
    src/QBitStatusPluginManager.cpp \
    src/QCaBitStatusPluginManager.cpp \
    src/QCaImagePluginManager.cpp \
    src/QCaImagePlugin.cpp \
    src/QCaPvPropertiesPluginManager.cpp \
    src/QCaStripChartPluginManager.cpp \
    src/QCaLoginPluginManager.cpp \
    src/QCaLogPluginManager.cpp \
    src/QCaMotorPluginManager.cpp \
    ../api/src/Generic.cpp \
    ../api/src/CaRecord.cpp \
    ../api/src/CaObject.cpp \
    ../api/src/CaConnection.cpp \
    ../archive/src/QCaArchiveInterface.cpp \
    ../archive/src/QCaArchiveManager.cpp \
    ../archive/src/maiaFault.cpp \
    ../archive/src/maiaObject.cpp \
    ../archive/src/maiaXmlRpcClient.cpp \
    ../data/src/VariableNameManager.cpp \
    ../data/src/UserMessage.cpp \
    ../data/src/QCaWidget.cpp \
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
    ../widgets/src/managePixmaps.cpp \
    ../widgets/src/QCaDragDrop.cpp \
    ../widgets/src/styleManager.cpp \
    ../widgets/src/QCaStringFormattingMethods.cpp \
    ../widgets/src/QCaSpinBox.cpp \
    ../widgets/src/QCaComboBox.cpp \
    ../widgets/src/QCaSlider.cpp \
    ../widgets/src/QCaShape.cpp \
    ../widgets/src/QCaPushButton.cpp \
    ../widgets/src/QCaPvProperties.cpp \
    ../widgets/src/QCaStripChart.cpp \
    ../widgets/src/QCaStripChartItem.cpp \
    ../widgets/src/QCaStripChartTimeDialog.cpp \
    ../widgets/src/QCaStripChartItemDialog.cpp \
    ../widgets/src/QCaRadioButton.cpp \
    ../widgets/src/QCaGenericButton.cpp \
    ../widgets/src/QCaLineEdit.cpp \
    ../widgets/QELabel/QELabel.cpp \
    ../widgets/QELabel/QELabelManager.cpp \
    ../widgets/src/QSubstitutedLabel.cpp \
    ../widgets/src/Link.cpp \
    ../widgets/src/QCaPlot.cpp \
    ../widgets/src/ASguiForm.cpp \
    ../widgets/src/QCaToolTip.cpp \
    ../widgets/src/QCaPeriodic.cpp \
    ../widgets/src/PeriodicDialog.cpp \
    ../widgets/src/QAnalogProgressBar.cpp \
    ../widgets/src/QCaAnalogProgressBar.cpp \
    ../widgets/src/QBitStatus.cpp \
    ../widgets/src/QCaBitStatus.cpp \
    ../widgets/src/QCaImage.cpp \
    ../widgets/src/videowidget.cpp \
    ../widgets/src/profilePlot.cpp \
    ../widgets/src/imageMarkup.cpp \
    ../widgets/src/QCaLogin.cpp \
    ../widgets/src/QCaLog.cpp \
    ../widgets/src/QCaMotor.cpp

#====================================
# deprecated
SOURCES += \
    src/QCaLabelPluginManager.cpp
#====================================



# Include the following gdbmacros line for debugging only
#SOURCES += $$(QCAQTDIR)/share/qtcreator/gdbmacros/gdbmacros.cpp
RESOURCES += src/QCaResources.qrc
INCLUDEPATH += $$(QCAFRAMEWORK)/plugins/include \
    $$(QCAFRAMEWORK)/api/include \
    $$(QCAFRAMEWORK)/archive/include \
    $$(QCAFRAMEWORK)/data/include \
    $$(QCAFRAMEWORK)/widgets/include \
    $$(QCAFRAMEWORK)/widgets/QELabel \
    /usr/include/qwt \
    $$(EPICS_BASE)/include
unix:INCLUDEPATH += $$(EPICS_BASE)/include/os/Linux
win32:INCLUDEPATH += $$(EPICS_BASE)/include/os/WIN32
INCLUDEPATH += $$(EPICS_BASE)/include
# Depending on build, the qwt library below may need to be -lqwt or -lqwt6
LIBS += -L$$(EPICS_BASE)/lib/$$(EPICS_HOST_ARCH) \
    -lca \
    -lCom \
#    -L$$(QCAFRAMEWORK)/qwt/lib \
    -lqwt

FORMS += \
    ../widgets/src/PeriodicDialog.ui \
    ../widgets/src/QCaStripChartTimeDialog.ui \
    ../widgets/src/QCaStripChartItemDialog.ui \
    src/PeriodicSetupDialog.ui \
    src/PeriodicElementSetupForm.ui

