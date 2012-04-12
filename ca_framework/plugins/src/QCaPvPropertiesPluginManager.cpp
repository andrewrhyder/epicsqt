/*
 *  This file is part of the EPICS QT Framework, initially developed at the
 *  Australian Synchrotron.
 *
 *  The EPICS QT Framework is free software: you can redistribute it and/or
 *  modify
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
 *  Copyright (c) 2012
 *
 *  Author:
 *    Andrew Starritt
 *  Contact details:
 *    andrew.starritt@synchrotron.org.au
 */

#include <QCaPvPropertiesPluginManager.h>
#include <QCaPvProperties.h>
#include <QtPlugin>

/*!
    ???
*/
QCaPvPropertiesPluginManager::QCaPvPropertiesPluginManager (QObject * parent) : QObject (parent)
{
   initialized = false;
}

/*!
    ???
*/
void QCaPvPropertiesPluginManager::initialize (QDesignerFormEditorInterface *)
{
   if (initialized) {
      return;
   }
   initialized = true;
}

/*!
    ???
*/
bool QCaPvPropertiesPluginManager::isInitialized () const
{
   return initialized;
}

/*!
    Widget factory. Creates a QCaPvProperties widget.
*/
QWidget *QCaPvPropertiesPluginManager::createWidget (QWidget * parent)
{
   return new QCaPvProperties (parent);
}

/*!
    Name for widget. Used by Qt Designer in widget list.
*/
QString QCaPvPropertiesPluginManager::name () const
{
   return "QCaPvProperties";
}

/*!
    Name of group Qt Designer will add widget to.
*/
QString QCaPvPropertiesPluginManager::group () const
{
   return "EPICS Widgets";
}

/*!
    Icon for widget. Used by Qt Designer in widget list.
*/
QIcon QCaPvPropertiesPluginManager::icon () const
{
   return QIcon (":/icons/QCaPvProperties.png");
}

/*!
    Tool tip for widget. Used by Qt Designer in widget list.
*/
QString QCaPvPropertiesPluginManager::toolTip () const
{
   return "EPICS IOC record fields";
}

/*!
    ???
*/
QString QCaPvPropertiesPluginManager::whatsThis () const
{
   return "EPICS IOC record fields";
}

/*!
    ???
*/
bool QCaPvPropertiesPluginManager::isContainer () const
{
   return false;  //???? true
}

/*!
    ???
*/
/*QString QCaPvPropertiesPluginManager::domXml() const {
    return "<widget class=\"QCaPvProperties\" name=\"qCaPvProperties\">\n"
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
QString QCaPvPropertiesPluginManager::includeFile () const
{
   return "QCaPvProperties.h";
}

// end
