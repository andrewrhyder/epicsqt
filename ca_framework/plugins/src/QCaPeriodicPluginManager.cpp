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
 *    Andrew Rhyder
 *  Contact details:
 *    andrew.rhyder@synchrotron.org.au
 */

#include <QtDesigner>
#include <QCaPeriodicPluginManager.h>
#include <QCaPeriodicPlugin.h>
#include <QCaPeriodicPluginTaskMenu.h>
#include <QtPlugin>

/*!
    ???
*/
QCaPeriodicPluginManager::QCaPeriodicPluginManager( QObject *parent ) : QObject( parent ) {
    initialized = false;
}

/*!
    ???
*/
void QCaPeriodicPluginManager::initialize( QDesignerFormEditorInterface * formEditor ) {
    if( initialized ) {
        return;
    }

    QExtensionManager *manager = formEditor->extensionManager();
    Q_ASSERT(manager != 0);

    manager->registerExtensions(new QCaPeriodicPluginTaskMenuFactory(manager),
                                Q_TYPEID(QDesignerTaskMenuExtension));


    initialized = true;
}

/*!
    ???
*/
bool QCaPeriodicPluginManager::isInitialized() const {
    return initialized;
}

/*!
    Widget factory. Creates a QCaPeriodicPlugin widget.
*/
QWidget *QCaPeriodicPluginManager::createWidget ( QWidget *parent ) {
    return new QCaPeriodicPlugin(parent);
}

/*!
    Name for widget. Used by Qt Designer in widget list.
*/
QString QCaPeriodicPluginManager::name() const {
    return "QCaPeriodicPlugin";
}

/*!
    Name of group Qt Designer will add widget to.
*/
QString QCaPeriodicPluginManager::group() const {
    return "EPICS Widgets";
}

/*!
    Icon for widget. Used by Qt Designer in widget list.
*/
QIcon QCaPeriodicPluginManager::icon() const {
    return QIcon(":/icons/QCaPeriodic.png");
}

/*!
    Tool tip for widget. Used by Qt Designer in widget list.
*/
QString QCaPeriodicPluginManager::toolTip() const {
    return "EPICS Push Button";
}

/*!
    ???
*/
QString QCaPeriodicPluginManager::whatsThis() const {
    return "EPICS Push Button";
}

/*!
    ???
*/
bool QCaPeriodicPluginManager::isContainer() const {
    return false;
}

/*!
    ???
*/
/*QString QCaPeriodicPluginManager::domXml() const {
    return "<widget class=\"QCaPeriodic\" name=\"qCaPeriodic\">\n"
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
QString QCaPeriodicPluginManager::includeFile() const {
    return "QCaPeriodicPlugin.h";
}

