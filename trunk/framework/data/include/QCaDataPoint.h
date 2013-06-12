/* QCaDataPoint.h
 *
 *  This file is part of the EPICS QT Framework, initially developed at the
 *  Australian Synchrotron.
 *
 *  The EPICS QT Framework is free software: you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as published
 *  by the Free Software Foundation, either version 3 of the License, or
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

#ifndef QCADATAPOINT_H
#define QCADATAPOINT_H

#include <QVector>
#include <QCaAlarmInfo.h>
#include <QCaDateTime.h>

// This struct used to hold a single data point. Objects of this type are
// intended for use QCaStripChart in particular, but also for the interface
// to the Channel Access archives.
//
class QCaDataPoint {
public:
   explicit QCaDataPoint ();
   bool isDisplayable ();     // i.e is okay

   // We don't bother with a variant but just use a double.  A double can be
   // used to hold all CA data types except strings (which is are not plotable).
   //
   double value;
   QCaDateTime datetime;      // datetime + nSec
   QCaAlarmInfo alarm;
};

// Defines a vector of data points.
//
class QCaDataPointList : public QVector<QCaDataPoint>  {
   // no extra members (so far).
};

#endif  // QCADATAPOINT_H
