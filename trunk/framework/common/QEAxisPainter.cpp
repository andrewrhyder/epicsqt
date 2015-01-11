/*  QEAxisPainter.cpp
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
 *  Copyright (c) 2015 Australian Synchrotron
 *
 */

#include <math.h>

#include <QDebug>
#include <QFontMetrics>
#include <QBrush>
#include <QPen>
#include <QPoint>
#include <QPainter>

#include <alarm.h>
#include <QECommon.h>
#include <QCaAlarmInfo.h>
#include <QEWidget.h>
#include <QEAxisPainter.h>

#define DEBUG qDebug () << "QEAxisPainter" << __LINE__ << __FUNCTION__

#define MIN_VALUE         (-1.0E+24)
#define MAX_VALUE         (+1.0E+24)
#define MIN_INTERVAL      (+1.0E-20)
#define MAX_INTERVAL      (+1.0E+23)
#define MAX_MINOR_TICKS   1000

//------------------------------------------------------------------------------
//
QEAxisPainter::QEAxisPainter (QWidget* parent) : QWidget (parent)
{
   this->setMinimumHeight (20);

   this->bandList.clear ();
   this->mMinimum = 0.0;
   this->mMaximum = 10.0;
   this->mMinorInterval = 0.2;
   this->mMajorMinorRatio = 5;   // => majorInterval = 1.0
   this->mIsLogScale = false;
   this->mIndent = 20;
   this->mGap = 2;
   this->mOrientation = Left_To_Right;
   this->mTextPosition = BelowLeft;

   this->mColour = QColor (0, 0, 0, 255);  // black
   // this->mLogScaleInterval = 1;
}

//------------------------------------------------------------------------------
//
void QEAxisPainter::setMinimum (const double minimum)
{
   // Ensure in range
   //
   this->mMinimum = LIMIT (minimum, MIN_VALUE, MAX_VALUE);

   // Ensure consistant.
   //
   this->mMaximum = MAX (this->mMaximum, this->mMinimum + MIN_INTERVAL);

   double n = (this->mMaximum - this->mMinimum) / this->getMinorInterval ();
   if (n > MAX_MINOR_TICKS) {
      this->setMinorInterval (this->mMinorInterval * n / MAX_MINOR_TICKS);
   }

   this->update ();
}

//------------------------------------------------------------------------------
//
double QEAxisPainter::getMinimum () const
{
   return this->mMinimum;
}

//------------------------------------------------------------------------------
//
void QEAxisPainter::setMaximum (const double maximum)
{
   // Ensure in range
   //
   this->mMaximum = LIMIT (maximum, MIN_VALUE, MAX_VALUE);

   // Ensure consistant.
   //
   this->mMinimum = MIN (this->mMinimum, this->mMaximum - MIN_INTERVAL);

   double n = (this->mMaximum - this->mMinimum) / this->getMinorInterval ();
   if (n > MAX_MINOR_TICKS) {
      this->setMinorInterval (this->mMinorInterval * n / MAX_MINOR_TICKS);
   }

   this->update ();
}

//------------------------------------------------------------------------------
//
double QEAxisPainter::getMaximum  () const
{
   return this->mMaximum;
}

//------------------------------------------------------------------------------
//
void QEAxisPainter::setMinorInterval (const double minorInterval)
{
   // Ensure in range
   //
   this->mMinorInterval = LIMIT (minorInterval, MIN_INTERVAL, MAX_INTERVAL);

   double dynamicMin = (this->mMaximum - this->mMinimum) / MAX_MINOR_TICKS;
   this->mMinorInterval = MAX (this->mMinorInterval, dynamicMin);
   this->update ();
}

//------------------------------------------------------------------------------
//
double QEAxisPainter::getMinorInterval  () const
{
    return this->mMinorInterval;
}

//------------------------------------------------------------------------------
//
void QEAxisPainter::setMajorMinorRatio (const int majorMinorRatio)
{
   // Ensure in range
   //
   this->mMajorMinorRatio = MAX (2, majorMinorRatio);
   this->update ();
}

//------------------------------------------------------------------------------
//
int QEAxisPainter::getMajorMinorRatio () const
{
    return this->mMajorMinorRatio;
}

//------------------------------------------------------------------------------
//
void QEAxisPainter::setLogScale (const bool value)
{
   this->mIsLogScale = value;
}

//------------------------------------------------------------------------------
//
bool QEAxisPainter::getLogScale () const
{
   return this->mIsLogScale;
}

//------------------------------------------------------------------------------
//
void QEAxisPainter::setOrientation (const Orientations orientation)
{
   this->mOrientation = orientation;
   this->update ();
}

