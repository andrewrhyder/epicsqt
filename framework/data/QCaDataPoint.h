/* QCaDataPoint.h
 *
 *  This file is part of the EPICS QT Framework, initially developed at the
 *  Australian Synchrotron.
 *
 *  The EPICS QT Framework is free software: you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public License as published
 *  by the Free Software Foundation, either version 3 of the License, or
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

#ifndef QCADATAPOINT_H
#define QCADATAPOINT_H

#include <QList>
#include <QMetaType>
#include <QString>
#include <QTextStream>

#include <QCaAlarmInfo.h>
#include <QCaDateTime.h>
#include <QEPluginLibrary_global.h>

// This struct used to hold a single data point. Objects of this type are
// intended for use QCaStripChart in particular, but also for the interface
// to the Channel Access archives.
//
class QEPLUGINLIBRARYSHARED_EXPORT QCaDataPoint {
public:
   explicit QCaDataPoint ();
   bool isDisplayable () const;     // i.e. is okay

   // Generate image of point.
   //
   QString toString () const;                                   // basic
   QString toString (const QCaDateTime& originDateTime) const;  // ... plus a relative time

   // We don't bother with a variant but just use a double.  A double can be
   // used to hold all CA data types except strings (which is are not plotable).
   //
   double value;
   QCaDateTime datetime;      // datetime + nSec
   QCaAlarmInfo alarm;
};


// Defines a list of data points.
//
// Note this class orginally extended QList<QCaDataPoint>, but this way of
// specificying this class has issues with the Windows Visual Studio Compiler.
// It has now been modified to include a QList<QCaDataPoint> member. The
// downside of this is that we must now provide list member access functions.
//
class QEPLUGINLIBRARYSHARED_EXPORT QCaDataPointList  {
public:
   explicit QCaDataPointList ();

   // Provide access to the inner list.
   //
   void clear ()                               { data.clear ();         }
   void removeLast ()                          { data.removeLast ();    }
   void removeFirst ()                         { data.removeFirst ();   }
   void append (const QCaDataPointList& other);
   void append (const QCaDataPoint& r)         { data.append (r);       }
   void replace (int i, const QCaDataPoint &t) { data.replace (i, t);   }
   int count () const                          { return data.count ();  }
   QCaDataPoint value (const int j) const;
   QCaDataPoint last () const                  { return data.last ();   }

   // Resamples the source list on points into current list.
   // Items are resamples into data points at fixed time intervals.
   // No interploation - the "current" value is carried forward tp the next sample point(s).
   // Note: any previous data is lost.
   //
   void resample (const QCaDataPointList& source,
                  const double interval,
                  const QCaDateTime& endTime);

   // Removes duplicate sample points.
   // Note: any previous data is lost.
   //
   void compact (const QCaDataPointList& source);

   // Write whole list to target stream.
   //
   void toStream (QTextStream& target, bool withIndex, bool withRelativeTime)  const;

private:
   QList<QCaDataPoint> data;
};

// These types are used in inter thread signals - must be registered.
//
Q_DECLARE_METATYPE (QCaDataPoint)
Q_DECLARE_METATYPE (QCaDataPointList)

#endif  // QCADATAPOINT_H
