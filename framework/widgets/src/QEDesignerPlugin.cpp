/*
 *  This file is part of the EPICS QT Framework, initially developed at the Australian Synchrotron.
 *
 *  The EPICS QT Framework is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  The EPICS QT Framework is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with the EPICS QT Framework.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Copyright (c) 2009, 2010
 *
 *  Author:
 *    Andrew Rhyder
 *  Contact details:
 *    andrew.rhyder@synchrotron.org.au
 */
 
#include <QELabelManager.h>
#include <QESubstitutedLabelManager.h>
#include <QELineEditManager.h>
#include <QEPushButtonManager.h>
#include <QERadioButtonManager.h>
#include <QEShapeManager.h>
#include <QESliderManager.h>
#include <QESpinBoxManager.h>
#include <QEComboBoxManager.h>
#include <QELinkManager.h>
#include <QEPlotManager.h>
#include <QEPeriodicManager.h>
#include <QEAnalogProgressBarManager.h>
#include <QEAnalogIndicatorManager.h>
#include <QEBitStatusManager.h>
#include <QBitStatusManager.h>
#include <QEImageManager.h>
#include <QEPvPropertiesManager.h>
#include <QEStripChartManager.h>
#include <QEConfiguredLayoutManager.h>
#include <QEFileBrowserManager.h>
#include <QERecipeManager.h>
#include <QELoginManager.h>
#include <QELogManager.h>
#include <QEScriptManager.h>
#include <QEFormManager.h>
#include <QEFrameManager.h>
#include <QEGroupBoxManager.h>
#include <QEFileImageManager.h>

//======================================================
// Deprecated widgets
#include <ASguiFormPluginManager.h>
#include <QCaLabelPluginManager.h>
#include <QCaAnalogProgressBarPluginManager.h>
#include <QCaBitStatusPluginManager.h>
#include <QCaLineEditPluginManager.h>
#include <QCaPvPropertiesPluginManager.h>
#include <QCaStripChartPluginManager.h>
#include <QCaPeriodicPluginManager.h>
#include <QCaSpinBoxPluginManager.h>
#include <LinkPluginManager.h>
#include <QCaPushButtonPluginManager.h>
#include <QCaRadioButtonPluginManager.h>
#include <QCaSliderPluginManager.h>
#include <QCaComboBoxPluginManager.h>
#include <QCaPlotPluginManager.h>
#include <QCaShapePluginManager.h>
#include <QSubstitutedLabelPluginManager.h>
//======================================================


#include <QtCore/qplugin.h>

#include <QEDesignerPlugin.h>

/*
    Add all plugins that will be provided in QT Designer.
*/
QEWidgets::QEWidgets(QObject *parent) : QObject(parent) {
    widgets.append(new QEAnalogProgressBarManager(this));
    widgets.append(new QEBitStatusManager(this));
    widgets.append(new QEConfiguredLayoutManager(this));
    widgets.append(new QEFileBrowserManager(this));
    widgets.append(new QELabelManager(this));
    widgets.append(new QELoginManager(this));
    widgets.append(new QELogManager(this));
    widgets.append(new QEPvPropertiesManager(this));
    widgets.append(new QERecipeManager(this));
    widgets.append(new QEScriptManager(this));
    widgets.append(new QEStripChartManager(this));
    widgets.append(new QEPeriodicManager(this));

    widgets.append(new QESubstitutedLabelManager(this));
    widgets.append(new QELineEditManager(this));
    widgets.append(new QEPushButtonManager(this));
    widgets.append(new QERadioButtonManager(this));
    widgets.append(new QEShapeManager(this));
    widgets.append(new QESliderManager(this));
    widgets.append(new QESpinBoxManager(this));
    widgets.append(new QEComboBoxManager(this));
    widgets.append(new QEFormManager(this));
    widgets.append(new QEPlotManager(this));
    widgets.append(new QEImageManager(this));
    widgets.append(new QEFileImageManager(this));

    // Non-EPICS aware framework widgets
    widgets.append(new QEAnalogIndicatorManager(this));
    widgets.append(new QBitStatusManager(this));
    widgets.append(new QEFrameManager(this));
    widgets.append(new QEGroupBoxManager(this));
    widgets.append(new QELinkManager(this));

//======================================================
// Deprecated widgets
    widgets.append(new QCaAnalogProgressBarPluginManager(this));
    widgets.append(new QCaBitStatusPluginManager(this));
    widgets.append(new QCaLabelPluginManager(this));
    widgets.append(new QCaLineEditPluginManager(this));
    widgets.append(new QCaPvPropertiesPluginManager(this));
    widgets.append(new QCaStripChartPluginManager(this));
    widgets.append(new ASguiFormPluginManager(this));
    widgets.append(new QCaPeriodicPluginManager(this));
    widgets.append(new QCaSpinBoxPluginManager(this));
    widgets.append(new LinkPluginManager(this));
    widgets.append(new QCaPushButtonPluginManager(this));
    widgets.append(new QCaRadioButtonPluginManager(this));
    widgets.append(new QCaSliderPluginManager(this));
    widgets.append(new QCaComboBoxPluginManager(this));
    widgets.append(new QCaPlotPluginManager(this));
    widgets.append(new QCaShapePluginManager(this));
    widgets.append(new QSubstitutedLabelPluginManager(this));
//======================================================

}
/*
    Multiplugin interface.
*/
QList<QDesignerCustomWidgetInterface*> QEWidgets::customWidgets() const {
    return widgets;
}

// Publish plugins through to QT designer.
Q_EXPORT_PLUGIN2(QEWidgetsPlugin, QEWidgets)

