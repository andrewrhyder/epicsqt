/*  QEAnalogIndicatorManager.cpp
 * 
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
 *  Copyright (c) 2012
 *
 *  Author:
 *    Andrew Starritt
 *  Contact details:
 *    andrew.starritt@synchrotron.org.au
 */

#include <QEAnalogIndicatorManager.h>
#include <QEAnalogIndicator.h>
#include <QtPlugin>

/*!
    ???
*/
QEAnalogIndicatorManager::QEAnalogIndicatorManager( QObject *parent ) : QObject( parent )
{
    initialized = false;
}

/*!
    ???
*/
void QEAnalogIndicatorManager::initialize( QDesignerFormEditorInterface * )
{
    if( initialized ) {
        return;
    }
    initialized = true;
}

/*!
    ???
*/
bool QEAnalogIndicatorManager::isInitialized() const
{
    return initialized;
}

/*!
    Widget factory. Creates a QEAnalogIndicator widget.
*/
QWidget *QEAnalogIndicatorManager::createWidget ( QWidget *parent )
{
    return new QEAnalogIndicator( parent );
}

/*!
    Name for widget. Used by Qt Designer in widget list.
*/
QString QEAnalogIndicatorManager::name() const
{
    return "QEAnalogIndicator";
}

/*!
    Name of group Qt Designer will add widget to.
*/
QString QEAnalogIndicatorManager::group() const
{
    return "EPICS Widgets";
}

/*!
    Icon for widget. Used by Qt Designer in widget list.
*/
QIcon QEAnalogIndicatorManager::icon() const
{
    return QIcon(":/qe/analogindicator/QEAnalogIndicator.png");
}

/*!
    Tool tip for widget. Used by Qt Designer in widget list.
*/
QString QEAnalogIndicatorManager::toolTip() const
{
    return "Analog Indicator";
}

/*!
    ???
*/
QString QEAnalogIndicatorManager::whatsThis() const
{
    return "Analog Indicator";
}

/*!
    ???
*/
bool QEAnalogIndicatorManager::isContainer() const
{
    return false;
}

/*!
    ???
*/
/*QString QEAnalogIndicatorManager::domXml() const {
    return "<widget class=\"QEAnalogIndicator\" name=\"qAnalogIndicator\">\n"
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
QString QEAnalogIndicatorManager::includeFile() const
{
    return "QEAnalogIndicator.h";
}

// end
