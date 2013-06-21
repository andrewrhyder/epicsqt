/* QCaDataPoint.cpp
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

#include <QEArchiveInterface.h>
#include <QCaDataPoint.h>

//------------------------------------------------------------------------------
//
QCaDataPoint::QCaDataPoint ()
{
   this->value = 0.0;

   // Register type.
   //
   qRegisterMetaType<QCaDataPoint> ("QCaDataPoint");
}

//------------------------------------------------------------------------------
//
bool QCaDataPoint::isDisplayable ()
{
   bool result;
   QEArchiveInterface::archiveAlarmSeverity severity;

   severity = (QEArchiveInterface::archiveAlarmSeverity) this->alarm.getSeverity ();

   switch (severity) {

      case QEArchiveInterface::archSevNone:
      case QEArchiveInterface::archSevMinor:
      case QEArchiveInterface::archSevMajor:
      case QEArchiveInterface::archSevEstRepeat:
      case QEArchiveInterface::archSevRepeat:
         result = true;
         break;

      case QEArchiveInterface::archSevInvalid:
      case QEArchiveInterface::archSevDisconnect:
      case QEArchiveInterface::archSevStopped:
      case QEArchiveInterface::archSevDisabled:
         result = false;
         break;

      default:
         result = false;
         break;
   }

   return result;
}

//------------------------------------------------------------------------------
//
QCaDataPointList::QCaDataPointList () : QList<QCaDataPoint> ()
{
   // Register type.
   //
   qRegisterMetaType<QCaDataPointList> ("QCaDataPointList");
}

//------------------------------------------------------------------------------
//
void QCaDataPointList::resample (const QCaDataPointList& source,
                                 const double interval,
                                 const QCaDateTime& endTime)
{
   QCaDateTime firstTime;
   int j;
   int next;
   QCaDateTime jthTime;
   QCaDataPoint point;

   this->clear ();
   if (source.count () <= 0) return;

   firstTime = source.value (0).datetime;
   jthTime = firstTime;
   next = 0;
   for (j = 0; jthTime < endTime; j++) {

      // Calculate to nearest mSec.
      //
      jthTime = firstTime.addMSecs ((qint64)( (double) j * 1000.0 * interval));

      while (next < source.count () && source.value (next).datetime <= jthTime) next++;
      point = source.value (next - 1);
      point.datetime = jthTime;
      this->append (point);
   }
}

// end
