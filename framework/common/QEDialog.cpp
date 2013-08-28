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
   // Did caller specify an widget (presumably a form) to centre this over?
   //
   if (centreOver) {

      const QRect cr = centreOver->geometry ();
      QPoint centre = QPoint (cr.left () + cr.width  () / 2,
                              cr.top ()  + cr.height () / 2);

      // Is centreOver a main window?
      //
      QMainWindow* w = dynamic_cast<QMainWindow*> (centreOver);
      if (!w) {
         // No, it is a regular widget - map centre to global position.
         //
         centre = centreOver->mapToGlobal (centre);
      }

      QRect dr = this->geometry ();
      dr.setLeft (centre.x () - dr.width  () / 2);
      dr.setTop  (centre.y () - dr.height () / 2);

      setGeometry (dr);
  }

   // Now call parent exec method to do actual work.
   //
   return QDialog::exec ();
}

// end
