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
 *  Copyright (c) 2012
 *
 *  Author:
 *    Ricardo Fernandes
 *  Contact details:
 *    ricardo.fernandes@synchrotron.org.au
 */

#include <QCaLoginPluginManager.h>
#include <QCaLoginPlugin.h>
#include <QtPlugin>


/*!
    ???
*/
QCaLoginPluginManager::QCaLoginPluginManager( QObject *parent ) : QObject( parent ) {
    initialized = false;
}

/*!
    ???
*/
void QCaLoginPluginManager::initialize( QDesignerFormEditorInterface * ) {
    if( initialized ) {
        return;
    }
    initialized = true;
}

/*!
    ???
*/
bool QCaLoginPluginManager::isInitialized() const {
    return initialized;
}

/*!
    Widget factory. Creates a QCaLoginPlugin widget.
*/
QWidget *QCaLoginPluginManager::createWidget ( QWidget *parent ) {
    return new QCaLoginPlugin(parent);
}

/*!
    Name for widget. Used by Qt Designer in widget list.
*/
QString QCaLoginPluginManager::name() const {
    return "QCaLoginPlugin";
}

/*!
    Name of group Qt Designer will add widget to.
*/
QString QCaLoginPluginManager::group() const {
    return "EPICS Widgets";
}

/*!
    Icon for widget. Used by Qt Designer in widget list.
*/
QIcon QCaLoginPluginManager::icon() const {
    return QIcon(":/icons/QCaLogin.png");
}

/*!
    Tool tip for widget. Used by Qt Designer in widget list.
*/
QString QCaLoginPluginManager::toolTip() const {
    return "EPICS Label";
}

/*!
    ???
*/
QString QCaLoginPluginManager::whatsThis() const {
    return "EPICS Label";
}

/*!
    ???
*/
bool QCaLoginPluginManager::isContainer() const {
    return false;
}

/*!
    ???
*/
/*QString QCaLoginPluginManager::domXml() const {
    return "<widget class=\"QCaLogin\" name=\"qCaLogin\">\n"
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
QString QCaLoginPluginManager::includeFile() const {
    return "QCaLoginPlugin.h";
}
