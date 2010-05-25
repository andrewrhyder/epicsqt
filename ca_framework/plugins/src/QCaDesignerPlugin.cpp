/*! 
  \class QCaDesignerPlugin
  \version $Revision: #3 $
  \date $DateTime: 2010/01/25 16:09:07 $
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
#include <LinkPluginManager.h>
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
    widgets.append(new LinkPluginManager(this));
}

/*!
    Multiplugin interface.
*/
QList<QDesignerCustomWidgetInterface*> QCaWidgets::customWidgets() const {
    return widgets;
}

//! Publish plugins through to QT designer.
Q_EXPORT_PLUGIN2(QCaWidgetsPlugin, QCaWidgets)
