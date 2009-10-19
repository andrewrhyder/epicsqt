/* $File: //ASP/Dev/SBS/4_Controls/4_8_GUI_Frameworks/4_8_2_Qt/sw/ca_framework/plugins/src/QCaSliderPluginManager.cpp $
 * $Revision: #1 $
 * $DateTime: 2009/07/14 15:59:56 $
 * Last checked in by: $Author: rhydera $
 */

/*! 
  \class QCaSliderPluginManager
  \version $Revision: #1 $
  \date $DateTime: 2009/07/14 15:59:56 $
  \author andrew.rhyder
  \brief CA Slider Widget Plugin Manager for designer.
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

#include <QCaSliderPluginManager.h>
#include <QCaSliderPlugin.h>
#include <QtPlugin>

/*!
    ???
*/
QCaSliderPluginManager::QCaSliderPluginManager( QObject *parent ) : QObject( parent ) {
    initialized = false;
}

/*!
    ???
*/
void QCaSliderPluginManager::initialize( QDesignerFormEditorInterface * ) {
    if( initialized ) {
        return;
    }
    initialized = true;
}

/*!
    ???
*/
bool QCaSliderPluginManager::isInitialized() const {
    return initialized;
}

/*!
    Widget factory. Creates a QCaLabel widget.
*/
QWidget *QCaSliderPluginManager::createWidget ( QWidget *parent ) {
    return new QCaSliderPlugin(parent);
}

/*!
    Name for widget. Used by Qt Designer in widget list.
*/
QString QCaSliderPluginManager::name() const {
    return "QCaSliderPlugin";
}

/*!
    Name of group Qt Designer will add widget to.
*/
QString QCaSliderPluginManager::group() const {
    return "EPICS Widgets";
}

/*!
    Icon for widget. Used by Qt Designer in widget list.
*/
QIcon QCaSliderPluginManager::icon() const {
    return QIcon(":/icons/QCaSlider.png");
}

/*!
    Tool tip for widget. Used by Qt Designer in widget list.
*/
QString QCaSliderPluginManager::toolTip() const {
    return "EPICS Slider";
}

/*!
    ???
*/
QString QCaSliderPluginManager::whatsThis() const {
    return "EPICS Slider";
}

/*!
    ???
*/
bool QCaSliderPluginManager::isContainer() const {
    return false;
}

/*!
    ???
*/
/*QString QCaSliderPluginManager::domXml() const {
    return "<widget class=\"QCaSlider\" name=\"qCaSlider\">\n"
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
QString QCaSliderPluginManager::includeFile() const {
    return "QCaSliderPlugin.h";
}