//------------------------------------------------------------------------------
//
QEAxisPainter::Orientations QEAxisPainter::getOrientation () const
{
   return this->mOrientation;
}

//------------------------------------------------------------------------------
//
void QEAxisPainter::setTextPosition (const TextPositions textPositions)
{
   this->mTextPosition = textPositions;
   this->update ();
}

//------------------------------------------------------------------------------
//
QEAxisPainter::TextPositions QEAxisPainter::getTextPosition () const
{
   return this->mTextPosition;
}

//------------------------------------------------------------------------------
//
void QEAxisPainter::setColour (const QColor colour)
{
   this->mColour = colour;
   this->update ();
}

//------------------------------------------------------------------------------
//
QColor QEAxisPainter::getColour () const
{
   return this->mColour;
}

//------------------------------------------------------------------------------
//
void QEAxisPainter::setIndent (const int indent)
{
   this->mIndent = MAX (indent, 0);
   this->update ();
}

//------------------------------------------------------------------------------
//
int QEAxisPainter::getIndent () const
{
   return this->mIndent;
}

//------------------------------------------------------------------------------
//
void QEAxisPainter::setGap (const int gap)
{
   this->mGap = MAX (gap, 0);
   this->update ();
}

//------------------------------------------------------------------------------
//
int QEAxisPainter::getGap  () const
{
   return this->mGap;
}

//------------------------------------------------------------------------------
//
QEAxisPainter::ColourBand QEAxisPainter::createColourBand (const double lower, const double upper,  const QColor& colour)
{
   QEAxisPainter::ColourBand result;
   result.lower = lower;
   result.upper = upper;
   result.colour = colour;
   return result;
}

//------------------------------------------------------------------------------
//
QEAxisPainter::ColourBand QEAxisPainter::createColourBand (const double lower, const double upper,  const unsigned short severity)
{
   QCaAlarmInfo alarmInfo (0, severity);
   int saturation;
   QColor colour;

   saturation = (severity == NO_ALARM) ? 32 : 128;
   colour = QEWidget::getColor (alarmInfo, saturation);
   return QEAxisPainter::createColourBand (lower, upper, colour);
}

//------------------------------------------------------------------------------
//
QEAxisPainter::ColourBandLists QEAxisPainter::calcAlarmColourBandList (qcaobject::QCaObject* qca)
{
   ColourBandLists result;

   result.clear ();
   if (!qca) return result;  // sanity check

   const double dispLower = this->getMinimum ();
   const double dispUpper = this->getMaximum ();
   const double alarmLower = qca->getAlarmLimitLower ();
   const double alarmUpper = qca->getAlarmLimitUpper ();
   const double warnLower = qca->getWarningLimitLower ();
   const double warnUpper = qca->getWarningLimitUpper ();
   bool alarmIsDefined;
   bool warnIsDefined;

   // Unfortunately, the Channel Access protocol only provides the alarm/warning
   // values, and not the associated severities. We assume major for alarms, and
   // minor for warnings. Also when not defined, alarm levels are NaN.
   //
   // Of course a QEAxisPainter user is noy obliged to use this function.
   //
   alarmIsDefined = ( !isnan (alarmLower) && !isnan (alarmUpper) &&
                      (alarmLower != alarmUpper) );

   warnIsDefined = ( !isnan (warnLower) && !isnan (warnUpper) &&
                     (warnLower != warnUpper) );

   if (alarmIsDefined) {
      if (warnIsDefined) {
         // All alarms defined.
         //
         result << QEAxisPainter::createColourBand (dispLower,  alarmLower, MAJOR_ALARM);
         result << QEAxisPainter::createColourBand (alarmLower, warnLower,  MINOR_ALARM);
         result << QEAxisPainter::createColourBand (warnLower,  warnUpper,  NO_ALARM);
         result << QEAxisPainter::createColourBand (warnUpper,  alarmUpper, MINOR_ALARM);
         result << QEAxisPainter::createColourBand (alarmUpper, dispUpper,  MAJOR_ALARM);
      } else {
         // Major alarms defined.
         //
         result << QEAxisPainter::createColourBand (dispLower,  alarmLower, MAJOR_ALARM);
         result << QEAxisPainter::createColourBand (alarmLower, alarmUpper, NO_ALARM);
         result << QEAxisPainter::createColourBand (alarmUpper, dispUpper,  MAJOR_ALARM);
      }
   } else {
      if (warnIsDefined) {
         // Minor alarms defined.
         //
         result << QEAxisPainter::createColourBand (dispLower, warnLower, MINOR_ALARM);
         result << QEAxisPainter::createColourBand (warnLower, warnUpper, NO_ALARM);
         result << QEAxisPainter::createColourBand (warnUpper, dispUpper, MINOR_ALARM);
      } else {
         // No alarms defined at all.
         //
         result << QEAxisPainter::createColourBand (dispLower, dispUpper, NO_ALARM);
      }
   }

   return result;
}

