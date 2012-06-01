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
 *  Copyright (c) 2012
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

// As intended to QCaStripChart in particular, we just use a double
// value and not bother with a variant.
//
// A double can use used for all EPICS data types except string.
//
struct QCaDataPoint {
   double value;
   QCaDateTime datetime;  // datetime + nSec
   QCaAlarmInfo alarm;
};

// Defines a vector of data points.
//
class QCaDataPointList : public QVector<struct QCaDataPoint>  {
   // no extra members (so far).
};

#endif  // QCADATAPOINT_H
