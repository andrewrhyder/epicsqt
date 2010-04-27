/* $File: //ASP/Dev/SBS/4_Controls/4_8_GUI_Frameworks/4_8_2_Qt/sw/ca_framework/plugins/src/LinkPluginManager.cpp $
 * $Revision: #1 $
 * $DateTime: 2010/01/25 16:09:07 $
 * Last checked in by: $Author: rhydera $
 */

/*! 
  \class LinkPluginManager
  \version $Revision: #1 $
  \date $DateTime: 2010/01/25 16:09:07 $
  \author andrew.rhyder
  \brief CA Label Widget Plugin Manager for designer.
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

#include <LinkPluginManager.h>
#include <LinkPlugin.h>
#include <QtPlugin>

/*!
    ???
*/
LinkPluginManager::LinkPluginManager( QObject *parent ) : QObject( parent ) {
    initialized = false;
}

/*!
    ???
*/
void LinkPluginManager::initialize( QDesignerFormEditorInterface * ) {
    if( initialized ) {
        return;
    }
    initialized = true;
}

/*!
    ???
*/
bool LinkPluginManager::isInitialized() const {
    return initialized;
}

/*!
    Widget factory. Creates a Link widget.
*/
QWidget *LinkPluginManager::createWidget ( QWidget *parent ) {
    return new LinkPlugin(parent);
}

/*!
    Name for widget. Used by Qt Designer in widget list.
*/
QString LinkPluginManager::name() const {
    return "LinkPlugin";
}

/*!
    Name of group Qt Designer will add widget to.
*/
QString LinkPluginManager::group() const {
    return "EPICS Widgets";
}

/*!
    Icon for widget. Used by Qt Designer in widget list.
*/
QIcon LinkPluginManager::icon() const {
    return QIcon(":/icons/Link.png");
}

/*!
    Tool tip for widget. Used by Qt Designer in widget list.
*/
QString LinkPluginManager::toolTip() const {
    return "Link";
}

/*!
    ???
*/
QString LinkPluginManager::whatsThis() const {
    return "Link";
}

/*!
    ???
*/
bool LinkPluginManager::isContainer() const {
    return false;
}

/*!
    ???
*/
/*QString LinkPluginManager::domXml() const {
    return "<widget class=\"Link\" name=\"Link\">\n"
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
QString LinkPluginManager::includeFile() const {
    return "LinkPlugin.h";
}
