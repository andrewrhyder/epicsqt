/*  QEDialog.cpp
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
 */

#include <QMainWindow>
#include "QEDialog.h"

//------------------------------------------------------------------------------
//
QEDialog::QEDialog (QWidget* parent) : QDialog (parent) {
   // place holder
}


//------------------------------------------------------------------------------
//
int QEDialog::exec (QWidget* centreOver)
{
   // Did caller specify an widget to centre this over?
   //
   if (centreOver) {
      // Find center and map this to global coordinates.
      //
      const QRect cr = centreOver->geometry ();
      QPoint centre = QPoint (cr.width () / 2, cr.height () / 2);
      centre = centreOver->mapToGlobal (centre);

      // Extract current dialog location and calculate translation offset.
      //
      QRect dr = this->geometry ();
      int dx = centre.x () - (dr.x () + dr.width ()/2);
      int dy = centre.y () - (dr.y () + dr.height ()/2);

      // Move dialog widget geometry rectangle, careful not to change width or
      // height and apply.
      //
      dr.translate (dx, dy);
      setGeometry (dr);
   }

   // Now call parent exec method to do actual work.
   //
   return QDialog::exec ();
}

// end