//------------------------------------------------------------------------------
//
void QEAxisPainter::setColourBandList (const ColourBandLists& bandListIn)
{
   this->bandList = bandListIn;
}

//------------------------------------------------------------------------------
//
QEAxisPainter::ColourBandLists QEAxisPainter::getColourBandList () const
{
   return this->bandList;
}

//------------------------------------------------------------------------------
//
bool QEAxisPainter::isLeftRight () const
{
   return (this->mOrientation == QEAxisPainter::Left_To_Right) ||
          (this->mOrientation == QEAxisPainter::Right_To_Left);
}

//------------------------------------------------------------------------------
//
void QEAxisPainter::paintEvent (QPaintEvent *)
{
   // Tick sizes on axis
   //
   const int minorTick = 5;
   const int majorTick = 10;
   const int pointSize = 7;

   QPainter painter (this);
   QPen pen;
   QBrush brush;
   QColor penColour;
   int width;
   int height;

   int sign;
   int x_first, x_last;
   int y_first, y_last;
   int j;
   bool ok;
   bool isMajor;
   double value;

   // Draw everything with antialiasing off.
   //
   painter.setRenderHint (QPainter::Antialiasing, false);

   // Alias/edge adjustment.
   // Note: Actual size appears to be 1 less than widget width/height.
   //
   width  = this->geometry ().width () - 1;
   height = this->geometry ().height () - 1;

   switch (this->mOrientation) {

      case Left_To_Right:
         sign = (this->mTextPosition == BelowLeft) ? +1 : -1;
         x_first = this->mIndent;
         x_last  = width - this->mIndent;
         y_first = (this->mTextPosition == BelowLeft) ? this->mGap : height - this->mGap;
         y_last  = y_first;
         break;

      case Right_To_Left:
         sign = (this->mTextPosition == BelowLeft) ? +1 : -1;
         x_first = width - this->mIndent;
         x_last  = this->mIndent;
         y_first = (this->mTextPosition == BelowLeft) ? this->mGap : height - this->mGap;
         y_last  = y_first;
         break;

      case Top_To_Bottom:
         sign = (this->mTextPosition == BelowLeft) ? -1 : +1;
         x_first = (this->mTextPosition == BelowLeft) ? width - this->mGap : this->mGap;
         x_last  = x_first;
         y_first = this->mIndent;
         y_last  = height - this->mIndent;
         break;

      case Bottom_To_Top:
         sign = (this->mTextPosition == BelowLeft) ? -1 : +1;
         x_first = (this->mTextPosition == BelowLeft) ? width - this->mGap : this->mGap;
         x_last  = x_first;
         y_first = height - this->mIndent;
         y_last  = this->mIndent;
         break;

      default:
         // report an error??
         //
         return;
   }

   for (int j = 0; j < this->bandList.count (); j++) {
      ColourBand band = this->bandList.at (j);
      double fl, gl;
      double fu, gu;
      int x1, x2;
      int y1, y2;
      QRect bandRect;

      pen.setWidth (0);
      pen.setColor (band.colour);
      painter.setPen (pen);

      brush.setColor (band.colour);
      brush.setStyle (Qt::SolidPattern);
      painter.setBrush (brush);

      fl = this->calcFraction (band.lower);
      fu =  this->calcFraction (band.upper);
      gl = 1.0 - fl;
      gu = 1.0 - fu;

      x1 = int (gl * double (x_first) +  fl * double (x_last));
      x2 = int (gu * double (x_first) +  fu * double (x_last));

      y1 = y_first;
      y2 = y1 + majorTick + 1 + pointSize + 1;

      bandRect.setTop (y1);
      bandRect.setBottom (y2);
      bandRect.setLeft (x1);
      bandRect.setRight (x2);
      painter.drawRect (bandRect);
   }

   pen.setWidth (1);
   penColour = this->mColour;
   if (!this->isEnabled()) {
      penColour = QEUtilities::blandColour (penColour);
   }
   pen.setColor (penColour);
   painter.setPen (pen);

   // Determine format.
   //
   double mi = this->mMinorInterval * this->mMajorMinorRatio;
   const char* format;

   if (this->getLogScale ()) {
      format = "%.0e";
   } else {
      if (mi >= 10.0) {
         format = "%.0f";
      } else if (mi >= 1.0) {
         format = "%.1f";
      } else if (mi >= 0.1) {
         format = "%.2f";
      } else if (mi >= 0.01) {
         format = "%.3f";
      } else { // < 0.01
         format = "%.0e";
      }
   }

   for (ok = this->firstValue (j, value, isMajor); ok;
        ok = this->nextValue  (j, value, isMajor)) {

      double f, g;
      int x, y;
      QPoint p1, p2;

      f = this->calcFraction (value);
      g = 1.0 - f;    // co-fraction

      x = int (g * double (x_first) +  f * double (x_last));
      y = int (g * double (y_first) +  f * double (y_last));

      p1 = QPoint (x, y);
      if (isMajor) {
         p2 = this->isLeftRight () ? QPoint (x, y + sign*majorTick) : QPoint (x + sign*majorTick, y);
      }  else {
         p2 = this->isLeftRight () ? QPoint (x, y + sign*minorTick) : QPoint (x + sign*minorTick, y);
      }

      painter.drawLine (p1, p2);

      if (isMajor) {
         QString vt;

         if (this->getLogScale () ) {
            vt.sprintf ("%.0e", value);
         } else {
            vt.sprintf ("%.1f", value);
         }

         p2 = this->isLeftRight () ? QPoint (x, y + sign*(majorTick + 1)) :
                                     QPoint (x + sign*(majorTick + 1), y);
         this->drawAxisText (painter, p2, vt, pointSize);
      }
   }
}

