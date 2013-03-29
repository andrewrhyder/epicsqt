/*  QEStripChartUtilities.cpp
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

#include <QECommon.h>
#include "QEStripChartUtilities.h"

//==============================================================================
//
TrackRange::TrackRange ()
{
   this->clear ();
}

//------------------------------------------------------------------------------
//
void TrackRange::clear ()
{
   this->isDefined = false;
   this->minimum = 0.0;
   this->maximum = 0.0;
}

//------------------------------------------------------------------------------
//
void TrackRange::merge (const double d)
{
   if (this->isDefined) {
      // already have at least one value
      //
      this->minimum = MIN (this->minimum, d);
      this->maximum = MAX (this->maximum, d);
   } else {
      // use single value to "start things off".
      this->minimum = d;
      this->maximum = d;
      this->isDefined = true;
   }
}

//------------------------------------------------------------------------------
//
void TrackRange::merge (const TrackRange &that)
{
   if ((this->isDefined) && (that.isDefined)) {
      // both are defined
      this->minimum = MIN (this->minimum, that.minimum);
      this->maximum = MAX (this->maximum, that.maximum);
   } else {
      // only this or that or neither are defined, but not both.
      if (that.isDefined) {
         this->isDefined = true;
         this->minimum = that.minimum;
         this->maximum = that.maximum;
      }
   }
}

//------------------------------------------------------------------------------
//
bool TrackRange::getMinMax (double &min, double &max) const
{
   min = this->minimum;
   max = this->maximum;
   return this->isDefined;
}


//==============================================================================
//
ValueScaling::ValueScaling ()
{
   d = 0.0;
   m = 1.0;
   c = 0.0;
}

//------------------------------------------------------------------------------
//
void ValueScaling::reset ()
{
   d = 0.0;
   m = 1.0;
   c = 0.0;
}

//------------------------------------------------------------------------------
//
void ValueScaling::assign (const ValueScaling & source)
{
   d = source.d;
   m = source.m;
   c = source.c;
}

//------------------------------------------------------------------------------
//
void ValueScaling::set (const double dIn, const double mIn, const double cIn)
{
   d = dIn;
   m = mIn;
   c = cIn;
}

//------------------------------------------------------------------------------
//
void ValueScaling::get (double &dOut, double &mOut, double &cOut)
{
   dOut = d;
   mOut = m;
   cOut = c;
}

//------------------------------------------------------------------------------
//
void ValueScaling::map (const double fromLower, const double fromUpper,
                        const double toLower,   const double toUpper)
{
   double delta;

   // Scaling is: y = (x - d)*m + c
   // We have three unknowns and two constraints, so have an extra
   // degree of freedom.
   //
   // Set origin to mid-point from value.
   //
   this->d = 0.5 * (fromLower + fromUpper);

   // Set Offset to mid-display value.
   //
   this->c = 0.5 * (toLower + toUpper);

   // Avoid the divide by zero.
   //
   delta = fromUpper - fromLower;
   if (delta >= 0.0) {
      delta = MAX (delta, +1.0E-9);
   } else {
      delta = MAX (delta, -1.0E-9);
   }

   // Set slope as ratio of to (display) span to form span.
   //
   this->m = (toUpper - toLower) / delta;
}

//------------------------------------------------------------------------------
//
bool ValueScaling::isScaled ()
{
   return ((d != 0.0) || (m != 1.0) || (c != 0.0));
}

//------------------------------------------------------------------------------
//
TrackRange ValueScaling::value (const TrackRange & x)
{
   TrackRange result;           // undefined
   bool okay;
   double min, max;

   okay = x.getMinMax (min, max);
   if (okay) {
      // The range is defined - the extracted min and max are good, so scale
      // each limit and merge into result.
      //
      result.merge (this->value (min));
      result.merge (this->value (max));
   }
   return result;
}

// end
