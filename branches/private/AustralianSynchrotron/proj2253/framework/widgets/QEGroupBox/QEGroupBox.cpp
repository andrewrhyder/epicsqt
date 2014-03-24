/*  QEGroupBox.cpp
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
 *  Copyright (c) 2012
 *
 *  Author:
 *    Andrew Starritt
 *  Contact details:
 *    andrew.starritt@synchrotron.org.au
 */

#include <QEGroupBox.h>

//------------------------------------------------------------------------------
//
QEGroupBox::QEGroupBox (QWidget *parent) : QGroupBox (parent), QEWidget (this)
{
   this->setTitle (" QEGroupBox ");

   // This is not an EPICS aware widget.
   //
   this->setVariableAsToolTip (false);
   this->setAllowDrop (false);
   this->setNumVariables (0);
}

//------------------------------------------------------------------------------
//
QEGroupBox::QEGroupBox (const QString& title, QWidget* parent) : QGroupBox (title, parent), QEWidget (this)
{
   // This is not an EPICS aware widget.
   //
   this->setVariableAsToolTip (false);
   this->setAllowDrop (false);
   this->setNumVariables (0);
   }

//------------------------------------------------------------------------------
//
QEGroupBox::~QEGroupBox () {
}

//------------------------------------------------------------------------------
//
QSize QEGroupBox::sizeHint () const {
    return QSize (120, 80);
}

// end