//------------------------------------------------------------------------------
//
double QEAxisPainter::calcFraction (const double x)
{
   double result;

   // Calculate the fractional scale and constrain to be in range.
   //
   if (this->getLogScale ()) {
      result = (LOG10 (x)              - LOG10 (this->mMinimum)) /
               (LOG10 (this->mMaximum) - LOG10 (this->mMinimum));
   } else {
      result = (x              - this->mMinimum) /
               (this->mMaximum - this->mMinimum);
   }
   result = LIMIT (result, 0.0, 1.0);

   return result;
}

//------------------------------------------------------------------------------
// Depending on orientation/edge, draws the text releatibe to mominated position.
//
void QEAxisPainter::drawAxisText (QPainter& painter, const QPoint& position,
                                  const QString& text, const int pointSize)
{
   QFont pf (this->font ());

   if (pointSize > 0) {
      pf.setPointSize (pointSize);
   }
   painter.setFont (pf);

   QFontMetrics fm = painter.fontMetrics ();
   const int textWidth = fm.width (text);
   const int textHeight = pf.pointSize ();  // For height, pointSize seems better than fm.height ()

   int x;
   int y;

   // Find text origin, painter.drawText needs bottom left coordinates.
   //
   if (this->isLeftRight ()) {
      x = position.x () - textWidth / 2;
      y = position.y ();
      if (this->mTextPosition == BelowLeft) {
         y += textHeight;
      }

   } else {
      x = position.x ();
      if (this->mTextPosition == BelowLeft) {
         x -= textWidth;
      }
      y = position.y () + (textHeight + 1) / 2;
   }

   // Font colour same as basix axis colour.
   // If text too wide, then ensure we show most significant part.
   //
   painter.drawText (MAX (1, x), y, text);
}

//------------------------------------------------------------------------------
//
bool QEAxisPainter::firstValue (int& itc, double& value, bool& isMajor)
{
   double real;
   bool result;

   if (this->getLogScale ()) {
      real = 9.0 * LOG10 (this->mMinimum);
   } else {
      real = this->mMinimum / this->mMinorInterval;
   }

   // Use floor to round down and - 0.5 to mitigate any rounding effects.
   // Subtract an addition -1 to ensure first call to nextValue returns a
   // value no greater than the first required value.
   //
   itc = int (floor (real) - 0.5) - 1;

   result = this->nextValue (itc, value, isMajor);
   while (result && (value < this->mMinimum)) {
      result = this->nextValue (itc, value, isMajor);
   }
   return result;
}

//------------------------------------------------------------------------------
//
bool QEAxisPainter::nextValue  (int& itc, double& value, bool& isMajor)
{
   const int fs = 9;

   int d;
   int f;

   itc++;
   if (this->getLogScale ()) {
      // Ensure round down towards -infinity (as opposed to 0)
      // (Oh how I wish C/C++ has a proper "mod" operator).
      //
      d = itc / fs;
      if ((fs * d) > itc) d--;
      f = itc -(fs * d);
      value = (1.0 + f) * pow (10.0, d);
      if (f == 0) {
         // Is an exact power of 10 - test for being major.
         //
         isMajor = ((d % 2) == 0);  // this->getLogScaleInterval ()) == 0);
      } else {
         // Is not an exact power of 10 - canot be major.
         //
         isMajor = false;
      }
   } else {
      value = itc * this->mMinorInterval;
      isMajor = ((itc % this->mMajorMinorRatio) == 0);
   }
   return (value <= this->mMaximum);
}


// end
