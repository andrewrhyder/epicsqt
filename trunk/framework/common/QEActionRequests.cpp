/*  QEActionRequests.cpp
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
 *  Copyright (c) 2013
 *
 *  Author:
 *    Andrew Starritt
 *  Contact details:
 *    andrew.starritt@synchrotron.org.au
 *
 */

#include "QEActionRequests.h"

//---------------------------------------------------------------------------
//
QEActionRequests::QEActionRequests ()
{
   this->kind = KindNone;
   this->arguments.clear ();
}

//---------------------------------------------------------------------------
//
QEActionRequests::QEActionRequests( const QString& action,
                                    const QString& pvName)
{
   kind = KindAction;
   arguments.clear ();
   arguments << action;
   arguments << pvName;
   option = OptionNewWindow;
}

//---------------------------------------------------------------------------
//
QEActionRequests::QEActionRequests( const QString& actionIn,
                                    const QString& widgetNameIn,
                                    const QStringList& argumentsIn )
{
   kind = KindWidgetAction;
   action = actionIn;
   widgetName = widgetNameIn;
   arguments = argumentsIn;
}

//---------------------------------------------------------------------------
//
QEActionRequests::QEActionRequests (const QString & filename,
                                          const QString & customisationIn,
                                          const Options optionIn)
{
   this->kind = KindOpenFile;
   this->arguments.clear ();
   this->arguments << filename;
   this->option = optionIn;
   customisation = customisationIn;
}

//---------------------------------------------------------------------------
//
QEActionRequests::QEActionRequests( const QList<windowCreationListItem> windowsIn )
{
   kind = KindOpenFiles;
   option = OptionNewWindow; // not required, but keep things neat
   for( int i = 0; i < windowsIn.count(); i++ )
   {
       windows.append( windowsIn.at(i) );
   }
}

//---------------------------------------------------------------------------
//
QEActionRequests::QEActionRequests( const QList<componentHostListItem> componentsIn )
{
   kind = KindHostComponents;
   option = OptionFloatingDockWindow; // not required, but keep things neat
   for( int i = 0; i < componentsIn.count(); i++ )
   {
       components.append( componentsIn.at(i) );
   }
}

//---------------------------------------------------------------------------
// Set and get functions.
//
void QEActionRequests::setKind (const Kinds kindIn)
{
   this->kind = kindIn;
}

QEActionRequests::Kinds QEActionRequests::getKind () const
{
   return this->kind;
}

//---------------------------------------------------------------------------
//
void QEActionRequests::setArguments (const QStringList & argumentsIn)
{
   this->arguments = argumentsIn;
}

QStringList QEActionRequests::getArguments () const
{
   return this->arguments;
}

//---------------------------------------------------------------------------//
//
void QEActionRequests::setOption (const Options optionIn)
{
   this->option = optionIn;
}

QEActionRequests::Options QEActionRequests::getOption () const
{
   return this->option;
}

//---------------------------------------------------------------------------//
//
void QEActionRequests::setAction(const QString actionIn )
{
   action = actionIn;
}

QString QEActionRequests::getAction() const
{
   return action;
}

//---------------------------------------------------------------------------//
//
void QEActionRequests::setWidgetName(const QString widgetNameIn )
{
   widgetName = widgetNameIn;
}

QString QEActionRequests::getWidgetName() const
{
   return widgetName;
}

//---------------------------------------------------------------------------//
//
QString QEActionRequests::getCustomisation() const
{
   return customisation;
}

//---------------------------------------------------------------------------//
//
QList<windowCreationListItem> QEActionRequests::getWindows() const
{
    return windows;
}

//---------------------------------------------------------------------------//
//
QList<componentHostListItem> QEActionRequests::getComponents() const
{
    return components;
}

// end
