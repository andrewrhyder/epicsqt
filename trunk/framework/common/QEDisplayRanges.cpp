/*  QEDisplayRanges.cpp
 *
 *  This file is part of the EPICS QT Framework, initially developed at the
 *  Australian Synchrotron.
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
 *  Copyright (c) 2014 Australian Synchrotron.
 *
 *  Author:
 *    Andrew Starritt
 *  Contact details:
 *    andrew.starritt@synchrotron.org.au
 */

#include <math.h>
#include <QECommon.h>
#include "QEDisplayRanges.h"

//==============================================================================
//
QEDisplayRanges::QEDisplayRanges ()
{
   this->clear ();
}

//------------------------------------------------------------------------------
//
QEDisplayRanges::QEDisplayRanges (const double minIn, const double maxIn)
{
   this->setRange (minIn, maxIn);
}


//------------------------------------------------------------------------------
//
bool QEDisplayRanges::operator == (const QEDisplayRanges& other) const
{
   bool result;

   if (this->isDefined == other.isDefined) {
      // Both defined or both not defined.
      //
      if (this->isDefined) {
         // Both defined.
         result = (this->minimum == other.minimum) && (this->maximum == other.maximum);
      } else {
         // Both undefined.
         result = true;
      }
   } else {
      result = false;
   }

   return result;
}

//------------------------------------------------------------------------------
// Define != as not == , this ensures consistancy as is only sensible definition.
//
bool QEDisplayRanges::operator != (const QEDisplayRanges& other) const
{
   return !(*this == other);
}

//------------------------------------------------------------------------------
//
void QEDisplayRanges::clear ()
{
   this->isDefined = false;
   this->minimum = 0.0;
   this->maximum = 0.0;
}

//------------------------------------------------------------------------------
//
void QEDisplayRanges::setRange (const double minIn, const double maxIn)
{
   this->minimum = minIn;
   this->maximum = maxIn;
   this->isDefined = true;
}

//------------------------------------------------------------------------------
//
void QEDisplayRanges::merge (const double d)
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
void QEDisplayRanges::merge (const QEDisplayRanges &other)
{
   if ((this->isDefined) && (other.isDefined)) {
      // both are defined
      this->minimum = MIN (this->minimum, other.minimum);
      this->maximum = MAX (this->maximum, other.maximum);
   } else {
      // only this or other or neither are defined, but not both.
      if (other.isDefined) {
         this->isDefined = true;
         this->minimum = other.minimum;
         this->maximum = other.maximum;
      }
   }
}

//------------------------------------------------------------------------------
//
bool QEDisplayRanges::getIsDefined () const
{
   return this->isDefined;
}

//------------------------------------------------------------------------------
//
double QEDisplayRanges::getMinimum () const
{
   return this->minimum;
}

//------------------------------------------------------------------------------
//
double QEDisplayRanges::getMaximum () const
{
   return this->maximum;
}

//------------------------------------------------------------------------------
//
bool QEDisplayRanges::getMinMax (double &min, double &max) const
{
   min = this->minimum;
   max = this->maximum;
   return this->isDefined;
}

//------------------------------------------------------------------------------
// static
//
void QEDisplayRanges::adjustLogMinMax (const QEDisplayRanges& useRange,
                                       double& minOut, double& maxOut, double& majorOut)
{
   minOut = floor (LOG10 (useRange.minimum));
   maxOut = ceil  (LOG10 (useRange.maximum));
   if ((maxOut - minOut) >= 16) {
      majorOut = 2.0;
   } else {
      majorOut = 1.0;
   }
}

