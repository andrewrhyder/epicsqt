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
 *  Copyright (c) 2011
 *
 *  Author:
 *    Andrew Starritt
 *  Contact details:
 *    andrew.starritt@synchrotron.org.au
 */

#include <QAnalogProgressBarPluginManager.h>
#include <QAnalogProgressBar.h>
#include <QtPlugin>

/*!
    ???
*/
QAnalogProgressBarPluginManager::QAnalogProgressBarPluginManager( QObject *parent ) : QObject( parent ) {
    initialized = false;
}

/*!
    ???
*/
void QAnalogProgressBarPluginManager::initialize( QDesignerFormEditorInterface * ) {
    if( initialized ) {
        return;
    }
    initialized = true;
}

/*!
    ???
*/
bool QAnalogProgressBarPluginManager::isInitialized() const {
    return initialized;
}

/*!
    Widget factory. Creates a QAnalogProgressBar widget.
*/
QWidget *QAnalogProgressBarPluginManager::createWidget ( QWidget *parent ) {
    return new QAnalogProgressBar( parent );
}

/*!
    Name for widget. Used by Qt Designer in widget list.
*/
QString QAnalogProgressBarPluginManager::name() const {
    return "QAnalogProgressBar";
}

/*!
    Name of group Qt Designer will add widget to.
*/
QString QAnalogProgressBarPluginManager::group() const {
    return "EPICS Widgets";
}

/*!
    Icon for widget. Used by Qt Designer in widget list.
*/
QIcon QAnalogProgressBarPluginManager::icon() const {
    return QIcon(":/icons/QAnalogProgressBar.png");
}

/*!
    Tool tip for widget. Used by Qt Designer in widget list.
*/
QString QAnalogProgressBarPluginManager::toolTip() const {
    return "Analog Progress Bar";
}

/*!
    ???
*/
QString QAnalogProgressBarPluginManager::whatsThis() const {
    return "Analog Progress Bar";
}

/*!
    ???
*/
bool QAnalogProgressBarPluginManager::isContainer() const {
    return false;
}

/*!
    ???
*/
/*QString QAnalogProgressBarPluginManager::domXml() const {
    return "<widget class=\"QAnalogProgressBar\" name=\"qAnalogProgressBar\">\n"
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
QString QAnalogProgressBarPluginManager::includeFile() const {
    return "QAnalogProgressBar.h";
}

// end
