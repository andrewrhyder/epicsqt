/*  QENumericEditManager.cpp
 *
 *  This file is part of the EPICS QT Framework, initially developed at the Australian Synchrotron.
 *
 *  The EPICS QT Framework is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  The EPICS QT Framework is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with the EPICS QT Framework.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Copyright (c) 2013 Australian Synchrotron
 *
 *  Author:
 *    Andrew Starritt
 *  Contact details:
 *    andrew.starritt@synchrotron.org.au
 */

#include <QENumericEditManager.h>
#include <QENumericEdit.h>
#include <QtPlugin>

//------------------------------------------------------------------------------
//
QENumericEditManager::QENumericEditManager (QObject * parent):QObject
    (parent)
{
   initialized = false;
}

//------------------------------------------------------------------------------
//
void QENumericEditManager::initialize (QDesignerFormEditorInterface *)
{
   if (initialized) {
      return;
   }
   initialized = true;
}

//------------------------------------------------------------------------------
//
bool QENumericEditManager::isInitialized () const
{
   return initialized;
}

//------------------------------------------------------------------------------
//
QWidget *QENumericEditManager::createWidget (QWidget * parent)
{
   return new QENumericEdit (parent);
}

//------------------------------------------------------------------------------
//
QString QENumericEditManager::name () const
{
   return "QENumericEdit";
}

//------------------------------------------------------------------------------
//
QString QENumericEditManager::group () const
{
   return "EPICSQt Controls";
}

//------------------------------------------------------------------------------
//
QIcon QENumericEditManager::icon () const
{
   return QIcon (":/qe/numericedit/QENumericEdit.png");
}

//------------------------------------------------------------------------------
//
QString QENumericEditManager::toolTip () const
{
   return "EPICS Numeric Edit";
}

//------------------------------------------------------------------------------
//
QString QENumericEditManager::whatsThis () const
{
   return "EPICS Numeric Edit";
}

//------------------------------------------------------------------------------
//
bool QENumericEditManager::isContainer () const
{
   return false;
}

//------------------------------------------------------------------------------
//
QString QENumericEditManager::includeFile () const
{
   return "QENumericEdit.h";
}

// end