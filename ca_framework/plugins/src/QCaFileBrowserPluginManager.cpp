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

#include <QCaFileBrowserPluginManager.h>
#include <QCaFileBrowser.h>
#include <QtPlugin>



QCaFileBrowserPluginManager::QCaFileBrowserPluginManager(QObject *pParent):QObject(pParent)
{

    initialized = false;

}



void QCaFileBrowserPluginManager::initialize(QDesignerFormEditorInterface *)
{

    if (initialized == false)
    {
        initialized = true;
    }

}



bool QCaFileBrowserPluginManager::isInitialized() const
{

    return initialized;

}



QWidget *QCaFileBrowserPluginManager::createWidget(QWidget *pParent)
{

    return new QCaFileBrowser(pParent);
}



QString QCaFileBrowserPluginManager::name() const
{

    return "QCaFileBrowser";

}



QString QCaFileBrowserPluginManager::group() const
{

    return "EPICS Widgets";

}



QIcon QCaFileBrowserPluginManager::icon() const
{

    return QIcon(":/icons/QCaFileBrowser.png");

}



QString QCaFileBrowserPluginManager::toolTip() const
{

    return "EPICS File Browser";

}



QString QCaFileBrowserPluginManager::whatsThis() const
{

    return "EPICS File Browser";

}



bool QCaFileBrowserPluginManager::isContainer() const
{

    return false;
}



QString QCaFileBrowserPluginManager::includeFile() const
{

    return "QCaFileBrowserPluginManager.h";

}




/*QString QCaFileBrowserPluginManager::domXml() const {
    return "<widget class=\"QCaFileBrowser\" name=\"qCaFileBrowser\">\n"
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


