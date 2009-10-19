/* $File: //ASP/Dev/SBS/4_Controls/4_8_GUI_Frameworks/4_8_2_Qt/sw/ca_framework/plugins/include/QCaPushButtonPluginManager.h $
 * $Revision: #2 $
 * $DateTime: 2009/08/18 12:28:07 $
 * Last checked in by: $Author: rhydera $
 */

/*! 
  \class QCaPushButtonPluginManager
  \version $Revision: #2 $
  \date $DateTime: 2009/08/18 12:28:07 $
  \author andrew.rhyder
  \brief CA Push Button Widget Plugin Manager for designer.
 */

/* Copyright (c) 2009 Australian Synchrotron
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * Licence as published by the Free Software Foundation; either
 * version 2.1 of the Licence, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public Licence for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * Licence along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Contact details:
 * andrew.rhyder@synchrotron.org.au
 * 800 Blackburn Road, Clayton, Victoria 3168, Australia.
 *
 */

#ifndef QCAPUSHBUTTONPLUGINMANAGER_H
#define QCAPUSHBUTTONPLUGINMANAGER_H

#include <QDesignerCustomWidgetInterface>
#include <QCaPluginLibrary_global.h>

/*!
    ???
*/
class QCAPLUGINLIBRARYSHARED_EXPORT QCaPushButtonPluginManager : public QObject, public QDesignerCustomWidgetInterface {
     Q_OBJECT
     Q_INTERFACES(QDesignerCustomWidgetInterface)

  public:
    QCaPushButtonPluginManager( QObject *parent = 0 );

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

#endif /// QCAPUSHBUTTONPLUGINMANAGER_H
