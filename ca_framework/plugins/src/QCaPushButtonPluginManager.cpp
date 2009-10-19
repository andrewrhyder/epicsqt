/* $File: //ASP/Dev/SBS/4_Controls/4_8_GUI_Frameworks/4_8_2_Qt/sw/ca_framework/plugins/src/QCaPushButtonPluginManager.cpp $
 * $Revision: #1 $
 * $DateTime: 2009/07/14 15:59:56 $
 * Last checked in by: $Author: rhydera $
 */

/*! 
  \class QCaPushButtonPluginManager
  \version $Revision: #1 $
  \date $DateTime: 2009/07/14 15:59:56 $
  \author andrew.rhyder
  \brief CA Push Button Widget Plugin Manager for designer.
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

#include <QCaPushButtonPluginManager.h>
#include <QCaPushButtonPlugin.h>
#include <QtPlugin>

/*!
    ???
*/
QCaPushButtonPluginManager::QCaPushButtonPluginManager( QObject *parent ) : QObject( parent ) {
    initialized = false;
}

/*!
    ???
*/
void QCaPushButtonPluginManager::initialize( QDesignerFormEditorInterface * ) {
    if( initialized ) {
        return;
    }
    initialized = true;
}

/*!
    ???
*/
bool QCaPushButtonPluginManager::isInitialized() const {
    return initialized;
}

/*!
    Widget factory. Creates a QCaLabel widget.
*/
QWidget *QCaPushButtonPluginManager::createWidget ( QWidget *parent ) {
    return new QCaPushButtonPlugin(parent);
}

/*!
    Name for widget. Used by Qt Designer in widget list.
*/
QString QCaPushButtonPluginManager::name() const {
    return "QCaPushButtonPlugin";
}

/*!
    Name of group Qt Designer will add widget to.
*/
QString QCaPushButtonPluginManager::group() const {
    return "EPICS Widgets";
}

/*!
    Icon for widget. Used by Qt Designer in widget list.
*/
QIcon QCaPushButtonPluginManager::icon() const {
    return QIcon(":/icons/QCaPushButton.png");
}

/*!
    Tool tip for widget. Used by Qt Designer in widget list.
*/
QString QCaPushButtonPluginManager::toolTip() const {
    return "EPICS Push Button";
}

/*!
    ???
*/
QString QCaPushButtonPluginManager::whatsThis() const {
    return "EPICS Push Button";
}

/*!
    ???
*/
bool QCaPushButtonPluginManager::isContainer() const {
    return false;
}

/*!
    ???
*/
/*QString QCaPushButtonPluginManager::domXml() const {
    return "<widget class=\"QCaPushButton\" name=\"qCaPushButton\">\n"
           " <property name=\"geometry\">\n"
           "  <rect>\n"
           "   <x>0</x>\n"
           "   <y>0</y>\n"
           "   <width>100</width>\n"
           "   <height>100</height>\n"
           "  </rect>\n"
           " </property>\n"
           " <property name=\"toolTip\" >\n"
           "  <string></string>\n"
           " </property>\n"
           " <property name=\"whatsThis\" >\n"
           "  <string> "
           ".</string>\n"
           " </property>\n"
           "</widget>\n";
}*/

/*!
    ???
*/
QString QCaPushButtonPluginManager::includeFile() const {
    return "QCaPushButtonPlugin.h";
}
