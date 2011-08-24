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

#include <QCaRadioButtonPluginManager.h>
#include <QCaRadioButtonPlugin.h>
#include <QtPlugin>

/*!
    ???
*/
QCaRadioButtonPluginManager::QCaRadioButtonPluginManager( QObject *parent ) : QObject( parent ) {
    initialized = false;
}

/*!
    ???
*/
void QCaRadioButtonPluginManager::initialize( QDesignerFormEditorInterface * ) {
    if( initialized ) {
        return;
    }
    initialized = true;
}

/*!
    ???
*/
bool QCaRadioButtonPluginManager::isInitialized() const {
    return initialized;
}

/*!
    Widget factory. Creates a QCaLabel widget.
*/
QWidget *QCaRadioButtonPluginManager::createWidget ( QWidget *parent ) {
    return new QCaRadioButtonPlugin(parent);
}

/*!
    Name for widget. Used by Qt Designer in widget list.
*/
QString QCaRadioButtonPluginManager::name() const {
    return "QCaRadioButtonPlugin";
}

/*!
    Name of group Qt Designer will add widget to.
*/
QString QCaRadioButtonPluginManager::group() const {
    return "EPICS Widgets";
}

/*!
    Icon for widget. Used by Qt Designer in widget list.
*/
QIcon QCaRadioButtonPluginManager::icon() const {
    return QIcon(":/icons/QCaRadioButton.png");
}

/*!
    Tool tip for widget. Used by Qt Designer in widget list.
*/
QString QCaRadioButtonPluginManager::toolTip() const {
    return "EPICS Push Button";
}

/*!
    ???
*/
QString QCaRadioButtonPluginManager::whatsThis() const {
    return "EPICS Push Button";
}

/*!
    ???
*/
bool QCaRadioButtonPluginManager::isContainer() const {
    return false;
}

/*!
    ???
*/
/*QString QCaRadioButtonPluginManager::domXml() const {
    return "<widget class=\"QCaRadioButton\" name=\"qCaRadioButton\">\n"
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
QString QCaRadioButtonPluginManager::includeFile() const {
    return "QCaRadioButtonPlugin.h";
}
