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

#include <QDebug>
#include <QMainWindow>

#include "QEDialog.h"

//------------------------------------------------------------------------------
//
QEDialog::QEDialog (QWidget* parent) : QDialog (parent) {
   this->sourceWidget = this;
   this->firstExec = true;
}


//------------------------------------------------------------------------------
//
int QEDialog::exec (QWidget* targetWidget)
{
   // Did caller specify an widget to centre this over?
   //
   if (targetWidget && this->sourceWidget) {
      // Find centres and map this to global coordinates.
      //
      const QRect sourceGeo = sourceWidget->geometry ();
      const QRect targetGeo = targetWidget->geometry ();

      QPoint sourceMiddle = QPoint (sourceGeo.width () / 2, sourceGeo.height () / 2);
      QPoint targetMiddle = QPoint (targetGeo.width () / 2, targetGeo.height () / 2);

      // Convert both to global coordinates.
      //
      targetMiddle = targetWidget->mapToGlobal (targetMiddle);
      sourceMiddle = sourceWidget->mapToGlobal (sourceMiddle);

      QPoint delta = targetMiddle - sourceMiddle;

      // Empircally determined offset needed for this to work as expected.
      //
      if (this->firstExec) {
         // Very first time this is different.
         // This seems to be related to fact taht widget has no-relative position
         // until first displayed.
         //
         delta = delta + QPoint (-112, -42);
      } else {
         // I think this is related to the window decoration sizes.
         //
         delta = delta + QPoint (4, 22);
      }

      // Extract current dialog location and calculate translation offset.
      // Move dialog widget geometry rectangle, careful not to change width or
      // height and apply.
      //
      QRect dialogGeo = this->geometry ();
      dialogGeo.translate (delta);
      setGeometry (dialogGeo);
   }

   this->firstExec = false;

   // Now call parent exec method to do actual work.
   //
   return QDialog::exec ();
}

// end