//------------------------------------------------------------------------------
// static
//
void QEDisplayRanges::adjustMinMax (const QEDisplayRanges& useRange, const int number,
                                    double& minOut, double& maxOut, double& majorOut)
{
   // The compiler does a better job of evaluating these constants and
   // minimising rounding errors than if self generated by the application.
   //
   // The range is somewhat arbitary, and may be extended.
   //
   static const double majorValues [] = {
      1.0e-12,  2.0e-12,  5.0e-12,    1.0e-11,  2.0e-11,  5.0e-11,
      1.0e-10,  2.0e-10,  5.0e-10,    1.0e-9,   2.0e-9,   5.0e-9,
      1.0e-8,   2.0e-8,   5.0e-8,     1.0e-7,   2.0e-7,   5.0e-7,
      1.0e-6,   2.0e-6,   5.0e-6,     1.0e-5,   2.0e-5,   5.0e-5,
      1.0e-4,   2.0e-4,   5.0e-4,     1.0e-3,   2.0e-3,   5.0e-3,
      1.0e-2,   2.0e-2,   5.0e-2,     1.0e-1,   2.0e-1,   5.0e-1,
      1.0e+0,   2.0e+0,   5.0e+0,     1.0e+1,   2.0e+1,   5.0e+1,
      1.0e+2,   2.0e+2,   5.0e+2,     1.0e+3,   2.0e+3,   5.0e+3,
      1.0e+4,   2.0e+4,   5.0e+4,     1.0e+5,   2.0e+5,   5.0e+5,
      1.0e+6,   2.0e+6,   5.0e+6,     1.0e+7,   2.0e+7,   5.0e+7,
      1.0e+8,   2.0e+8,   5.0e+8,     1.0e+9,   2.0e+9,   5.0e+9,
      1.0e+10,  2.0e+10,  5.0e+10,    1.0e+11,  2.0e+11,  5.0e+11,
      1.0e+12,  2.0e+12,  5.0e+12,    1.0e+13,  2.0e+13,  5.0e+13,
      1.0e+14,  2.0e+14,  5.0e+14,    1.0e+15,  2.0e+15,  5.0e+15,
      1.0e+16,  2.0e+16,  5.0e+16,    1.0e+17,  2.0e+17,  5.0e+17,
      1.0e+18,  2.0e+18,  5.0e+18,    1.0e+19,  2.0e+19,  5.0e+19,
      1.0e+20,  2.0e+20,  5.0e+20,    1.0e+21,  2.0e+21,  5.0e+21,
      1.0e+22,  2.0e+22,  5.0e+22,    1.0e+23,  2.0e+23,  5.0e+23,
      1.0e+24,  2.0e+24,  5.0e+24,    1.0e+25,  2.0e+25,  5.0e+25
   };

   double major;
   double minor;
   int s;
   int p, q;

   // Find estimated major value - use size (width or height) to help here.
   //
   major = (useRange.maximum - useRange.minimum) / MAX (number, 2);

   // Round up major to next standard value.
   //
   s = major <= 1.0 ? 0 : 36;  // short cut
   while ((major > majorValues [s]) &&
          ((s + 1) < ARRAY_LENGTH (majorValues))) s++;

   majorOut = major = majorValues [s];

   if ((s%3) == 1) {
      // Is a 2.0eN number.
      minor = major / 4.0;
   } else {
      // Is a 1.0eN or 5.0eN number.
      minor = major / 5.0;
   }

   // Determine minOut and maxOut such that they are both exact multiples of
   // minor and that:
   //
   //  minOut <= minIn <= maxIn << maxOut
   //
   p = (int) (useRange.minimum / minor);
   if ((p * minor) > useRange.minimum) p--;

   q = (int) (useRange.maximum / minor);
   if ((q * minor) < useRange.maximum) q++;

   q = MAX (q, p+1);   // Ensure p < q

   // Extend lower/upper limit to include 0 if min < 5% max
   //
   if ((p > 0) && (q > 20*p)) {
      p = 0;
   } else if ((q < 0) && (p < 20*q)) {
      q = 0;
   }

   // Subtract/add tolerance as Qwt Axis ploting of minor ticks a bit slack.
   //
   minOut = ((double)p - 0.05) * minor;
   maxOut = ((double)q + 0.05) * minor;
}

// end

