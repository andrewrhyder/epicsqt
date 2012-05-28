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

#include <QCaLogPluginManager.h>
#include <QCaLog.h>
#include <QtPlugin>



QCaLogPluginManager::QCaLogPluginManager(QObject *pParent):QObject(pParent)
{

    initialized = false;

}



void QCaLogPluginManager::initialize(QDesignerFormEditorInterface *)
{

    if (initialized == false)
    {
        initialized = true;
    }

}



bool QCaLogPluginManager::isInitialized() const
{

    return initialized;

}



QWidget *QCaLogPluginManager::createWidget(QWidget *pParent)
{

    return new QCaLog(pParent);
}



QString QCaLogPluginManager::name() const
{

    return "QCaLog";

}



QString QCaLogPluginManager::group() const
{

    return "EPICS Widgets";

}



QIcon QCaLogPluginManager::icon() const
{

    return QIcon(":/icons/QCaLog.png");

}



QString QCaLogPluginManager::toolTip() const
{

    return "EPICS Log";

}



QString QCaLogPluginManager::whatsThis() const
{

    return "EPICS Log";

}



bool QCaLogPluginManager::isContainer() const
{

    return false;
}



QString QCaLogPluginManager::includeFile() const
{

    return "QCaLogPluginManager.h";

}




/*QString QCaLogPluginManager::domXml() const {
    return "<widget class=\"QCaLog\" name=\"qCaLog\">\n"
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


