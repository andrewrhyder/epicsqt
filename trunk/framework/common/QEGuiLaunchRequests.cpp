/*  QEGuiLaunchRequests.cpp
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

#include "QEGuiLaunchRequests.h"

//---------------------------------------------------------------------------
//
QEGuiLaunchRequests::QEGuiLaunchRequests ()
{
   this->kind = KindNone;
   this->arguments.clear ();
}

//---------------------------------------------------------------------------
//
QEGuiLaunchRequests::QEGuiLaunchRequests (const Kinds kindIn,
                                          const QStringList & argumentsIn,
                                          const Options optionIn)
{
   this->kind = kindIn;
   this->arguments = argumentsIn;
   this->option = optionIn;
}

//---------------------------------------------------------------------------
//
QEGuiLaunchRequests::QEGuiLaunchRequests (const QString & filename,
                                          const Options optionIn)
{
   this->kind = KindFileName;
   this->arguments.clear ();
   this->arguments << filename;
   this->option = optionIn;
}

//---------------------------------------------------------------------------
// Set and get functions.
//
void QEGuiLaunchRequests::setKind (const Kinds kindIn)
{
   this->kind = kindIn;
}

QEGuiLaunchRequests::Kinds QEGuiLaunchRequests::getKind () const
{
   return this->kind;
}

//---------------------------------------------------------------------------
//
void QEGuiLaunchRequests::setArguments (const QStringList & argumentsIn)
{
   this->arguments = argumentsIn;
}

QStringList QEGuiLaunchRequests::getArguments () const
{
   return this->arguments;
}

//---------------------------------------------------------------------------//
//
void QEGuiLaunchRequests::setOption (const Options optionIn)
{
   this->option = optionIn;
}

QEGuiLaunchRequests::Options QEGuiLaunchRequests::getOption () const
{
   return this->option;
}

// end
