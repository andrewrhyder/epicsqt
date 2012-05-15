/*!
  \class QCaLoginPluginManager
  \version $Revision: #4 $
  \date $DateTime: 2010/06/23 07:49:40 $
  \author andrew.rhyder
  \brief CA Label Widget Plugin Manager for designer.
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
 *  Copyright (c) 2012
 *
 *  Author:
 *    Ricardo Fernandes
 *  Contact details:
 *    ricardo.fernandes@synchrotron.org.au
 */

#ifndef QCALOGINPLUGINMANAGER_H
#define QCALOGINPLUGINMANAGER_H

#include <QDesignerCustomWidgetInterface>
#include <QCaPluginLibrary_global.h>

/*!
    ???
*/
class QCAPLUGINLIBRARYSHARED_EXPORT QCaLoginPluginManager : public QObject, public QDesignerCustomWidgetInterface {
     Q_OBJECT
     Q_INTERFACES(QDesignerCustomWidgetInterface)

  public:
    QCaLoginPluginManager( QObject *parent = 0 );

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

#endif /// QCALOGINPLUGINMANAGER_H
