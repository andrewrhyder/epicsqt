/*  QEDialog.cpp
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

#include <QDebug>
#include <QApplication>
#include <QDesktopWidget>
#include <QMainWindow>
#include <QTimer>

#include "QECommon.h"
#include "QEDialog.h"

#define DEBUG qDebug () << "QEDialog" << __FUNCTION__ << ":" << __LINE__

//------------------------------------------------------------------------------
//
QEDialog::QEDialog (QWidget* parent) : QDialog (parent) {
   this->sourceWidget = this;
}

//------------------------------------------------------------------------------
//
void QEDialog::setSourceWidget (QWidget* widget)
{
   this->sourceWidget = widget;
}

//------------------------------------------------------------------------------
//
int QEDialog::exec (QWidget* targetWidgetIn)
{
   this->targetWidget = targetWidgetIn;

   // Allow 5 mSec to allow dialog widget to "sort itself out" before trying to
   // relocate it. This is particularly important on first activation.
   // Empirically found that we need more than 1 mSec.
   //
   QTimer::singleShot (10, this, SLOT (relocateToCenteredPosition ()));

   // Now call parent exec method to do actual work.
   //
   return QDialog::exec ();
}

//------------------------------------------------------------------------------
//
void QEDialog::relocateToCenteredPosition ()
{
   // Did caller specify an widget to centre this over?
   //
   if (this->targetWidget && this->sourceWidget) {

      // Find centres and map this to global coordinates.
      //
      const QRect sourceGeo = this->sourceWidget->geometry ();
      const QRect targetGeo = this->targetWidget->geometry ();

      QPoint sourceMiddle = QPoint (sourceGeo.width () / 2, sourceGeo.height () / 2);
      QPoint targetMiddle = QPoint (targetGeo.width () / 2, targetGeo.height () / 2);

      // Convert both to global coordinates.
      //
      sourceMiddle = this->sourceWidget->mapToGlobal (sourceMiddle);
      targetMiddle = this->targetWidget->mapToGlobal (targetMiddle);

      // Calculate difference between where we are and where we want to get to.
      //
      QPoint delta = targetMiddle - sourceMiddle;

      // Extract current dialog location and calculate translation offset.
      // Move dialog widget geometry rectangle, careful not to change width or
      // height and apply.
      //
      QRect dialogGeo = this->geometry ();
      dialogGeo.translate (delta);

      // Sanity check - ensure no off screen mis-calculations.
      //
      dialogGeo =  QEDialog::constrainGeometry (dialogGeo);

      this->setGeometry (dialogGeo);
   }
}

//------------------------------------------------------------------------------
//
QRect QEDialog::constrainGeometry (const QRect& geometry)
{
   const int gap = 20;
   const QRect screen = QApplication::desktop ()->screenGeometry (0);
   const QSize size = geometry.size ();
   QPoint position = geometry.topLeft ();

   int stop = screen.top ();
   int sleft = screen.left ();
   int sright = screen.right ();
   int sbottom = screen.bottom ();

   // We assume screens are in a regular block, e.g. 1x1, 1x4, 2x2, 2x1 etc.
   //
   for (int j = 1; j < QApplication::desktop ()->screenCount(); j++) {
      const QRect s2 = QApplication::desktop ()->screenGeometry (j);
      stop    = MIN (stop,    s2.top());
      sleft   = MIN (sleft,   s2.left());
      sright  = MAX (sright,  s2.right());
      sbottom = MAX (sbottom, s2.bottom());
   }

   // Constain X position.
   //
   position.setX (MIN (position.x (), sright - size.width () - gap));
   position.setX (MAX (position.x (), sleft  + gap));

   // Constain Y position.
   //
   position.setY (MIN (position.y (), sbottom - size.height () - gap));
   position.setY (MAX (position.y (), stop    + gap));

   return QRect (position, size);
}

// end
