/* $File: //ASP/Dev/SBS/4_Controls/4_8_GUI_Frameworks/4_8_2_Qt/sw/ca_framework/plugins/src/QCaDesignerPlugin.cpp $
 * $Revision: #2 $
 * $DateTime: 2009/07/22 14:48:46 $
 * Last checked in by: $Author: rhydera $
 */

/*! 
  \class QCaDesignerPlugin
  \version $Revision: #2 $
  \date $DateTime: 2009/07/22 14:48:46 $
  \author andrew.rhyder
  \brief Collection of CA plugins for QT Designer.
 */

/* Copyright (c) 2009 Australian Synchrotron
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * Licence as published by the Free Software Foundation; either
 * version 2.1 of the Licence, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public Licence for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * Licence along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Contact details:
 * andrew.rhyder@synchrotron.org.au
 * 800 Blackburn Road, Clayton, Victoria 3168, Australia.
 *
 */
 
#include <QCaLabelPluginManager.h>
#include <QCaLineEditPluginManager.h>
#include <QCaPushButtonPluginManager.h>
#include <GuiPushButtonPluginManager.h>
#include <CmdPushButtonPluginManager.h>
#include <QCaShapePluginManager.h>
#include <QCaSliderPluginManager.h>
#include <QCaSpinBoxPluginManager.h>
#include <QCaComboBoxPluginManager.h>
#include <ASguiFormPluginManager.h>
#include <QtCore/qplugin.h>

#include <QCaDesignerPlugin.h>

/*!
    Add all plugins that will be provided in QT Designer.
*/
QCaWidgets::QCaWidgets(QObject *parent) : QObject(parent) {
    widgets.append(new QCaLabelPluginManager(this));
    widgets.append(new QCaLineEditPluginManager(this));
    widgets.append(new QCaPushButtonPluginManager(this));
    widgets.append(new GuiPushButtonPluginManager(this));
    widgets.append(new CmdPushButtonPluginManager(this));
    widgets.append(new QCaShapePluginManager(this));
    widgets.append(new QCaSliderPluginManager(this));
    widgets.append(new QCaSpinBoxPluginManager(this));
    widgets.append(new QCaComboBoxPluginManager(this));
    widgets.append(new ASguiFormPluginManager(this));
}

/*!
    Multiplugin interface.
*/
QList<QDesignerCustomWidgetInterface*> QCaWidgets::customWidgets() const {
    return widgets;
}

//! Publish plugins through to QT designer.
Q_EXPORT_PLUGIN2(QCaWidgetsPlugin, QCaWidgets)
