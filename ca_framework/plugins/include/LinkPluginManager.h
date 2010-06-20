/*! 
  \class LinkPluginManager
  \version $Revision: #1 $
  \date $DateTime: 2010/01/25 16:09:07 $
  \author andrew.rhyder
  \brief Link Widget Plugin Manager for designer.
 */
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
 *    andrew.rhyder@synchrotron.org
 */

#ifndef LINKPLUGINMANAGER_H
#define LINKPLUGINMANAGER_H

#include <QDesignerCustomWidgetInterface>
#include <QCaPluginLibrary_global.h>

/*!
    ???
*/
class QCAPLUGINLIBRARYSHARED_EXPORT LinkPluginManager : public QObject, public QDesignerCustomWidgetInterface {
     Q_OBJECT
     Q_INTERFACES(QDesignerCustomWidgetInterface)

  public:
    LinkPluginManager( QObject *parent = 0 );

    bool isContainer() const;
    bool isInitialized() const;
    QIcon icon() const;
    //QString domXml() const;
    QString group() const;
    QString includeFile() const;
    QString name() const;
    QString toolTip() const;
    QString whatsThis() const;
    QWidget *createWidget( QWidget *parent );
    void initialize( QDesignerFormEditorInterface *core );

  private:
    bool initialized;
};

#endif /// LINKPLUGINMANAGER_H
