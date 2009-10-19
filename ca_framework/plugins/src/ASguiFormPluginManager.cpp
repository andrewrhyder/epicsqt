/* $File: //ASP/Dev/SBS/4_Controls/4_8_GUI_Frameworks/4_8_2_Qt/sw/ca_framework/plugins/src/ASguiFormPluginManager.cpp $
 * $Revision: #1 $ 
 * $DateTime: 2009/07/14 15:59:56 $
 * Last checked in by: $Author: rhydera $
 */

/*! 
  \version $Revision: #1 $
  \date $DateTime: 2009/07/14 15:59:56 $
  \author <andrew.rhyder>
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
 * Descrition:
 *
 */

#include <ASguiFormPluginManager.h>
#include <ASguiFormPlugin.h>

#include <QtPlugin>

ASguiFormPluginManager::ASguiFormPluginManager( QObject *parent ) : QObject( parent ) {
    initialized = false;
}

void ASguiFormPluginManager::initialize( QDesignerFormEditorInterface * ) {
    if( initialized ) {
        return;
    }
    initialized = true;
}

bool ASguiFormPluginManager::isInitialized() const {
    return initialized;
}

/// Widget factory. Creates a ASguiForm widget.
QWidget *ASguiFormPluginManager::createWidget ( QWidget *parent ) {
    return new ASguiFormPlugin(parent);
}

/// Name for widget. Used by Qt Designer in widget list
QString ASguiFormPluginManager::name() const {
    return "ASguiFormPlugin";
}

/// Name of group Qt Designer will add widget to
QString ASguiFormPluginManager::group() const {
    return "EPICS Widgets";
}

/// Icon for widget. Used by Qt Designer in widget list
QIcon ASguiFormPluginManager::icon() const {
    return QIcon(":/icons/ASguiForm.png");
}

/// Tool tip for widget. Used by Qt Designer in widget list
QString ASguiFormPluginManager::toolTip() const {
    return "ASgui form";
}

///
QString ASguiFormPluginManager::whatsThis() const {
    return "ASgui form";
}

///
bool ASguiFormPluginManager::isContainer() const {
    return false;
}

///
/*QString ASguiFormPluginManager::domXml() const {
    return "<widget class=\"ASguiForm\" name=\"ASguiForm\">\n"
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

///
QString ASguiFormPluginManager::includeFile() const {
    return "ASguiFormPlugin.h";
}
