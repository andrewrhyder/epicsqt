/*  QEStripChartUtilities.h
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
 *
 */

#ifndef QSTRIPCHARTUTILITIES_H
#define QSTRIPCHARTUTILITIES_H

#include <QDateTime>

//==============================================================================
// Utility classes
//==============================================================================
// Tracks the minimum and maximum range of a value.
//
class TrackRange {
public:
   TrackRange ();
   void clear ();
   void merge (const double d);           // defines/extends range to include d.
   void merge (const TrackRange &that);   // defines/extends range to include that.

   // returns true if range is defined together with min and max.
   //
   bool getMinMax (double & min, double& max) const;
private:
   double minimum;
   double maximum;
   bool isDefined;
};


//==============================================================================
// Allows PV points to be scaled as:  y' = (y - d)*m + c
// This is useful whem comparing values with disparate ranages.
// It is a simple linear scaling. While d and c are not independent, from a
// user point of view is it often easier to specify a 'd' and/or a 'c' value.
//
class ValueScaling {
public:
   ValueScaling ();

   void reset ();
   void assign (const ValueScaling & s);
   void set (const double dIn, const double mIn, const double cIn);
   void get (double &dOut, double &mOut, double &cOut);

   // Find d, m and c such that the from values map to the to values,
   // e.g a PVs HOPR/LOPR values map to current chart range values.
   //
   void map (const double fromLower, const double fromUpper,
             const double toLower,   const double toUpper);

   bool isScaled ();

   inline double value (const double x) {
      return (x - d) * m + c;
   }
   TrackRange value (const TrackRange & x);

   // overloaded function

private:
   double d;   // origin
   double m;   // slope
   double c;   // offset
};


//==============================================================================
//
class TimeZone {
public:
   // Get the local time zone offset (in seconds) for the nominated UTC time.
   // Maybe default atTime = QDateTime::currentDateTime () ??
   //
   static int getZoneOffset (const QDateTime & atTime);

   static QString getZoneTLA (const Qt::TimeSpec timeSpec,
                              const QDateTime & atTime);
};

#endif  // QSTRIPCHARTUTILITIES_H
