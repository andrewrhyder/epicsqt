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
#include <QSubstitutedLabelPluginManager.h>
#include <QCaLineEditPluginManager.h>
#include <QCaPushButtonPluginManager.h>
#include <QCaRadioButtonPluginManager.h>
#include <QCaShapePluginManager.h>
#include <QCaSliderPluginManager.h>
#include <QCaSpinBoxPluginManager.h>
#include <QCaComboBoxPluginManager.h>
#include <LinkPluginManager.h>
#include <QCaPlotPluginManager.h>
#include <QCaPeriodicPluginManager.h>
#include <QEAnalogProgressBarManager.h>
#include <QCaAnalogProgressBarPluginManager.h>
#include <QAnalogProgressBarPluginManager.h>
#include <QEBitStatusManager.h>
#include <QCaBitStatusPluginManager.h>
#include <QBitStatusPluginManager.h>
#include <QEImageManager.h>
#include <QEPvPropertiesManager.h>
#include <QCaPvPropertiesPluginManager.h>
#include <QEStripChartManager.h>
#include <QCaStripChartPluginManager.h>
#include <QEConfiguredLayoutManager.h>
#include <QEFileBrowserManager.h>
#include <QERecipeManager.h>
#include <QELoginManager.h>
#include <QELogManager.h>
#include <QELauncherManager.h>
#include <QEFormManager.h>

//======================================================
// Deprecated widgets
#include <ASguiFormPluginManager.h>
#include <QCaLabelPluginManager.h>
//======================================================


#include <QtCore/qplugin.h>

#include <QCaDesignerPlugin.h>

/*!
    Add all plugins that will be provided in QT Designer.
*/
QCaWidgets::QCaWidgets(QObject *parent) : QObject(parent) {
    widgets.append(new QEAnalogProgressBarManager(this));
    widgets.append(new QEBitStatusManager(this));
    widgets.append(new QEConfiguredLayoutManager(this));
    widgets.append(new QEFileBrowserManager(this));
    widgets.append(new QELabelManager(this));
    widgets.append(new QELauncherManager(this));
    widgets.append(new QELoginManager(this));
    widgets.append(new QELogManager(this));
    widgets.append(new QEPvPropertiesManager(this));
    widgets.append(new QERecipeManager(this));
    widgets.append(new QEStripChartManager(this));


    widgets.append(new QSubstitutedLabelPluginManager(this));
    widgets.append(new QCaLineEditPluginManager(this));
    widgets.append(new QCaPushButtonPluginManager(this));
    widgets.append(new QCaRadioButtonPluginManager(this));
    widgets.append(new QCaShapePluginManager(this));
    widgets.append(new QCaSliderPluginManager(this));
    widgets.append(new QCaSpinBoxPluginManager(this));
    widgets.append(new QCaComboBoxPluginManager(this));
    widgets.append(new QEFormManager(this));
    widgets.append(new LinkPluginManager(this));
    widgets.append(new QCaPlotPluginManager(this));
    widgets.append(new QCaPeriodicPluginManager(this));
    widgets.append(new QEImageManager(this));

    // Non-EPICS aware framework widgets
    widgets.append(new QAnalogProgressBarPluginManager(this));
    widgets.append(new QBitStatusPluginManager(this));

//======================================================
// Deprecated widgets
    widgets.append(new QCaAnalogProgressBarPluginManager(this));
    widgets.append(new QCaBitStatusPluginManager(this));
    widgets.append(new QCaLabelPluginManager(this));
    widgets.append(new QCaPvPropertiesPluginManager(this));
    widgets.append(new QCaStripChartPluginManager(this));
    widgets.append(new ASguiFormPluginManager(this));
//======================================================

}
/*!
    Multiplugin interface.
*/
QList<QDesignerCustomWidgetInterface*> QCaWidgets::customWidgets() const {
    return widgets;
}

//! Publish plugins through to QT designer.
Q_EXPORT_PLUGIN2(QCaWidgetsPlugin, QCaWidgets)

